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
#ifndef _VIRTUAL_CALL_STUB_PPC_H
#define _VIRTUAL_CALL_STUB_PPC_H

// ROTORTODO: Port this for PPC?

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

    BYTE    _entryPoint [1];        // 68		push
    size_t  _token;	            // xx xx xx xx               32-bit constant
#ifdef STUB_LOGGING
    BYTE cntr2[2];		// ff 05		inc
    size_t* c_lookup;	// xx xx xx xx		[call_lookup_counter]
#endif //STUB_LOGGING 
    BYTE part2 [1];	    // e9			jmp
    DISPL   _resolveWorkerDispl;    // xx xx xx xx               pc-rel displ
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

#ifndef STUB_LOGGING
    BYTE    _entryPoint [1];        // 3d               cmp     eax,
    size_t  _expectedMT;            // xx xx xx xx                  expectedMT
    BYTE jmpOp1[2];			// 0f 85		jne					
    DISPL   _failDispl;             // xx xx xx xx              failEntry         ;must be forward jmp for perf reasons
	BYTE jmpOp2;			// e9			jmp		
    DISPL   _implDispl;             // xx xx xx xx              implTarget
#else //STUB_LOGGING
    BYTE    _entryPoint [2];        // ff 05            inc             
	size_t* d_call;			// xx xx xx xx		[call_mono_counter]
	BYTE cmpOp [1];			// 3d			cmp	eax,	
    size_t  _expectedMT;            // xx xx xx xx                  expectedMT
    BYTE jmpOp1[2];			// 0f 84		je 
    DISPL   _implDispl;             // xx xx xx xx              implTarget        ;during logging, perf is not so important               
	BYTE fail [2];			// ff 05		inc	
	size_t* d_miss;			// xx xx xx xx		[miss_mono_counter]
	BYTE jmpFail;			// e9			jmp		
    DISPL   _failDispl;             // xx xx xx xx              failEntry 
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
    BYTE align[(sizeof(void*)-(offsetof(DispatchStub,_expectedMT)%sizeof(void*)))%sizeof(void*)];
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

    BYTE   _failEntryPoint [2];     // 83 2d		sub
    INT32* _pCounter;	            // xx xx xx xx		[counter],
    BYTE   part0 [2];		    // 01				   01
						// 7c			jl
	BYTE toPatcher;		// xx				backpatcher		;must be forward jump, for perf reasons
                                    // ;fallinto the resolver stub
    BYTE    _resolveEntryPoint[8];  // 52               push    edx         ;at entry eax = <this> mt
                                    // 0f a4 c2 14      shld    edx,eax,20  ;we are adding upper bits into lower bits of mt
                                    // 03 c2            add     eax,edx
                                    // 35               xor     eax, 
    UINT32  _hashedToken;           // xx xx xx xx	             hashedToken        ;along with pre-hashed token
    BYTE    part1 [3];              // 8b 11		mov     edx,[ecx]
						// 25			and eax,
	size_t mask;		// xx xx xx xx		cache_mask
	BYTE part2 [2];		// 8b 80		mov eax,[eax+
    size_t  _cacheAddress;          // xx xx xx xx		         lookupCache]
#ifdef STUB_LOGGING
	BYTE cntr1[2];		// ff 05		inc		
	size_t* c_call;		// xx xx xx xx		[call_cache_counter]
#endif //STUB_LOGGING 
	BYTE part3 [2];		// 3b 50		cmp edx,[eax+
	BYTE mtOffset;		// xx				ResolverCacheElem.pMT]
	BYTE part4 [1];		// 75			jne
	BYTE toMiss1;		// xx				miss	;must be forward jump, for perf reasons
	BYTE part5 [2];		// 81 78		cmp [eax+
	BYTE tokenOffset;	// xx				ResolverCacheElem.token],
    size_t  _token;		    // xx xx xx xx			                      token	
	BYTE part6 [1];		// 75			jne
	BYTE toMiss2;		// xx				miss	;must be forward jump, for perf reasons
	BYTE part7 [3]; 	// 5a			pop edx
                                    // ff 60		jmp     [eax+
    BYTE    targetOffset;           // xx			     ResolverCacheElem.target]
						//			miss:
    BYTE    miss [1];		    // 5a		pop     edx
    BYTE    _slowEntryPoint[1];
						// 68			push
    size_t  _tokenPush;              // xx xx xx xx		stubAddr
#ifdef STUB_LOGGING
	BYTE cntr2[2];		// ff 05		inc
	size_t* c_miss;		// xx xx xx xx		[miss_cache_counter]
#endif //STUB_LOGGING 
	BYTE part10 [1];    // e9			jmp
    DISPL   _resolveWorkerDispl;    // xx xx xx xx		resolveWorker
    
    BYTE  patch[1];	            // e8		call
    DISPL _backpatcherDispl;	    // xx xx xx xx		backpatcherWorker
    BYTE  part11 [1];		    // eb		jmp
	BYTE toResolveStub;	// xx 				resolveStub	
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

#ifdef STUB_LOGGING // This turns out to be zero-sized in non stub_logging case, and is an error. So remove
    BYTE pad[(sizeof(void*)-((sizeof(ResolveStub))%sizeof(void*))+offsetof(ResolveStub,_token))%sizeof(void*)];	//fill out DWORD
#endif
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

//@ROTORTODO: What do we do with these?

void TPResolveWorkerAsmStub(void)
{
    _ASSERTE(!"NYI");
}

void TransparentProxyWorkerAsmStub()
{
    _ASSERTE(!"NYI");
}

void BackPatchWorkerAsmStub(void)
{
    _ASSERTE(!"NYI");
}

void ResolveWorkerAsmStub(void)
{
    _ASSERTE(!"NYI");
}

void ResolveWorkerChainLookupAsmStub(void)
{
    _ASSERTE(!"NYI");
}

#endif // #ifndef DACCESS_COMPILE

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

    // ROTORTODO: Implement this
    _ASSERTE(false);
    return FALSE;
}

//----------------------------------------------------------------------------
BOOL VirtualCallStubManagerManager::TraceManager(
                    Thread *thread, TraceDestination *trace,
                    CONTEXT *pContext, BYTE **pRetAddr)
{
    WRAPPER_CONTRACT;

    // ROTORTODO: Implement this
    _ASSERTE(false);
    return FALSE;
}

#ifndef DACCESS_COMPILE

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

    lookupInit._entryPoint [0]     = 0x68;
    lookupInit._token              = 0xcccccccc;
#ifdef STUB_LOGGING
    lookupInit.cntr2 [0]           = 0xff;
    lookupInit.cntr2 [1]           = 0x05;
    lookupInit.c_lookup            = &g_call_lookup_counter;
#endif //STUB_LOGGING 
    lookupInit.part2 [0]           = 0xe9;
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
    dispatchInit._entryPoint [0] = 0x3d;
    dispatchInit._expectedMT     = 0xcccccccc;
    dispatchInit.jmpOp1 [0]      = 0x0f;
    dispatchInit.jmpOp1 [1]      = 0x85;
    dispatchInit._failDispl      = 0xcccccccc;
    dispatchInit.jmpOp2          = 0xe9;
    dispatchInit._implDispl      = 0xcccccccc;
#else //STUB_LOGGING
    dispatchInit._entryPoint [0] = 0xff;
    dispatchInit._entryPoint [1] = 0x05;
    dispatchInit.d_call          = &g_mono_call_counter;
    dispatchInit.cmpOp [0]       = 0x3d;
    dispatchInit._expectedMT     = 0xcccccccc;
    dispatchInit.jmpOp1 [0]      = 0x0f;
    dispatchInit.jmpOp1 [1]      = 0x84;
    dispatchInit._implDispl      = 0xcccccccc;
    dispatchInit.fail [0]        = 0xff;
    dispatchInit.fail [1]        = 0x05;
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
    // ROTORTODO: Implement
    _ASSERTE(false);
}

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
    // ROTORTODO: Implement this
    _ASSERTE(false);
    return SK_BREAKPOINT;
}

#endif //DECLARE_DATA

#endif // _VIRTUAL_CALL_STUB_PPC_H
