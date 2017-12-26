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
// File: IPCReaderImpl.cpp
//
// Read a COM+ memory mapped file
//
//*****************************************************************************

#include "stdafx.h"

#include "ipcmanagerinterface.h"
#include "ipcheader.h"
#include "ipcshared.h"
#include <safewrap.h>


//-----------------------------------------------------------------------------
// Ctor sets members
//-----------------------------------------------------------------------------
IPCReaderImpl::IPCReaderImpl()
{
    LEAF_CONTRACT;
    m_handlePrivateBlock = NULL;
    m_ptrPrivateBlock = NULL;
    m_handlePublicBlock = NULL;
    m_ptrPublicBlock = NULL;
}

//-----------------------------------------------------------------------------
// dtor
//-----------------------------------------------------------------------------
IPCReaderImpl::~IPCReaderImpl()
{
    LEAF_CONTRACT;

    LOG((LF_CORDB, LL_INFO10, "IPCRI::IPCReaderImpl::~IPCReaderImpl 0x%08x\n", m_handlePrivateBlock));
    LOG((LF_CORDB, LL_INFO10, "IPCRI::IPCReaderImpl::~IPCReaderImpl 0x%08x (Public)\n", m_handlePublicBlock));
    _ASSERTE(m_handlePrivateBlock == NULL);
    _ASSERTE(m_ptrPrivateBlock == NULL);

    _ASSERTE(m_handlePublicBlock == NULL);
    _ASSERTE(m_ptrPublicBlock == NULL);
}

//-----------------------------------------------------------------------------
// dtor
//-----------------------------------------------------------------------------
IPCReaderInterface::~IPCReaderInterface()
{
    LEAF_CONTRACT;

    LOG((LF_CORDB, LL_INFO10, "IPCRI::IPCReaderInterface::~IPCReaderInterface 0x%08x\n", m_handlePrivateBlock));
    LOG((LF_CORDB, LL_INFO10, "IPCRI::IPCReaderInterface::~IPCReaderInterface 0x%08x (Public)\n", m_handlePublicBlock));

    if (m_handlePrivateBlock)
    {
        ClosePrivateBlock();
    }
    _ASSERTE(m_handlePrivateBlock == NULL);
    _ASSERTE(m_ptrPrivateBlock == NULL);

    if (m_handlePublicBlock)
    {
        ClosePublicBlock();
    }
    _ASSERTE(m_handlePublicBlock == NULL);
    _ASSERTE(m_ptrPublicBlock == NULL);

}

//-----------------------------------------------------------------------------
// Close whatever block we opened
//-----------------------------------------------------------------------------
void IPCReaderInterface::ClosePrivateBlock()
{
    WRAPPER_CONTRACT;


    LOG((LF_CORDB, LL_INFO10, "IPCRI::ClosePrivateBlock 0x%08x\n", m_handlePrivateBlock));
    
    IPCShared::CloseGenericIPCBlock(
        m_handlePrivateBlock,
        (void * &) m_ptrPrivateBlock
    );
    _ASSERTE(m_handlePrivateBlock == NULL);
    _ASSERTE(m_ptrPrivateBlock == NULL);
}

//-----------------------------------------------------------------------------
// Close whatever block we opened
//-----------------------------------------------------------------------------
void IPCReaderInterface::ClosePublicBlock()
{
    WRAPPER_CONTRACT;

    LOG((LF_CORDB, LL_INFO10, "IPCRI::ClosePublicBlock 0x%08x\n", m_handlePublicBlock));
    
    IPCShared::CloseGenericIPCBlock(
        m_handlePublicBlock,
        (void * &) m_ptrPublicBlock
    );
    _ASSERTE(m_handlePublicBlock == NULL);
    _ASSERTE(m_ptrPublicBlock == NULL);
}


HRESULT IPCReaderInterface::IsCompatablePlatformForDebuggerAndDebuggee(
    DWORD       pid,
    bool        *pfCompatible)
{
    if (pfCompatible == NULL)
    {
        return E_INVALIDARG;
    }
    // assume compatible unless otherwise
    
    *pfCompatible = true;
    return S_OK;


}

//-----------------------------------------------------------------------------
// Open our private block
//-----------------------------------------------------------------------------
HRESULT IPCReaderInterface::OpenPrivateBlockOnPid(DWORD pid, DWORD dwDesiredAccess)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;


    // Note: if our private block is open, we shouldn't be attaching to a new one.
    _ASSERTE(!IsPrivateBlockOpen());
    if (IsPrivateBlockOpen())
    {
        // if we goto errExit, it will close the file. We don't want that.
        return HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);
    }

    HRESULT hr = S_OK;

    EX_TRY
    {
        // We should not be trying to open the IPC block of an x86 process from
        // within an ia64 process, or vice versa. This can only happen on
        // Server2003 and later.

        bool    fCompatible = false;
        hr = IsCompatablePlatformForDebuggerAndDebuggee(pid, &fCompatible);
        if (FAILED(hr))
        {
            goto end;
        }
        if (fCompatible == false)
        {
            hr = CORDBG_E_UNCOMPATIBLE_PLATFORMS;
        }


        {
            SString szMemFileName;
            IPCShared::GenerateName(pid, szMemFileName);
            m_handlePrivateBlock = WszOpenFileMapping(dwDesiredAccess,
                                                      FALSE,
                                                      szMemFileName);
            if (m_handlePrivateBlock == NULL)
            {
                hr = HRESULT_FROM_GetLastError();
            }

            LOG((LF_CORDB, LL_INFO10, "IPCRI::OPBOP: CreateFileMapping of %S, handle = 0x%08x, pid = 0x%8.8x GetLastError=%d\n",
                szMemFileName.GetUnicode(), m_handlePrivateBlock, pid, GetLastError()));
            if (m_handlePrivateBlock == NULL)
            {
                goto end;
            }
        }


        m_ptrPrivateBlock = (PrivateIPCControlBlock*) MapViewOfFile(
            m_handlePrivateBlock,
            dwDesiredAccess,
            0, 0, 0);
        if (m_ptrPrivateBlock== NULL)
        {
            hr = HRESULT_FROM_GetLastError();
            goto end;
        }
        // Client must connect their pointers by calling GetXXXBlock() functions
        
        end:;
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
        ClosePrivateBlock();
    }

    return hr;
}


//-----------------------------------------------------------------------------
// Open our public block
//-----------------------------------------------------------------------------
HRESULT IPCReaderInterface::OpenPublicBlockOnPid(DWORD pid, DWORD dwDesiredAccess)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // Note: if our public block is open, we shouldn't be attaching to a new one.
    _ASSERTE(!IsPublicBlockOpen());
    if (IsPublicBlockOpen())
    {
        // if we goto errExit, it will close the file. We don't want that.
        return HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);
    }

    HRESULT hr = S_OK;
    
    EX_TRY
    {
        // We should not be trying to open the IPC block of an x86 process from
        // within an ia64 process, or vice versa. This can only happen on
        // Server2003 and later.

        bool    fCompatible = false;
        hr = IsCompatablePlatformForDebuggerAndDebuggee(pid, &fCompatible);
        if (FAILED(hr))
        {
            goto end;
        }
        if (fCompatible == false)
        {
            hr = CORDBG_E_UNCOMPATIBLE_PLATFORMS;
        }

        {
            SString szMemFileName;
            IPCShared::GeneratePublicName(pid, szMemFileName);
            m_handlePublicBlock = WszOpenFileMapping(dwDesiredAccess,
                                                    FALSE,
                                                    szMemFileName);
            if (m_handlePublicBlock == NULL)
            {
                hr = HRESULT_FROM_GetLastError();
            }

            LOG((LF_CORDB, LL_INFO10, "IPCRI::OPBOP: CreateFileMapping of %S, handle = 0x%08x, pid = 0x%8.8x GetLastError=%d\n",
                szMemFileName.GetUnicode(), m_handlePublicBlock, pid, GetLastError()));
            if (m_handlePublicBlock == NULL)
            {
                goto end;
            }
        }


        m_ptrPublicBlock = (PublicIPCControlBlock*) MapViewOfFile(
            m_handlePublicBlock,
            dwDesiredAccess,
            0, 0, 0);
        if (m_ptrPublicBlock== NULL)
        {
            hr = HRESULT_FROM_GetLastError();
            goto end;
        }

        // Client must connect their pointers by calling GetXXXBlock() functions

        end:;
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
        ClosePublicBlock();
    }

    return hr;
}



//-----------------------------------------------------------------------------
// Open our private block for all access
//-----------------------------------------------------------------------------
HRESULT IPCReaderInterface::OpenPrivateBlockOnPid(DWORD pid)
{
    WRAPPER_CONTRACT;

    return (OpenPrivateBlockOnPid(pid, FILE_MAP_ALL_ACCESS));
}

//-----------------------------------------------------------------------------
// Open our public block for all access
//-----------------------------------------------------------------------------
HRESULT IPCReaderInterface::OpenPublicBlockOnPid(DWORD pid)
{
    WRAPPER_CONTRACT;

    return (OpenPublicBlockOnPid(pid, FILE_MAP_ALL_ACCESS));
}


//-----------------------------------------------------------------------------
// Open our private block for read/write access
//-----------------------------------------------------------------------------
HRESULT IPCReaderInterface::OpenPrivateBlockOnPidReadWrite(DWORD pid)
{
    WRAPPER_CONTRACT;

    return (OpenPrivateBlockOnPid(pid, FILE_MAP_READ | FILE_MAP_WRITE));
}

//-----------------------------------------------------------------------------
// Open our private block for read only access
//-----------------------------------------------------------------------------
HRESULT IPCReaderInterface::OpenPrivateBlockOnPidReadOnly(DWORD pid)
{
    WRAPPER_CONTRACT;

    return (OpenPrivateBlockOnPid(pid, FILE_MAP_READ));
}

//-----------------------------------------------------------------------------
// Open our private block for read only access
//-----------------------------------------------------------------------------
HRESULT IPCReaderInterface::OpenPublicBlockOnPidReadOnly(DWORD pid)
{
    WRAPPER_CONTRACT;

    return (OpenPublicBlockOnPid(pid, FILE_MAP_READ));
}


//-----------------------------------------------------------------------------
// Get a client's private block based on enum
// This is a robust function.
// It will return NULL if:
//  * the IPC block is closed (also ASSERT),
//  * the eClient is out of range (From version mismatch)
//  * the request block is removed (probably version mismatch)
// Else it will return a pointer to the requested block
//-----------------------------------------------------------------------------
void * IPCReaderInterface::GetPrivateBlock(EPrivateIPCClient eClient)
{
    WRAPPER_CONTRACT;

    _ASSERTE(IsPrivateBlockOpen());

// This block doesn't exist if we're closed or out of the table's range
    if (!IsPrivateBlockOpen() || (DWORD) eClient >= m_ptrPrivateBlock->m_FullIPCHeader.m_header.m_numEntries)
    {
        return NULL;
    }

    if (Internal_CheckEntryEmpty(*m_ptrPrivateBlock, eClient))
    {
        return NULL;
    }

    return Internal_GetBlock(*m_ptrPrivateBlock, eClient);
}

//-----------------------------------------------------------------------------
// Get a client's private block based on enum
// This is a robust function.
// It will return NULL if:
//  * the IPC block is closed (also ASSERT),
//  * the eClient is out of range (From version mismatch)
//  * the request block is removed (probably version mismatch)
// Else it will return a pointer to the requested block
//-----------------------------------------------------------------------------
void * IPCReaderInterface::GetPublicBlock(EPublicIPCClient eClient)
{
    WRAPPER_CONTRACT;

    _ASSERTE(IsPublicBlockOpen());

// This block doesn't exist if we're closed or out of the table's range
    if (!IsPublicBlockOpen() || (DWORD) eClient >= m_ptrPublicBlock->m_FullIPCHeaderPublic.m_header.m_numEntries)
    {
        return NULL;
    }

    if (Internal_CheckEntryEmptyPublic(*m_ptrPublicBlock, eClient))
    {
        return NULL;
    }

    return Internal_GetBlockPublic(*m_ptrPublicBlock, eClient);
}

//-----------------------------------------------------------------------------
// Is our private block open?
//-----------------------------------------------------------------------------
bool IPCReaderInterface::IsPrivateBlockOpen() const
{
    LEAF_CONTRACT;

    return m_ptrPrivateBlock != NULL;
}

bool IPCReaderInterface::IsPublicBlockOpen() const
{
    LEAF_CONTRACT;

    return m_ptrPublicBlock != NULL;
}

PerfCounterIPCControlBlock *    IPCReaderInterface::GetPerfBlockPublic()
{
    WRAPPER_CONTRACT;

    return (PerfCounterIPCControlBlock*) GetPublicBlock(ePubIPC_PerfCounters);
}

PerfCounterIPCControlBlock *    IPCReaderInterface::GetPerfBlockPrivate()
{
    WRAPPER_CONTRACT;

    return (PerfCounterIPCControlBlock*) GetPrivateBlock(ePrivIPC_PerfCounters);
}

DebuggerIPCControlBlock * IPCReaderInterface::GetDebugBlock()
{
    WRAPPER_CONTRACT;

    return (DebuggerIPCControlBlock*) GetPrivateBlock(ePrivIPC_Debugger);
}

AppDomainEnumerationIPCBlock * IPCReaderInterface::GetAppDomainBlock()
{
    WRAPPER_CONTRACT;

    return (AppDomainEnumerationIPCBlock*) GetPrivateBlock(ePrivIPC_AppDomain);
}

//-----------------------------------------------------------------------------
// Check if the block is valid. Current checks include:
// * Check Directory structure
//-----------------------------------------------------------------------------
bool IPCReaderInterface::IsValid()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

#ifdef _X86_
    WRAPPER_CONTRACT;

    DWORD offsetExpected = 0;
    DWORD nId = 0;
    DWORD offsetActual;

    for(nId = 0; nId < m_ptrPrivateBlock->m_FullIPCHeader.m_header.m_numEntries; nId ++)
    {
        if (!Internal_CheckEntryEmpty(*m_ptrPrivateBlock, nId))
        {
            offsetActual = m_ptrPrivateBlock->m_FullIPCHeader.m_table[nId].m_Offset;
            if (offsetExpected != offsetActual)
            {
                _ASSERTE(0 && "Invalid IPCBlock Directory Table");
                return false;
            }
            offsetExpected += m_ptrPrivateBlock->m_FullIPCHeader.m_table[nId].m_Size;
        } else {
            if (m_ptrPrivateBlock->m_FullIPCHeader.m_table[nId].m_Size != EMPTY_ENTRY_SIZE)
            {
                _ASSERTE(0 && "Invalid IPCBlock: Empty Block with non-zero size");
                return false;
            }
        }
    }
#else
    LEAF_CONTRACT;

#endif
    return true;
}


DWORD IPCReaderInterface::GetBlockVersion()
{
    WRAPPER_CONTRACT;

    _ASSERTE(IsPrivateBlockOpen());
    return m_ptrPrivateBlock->m_FullIPCHeader.m_header.m_dwVersion;
}

DWORD IPCReaderInterface::GetBlockSize()
{
    WRAPPER_CONTRACT;

    _ASSERTE(IsPrivateBlockOpen());
    return m_ptrPrivateBlock->m_FullIPCHeader.m_header.m_blockSize;
}

HINSTANCE IPCReaderInterface::GetInstance()
{
    WRAPPER_CONTRACT;

    _ASSERTE(IsPrivateBlockOpen());
    return m_ptrPrivateBlock->m_FullIPCHeader.m_header.m_hInstance;
}

USHORT IPCReaderInterface::GetBuildYear()
{
    WRAPPER_CONTRACT;

    _ASSERTE(IsPrivateBlockOpen());
    return m_ptrPrivateBlock->m_FullIPCHeader.m_header.m_BuildYear;
}

USHORT IPCReaderInterface::GetBuildNumber()
{
    WRAPPER_CONTRACT;

    _ASSERTE(IsPrivateBlockOpen());
    return m_ptrPrivateBlock->m_FullIPCHeader.m_header.m_BuildNumber;
}

PVOID IPCReaderInterface::GetBlockStart()
{
    LEAF_CONTRACT;

    return (PVOID) m_ptrPrivateBlock;
}

PCWSTR IPCReaderInterface::GetInstancePath()
{
    WRAPPER_CONTRACT;

    return (PCWSTR) GetPrivateBlock(ePrivIPC_InstancePath);
}
