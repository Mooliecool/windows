// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==
//*****************************************************************************
// File: SecurityWrapper.cpp
//
// Wrapper around Win32 Security functions
//
//*****************************************************************************

#include "stdafx.h"
#include "securitywrapper.h"
#include "ex.h"
#include "holder.h"


// For GetSidFromProcess*
#include <tlhelp32.h>
#include "wtsapi32.h"


//-----------------------------------------------------------------------------
// Constructor for Sid wrapper class.
// pSid - OS sid to wrap
//-----------------------------------------------------------------------------
Sid::Sid(PSID pSid)
{
    _ASSERTE(pSid != NULL);
    m_pSid = pSid;
}

//-----------------------------------------------------------------------------
// Aesthetic wrapper for Sid equality
//-----------------------------------------------------------------------------
bool Sid::Equals(PSID a, PSID b)
{ 
    return EqualSid(a, b) != 0; 
}

//-----------------------------------------------------------------------------
// Ctor for SidBuffer class
//-----------------------------------------------------------------------------
SidBuffer::SidBuffer()
{
    m_pBuffer = NULL;
}

//-----------------------------------------------------------------------------
// Dtor for SidBuffer class.
//-----------------------------------------------------------------------------
SidBuffer::~SidBuffer()
{
    delete [] m_pBuffer;
}

//-----------------------------------------------------------------------------
// Get the underlying sid
// Caller assumes SidBuffer has been initialized.
//-----------------------------------------------------------------------------
Sid SidBuffer::GetSid()
{
    _ASSERTE(m_pBuffer != NULL);
    Sid s((PSID) m_pBuffer);
    return s;
}

//-----------------------------------------------------------------------------
// Internal helper.
// Gets the SID for the given process.
// Returns:
//   S_OK on success, *ppSid is set to the given sid (must be deleted by caller).
//   else failure, *ppSid == NULL.
//-----------------------------------------------------------------------------
HRESULT GetSidFromProcessWorker(DWORD dwProcessId, PSID *ppSid)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    HRESULT     hr = S_OK;
    TOKEN_USER  *pTokUser = NULL;
    HANDLE      hProc = INVALID_HANDLE_VALUE;
    HANDLE      hToken = INVALID_HANDLE_VALUE;
    DWORD       dwRetLength;
    TOKEN_OWNER *ptkOwner = NULL;
    DWORD       cbSid;
    PSID        pSid = NULL;

    LOG((LF_CORDB, LL_INFO10000,
         "SecurityUtil::GetSidFromProcess: 0x%08x\n",
         dwProcessId));

    _ASSERTE(ppSid);
    *ppSid = NULL;

    hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);

    if (hProc == NULL)
    {
        hr = HRESULT_FROM_GetLastError();
        goto exit;
    }
    if (!OpenProcessToken(hProc, TOKEN_QUERY, &hToken))
    {
        hr = HRESULT_FROM_GetLastError();
        goto exit;
    }

    // figure out the length
    GetTokenInformation(hToken, TokenOwner, NULL, 0, &dwRetLength);
    _ASSERTE(dwRetLength);

    ptkOwner = (TOKEN_OWNER *) new (nothrow) char[dwRetLength];
    if (ptkOwner == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    if (!GetTokenInformation(hToken, TokenOwner, (LPVOID) ptkOwner, dwRetLength, &dwRetLength))
    {
        hr = HRESULT_FROM_GetLastError();
        goto exit;
    }

    // Copy over the SID
    cbSid = GetLengthSid(ptkOwner->Owner);
    pSid = new (nothrow) char[cbSid];
    if (pSid == NULL)
    {
        hr = E_OUTOFMEMORY;
    }
    else
    {
        if (!CopySid(cbSid, pSid, ptkOwner->Owner))
        {
            hr = HRESULT_FROM_GetLastError();
            goto exit;
        }
    }


    *ppSid = pSid;
    pSid = NULL;
    
exit:
    if (hToken != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hToken);
    }
    if (hProc != INVALID_HANDLE_VALUE)
    {
        // clean up
        CloseHandle(hProc);
    }
    if (ptkOwner)
    {
        delete [] (reinterpret_cast<char*>(ptkOwner));
    }

    if (pSid)
    {
        delete [] (reinterpret_cast<char*>(pSid));
    }

    LOG((LF_CORDB, LL_INFO10000,
         "SecurityUtil::GetSidFromProcess return hr : 0x%08x\n",
         hr));

    return hr;
}

//
HRESULT GetSidFromProcessEXWorker(DWORD dwProcessId, PSID *ppSid)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    HRESULT            hr = S_OK;
    PWTS_PROCESS_INFOW rgProcessInfo = NULL;
    DWORD              dwNumProcesses;
    DWORD              iProc;
    DWORD              cbSid;
    PSID               pSid = NULL;

    LOG((LF_CORDB, LL_INFO10000,
         "SecurityUtil::GetSidFromProcessEx: 0x%08x\n",
         dwProcessId));


    //
    // Since WTSapi32.dll is only available on WindowsNT, we can't make static
    // calls into WTS* functions; we have to do the LoadLibrary/GetProcAddress
    // dance.
    //
    // Note we must only call this function on NT.
    //
    //
    // more note: WTSxxx apis are only in wtsapi32. If we don't find this library, don't
    // bother to try. Also this code path is exercised when GetSidFromProcess failed only.
    //
    if (RunningOnWin95())
    {
        // don't bother to try on Win9X
        return E_FAIL;
    }

    HMODULE hWtsapi32 = WszLoadLibrary(L"wtsapi32");

    // Since we're on NT, wtsapi32.dll better be available!
    _ASSERTE(NULL != hWtsapi32);
    if (NULL == hWtsapi32)
    {
        LOG((LF_CORDB, LL_INFO1000,
             "Unable to get security attributes; LoadLibrary(wtsapi32) failed\n"));
        hr = E_FAIL;
        goto exit;
    }

    typedef BOOL (*WTSEnumerateProcessFunc) (HANDLE, DWORD, DWORD,
                                             PWTS_PROCESS_INFO*, DWORD*);
    WTSEnumerateProcessFunc pWTSEnumerateProcessesFunc =
        reinterpret_cast< WTSEnumerateProcessFunc >(GetProcAddress(hWtsapi32,

                                                                   "WTSEnumerateProcessesW"));

    if (pWTSEnumerateProcessesFunc == NULL)
    {
        // still cannot find WTSEnumerateProcessesW
        hr = E_FAIL;
        goto exit;
    }

    _ASSERTE(NULL != pWTSEnumerateProcessesFunc);

    typedef void (*WTSFreeMemoryFunc) (PVOID);
    WTSFreeMemoryFunc pWTSFreeMemoryFunc =
        reinterpret_cast< WTSFreeMemoryFunc >(GetProcAddress(hWtsapi32,
                                                             "WTSFreeMemory"));
    if (NULL == pWTSFreeMemoryFunc)
    {
        hr = E_FAIL;
        goto exit;
    }

    _ASSERTE(ppSid);
    *ppSid = NULL;
    if (!pWTSEnumerateProcessesFunc(WTS_CURRENT_SERVER_HANDLE,   // use local server
                                    0,              // Reserved must be zero
                                    1,              // version must be 1
                                    &rgProcessInfo, // Receives pointer to process list
                                    &dwNumProcesses))
    {
        hr = HRESULT_FROM_GetLastError();
        goto exit;
    }

    for (iProc = 0; iProc < dwNumProcesses; iProc++)
    {

        if (rgProcessInfo[iProc].ProcessId == dwProcessId)
        {
            if (rgProcessInfo[iProc].pUserSid == NULL)
            {
                LOG((LF_CORDB, LL_INFO10000,
                     "SecurityUtil::GetSidFromProcessEx is not able to retreive SID\n"));

                // if there is no Sid for the user, don't call GetLengthSid.
                // It will crash! It is ok to return E_FAIL as caller will ignore it.
                hr = E_FAIL;
                goto exit;
            }
            cbSid = GetLengthSid(rgProcessInfo[iProc].pUserSid);
            pSid = new (nothrow) char[cbSid];
            if (pSid == NULL)
            {
                hr = E_OUTOFMEMORY;
            }
            else
            {
                if (!CopySid(cbSid, pSid, rgProcessInfo[iProc].pUserSid))
                {
                    hr = HRESULT_FROM_GetLastError();
                }
                else
                {
                    // We are done. Go to exit
                    hr = S_OK;
                }
            }

            // we already find a match. Even if we fail from memory allocation of CopySid, still
            // goto exit.
            goto exit;
        }
    }

    // Walk the whole list and cannot find the matching PID
    // Find a better error code.
    hr = E_FAIL;

exit:

    if (rgProcessInfo)
    {
        pWTSFreeMemoryFunc(rgProcessInfo);
    }

    if (NULL != hWtsapi32)
    {
        FreeLibrary(hWtsapi32);
    }

    if (FAILED(hr) && pSid)
    {
        delete [] (reinterpret_cast<char*>(pSid));
    }

    if (SUCCEEDED(hr))
    {
        _ASSERTE(pSid);
        *ppSid = pSid;
    }
    LOG((LF_CORDB, LL_INFO10000,
         "SecurityUtil::GetSidFromProcessEx return hr : 0x%08x\n",
         hr));


    return hr;
}

//-----------------------------------------------------------------------------
// Initialize this SidBuffer instance with a Sid for the owner of the 
// specified process.
//-----------------------------------------------------------------------------
HRESULT SidBuffer::InitFromProcessNoThrow(DWORD pid)
{
    // Windows 95 is not secure, so all the security checks will fail.
    _ASSERTE(!RunningOnWin95() || !"Don't bother doing security checks on Win9x");

    _ASSERTE(m_pBuffer == NULL);
    HRESULT hr = GetSidFromProcessWorker(pid, (PSID*) &m_pBuffer);
    if (FAILED(hr))
    {
        hr = GetSidFromProcessEXWorker(pid, (PSID*) &m_pBuffer);
    }
    if (FAILED(hr))
    {
        return hr;
    }    
    
    _ASSERTE(m_pBuffer != NULL);
    return S_OK;
}
void SidBuffer::InitFromProcess(DWORD pid) // throws
{
    HRESULT hr = InitFromProcessNoThrow( pid );
    if (FAILED(hr))
    {
        ThrowHR(hr);
    }    
}


//-----------------------------------------------------------------------------
// Ctor for Dacl class. Wraps a win32 dacl.
//-----------------------------------------------------------------------------
Dacl::Dacl(PACL pAcl)
{
    m_acl = pAcl;   
}

//-----------------------------------------------------------------------------
// Get number of ACE (Access Control Entries) in this DACL.
//-----------------------------------------------------------------------------
SIZE_T Dacl::GetAceCount()
{
    return (SIZE_T) m_acl->AceCount;
}

//-----------------------------------------------------------------------------
// Get Raw a ACE at the given index.
// Caller assumes index is valid (0 <= dwAceIndex < GetAceCount())
// Throws on error (which should only be if the index is out of bounds).
//-----------------------------------------------------------------------------
ACE_HEADER * Dacl::GetAce(SIZE_T dwAceIndex)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;
    
    ACE_HEADER * pAce = NULL;
    BOOL fOk = ::GetAce(m_acl, (DWORD) dwAceIndex, (LPVOID*) &pAce);
    _ASSERTE(fOk == (pAce != NULL));
    if (!fOk)
    {
        ThrowLastError();
    }
    return pAce;
}



//-----------------------------------------------------------------------------
// Ctor for SecurityDescriptor
//-----------------------------------------------------------------------------
Win32SecurityDescriptor::Win32SecurityDescriptor()
{
    m_pDesc = NULL;
}

//-----------------------------------------------------------------------------
// Dtor for security Descriptor.
//-----------------------------------------------------------------------------
Win32SecurityDescriptor::~Win32SecurityDescriptor()
{
    delete [] ((BYTE*) m_pDesc);
}



//-----------------------------------------------------------------------------
// Get the dacl for this security descriptor.
//-----------------------------------------------------------------------------
Dacl Win32SecurityDescriptor::GetDacl()
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    _ASSERTE(m_pDesc != NULL);

    BOOL bPresent;
    BOOL bDaclDefaulted;
    PACL acl;
    
    if (GetSecurityDescriptorDacl(m_pDesc, &bPresent, &acl, &bDaclDefaulted) == 0)
    {
        ThrowLastError();
    }
    if (!bPresent)
    {
        ThrowHR(E_INVALIDARG);
    }

    Dacl d(acl);
    return d;
}

//-----------------------------------------------------------------------------
// Get the owner from the security descriptor.
//-----------------------------------------------------------------------------
HRESULT Win32SecurityDescriptor::GetOwnerNoThrow( PSID* ppSid)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    _ASSERTE(m_pDesc != NULL);
    BOOL bOwnerDefaulted;

    if( ppSid == NULL )
    {
        return E_INVALIDARG;
    }

    if (GetSecurityDescriptorOwner(m_pDesc, ppSid, &bOwnerDefaulted) == 0)
    {
        DWORD err = GetLastError();
        return HRESULT_FROM_WIN32(err);
    }

    return S_OK;
}
Sid Win32SecurityDescriptor::GetOwner()
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    PSID pSid;
    HRESULT hr = GetOwnerNoThrow( &pSid );
    if( FAILED(hr) )
    {
        ThrowHR( hr );
    }

    Sid s(pSid);
    return s;
}

//-----------------------------------------------------------------------------
// Initialize this instance of a SecurityDescriptor with the SD for the handle.
// The handle must ahve READ_CONTROL permissions to do this.
// Throws on error.
//-----------------------------------------------------------------------------
HRESULT Win32SecurityDescriptor::InitFromHandleNoThrow(HANDLE h)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;
    
    _ASSERTE(m_pDesc == NULL); //  only init once.

    // Windows 95 is not secure, so all the security checks will fail.
    _ASSERTE(!RunningOnWin95() || !"Don't bother doing security checks on Win9x");
     
    DWORD       cbNeeded = 0;

    DWORD flags = OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION;
    
    // Now get the creator's SID. First get the size of the array needed.
    BOOL fOk = GetKernelObjectSecurity(h, flags, NULL, 0, &cbNeeded);
    DWORD err = GetLastError();

    // Caller should give us a handle for which this succeeds. First call will 
    // fail w/ InsufficientBuffer.
    CONSISTENCY_CHECK_MSGF(fOk || (err == ERROR_INSUFFICIENT_BUFFER), ("Failed to get KernelSecurity for object handle=%p.Err=%d\n", h, err));
    
    PSECURITY_DESCRIPTOR pSD = (PSECURITY_DESCRIPTOR) new(nothrow) BYTE[cbNeeded]; 
    if( pSD == NULL )
    {
        return E_OUTOFMEMORY;
    }

    if (GetKernelObjectSecurity(h, flags, pSD, cbNeeded, &cbNeeded) == 0)
    {
        // get last error and fail out.
        err = GetLastError();
        delete [] ((BYTE*) pSD);
        return HRESULT_FROM_WIN32(err);
    }
    
    m_pDesc = pSD;
    return S_OK;
}
void Win32SecurityDescriptor::InitFromHandle(HANDLE h)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    HRESULT hr = InitFromHandleNoThrow(h);
    if (FAILED(hr))
    {
        ThrowHR(hr);
    }      
}

//-----------------------------------------------------------------------------
// We open several named kernel objects that are well-known names decorated with 
// pid of some target process (usually a debuggee).
// Since anybody can create any kernel object with any name, we we want to make
// sure the objects we're opening were actually created by who we think they 
// were. Each kernel object has an "owner" property which serves as the 
// fingerprints of who created the handle.
// 
// Check if the handle owner belongs to either the process specified by the 
// pid or the current process (in case the target process is impersonating us).
// This lets us know if the handle is spoofed.
// 
// Parameters:
//   handle - handle for kernel object to test
//   pid - target process that it may belong to.
//
// Returns:
//   false- if we can verify that Owner(handle) is in the set of { Owner(Process(pid)), or Owner(this Process) }
//          
//   true - Elsewise, including if we can't verify that it's false.
//-----------------------------------------------------------------------------
bool IsHandleSpoofed(HANDLE handle, DWORD pid)
{
    CONTRACTL 
    {
        NOTHROW;
    }
    CONTRACTL_END;
    

    // Win9x has no security, so nothing to check.
    if (RunningOnWin95())
    {
        return false;
    }

    _ASSERTE(handle != NULL);
    _ASSERTE(pid != 0);
    bool fIsSpoofed = true;

    EX_TRY
    {
        // Get the owner of the kernel object referenced by the handle.
        Win32SecurityDescriptor sdHandle;
        sdHandle.InitFromHandle(handle);
        Sid sidOwner(sdHandle.GetOwner());

        SidBuffer sbPidOther;
        SidBuffer sbPidThis;    

        // Is the object owner the "other" pid?
        sbPidOther.InitFromProcess(pid);
        if (Sid::Equals(sbPidOther.GetSid(), sidOwner))
        {
            // We now know that the kernel object was created by the user of the "other" pid.
            // This should be the common case by far. It's not spoofed. All is well.
            fIsSpoofed = false;
            goto Label_Done;
        }
        
        // Test against our current pid if it's different than the "other" pid.
        // This can happen if the other process impersonates us. The most common case would
        // be if we're an admin and the other process (say some service) is impersonating Admin
        // when it spins up the CLR.
        DWORD pidThis = GetCurrentProcessId();
        if (pidThis != pid)
        {
            sbPidThis.InitFromProcess(pidThis);
            if (Sid::Equals(sbPidThis.GetSid(), sidOwner))
            {     
                // The object was created by somebody pretending to be us. If they had sufficient permissions
                // to pretend to be us, then we still trust them.
                fIsSpoofed = false;
                goto Label_Done;
            }
        }
        
        // This should only happen if we're being attacked.
        _ASSERTE(fIsSpoofed);
        STRESS_LOG2(LF_CORDB, LL_INFO1000, "Security Check failed with mismatch. h=%x,pid=%x", handle, pid);

Label_Done: 
        ;        
    }
    EX_CATCH
    {
        // This should only happen if something goes really bad and we can't find the information.
        STRESS_LOG2(LF_CORDB, LL_INFO1000, "Security Check failed with exception. h=%x,pid=%x", handle, pid);
        _ASSERTE(fIsSpoofed); // should still have its default value
    }
    EX_END_CATCH(SwallowAllExceptions);

    return fIsSpoofed;
}


