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
// File: IPCManagerInterface.h
//
// Interface for InterProcess Communication with a COM+ process.
//
//*****************************************************************************

#ifndef _IPCMANAGERINTERFACE_H_
#define _IPCMANAGERINTERFACE_H_

#include "../ipcman/ipcheader.h"

struct PerfCounterIPCControlBlock;
struct DebuggerIPCControlBlock;
struct AppDomainEnumerationIPCBlock;
struct ServiceIPCControlBlock;

#include "../ipcman/ipcmanagerimpl.h"

//
#define CLR_IPC_GENERIC_RIGHT (GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE | STANDARD_RIGHTS_ALL | SECTION_ALL_ACCESS)


//-----------------------------------------------------------------------------
// Interface to the IPCManager for COM+.
// This is a little backwards. To avoid the overhead of a vtable (since
// we don't need it).
// Implementation - the base class with all data and private helper functions
// Interface - derive from base, provide public functions to access impl's data
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Writer - create a COM+ IPC Block with security attributes.
//-----------------------------------------------------------------------------
class IPCWriterInterface : public IPCWriterImpl
{
public:
    //.............................................................................
    // Creation / Destruction only on implementation
    //.............................................................................
    HRESULT Init();
    void Terminate();

    //.............................................................................
    // Create the private IPC block. If this fails because the IPC block has already been
    // created by another module then the phInstIPCBlockOwner argument will be set to the
    // HINSTANCE of the module that created the IPC block.
    // Set inService to TRUE if creating from within a service on behalf of a process.
    //.............................................................................
    HRESULT CreatePrivateBlockOnPid(DWORD PID, BOOL inService, HINSTANCE *phInstIPCBlockOwner);

    //.............................................................................
    // Create the public IPC block. 
    //.............................................................................
    HRESULT CreatePublicBlockOnPid(DWORD PID);

    //.............................................................................
    // Open the private IPC block that has alread been created.
    //.............................................................................
    HRESULT OpenPrivateBlockOnPid(DWORD PID);

    //.............................................................................
    // Open the public IPC block that has alread been created.
    //.............................................................................
    HRESULT OpenPublicBlockOnPid(DWORD PID);

    //.............................................................................
    // ReDacl our private block after it has been created.
    //.............................................................................
    HRESULT ReDaclPrivateBlock(PSECURITY_DESCRIPTOR pSecurityDescriptor);

    //.............................................................................
    // Accessors - return info from header - 
    // These functions work on Private Block
    //.............................................................................
    DWORD       GetBlockVersion();
    DWORD       GetBlockSize();
    HINSTANCE   GetInstance();
    USHORT      GetBuildYear();
    USHORT      GetBuildNumber();
    PVOID       GetBlockStart();
    PCWSTR      GetInstancePath();

    //.............................................................................
    // These functions will work on public block
    //.............................................................................
    DWORD       GetPublicBlockSize();
    PVOID       GetPublicBlockStart();

    //.............................................................................
    // Accessors to get each clients' blocks
    //.............................................................................
    PerfCounterIPCControlBlock *    GetPerfBlock();
    DebuggerIPCControlBlock *   GetDebugBlock();
    AppDomainEnumerationIPCBlock * GetAppDomainBlock();


    //.............................................................................
    // Get Security attributes for a block for a given process. This can be used
    // to create other kernal objects with the same security level.
    //
    // Note: there is no caching, the SD is formed every time, and its not cheap.
    // Note: you must destroy the result with DestroySecurityAttributes().
    //.............................................................................
    HRESULT GetSecurityAttributes(DWORD pid, SECURITY_ATTRIBUTES **ppSA);
    HRESULT GetPublicSecurityAttributes(DWORD pid, SECURITY_ATTRIBUTES **ppSA);    
    void DestroySecurityAttributes(SECURITY_ATTRIBUTES *pSA);
};


//-----------------------------------------------------------------------------
// IPCReader class connects to a COM+ IPC block and reads from it
//-----------------------------------------------------------------------------
class IPCReaderInterface : public IPCReaderImpl
{
public:

    //.............................................................................
    // Create & Destroy
    //.............................................................................
    ~IPCReaderInterface();
    HRESULT OpenPrivateBlockOnPid(DWORD pid);
    HRESULT OpenPrivateBlockOnPid(DWORD pid, DWORD dwDesiredAccess);
    HRESULT OpenPrivateBlockOnPidReadWrite(DWORD pid);
    HRESULT OpenPrivateBlockOnPidReadOnly(DWORD pid);
    HRESULT OpenPublicBlockOnPidReadOnly(DWORD pid);    
    void ClosePrivateBlock();

    HRESULT OpenPublicBlockOnPid(DWORD pid);
    HRESULT OpenPublicBlockOnPid(DWORD pid, DWORD dwDesiredAccess);
    void ClosePublicBlock();

    //.............................................................................
    // Accessors - return info from header
    //.............................................................................
    DWORD       GetBlockVersion();
    DWORD       GetBlockSize();
    HINSTANCE   GetInstance();
    USHORT      GetBuildYear();
    USHORT      GetBuildNumber();
    PVOID       GetBlockStart();
    PCWSTR      GetInstancePath();

    //.............................................................................
    // Check the Block to see if it's corrupted. Returns true if the block is safe
    //.............................................................................
    bool IsValid();

    //.............................................................................
    // Get different sections of the IPC
    //.............................................................................
    void * GetPrivateBlock(EPrivateIPCClient eClient);
    void * GetPublicBlock(EPublicIPCClient eClient);    

    PerfCounterIPCControlBlock *    GetPerfBlockPrivate();
    PerfCounterIPCControlBlock *    GetPerfBlockPublic();    
    DebuggerIPCControlBlock * GetDebugBlock();
    AppDomainEnumerationIPCBlock * GetAppDomainBlock();

    //.............................................................................
    // Return true if we're connected to a memory-mapped file, else false.
    //.............................................................................
    bool IsPrivateBlockOpen() const;
    bool IsPublicBlockOpen() const;    
    HRESULT IsCompatablePlatformForDebuggerAndDebuggee(DWORD pid, bool *pfCompatible);
};

#endif
