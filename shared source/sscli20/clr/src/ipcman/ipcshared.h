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
// File: IPCShared.h
//
// Shared private utility functions for COM+ IPC operations
//
//*****************************************************************************

#ifndef _IPCSHARED_H_
#define _IPCSHARED_H_

class SString;

// This is the name of the file backed session's name on the LS (debuggee)
// Name of the Private (per-process) block. %d resolved to a PID
#define CorPrivateIPCBlock      L"Cor_Private_IPCBlock_%d"
#define CorPublicIPCBlock       L"Cor_Public_IPCBlock_%d"
#define CorPrivateIPCBlock_RS   L"CLR_PRIVATE_RS_IPCBlock_%d"
#define CorPublicIPCBlock_RS    L"CLR_PUBLIC_IPCBlock_%d"

inline bool IsValidHandle(HANDLE handle)
{
    return handle != NULL && handle != INVALID_HANDLE_VALUE;
}

class IPCShared
{
public:
// Close a handle and pointer to any memory mapped file
    static void CloseGenericIPCBlock(HANDLE & hMemFile, void * & pBlock);

// Based on the pid, write a unique name for a memory mapped file
    static void GenerateName(DWORD pid, SString & sName);
    static void GeneratePublicName(DWORD pid, SString & sName);
    static HRESULT CreateWinNTDescriptor(DWORD pid, BOOL bRestrictiveACL, SECURITY_ATTRIBUTES **ppSA);

};

#endif
