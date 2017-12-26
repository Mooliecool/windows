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
// This module contains the native methods for the delegate class
//
// Date: June 1998
////////////////////////////////////////////////////////////////////////////////

#ifndef _COMDELEGATE_H_
#define _COMDELEGATE_H_

class Stub;
class ShuffleThunkCache;

#include "cgensys.h"
#include "dllimportcallback.h"
#include "comvariant.h"
#include "mlcache.h"

typedef ArgBasedStubCache MulticastStubCache;


VOID GenerateShuffleArray(MethodDesc* pInvoke, MethodDesc *pTargetMeth, struct ShuffleEntry * pShuffleEntryArray, size_t nEntries);


// This class represents the native methods for the Delegate class
class COMDelegate
{
private:
    // friend VOID CPUSTUBLINKER::EmitMulticastInvoke(...);
    // friend VOID CPUSTUBLINKER::EmitShuffleThunk(...);
    friend class CPUSTUBLINKER;
    friend class DelegateInvokeStubManager;
    friend class SecureDelegateFrame;
    friend BOOL MulticastFrame::TraceFrame(Thread *thread, BOOL fromPatch, 
                                TraceDestination *trace, REGDISPLAY *regs);

    static MulticastStubCache* m_pSecureDelegateStubCache;
    static MulticastStubCache* m_pMulticastStubCache;
    static Stub* s_pInvokeStub;
    static Stub* s_pHasRetBuffInvokeStub;

    static MethodTable* s_pIAsyncResult;    // points to System.IAsyncResult's method table
    static MethodTable* s_pAsyncCallback;   // points to System.AsyncCallBack's method table

    static CrstStatic   s_DelegateToFPtrHashCrst;   // Lock for the following hash.
    static PtrHashMap*  s_pDelegateToFPtrHash;      // Hash table containing the Delegate->FPtr pairs
                                                    // passed out to unmanaged code.
public:
    static ShuffleThunkCache *m_pShuffleThunkCache;

    static void Init();

    static FCDECL3(void, DelegateConstruct, Object* refThis, Object* target, SLOT method);

    static FCDECL1(Object*, InternalAlloc, void* target);
    static FCDECL1(Object*, InternalAllocLike, Object* pThis);
    static FCDECL2(FC_BOOL_RET, InternalEqualTypes, Object* pThis, Object *pThat);
    
    static FCDECL3(void*, AdjustTarget, Object* refThis, Object* target, SLOT method);
    static FCDECL2(void*, GetCallStub, Object* refThis, SLOT method);

    static FCDECL5(FC_BOOL_RET, BindToMethodName, Object* refThisUNSAFE, Object* targetUNSAFE, void *methodType, StringObject* methodNameUNSAFE, int flags);

    static FCDECL5(FC_BOOL_RET, BindToMethodInfo, Object* refThisUNSAFE, Object* targetUNSAFE, MethodDesc *method, void *methodType, int flags);

    // This gets the MethodInfo for a delegate, creating it if necessary
    static FCDECL1(MethodDesc*, FindMethodHandle, Object* refThis);

    // Get the invoke method for the delegate. Used to transition delegates to multicast delegates.
    static FCDECL1(SLOT, GetMulticastInvoke, Object* refThis);
    static FCDECL1(MethodDesc*, GetInvokeMethod, Object* refThis);
    static SLOT GetSecureInvoke(MethodDesc* pMD);
    // determines whether the delegate needs to be wrapped
    static BOOL NeedsSecureDelegate(Assembly* pCreatorAssembly, AppDomain *pCreatorDomain, MethodDesc* pTargetMD);
    // on entry delegate points to the delegate to wrap
    static DELEGATEREF CreateSecureDelegate(DELEGATEREF delegate, Assembly* pCreatorAssembly, MethodDesc* pTargetMD);

    // Marshals a delegate to a unmanaged callback.
    static LPVOID ConvertToCallback(OBJECTREF pDelegate);

    // Marshals an unmanaged callback to Delegate
    static OBJECTREF ConvertToDelegate(LPVOID pCallback, MethodTable* pMT);

    // Returns the unmanaged callsite for a function pointer wrapped in a delegate
    static FCDECL1(void*, GetUnmanagedCallSite, Object* refThisUNSAFE);


    // Creates the ML marshaling stub for a delegate -> unmanaged call
    static Stub* ComputeDllImportStub(MethodDesc *pMD);

    static void ValidateDelegatePInvoke(MethodDesc* pMD);

    // Creates the runtime "DllImport" stub for a delegate -> unmanaged call
    static Stub* CreateGenericDllImportStubForDelegate(StubLinker *pstublinker, UINT numStackBytes, DelegateEEClass* pClass);

    static void RemoveEntryFromFPtrHash(UPTR key);
    
    // Decides if pcls derives from Delegate.
    static BOOL IsDelegate(MethodTable *pMT);

    // Decides if this is a secure delegate
    static BOOL IsSecureDelegate(DELEGATEREF dRef);
   
    // Get the cpu stub for a delegate invoke.
    static Stub *GetInvokeMethodStub(EEImplMethodDesc* pMD);

    // get the one single delegate invoke stub
    static Stub *TheDelegateInvokeStub(BOOL bHasReturnBuffer = FALSE);

    static void DoUnmanagedCodeAccessCheck(MethodDesc* pMeth);

    static MethodDesc * __fastcall GetMethodDesc(OBJECTREF obj);
    static OBJECTREF GetTargetObject(OBJECTREF obj);

    static BOOL IsTrueMulticastDelegate(OBJECTREF delegate);

private:

    static Stub* SetupShuffleThunk(DelegateEEClass *pDelCls, MethodDesc *pTargetMeth);
    static size_t GetShuffleArrayEntryCount(MethodDesc* pMD);

public:
    static MethodDesc* FindDelegateInvokeMethod(MethodTable *pMT);

    static BOOL IsMethodDescCompatible(TypeHandle   thFirstArg,
                                       TypeHandle   thExactMethodType,
                                       MethodDesc  *pTargetMethod,
                                       TypeHandle   thDelegate,
                                       MethodDesc  *pInvokeMethod,
                                       int          flags,
                                       BOOL        *pfIsOpenDelegate);
    static MethodDesc* GetDelegateCtor(TypeHandle delegateType, MethodDesc *pTargetMethod, DelegateCtorArgs *pCtorData);
    //@GENERICSVER: new (suitable for generics)
    // Method to do static validation of delegate .ctor
    static BOOL ValidateCtor(TypeHandle objHnd, TypeHandle ftnParentHnd, MethodDesc *pFtn, TypeHandle dlgtHnd);

private:
    static BOOL ValidateBeginInvoke(DelegateEEClass* pClass);   // make certain the BeginInvoke method is consistant with the Invoke Method
    static BOOL ValidateEndInvoke(DelegateEEClass* pClass);     // make certain the EndInvoke method is consistant with the Invoke Method

    static void BindToMethod(DELEGATEREF   *pRefThis,
                             OBJECTREF     *pRefFirstArg,
                             MethodDesc    *pTargetMethod,
                             MethodTable   *pExactMethodType,
                             BOOL           fIsOpenDelegate,
                             BOOL           fCheckSecurity);
};

// These flags effect the way BindToMethodInfo and BindToMethodName are allowed to bind a delegate to a target method. Their
// values must be kept in sync with the definition in bcl\system\delegate.cs.
enum DelegateBindingFlags
{
    DBF_StaticMethodOnly    =   0x00000001, // Can only bind to static target methods
    DBF_InstanceMethodOnly  =   0x00000002, // Can only bind to instance (including virtual) methods
    DBF_OpenDelegateOnly    =   0x00000004, // Only allow the creation of delegates open over the 1st argument
    DBF_ClosedDelegateOnly  =   0x00000008, // Only allow the creation of delegates closed over the 1st argument
    DBF_NeverCloseOverNull  =   0x00000010, // A null target will never been considered as a possible null 1st argument
    DBF_CaselessMatching    =   0x00000020, // Use case insensitive lookup for methods matched by name
    DBF_SkipSecurityChecks  =   0x00000040, // Skip security checks (visibility, link demand etc.)
    DBF_RelaxedSignature    =   0x00000080, // Allow relaxed signature matching (co/contra variance)
};

void DistributeEventReliably(OBJECTREF *pDelegate,
                             OBJECTREF *pDomain);

void DistributeUnhandledExceptionReliably(OBJECTREF *pDelegate,
                                          OBJECTREF *pDomain,
                                          OBJECTREF *pThrowable,
                                          BOOL       isTerminating);

// Want no unused bits in ShuffleEntry since unused bits can make
// equivalent ShuffleEntry arrays look unequivalent and deoptimize our
// hashing.
#include <pshpack1.h>

// To handle a call to a static delegate, we create an array of ShuffleEntry
// structures. Each entry instructs the shuffler to move a chunk of bytes.
// The size of the chunk is StackElemSize (typically a DWORD): long arguments
// have to be expressed as multiple ShuffleEntry's.
//
// The ShuffleEntry array serves two purposes:
//
//  1. A platform-indepedent blueprint for creating the platform-specific
//     shuffle thunk.
//  2. A hash key for finding the shared shuffle thunk for a particular
//     signature.
struct ShuffleEntry
{
    enum {
        REGMASK  = 0x8000,
        OFSMASK  = 0x7fff,
        SENTINEL = 0xffff,
    };

    // Special values:
    //  -1       - indicates end of shuffle array: stacksizedelta
    //             == difference in stack size between virtual and static sigs.
    //  high bit - indicates a register argument: mask it off and
    //             the result is an offset into ArgumentRegisters.

    union {
        UINT16    srcofs;
        CorElementType  argtype;    // AMD64: shuffle array is just types
    };
    union {
        UINT16    nStackAlignmentUnits; // IA64: shuffle array just has one element that contains 
                                        //       the number stack bytes used by the method
        UINT16    dstofs;           //if srcofs != SENTINEL
        UINT16    stacksizedelta;   //if dstofs == SENTINEL
        BOOL      isretbuffarg;     // AMD64: ret buff? (first arg only)
    };
};


#include <poppack.h>

void __stdcall DoDelegateInvokeForHostCheck(Object* pDelegate);

#endif  // _COMDELEGATE_H_
