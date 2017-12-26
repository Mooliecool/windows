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
//
// VirtualCallStubCpu.hpp
//
#ifndef _VIRTUAL_CALL_STUB_X86_H
#define _VIRTUAL_CALL_STUB_X86_H

#ifdef DECLARE_DATA
#include "asmconstants.h"
#include "remoting.h"
#endif

//#define STUB_LOGGING

#include <pshpack1.h>  // Since we are placing code, we want byte packing of the structs

#define USES_LOOKUP_STUBS	1
#define USES_ENTRYPOINT_STUBS	0

/*********************************************************************************************
Stubs that contain code are all part of larger structs called Holders.  There is a
Holder for each kind of stub, i.e XXXStub is contained with XXXHolder.  Holders are
essentially an implementation trick that allowed rearranging the code sequences more
easily while trying out different alternatives, and for dealing with any alignment 
issues in a way that was mostly immune to the actually code sequences.  These Holders
should be revisited when the stub code sequences are fixed, since in many cases they
add extra space to a stub that is not really needed.  

Stubs are placed in cache and hash tables.  Since unaligned access of data in memory
is very slow, the keys used in those tables should be aligned.  The things used as keys
typically also occur in the generated code, e.g. a token as an immediate part of an instruction.
For now, to avoid alignment computations as different code strategies are tried out, the key
fields are all in the Holders.  Eventually, many of these fields should be dropped, and the instruction
streams aligned so that the immediate fields fall on aligned boundaries.  
*/


#if USES_LOOKUP_STUBS

struct LookupStub;
struct LookupHolder;

/*LookupStub**************************************************************************************
Virtual and interface call sites are initially setup to point at LookupStubs.  
This is because the runtime type of the <this> pointer is not yet known, 
so the target cannot be resolved.  Note: if the jit is able to determine the runtime type 
of the <this> pointer, it should be generating a direct call not a virtual or interface call.
This stub pushes a lookup token onto the stack to identify the sought after method, and then 
jumps into the EE (VirtualCallStubManager::ResolveWorkerStub) to effectuate the lookup and
transfer of control to the appropriate target method implementation, perhaps patching of the call site
along the way to point to a more appropriate stub.  Hence callsites that point to LookupStubs 
get quickly changed to point to another kind of stub.
*/
struct LookupStub
{
    inline const BYTE * entryPoint()    { LEAF_CONTRACT; return &_entryPoint[0]; }
    inline size_t token() { LEAF_CONTRACT; return _token; }
    inline size_t       size()          { LEAF_CONTRACT; return sizeof(LookupStub); }

private:
    friend struct LookupHolder;

    // DispatchStub:: _entryPoint expects:
    //       ecx: object (the "this" pointer)
    //       eax: siteAddrForRegisterIndirect if this is a RegisterIndirect dispatch call
    BYTE    _entryPoint [2];    // 50           push    eax             ;save siteAddrForRegisterIndirect - this may be an indirect call
                                // 68           push
    size_t  _token;             // xx xx xx xx          32-bit constant
#ifdef STUB_LOGGING
    BYTE cntr2[2];              // ff 05        inc
    size_t* c_lookup;           // xx xx xx xx          [call_lookup_counter]
#endif //STUB_LOGGING 
    BYTE part2 [1];             // e9           jmp
    DISPL   _resolveWorkerDispl;// xx xx xx xx          pc-rel displ
};

/* LookupHolders are the containers for LookupStubs, they provide for any alignment of 
stubs as necessary.  In the case of LookupStubs, alignment is necessary since
LookupStubs are placed in a hash table keyed by token. */
struct LookupHolder
{
    static void InitializeStatic();

    void  Initialize(const BYTE *resolveWorkerTarget, size_t dispatchToken,
                     VirtualCallStubManager::ThisCallingConvention passThis);

    LookupStub*    stub()               { LEAF_CONTRACT;  return &_stub;    }

    static LookupHolder*  FromStubAddr(const BYTE* stubAddr);
    static LookupHolder*  FromLookupEntry(const BYTE* lookupEntry);

private:
    friend struct LookupStub;

    BYTE align[(sizeof(void*)-(offsetof(LookupStub,_token)%sizeof(void*)))%sizeof(void*)];
    LookupStub _stub;
    BYTE pad[sizeof(void*) -
             ((sizeof(void*)-(offsetof(LookupStub,_token)%sizeof(void*))) +
              (sizeof(LookupStub))
             ) % sizeof(void*)];    //complete DWORD

    C_ASSERT((sizeof(void*) -
             ((sizeof(void*)-(offsetof(LookupStub,_token)%sizeof(void*))) +
              (sizeof(LookupStub))
             ) % sizeof(void*)) != 0);
};

#endif // USES_LOOKUP_STUBS

struct DispatchStub;
struct DispatchHolder;

/*DispatchStub**************************************************************************************
Monomorphic and mostly monomorphic call sites eventually point to DispatchStubs.
A dispatch stub has an expected type (expectedMT), target address (target) and fail address (failure).  
If the calling frame does in fact have the <this> type be of the expected type, then
control is transfered to the target address, the method implementation.  If not, 
then control is transfered to the fail address, a fail stub (see below) where a polymorphic 
lookup is done to find the correct address to go to.  

implementation note: Order, choice of instructions, and branch directions
should be carefully tuned since it can have an inordinate effect on performance.  Particular
attention needs to be paid to the effects on the BTB and branch prediction, both in the small
and in the large, i.e. it needs to run well in the face of BTB overflow--using static predictions.
Note that since this stub is only used for mostly monomorphic callsites (ones that are not, get patched
to something else), therefore the conditional jump "jne failure" is mostly not taken, and hence it is important
that the branch prediction staticly predict this, which means it must be a forward jump.  The alternative 
is to reverse the order of the jumps and make sure that the resulting conditional jump "je implTarget" 
is statically predicted as taken, i.e a backward jump. The current choice was taken since it was easier
to control the placement of the stubs than control the placement of the jitted code and the stubs. */
struct DispatchStub 
{
    inline const BYTE * entryPoint()  { LEAF_CONTRACT;  return &_entryPoint[0]; }

    inline size_t       expectedMT()  { LEAF_CONTRACT;  return _expectedMT;     }
    inline const BYTE * implTarget()  { LEAF_CONTRACT;  return (const BYTE *) &_implDispl + sizeof(DISPL) + _implDispl; }
    inline const BYTE * failTarget()  { LEAF_CONTRACT;  return (const BYTE *) &_failDispl + sizeof(DISPL) + _failDispl; }
    inline size_t       size()        { LEAF_CONTRACT;  return sizeof(DispatchStub); }

private:
    friend struct DispatchHolder;

    // DispatchStub:: _entryPoint expects:
    //       ecx: object (the "this" pointer)
    //       eax: siteAddrForRegisterIndirect if this is a RegisterIndirect dispatch call
#ifndef STUB_LOGGING
    BYTE    _entryPoint [2];    // 81 39        cmp  [ecx],                   ; This is the place where we are going to fault on null this.
    size_t  _expectedMT;        // xx xx xx xx              expectedMT        ; If you change it, change also AdjustContextForVirtualStub in excep.cpp!!!
    BYTE    jmpOp1[2];          // 0f 85        jne                 
    DISPL   _failDispl;         // xx xx xx xx              failEntry         ;must be forward jmp for perf reasons
    BYTE jmpOp2;                // e9           jmp     
    DISPL   _implDispl;         // xx xx xx xx              implTarget
#else //STUB_LOGGING
    BYTE    _entryPoint [2];    // ff 05        inc
    size_t* d_call;             // xx xx xx xx              [call_mono_counter]
    BYTE cmpOp [2];             // 81 39        cmp  [ecx],
    size_t  _expectedMT;        // xx xx xx xx              expectedMT
    BYTE jmpOp1[2];             // 0f 84        je 
    DISPL   _implDispl;         // xx xx xx xx              implTarget        ;during logging, perf is not so important               
    BYTE fail [2];              // ff 05        inc 
    size_t* d_miss;             // xx xx xx xx      [miss_mono_counter]
    BYTE jmpFail;               // e9           jmp     
    DISPL   _failDispl;         // xx xx xx xx              failEntry 
#endif //STUB_LOGGING 
};

/* DispatchHolders are the containers for DispatchStubs, they provide for any alignment of 
stubs as necessary.  DispatchStubs are placed in a hashtable and in a cache.  The keys for both
are the pair expectedMT and token.  Efficiency of the of the hash table is not a big issue,
since lookups in it are fairly rare.  Efficiency of the cache is paramount since it is accessed frequently
o(see ResolveStub below).  Currently we are storing both of these fields in the DispatchHolder to simplify
alignment issues.  If inlineMT in the stub itself was aligned, then it could be the expectedMT field.
While the token field can be logically gotten by following the failure target to the failEntryPoint 
of the ResolveStub and then to the token over there, for perf reasons of cache access, it is duplicated here.
This allows us to use DispatchStubs in the cache.  The alternative is to provide some other immutable struct
for the cache composed of the triplet (expectedMT, token, target) and some sort of reclaimation scheme when
they are thrown out of the cache via overwrites (since concurrency will make the obvious approaches invalid).
*/

/* @hack for ee resolution - Since the EE does not currently have a resolver function that
does what we want, see notes in implementation of VirtualCallStubManager::Resolver, we are 
using dispatch stubs to siumulate what we want.  That means that inlineTarget, which should be immutable
is in fact written.  Hence we have moved target out into the holder and aligned it so we can 
atomically update it.  When we get a resolver function that does what we want, we can drop this field,
and live with just the inlineTarget field in the stub itself, since immutability will hold.*/
struct DispatchHolder
{
    static void InitializeStatic();

    void  Initialize(const BYTE * implTarget, const BYTE * failTarget, size_t expectedMT,
                     VirtualCallStubManager::ThisCallingConvention passThis);

    DispatchStub* stub()      { LEAF_CONTRACT;  return &_stub; }

    static DispatchHolder*  FromStubAddr(const BYTE* stubAddr);
    static DispatchHolder*  FromDispatchEntry(const BYTE* dispatchEntry);

private:
    //force expectedMT to be aligned since used as key in hash tables.
#ifndef STUB_LOGGING
    BYTE align[(sizeof(void*)-(offsetof(DispatchStub,_expectedMT)%sizeof(void*)))%sizeof(void*)];
#endif
    DispatchStub _stub;
    BYTE pad[(sizeof(void*)-(sizeof(DispatchStub)%sizeof(void*))+offsetof(DispatchStub,_expectedMT))%sizeof(void*)];	//complete DWORD
};

struct ResolveStub;
struct ResolveHolder;

/*ResolveStub**************************************************************************************
Polymorphic call sites and monomorphic calls that fail end up in a ResolverStub.  There is only 
one resolver stub built for any given token, even though there may be many call sites that
use that token and many distinct <this> types that are used in the calling call frames.  A resolver stub 
actually has two entry points, one for polymorphic call sites and one for dispatch stubs that fail on their
expectedMT test.  There is a third part of the resolver stub that enters the ee when a decision should
be made about changing the callsite.  Therefore, we have defined the resolver stub as three distinct pieces,
even though they are actually allocated as a single contiguous block of memory.  These pieces are:

A ResolveStub has two entry points:

FailEntry - where the dispatch stub goes if the expected MT test fails.  This piece of the stub does
a check to see how often we are actually failing. If failures are frequent, control transfers to the 
patch piece to cause the call site to be changed from a mostly monomorphic callsite 
(calls dispatch stub) to a polymorphic callsize (calls resolve stub).  If failures are rare, control
transfers to the resolve piece (see ResolveStub).  The failEntryPoint decrements a counter 
every time it is entered.  The ee at various times will add a large chunk to the counter. 

ResolveEntry - does a lookup via in a cache by hashing the actual type of the calling frame s
<this> and the token identifying the (contract,method) pair desired.  If found, control is transfered
to the method implementation.  If not found in the cache, the token is pushed and the ee is entered via
the ResolveWorkerStub to do a full lookup and eventual transfer to the correct method implementation.  Since
there is a different resolve stub for every token, the token can be inlined and the token can be pre-hashed.
The effectiveness of this approach is highly sensitive to the effectiveness of the hashing algorithm used,
as well as its speed.  It turns out it is very important to make the hash function sensitive to all 
of the bits of the method table, as method tables are laid out in memory in a very non-random way.  Before
making any changes to the code sequences here, it is very important to measure and tune them as perf
can vary greatly, in unexpected ways, with seeming minor changes.

Implementation note - Order, choice of instructions, and branch directions
should be carefully tuned since it can have an inordinate effect on performance.  Particular
attention needs to be paid to the effects on the BTB and branch prediction, both in the small
and in the large, i.e. it needs to run well in the face of BTB overflow--using static predictions. 
Note that this stub is called in highly polymorphic cases, but the cache should have been sized
and the hash function chosen to maximize the cache hit case.  Hence the cmp/jcc instructions should
mostly be going down the cache hit route, and it is important that this be statically predicted as so.
Hence the 3 jcc instrs need to be forward jumps.  As structured, there is only one jmp/jcc that typically
gets put in the BTB since all the others typically fall straight thru.  Minimizing potential BTB entries
is important. */

struct ResolveStub 
{
    inline const BYTE * failEntryPoint()     { LEAF_CONTRACT; return &_failEntryPoint[0];    }
    inline const BYTE * resolveEntryPoint()  { LEAF_CONTRACT; return &_resolveEntryPoint[0]; }
    inline const BYTE * slowEntryPoint()     { LEAF_CONTRACT; return &_slowEntryPoint[0]; }

    inline INT32*  pCounter()                { LEAF_CONTRACT; return _pCounter; }
    inline UINT32  hashedToken()             { LEAF_CONTRACT; return _hashedToken >> LOG2_PTRSIZE;    }
    inline size_t  cacheAddress()            { LEAF_CONTRACT; return _cacheAddress;   }
    inline size_t  token()                   { LEAF_CONTRACT; return _token;          }
    inline size_t  size()                    { LEAF_CONTRACT;  return sizeof(ResolveStub); }

private:
    friend struct ResolveHolder;

    // ResolveStub::_failEntryPoint expects:
    //       ecx: object (the "this" pointer)
    //       eax: siteAddrForRegisterIndirect if this is a RegisterIndirect dispatch call
    BYTE   _failEntryPoint [2];     // 83 2d        sub
    INT32* _pCounter;               // xx xx xx xx          [counter],
    BYTE   part0 [2];               // 01                   01
                                    // 7c           jl
    BYTE toPatcher;                 // xx                   backpatcher     ;must be forward jump, for perf reasons
                                    //                                      ;fall into the resolver stub

    // ResolveStub::_resolveEntryPoint expects:
    //       ecx: object (the "this" pointer)
    //       eax: siteAddrForRegisterIndirect if this is a RegisterIndirect dispatch call
    BYTE    _resolveEntryPoint[6];  // 50           push    eax             ;save siteAddrForRegisterIndirect - this may be an indirect call
                                    // 8b 01        mov     eax,[ecx]       ;get the method table from the "this" pointer. This is the place
                                    //                                      ;    where we are going to fault on null this. If you change it,
                                    //                                      ;    change also AdjustContextForVirtualStub in excep.cpp!!!
                                    // 52           push    edx            
                                    // 8b d0        mov     edx, eax
    BYTE    part1 [6];              // c1 e8 0C     shr     eax,12          ;we are adding upper bits into lower bits of mt
                                    // 03 c2        add     eax,edx
                                    // 35           xor     eax,
    UINT32  _hashedToken;           // xx xx xx xx              hashedToken ;along with pre-hashed token
    BYTE    part2 [1];              // 25           and     eax,
    size_t mask;                    // xx xx xx xx              cache_mask
    BYTE part3 [2];                 // 8b 80        mov     eax, [eax+
    size_t  _cacheAddress;          // xx xx xx xx                lookupCache]
#ifdef STUB_LOGGING
    BYTE cntr1[2];                  // ff 05        inc
    size_t* c_call;                 // xx xx xx xx          [call_cache_counter]
#endif //STUB_LOGGING 
    BYTE part4 [2];                 // 3b 10        cmp     edx,[eax+
    // BYTE mtOffset;               //                          ResolverCacheElem.pMT]
    BYTE part5 [1];                 // 75           jne
    BYTE toMiss1;                   // xx                   miss            ;must be forward jump, for perf reasons
    BYTE part6 [2];                 // 81 78        cmp     [eax+
    BYTE tokenOffset;               // xx                        ResolverCacheElem.token],
    size_t  _token;                 // xx xx xx xx              token
    BYTE part7 [1];                 // 75           jne
    BYTE toMiss2;                   // xx                   miss            ;must be forward jump, for perf reasons
    BYTE part8 [2];                 // 8B 40 xx     mov     eax,[eax+
    BYTE targetOffset;              //                          ResolverCacheElem.target]
    BYTE part9 [6];                 // 5a           pop     edx
                                    // 83 c4 04     add     esp,4           ;throw away siteAddrForRegisterIndirect - we don't need it now
                                    // ff e0        jmp     eax
                                    //         miss:
//    BYTE    miss [2];               // 5a           pop     edx             ; don't pop siteAddrForRegisterIndirect - leave it on the stack for use by ResolveWorkerChainLookupAsmStub and/or ResolveWorkerAsmStub
//                                    // b8           mov     eax,
//    UINT32  _hashedTokenMov;        // xx xx xx xx              hashedToken
    BYTE    miss [1];               // 5a           pop     edx             ; don't pop siteAddrForRegisterIndirect - leave it on the stack for use by ResolveWorkerChainLookupAsmStub and/or ResolveWorkerAsmStub
    BYTE    _slowEntryPoint[1];     // 68           push
    size_t  _tokenPush;             // xx xx xx xx          token
#ifdef STUB_LOGGING
    BYTE cntr2[2];                  // ff 05        inc
    size_t* c_miss;                 // xx xx xx xx          [miss_cache_counter]
#endif //STUB_LOGGING
    BYTE part10 [1];                // e9           jmp
    DISPL   _resolveWorkerDispl;    // xx xx xx xx          resolveWorker == ResolveWorkerChainLookupAsmStub or ResolveWorkerAsmStub
    BYTE  patch[1];                 // e8           call
    DISPL _backpatcherDispl;        // xx xx xx xx          backpatcherWorker  == BackPatchWorkerAsmStub
    BYTE  part11 [1];               // eb           jmp
    BYTE toResolveStub;             // xx                   resolveStub, i.e. go back to _resolveEntryPoint
};

/* ResolveHolders are the containers for ResolveStubs,  They provide 
for any alignment of the stubs as necessary. The stubs are placed in a hash table keyed by 
the token for which they are built.  Efficiency of access requires that this token be aligned.  
For now, we have copied that field into the ResolveHolder itself, if the resolve stub is arranged such that
any of its inlined tokens (non-prehashed) is aligned, then the token field in the ResolveHolder
is not needed. */ 
struct ResolveHolder
{
    static void  InitializeStatic();

    void  Initialize(const BYTE *resolveWorkerTarget, const BYTE* patcherTarget, 
                     size_t dispatchToken, UINT32 hashedToken,
                     void * cacheAddr, INT32 * counterAddr,
                     VirtualCallStubManager::ThisCallingConvention passThis);

    ResolveStub* stub()      { LEAF_CONTRACT;  return &_stub; }

    static ResolveHolder*  FromStubAddr(const BYTE* stubAddr);
    static ResolveHolder*  FromFailEntry(const BYTE* failEntry);
    static ResolveHolder*  FromResolveEntry(const BYTE* resolveEntry);

private:
    //align _token in resolve stub

    BYTE align[(sizeof(void*)-((offsetof(ResolveStub,_token))%sizeof(void*)))%sizeof(void*)
#ifdef STUB_LOGGING // This turns out to be zero-sized in stub_logging case, and is an error. So round up.
               +sizeof(void*)
#endif
              ];

    ResolveStub _stub;

//#ifdef STUB_LOGGING // This turns out to be zero-sized in non stub_logging case, and is an error. So remove
    BYTE pad[(sizeof(void*)-((sizeof(ResolveStub))%sizeof(void*))+offsetof(ResolveStub,_token))%sizeof(void*)];	//fill out DWORD
//#endif
};
#include <poppack.h>


#ifdef DECLARE_DATA

#ifndef DACCESS_COMPILE

extern "C" size_t g_dwOOContextAddr;

//NOTE: This is duplicated in assembly in TransparentProxyWorkerAsmStub and
//      DispatchInterfaceCallWorkerAsmStub. If you change this, please
//      update there too
enum
{
    ITF_MASK = 0x1,
    CLS_MASK = 0x2,
    SLOT_MASK = ITF_MASK | CLS_MASK,
    MD_MASK = ~SLOT_MASK
};

#ifndef _MSC_VER

//@ROTORTODO: What do we do with these?
void ResolveWorkerChainLookupAsmStub()
{
    _ASSERTE(!"NYI");
}

#else   // _MSC_VER

#pragma warning(push)
#pragma warning(disable : 4414)     // disable "short jump to function converted to near"

#ifdef CHAIN_LOOKUP
/* This will perform a chained lookup of the entry if the initial cache lookup fails

   Entry stack:
            dispatch token
            siteAddrForRegisterIndirect (used only if this is a RegisterIndirect dispatch call)
            return address of caller to stub
        Also, EAX contains the pointer to the first ResolveCacheElem pointer for the calculated
        bucket in the cache table.
*/
__declspec (naked) void ResolveWorkerChainLookupAsmStub()
{
    enum
    {
        e_token_size                = 4,
        e_indirect_addr_size        = 4,
        e_caller_ret_addr_size      = 4,
    };
    enum
    {
        // this is the part of the stack that is present as we enter this function:
        e_token                     = 0,
        e_indirect_addr             = e_token + e_token_size,
        e_caller_ret_addr           = e_indirect_addr + e_indirect_addr_size,
        e_ret_esp                   = e_caller_ret_addr + e_caller_ret_addr_size,
    };
    enum
    {
        e_spilled_reg_size          = 8,
    };

    // main loop setup
    __asm {
#ifdef STUB_LOGGING
        inc     g_chained_lookup_call_counter
#endif
        // spill regs
        push    edx
        push    ecx
        // move the token into edx
        mov     edx,[esp+e_spilled_reg_size+e_token]
        // move the MT into ecx
        mov     ecx,[ecx]
    }
    main_loop:
    __asm {
        // get the next entry in the chain (don't bother checking the first entry again)
        mov     eax,[eax+e_resolveCacheElem_offset_next]
        // test if we hit a terminating NULL
        test    eax,eax
        jz      fail
        // compare the MT of the ResolveCacheElem
        cmp     ecx,[eax+e_resolveCacheElem_offset_mt]
        jne     main_loop
        // compare the token of the ResolveCacheElem
        cmp     edx,[eax+e_resolveCacheElem_offset_token]
        jne     main_loop
        // success
        // decrement success counter and move entry to start if necessary
        sub     g_dispatch_cache_chain_success_counter,1
        jge     nopromote
        // be quick to reset the counter so we don't get a bunch of contending threads
        add     g_dispatch_cache_chain_success_counter,CALL_STUB_CACHE_INITIAL_SUCCESS_COUNT
        // promote the entry to the beginning of the chain
        mov     ecx,eax
        call    VirtualCallStubManager::PromoteChainEntry
    }
    nopromote:
    __asm {
        // clean up the stack and jump to the target
        pop     ecx
        pop     edx
        add     esp,(e_caller_ret_addr - e_token)
        mov     eax,[eax+e_resolveCacheElem_offset_target]
        jmp     eax
    }
    fail:
    __asm {
#ifdef STUB_LOGGING
        inc     g_chained_lookup_miss_counter
#endif
        // restore registers
        pop     ecx
        pop     edx
        jmp     ResolveWorkerAsmStub
    }
}
#endif 

#endif // _MSC_VER

#ifndef _MSC_VER

//@ROTORTODO: What do we do with these?
void ResolveWorkerAsmStub()
{
    _ASSERTE(!"NYI");
}

void InContextTPDispatchAsmStub()
{
    _ASSERTE(!"NYI");
}

void InContextTPQuickDispatchAsmStub()
{
    _ASSERTE(!"NYI");
}

void TransparentProxyWorkerAsmStub()
{
    _ASSERTE(!"NYI");
}

void DispatchInterfaceCallWorkerAsmStub()
{
    _ASSERTE(!"NYI");
}

void BackPatchWorkerAsmStub()
{
    _ASSERTE(!"NYI");
}

#else

/* Call the resolver, it will return where we are supposed to go.
   There is a little stack magic here, in that we are entered with one
   of the arguments for the resolver (the token) on the stack already.
   We just push the other arguments, <this> in the call frame and the call site pointer, 
   and call the resolver.
   
   On return we have the stack frame restored to the way it was when the ResolveStub
   was called, i.e. as it was at the actual call site.  The return value from
   the resolver is the address we need to transfer control to, simulating a direct
   call from the original call site.  If we get passed back NULL, it means that the
   resolution failed, an unimpelemented method is being called.

   Entry stack:
            dispatch token
            siteAddrForRegisterIndirect (used only if this is a RegisterIndirect dispatch call)
            return address of caller to stub

   Call stack:
            call site
            this object
            dispatch token
            Pointer to MachState object on the stack
            Pointer to enregistered arguments
            MachState (40 bytes)
            Arg 2
            Arg 1
            dispatch token
            return address of caller to stub
   */
__declspec (naked) void ResolveWorkerAsmStub()
{
    CANNOT_HAVE_CONTRACT;

    enum
    {
        e_ret_addr_size                  = 4,
        e_indirect_addr_arg_size         = 4,
        e_this_object_arg_size           = 4,
        e_token_arg_size                 = 4,
        e_ptr_MachState_size             = 4,
        e_ptr_Args_size                  = 4,
        e_MachState_size                 = sizeof(MachState),
        e_arg_2_size                     = 4,
        e_arg_1_size                     = 4,
        e_token_size                     = 4,
        e_indirect_addr_size             = 4,
        e_caller_ret_addr_size           = 4,
    };
    enum
    {
        // this is the part of the stack we push:
        e_ret_addr                  = 0,
        e_indirect_addr_arg         = e_ret_addr + e_ret_addr_size,
        e_this_object_arg           = e_indirect_addr_arg + e_indirect_addr_arg_size,
        e_token_arg                 = e_this_object_arg + e_this_object_arg_size,
        e_ptr_MachState             = e_token_arg + e_token_arg_size,
        e_ptr_Args                  = e_ptr_MachState + e_ptr_MachState_size,
        e_MachState                 = e_ptr_Args + e_ptr_Args_size,
        e_arg_2                     = e_MachState + e_MachState_size,
        e_arg_1                     = e_arg_2 + e_arg_2_size,
        // this is the part of the stack that is present as we enter this function:
        e_token                     = e_arg_1 + e_arg_1_size,
        e_indirect_addr             = e_token + e_token_size,
        e_caller_ret_addr           = e_indirect_addr + e_indirect_addr_size,
        e_ret_esp                   = e_caller_ret_addr + e_caller_ret_addr_size,
    };
    __asm {
        // Save the enregistered arguments
        push   ARGUMENT_REG1    // pass arg1    Need to be in this order for ArgIterator
        push   ARGUMENT_REG2    // pass arg2    (save the value)

        // From here we are making a MachState structure.  
        lea    edx, [esp+(e_caller_ret_addr-e_arg_2)]
        push   edx              // address of return address of whoever called the thunk
        lea    edx, [esp+(e_ret_esp-e_arg_2)+4]
        push   edx              // the ESP after we return.
        push   ebp          
        push   esp              // pEbp
        push   ebx          
        push   esp              // pEbx
        push   esi          
        push   esp              // pEsi
        push   edi          
        push   esp              // pEdi

        // ARG 6 - ArgumentRegisters*
        lea    edx, [esp+(e_arg_2-e_MachState)] // pointer to the struct containing the enregisted arguments 
        push   edx          

        // ARG 5 - MachState*
        lea    edx, [esp+(e_MachState-e_ptr_Args)]  // pointer to the MachState structure 
        push   edx          

        // ARG 4 - dispatch token
        mov    eax, [esp+(e_token-e_ptr_MachState)]
        push   eax

        // ARG 3 - Object*
        push   ecx

        // ARG 2 - siteAddrForRegisterIndirect
        mov    edx, [esp+(e_indirect_addr-e_this_object_arg)]
        push   edx

        // ARG 1 - Return address
        mov    edx, [esp+(e_caller_ret_addr-e_indirect_addr_arg)]
        push   edx

        // Make the call
        call   VirtualCallStubManager::ResolveWorkerStatic

        // Don't pop off the arguments, it's a __stdcall method so callee does it

        // Restore the registers
        mov     edi, [esp+4] // restore regs
        mov     esi, [esp+12]
        mov     ebx, [esp+20]
        mov     ebp, [esp+28]
        add     esp, 40         // pop off sizeof(MachineState)

        pop     ARGUMENT_REG2   // restore arg2
        pop     ARGUMENT_REG1   // restore arg1 

        // On success, clear the stack and jump to the target
        add     esp,(e_caller_ret_addr - e_token)
        
        // Now jump to the target
        jmp     eax             // continue on into the method
    }
}

/*  For an in-context dispatch, we will find the target. This
    is the slow path, and erects a MachState structure for 
    creating a HelperMethodFrame

    Entry stack:
            dispatch token
            return address of caller to stub

    Call stack:
            call site
            this object
            dispatch slot number of interface MD
            Pointer to MachState object on the stack
            Pointer to enregistered arguments
            MachState (40 bytes)
            Arg 2
            Arg 1
            dispatch slot number of interface MD
            return address of caller to stub
*/    
__declspec (naked) void InContextTPDispatchAsmStub()
{
    CANNOT_HAVE_CONTRACT;

    enum
    {
        e_token_arg_size                 = 4,
        e_target_mt_size                 = 4,
        e_ptr_MachState_size             = 4,
        e_ptr_Args_size                  = 4,
        e_MachState_size                 = sizeof(MachState),
        e_arg_2_size                     = 4,
        e_arg_1_size                     = 4,
        e_token_size                     = 4,
        e_caller_ret_addr_size           = 4,
    };
    enum
    {
        e_token_arg                 = 0,
        e_target_mt                 = e_token_arg + e_token_arg_size,
        e_ptr_MachState             = e_target_mt + e_target_mt_size,
        e_ptr_Args                  = e_ptr_MachState + e_ptr_MachState_size,
        e_MachState                 = e_ptr_Args + e_ptr_Args_size,
        e_arg_2                     = e_MachState + e_MachState_size,
        e_arg_1                     = e_arg_2 + e_arg_2_size,
        e_token                     = e_arg_1 + e_arg_1_size,
        e_caller_ret_addr           = e_token + e_token_size,
        e_ret_esp                   = e_caller_ret_addr + e_caller_ret_addr_size,
    };
    __asm {
        // Save the enregistered arguments
        push   ARGUMENT_REG1    // pass arg1    Need to be in this order for ArgIterator
        push   ARGUMENT_REG2    // pass arg2    (save the value)

        // From here we are making a MachState structure.  
        lea    edx, [esp+(e_caller_ret_addr-e_arg_2)]
        push   edx              // address of return address of whoever called the thunk
        lea    edx, [esp+(e_ret_esp-e_arg_2)+4]
        push   edx              // the ESP after we return.
        push   ebp          
        push   esp              // pEbp
        push   ebx          
        push   esp              // pEbx
        push   esi          
        push   esp              // pEsi
        push   edi          
        push   esp              // pEdi

        // ARG 4 - ArgumentRegisters*
        lea    edx, [esp+(e_arg_2-e_MachState)] // pointer to the struct containing the enregisted arguments 
        push   edx          

        // ARG 3 - MachState*
        lea    edx, [esp+(e_MachState-e_ptr_Args)]  // pointer to the MachState structure 
        push   edx          

        // Arg 2 - MethodTable*
        mov    eax, [ecx + TP_OFFSET_MT]
        push   eax

        // ARG 1 - dispatch token
        mov    eax, [esp+(e_token-e_target_mt)]
        push   eax

        // Make the call
        call   VirtualCallStubManager::GetTargetForTPWorker

        // Don't pop off the arguments, it's a __stdcall method so callee does it

        // Restore the registers
        mov     edi, [esp+4] // restore regs
        mov     esi, [esp+12]
        mov     ebx, [esp+20]
        mov     ebp, [esp+28]
        add     esp, 40         // pop off sizeof(MachineState)

        pop     ARGUMENT_REG2   // restore arg2
        pop     ARGUMENT_REG1   // restore arg1 

        // pop off dispatch token
        add     esp, 4
        
        // Now jump to the target
        jmp     eax             // continue on into the method
    }
}

/*  For an in-context dispatch, we will try to find the target in
    the resolve cache. If this fails, we will jump to the full
    version of InContextTPDispatchAsmStub
    
    Entry stack:
        dispatch slot number of interface MD
        caller return address
    ECX: this object
*/    
__declspec (naked) void InContextTPQuickDispatchAsmStub()
{
    CANNOT_HAVE_CONTRACT;

    __asm {
        // Spill registers
        push        ecx
        push        edx

        // Arg 2 - MethodTable
        mov         eax, [ecx + TP_OFFSET_MT]
        push        eax

        // Arg 1 -  token
        mov         eax, [esp + 12]
        push        eax

        // Make the call
        call        VirtualCallStubManager::GetTargetForTPWorkerQuick

        // Restore registers
        pop         edx
        pop         ecx

        // Test to see if we found a target
        test        eax, eax
        jnz         TargetFound

        // If no target, jump to the slow worker
        jmp         InContextTPDispatchAsmStub

    TargetFound:
        // We got a target, so pop off the token and jump to it
        add         esp,4
        jmp         eax
    }
}

/*  This will check to see if a TP object is in context, and if it is will look in
    the hash for a match. If not, call the heavyweight worker.
    
    Entry stack:
        dispatch token
        caller return address
    ECX: this object
*/    
__declspec (naked) void TransparentProxyWorkerAsmStub()
{
    CANNOT_HAVE_CONTRACT;

    __asm {
        // Check for in context
        mov     eax, [ecx + TP_OFFSET_STUBDATA]
        call    [ecx + TP_OFFSET_STUB]
#ifdef _DEBUG
        nop     // Mark this as a special call site that can directly
                // call managed code
#endif
        test    eax, eax
        jz      InContextTPQuickDispatchAsmStub

        //
        // OUT OF CONTEXT
        //

        // Mask off the token flags to get a method desc
        // NOTE: This depends on SLOT_MASK being 0x3
        mov     eax, [esp]
        and     eax, MD_MASK
        mov     [esp],eax

        // Jump to OOContext label in TPStub
        jmp     g_dwOOContextAddr;
    }
}

/*  This will munge the interface MethodDesc to be an interface token.
    Then it will call the regular TP worker that a stub calls directly.
    
    Entry stack:
        interface MethodDesc
        caller return address
    ECX: this object
*/    
__declspec (naked) void DispatchInterfaceCallWorkerAsmStub()
{
    CANNOT_HAVE_CONTRACT;

    __asm {
        // Mask off the token flags to get a method desc
        // NOTE: This depends on SLOT_MASK being 0x3
        mov     eax, [esp]
        or      eax, ITF_MASK
        mov     [esp],eax

        jmp     TransparentProxyWorkerAsmStub
    }
}

/* Call the callsite back patcher.  The fail stub piece of the resolver is being
call too often, i.e. dispatch stubs are failing the expect MT test too often.
In this stub wraps the call to the BackPatchWorker to take care of any stack magic
needed.
*/
__declspec (naked) void BackPatchWorkerAsmStub()
{
    CANNOT_HAVE_CONTRACT;

    __asm {
        push EBP
        mov ebp,esp
        push EAX         // it may contain siteAddrForRegisterIndirect
        push EBX
        push ECX
        push EDX
        push ESI
        push EDI
        mov EBX,[EBP+8]  // fetch the return address
        push EAX         //  push any indirect call address as the second arg to BackPatchWorker
        push EBX         //  and push it as the first arg to BackPatchWorker
        call VirtualCallStubManager::BackPatchWorkerStatic
        pop EDI
        pop ESI
        pop EDX
        pop ECX
        pop EBX
        pop EAX
        mov esp,ebp
        pop ebp
        ret
    }
}
#endif // _MSC_VER

#pragma warning(pop) // 4414: disable "short jump to function converted to near"

// the special return address for VSD tailcalls
extern "C" void __stdcall JIT_TailCallReturnFromVSD();


#endif // #ifndef DACCESS_COMPILE

//----------------------------------------------------------------------------
BOOL VirtualCallStubManager::TraceManager(Thread *thread, 
                              TraceDestination *trace,
                              CONTEXT *pContext, 
                              BYTE **pRetAddr)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    {
        TADDR pStub = (TADDR)pContext->Eip;
        // The return address should be on the top of the stack
        *pRetAddr = (BYTE*)(TADDR)*PTR_TADDR((TADDR)pContext->Esp);

        // Get the token from the stub
        CONSISTENCY_CHECK(isStub(pStub));
        size_t token = GetTokenFromStub(pStub);

        // Get the this object from ECX
        Object *pObj = PTR_Object((TADDR)pContext->Ecx);

        // Call common trace code.
        return (TraceResolver(pObj, token, trace));
    }
}

//----------------------------------------------------------------------------
BOOL VirtualCallStubManagerManager::TraceManager(
                    Thread *thread, TraceDestination *trace,
                    CONTEXT *pContext, BYTE **pRetAddr)
{
    WRAPPER_CONTRACT;

    // Find the owning manager. We should succeed, since presumably someone already
    // called CheckIsStub on us to find out that we own the address.
    VirtualCallStubManager *pMgr = FindVirtualCallStubManager((TADDR)pContext->Eip);
    CONSISTENCY_CHECK(CheckPointer(pMgr));

    // Forward the call to the appropriate manager.
    return pMgr->TraceManager(thread, trace, pContext, pRetAddr);
}

#ifndef DACCESS_COMPILE

//
// This function verifies that a pointer to an indirection cell lives inside a delegate object.
// The whole check that the pointer actually is inside a delegate object is only performed in checked builds,
// otherwise the fact that the pointer is in the GCHeap is enough to "declare" the call stub as a virtual dispatch for delegates.
// In the deelgate case the indirection cell is held by the delegate itself in _methodPtrAux, when the delegate Invoke is
// called the shuffle thunk is first invoked and that will call into the virtual dispatch stub.
// Before control is given to the virtual dispatch stub a pointer to the indirection cell (thus an interior pointer to the delegate)
// is pushed in EAX
//
BOOL isDelegateCall(BYTE *interiorPtr)
{
    WRAPPER_CONTRACT; 
    if (GCHeap::GetGCHeap()->IsHeapPointer((void*)interiorPtr))
    {
#ifdef _DEBUG
        Object *delegate = (Object*)(interiorPtr - DelegateObject::GetOffsetOfMethodPtrAux());
        VALIDATEOBJECTREF(ObjectToOBJECTREF(delegate));
        _ASSERTE(delegate->GetMethodTable()->IsAnyDelegateClass());
#endif
        return TRUE;
    }
    return FALSE;
}

const BYTE ***StubCallSite::ComputeSiteAddr(const BYTE *returnAddr, const BYTE ***siteAddrForRegisterIndirect)
{ 
    LEAF_CONTRACT; 
    if (isCallRelative(returnAddr) || isCallRelativeIndirect(returnAddr))
    {
        return (const BYTE ***) (returnAddr - sizeof(DISPL));
    }
    else
    {
        // if it's not a "classic" code sequence for a virtual call must be either a call site for generics virtual invocation
        // or a virtual delegate disaptch.
        // Both these cases carry a pointer to the indirection cell in EAX. 
        // EAX is eventually siteAddrForRegisterIndirect which is what this function returns as a site addr.
        // In the delegate case the shuffle thunk is responsible of pushing the indirection cell in the proper register (EAX for x86)
        _ASSERTE(isCallRegisterIndirect(returnAddr) || isDelegateCall((BYTE*)siteAddrForRegisterIndirect));
        return siteAddrForRegisterIndirect;
    }
}

const BYTE * StubCallSite::GetCallerAddress()
{
    LEAF_CONTRACT; 
    if (m_returnAddr != (const BYTE *)JIT_TailCallReturnFromVSD)
        return m_returnAddr;

    // Find the tailcallframe in the frame chain and get the actual caller from the first TailCallFrame
    return (const BYTE*)TailCallFrame::FindTailCallFrame(GetThread()->GetFrame())->GetCallerAddress();
}

const BYTE **  StubCallSite::GetIndirectCell()
{ 
    LEAF_CONTRACT; 
    _ASSERTE(!IsRelative()); 
    if (IsRegisterIndirect() || IsDelegateCallSite())
        return (const BYTE **)m_siteAddr; 
    else
        return *m_siteAddr; 
}


// Private utility functions
DISPL StubCallSite::ComputeJmpRelTargetEncoding(const BYTE * target)
{
    LEAF_CONTRACT;

    return target - GetReturnAddress();
}
    
const BYTE * StubCallSite::ComputeJmpRelTarget()
{
    LEAF_CONTRACT; 

    return GetReturnAddress() + *((DISPL *)m_siteAddr); 
}

// Public utility functions
BOOL StubCallSite::IsRelative()
{
    WRAPPER_CONTRACT;

    return isCallRelative(GetReturnAddress());
}

BOOL StubCallSite::IsRelativeIndirect()
{
    WRAPPER_CONTRACT;

    return isCallRelativeIndirect(GetReturnAddress());
}

BOOL StubCallSite::IsRegisterIndirect()
{
    WRAPPER_CONTRACT;

    return isCallRegisterIndirect(GetReturnAddress());
}

// see comment on isDelegateCall 
BOOL StubCallSite::IsDelegateCallSite()
{
    WRAPPER_CONTRACT;

    return isDelegateCall((BYTE*)m_siteAddr);
}



const BYTE * StubCallSite::GetSiteTarget()
{
    WRAPPER_CONTRACT;

    if (IsRelativeIndirect())
        return *(GetIndirectCell());
    else if (IsRelative())
        return ComputeJmpRelTarget();
    else
    {
        _ASSERTE(IsRegisterIndirect() || IsDelegateCallSite());
        return *((BYTE **) m_siteAddr);
    }

}

void StubCallSite::SetSiteTarget(const BYTE * newTarget)
{
    WRAPPER_CONTRACT;

    if (IsRelativeIndirect())
        *(GetIndirectCell()) = newTarget;
    else if (IsRelative())
        *((DISPL *)m_siteAddr) = ComputeJmpRelTargetEncoding(newTarget);
    else
    {
        _ASSERTE(IsRegisterIndirect() || IsDelegateCallSite());
        *((const BYTE **)m_siteAddr) = newTarget;
    }
}

#ifdef STUB_LOGGING
extern size_t g_lookup_inline_counter;
extern size_t g_mono_call_counter;
extern size_t g_mono_miss_counter;
extern size_t g_poly_call_counter;
extern size_t g_poly_miss_counter;
#endif


/* Template used to generate the stub.  We generate a stub by allocating a block of 
   memory and copy the template over it and just update the specific fields that need 
   to be changed.
*/ 
LookupStub lookupInit;

void LookupHolder::InitializeStatic()
{
    C_ASSERT(((offsetof(LookupStub, _token)+offsetof(LookupHolder, _stub)) % sizeof(void*)) == 0);
    C_ASSERT((sizeof(LookupHolder) % sizeof(void*)) == 0);

    lookupInit._entryPoint [0]     = 0x50;
    lookupInit._entryPoint [1]     = 0x68;
    C_ASSERT(sizeof(lookupInit._entryPoint) == 2);
    lookupInit._token              = 0xcccccccc;
#ifdef STUB_LOGGING
    lookupInit.cntr2 [0]           = 0xff;
    lookupInit.cntr2 [1]           = 0x05;
    C_ASSERT(sizeof(lookupInit.cntr2) == 2);
    lookupInit.c_lookup            = &g_call_lookup_counter;
#endif //STUB_LOGGING 
    lookupInit.part2 [0]           = 0xe9;
    C_ASSERT(sizeof(lookupInit.part2) == 1);
    lookupInit._resolveWorkerDispl = 0xcccccccc;
}

void  LookupHolder::Initialize(const BYTE *resolveWorkerTarget, size_t dispatchToken,
                               VirtualCallStubManager::ThisCallingConvention passThis)
{
    _stub = lookupInit;

    _stub._token              = dispatchToken;
    _stub._resolveWorkerDispl = resolveWorkerTarget - ((const BYTE *) &_stub._resolveWorkerDispl + sizeof(DISPL));
}

LookupHolder* LookupHolder::FromStubAddr(const BYTE* stubAddr)
{ 
    LEAF_CONTRACT;
    LookupHolder* lookupHolder = (LookupHolder*) ( stubAddr - offsetof(LookupHolder, _stub) );
    //    _ASSERTE(lookupHolder->_stub._entryPoint[0] == lookupInit._entryPoint[0]);
    return lookupHolder;
}

LookupHolder* LookupHolder::FromLookupEntry(const BYTE* lookupEntry)
{ 
    LEAF_CONTRACT;
    LookupHolder* lookupHolder = (LookupHolder*) ( lookupEntry - offsetof(LookupHolder, _stub) - offsetof(LookupStub, _entryPoint)  );
    //    _ASSERTE(lookupHolder->_stub._entryPoint[0] == lookupInit._entryPoint[0]);
    return lookupHolder;
}


/* Template used to generate the stub.  We generate a stub by allocating a block of 
   memory and copy the template over it and just update the specific fields that need 
   to be changed.
*/ 
DispatchStub dispatchInit;

void DispatchHolder::InitializeStatic()
{
    // Check that _expectedMT is aligned in the DispatchHolder
    C_ASSERT(((offsetof(DispatchHolder, _stub) + offsetof(DispatchStub,_expectedMT)) % sizeof(void*)) == 0);
    C_ASSERT((sizeof(DispatchHolder) % sizeof(void*)) == 0);

#ifndef STUB_LOGGING
    dispatchInit._entryPoint [0] = 0x81;
    dispatchInit._entryPoint [1] = 0x39;
    C_ASSERT(sizeof(dispatchInit._entryPoint) == 2);

    dispatchInit._expectedMT     = 0xcccccccc;
    dispatchInit.jmpOp1 [0]      = 0x0f;
    dispatchInit.jmpOp1 [1]      = 0x85;
    C_ASSERT(sizeof(dispatchInit.jmpOp1) == 2);

    dispatchInit._failDispl      = 0xcccccccc;
    dispatchInit.jmpOp2          = 0xe9;
    dispatchInit._implDispl      = 0xcccccccc;
#else //STUB_LOGGING
    dispatchInit._entryPoint [0] = 0xff;
    dispatchInit._entryPoint [1] = 0x05;
    C_ASSERT(sizeof(dispatchInit._entryPoint) == 2);

    dispatchInit.d_call          = &g_mono_call_counter;
    dispatchInit.cmpOp [0]       = 0x81;
    dispatchInit.cmpOp [1]       = 0x39;              
    C_ASSERT(sizeof(dispatchInit.cmpOp) == 2);

    dispatchInit._expectedMT     = 0xcccccccc;
    dispatchInit.jmpOp1 [0]      = 0x0f;
    dispatchInit.jmpOp1 [1]      = 0x84;
    C_ASSERT(sizeof(dispatchInit.jmpOp1) == 2);

    dispatchInit._implDispl      = 0xcccccccc;
    dispatchInit.fail [0]        = 0xff;
    dispatchInit.fail [1]        = 0x05;
    C_ASSERT(sizeof(dispatchInit.fail) == 2);

    dispatchInit.d_miss          = &g_mono_miss_counter;
    dispatchInit.jmpFail         = 0xe9;
    dispatchInit._failDispl      = 0xcccccccc;
#endif //STUB_LOGGING 
};

void  DispatchHolder::Initialize(const BYTE * implTarget, const BYTE * failTarget, size_t expectedMT,
                                 VirtualCallStubManager::ThisCallingConvention passThis)
{
    _stub = dispatchInit;

    //fill in the stub specific fields
    _stub._expectedMT  = (size_t) expectedMT;
    _stub._failDispl   = failTarget - ((const BYTE *) &_stub._failDispl + sizeof(DISPL));
    _stub._implDispl   = implTarget - ((const BYTE *) &_stub._implDispl + sizeof(DISPL));
}

DispatchHolder* DispatchHolder::FromStubAddr(const BYTE* stubAddr)
{ 
    LEAF_CONTRACT;
    DispatchHolder* dispatchHolder = (DispatchHolder*) ( stubAddr - offsetof(DispatchHolder, _stub) );
    //    _ASSERTE(dispatchHolder->_stub._entryPoint[0] == dispatchInit._entryPoint[0]);
    return dispatchHolder;
}

DispatchHolder* DispatchHolder::FromDispatchEntry(const BYTE* dispatchEntry)
{ 
    LEAF_CONTRACT;
    DispatchHolder* dispatchHolder = (DispatchHolder*) ( dispatchEntry - offsetof(DispatchHolder, _stub) - offsetof(DispatchStub, _entryPoint) );
    //    _ASSERTE(dispatchHolder->_stub._entryPoint[0] == dispatchInit._entryPoint[0]);
    return dispatchHolder;
}


/* Template used to generate the stub.  We generate a stub by allocating a block of 
   memory and copy the template over it and just update the specific fields that need 
   to be changed.
*/ 

ResolveStub resolveInit;

void ResolveHolder::InitializeStatic()
{
    //Check that _token is aligned in ResolveHolder
    C_ASSERT(((offsetof(ResolveHolder, _stub) + offsetof(ResolveStub, _token)) % sizeof(void*)) == 0);
    C_ASSERT((sizeof(ResolveHolder) % sizeof(void*)) == 0);

    resolveInit._failEntryPoint [0]    = 0x83;
    resolveInit._failEntryPoint [1]    = 0x2d;
    C_ASSERT(sizeof(resolveInit._failEntryPoint) == 2);

    resolveInit._pCounter              = (INT32 *) (size_t) 0xcccccccc;
    resolveInit.part0 [0]              = 0x01;
    resolveInit.part0 [1]              = 0x7c;
    C_ASSERT(sizeof(resolveInit.part0) == 2);

    resolveInit.toPatcher              = (offsetof(ResolveStub, patch) - (offsetof(ResolveStub, toPatcher) + 1)) & 0xFF;

    resolveInit._resolveEntryPoint [0] = 0x50;
    resolveInit._resolveEntryPoint [1] = 0x8b;
    resolveInit._resolveEntryPoint [2] = 0x01;
    resolveInit._resolveEntryPoint [3] = 0x52;
    resolveInit._resolveEntryPoint [4] = 0x8b;
    resolveInit._resolveEntryPoint [5] = 0xd0;
    C_ASSERT(sizeof(resolveInit._resolveEntryPoint) == 6);

    resolveInit.part1 [0]              = 0xc1;
    resolveInit.part1 [1]              = 0xe8;
    resolveInit.part1 [2]              = CALL_STUB_CACHE_NUM_BITS;
    resolveInit.part1 [3]              = 0x03;
    resolveInit.part1 [4]              = 0xc2;
    resolveInit.part1 [5]              = 0x35;
    C_ASSERT(sizeof(resolveInit.part1) == 6);

    resolveInit._hashedToken           = 0xcccccccc;
    resolveInit.part2 [0]              = 0x25;
    C_ASSERT(sizeof(resolveInit.part2) == 1);

    resolveInit.mask                   = (CALL_STUB_CACHE_MASK << LOG2_PTRSIZE);
    resolveInit.part3 [0]              = 0x8b;
    resolveInit.part3 [1]              = 0x80;;
    C_ASSERT(sizeof(resolveInit.part3) == 2);

    resolveInit._cacheAddress          = 0xcccccccc;
#ifdef STUB_LOGGING
    resolveInit.cntr1 [0]              = 0xff;
    resolveInit.cntr1 [1]              = 0x05;
    C_ASSERT(sizeof(resolveInit.cntr1) == 2);

    resolveInit.c_call                 = &g_poly_call_counter;
#endif //STUB_LOGGING 
    resolveInit.part4 [0]              = 0x3b;
    resolveInit.part4 [1]              = 0x10;
    C_ASSERT(sizeof(resolveInit.part4) == 2);

    // resolveInit.mtOffset               = offsetof(ResolveCacheElem,pMT) & 0xFF;
    C_ASSERT(offsetof(ResolveCacheElem,pMT) == 0);

    resolveInit.part5 [0]              = 0x75;
    C_ASSERT(sizeof(resolveInit.part5) == 1);

    resolveInit.toMiss1                = offsetof(ResolveStub,miss)-(offsetof(ResolveStub,toMiss1)+1);

    resolveInit.part6 [0]              = 0x81;
    resolveInit.part6 [1]              = 0x78;
    C_ASSERT(sizeof(resolveInit.part6) == 2);

    resolveInit.tokenOffset            = offsetof(ResolveCacheElem,token) & 0xFF;    

    resolveInit._token                 = 0xcccccccc;

    resolveInit.part7 [0]              = 0x75;
    C_ASSERT(sizeof(resolveInit.part7) == 1);

    resolveInit.part8 [0]              = 0x8b;
    resolveInit.part8 [1]              = 0x40;
    C_ASSERT(sizeof(resolveInit.part8) == 2);

    resolveInit.targetOffset           = offsetof(ResolveCacheElem,target) & 0xFF;

    resolveInit.toMiss2                = offsetof(ResolveStub,miss)-(offsetof(ResolveStub,toMiss2)+1);

    resolveInit.part9 [0]              = 0x5a;
    resolveInit.part9 [1]              = 0x83;
    resolveInit.part9 [2]              = 0xc4;
    resolveInit.part9 [3]              = 0x04;
    resolveInit.part9 [4]              = 0xff;
    resolveInit.part9 [5]              = 0xe0;
    C_ASSERT(sizeof(resolveInit.part9) == 6);

    resolveInit.miss [0]               = 0x5a;
//    resolveInit.miss [1]               = 0xb8;
//    resolveInit._hashedTokenMov        = 0xcccccccc;
    resolveInit._slowEntryPoint [0]    = 0x68;
    resolveInit._tokenPush             = 0xcccccccc;
#ifdef STUB_LOGGING
    resolveInit.cntr2 [0]              = 0xff;
    resolveInit.cntr2 [1]              = 0x05;
    resolveInit.c_miss                 = &g_poly_miss_counter;
#endif //STUB_LOGGING 
    resolveInit.part10 [0]             = 0xe9;
    resolveInit._resolveWorkerDispl    = 0xcccccccc;

    resolveInit.patch [0]              = 0xe8;
    resolveInit._backpatcherDispl      = 0xcccccccc;
    resolveInit.part11 [0]             = 0xeb;
    resolveInit.toResolveStub          = (offsetof(ResolveStub, _resolveEntryPoint) - (offsetof(ResolveStub, toResolveStub) + 1)) & 0xFF;
};

void  ResolveHolder::Initialize(const BYTE *resolveWorkerTarget, const BYTE* patcherTarget, 
                                size_t dispatchToken, UINT32 hashedToken,
                                void * cacheAddr, INT32 * counterAddr,
                                VirtualCallStubManager::ThisCallingConvention passThis)
{
    _stub = resolveInit;

    //fill in the stub specific fields
    _stub._pCounter           = counterAddr;
    _stub._hashedToken        = hashedToken << LOG2_PTRSIZE;
    _stub._cacheAddress       = (size_t) cacheAddr;
    _stub._token              = dispatchToken;
//    _stub._hashedTokenMov     = hashedToken;
    _stub._tokenPush          = dispatchToken;
    _stub._resolveWorkerDispl = resolveWorkerTarget - ((const BYTE *) &_stub._resolveWorkerDispl + sizeof(DISPL));
    _stub._backpatcherDispl   = patcherTarget       - ((const BYTE *) &_stub._backpatcherDispl   + sizeof(DISPL));
}

ResolveHolder* ResolveHolder::FromStubAddr(const BYTE* stubAddr)
{ 
    LEAF_CONTRACT;
    ResolveHolder* resolveHolder = (ResolveHolder*) ( stubAddr - offsetof(ResolveHolder, _stub));
    //    _ASSERTE(resolveHolder->_stub._resolveEntryPoint[0] == resolveInit._resolveEntryPoint[0]);
    return resolveHolder;
}

ResolveHolder* ResolveHolder::FromFailEntry(const BYTE* failEntry)
{ 
    LEAF_CONTRACT;
    ResolveHolder* resolveHolder = (ResolveHolder*) ( failEntry - offsetof(ResolveHolder, _stub) - offsetof(ResolveStub, _failEntryPoint) );
    //    _ASSERTE(resolveHolder->_stub._resolveEntryPoint[0] == resolveInit._resolveEntryPoint[0]);
    return resolveHolder;
}

ResolveHolder* ResolveHolder::FromResolveEntry(const BYTE* resolveEntry)
{ 
    LEAF_CONTRACT;
    ResolveHolder* resolveHolder = (ResolveHolder*) ( resolveEntry - offsetof(ResolveHolder, _stub) - offsetof(ResolveStub, _resolveEntryPoint) );
    //    _ASSERTE(resolveHolder->_stub._resolveEntryPoint[0] == resolveInit._resolveEntryPoint[0]);
    return resolveHolder;
}

#endif // DACCESS_COMPILE

VirtualCallStubManager::StubKind VirtualCallStubManager::predictStubKind(TADDR stubStartAddress)
{
#ifdef DACCESS_COMPILE

    return SK_BREAKPOINT;  // Dac always uses the slower lookup

#else

    StubKind stubKind = SK_UNKNOWN;

    EX_TRY
    {
        // If stubStartAddress is completely bogus, then this might AV,
        // so we protect it with SEH. An AV here is OK.
        AVInRuntimeImplOkayHolder AVOkay(TRUE);

        WORD firstWord = *((WORD*) stubStartAddress);

#ifndef STUB_LOGGING
        if (firstWord == 0x3981)
#else //STUB_LOGGING
        if (firstWord == 0x05ff)
#endif
        {
            stubKind = SK_DISPATCH;
        }
        else if (firstWord == 0x6850)
        {
            stubKind = SK_LOOKUP;
        }
        else if (firstWord == 0x8b50)
        {
            stubKind = SK_RESOLVE;
        }
        else
        {
            BYTE firstByte  = ((BYTE*) stubStartAddress)[0];
            BYTE secondByte = ((BYTE*) stubStartAddress)[1];

            if ((firstByte  == X86_INSTR_INT3) ||
                (secondByte == X86_INSTR_INT3))
            {
                stubKind = SK_BREAKPOINT;
            }
        }
    }
    EX_CATCH
    {
        stubKind = SK_UNKNOWN;
    }
    EX_END_CATCH(SwallowAllExceptions);        

    return stubKind;

#endif // DACCESS_COMPILE
}

#endif //DECLARE_DATA

#endif // _VIRTUAL_CALL_STUB_X86_H
