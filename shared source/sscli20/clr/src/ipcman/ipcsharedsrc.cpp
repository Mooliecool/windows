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
// File: IPCSharedSrc.cpp
//
// Shared source for COM+ IPC Reader & Writer classes
//
//*****************************************************************************

#include "stdafx.h"
#include "ipcshared.h"
#include "ipcmanagerinterface.h"

//-----------------------------------------------------------------------------
// Close a handle and pointer to any memory mapped file
//-----------------------------------------------------------------------------
void IPCShared::CloseGenericIPCBlock(HANDLE & hMemFile, void * & pBlock)
{
    WRAPPER_CONTRACT;

    LOG((LF_CORDB, LL_INFO10, "IPCS::CloseGenericIPCBlock: closing 0x%08x\n", hMemFile));

    if (pBlock != NULL) {
        if (!UnmapViewOfFile(pBlock))
            _ASSERTE(!"UnmapViewOfFile failed");
        pBlock = NULL;
    }

    if (hMemFile != NULL) {
        CloseHandle(hMemFile);
        hMemFile = NULL;
    }
}

//-----------------------------------------------------------------------------
// Based on the pid, write a unique name for a memory mapped file
//-----------------------------------------------------------------------------
void IPCShared::GenerateName(DWORD pid, SString & sName)
{
    WRAPPER_CONTRACT;

    const WCHAR * szFormat = CorPrivateIPCBlock;

    sName.Printf(szFormat, pid);
}

//-----------------------------------------------------------------------------
// Based on the pid, write a unique name for a memory mapped file
//-----------------------------------------------------------------------------
void IPCShared::GeneratePublicName(DWORD pid, SString & sName)
{
    WRAPPER_CONTRACT;

    const WCHAR * szFormat = CorPublicIPCBlock;

    sName.Printf(szFormat, pid);
}


//-----------------------------------------------------------------------------
// Setup a security descriptor for the named kernel objects if we're on NT.
//-----------------------------------------------------------------------------
HRESULT IPCShared::CreateWinNTDescriptor(DWORD pid, BOOL bRestrictiveACL, SECURITY_ATTRIBUTES **ppSA)
{
    WRAPPER_CONTRACT;

    HRESULT hr = NO_ERROR;

    // Gotta have a place to stick the new SA...
    if (ppSA == NULL)
    {
        _ASSERTE(!"Caller must supply ppSA");
        return E_INVALIDARG;
    }

    *ppSA = NULL;


    return hr;
}

