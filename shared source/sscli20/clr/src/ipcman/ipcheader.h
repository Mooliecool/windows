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
// File: IPCHeader.h
//
// Define the private header format for COM+ memory mapped files. Everyone
// outside of IPCMan.lib will use the public header, IPCManagerInterface.h
//
//*****************************************************************************

#ifndef _IPCManagerPriv_h_
#define _IPCManagerPriv_h_


//-----------------------------------------------------------------------------
// We must pull in the headers of all client blocks
//-----------------------------------------------------------------------------
#include "../debug/inc/dbgipcevents.h"
#include "perfcounterdefs.h"

//-----------------------------------------------------------------------------
// Each IPC client for a private block (debugging, perf counters, etc)
// has one entry.
// IMPORTANT: Do not remove any entries from this enumeration as
// the directory indexing cannot change from version to version
// in order to preserve compatibility.  The actual directory
// entry for an obsolete index can be zeroed (which all users
// should be written to handle for forward compatibility) but
// this enumeration can only be appended to.
//-----------------------------------------------------------------------------
enum EPrivateIPCClient
{
    ePrivIPC_PerfCounters = 0,
    ePrivIPC_Debugger,
    ePrivIPC_AppDomain,
    ePrivIPC_Obsolete_Service,
    ePrivIPC_Obsolete_ClassDump,
    ePrivIPC_Obsolete_MiniDump,
    ePrivIPC_InstancePath,

// MAX used for arrays, insert above this.
    ePrivIPC_MAX
};

//-----------------------------------------------------------------------------
// Each IPC client for a public block has one entry.
// IMPORTANT: Do not remove any entries from this enumeration as
// the directory indexing cannot change from version to version
// in order to preserve compatibility.  The actual directory
// entry for an obsolete index can be zeroed (which all users
// should be written to handle for forward compatibility) but
// this enumeration can only be appended to.
//-----------------------------------------------------------------------------
enum EPublicIPCClient
{
    ePubIPC_PerfCounters = 0,

// MAX used for arrays, insert above this.
    ePubIPC_MAX
};


//-----------------------------------------------------------------------------
// Entry in the IPC Directory. Ensure binary compatibility across versions
// if we add (or remove) entries.
// If we remove an block, the entry should be EMPTY_ENTRY_OFFSET
//-----------------------------------------------------------------------------

// Since offset is from end of directory, first offset is 0, so we can't
// use that to indicate empty. However, Size can still be 0.
const DWORD EMPTY_ENTRY_OFFSET  = 0xFFFFFFFF;
const DWORD EMPTY_ENTRY_SIZE    = 0;

struct IPCEntry
{
    DWORD m_Offset; // offset of the IPC Block from the end of the Full IPC Header
    DWORD m_Size;       // size (in bytes) of the block
};

//-----------------------------------------------------------------------------
// Private header - put in its own structure so we can easily get the
// size of the header. It will compile to the same thing either way.
// This header must remain completely binary compatible w/ older versions.
//-----------------------------------------------------------------------------
struct PrivateIPCHeader
{
// header
    DWORD       m_dwVersion;    // version of the IPC Block
    DWORD       m_blockSize;    // Size of the entire shared memory block
    HINSTANCE   m_hInstance;    // instance of module that created this header
    USHORT      m_BuildYear;    // stamp for year built
    USHORT      m_BuildNumber;  // stamp for Month/Day built
    DWORD       m_numEntries;   // Number of entries in the table
};

//-----------------------------------------------------------------------------
// This fixes alignment & packing issues.
// This header must remain completely binary compatible w/ older versions.
//-----------------------------------------------------------------------------
struct FullIPCHeader
{
// Header
    struct PrivateIPCHeader             m_header;

// Directory
    IPCEntry m_table[ePrivIPC_MAX]; // entry describing each client's block

};


//-----------------------------------------------------------------------------
// This fixes alignment & packing issues.
// This header must remain completely binary compatible w/ older versions.
//-----------------------------------------------------------------------------
struct FullIPCHeaderPublic
{
// Header
    struct PrivateIPCHeader             m_header;

// Directory
    IPCEntry m_table[ePubIPC_MAX]; // entry describing each client's block

};


// In hindsight, we should have made the offsets be absolute, but we made them
// relative to the end of the FullIPCHeader. 
// The problem is that as future versions added new Entries to the directory,
// the header size grew. 
// Thus we make IPCEntry::m_Offset is relative to IPC_ENTRY_OFFSET_BASE, which
// corresponds to sizeof(PrivateIPCHeader) for an v1.0 /v1.1 build. 
#ifdef _X86_
    const DWORD IPC_ENTRY_OFFSET_BASE = 0x14;
#else
    const DWORD IPC_ENTRY_OFFSET_BASE = 0x0;
#endif


//-----------------------------------------------------------------------------
// Private (per process) IPC Block for COM+ apps
//-----------------------------------------------------------------------------
struct PrivateIPCControlBlock
{
    FullIPCHeader m_FullIPCHeader;


// Client blocks
	struct PerfCounterIPCControlBlock	m_perf;		// no longer used but kept for compat
    struct DebuggerIPCControlBlock      m_dbg;
    struct AppDomainEnumerationIPCBlock m_appdomain;
    WCHAR                               m_instancePath[MAX_PATH];
};

typedef DPTR(PrivateIPCControlBlock) PTR_PrivateIPCControlBlock;


#ifdef _X86_
// For perf reasons, we'd like to keep the IPC block small enough to fit on 
// a single page. This assert ensures it won't silently grow past the page boundary
// w/o us knowing about it. If this assert fires, then either:
// - consciously adjust it to let the IPC block be 2 pages.
// - shrink the IPC blocks.
C_ASSERT(sizeof(PrivateIPCControlBlock) <= 4096);
#endif


//-----------------------------------------------------------------------------
// Public (per process) IPC Block for CLR apps
//-----------------------------------------------------------------------------
struct PublicIPCControlBlock
{
    FullIPCHeaderPublic m_FullIPCHeaderPublic;

// Client blocks
    struct PerfCounterIPCControlBlock   m_perf;
};


typedef DPTR(PublicIPCControlBlock) PTR_PublicIPCControlBlock;



//=============================================================================
// Internal Helpers: Encapsulate any error-prone math / comparisons.
// The helpers are very streamlined and don't handle error conditions.
// Also, Table access functions use DWORD instead of typesafe Enums
// so they can be more flexible (not just for private blocks).
//=============================================================================


//-----------------------------------------------------------------------------
// Internal helper. Enforces a formal definition for an "empty" entry
// Returns true if the entry is empty and false if the entry is usable.
//-----------------------------------------------------------------------------
inline bool Internal_CheckEntryEmpty(
    const PrivateIPCControlBlock & block,   // ipc block
    DWORD Id                                // id of block we want
)
{
// Directory has offset in bytes of block
    const DWORD offset = block.m_FullIPCHeader.m_table[Id].m_Offset;

    return (EMPTY_ENTRY_OFFSET == offset);
}

//-----------------------------------------------------------------------------
// Internal helper. Enforces a formal definition for an "empty" entry
// Returns true if the entry is empty and false if the entry is usable.
//-----------------------------------------------------------------------------
inline bool Internal_CheckEntryEmptyPublic(
    const PublicIPCControlBlock & block,   // ipc block
    DWORD Id                                // id of block we want
)
{
// Directory has offset in bytes of block
    const DWORD offset = block.m_FullIPCHeaderPublic.m_table[Id].m_Offset;

    return (EMPTY_ENTRY_OFFSET == offset);
}


//-----------------------------------------------------------------------------
// Get the base that entry offsets are relative to.
//-----------------------------------------------------------------------------
inline SIZE_T Internal_GetOffsetBase(const PrivateIPCControlBlock & block)
{
    return IPC_ENTRY_OFFSET_BASE + 
           block.m_FullIPCHeader.m_header.m_numEntries 
            * sizeof(IPCEntry);            // skip over directory (variable size)
}

//-----------------------------------------------------------------------------
// Get the base that entry offsets are relative to.
//-----------------------------------------------------------------------------
inline SIZE_T Internal_GetOffsetBasePublic(const PublicIPCControlBlock & block)
{
    return IPC_ENTRY_OFFSET_BASE + 
           block.m_FullIPCHeaderPublic.m_header.m_numEntries 
            * sizeof(IPCEntry);            // skip over directory (variable size)
}


//-----------------------------------------------------------------------------
// Internal helper: Encapsulate error-prone math
// Helper that returns a BYTE* to a block within a header.
//-----------------------------------------------------------------------------
inline BYTE* Internal_GetBlock(
    const PrivateIPCControlBlock & block,   // ipc block
    DWORD Id                                // id of block we want
)
{

// Directory has offset in bytes of block
    const DWORD offset = block.m_FullIPCHeader.m_table[Id].m_Offset;


// This block has been removed. Callee should have caught that and not called us.
    _ASSERTE(!Internal_CheckEntryEmpty(block, Id));
    return
        ((BYTE*) &block)                    // base pointer to start of block
        + Internal_GetOffsetBase(block)
        +offset;                            // jump to block
}


//-----------------------------------------------------------------------------
// Internal helper: Encapsulate error-prone math
// Helper that returns a BYTE* to a block within a header.
//-----------------------------------------------------------------------------
inline BYTE* Internal_GetBlockPublic(
    const PublicIPCControlBlock & block,   // ipc block
    DWORD Id                                // id of block we want
)
{

// Directory has offset in bytes of block
    const DWORD offset = block.m_FullIPCHeaderPublic.m_table[Id].m_Offset;


// This block has been removed. Callee should have caught that and not called us.
    _ASSERTE(!Internal_CheckEntryEmptyPublic(block, Id));
    return
        ((BYTE*) &block)                    // base pointer to start of block
        + Internal_GetOffsetBasePublic(block)
        +offset;                            // jump to block
}



#endif // _IPCManagerPriv_h_
