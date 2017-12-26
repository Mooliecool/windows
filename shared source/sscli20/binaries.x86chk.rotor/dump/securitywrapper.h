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
// File: SecurityWrapper.h
//
// Wrapper around Win32 Security functions
//
//*****************************************************************************

#ifndef _SECURITY_WRAPPER_H
#define _SECURITY_WRAPPER_H

// This file should not even be included on Rotor.

//-----------------------------------------------------------------------------
// Wrapper around a PSID.
// This class does not own the memory.
//-----------------------------------------------------------------------------
class Sid
{
public:
    // Initial the Sid wrapper around an existing SID.
    Sid(PSID pSid);
    static bool Equals(const Sid & a, const Sid & b) { return Equals(a.m_pSid, b.m_pSid); }
    static bool Equals(const Sid & a, PSID b)        { return Equals(a.m_pSid, b); }
    static bool Equals(PSID a, const Sid & b)        { return Equals(a, b.m_pSid); }    
    static bool Equals(PSID a, PSID b);

    PSID RawSid() { return m_pSid; }
protected:
    // Pointer to Sid buffer. We don't owner the data.
    PSID m_pSid;  
};

//-----------------------------------------------------------------------------
// Wrapper around a PSID with buffer.
//-----------------------------------------------------------------------------
class SidBuffer
{
public:
    SidBuffer();
    ~SidBuffer();

    // Get the underlying sid
    Sid GetSid();

    // Do we not have a sid? This will be true if init fails.
    bool IsNull() { return m_pBuffer == NULL; }
    
    // Get a Sid for the owner of the specified process
    HRESULT InitFromProcessNoThrow(DWORD pid);
    void InitFromProcess(DWORD pid); // throws

protected:
    BYTE * m_pBuffer;
};

//-----------------------------------------------------------------------------
// Access Control List.
//-----------------------------------------------------------------------------
class Dacl
{
public:
    Dacl(PACL pAcl);

    SIZE_T GetAceCount();    
    ACE_HEADER * GetAce(SIZE_T dwAceIndex);
protected:
    PACL m_acl;
};

//-----------------------------------------------------------------------------
// Represent a win32 SECURITY_DESCRIPTOR object.
// (Note there's a "SecurityDescriptor" class in the VM for managed goo, 
// so we prefix this with "Win32" to avoid a naming collision.)
//-----------------------------------------------------------------------------
class Win32SecurityDescriptor
{
public:
    Win32SecurityDescriptor();
    ~Win32SecurityDescriptor();

    HRESULT InitFromHandleNoThrow(HANDLE h);
    void InitFromHandle(HANDLE h); // throws

    // Gets the owner SID from this SecurityDescriptor.
    HRESULT GetOwnerNoThrow( PSID* ppSid );
    Sid GetOwner(); // throws
    Dacl GetDacl(); // throws

protected:
    PSECURITY_DESCRIPTOR m_pDesc;
};


//-----------------------------------------------------------------------------
// Check if the handle owner belongs to either the process specified by the pid 
// or the current process. This lets us know if the handle is spoofed.
//-----------------------------------------------------------------------------
bool IsHandleSpoofed(HANDLE handle, DWORD pid);


#endif // _SECURITY_WRAPPER_H
