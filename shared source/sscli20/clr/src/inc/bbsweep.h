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
/*****************************************************************************\
*                                                                             *
* BBSweep.h -    Classes for sweeping profile data to disk                    *
*                                                                             *
*               Version 1.0                                                   *
*******************************************************************************
*                                                                             *
*  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY      *
*  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE        *
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR      *
*  PURPOSE.                                                                   *
*                                                                             *
\*****************************************************************************/

#ifndef _BBSWEEP_H_
#define _BBSWEEP_H_

// The CLR headers don't allow us to use methods like SetEvent directly (instead
// we need to use the host APIs).  However, this file is included both in the CLR
// and in the BBSweep tool, and the host API is not available in the tool.  Moreover,
// BBSweep is not designed to work in an environment where the host controls
// synchronization.  For this reason, we work around the problem by undefining
// these APIs (the CLR redefines them so that they will not be used).
#pragma push_macro("SetEvent")
#pragma push_macro("ResetEvent")
#pragma push_macro("ReleaseSemaphore")
#pragma push_macro("LocalFree")
#undef SetEvent
#undef ResetEvent
#undef ReleaseSemaphore
#undef LocalFree

// MAX_COUNT is the maximal number of runtime processes that can run at a given time
#define MAX_COUNT 20

/* CLRBBSweepCallback is implemented by the CLR which passes it as an argument to WatchForSweepEvents.
 * It is used by BBSweep to tell the CLR to write the profile data to disk at the right time.
 */

class ICLRBBSweepCallback
{
public:
    virtual HRESULT WriteProfileData() = NULL;  // tells the runtime to write the profile data to disk
};

/* BBSweep is used by both the CLR and the BBSweep utility.
 * BBSweep: calls the PerformSweep method which returns after all the CLR processes
 *          have written their profile data to disk. 
 * CLR:     starts up a sweeper thread which calls WatchForSweepEvents and waits until the
 *          sweeper program is invoked.  At that point, all the CLR processes will synchronize
 *          and write their profile data to disk one at a time.  The sweeper threads will then
 *          wait for the next sweep event.  The CLR also calls ShutdownBBSweepThread at
 *          shutdown which returns when the BBSweep thread has terminated.
 */

class BBSweep
{
public:
    BBSweep() 
    {
        // The BBSweep constructor could be called even the the object is not used, so
        // don't do any work here.
        bInitialized = false;
        bTerminate   = false;
        hSweepMutex          = NULL;
        hProfDataWriterMutex = NULL;
        hSweepEvent          = NULL;
        hTerminationEvent    = NULL;
        hProfWriterSemaphore = NULL;
        hBBSweepThread       = NULL;
    }

    ~BBSweep()
    {
        // When the destructor is called, everything should be cleaned up already.

        _ASSERTE(hSweepMutex          == NULL);
        _ASSERTE(hProfDataWriterMutex == NULL);
        _ASSERTE(hSweepEvent          == NULL);
        _ASSERTE(hTerminationEvent    == NULL);
        _ASSERTE(hProfWriterSemaphore == NULL);
    }

    // Called by the sweeper utility to tell all the CLR threads to write their profile
    // data to disk.
    bool PerformSweep()
    {
        bool success = true;

        if (!Initialize()) return false;

        ::WaitForSingleObject(hSweepMutex, INFINITE);
        {
            success = success && ::SetEvent(hSweepEvent);
            {
                for (int i=0; i<MAX_COUNT; i++) 
                {
                    ::WaitForSingleObject(hProfWriterSemaphore, INFINITE);
                }

                ::ReleaseSemaphore(hProfWriterSemaphore, MAX_COUNT, NULL);            

            }
            success = success && ::ResetEvent(hSweepEvent);
        }
        ::ReleaseMutex(hSweepMutex);

        return success;
    }

    // Called by the CLR sweeper thread to wait until a sweep event, at which point
    // it calls back into the CLR via the clrCallback interface to write the profile
    // data to disk.
    bool WatchForSweepEvents(ICLRBBSweepCallback *clrCallback)
    {
        if (!Initialize()) return false;

        bool success = true;

        while (!bTerminate) 
        {
            ::WaitForSingleObject(hSweepMutex, INFINITE);
            {
                ::WaitForSingleObject(hProfWriterSemaphore, INFINITE);
            }
            ::ReleaseMutex(hSweepMutex);

            HANDLE hEvents[2];
            hEvents[0] = hSweepEvent;
            hEvents[1] = hTerminationEvent;
            ::WaitForMultipleObjects(2, hEvents, false, INFINITE);

            ::WaitForSingleObject(hProfDataWriterMutex, INFINITE);
            {
                if (!bTerminate && FAILED(clrCallback->WriteProfileData()))
                    success = false;
            }
            ::ReleaseMutex(hProfDataWriterMutex);

            ::ReleaseSemaphore(hProfWriterSemaphore, 1, NULL);
        }

        return success;
    }

    void SetBBSweepThreadHandle(HANDLE threadHandle)
    {
        hBBSweepThread = threadHandle;
    }

    void ShutdownBBSweepThread()
    {
        // Set the termination event and wait for the BBSweep thread to terminate on its own.
        // Note that this is called by the shutdown thread (and never called by the BBSweep thread).
        if (hBBSweepThread && bInitialized) 
        {
            bTerminate = true;
            ::SetEvent(hTerminationEvent);
            ::WaitForSingleObject(hBBSweepThread, INFINITE);
            Cleanup();
        }
    }

    void Cleanup()
    {
        if (hSweepMutex)          { ::CloseHandle(hSweepMutex);           hSweepMutex =          NULL;}
        if (hProfDataWriterMutex) { ::CloseHandle(hProfDataWriterMutex);  hProfDataWriterMutex = NULL;}
        if (hSweepEvent)          { ::CloseHandle(hSweepEvent);           hSweepEvent =          NULL;}
        if (hTerminationEvent)    { ::CloseHandle(hSweepEvent);           hTerminationEvent =    NULL;}
        if (hProfWriterSemaphore)  { ::CloseHandle(hProfWriterSemaphore);  hProfWriterSemaphore = NULL;}
    }

private:

    bool Initialize()
    {
        OnUnicodeSystem();
        if (!bInitialized) 
        {
            SECURITY_ATTRIBUTES securityAttributes;
            PSECURITY_DESCRIPTOR pSD = NULL;
            PSID pUserSid = NULL, pAdminSid = NULL;
            HANDLE hToken = NULL;
            PACL pACL = NULL;
            LPVOID buffer = NULL;
            SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
            SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;

            pSD = (PSECURITY_DESCRIPTOR) new char[SECURITY_DESCRIPTOR_MIN_LENGTH];
            if (!pSD) goto cleanup;

            // Get the SID for the current user
            DWORD retLength;
            if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
                goto cleanup;

            if (GetTokenInformation(hToken, TokenOwner, NULL, 0, &retLength))
                goto cleanup;
            buffer = (LPVOID) new char[retLength];
            if (!buffer) goto cleanup;
            if (!GetTokenInformation(hToken, TokenOwner, (LPVOID) buffer, retLength, &retLength))
                goto cleanup;

            pUserSid = ((TOKEN_OWNER *) buffer)->Owner;
            
            // Get the SID for the admin group
            // Create a SID for the BUILTIN\Administrators group.
            if(! AllocateAndInitializeSid(&SIDAuthNT, 2,
                                          SECURITY_BUILTIN_DOMAIN_RID,
                                          DOMAIN_ALIAS_RID_ADMINS,
                                          0, 0, 0, 0, 0, 0,
                                          &pAdminSid))
                goto cleanup;

            EXPLICIT_ACCESS ea[2];
            ZeroMemory(ea, 2 * sizeof(EXPLICIT_ACCESS));
                
            // Initialize an EXPLICIT_ACCESS structure for an ACE.
            // The ACE will allow the current user full access
            ea[0].grfAccessPermissions = STANDARD_RIGHTS_ALL | SPECIFIC_RIGHTS_ALL; // KEY_ALL_ACCESS;
            ea[0].grfAccessMode = SET_ACCESS;
            ea[0].grfInheritance= NO_INHERITANCE;
            ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
            ea[0].Trustee.TrusteeType = TRUSTEE_IS_USER;
            ea[0].Trustee.ptstrName  = (LPTSTR) pUserSid;

            // Initialize an EXPLICIT_ACCESS structure for an ACE.
            // The ACE will allow admins full access
            ea[1].grfAccessPermissions = STANDARD_RIGHTS_ALL | SPECIFIC_RIGHTS_ALL; //KEY_ALL_ACCESS;
            ea[1].grfAccessMode = SET_ACCESS;
            ea[1].grfInheritance= NO_INHERITANCE;
            ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
            ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
            ea[1].Trustee.ptstrName  = (LPTSTR) pAdminSid;

            if (SetEntriesInAcl(2, ea, NULL, &pACL) != ERROR_SUCCESS) 
                goto cleanup;
                    
            if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
                goto cleanup;

            if (!SetSecurityDescriptorDacl(pSD, TRUE, pACL, FALSE))
                goto cleanup;

            memset((void *) &securityAttributes, 0, sizeof(SECURITY_ATTRIBUTES));
            securityAttributes.nLength              = sizeof(SECURITY_ATTRIBUTES);
            securityAttributes.lpSecurityDescriptor = pSD;
            securityAttributes.bInheritHandle       = FALSE;

            hSweepMutex          = ::WszCreateMutex(&securityAttributes, false,                    L"BBSweep_hSweepMutex");
            hProfDataWriterMutex = ::WszCreateMutex(&securityAttributes, false,                    L"BBSweep_hProfDataWriterMutex");
            hSweepEvent          = ::WszCreateEvent(&securityAttributes, true, false,              L"BBSweep_hSweepEvent");
            // Note that hTerminateEvent is not a named event.  That is because it is not
            // shared amongst the CLR processes (each process terminates at a different time)
            hTerminationEvent    = ::WszCreateEvent(&securityAttributes, true, false,              NULL);
            hProfWriterSemaphore = ::WszCreateSemaphore(&securityAttributes, MAX_COUNT, MAX_COUNT, L"BBSweep_hProfWriterSemaphore");

            cleanup:
                if (pSD) delete [] ((char *) pSD);
                if (pAdminSid) FreeSid(pAdminSid);
                if (hToken) CloseHandle(hToken);
                if (pACL) LocalFree(pACL);
                if (buffer) delete [] ((char *) buffer);
        }

        bInitialized = hSweepMutex          && 
                      hProfDataWriterMutex &&
                      hSweepEvent          &&
                      hTerminationEvent    &&
                      hProfWriterSemaphore;

        if (!bInitialized) Cleanup();
        return bInitialized;
    }

private:

    bool bInitialized;            // true when the BBSweep object has initialized successfully
    bool bTerminate;              // set to true when the CLR wants us to terminate
    HANDLE hSweepMutex;           // prevents processing from incrementing the semaphore after the sweep has began
    HANDLE hProfDataWriterMutex;  // guarantees that profile data will be written by one process at a time
    HANDLE hSweepEvent;           // tells the CLR processes to sweep their profile data
    HANDLE hTerminationEvent;     // set when the CLR process is ready to terminate
    HANDLE hProfWriterSemaphore;  // helps determine when all the writers are finished
    HANDLE hBBSweepThread;        // a handle to the CLR sweeper thread (that calls watch for sweep events)
};

#pragma pop_macro("LocalFree")
#pragma pop_macro("ReleaseSemaphore")
#pragma pop_macro("ResetEvent")
#pragma pop_macro("SetEvent")

#endif //_BBSWEEP_H
