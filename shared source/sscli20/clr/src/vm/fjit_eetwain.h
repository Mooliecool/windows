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
#ifdef FJIT

#ifndef _FJIT_EETWAIN_H
#define _FJIT_EETWAIN_H

#include <daccess.h>
#include "eetwain.h"

#include "corjit.h"
#include "../fjit/ifjitcompiler.h"

class Fjit_EETwain : public ICodeManager
{
    VPTR_VTABLE_CLASS(Fjit_EETwain, ICodeManager)

public:

#ifndef DACCESS_COMPILE

/*
    Last chance for the runtime support to do fixups in the context
    before execution continues inside a filter, catch handler, or finally
*/
virtual void FixContext(
                ContextType     ctxType,
                EHContext      *ctx,
                LPVOID          methodInfoPtr,
                LPVOID          methodStart,
                DWORD           nestingLevel,
                OBJECTREF       thrownObject,
                CodeManState   *pState,
                size_t       ** ppShadowSP,             // OUT
                size_t       ** ppEndRegion);           // OUT


/*
    Gets the ambient stack pointer value at the given nesting level within
    the method.
*/
virtual
TADDR GetAmbientSP(EHContext      *ctx,
                   LPVOID          methodInfoPtr,
                   DWORD           nestingLevel,
                   ICodeInfo      *pCodeInfo,
                   CodeManState   *pState);

#endif // #ifndef DACCESS_COMPILE

/*
    Unwind the current stack frame, i.e. update the virtual register
    set in pContext. This will be similar to the state after the function
    returns back to caller (IP points to after the call, Frame and Stack
    pointer has been reset, callee-saved registers restored
    (if UpdateAllRegs), callee-UNsaved registers are trashed)
    Returns success of operation.
*/
virtual bool UnwindStackFrame(
                PREGDISPLAY     pContext,
                LPVOID          methodInfoPtr,
                ICodeInfo      *pCodeInfo,
                unsigned        flags,
                CodeManState   *pState,
                StackwalkCacheUnwindInfo  *); // unused for Fjit, look at EECodeManager for sample implementation

typedef void (*CREATETHUNK_CALLBACK)(IJitManager* jitMgr,
                                     LPVOID* pHijackLocation,
                                     ICodeInfo *pCodeInfo
                                     );

static void HijackHandlerReturns(PREGDISPLAY     ctx,
                                 LPVOID          methodInfoPtr,
                                 ICodeInfo      *pCodeInfo,
                                 IJitManager*    jitmgr,
                 CREATETHUNK_CALLBACK pCallBack
                                 );
/*
    Is the function currently at a "GC safe point" ?
    Can call EnumGcRefs() successfully
*/
virtual bool IsGcSafe(  PREGDISPLAY     pContext,
                LPVOID          methodInfoPtr,
                ICodeInfo      *pCodeInfo,
                unsigned        flags);


/*
    Enumerate all live object references in that function using
    the virtual register set. Same reference location cannot be enumerated
    multiple times (but all differenct references pointing to the same
    object have to be individually enumerated).
    Returns success of operation.
*/
virtual bool EnumGcRefs(PREGDISPLAY     pContext,
                LPVOID          methodInfoPtr,
                ICodeInfo      *pCodeInfo,
                unsigned        pcOffset,
                unsigned        flags,
                GCEnumCallback  pCallback,
                LPVOID          hCallBack);

/*
    Return the address of the local security object reference
    (if available).
*/
virtual OBJECTREF* GetAddrOfSecurityObject(CrawlFrame *pCF);

/*
    Returns "this" pointer if it is a non-static method AND
    the object is still alive.
    Returns NULL in all other cases.
*/
virtual OBJECTREF GetInstance(
                PREGDISPLAY     pContext,
                LPVOID          methodInfoPtr,
                ICodeInfo *     pCodeInfo,
                unsigned        relOffset);

/*
    Returns the extra argument passed to to shared generic code if it is still alive.
    Returns NULL in all other cases.
*/
virtual void * GetParamTypeArg(PREGDISPLAY     pContext,
                               LPVOID          methodInfoPtr,
                               ICodeInfo *     pCodeInfo,
                               unsigned        relOffset);

/*
    Returns the offset of the GuardStack cookie if it exists.
    Returns NULL if there is on cookie.
*/
virtual void * GetGSCookieAddr(PREGDISPLAY     pContext,
                               LPVOID          methodInfoPtr,
                               ICodeInfo     * pCodeInfo,
                               unsigned        relOffset,
                               CodeManState  * pState);

/*
  Returns true if the given IP is in the given method's prolog or an epilog.
*/

virtual bool IsInPrologOrEpilog(
                DWORD  relPCOffset,
                LPVOID methodInfoPtr,
                size_t* prologSize);

/*
  Returns true if the given IP is in the synchronized region of the method (valid for synchronized functions only)
*/
virtual bool IsInSynchronizedRegion(
                DWORD           relOffset,
                LPVOID          methodInfoPtr,
                unsigned        flags);

/*
  Returns the size of a given function.
*/
virtual size_t GetFunctionSize(
                LPVOID methodInfoPtr);

/*
  Returns the size of the frame (barring localloc)
*/
virtual unsigned int GetFrameSize(
                LPVOID methodInfoPtr);

#ifndef DACCESS_COMPILE

virtual const BYTE* GetFinallyReturnAddr(PREGDISPLAY pReg);
virtual BOOL LeaveFinally(void *methodInfoPtr,
                          unsigned offset,
                          PCONTEXT pCtx);
virtual BOOL IsInFilter(void *methodInfoPtr,
                        unsigned offset,
                        PCONTEXT pCtx,
                        DWORD nestingLevel);
virtual void LeaveCatch(void *methodInfoPtr,
                         unsigned offset,
                         PCONTEXT pCtx);


#endif // DACCESS_COMPILE

#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif
};

////////////////////// Methods for manipulating the internal managed frame

inline PVOID* getInternalFP( PVOID externalFP )
{
    LEAF_CONTRACT;

   PVOID* pFrameBase;
   pFrameBase = (PVOID*)(((size_t)externalFP)+prolog_bias);
   pFrameBase--;
   return pFrameBase;
}

inline PVOID getSavedSPForEHClause( PVOID* internalFP, unsigned nestingLevel )
{
    LEAF_CONTRACT;
   
    _ASSERTE( nestingLevel > 0 );
    return (PVOID)(size_t)((*PTR_DWORD((TADDR)((DWORD *)internalFP - (1 + nestingLevel)*JIT_SIZE_EH_SLOT))) & ~1);
}

inline void setSavedSPForEHClause( PVOID* internalFP, unsigned nestingLevel, PVOID newVal )
{
   _ASSERTE( nestingLevel > 0 );
   internalFP[(1 + nestingLevel)*JIT_SIZE_EH_SLOT] = newVal;
}

inline BOOL isFilterEHClause( PVOID* internalFP, unsigned nestingLevel )
{
    LEAF_CONTRACT;

    return (*PTR_DWORD((TADDR)((DWORD *)internalFP - (1 + nestingLevel)*JIT_SIZE_EH_SLOT))) & 1;
}

inline BOOL isLocalStorage( PVOID* internalFP  )
{
    LEAF_CONTRACT;

  return ( internalFP[JIT_GENERATED_LOCAL_LOCALLOC_OFFSET] ? TRUE : FALSE );
}

inline PVOID getSPForInternalFrame( PVOID* internalFP, PVOID externalFP, DWORD FrameSize  )
{
  return isLocalStorage(internalFP) ?
       internalFP[JIT_GENERATED_LOCAL_LOCALLOC_OFFSET] : (PVOID)((size_t)externalFP - FrameSize*sizeof(void*));
}

inline unsigned getEHClauseNestingDepth( PVOID* internalFP  )
{
    LEAF_CONTRACT;
    
    // This value is only valid for functions that have EH clauses
    return (unsigned) * PTR_DWORD((TADDR)(internalFP+JIT_GENERATED_LOCAL_NESTING_COUNTER));
}

inline void setEHClauseNestingDepth( PVOID* internalFP, unsigned val  )
{
    LEAF_CONTRACT;

  internalFP[JIT_GENERATED_LOCAL_NESTING_COUNTER] = (PVOID)(UINT_PTR)val;
}

inline OBJECTREF* GetAddrOfSecurityObjectInternal( PVOID * internalFP  )
{
  return (OBJECTREF*)(internalFP + offsetof(prolog_data, security_obj)/sizeof(void *) + 1);
}

inline OBJECTREF* GetInstanceInternal( PVOID* internalFP, BOOL retBuff )
{
  if ( retBuff && ReturnBufferFirst && EnregReturnBuffer )
    return (OBJECTREF*)(internalFP + 1 + (sizeof(prolog_data) + offsetOfRegister(1))/(sizeof(void*)));
  else
    return (OBJECTREF*)(internalFP + 1 + (sizeof(prolog_data) + offsetOfRegister(0))/(sizeof(void*)));
}

inline PVOID GetAddrOfSavedRegisterInternal( PVOID* internalFP  )
{
#if defined(_X86_) || defined(_PPC_)
  return internalFP + offsetof(prolog_data, callee_saved_esi)/sizeof(void *) + 1;
#else
  return 0;
#endif
}

inline PVOID GetReturnBufferInternal( PVOID* internalFP, BOOL thisPtr  )
{
  if ( EnregReturnBuffer )
  {
    if ( thisPtr && !ReturnBufferFirst)
      return internalFP + 1 + (sizeof(prolog_data) + offsetOfRegister(1))/(sizeof(void*));
    else
      return internalFP + 1 + (sizeof(prolog_data) + offsetOfRegister(0))/(sizeof(void*));
  }
  else
      return internalFP + 1 + (sizeof(prolog_data) + 64)/(sizeof(void*));
}


inline LPVOID GetCalleeSavedRegP(REGDISPLAY *display)
{
    LEAF_CONTRACT;

#if defined(_X86_)
   return (LPVOID)(size_t)(display->pEsi);
#elif defined(_PPC_)
   return (LPVOID)(size_t)display->pR[29-13];
#else
   _ASSERTE(!"NYI - GetCalleeSavedReg");
   return (LPVOID)0;
#endif
}


#endif
#endif // #ifdef FJIT

