/* ==++==

   
    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
   
    The use and distribution terms for this software are contained in the file
    named license.txt, which can be found in the root of this distribution.
    By using this software in any fashion, you are agreeing to be bound by the
    terms of this license.
   
    You must not remove this notice, or any other, from this software.
   

  ==--==
*/

/* Since this file defines functions called from the debugger, I don't
   want it to call any PAL code. So, it's a C file to prevent cpp munge from
   being run on it, and it undefs the stdio functions it uses (just printf,
   isprint is a safe macro).
*/

#include <rotor_pal.h>

/* This file defines temporary debugger functions for use FJIT, which we don't
 use on IA64.  Long term, we'll be getting this info via strike.  So just
 don't build it for IA64 for now.
*/
#undef printf
PALIMPORT int __cdecl printf(const char *, ...);

#define MAXIMUM_SEARCH_DISTANCE 1000

typedef unsigned long Address;

/* MethodDesc is defined in ndp/clr/src/vm/method.hpp

   That one is defined as a class. This struct has the
   same data layout. The fields we care about are:
    m_pszDebugClassName
    m_pszDebugMethodName
    m_pszDebugMethodSignature
*/

typedef struct MethodDesc {
#ifdef _DEBUG

    LPCSTR         m_pszDebugMethodName;
    LPSTR          m_pszDebugClassName;
    LPSTR          m_pszDebugMethodSignature;
    PVOID          m_pDebugEEClass;
    PVOID          m_pDebugMethodTable;

#ifdef STRESS_HEAP
    PVOID          m_GcCover;
#else
    DWORD_PTR       m_DebugAlignPad; 
#endif

    unsigned short  m_iPrestubCalls;
    unsigned short  m_iSharedSlots;
    unsigned short  m_iDirectCalls;

    unsigned short  m_DebugAlignPad2;
#endif

    DWORD_PTR      m_CodeOrIL;
    WORD           m_wSlotNumber;
    WORD           m_wFlags;
} MethodDesc;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Private Functions
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifdef _DEBUG

static
int
IsAString (LPCSTR String, size_t Len)
{
    int is_a_string = 0;
    size_t i;
    for (i = 0; is_a_string && i < Len && String[i]; i++) {
        if (!isprint (String[i])) {
            is_a_string = 0;
        }
    }
    return 1;
}

#define MAX_EDGES 4
static const int ACCEPT_STATE=-1, REJECT_STATE=-2;
typedef enum { MATCH_NULL, MATCH_VALUE, MATCH_ANY } MATCH_TYPE;

typedef struct {
    MATCH_TYPE type;
    DWORD      value;
    int        next_state;
} StateEdge;
typedef StateEdge FSM[][MAX_EDGES];

static
Address
regexp_match_at (FSM fsm, Address start, int step_size)
{
    int state = 0, n = 0;
    Address match = 0;
    do {
        int edge;
        DWORD current;
        if (step_size == 1) {
            current = *(unsigned char *)(start + n * step_size);
        }
        else if (step_size == 4) {
            current = *(DWORD *)(start + n * step_size);
        }
        else {
            printf (
                "Internal error in regexp_match_at: invalid step_size: %d\n",
                step_size);
            return 0;
        }
        
        for (edge = 0; edge < MAX_EDGES && fsm[state][edge].type != MATCH_NULL; edge++) {
            switch (fsm[state][edge].type) {
            case MATCH_VALUE:
                if (current == fsm[state][edge].value) {
                    state = fsm[state][edge].next_state;
                    n++;
                    goto advance;
                }
                break;

            case MATCH_ANY:
                state = fsm[state][edge].next_state;
                n++;
                goto advance;

            default:
                printf ("Internal error in regexp_match_at\n");
                return 0;
            }
        }
        state = REJECT_STATE;
    advance:
        ;
    } while (state >= 0);

    if (state == ACCEPT_STATE) {
        match = start;
    }

    return match;
}


/* find_prologue_from_pc implements the bare-bones brute force finite state
   machine we use to find the function prologue. Things to note - we search
   backwards from our starting PC, and our token width (step_size) is the GCD
   of the instruction width on the target platform */

static
PVOID
find_prologue_from_pc (Address PC)
{
    Address start, prologue = 0, retval = 0;
    const int max_search = MAXIMUM_SEARCH_DISTANCE;

#if _X86_
#ifdef FJITONLY
    FSM fsm = {
        /* state */ /* list of edges */                          /* next */
        /* 0 */ { { MATCH_VALUE, 0x55 /* push %ebp               */, 2}, 
                  { MATCH_VALUE, 0xcc /* int  3                  */, 1},
                  { MATCH_VALUE, 0x90 /* nop                     */, 1}, },
        /* 1 */ { { MATCH_VALUE, 0x55 /* push %ebp               */, 2}, },
        /* 2 */ { { MATCH_VALUE, 0x8b /* mov  %esp,%ebp (part 1) */, 3}, },
        /* 3 */ { { MATCH_VALUE, 0xec /* mov  %esp,%ebp (part 2) */, 4}, },
        /* 4 */ { { MATCH_VALUE, 0x56 /* push %esi               */, 5}, },
        /* 5 */ { { MATCH_VALUE, 0x33 /* xor  %esi,%esi (part 1) */, 6}, },
        /* 6 */ { { MATCH_VALUE, 0xf6 /* xor  %esi,%esi (part 2) */, ACCEPT_STATE}, },
    };
    const int step_size = 1;
#else /* FJITONLY */
    FSM fsm = {
        /* state */ /* list of edges */                          /* next */
        /* 0 */ { { MATCH_VALUE, 0x55 /* push %ebp               */, 2}, 
                  { MATCH_VALUE, 0xcc /* int  3                  */, 1},
                  { MATCH_VALUE, 0x90 /* nop                     */, 1}, },
        /* 1 */ { { MATCH_VALUE, 0x55 /* push %ebp               */, 2}, },
        /* 2 */ { { MATCH_VALUE, 0x8b /* mov  %esp,%ebp (part 1) */, 3}, },
        /* 3 */ { { MATCH_VALUE, 0xec /* mov  %esp,%ebp (part 2) */, ACCEPT_STATE}, },
    };
    const int step_size = 1;
    /* note, this prolog will only exist if the jitted method has a full EBP
       frame. We could check g_pConfig->JitFramed(), but that requires pulling
       in lots of C++ header files. g_pConfig->JitFramed() just reads in
       COMPlus_JitFramed on UNIX, so I'll check that */
    char *framed = getenv ("COMPlus_JitFramed");
    if (!framed || !*framed || *framed == '0') {
        printf ("WARNING: COMPlus_JitFramed isn't set. Most jitted functions\n"
                "won't have a EBP frame, so this isn't likely to work.\n\n");
    }
#endif /* FJITONLY */
#elif _PPC_
    FSM fsm = {
        /* state */ /* list of edges */                              /* next */
        /* 0 */ { { MATCH_VALUE, 0x7c0802a6 /* mflr    r0             */, 2}, 
                  { MATCH_VALUE, 0x60000000 /* nop                    */, 1},
                  { MATCH_VALUE, 0x7ef00008 /* tw 23,r16,r0 ; JitHalt */, 1},},
        /* 1 */ { { MATCH_VALUE, 0x7c0802a6 /* mflr    r0             */, 2},},
        /* 2 */ { { MATCH_VALUE, 0x90010008 /* stw     r0,8(r1)       */, 3},},
        /* 3 */ { { MATCH_VALUE, 0x93c1fffc /* stw     r30,-4(r1)     */, 4},},
        /* 4 */ { { MATCH_VALUE, 0x7c3e0b78 /* mr      r30,r1         */, 5},},
        /* 5 */ { { MATCH_VALUE, 0x97a1fff8 /* stwu    r29,-8(r1)     */, ACCEPT_STATE}, },
    };
    const int step_size = 4;
#else
    FSM fsm = {
        /* 0 */ { { 0, 0 } }
    };
    const int step_size = 1;

    printf ("Unsupported platform\n");
    return 0;
#endif

    PC = PC & ~(step_size - 1);

    for (start = PC;
         !prologue && start > 0 && start > PC - max_search * step_size;
         start -= step_size) {
        prologue = regexp_match_at (fsm, start, step_size);
    }

    if (prologue) {
        /* Be greedy. If we have overlapping matches, we want to find the
           first */
        do {
            prologue -= step_size;
        } while (regexp_match_at (fsm, prologue, step_size));
        retval = prologue + step_size;
    }
    else {
        printf ("Failed to find function prologue.\n"
                "You might restart search at 0x%lx\n", start);
    }
    
    return (PVOID)retval;
}

static
PVOID
find_methoddesc_from_prologue (PVOID Prologue)
{
    PVOID pmd = NULL;
    if (Prologue) {
        pmd = (PVOID)*((DWORD *)Prologue - 1);
    }
    return pmd;
}

#endif /* _DEBUG */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Exported Functions
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
DebugPrintMethodDesc (PVOID pMethodDesc)
{
#ifdef _DEBUG
    MethodDesc *pmd = (MethodDesc *)pMethodDesc;
    printf ("MethodDesc %p:\n", pmd);
    if (!IsAString (pmd->m_pszDebugClassName, 1024) ||
        !IsAString (pmd->m_pszDebugMethodName, 1024) ||
        !IsAString (pmd->m_pszDebugMethodSignature, 1024)) {
        printf ("           is not a valid MethodDesc.\n");
    }
    else {
        printf (
            "           Class: %s\n",
            pmd->m_pszDebugClassName);
        printf (
            "           Method: %s\n",
            pmd->m_pszDebugMethodName);
        
        printf (
            "           Signature: %s\n",
            pmd->m_pszDebugMethodSignature);
    }
#else /* DEBUG */
    printf ("DebugPrintMethodDesc: not enabled for free builds\n");
#endif /* DEBUG */
}

PVOID
DebugFindMethodDescFromPC (PVOID PC)
{
#ifdef _DEBUG
    PVOID prologue = find_prologue_from_pc ((Address)PC);
    if (prologue) {
        printf ("Function starts at %p\n", prologue);
    }
    return find_methoddesc_from_prologue (prologue);
#else /* _DEBUG */
    return NULL;
#endif /* _DEBUG */
}

void
DebugPrintMethodDescFromPC (PVOID PC)
{
    PVOID pmd = DebugFindMethodDescFromPC (PC);
    if (!pmd) {
        printf ("Could not find a MethodDesc for pc=%p\n", PC);
    }
    else {
        DebugPrintMethodDesc (pmd);
    }
}

