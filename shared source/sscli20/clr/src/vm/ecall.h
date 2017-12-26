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
// ECALL.H -
//
// Handles our private native calling interface.
//


#ifndef _ECALL_H_
#define _ECALL_H_

#include "fcall.h"
#include "mlcache.h"
#include "corinfo.h"

class StubLinker;
class FCallMethodDesc;
class ArgBasedStubCache;
class MethodDesc;

enum  StubStyle;

class FCallMap;
typedef DPTR(class FCallMap) PTR_FCallMap;

#define FCALL_HASH_SIZE 512
typedef DPTR(struct ECFunc) PTR_ECFunc;

#ifdef DACCESS_COMPILE
GVAL_DECL(TADDR, gLowestFCall);
GVAL_DECL(TADDR, gHighestFCall);
GARY_DECL(PTR_ECFunc, gFCallMethods, FCALL_HASH_SIZE);
#endif

enum {
    FCFuncFlag_EndOfArray   = 0x01,
    FCFuncFlag_HasSignature = 0x02,
    FCFuncFlag_Unreferenced = 0x04, // Suppress unused fcall check
};

struct ECFunc {
    UINT_PTR            m_dwFlags;

#ifndef DACCESS_COMPILE
    LPVOID              m_pImplementation;
#else
    TADDR               m_pImplementation;
#endif
    PTR_MethodDesc      m_pMD;               // for reverse mapping

    PTR_ECFunc          m_pNext;             // linked list for hash table

    LPCUTF8             m_wszMethodName;
    LPHARDCODEDMETASIG  m_wszMethodSig;      // Optional field. It is valid only if HasSignature() is set.

    bool                IsEndOfArray()  { LEAF_CONTRACT; return !!(m_dwFlags & FCFuncFlag_EndOfArray); }
    bool                HasSignature()  { LEAF_CONTRACT; return !!(m_dwFlags & FCFuncFlag_HasSignature); }
    bool                IsUnreferenced(){ LEAF_CONTRACT; return !!(m_dwFlags & FCFuncFlag_Unreferenced); }
    CorInfoIntrinsics   IntrinsicID()   { LEAF_CONTRACT; return (CorInfoIntrinsics)((INT8)(m_dwFlags >> 16)); }
    int                 DynamicID()     { LEAF_CONTRACT; return (int)              ((INT8)(m_dwFlags >> 24)); }

    ECFunc*             NextInArray()
    { 
        LEAF_CONTRACT; 
        
        return (ECFunc*)((BYTE*)this + 
            (HasSignature() ? sizeof(ECFunc) : offsetof(ECFunc, m_wszMethodSig)));
    }
};

struct ECClass
{
    LPCUTF8     m_wszClassName;
    LPCUTF8     m_wszNameSpace;
    LPVOID*     m_pECFunc;
};

class ArrayStubCache : public MLStubCache
{
    virtual MLStubCompilationMode CompileMLStub(const BYTE *pRawMLStub,
                                                StubLinker *psl,
                                                void *callerContext);
    virtual UINT Length(const BYTE *pRawMLStub);
};


//=======================================================================
// Collects code and data pertaining to the ECall interface.
//=======================================================================
class ECall
{
    public:
        //---------------------------------------------------------
        // One-time init
        //---------------------------------------------------------
        static void Init();

        //---------------------------------------------------------
        // Handles missing fcall implementation
        //---------------------------------------------------------
        static void ReportMissingFCall(MethodDesc *pMD);

        //---------------------------------------------------------
        // Cache for array stubs
        //---------------------------------------------------------
        static ArrayStubCache *m_pArrayStubCache;


        static LPVOID GetFCallImpl(MethodDesc* pMDofCall, BOOL fStaticOnly = FALSE);
        static LPVOID GetStaticFCallImpl(MethodDesc* pMDofCall)
        {
            return GetFCallImpl(pMDofCall, TRUE);
        }
        static MethodDesc* MapTargetBackToMethod(TADDR pTarg);
        static DWORD GetIDForMethod(MethodDesc *pMD);
        static CorInfoIntrinsics GetIntrinsicID(MethodDesc *pMD);

        // Some fcalls (delegate ctors and tlbimpl ctors) shared one implementation.
        // We should never patch vtable for these since they have 1:N mapping between
        // MethodDesc and the actual implementation
        static BOOL IsSharedFCallImpl(TADDR pImpl);

        static BOOL CheckUnusedFCalls();

        static void DynamicallyAssignFCallImpl(LPVOID impl, DWORD index);

        static void PopulateDynamicallyAssignedFCalls();
        static void PopulateManagedStringConstructors();
#ifdef DACCESS_COMPILE
        // Enumerates all gFCallMethods for minidumps.
        static void EnumFCallMethods();
#endif // DACCESS_COMPILE

#define DYNAMICALLY_ASSIGNED_FCALLS() \
    DYNAMICALLY_ASSIGNED_FCALL_IMPL(FastAllocateString,                FramedAllocateString) \
    DYNAMICALLY_ASSIGNED_FCALL_IMPL(CtorCharArrayManaged,              NULL) \
    DYNAMICALLY_ASSIGNED_FCALL_IMPL(CtorCharArrayStartLengthManaged,   NULL) \
    DYNAMICALLY_ASSIGNED_FCALL_IMPL(CtorCharCountManaged,              NULL) \
    DYNAMICALLY_ASSIGNED_FCALL_IMPL(CtorCharPtrManaged,                NULL) \
    DYNAMICALLY_ASSIGNED_FCALL_IMPL(CtorCharPtrStartLengthManaged,     NULL) \
    DYNAMICALLY_ASSIGNED_FCALL_IMPL(InternalGetCurrentThread,          NULL) \

        enum
        {
            #undef DYNAMICALLY_ASSIGNED_FCALL_IMPL
            #define DYNAMICALLY_ASSIGNED_FCALL_IMPL(id,defaultimpl) id,

            DYNAMICALLY_ASSIGNED_FCALLS()

            NUM_DYNAMICALLY_ASSIGNED_FCALL_IMPLEMENTATIONS,
            InvalidDynamicFCallId = -1
        };

    private:
        static ECFunc* FindTarget(TADDR pTarg);
};


#endif // _ECALL_H_
