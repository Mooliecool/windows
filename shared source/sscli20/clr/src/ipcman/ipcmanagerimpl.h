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
// File: IPCManagerImpl.h
//
// Defines Classes to implement InterProcess Communication Manager for a COM+
//
//*****************************************************************************

#ifndef _IPCManagerImpl_H_
#define _IPCManagerImpl_H_


#include "contract.h"


enum EPrivateIPCClient;

struct PrivateIPCControlBlock;


// Version of the IPC Block that this lib was compiled for.
const int VER_IPC_BLOCK = 2;


//-----------------------------------------------------------------------------
// Implementation for the IPCManager for COM+.
//-----------------------------------------------------------------------------
class IPCWriterImpl
{
public:
    IPCWriterImpl();
    ~IPCWriterImpl();

    // All interface functions should be provided in a derived class

protected:
    // Helpers
    HRESULT CreateWinNTDescriptor(DWORD pid, BOOL bRestrictiveACL, SECURITY_ATTRIBUTES **ppSA);

    void CloseGenericIPCBlock(HANDLE & hMemFile, void * & pBlock);
    HRESULT CreateNewPrivateIPCBlock();
    HRESULT CreateNewPublicIPCBlock();

    void WriteEntryHelper(EPrivateIPCClient eClient, DWORD offs, DWORD size);
    void WriteEntryHelper(EPublicIPCClient eClient, DWORD offs, DWORD size);
    
    void CreatePrivateIPCHeader();
    void CreatePublicIPCHeader();    

    void OpenPrivateIPCHeader();
    void OpenPublicIPCHeader();    

    bool IsPrivateBlockOpen() const;
    bool IsPublicBlockOpen() const;    

    // Cache pointers to each section
    struct PerfCounterIPCControlBlock   *m_pPerf;
    struct DebuggerIPCControlBlock      *m_pDebug;
    struct AppDomainEnumerationIPCBlock *m_pAppDomain;
    PCWSTR                               m_pInstancePath;

    // Stats on MemoryMapped file for the given pid
    HANDLE                               m_handlePrivateBlock;
    HANDLE                               m_handlePublicBlock;    
    PTR_PrivateIPCControlBlock           m_ptrPrivateBlock;
    PTR_PublicIPCControlBlock            m_ptrPublicBlock;    


    PTR_PrivateIPCControlBlock           m_pIPCBackupBlock;
    PTR_PublicIPCControlBlock            m_pIPCBackupBlockPublic;
};


//-----------------------------------------------------------------------------
// IPCReader class connects to a COM+ IPC block and reads from it
//-----------------------------------------------------------------------------
class IPCReaderImpl
{
public:
    IPCReaderImpl();
    ~IPCReaderImpl();

protected:

    HANDLE  m_handlePrivateBlock;
    PrivateIPCControlBlock * m_ptrPrivateBlock;
    HANDLE  m_handlePublicBlock;
    PublicIPCControlBlock * m_ptrPublicBlock;
};



//-----------------------------------------------------------------------------
// Return true if our Private block is available.
//-----------------------------------------------------------------------------
inline bool IPCWriterImpl::IsPrivateBlockOpen() const
{
    LEAF_CONTRACT;
    return m_ptrPrivateBlock != NULL;
}

//-----------------------------------------------------------------------------
// Return true if our Private block is available.
//-----------------------------------------------------------------------------
inline bool IPCWriterImpl::IsPublicBlockOpen() const
{
    LEAF_CONTRACT;
    return m_ptrPublicBlock != NULL;
}


#endif // _IPCManagerImpl_H_
