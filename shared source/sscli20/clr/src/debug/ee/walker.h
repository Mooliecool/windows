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
// File: walker.h
//
// Debugger code stream analysis routines
//
//*****************************************************************************

#ifndef WALKER_H_
#define WALKER_H_

/* ========================================================================= */

/* ------------------------------------------------------------------------- *
 * Constants
 * ------------------------------------------------------------------------- */

enum WALK_TYPE
{
  WALK_NEXT,
  WALK_BRANCH,
  WALK_COND_BRANCH,
  WALK_CALL,
  WALK_RETURN,
  WALK_BREAK,
  WALK_THROW,
  WALK_META,
  WALK_UNKNOWN
};

/* ------------------------------------------------------------------------- *
 * Classes
 * ------------------------------------------------------------------------- */

class Walker
{
protected:
    Walker()
      : m_type(WALK_UNKNOWN), m_registers(NULL), m_ip(0), m_skipIP(0), m_nextIP(0), m_isAbsoluteBranch(false)
      {LEAF_CONTRACT; }

public:

    virtual void Init(const BYTE *ip, REGDISPLAY *pregisters)
    { 
        PREFIX_ASSUME(pregisters != NULL);
        _ASSERTE(GetControlPC(pregisters) == ip);
        
        m_registers = pregisters;
        SetIP(ip);
    }

    const BYTE *GetIP()
      { return m_ip; }

    WALK_TYPE GetOpcodeWalkType()
      { return m_type; }

    const BYTE *GetSkipIP()
      { return m_skipIP; }

    bool IsAbsoluteBranch()
      { return m_isAbsoluteBranch; }

    const BYTE *GetNextIP()
      { return m_nextIP; }

    // We don't currently keep the registers up to date
    virtual void Next() { m_registers = NULL; SetIP(m_nextIP); }
    virtual void Skip() { m_registers = NULL; SetIP(m_skipIP); }

    // Decode the instruction
    virtual void Decode() = 0;

private:
    void SetIP(const BYTE *ip)
      { m_ip = ip; Decode(); }

protected:
    WALK_TYPE           m_type;             // Type of instructions
    REGDISPLAY         *m_registers;        // Registers
    const BYTE         *m_ip;               // Current IP
    const BYTE         *m_skipIP;           // IP if we skip the instruction
    const BYTE         *m_nextIP;           // IP if the instruction is taken
    bool                m_isAbsoluteBranch; // Is it an obsolute branch or not
};

#ifdef _X86_

class NativeWalker : public Walker
{
public:
    void Init(const BYTE *ip, REGDISPLAY *pregisters)
    { 
        m_opcode = 0;
        Walker::Init(ip, pregisters);
    }

    DWORD GetOpcode()
      { return m_opcode; }
/*
    void SetRegDisplay(REGDISPLAY *registers)
      { m_registers = registers; }
*/
    REGDISPLAY *GetRegDisplay()
      { return m_registers; }

    void Decode();
    void DecodeModRM(BYTE mod, BYTE reg, BYTE rm, const BYTE *ip);
    static void DecodeInstructionForPatchSkip(const BYTE *address, BOOL *pFIsCall, BOOL *pFIsAbsBranch);

private:
    DWORD GetRegisterValue(int registerNumber);

    DWORD m_opcode;           // Current instruction or opcode
};

#elif defined (_PPC_)

class NativeWalker : public Walker
{
public:
    void Init(const BYTE *ip, REGDISPLAY *pregisters)
    { 
        m_opcode = 0;
        Walker::Init(ip, pregisters);
    }

    DWORD GetOpcode()
      { return m_opcode; }

    void Decode();

    static void DecodeInstructionForPatchSkip(const BYTE *address, BOOL *pFIsCall, BOOL *pFIsAbsBranch);
private:
    bool TakeBranch(DWORD instruction);

    DWORD m_opcode;           // Current instruction or opcode
};

#else
PORTABILITY_WARNING("NativeWalker not implemented on this platform");
class NativeWalker : public Walker
{
public:
    void Init(const BYTE *ip, REGDISPLAY *pregisters)
    { 
        m_opcode = 0;
        Walker::Init(ip, pregisters);
    }
    DWORD GetOpcode()
      { return m_opcode; }
    void Next()
      { Walker::Next(); }
    void Skip()
      { Walker::Skip(); }

    void Decode()
    {
    PORTABILITY_ASSERT("NativeWalker not implemented on this platform");
        m_type = WALK_UNKNOWN;
        m_skipIP = m_ip++;
        m_nextIP = m_ip++;        
    }

    static void DecodeInstructionForPatchSkip(const BYTE *address, BOOL *pFIsCall, BOOL *pFIsAbsBranch)
    {
    PORTABILITY_ASSERT("NativeWalker not implemented on this platform");
        if (NULL != pFIsCall)
        {
            *pFIsCall = FALSE;
        }
        if (NULL != pFIsAbsBranch)
        {
            *pFIsAbsBranch = FALSE;
        }
    }

private:
    DWORD m_opcode;           // Current instruction or opcode
};
#endif

#endif // WALKER_H_
