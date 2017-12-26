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
// File: IPCWriterImpl.cpp
//
// Implementation for COM+ memory mapped file writing
//
//*****************************************************************************

#include "stdafx.h"

#include "ipcmanagerinterface.h"
#include "ipcheader.h"
#include "ipcshared.h"


const USHORT BuildYear = VER_ASSEMBLYMAJORVERSION;
const USHORT BuildNumber = VER_ASSEMBLYBUILD;


// Import from mscorwks.obj
HINSTANCE GetModuleInst();


//-----------------------------------------------------------------------------
// Generic init
//-----------------------------------------------------------------------------
HRESULT IPCWriterInterface::Init()
{
    LEAF_CONTRACT;

    // Nothing to do anymore in here...
    return S_OK;
}

//-----------------------------------------------------------------------------
// Generic terminate
//-----------------------------------------------------------------------------
void IPCWriterInterface::Terminate()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10, "IPCWI::Terminate: Writer: closing 0x%08x and 0x%08x\n", m_handlePrivateBlock, m_handlePublicBlock));
    
    if (m_ptrPrivateBlock == m_pIPCBackupBlock)
    {
        // This is the case that we allocate a block of memory and pretending it is the map file view,
        // so we don't need to unmap the file view on m_ptrPrivateBlock/
        m_ptrPrivateBlock = NULL;
    }

    if (m_ptrPublicBlock == m_pIPCBackupBlockPublic)
    {
        // This is the case that we allocate a block of memory and pretending it is the map file view,
        // so we don't need to unmap the file view on m_ptrPublicBlock/
        m_ptrPublicBlock = NULL;
    }
    
    IPCShared::CloseGenericIPCBlock(m_handlePrivateBlock, (void*&) m_ptrPrivateBlock);
    IPCShared::CloseGenericIPCBlock(m_handlePublicBlock, (void*&) m_ptrPublicBlock);    

}



//-----------------------------------------------------------------------------
// ReDacl our private block after it has been created.
//-----------------------------------------------------------------------------
HRESULT IPCWriterInterface::ReDaclPrivateBlock(PSECURITY_DESCRIPTOR pSecurityDescriptor)
{

    return S_OK;
}



//-----------------------------------------------------------------------------
// Open our Private IPC block on the given pid.
//-----------------------------------------------------------------------------
HRESULT IPCWriterInterface::CreatePrivateBlockOnPid(DWORD pid, BOOL inService, HINSTANCE *phInstIPCBlockOwner)
{
#ifndef DACCESS_COMPILE

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // Init the IPC block owner HINSTANCE to 0.
    *phInstIPCBlockOwner = 0;

    // Note: if our private block is open, we shouldn't be creating it again.
    _ASSERTE(!IsPrivateBlockOpen());

    if (IsPrivateBlockOpen())
    {
        // if we goto errExit, it will close the file. We don't want that.
        return HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);
    }

    // Note: if PID != GetCurrentProcessId(), we're expected to be opening
    // someone else's IPCBlock, so if it doesn't exist, we should assert.
    HRESULT hr = S_OK;

    SECURITY_ATTRIBUTES *pSA = NULL;

    EX_TRY
    {
        // Grab the SA
        hr = CreateWinNTDescriptor(pid, TRUE, &pSA);
        if (FAILED(hr))
            ThrowHR(hr);
        
        SString szMemFileName;

        IPCShared::GenerateName(pid, szMemFileName);

        // Connect the handle
        m_handlePrivateBlock = WszCreateFileMapping(INVALID_HANDLE_VALUE,
                                                    pSA,
                                                    PAGE_READWRITE,
                                                    0,
                                                    sizeof(PrivateIPCControlBlock),
                                                    szMemFileName);

        DWORD dwFileMapErr = GetLastError();

        LOG((LF_CORDB, LL_INFO10, "IPCWI::CPBOP: CreateFileMapping of %S, handle = 0x%08x, pid = 0x%8.8x GetLastError=%d\n",
            szMemFileName.GetUnicode(), m_handlePrivateBlock, pid, GetLastError()));

        // If unsuccessful, don't ever bail out.
        if (m_handlePrivateBlock != NULL && dwFileMapErr != ERROR_ALREADY_EXISTS)
        {
            // Get the pointer - must get it even if dwFileMapErr == ERROR_ALREADY_EXISTS,
            // since the IPC block is allowed to already exist if the URT service created it.
            m_ptrPrivateBlock = (PrivateIPCControlBlock *) MapViewOfFile(m_handlePrivateBlock,
                                                                        FILE_MAP_ALL_ACCESS,
                                                                        0, 0, 0);
        }

        if (m_ptrPrivateBlock == NULL)
        {
            m_pIPCBackupBlock = (PrivateIPCControlBlock *) new BYTE[sizeof(PrivateIPCControlBlock)];
            _ASSERTE(m_pIPCBackupBlock != NULL); // throws on OOM.
            m_ptrPrivateBlock = m_pIPCBackupBlock;
        }

        // Hook up each sections' pointers
        CreatePrivateIPCHeader();
    }
    EX_CATCH
    {
        Exception *e = GET_EXCEPTION();
        hr = e->GetHR();
        if (hr == S_OK) 
        {
            hr = E_FAIL;
        }
    }
    EX_END_CATCH(SwallowAllExceptions);

    if (!SUCCEEDED(hr))
    {
        IPCShared::CloseGenericIPCBlock(m_handlePrivateBlock, (void*&)m_ptrPrivateBlock);
    }
    DestroySecurityAttributes(pSA);

    return hr;
#else
    DacNotImpl();
    return E_NOTIMPL;
#endif // DACCESS_COMPILE

}


//-----------------------------------------------------------------------------
// Open our Private IPC block on the given pid.
//-----------------------------------------------------------------------------
HRESULT IPCWriterInterface::CreatePublicBlockOnPid(DWORD pid)
{
#ifndef DACCESS_COMPILE
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // Note: if our public block is open, we shouldn't be creating it again.
    _ASSERTE(!IsPublicBlockOpen());

    if (IsPublicBlockOpen())
    {
        // if we goto errExit, it will close the file. We don't want that.
        return HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);
    }

    // Note: if PID != GetCurrentProcessId(), we're expected to be opening
    // someone else's IPCBlock, so if it doesn't exist, we should assert.
    HRESULT hr = S_OK;

    SECURITY_ATTRIBUTES *pSA = NULL;

    EX_TRY
    {
        // Grab the SA
        hr = CreateWinNTDescriptor(pid, FALSE, &pSA);
        if (FAILED(hr))
            ThrowHR(hr);
        
        SString szMemFileName;

        IPCShared::GeneratePublicName(pid, szMemFileName);

        // Connect the handle
        m_handlePublicBlock = WszCreateFileMapping(INVALID_HANDLE_VALUE,
                                                    pSA,
                                                    PAGE_READWRITE,
                                                    0,
                                                    sizeof(PublicIPCControlBlock),
                                                    szMemFileName);

        LOG((LF_CORDB, LL_INFO10, "IPCWI::CPBOP: CreateFileMapping of %S, handle = 0x%08x, pid = 0x%8.8x GetLastError=%d\n",
            szMemFileName.GetUnicode(), m_handlePublicBlock, pid, GetLastError()));

        // If unsuccessful, don't ever bail out.
        if (m_handlePublicBlock != NULL)
        {
            // Get the pointer - must get it even if ERROR_ALREADY_EXISTS,
            // since the IPC block is allowed to already exist if the URT service created it.
            m_ptrPublicBlock = (PublicIPCControlBlock *) MapViewOfFile(m_handlePublicBlock,
                                                                        FILE_MAP_ALL_ACCESS,
                                                                        0, 0, 0);
        }

        if (m_ptrPublicBlock == NULL)
        {
            m_pIPCBackupBlockPublic = (PublicIPCControlBlock *) new BYTE[sizeof(PublicIPCControlBlock)];
            _ASSERTE(m_pIPCBackupBlockPublic != NULL); // throws on OOM.
            m_ptrPublicBlock = m_pIPCBackupBlockPublic;
        }

        // Hook up each sections' pointers
        CreatePublicIPCHeader();
    }
    EX_CATCH
    {
        Exception *e = GET_EXCEPTION();
        hr = e->GetHR();
        if (hr == S_OK) 
        {
            hr = E_FAIL;
        }
    }
    EX_END_CATCH(SwallowAllExceptions);

    if (!SUCCEEDED(hr))
    {
        IPCShared::CloseGenericIPCBlock(m_handlePublicBlock, (void*&)m_ptrPublicBlock);
    }
    DestroySecurityAttributes(pSA);

    return hr;
#else
    DacNotImpl();
    return E_NOTIMPL;
#endif // DACCESS_COMPILE
}


//-----------------------------------------------------------------------------
// Accessors to get each clients' blocks
//-----------------------------------------------------------------------------
struct PerfCounterIPCControlBlock * IPCWriterInterface::GetPerfBlock()
{
    LEAF_CONTRACT;
    return m_pPerf;
}

struct DebuggerIPCControlBlock * IPCWriterInterface::GetDebugBlock()
{
    LEAF_CONTRACT;
    return m_pDebug;
}

struct AppDomainEnumerationIPCBlock * IPCWriterInterface::GetAppDomainBlock()
{
    LEAF_CONTRACT;
    return m_pAppDomain;
}

//-----------------------------------------------------------------------------
// Return the security attributes for the shared memory for a given process.
//-----------------------------------------------------------------------------
HRESULT IPCWriterInterface::GetSecurityAttributes(DWORD pid, SECURITY_ATTRIBUTES **ppSA)
{
    WRAPPER_CONTRACT;
    return CreateWinNTDescriptor(pid, TRUE, ppSA);
}

//-----------------------------------------------------------------------------
// Return the security attributes for the shared memory for a given process.
//-----------------------------------------------------------------------------
HRESULT IPCWriterInterface::GetPublicSecurityAttributes(DWORD pid, SECURITY_ATTRIBUTES **ppSA)
{
    WRAPPER_CONTRACT;
    return CreateWinNTDescriptor(pid, FALSE, ppSA);
}


//-----------------------------------------------------------------------------
// Helper to destroy the security attributes for the shared memory for a given
// process.
//-----------------------------------------------------------------------------
void IPCWriterInterface::DestroySecurityAttributes(SECURITY_ATTRIBUTES *pSA)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

}

//-----------------------------------------------------------------------------
// Have ctor zero everything out
//-----------------------------------------------------------------------------
IPCWriterImpl::IPCWriterImpl()
{
    LEAF_CONTRACT;

    // Cache pointers to sections
    m_pPerf      = NULL;
    m_pDebug     = NULL;
    m_pAppDomain = NULL;
    m_pInstancePath = NULL;

    // Mem-Mapped file for Private Block
    m_handlePrivateBlock    = NULL;
    m_ptrPrivateBlock       = NULL;

    // Mem-Mapped file for Public Block
    m_handlePublicBlock    = NULL;
    m_ptrPublicBlock       = NULL;

    m_pIPCBackupBlock = NULL;
    m_pIPCBackupBlockPublic = NULL;
}

//-----------------------------------------------------------------------------
// Assert that everything was already shutdown by a call to terminate.
// Shouldn't be anything left to do in the dtor
//-----------------------------------------------------------------------------
IPCWriterImpl::~IPCWriterImpl()
{
#ifndef DACCESS_COMPILE
    LEAF_CONTRACT;

    _ASSERTE(!IsPrivateBlockOpen());
    if (m_pIPCBackupBlock)
    {
        delete [] ((BYTE *)m_pIPCBackupBlock);
    }

    _ASSERTE(!IsPublicBlockOpen());
    if (m_pIPCBackupBlockPublic)
    {
        delete [] ((BYTE *)m_pIPCBackupBlockPublic);
    }
#endif // DACCESS_COMPILE    
}

//-----------------------------------------------------------------------------
// Creation / Destruction
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Setup a security descriptor for the named kernel objects if we're on NT.
//-----------------------------------------------------------------------------
HRESULT IPCWriterImpl::CreateWinNTDescriptor(DWORD pid, BOOL bRestrictiveACL, SECURITY_ATTRIBUTES **ppSA)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    HRESULT hr = NO_ERROR;

    *ppSA = NULL;


    return hr;
}


//-----------------------------------------------------------------------------
// Helper: Fill out a directory entry.
//-----------------------------------------------------------------------------
void IPCWriterImpl::WriteEntryHelper(EPrivateIPCClient eClient,
                                     DWORD offs,
                                     DWORD size)
{
    LEAF_CONTRACT;

    if (offs != EMPTY_ENTRY_OFFSET)
    {
        // The incoming offset is the actual data structure offset
        // but the directory is relative to the end of the full header
        // (on v1.2) so subtract that out.
        
        DWORD offsetBase = (DWORD) Internal_GetOffsetBase(*m_ptrPrivateBlock);
        _ASSERTE(offs >= offsetBase);
        m_ptrPrivateBlock->m_FullIPCHeader.m_table[eClient].m_Offset = (offs - offsetBase);
    }
    else
    {
        m_ptrPrivateBlock->m_FullIPCHeader.m_table[eClient].m_Offset = offs;
    }
    m_ptrPrivateBlock->m_FullIPCHeader.m_table[eClient].m_Size = size;
}

//-----------------------------------------------------------------------------
// Helper: Fill out a directory entry.
//-----------------------------------------------------------------------------
void IPCWriterImpl::WriteEntryHelper(EPublicIPCClient eClient,
                                     DWORD offs,
                                     DWORD size)
{
    LEAF_CONTRACT;

    if (offs != EMPTY_ENTRY_OFFSET)
    {
        // The incoming offset is the actual data structure offset
        // but the directory is relative to the end of the full header
        // (on v1.2) so subtract that out.
        
        DWORD offsetBase = (DWORD) Internal_GetOffsetBasePublic(*m_ptrPublicBlock);
        _ASSERTE(offs >= offsetBase);
        m_ptrPublicBlock->m_FullIPCHeaderPublic.m_table[eClient].m_Offset = (offs - offsetBase);
    }
    else
    {
        m_ptrPublicBlock->m_FullIPCHeaderPublic.m_table[eClient].m_Offset = offs;
    }
    m_ptrPublicBlock->m_FullIPCHeaderPublic.m_table[eClient].m_Size = size;
}

//-----------------------------------------------------------------------------
// Initialize the header for our private IPC block
//-----------------------------------------------------------------------------
void IPCWriterImpl::CreatePrivateIPCHeader()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // Stamp the IPC block with the version
    m_ptrPrivateBlock->m_FullIPCHeader.m_header.m_dwVersion = VER_IPC_BLOCK;
    m_ptrPrivateBlock->m_FullIPCHeader.m_header.m_blockSize = sizeof(PrivateIPCControlBlock);

    m_ptrPrivateBlock->m_FullIPCHeader.m_header.m_hInstance = GetModuleInst();

    m_ptrPrivateBlock->m_FullIPCHeader.m_header.m_BuildYear = BuildYear;
    m_ptrPrivateBlock->m_FullIPCHeader.m_header.m_BuildNumber = BuildNumber;

    m_ptrPrivateBlock->m_FullIPCHeader.m_header.m_numEntries = ePrivIPC_MAX;

    //
    // Fill out directory (offset and size of each block).
    // First fill in the used entries.
    //

    // Even though this first entry is obsolete, it needs to remain
    // here for binary compatibility and can't be marked empty/obsolete
    // as long as m_perf exists in the struct.
    WriteEntryHelper(ePrivIPC_PerfCounters,
                     offsetof(PrivateIPCControlBlock, m_perf),
                     sizeof(PerfCounterIPCControlBlock));
    WriteEntryHelper(ePrivIPC_Debugger,
                     offsetof(PrivateIPCControlBlock, m_dbg),
                     sizeof(DebuggerIPCControlBlock));
    WriteEntryHelper(ePrivIPC_AppDomain,
                     offsetof(PrivateIPCControlBlock, m_appdomain),
                     sizeof(AppDomainEnumerationIPCBlock));
    WriteEntryHelper(ePrivIPC_InstancePath,
                     offsetof(PrivateIPCControlBlock, m_instancePath),
                     sizeof(m_ptrPrivateBlock->m_instancePath));

    //
    // Now explicitly mark the unused entries as empty.
    //

    WriteEntryHelper(ePrivIPC_Obsolete_ClassDump,
                     EMPTY_ENTRY_OFFSET, EMPTY_ENTRY_SIZE);
    WriteEntryHelper(ePrivIPC_Obsolete_MiniDump,
                     EMPTY_ENTRY_OFFSET, EMPTY_ENTRY_SIZE);
    WriteEntryHelper(ePrivIPC_Obsolete_Service,
                     EMPTY_ENTRY_OFFSET, EMPTY_ENTRY_SIZE);

    // Cache our client pointers
    m_pDebug    = &(m_ptrPrivateBlock->m_dbg);
    m_pAppDomain= &(m_ptrPrivateBlock->m_appdomain);
    m_pInstancePath = m_ptrPrivateBlock->m_instancePath;
}

//-----------------------------------------------------------------------------
// Initialize the header for our private IPC block
//-----------------------------------------------------------------------------
void IPCWriterImpl::OpenPrivateIPCHeader()
{
    LEAF_CONTRACT;

    // Cache our client pointers
    m_pDebug    = &(m_ptrPrivateBlock->m_dbg);
    m_pAppDomain= &(m_ptrPrivateBlock->m_appdomain);
    m_pInstancePath = m_ptrPrivateBlock->m_instancePath;
}


//-----------------------------------------------------------------------------
// Initialize the header for our private IPC block
//-----------------------------------------------------------------------------
void IPCWriterImpl::CreatePublicIPCHeader()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // Stamp the IPC block with the version
    m_ptrPublicBlock->m_FullIPCHeaderPublic.m_header.m_dwVersion = VER_IPC_BLOCK;
    m_ptrPublicBlock->m_FullIPCHeaderPublic.m_header.m_blockSize = sizeof(PublicIPCControlBlock);

    m_ptrPublicBlock->m_FullIPCHeaderPublic.m_header.m_hInstance = GetModuleInst();

    m_ptrPublicBlock->m_FullIPCHeaderPublic.m_header.m_BuildYear = BuildYear;
    m_ptrPublicBlock->m_FullIPCHeaderPublic.m_header.m_BuildNumber = BuildNumber;

    m_ptrPublicBlock->m_FullIPCHeaderPublic.m_header.m_numEntries = ePubIPC_MAX;

    //
    // Fill out directory (offset and size of each block).
    // First fill in the used entries.
    //

    WriteEntryHelper(ePubIPC_PerfCounters,
                     offsetof(PublicIPCControlBlock, m_perf),
                     sizeof(PerfCounterIPCControlBlock));

    // Cache our client pointers
    m_pPerf     = &(m_ptrPublicBlock->m_perf);
}

//-----------------------------------------------------------------------------
// Initialize the header for our private IPC block
//-----------------------------------------------------------------------------
void IPCWriterImpl::OpenPublicIPCHeader()
{
    LEAF_CONTRACT;

    // Cache our client pointers
    m_pPerf     = &(m_ptrPublicBlock->m_perf);
}


DWORD IPCWriterInterface::GetBlockVersion()
{
    LEAF_CONTRACT;

    _ASSERTE(IsPrivateBlockOpen());
    return m_ptrPrivateBlock->m_FullIPCHeader.m_header.m_dwVersion;
}

DWORD IPCWriterInterface::GetBlockSize()
{
    LEAF_CONTRACT;

    _ASSERTE(IsPrivateBlockOpen());
    return m_ptrPrivateBlock->m_FullIPCHeader.m_header.m_blockSize;
}

HINSTANCE IPCWriterInterface::GetInstance()
{
    LEAF_CONTRACT;

    _ASSERTE(IsPrivateBlockOpen());
    return m_ptrPrivateBlock->m_FullIPCHeader.m_header.m_hInstance;
}

USHORT IPCWriterInterface::GetBuildYear()
{
    LEAF_CONTRACT;

    _ASSERTE(IsPrivateBlockOpen());
    return m_ptrPrivateBlock->m_FullIPCHeader.m_header.m_BuildYear;
}

USHORT IPCWriterInterface::GetBuildNumber()
{
    LEAF_CONTRACT;

    _ASSERTE(IsPrivateBlockOpen());
    return m_ptrPrivateBlock->m_FullIPCHeader.m_header.m_BuildNumber;
}

PVOID IPCWriterInterface::GetBlockStart()
{
    LEAF_CONTRACT;

    return (PVOID) m_ptrPrivateBlock;
}


PVOID IPCWriterInterface::GetPublicBlockStart()
{
    LEAF_CONTRACT;

    return (PVOID) m_ptrPublicBlock;
}

DWORD IPCWriterInterface::GetPublicBlockSize()
{
    LEAF_CONTRACT;

    _ASSERTE(IsPublicBlockOpen());
    return m_ptrPublicBlock->m_FullIPCHeaderPublic.m_header.m_blockSize;
}

PCWSTR IPCWriterInterface::GetInstancePath()
{
    LEAF_CONTRACT;

    _ASSERTE(IsPrivateBlockOpen());
    return m_pInstancePath;
}
