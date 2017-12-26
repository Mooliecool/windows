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
// CROSSDOMAINCALLS.CPP
// The CrossDomainCall class provides a fast path of execution for qualifying
// cross domain calls. Asynch calls, one way calls, calls on context bound objects
// etc dont qualify.

#include "common.h"
#include "crossdomaincalls.h"
#include "remoting.h"
#include "objectclone.h"
#include "dbginterface.h"
#include "stackprobe.h"
#include "virtualcallstub.h"
#include "typeparse.h"
#include "typestring.h"
#include "appdomain.inl"

PtrHashMap *CrossDomainTypeMap::s_crossDomainMTMap = NULL;
SimpleRWLock *CrossDomainTypeMap::s_MTMapLock = NULL;

// See explanation of flags in crossdomaincalls.h
RemotableMethodInfo::XADOptimizationType
RemotableMethodInfo::IsCrossAppDomainOptimizable(MethodDesc *pMeth, DWORD *pNumStackSlotsToCopy)
{
    WRAPPER_CONTRACT;

    // This method table might be representative, but that's OK for the kinds of analysis we're about to do.
    MethodTable *pMT = pMeth->GetMethodTable();

    _ASSERTE(pMT->HasRemotableMethodInfo());
    _ASSERTE(pMT->GetRemotableMethodInfo());

    if (pMT->IsContextful())
        return XAD_NOT_OPTIMIZABLE;

    DWORD flags;

    // If this method is generic then we can't used cached analysis data stored on the method table and keyed by slot -- the same
    // slot is shared by methods with very different characteristics (such as whether the return type is a GC ref etc.).
    if (pMeth->GetNumGenericMethodArgs() > 0)
    {
        flags = DoStaticAnalysis(pMeth);
    }
    else
    {
        RemotableMethodInfo *pRMI = &(pMT->GetRemotableMethodInfo()->GetRemotableMethodInfo()[pMeth->GetSlot()]);
        flags = pRMI->m_OptFlags;

        if (!(flags & XAD_FLAGS_INITIALIZED))
        {
            flags = DoStaticAnalysis(pMeth);
            pRMI->m_OptFlags = flags;
        }
    }

    *pNumStackSlotsToCopy = flags & XAD_ARG_COUNT_MASK;

    return (XADOptimizationType) (flags & XAD_FLAG_MASK);
}

// This method is not synchronized because the operation is idempotent
DWORD
RemotableMethodInfo::DoStaticAnalysis(MethodDesc *pMeth)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        THROWS;
        PRECONDITION(CheckPointer(pMeth));
    }
    CONTRACTL_END

    BOOL bCallArgsBlittable = TRUE;
    BOOL bRetArgBlittable = TRUE;
    BOOL bOptimizable = TRUE;
    BOOL bMethodIsVirtual = FALSE;
    BOOL bArgsHaveAFloat  = FALSE;

    DWORD numStackBytes = 0;
    DWORD numStackSlots = 0;
    DWORD returnTypeFlags = 0;

    if (pMeth->ContainsGenericVariables())
    {
        bOptimizable = FALSE;
    }
    else
    {
        MetaSig mSig(pMeth);
        SigPointer spRet;
        CorElementType retElem;

        IMDInternalImport *pMDImport = pMeth->GetModule()->GetMDImport();
        MDEnumHolder    ePm(pMDImport);         // For enumerating  params.
        mdParamDef      tkPm;                   // A param token.
        DWORD           dwFlags;                // Param flags.
        USHORT          usSeq;                  // Sequence of a parameter.

        if (pMeth->IsOneWay())
        {
            bOptimizable = FALSE;
            goto SetFlags;
        }

        if (pMeth->IsVirtual())
        {
            bMethodIsVirtual = TRUE;
        }

        numStackBytes = mSig.SizeOfFrameArgumentArray(pMeth->IsStatic());
        _ASSERTE(numStackBytes % sizeof(SIZE_T) == 0);
        numStackSlots = numStackBytes / sizeof(SIZE_T);

        if (numStackSlots > XAD_ARG_COUNT_MASK)
        {
            bOptimizable = FALSE;
            goto SetFlags;
        }

        // Check if there are any [Out] args. If there are, skip the fast path
        IfFailThrow(pMDImport->EnumInit(mdtParamDef, pMeth->GetMemberDef(), &ePm));

        // Enumerate through the params and check the flags.
        while (pMDImport->EnumNext(&ePm, &tkPm))
        {
            pMDImport->GetParamDefProps(tkPm, &usSeq, &dwFlags);
            if (usSeq == 0)     // Skip return type flags.
                continue;
            // If the param has Out attribute, do not use fast path for this method
            if (IsPdOut(dwFlags))
            {
                bOptimizable = FALSE;
                goto SetFlags;
            }
        }

        // We're getting SigPointer first because this way we can differentiate E_T_STRING and E_T_CLASS
        spRet = mSig.GetReturnProps();
        IfFailThrow(spRet.GetElemType(&retElem));
        if (retElem > ELEMENT_TYPE_PTR &&
            retElem != ELEMENT_TYPE_I &&
            retElem != ELEMENT_TYPE_U &&
            retElem != ELEMENT_TYPE_FNPTR)
        {
            bRetArgBlittable = FALSE;
        }

        // Now we can normalize the return type so as to get rid of any generic type variables and the like.
        retElem = mSig.GetReturnType();

        if (retElem == ELEMENT_TYPE_VALUETYPE)
        {
            // Make a note that we have a struct in the signature. This way we wont blit the contents
            // and end up in a situation where we have data, but the type isnt loaded yet
            bCallArgsBlittable = FALSE;

            // Do some further inspection
            TypeHandle retTypeHandle = mSig.GetRetTypeHandleThrowing();

                // Currently we don't handle the special unbox handling for ret values of Nullable<T> in MarshalAndCall
            if (Nullable::IsNullableType(retTypeHandle)) 
            {
                bOptimizable = FALSE;
            }

            retElem = retTypeHandle.GetInternalCorElementType();
            if ((retElem <= ELEMENT_TYPE_PTR || retElem == ELEMENT_TYPE_I || retElem == ELEMENT_TYPE_U) &&
                !retTypeHandle.AsMethodTable()->CannotBeBlittedByObjectCloner())
                bRetArgBlittable = TRUE;
        }

        // Check if the return type is a GC ref
        if (gElementTypeInfo[retElem].m_gc == TYPE_GC_REF)
        {
            returnTypeFlags = XAD_RET_GC_REF;
        }
        else
        {
            UINT fpRetSize  = mSig.GetFPReturnSize();
            UpdateFPReturnSizeForHFAReturn(&mSig, &fpRetSize);        // this is an nop on non-IA64 platforms
            returnTypeFlags = GetRetTypeFlagsFromFPReturnSize(fpRetSize);
        }

        while (mSig.PeekArg() != ELEMENT_TYPE_END)
        {
            CorElementType currType = mSig.PeekArg();
            mSig.NextArg();

            SigPointer sp = mSig.GetArgProps();
            CorElementType retTy;
            IfFailThrow(sp.GetElemType(&retTy));
            if (retTy > ELEMENT_TYPE_PTR &&
                retTy != ELEMENT_TYPE_VALUETYPE &&
                retTy != ELEMENT_TYPE_I &&
                retTy != ELEMENT_TYPE_U &&
                retTy != ELEMENT_TYPE_FNPTR)
            {
                bCallArgsBlittable = FALSE;
            }

                // Currently we don't handle the special unbox handling for Nullable<T> for byrefs in MarshalAndCall
            if (currType == ELEMENT_TYPE_BYREF) 
            {
                TypeHandle refType;
                if (mSig.GetByRefType(&refType) == ELEMENT_TYPE_VALUETYPE)
                    if (Nullable::IsNullableType(refType))  
                    {
                        bOptimizable = FALSE;
                    }
            }
            else if (currType == ELEMENT_TYPE_VALUETYPE)
            {
#if ENREGISTERED_PARAMTYPE_MAXSIZE
                // Since we also do implict ByRef in some cases, we also have to probit the optimization there too
                TypeHandle argType = mSig.GetLastTypeHandleThrowing();
                if (Nullable::IsNullableType(argType)) 
                {
                    if (MetaSig::IsArgPassedByRef(argType.GetSize()))
                        bOptimizable = FALSE;
                }
#endif
                bCallArgsBlittable = FALSE;
            }
            else if (currType == ELEMENT_TYPE_R4 || currType == ELEMENT_TYPE_R8)
            {
                bArgsHaveAFloat = TRUE;
            }
        }
    }

SetFlags:
    DWORD optimizationFlags = 0;
    if (!bOptimizable)
    {
        optimizationFlags |= XAD_NOT_OPTIMIZABLE;
    }
    else
    {
        optimizationFlags |= returnTypeFlags;

        if (bCallArgsBlittable)
        {
            optimizationFlags |= XAD_BLITTABLE_ARGS;
        }
        if (bRetArgBlittable)
        {
            optimizationFlags |= XAD_BLITTABLE_RET;
        }
        if (bMethodIsVirtual)
        {
            optimizationFlags |= XAD_METHOD_IS_VIRTUAL;
        }
        if (bArgsHaveAFloat)
        {
            optimizationFlags |= XAD_ARGS_HAVE_A_FLOAT;
        }
    }
    optimizationFlags |= numStackSlots;
    optimizationFlags |= XAD_FLAGS_INITIALIZED;

    return optimizationFlags;
}

DWORD RemotableMethodInfo::GetSizeOfRemotableMethodInfo(MethodTable *pMT)
{
    LEAF_CONTRACT;
    return sizeof(CrossDomainOptimizationInfo) + (sizeof(RemotableMethodInfo) * pMT->GetNumMethods());
}

BOOL RemotableMethodInfo::TypeIsConduciveToBlitting(MethodTable *pFromMT, MethodTable *pToMT)
{
    // Presence of GC fields or certain atributes, rules out blittability
    if (pFromMT->CannotBeBlittedByObjectCloner() ||
        pToMT->CannotBeBlittedByObjectCloner())
        return FALSE;

    // Shared types are okay to blit
    if (pFromMT == pToMT)
        return TRUE;

   if (pFromMT->IsEnum() && pToMT->IsEnum()
   && pFromMT->GetBaseSize() == pToMT->GetBaseSize())
        return TRUE;

   return FALSE;
}

BOOL CrossDomainTypeMap::CompareMTMapEntry (UPTR val1, UPTR val2)
{
    CONTRACTL {
        MODE_ANY;
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    CrossDomainTypeMap::MTMapEntry *entry1 = (CrossDomainTypeMap::MTMapEntry *)(val1 << 1);
    CrossDomainTypeMap::MTMapEntry *entry2 = (CrossDomainTypeMap::MTMapEntry *)val2;

    if (entry1->m_pFromMT == entry2->m_pFromMT &&
        entry1->m_dwFromDomain == entry2->m_dwFromDomain &&
        entry1->m_dwToDomain == entry2->m_dwToDomain)
        return TRUE;

    return FALSE;
}

CrossDomainTypeMap::MTMapEntry::MTMapEntry(AppDomain *pFromDomain, MethodTable *pFromMT, AppDomain *pToDomain, MethodTable *pToMT)
{
    WRAPPER_CONTRACT;

    m_dwFromDomain = pFromDomain->GetId();
    m_dwToDomain = pToDomain->GetId();
    m_pFromMT = pFromMT;
    m_pToMT = pToMT;
}

static BOOL IsOwnerOfRWLock(LPVOID lock)
{
    return TRUE;
}

/*static*/
PtrHashMap *CrossDomainTypeMap::GetTypeMap()
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        THROWS;
    }
    CONTRACTL_END

    if (s_MTMapLock == NULL)
    {
        void *tempLockSpace = SystemDomain::System()->GetLowFrequencyHeap()->AllocMem(sizeof(SimpleRWLock));
        SimpleRWLock *tempLock = new (tempLockSpace) SimpleRWLock(COOPERATIVE_OR_PREEMPTIVE, LOCK_TYPE_DEFAULT);

        if (FastInterlockCompareExchangePointer((void **) &s_MTMapLock,
                                                tempLock,
                                                NULL) != NULL)
        {
            // We lost the race
            SystemDomain::System()->GetLowFrequencyHeap()->BackoutMem(tempLockSpace, sizeof(SimpleRWLock));
        }
    }

    // Now we have a Crst we can use to synchronize the remainder of the init.
    if (s_crossDomainMTMap == NULL)
    {
        SimpleWriteLockHolder swlh(s_MTMapLock);

        if (s_crossDomainMTMap == NULL)
        {
            PtrHashMap *map = new (SystemDomain::System()->GetLowFrequencyHeap()) PtrHashMap ();
            LockOwner lock = {s_MTMapLock, IsOwnerOfRWLock};
            map->Init (32, CompareMTMapEntry, TRUE, &lock);
            s_crossDomainMTMap = map;
        }
    }

    return s_crossDomainMTMap;
}

MethodTable *
CrossDomainTypeMap::GetMethodTableForDomain(MethodTable *pMT, AppDomain *pFromDomain, AppDomain *pToDomain)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        THROWS;
    }
    CONTRACTL_END
    PtrHashMap *map = GetTypeMap();

    MTMapEntry admt(pFromDomain, pMT, pToDomain, NULL);

    SimpleReadLockHolder srlh(s_MTMapLock);
    MTMapEntry *pFound = (MTMapEntry *) map->LookupValue(admt.GetHash(), (LPVOID) &admt);
    if ((MTMapEntry *)INVALIDENTRY == pFound)
        return NULL;

    return pFound->m_pToMT;
}

void
CrossDomainTypeMap::SetMethodTableForDomain(MethodTable *pFromMT, AppDomain *pFromDomain, MethodTable *pToMT, AppDomain *pToDomain)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        THROWS;
    }
    CONTRACTL_END
    PtrHashMap *map = GetTypeMap();

    NewHolder<MTMapEntry> admt(new MTMapEntry(pFromDomain, pFromMT, pToDomain, pToMT));
    PREFIX_ASSUME(admt != NULL);

    SimpleWriteLockHolder swlh(s_MTMapLock);

    UPTR key = admt->GetHash();

    MTMapEntry *pFound = (MTMapEntry *) map->LookupValue(key, (LPVOID) admt);
    if ((MTMapEntry *)INVALIDENTRY == pFound)
    {
        map->InsertValue(key, (LPVOID) admt);
        admt.SuppressRelease();
    }
}

// Remove any entries in the table that refer to an appdomain that is no longer live.
void CrossDomainTypeMap::FlushStaleEntries()
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        NOTHROW;
    }
    CONTRACTL_END

    if (s_MTMapLock == NULL || s_crossDomainMTMap == NULL)
        return;

    SimpleWriteLockHolder swlh(s_MTMapLock);

    bool fDeletedEntry = false;
    PtrHashMap::PtrIterator iter = s_crossDomainMTMap->begin();
    while (!iter.end())
    {
        MTMapEntry *pEntry = (MTMapEntry *)iter.GetValue();
        AppDomainFromIDHolder adFrom(pEntry->m_dwFromDomain, TRUE);
        AppDomainFromIDHolder adTo(pEntry->m_dwFromDomain, TRUE);        
        if (adFrom.IsUnloaded() ||
            adTo.IsUnloaded())
        {
#ifdef _DEBUG 
            LPVOID pDeletedEntry =
#endif
                s_crossDomainMTMap->DeleteValue(pEntry->GetHash(), pEntry);
            _ASSERTE(pDeletedEntry == pEntry);
            delete pEntry;
            fDeletedEntry = true;
        }
        ++iter;
    }

    if (fDeletedEntry)
        s_crossDomainMTMap->Compact();
}


#if defined(_X86_) || defined(_WIN64) 

ARG_SLOT DispatchCallBody(
    SIZE_T *pSrc,
    DWORD numStackSlotsToCopy,
    SIZE_T *pRegisterArgs,
    UINT64 uRegTypeMap,
    LPVOID pvRetBuff,
    UINT64 cbRetBuff,
    UINT32 fpRetSize,
    BYTE *pTargetAddress
)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_COOPERATIVE;

    ARG_SLOT retVal = CallDescrWorkerWithHandler(
            pSrc
#ifndef CALLDESCR_BOTTOMUP 
            + numStackSlotsToCopy
#endif
            ,
            numStackSlotsToCopy,
#ifdef CALLDESCR_ARGREGS 
            (const ArgumentRegisters *) pRegisterArgs,
#endif
#ifdef CALLDESCR_REGTYPEMAP 
            uRegTypeMap,
#endif
#ifdef CALLDESCR_RETBUF 
            pvRetBuff,
            cbRetBuff,
#endif // CALLDESCR_RETBUF
            fpRetSize,
            (LPVOID) pTargetAddress,
            FALSE);

    return retVal;
}

// Wrapper so that debugger can put in an FS:0 handler
ARG_SLOT DispatchCallDebuggerWrapper(
    SIZE_T *pSrc,
    DWORD numStackSlotsToCopy,
    SIZE_T *pRegisterArgs,
    UINT64 uRegTypeMap,
    LPVOID pvRetBuff,
    UINT64 cbRetBuff,
    UINT32 fpRetSize,
    BYTE *pTargetAddress,
    ContextTransitionFrame* pFrame
)
{
    // Use static contracts b/c we have SEH.
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_COOPERATIVE;

    ARG_SLOT retVal = 0;

    PAL_TRY
    {
        retVal = DispatchCallBody(pSrc, numStackSlotsToCopy, pRegisterArgs, uRegTypeMap, pvRetBuff, cbRetBuff, fpRetSize, pTargetAddress);
    }
    PAL_EXCEPT_FILTER(NotifyOfCHFFilterWrapper, pFrame)
    {
        // Should never reach here b/c handler should always continue search.
        _ASSERTE(!"Unreachable");
    }
    PAL_ENDTRY

    return retVal;

}


// This method performs the proper profiler and debugger callbacks before dispatching the
// call. The caller has the responsibility of furnishing the target address, register and stack arguments.
// Stack arguments should be in reverse order, and pSrc should point to past the last argument
// Returns the return value or the exception object if one was thrown.
ARG_SLOT DispatchCall   (
                    SIZE_T *pSrc,
                    DWORD numStackSlotsToCopy,
                    SIZE_T *pRegisterArgs,
                    UINT64 uRegTypeMap,
                    LPVOID pvRetBuff,
                    UINT64 cbRetBuff,
                    UINT32 fpRetSize,
                    BYTE *pTargetAddress,
                    OBJECTREF *pRefException,
                    ContextTransitionFrame* pFrame)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    ARG_SLOT retVal = 0;

#ifdef DEBUGGING_SUPPORTED 
    // debugger goo What does this do? can someone put a comment here?
    if (CORDebuggerTraceCall())
        g_pDebugInterface->TraceCall(pTargetAddress);
#endif // DEBUGGING_SUPPORTED

    WIN64_ONLY(_ASSERTE(pRegisterArgs == NULL));

    EX_TRY
    {
        retVal = DispatchCallDebuggerWrapper(pSrc, numStackSlotsToCopy, pRegisterArgs, uRegTypeMap,
                                             pvRetBuff, cbRetBuff, fpRetSize, pTargetAddress, pFrame);
    }
    EX_CATCH
    {
        *pRefException = GETTHROWABLE();
    }
    EX_END_CATCH(RethrowTransientExceptions);

    return retVal;
}

ARG_SLOT DispatchCallNoEH   (
                    SIZE_T *pSrc,
                    DWORD numStackSlotsToCopy,
                    SIZE_T *pRegisterArgs,
                    UINT64 uRegTypeMap,
                    LPVOID pvRetBuff,
                    UINT64 cbRetBuff,
                    UINT32 fpRetSize,
                    BYTE *pTargetAddress)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    ARG_SLOT retVal = 0;

#ifdef DEBUGGING_SUPPORTED 
    // debugger goo What does this do? can someone put a comment here?
    if (CORDebuggerTraceCall())
        g_pDebugInterface->TraceCall(pTargetAddress);
#endif // DEBUGGING_SUPPORTED

    WIN64_ONLY(_ASSERTE(pRegisterArgs == NULL));

    retVal = DispatchCallDebuggerWrapper(pSrc, numStackSlotsToCopy, pRegisterArgs, uRegTypeMap,
                                         pvRetBuff, cbRetBuff, fpRetSize, pTargetAddress, NULL);

    return retVal;
}


// Before a cross appdomain call, we read the principal on the thread and set it aside, so that it can
// be restored when the call returns.
// In addition, we let the principal flow thru to the called appdomain, if the principal is serializable
OBJECTREF CrossDomainChannel::ReadPrincipal()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        THROWS;
    }
    CONTRACTL_END

    FieldDesc *pPrincipal  = g_Mscorlib.FetchField(FIELD__CALL_CONTEXT_SECURITY_DATA__PRINCIPAL);
    _ASSERTE(pPrincipal);

    FieldDesc *pSecurityData = g_Mscorlib.FetchField(FIELD__LOGICAL_CALL_CONTEXT__SECURITY_DATA);
    _ASSERTE(pSecurityData);

    THREADBASEREF ref = (THREADBASEREF) GetThread()->GetExposedObjectRaw();
    _ASSERTE(ref != NULL);

    EXECUTIONCONTEXTREF refExecCtx = (EXECUTIONCONTEXTREF )ref->GetExecutionContext();
    if (refExecCtx == NULL)
        return NULL;

    OBJECTREF refCallContext = refExecCtx->GetLogicalCallContext();
    if (refCallContext == NULL)
        return NULL;

    OBJECTREF refSecurityData = pSecurityData->GetRefValue(refCallContext);
    if (refSecurityData == NULL)
        return NULL;

    OBJECTREF refPrincipal = pPrincipal->GetRefValue(refSecurityData);
    if (refPrincipal != NULL)
    {
        MethodTable *pPrincipalType = refPrincipal->GetMethodTable();
        if (!pPrincipalType->IsSerializable())
        {
            pPrincipal->SetRefValue(refSecurityData, NULL);
        }
    }

    return refPrincipal;
}

// Principal never flows from called appdomain back to caller domain.
VOID CrossDomainChannel::ResetPrincipal()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        THROWS;
    }
    CONTRACTL_END

    FieldDesc *pPrincipal  = g_Mscorlib.FetchField(FIELD__CALL_CONTEXT_SECURITY_DATA__PRINCIPAL);
    _ASSERTE(pPrincipal);

    FieldDesc *pSecurityData = g_Mscorlib.FetchField(FIELD__LOGICAL_CALL_CONTEXT__SECURITY_DATA);
    _ASSERTE(pSecurityData);

    THREADBASEREF ref = (THREADBASEREF) GetThread()->GetExposedObjectRaw();
    _ASSERTE(ref != NULL);

    EXECUTIONCONTEXTREF refExecCtx = (EXECUTIONCONTEXTREF )ref->GetExecutionContext();
    if (refExecCtx == NULL)
        return;

    OBJECTREF refCallContext = refExecCtx->GetLogicalCallContext();
    if (refCallContext == NULL)
        return;

    OBJECTREF refSecurityData = pSecurityData->GetRefValue(refCallContext);
    if (refSecurityData == NULL)
        return;

    pPrincipal->SetRefValue(refSecurityData, NULL);

}

// At the end of a cross-appdomain call, we restore whatever principal was on the thread at the beginning of the call
VOID CrossDomainChannel::RestorePrincipal(OBJECTREF *prefPrincipal)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        THROWS;
    }
    CONTRACTL_END

    FieldDesc *pPrincipal  = g_Mscorlib.FetchField(FIELD__CALL_CONTEXT_SECURITY_DATA__PRINCIPAL);
    _ASSERTE(pPrincipal);

    FieldDesc *pSecurityData = g_Mscorlib.FetchField(FIELD__LOGICAL_CALL_CONTEXT__SECURITY_DATA);
    _ASSERTE(pSecurityData);

    THREADBASEREF ref = (THREADBASEREF) GetThread()->GetExposedObjectRaw();
    if (ref == NULL)
        return;

    EXECUTIONCONTEXTREF refExecCtx = (EXECUTIONCONTEXTREF )ref->GetExecutionContext();
    _ASSERTE(*prefPrincipal == NULL || refExecCtx != NULL);

    if (refExecCtx == NULL)
        return;

    OBJECTREF refCallContext = refExecCtx->GetLogicalCallContext();
    if (refCallContext == NULL)
        return;

    OBJECTREF refSecurityData = pSecurityData->GetRefValue(refCallContext);
    _ASSERTE(*prefPrincipal == NULL || refSecurityData != NULL);

    if (refSecurityData == NULL)
        return;

    pPrincipal->SetRefValue(refSecurityData, *prefPrincipal);

}

// This method mimics the Lease renewal mechanism of the managed CrossDomainChannel
// The lease object for the server can be accessed via its ServerIdentity.
VOID CrossDomainChannel::RenewLease()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        THROWS;
    }
    CONTRACTL_END
    // Check if lease needs to be renewed
    OBJECTREF refSrvIdentity = ObjectFromHandle(m_refSrvIdentity);
    if (refSrvIdentity == NULL)
        return;

    OBJECTREF refLease = ObjectToOBJECTREF((Object *)refSrvIdentity->GetPtrOffset(CRemotingServices::GetOffsetOfLeaseInIdentity()));
    if (refLease != NULL)
    {
        GCPROTECT_BEGIN(refLease);
        MethodDesc *pLeaseMeth = CRemotingServices::MDofRenewLeaseOnCall();
        BYTE *pCode = (BYTE *)pLeaseMeth->GetCallTarget(&refLease);
        OBJECTREF refException = NULL;

#if !defined(_X86_) && !defined(_WIN64) 
        PORTABILITY_ASSERT("CrossDomainChannel::RenewLease() - NYI on this platform");
#endif

        LPVOID  args[2];
        SIZE_T* pRegArgs   = NULL;
        SIZE_T* pStackArgs = NULL;
        DWORD   numStackSlotsToCopy = 0;
#if defined(CALLDESCR_ARGREGS) 
        args[0] = NULL;                                 // no more args for this method
        args[1] = (LPVOID)OBJECTREFToObject(refLease);  // arg 0

        pRegArgs   = (SIZE_T *)(&args[0]);
#else // !defined(CALLDESCR_ARGREGS)
        args[0] = (LPVOID)OBJECTREFToObject(refLease);  // arg 0
        args[1] = NULL;                                 // no more args for this method

        pStackArgs = (SIZE_T *)(&args[0]);
        numStackSlotsToCopy = 1;
#endif // !defined(CALLDESCR_ARGREGS)

        DispatchCall(pStackArgs, numStackSlotsToCopy, pRegArgs, 0, NULL, 0, 0, pCode, &refException);
        if (refException != NULL)
        {
            COMPlusThrow(refException);
        }
        GCPROTECT_END();
    }
}

// Given a client side instantiated method desc and a server side generic definition method desc extract the instantiation,
// translate all the types involved into the server domain and return the fully instantiated server method desc. Note that the
// client and server method descs might not represent the same type -- the client method might be from an interface, whereas the
// server method will always be on the real class.
MethodDesc *InstantiateMethod(MethodDesc *pClientMD, MethodDesc *pServerMD, MethodTable *pServerMT)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        THROWS;
        PRECONDITION(CheckPointer(pClientMD));
        PRECONDITION(pClientMD->HasMethodInstantiation());
        PRECONDITION(CheckPointer(pServerMD));
        PRECONDITION(pServerMD->HasMethodInstantiation());
        PRECONDITION(pClientMD->GetNumGenericMethodArgs() == pServerMD->GetNumGenericMethodArgs());
    }
    CONTRACTL_END;

    TypeHandle *pClientInst = pClientMD->GetMethodInstantiation();

    DWORD dwAllocaSize;
    if (!ClrSafeInt<DWORD>::multiply(pServerMD->GetNumGenericMethodArgs(), sizeof(TypeHandle), dwAllocaSize))
        COMPlusThrowHR(COR_E_OVERFLOW);

    CQuickBytes qbServerInst;
    TypeHandle *pServerInst = reinterpret_cast<TypeHandle*>(qbServerInst.AllocThrows(dwAllocaSize));

    for (DWORD dwArgNum = 0; dwArgNum < pClientMD->GetNumGenericMethodArgs(); dwArgNum++)
    {
        SString thName;
        TypeString::AppendType(thName, pClientInst[dwArgNum], TypeString::FormatNamespace|TypeString::FormatFullInst|TypeString::FormatAssembly);

        pServerInst[dwArgNum] = TypeName::GetTypeFromAsmQualifiedName(thName.GetUnicode(), pClientMD->IsIntrospectionOnly());

        _ASSERTE(!pServerInst[dwArgNum].IsNull());

        // Check that the type is actually visible on the server side. This prevents a malicious client from luring a trusted server
        // into manipulating types that would be normally invisible to it.
        if (!pServerInst[dwArgNum].IsExternallyVisible())
            COMPlusThrow(kRemotingException, L"Remoting_NonPublicOrStaticCantBeCalledRemotely");
    }

    // Find or create the method will the full instantiation.
    return MethodDesc::FindOrCreateAssociatedMethodDesc(pServerMD,
                                                        pServerMT,
                                                        FALSE,
                                                        pServerMD->GetNumGenericMethodArgs(),
                                                        pServerInst,
                                                        FALSE);
}

BOOL CrossDomainChannel::GetGenericMethodAddress(MethodTable *pServerType)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        THROWS;
    }
    CONTRACTL_END;

    m_pSrvMD = InstantiateMethod(m_pCliMD, pServerType->GetUnknownMethodDescForSlot(m_pCliMD->GetSlot()), pServerType);

    OBJECTREF thisObj = GetServerObject();
    m_pTargetAddress = (BYTE *)m_pSrvMD->GetCallTarget(&thisObj);

    return TRUE;
}

// We use this method to find the target address when we are convinced that the most derived type the proxy is cast
// to on the client side is equivalent to the corresponding type on the server side, in the sense the method table
// layouts are similar. This fact can be used to look up method addresses faster
BOOL CrossDomainChannel::GetTargetAddressFast(DWORD optFlags, MethodTable *pSrvMT, BOOL bFindServerMD)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        THROWS;
    }
    CONTRACTL_END
    _ASSERTE(m_pCliMD);
    _ASSERTE(m_pSrvDomain == SystemDomain::GetCurrentDomain()->GetId());

    MethodTable *pCliMT = m_pCliMD->GetMethodTable();
    _ASSERTE(!pCliMT->IsInterface());

    m_pSrvMD = NULL;

    DWORD dwMethodSlot = m_pCliMD->GetSlot();
    if (!RemotableMethodInfo::IsMethodVirtual(m_xret))
    {
        // This is a non-virtual method. Find the matching MT on the
        // server side, dereference the slot and get the method address

        MethodTable *pSrvSideMT = pSrvMT;

        // We now need to walk the server type hierarchy till we find the type that
        // declared the method we're going to call

        // First find how far is the type declaring the called method, from System.Object
        DWORD cliDepth = 0;
        MethodTable *pCurrLevel = pCliMT;
        while (pCurrLevel != NULL)
        {
            _ASSERTE(pCurrLevel);
            pCurrLevel = pCurrLevel->GetParentMethodTable();
            cliDepth++;
        };

        // Next find how deep is the server type from System.Object
        DWORD srvDepth = 0;
        pCurrLevel = pSrvMT;
        while (pCurrLevel != NULL)
        {
            _ASSERTE(pCurrLevel);
            pCurrLevel = pCurrLevel->GetParentMethodTable();
            srvDepth++;
        };

        _ASSERTE(srvDepth >= cliDepth);

        while (srvDepth > cliDepth)
        {
            _ASSERTE(pSrvSideMT);
            _ASSERTE(srvDepth != 0);
            pSrvSideMT = pSrvSideMT->GetParentMethodTable();
            srvDepth--;
        };

        if (m_pCliMD->HasMethodInstantiation())
            GetGenericMethodAddress(pSrvSideMT);
        else
        {
            m_pTargetAddress = (BYTE *)pSrvSideMT->GetSlot(dwMethodSlot);

#ifndef _DEBUG 
        if (bFindServerMD)
#endif
            m_pSrvMD = pSrvSideMT->GetUnknownMethodDescForSlot(dwMethodSlot);
        }
    }
    else
    {
        if (m_pCliMD->HasMethodInstantiation())
            GetGenericMethodAddress(pSrvMT);
        else
        {
            m_pTargetAddress = (BYTE *)pSrvMT->GetSlot(dwMethodSlot);

#ifndef _DEBUG 
            if (bFindServerMD)
#endif
            m_pSrvMD = pSrvMT->GetUnknownMethodDescForSlot(dwMethodSlot);
        }
    }

    _ASSERTE(m_pTargetAddress);
#ifdef _DEBUG 
    _ASSERTE(!strcmp(m_pSrvMD->GetName(), m_pCliMD->GetName()));
    DefineFullyQualifiedNameForClass();
    LPCUTF8 szSrvTypeName = GetFullyQualifiedNameForClassNestedAware(pSrvMT->GetClass());
    LPCUTF8 pszMethodName = m_pCliMD->GetName();
    LOG((LF_REMOTING, LL_INFO100, "GetTargetAddressFast. Address of %s::%s is 0x%x\n", &szSrvTypeName[0], pszMethodName, m_pTargetAddress));
#endif // _DEBUG
    return TRUE;
}

BOOL
CrossDomainChannel::GetInterfaceMethodAddressFast(DWORD optFlags, MethodTable *pSrvMT, BOOL bFindServerMD)
{
    CONTRACTL
    {
        THROWS;
        MODE_COOPERATIVE;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    _ASSERTE(m_pCliMD);

    MethodTable *pCliItfMT = m_pCliMD->GetMethodTable();
    _ASSERTE(pCliItfMT->IsInterface());

    // Only use the fast path if the interface is shared. If the interface isnt shared, then we'll have to search the
    // interface map on server type using name as key and then deref the slot # etc. I think shared interfaces will
    // be the common pattern. If not they should be.
    // Note that it's not enough to check that the client interface is shared, it must also be loaded in the server appdomain (since
    // it's now possible to have more than one instance of a shared assembly in a process).
    _ASSERTE(pCliItfMT->IsDomainNeutral());
    AppDomain* ad = SystemDomain::GetAppDomainFromId(m_pSrvDomain,ADV_RUNNINGIN);
    if (ad->FindDomainAssembly(pCliItfMT->GetAssembly()) == NULL)
        return FALSE;

    m_pSrvMD = NULL;

    OBJECTREF thisObj = GetServerObject();


    GCPROTECT_BEGIN(thisObj);

    DispatchSlot impl(pSrvMT->FindDispatchSlotForInterfaceMD(m_pCliMD));
    CONSISTENCY_CHECK(!impl.IsNull());
    m_pSrvMD = impl.GetMethodDesc();

    _ASSERTE(m_pSrvMD);

    // If the method called has a generic instantiation then the server method desc we've just received doesn't contain that
    // information (generic method slots are filled with generic definition method descs). We need to build the exact method desc by
    // copying the instantiation from the client method (translating each type into the new domain of course).
    if (m_pSrvMD->HasMethodInstantiation())
        m_pSrvMD = InstantiateMethod(m_pCliMD, m_pSrvMD, pSrvMT);

    m_pTargetAddress = (BYTE *)m_pSrvMD->GetCallTarget(&thisObj);

    GCPROTECT_END();

    return TRUE;
}


// Here we check whether the remote call is a cross domain call, if so, does it qualify
BOOL
CrossDomainChannel::CheckCrossDomainCall(TPMethodFrame *pFrame, ARG_SLOT *pReturn)
{
    CONTRACTL
    {
        THROWS;
        MODE_COOPERATIVE;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    BOOL bResult = FALSE;
    RealProxyObject *pRP = NULL;
    DWORD domainID;

    INSTALL_UNWIND_AND_CONTINUE_HANDLER;

    OBJECTREF refTP = pFrame->GetThis();
    OBJECTREF refRP = CTPMethodTable::GetRP(refTP);

    // Check if this is an async delegate call
    m_pFrame = pFrame;
    m_pCliMD = m_pFrame->GetFunction();
    MethodTable *pCliMT = m_pCliMD->GetMethodTable();
    // Only use the fast path if the interface is shared. If the interface isnt shared, then we'll have to search the
    // interface map on server type using name as key and then deref the slot # etc. I think shared interfaces will
    // be the common pattern.
    if (pCliMT->IsInterface() && !pCliMT->IsDomainNeutral())
        goto lDone;

    if (m_pCliMD->GetMethodTable()->IsAnyDelegateClass())
        goto lDone;

    // Check if this is a x-domain call
    pRP = (RealProxyObject *)OBJECTREFToObject(refRP);
    domainID = pRP->GetDomainID();
    if (domainID == 0)
        goto lDone;   // Not x-appdomain call, or proxy not initted for optimization

    // Check if we are in a context different from default. If so, we may need to run context
    // policies etc. Use regular path.
    if (GetThread()->GetContext() != SystemDomain::GetCurrentDomain()->GetDefaultContext())
        goto lDone;


    // Check if method call args can be blitted (or not optimizable at all)
    m_xret = RemotableMethodInfo::IsCrossAppDomainOptimizable(m_pCliMD, &m_numStackSlotsToCopy);
    if (RemotableMethodInfo::IsCallNotOptimizable(m_xret))
    {
#ifdef _DEBUG 
        DefineFullyQualifiedNameForClass();
        LPCUTF8 szSrvTypeName = GetFullyQualifiedNameForClassNestedAware(m_pCliMD->GetMethodTable()->GetClass());
        LOG((LF_REMOTING, LL_INFO100, "CheckCrossDomainCall. Call to %s::%s is not optimizable\n",
            &szSrvTypeName[0], m_pCliMD->GetName()));
#endif // _DEBUG
        goto lDone;
    }

    // Check if the domain has been unloaded
    m_pCliDomain = SystemDomain::GetCurrentDomain();
    m_pSrvDomain = ADID(domainID);



    bResult = ExecuteCrossDomainCall(pReturn);

lDone:  ;

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;

    return bResult;
}

// Dereference the server identity handle, to reach the server object
// If the handle is null, someone either called Disconnect on the server
// or the server's lease ran out
OBJECTREF CrossDomainChannel::GetServerObject()
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END

    _ASSERTE(m_pSrvDomain == SystemDomain::GetCurrentDomain()->GetId());

    OBJECTREF refSrvIdentity = ObjectFromHandle(m_refSrvIdentity);
    if (refSrvIdentity == NULL)
    {
        OBJECTREF refTP = NULL;
        {
            // On AMD64 we have to walk the signature to find out the location of the "this" object.
            // Since the type of the "this" object may not have been loaded in the server domain, we
            // may trigger a type load here which will lead to an assertion.  So we explicity tell the
            // loader not to load anything here.
            AMD64_ONLY(ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE());
            refTP = m_pFrame->GetThis();
        }
        OBJECTREF refRP = CTPMethodTable::GetRP(refTP);
        OBJECTREF refIdentity = ObjectToOBJECTREF((Object *)refRP->GetPtrOffset(CRemotingServices::GetOffsetOfCliIdentityInRP()));
        STRINGREF refURI = (STRINGREF)ObjectToOBJECTREF((Object *)refIdentity->GetPtrOffset(CRemotingServices::GetOffsetOfURIInIdentity()));
        SString sURI;
        refURI->GetSString(sURI);

        COMPlusThrow(kRemotingException,
            IDS_REMOTING_SERVER_DISCONNECTED,
            sURI.GetUnicode());
    }
    OBJECTREF srvObject = ObjectToOBJECTREF((Object *)refSrvIdentity->GetPtrOffset(CRemotingServices::GetOffsetOfTPOrObjInIdentity()));
    return srvObject;
}


void ProfilerCallRemotingClientInvocationStarted()
{
    PROFILER_CALL;
    g_profControlBlock.pProfInterface->RemotingClientInvocationStarted(
                reinterpret_cast<ThreadID>(GetThread()));
}

void ProfilerCallRemotingClientInvocationFinished()
{
    PROFILER_CALL;
    g_profControlBlock.pProfInterface->RemotingClientInvocationFinished(
                reinterpret_cast<ThreadID>(GetThread()));
}


// Here the we find the target method address, make a decision whether to
// execute the call locally, if remote whether to blit the arguments or to marshal them,
BOOL CrossDomainChannel::ExecuteCrossDomainCall(ARG_SLOT * pReturn)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_COOPERATIVE;

    BOOL bOptimizable = TRUE;

    EX_TRY_FOR_FINALLY
    {
#ifdef PROFILING_SUPPORTED 
        // If profiling is active, notify it that remoting stuff is kicking in
        if (CORProfilerTrackRemoting())
        {
            ProfilerCallRemotingClientInvocationStarted();
        }
#endif // PROFILING_SUPPORTED

        // Short circuit calls to Object::GetType and run them locally
        if (m_pCliMD == CRemotingServices::MDofObjectGetType())
        {
            LOG((LF_REMOTING, LL_INFO100, "ExecuteCrossDomainCall. Short circuiting call to Object::GetType and running it locally.\n"));
            OBJECTREF refTP = m_pFrame->GetThis();
            OBJECTREF refType = CRemotingServices::GetClass(refTP);
            *pReturn = ObjToArgSlot(refType);
        }
        else if (RemotableMethodInfo::AreArgsBlittable(m_xret))
        {
            bOptimizable = BlitAndCall(pReturn);
        }
        else
        {
            bOptimizable = MarshalAndCall(pReturn);
        }
    }
    EX_FINALLY
    {
#ifdef PROFILING_SUPPORTED 
        // If profiling is active, tell profiler we've made the call, received the
        // return value, done any processing necessary, and now remoting is done.
        if (CORProfilerTrackRemoting())
        {
            ProfilerCallRemotingClientInvocationFinished();
        }
#endif // PROFILING_SUPPORTED
    }
    EX_END_FINALLY

    if (!bOptimizable)
        goto Exit;

    // Check for the need to trip thread
    if (GetThread()->CatchAtSafePoint())
    {
        // There is no need to GC protect the return object as
        // TPFrame is GC protecting it
        CommonTripThread();
    }

#if defined(_X86_)
    m_pFrame->SetFunction((void *)(size_t)(m_numStackSlotsToCopy * sizeof(SIZE_T)));
#endif // defined(_X86_)

Exit:
    return bOptimizable;
}

BOOL
CrossDomainChannel::InitServerInfo()
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END

    _ASSERTE(m_pFrame);
    _ASSERTE(m_pSrvDomain == SystemDomain::GetCurrentDomain()->GetId());

    // Get the server object
    OBJECTREF refTP = NULL;
    {
        // See comment in CrossDomainChannel::GetServerObject().
        AMD64_ONLY(ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE());
        refTP = m_pFrame->GetThis();
    }
    OBJECTREF refRP = CTPMethodTable::GetRP(refTP);
    m_refSrvIdentity = (OBJECTHANDLE)refRP->GetPtrOffset(CRemotingServices::GetOffsetOfSrvIdentityInRP());
    OBJECTREF srvObject = GetServerObject();

    MethodTable *pSrvMT = srvObject->GetMethodTable();

    // Find the target address
    RealProxyObject *pRP = (RealProxyObject *)OBJECTREFToObject(refRP);
    DWORD optFlag = pRP->GetOptFlags();

    // If we are cloning some arguments to server domain, we want to do a type check
    // on the cloned objects against the expected type. To find the expected type, we need to
    // know the method signature on the server side, which in turn is obtainable if we know
    // the server MethodDesc. Finding MethodDesc from a slot isnt cheap, so we do it only if we need it
    BOOL bFindServerMD = (RemotableMethodInfo::AreArgsBlittable(m_xret)) ? FALSE : TRUE;
    BOOL bResultOfAddressLookup = FALSE;

    if (m_pCliMD->GetMethodTable()->IsInterface())
    {
        bResultOfAddressLookup = GetInterfaceMethodAddressFast(optFlag, pSrvMT, bFindServerMD);
    }
    else if ((optFlag & OPTIMIZATION_FLAG_INITTED) && (optFlag & OPTIMIZATION_FLAG_PROXY_EQUIVALENT))
    {
        bResultOfAddressLookup = GetTargetAddressFast(optFlag, pSrvMT, bFindServerMD);
    }
    else
    {
        // If the proxy is not cast to an equivalent type, do not perform any optimizations
        bResultOfAddressLookup = FALSE;
    }

#ifdef _DEBUG 
    if (!bResultOfAddressLookup)
        LOG((LF_REMOTING, LL_INFO100, "InitServerInfo. Skipping fast path because failed to find target address.\n"));
#endif // _DEBUG

    _ASSERTE(!bResultOfAddressLookup || !bFindServerMD || m_pSrvMD);
    return bResultOfAddressLookup;
}

// A macro used to help calculate the exact declaring type of a method (this may not be as simple as calling GetMethodTable on the
// method desc if the type is generic and not an interface). We get the additional information from the instance (which provides an
// exact method table, though not necessarily the one the method is actually _declared_ on). We don't compute the instance or the
// method table from that instance in this macro since the logic varies greatly from client to server (the client has to adjust for
// the fact that the instance is a TP).
// We assume a variable called thDeclaringType has already been declared in the current scope.
#define CDC_DETERMINE_DECLARING_TYPE(_pMD, _thInst)                                                                 \
    if (!(_pMD)->HasClassInstantiation() || (_pMD)->IsInterface())                                                  \
    {                                                                                                               \
        thDeclaringType = TypeHandle((_pMD)->GetMethodTable());                                                     \
    }                                                                                                               \
    else                                                                                                            \
    {                                                                                                               \
        TypeHandle *pInst = (_pMD)->GetExactClassInstantiation(_thInst);                                            \
        MethodTable *pApproxDeclaringMT = (_pMD)->GetMethodTable();                                                 \
        thDeclaringType = ClassLoader::LoadGenericInstantiationThrowing(pApproxDeclaringMT->GetModule(),            \
                                                                        pApproxDeclaringMT->GetCl(),                \
                                                                        pApproxDeclaringMT->GetNumGenericArgs(),    \
                                                                        pInst);                                     \
    }

// We have decided the arguments are blittable. We may still need to marshal
// call context if any.
BOOL
CrossDomainChannel::BlitAndCall(ARG_SLOT *pReturn)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        THROWS;
    }
    CONTRACTL_END

    SIZE_T *pRegArgs   = NULL;
    SIZE_T *pStackArgs = NULL;
#ifdef CALLDESCR_ARGREGS 
    SIZE_T  RegArgs[2] = {0};
    pRegArgs = &RegArgs[0];
#endif // CALLDESCR_ARGREGS

    BOOL bOptimizable        = TRUE;
    BOOL bHasObjRefReturnVal = FALSE;

    ARG_SLOT       retVal  = 0;

    Thread  *pCurThread = GetThread();

#ifdef _DEBUG 
    LPCUTF8 pszMethodName;
    pszMethodName = m_pCliMD->GetName();
    LOG((LF_REMOTING, LL_INFO100, "BlitAndCall. Blitting arguments to method %s\n", pszMethodName));
#endif // _DEBUG

    // Collect all client domain GC references together in a single GC frame.
    // refReturnValue contains the returned object.
    // It can also contain a boxed object when the return is a value type and needs marshalling
    struct _gc {
        OBJECTREF   refReturnValue;
        OBJECTREF   refException;
        OBJECTREF   refExecutionContext;
        OBJECTREF   refPrincipal;
    } ClientGC;
    ZeroMemory(&ClientGC, sizeof(ClientGC));
    GCPROTECT_BEGIN(ClientGC);

    _ASSERTE(RemotableMethodInfo::IsReturnBlittable(m_xret));

    // Check for any call context
    BOOL bMarshalCallContext = FALSE;
    BOOL bMarshalReturnCallContext = FALSE;
    if (pCurThread->IsExposedObjectSet())
    {
        THREADBASEREF ref = (THREADBASEREF) pCurThread->GetExposedObjectRaw();
        _ASSERTE(ref != NULL);

        if (ref->GetExecutionContext() != NULL)
        {
            ClientGC.refExecutionContext = ref->GetExecutionContext();
            bMarshalCallContext = TRUE;

            ClientGC.refPrincipal = ReadPrincipal();
        }
    }

    // Push the frame
    ENTER_DOMAIN_ID(m_pSrvDomain);

    // Now create a server domain GC frame for all server side GC references.
    struct _gc {
        OBJECTREF   refReturnValue;
        OBJECTREF   refException;
        OBJECTREF   refExecutionContext;
    } ServerGC;
    ZeroMemory(&ServerGC, sizeof(ServerGC));
    GCPROTECT_BEGIN(ServerGC);

    // Initialize server side info, such as method address etc
    bOptimizable = InitServerInfo();

    if (!bOptimizable)
        goto LeaveDomain;

    RenewLease();

    if (bMarshalCallContext)
    {
        LOG((LF_REMOTING, LL_INFO100, "BlitAndCall. Marshalling call context\n", pszMethodName));
        CrossAppDomainClonerCallback cadcc;
        ObjectClone Marshaller(&cadcc, CrossAppDomain, FALSE);
        ServerGC.refExecutionContext = Marshaller.Clone(ClientGC.refExecutionContext,
                                                        m_pCliDomain,
                                                        GetAppDomain(),
                                                        ClientGC.refExecutionContext);
        THREADBASEREF ref = (THREADBASEREF) pCurThread->GetExposedObjectRaw();
        _ASSERTE(ref != NULL);

        ref->SetExecutionContext(ServerGC.refExecutionContext);

        Marshaller.RemoveGCFrames();
    }

#ifdef PROFILING_SUPPORTED 
    // If we're profiling, notify the profiler that we're about to invoke the remoting target
    if (CORProfilerTrackRemoting())
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->RemotingServerInvocationStarted(reinterpret_cast<ThreadID>(pCurThread));
    }
#endif // PROFILING_SUPPORTED

    {
        GCX_COOP();
        UINT64 uRegTypeMap = 0;
        pStackArgs = (SIZE_T *)((BYTE *)m_pFrame + m_pFrame->GetOffsetOfArgs());

        // Get the 'this' object
        OBJECTREF srvObject = GetServerObject();
#ifdef CALLDESCR_ARGREGS 
        RegArgs[0] = *((SIZE_T*)((BYTE *) m_pFrame + m_pFrame->GetOffsetOfArgumentRegisters()));
        RegArgs[1] = (SIZE_T) OBJECTREFToObject(srvObject);

#else // !CALLDESCR_ARGREGS
        int regArgNum   = 0;
        int fpRegArgNum = 0;

        // It's a pity that we have to allocate a buffer for the arguments on the stack even in BlitAndCall().
        // The problem is we can't use the portion of the stack protected by m_pFrame to store the srvObject,
        // since the srvObject is in the server domain and the TPMethodFrame m_pFrame is in the client domain.
        // I don't think we need to protect the srvOjbect in this case, since it's reachable from the transparent
        // proxy, which is protected by the TPMethodFrame.
        SIZE_T* pTmpStackArgs = (SIZE_T*)_alloca(m_numStackSlotsToCopy * sizeof(SIZE_T));
        memcpy(pTmpStackArgs, pStackArgs, m_numStackSlotsToCopy * sizeof(SIZE_T));
        pStackArgs = pTmpStackArgs;

        pStackArgs[regArgNum++] = (SIZE_T)OBJECTREFToObject(srvObject);

        PORTABILITY_ASSERT("BlitAndCall() - NYI on this platform");

#endif // !CALLDESCR_ARGREGS

        retVal = DispatchCall(pStackArgs,
            m_numStackSlotsToCopy,
            pRegArgs,
            uRegTypeMap,
            NULL,                       // pvRetBuff
            0,                          // cbRetBuff
            GetFPReturnSize(),
            m_pTargetAddress,
            &ServerGC.refException,
            GET_CTX_TRANSITION_FRAME());

        // If the return value is a GC ref, store it in a protected place
        if (RemotableMethodInfo::IsReturnGCRef(m_xret))
        {
            ServerGC.refReturnValue = ArgSlotToObj(retVal);
            bHasObjRefReturnVal = TRUE;
        }
    }

#ifdef PROFILING_SUPPORTED 
    if (CORProfilerTrackRemoting())
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->RemotingServerInvocationReturned(reinterpret_cast<ThreadID>(pCurThread));
    }
#endif // PROFILING_SUPPORTED

    if (pCurThread->IsExposedObjectSet())
    {
        THREADBASEREF ref = (THREADBASEREF) pCurThread->GetExposedObjectRaw();
        _ASSERTE(ref != NULL);

        if (ref->GetExecutionContext() != NULL)
        {
            ServerGC.refExecutionContext = ref->GetExecutionContext();
            bMarshalReturnCallContext = TRUE;
            LOG((LF_REMOTING, LL_INFO100, "BlitAndCall. Marshalling return call context\n", pszMethodName));
            CrossAppDomainClonerCallback cadcc;
            ObjectClone Marshaller(&cadcc, CrossAppDomain, FALSE);

            ResetPrincipal();

            EXECUTIONCONTEXTREF ecref = (EXECUTIONCONTEXTREF)Marshaller.Clone(ServerGC.refExecutionContext,
                                                                              GetAppDomain(),
                                                                              m_pCliDomain,
                                                                              ServerGC.refExecutionContext);
            if (ClientGC.refExecutionContext != NULL)
                ((EXECUTIONCONTEXTREF)ClientGC.refExecutionContext)->SetLogicalCallContext(ecref->GetLogicalCallContext());
            else
                ClientGC.refExecutionContext = (OBJECTREF)ecref;

            Marshaller.RemoveGCFrames();
        }
    }

    if (ServerGC.refException != NULL)
    {
        LOG((LF_REMOTING, LL_INFO100, "BlitAndCall. Exception thrown ! Marshalling exception. \n", pszMethodName));
        CrossAppDomainClonerCallback cadcc;
        ObjectClone    Marshaller(&cadcc, CrossAppDomain, FALSE);
        ClientGC.refException = Marshaller.Clone(ServerGC.refException, GetAppDomain(), m_pCliDomain, ServerGC.refExecutionContext);

        Marshaller.RemoveGCFrames();

        // We have to be in the right domain before we throw the exception
        goto LeaveDomain;
    }

    if (bHasObjRefReturnVal)
    {
        // Must be a domain agile GC ref. We can just copy the reference into the client GC frame.
        ClientGC.refReturnValue = ServerGC.refReturnValue;
    }

LeaveDomain: ;

    GCPROTECT_END(); // ServerGC

    END_DOMAIN_TRANSITION;

    if (ClientGC.refException != NULL)
    {
        RestorePrincipal(&ClientGC.refPrincipal);
        COMPlusThrow(ClientGC.refException);
    }

    if (bMarshalReturnCallContext)
    {
        THREADBASEREF ref = (THREADBASEREF) pCurThread->GetExposedObjectRaw();
        _ASSERTE(ref != NULL);

        ref->SetExecutionContext(ClientGC.refExecutionContext);
    }

    RestorePrincipal(&ClientGC.refPrincipal);

    // If the return type is an object, take it out of the protected ref
    if (bHasObjRefReturnVal)
    {
        retVal = (ARG_SLOT)(size_t)OBJECTREFToObject(ClientGC.refReturnValue);
    }

    GCPROTECT_END(); // ClientGC

    *pReturn = retVal;
    return bOptimizable;
}

// Argument attributes
#define ARG_NEEDS_UNBOX 0x80000000
#define ARG_GOES_IN_EDX 0x40000000
#define ARG_IS_BYREF    0x20000000
#define ARG_OFFSET_MASK 0x00FFFFFF

// Structure to hold arguments for MarshalAndCall
struct MarshalAndCallArgs  : public CtxTransitionBaseArgs
{
    MarshalAndCallArgs() : Marshaller(&cadcc, CrossAppDomain, FALSE)
    {
        STATIC_CONTRACT_SO_INTOLERANT;
    }

    CrossDomainChannel * pThis;

    struct _gc {
        OBJECTREF   refReturnValue;
        OBJECTREF   refException;
        OBJECTREF   refExecutionContext;
        OBJECTREF   refPrincipal;
    } ClientGC;

    BOOL bOptimizable;

    ObjectClone Marshaller;
    CrossAppDomainClonerCallback cadcc;

    MetaSig     *mSig;
    ArgIterator *argit;

    DWORD   dwNumArgs;
    SIZE_T *pRegArgs;
    SIZE_T *pStackArgs;
    DWORD  *pArgAttribs;

    DWORD      dwNumObjectsMarshalled;
    BOOL      *bMarshalledArgs;
    OBJECTREF *pClientArgArray;

    BOOL        bHasByRefArgsToMarshal;
    int        *pByRefArgAttribs;
    TypeHandle *pThByRefs;

    TypeHandle retTh;
    ARG_SLOT   returnVal;
    LPVOID     pvRetBuff;
    UINT64     cbRetBuff;
    BOOL bHasObjRefReturnVal;
    BOOL bHasRetBuffArg;
    BOOL bHasNonStandardReturnVal;          // only used on IA64
    BOOL bHasValueTypeReturnValToMarshal;

    BOOL bMarshalCallContext;
    BOOL bMarshalReturnCallContext;

    UINT64 uRegTypeMap;
};

// Simple wrapper to go from C to C++.
void MarshalAndCall_Wrapper2(MarshalAndCallArgs * pArgs)
{
    WRAPPER_CONTRACT;

    pArgs->pThis->MarshalAndCall_Wrapper(pArgs);
}

void CrossDomainChannel::MarshalAndCall_Wrapper(MarshalAndCallArgs * pArgs)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        THROWS;
    }
    CONTRACTL_END;

    // Set up a rip-cord that will immediately stop us reporting GC references we're keeping alive in the Marshaller that was passed
    // to us in the event that this appdomain is unloaded underneath our feet. This avoids us keeping any server objects alive after
    // their domain has unloaded.
    ReportClonerRefsHolder sHolder(&pArgs->Marshaller);

    Thread*    pCurThread    = GetThread();
    AppDomain* pCurAppDomain = GetAppDomain();

        // Now create a server domain GC frame for all non-arg server side GC references.
        struct _gc {
            OBJECTREF   refReturnValue;
            OBJECTREF   refException;
            OBJECTREF   refExecutionContext;
        } ServerGC;
        ZeroMemory(&ServerGC, sizeof(ServerGC));
        GCPROTECT_BEGIN(ServerGC);

        // And a variable sized array and frame of marshaled arg GC references.
        OBJECTREF *pServerArgArray = NULL;
        pServerArgArray = (OBJECTREF *) _alloca(pArgs->dwNumObjectsMarshalled * sizeof(OBJECTREF));
        ZeroMemory(pServerArgArray, sizeof(OBJECTREF) * pArgs->dwNumObjectsMarshalled);

        TypeHandle* pServerArgTH = (TypeHandle *) _alloca(pArgs->dwNumObjectsMarshalled * sizeof(TypeHandle));
        GCPROTECT_ARRAY_BEGIN(pServerArgArray[0], pArgs->dwNumObjectsMarshalled);

        // Initialize server side info, such as method address etc
        pArgs->bOptimizable = InitServerInfo();

        if (!pArgs->bOptimizable)
            goto LeaveDomain;

        RenewLease();

        // First clone objref arguments into the called domain
        if (!RemotableMethodInfo::AreArgsBlittable(m_xret))
        {
            // When computing the method signature we need to take special care if the call is on a non-interface class with a
            // generic instantiation (since in that case we may have a representative method with a non-concrete signature).
            TypeHandle thDeclaringType;
            CDC_DETERMINE_DECLARING_TYPE(m_pSrvMD, TypeHandle(GetServerObject()->GetTypeHandle()));
            MetaSig mSrvSideSig(m_pSrvMD, thDeclaringType);
            DWORD dwMarshalledArg = 0;
            for (DWORD i = 0; i < pArgs->dwNumArgs; i++)
            {
                CorElementType cType = mSrvSideSig.NextArg();
                if (pArgs->bMarshalledArgs[i] != TRUE)
                    continue;

                TypeHandle argTh;
                if (cType == ELEMENT_TYPE_BYREF)
                    mSrvSideSig.GetByRefType(&argTh);
                else
                    argTh = mSrvSideSig.GetLastTypeHandleThrowing();

                pServerArgTH[dwMarshalledArg] = argTh;
                pServerArgArray[dwMarshalledArg] = pArgs->Marshaller.Clone(pArgs->pClientArgArray[dwMarshalledArg],
                                                                           argTh,
                                                                           m_pCliDomain,
                                                                           pCurAppDomain,
                                                                           pArgs->ClientGC.refExecutionContext);
                dwMarshalledArg++;
            }

            // Make sure return type is loaded
            TypeHandle thReturn = mSrvSideSig.GetRetTypeHandleThrowing();
            _ASSERTE(!thReturn.IsNull());

            if (pArgs->bHasValueTypeReturnValToMarshal)
            {
                // The return is a value type which could have GC ref fields. Allocate a boxed value type so that the
                // return value goes into that. During return from call we'll clone it and copy it onto the stack
                ServerGC.refReturnValue = thReturn.AsMethodTable()->Allocate();
            }
        }

        // Then clone the call context if any
        if (pArgs->bMarshalCallContext)
        {
#ifdef _DEBUG 
            LOG((LF_REMOTING, LL_INFO100, "MarshalAndCall. Marshalling call context\n"));
#endif
            ServerGC.refExecutionContext = pArgs->Marshaller.Clone(pArgs->ClientGC.refExecutionContext,
                                                                   m_pCliDomain,
                                                                   pCurAppDomain,
                                                                   pArgs->ClientGC.refExecutionContext);
            THREADBASEREF ref = (THREADBASEREF) pCurThread->GetExposedObjectRaw();
            _ASSERTE(ref != NULL);

            ref->SetExecutionContext(ServerGC.refExecutionContext);
        }

#ifdef PROFILING_SUPPORTED 
        // If we're profiling, notify the profiler that we're about to invoke the remoting target
        if (CORProfilerTrackRemoting())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->RemotingServerInvocationStarted(reinterpret_cast<ThreadID>(pCurThread));
        }
#endif // PROFILING_SUPPORTED

        {
            GCX_COOP();
            if (!RemotableMethodInfo::AreArgsBlittable(m_xret))
            {
                // Next place arguments into the destination array
                // No GC should occur between now and call dispatch
                for (DWORD i = 0 ; i < pArgs->dwNumObjectsMarshalled; i++)
                {
                    BOOL bNeedUnbox = pArgs->pArgAttribs[i] & ARG_NEEDS_UNBOX;
                    BOOL bGoesInEDX = pArgs->pArgAttribs[i] & ARG_GOES_IN_EDX;
                    BOOL bIsByRef = pArgs->pArgAttribs[i] & ARG_IS_BYREF;
                    DWORD dwOffset = pArgs->pArgAttribs[i] & ARG_OFFSET_MASK;

                    SIZE_T *pDest = NULL;
#if defined(CALLDESCR_ARGREGS) 
                    if (bGoesInEDX)
                    {
                        pDest = pArgs->pRegArgs;
                    }
                    else
#endif // defined(CALLDESCR_ARGREGS)
                    {
                        pDest = (SIZE_T *)((BYTE *)(pArgs->pStackArgs) + dwOffset);
                    }

                    if (bNeedUnbox && !bIsByRef)
                    {
                        pServerArgTH[i].GetMethodTable()->UnBoxIntoUnchecked(pDest, pServerArgArray[i]);
                    }
                    else if (bIsByRef)
                    {
                        if (bNeedUnbox)
                        {
                            // We don't use the fast path for byref nullables, so UnBox() can be used
                            *pDest = (SIZE_T)pServerArgArray[i]->UnBox();
                        }
                        else
                        {
                            // Point to the OBJECTREF
                            *pDest = (SIZE_T)&pServerArgArray[i];
                        }
                    }
                    else
                    {
                        *pDest = (SIZE_T)OBJECTREFToObject(pServerArgArray[i]);
                    }
                }
            }

            // Get the 'this' object
            OBJECTREF srvObject    = GetServerObject();
            SIZE_T*   pRegArgs     = NULL;
            LPVOID    pvTmpRetBuff = NULL;
#if defined(CALLDESCR_ARGREGS) 
            // Check if EDX should point inside a temporary boxed object we created
            if (pArgs->bHasValueTypeReturnValToMarshal && pArgs->bHasRetBuffArg)
            {
                    // We don't use the fast path for values that return nullables, so UnBox() can be used
                *(pArgs->pRegArgs) = (SIZE_T)ServerGC.refReturnValue->UnBox();
            }
            (pArgs->pRegArgs)[1] = (SIZE_T)OBJECTREFToObject(srvObject);

            pRegArgs = &((pArgs->pRegArgs)[0]);

#else // !defined(CALLDESCR_ARGREGS)
#if THISPTR_LOCATION > 0 
            if (pArgs->bHasRetBuffArg)
            {
                if (pArgs->bHasValueTypeReturnValToMarshal)
                {
                        // We don't use the fast path for values that return nullables, so UnBox() can be used
                    (pArgs->pStackArgs)[0] = (SIZE_T)ServerGC.refReturnValue->UnBox();
                    (pArgs->pStackArgs)[1] = (SIZE_T)OBJECTREFToObject(srvObject);
                }
                else
                {
                    UNREACHABLE();
                }
            }
            else
#endif // THISPTR_LOCATION > 0
            {
                (pArgs->pStackArgs)[0] = (SIZE_T)OBJECTREFToObject(srvObject);
            }

#if defined(CALLDESCR_RETBUF) 
            if (pArgs->bHasValueTypeReturnValToMarshal)
            {
                _ASSERTE(pArgs->cbRetBuff != 0);
                if (!pArgs->bHasRetBuffArg)
                {
                    pvTmpRetBuff = _alloca(ENREGISTERED_RETURNTYPE_MAXSIZE);
                }
                else
                {
                        // We don't use the fast path for values that return nullables, so UnBox() can be used
                    pvTmpRetBuff = ServerGC.refReturnValue->UnBox();
                }
            }
            else
            {
                _ASSERTE(pArgs->cbRetBuff == 0);
            }
#endif // defined(CALLDESCR_RETBUF)
#endif // !defined(CALLDESCR_ARGREGS)

            pArgs->returnVal = DispatchCall(
                pArgs->pStackArgs,
                m_numStackSlotsToCopy,
                pRegArgs,
                pArgs->uRegTypeMap,
                pvTmpRetBuff,
                pArgs->cbRetBuff,
                GetFPReturnSize(),
                m_pTargetAddress,
                &ServerGC.refException,
                pArgs->GetCtxTransitionFrame());

            // If the return value is a GC ref, store it in a protected place
            if (RemotableMethodInfo::IsReturnGCRef(m_xret))
            {
                ServerGC.refReturnValue = ArgSlotToObj(pArgs->returnVal);
                pArgs->bHasObjRefReturnVal = TRUE;
            }

            if (pArgs->bHasValueTypeReturnValToMarshal)
            {
                if (!pArgs->bHasRetBuffArg)
                {
                    if (pArgs->pvRetBuff == NULL)
                    {
                        // We don't use the fast path for values that return nullables, so UnBox() can be used

                        // The value type return value is returned by value in this case.
                        // We have to copy it back into our server object.
                        // In all other cases (except the non-standard return value case below),
                        // the return value should be in the server object already.
                        *(SIZE_T*)(ServerGC.refReturnValue->UnBox()) = pArgs->returnVal;
                    }
                    else
                    {
#if defined(CALLDESCR_RETBUF) 
                        // We don't use the fast path for values that return nullables, so UnBox() can be used
                        CopyValueClass(ServerGC.refReturnValue->UnBox(), pvTmpRetBuff, ServerGC.refReturnValue->GetMethodTable(), pCurAppDomain);
#else  // !CALLDESCR_RETBUF
                        UNREACHABLE();
#endif // !CALLDESCR_RETBUF
                    }
                }
            }
        }

#ifdef PROFILING_SUPPORTED 
        if (CORProfilerTrackRemoting())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->RemotingServerInvocationReturned( reinterpret_cast<ThreadID>(pCurThread));
        }
#endif // PROFILING_SUPPORTED

        if (pCurThread->IsExposedObjectSet())
        {
            THREADBASEREF ref = (THREADBASEREF) pCurThread->GetExposedObjectRaw();
            _ASSERTE(ref != NULL);

            if (ref->GetExecutionContext() != NULL)
            {
                ServerGC.refExecutionContext = ref->GetExecutionContext();
                pArgs->bMarshalReturnCallContext = TRUE;
#ifdef _DEBUG 
                LOG((LF_REMOTING, LL_INFO100, "MarshalAndCall. Marshalling return call context\n"));
#endif
                ResetPrincipal();
                EXECUTIONCONTEXTREF ecref = (EXECUTIONCONTEXTREF)pArgs->Marshaller.Clone(ServerGC.refExecutionContext,
                                                                                         pCurAppDomain,
                                                                                         m_pCliDomain,
                                                                                         ServerGC.refExecutionContext);
                if (pArgs->ClientGC.refExecutionContext != NULL)
                {
                    ((EXECUTIONCONTEXTREF)pArgs->ClientGC.refExecutionContext)->SetLogicalCallContext(ecref->GetLogicalCallContext());
                }
                else
                {
                    pArgs->ClientGC.refExecutionContext = (OBJECTREF)ecref;
                }
            }
        }

        if (ServerGC.refException != NULL)
        {
#ifdef _DEBUG 
            LOG((LF_REMOTING, LL_INFO100, "MarshalAndCall. Exception thrown ! Marshalling exception. \n"));
#endif
            pArgs->ClientGC.refException = pArgs->Marshaller.Clone(ServerGC.refException,
                                                                   pCurAppDomain,
                                                                   m_pCliDomain,
                                                                   ServerGC.refExecutionContext);
            goto LeaveDomain;
        }

        if (!RemotableMethodInfo::IsReturnBlittable(m_xret))
        {
            LOG((LF_REMOTING, LL_INFO100, "MarshalAndCall. Marshalling return object\n"));
            // Need to marshal the return object

            pArgs->ClientGC.refReturnValue = pArgs->Marshaller.Clone(ServerGC.refReturnValue,
                                                                     pArgs->retTh,
                                                                     pCurAppDomain,
                                                                     m_pCliDomain,
                                                                     ServerGC.refExecutionContext);

            if (pArgs->bHasValueTypeReturnValToMarshal)
            {
                // Need to copy contents from temp return buffer to the original return buffer
                void *pDest;
                if (!pArgs->bHasRetBuffArg)
                {
                    if (pArgs->pvRetBuff == NULL)
                    {
                        pDest = (void*)&(pArgs->returnVal);
                    }
                    else
                    {
                        pDest = pArgs->pvRetBuff;
                    }
                }
                else
                {
                    pDest = *(pArgs->argit->GetRetBuffArgAddr());
                }
                // We don't use the fast path for values that return nullables, so UnBox() can be used
                CopyValueClass(pDest, pArgs->ClientGC.refReturnValue->UnBox(), pArgs->ClientGC.refReturnValue->GetMethodTable(), m_pCliDomain);
            }
        }
        else if (pArgs->bHasObjRefReturnVal)
        {
            // Must be a domain agile GC ref. We can just copy the reference into the client GC frame.
            pArgs->ClientGC.refReturnValue = ServerGC.refReturnValue;
        }

        // Marshal any by-ref args into calling domain
        if (pArgs->bHasByRefArgsToMarshal)
        {
#ifdef _DEBUG 
            LOG((LF_REMOTING, LL_INFO100, "MarshalAndCall. Marshalling by-ref args\n"));
#endif
            int iMarshalledArg = -1;
            // Look for by ref args
            for (DWORD i = 0; i < pArgs->dwNumArgs; i++)
            {
                if (pArgs->bMarshalledArgs[i] != TRUE)
                    continue;

                iMarshalledArg++;

                BOOL bNeedUnbox = pArgs->pArgAttribs[iMarshalledArg] & ARG_NEEDS_UNBOX;
                BOOL bIsByRef = pArgs->pArgAttribs[iMarshalledArg] & ARG_IS_BYREF;

                if (!bIsByRef)
                    continue;

                TypeHandle argTh = pArgs->pThByRefs[iMarshalledArg];
                int offset = pArgs->pByRefArgAttribs[iMarshalledArg];
                OBJECTREF refReturn = pServerArgArray[iMarshalledArg];
                GCPROTECT_BEGIN(refReturn);

                refReturn = pArgs->Marshaller.Clone(refReturn,
                                                    argTh,
                                                    pCurAppDomain,
                                                    m_pCliDomain,
                                                    ServerGC.refExecutionContext);
                if (bNeedUnbox)
                {
                    // We don't use the fast path for byref nullables, so UnBox() can be used
                    BYTE *pTargetAddress = *((BYTE **)((BYTE*)m_pFrame + offset));
                    CopyValueClass(pTargetAddress, refReturn->UnBox(), refReturn->GetMethodTable(), m_pCliDomain);
                }
                else
                {
                    SetObjectReference(*((OBJECTREF **)((BYTE*)m_pFrame + offset)), refReturn, m_pCliDomain);
                }
                GCPROTECT_END();
            }
        }

     LeaveDomain:;

        GCPROTECT_END(); // pServerArgArray
        GCPROTECT_END(); // ServerGC
}


// Arguments need to be marshalled before dispatch. We walk thru each argument,
// inspect its type, make a list of objects that need to be marshalled, cross over to the new domain,
// marshal the objects and dispatch the call. Upon return, we marshal the return object if any and
// by ref objects if any. Call contexts flows either way
BOOL
CrossDomainChannel::MarshalAndCall(ARG_SLOT *pReturn)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        THROWS;
    }
    CONTRACTL_END

    MarshalAndCallArgs args;

    args.returnVal = 0;
    args.bHasByRefArgsToMarshal = FALSE;

    args.pvRetBuff = NULL;
    args.cbRetBuff = 0;
    args.bHasObjRefReturnVal             = FALSE;
    args.bHasRetBuffArg                  = FALSE;
    args.bHasNonStandardReturnVal        = FALSE;
    args.bHasValueTypeReturnValToMarshal = FALSE;

    DWORD dwNumArgs = 0;
    DWORD dwNumObjectsMarshalled = 0;

    DWORD *pArgAttribs = NULL;
    BOOL *bMarshalledArgs = NULL;
    int *pByRefArgAttribs = NULL;
    TypeHandle *pThByRefs = NULL;

    SIZE_T *pRegArgs   = NULL;
    SIZE_T *pStackArgs = NULL;
#ifdef CALLDESCR_ARGREGS 
    SIZE_T  RegArgs[2] = {0};
    pRegArgs = &RegArgs[0];
#endif // CALLDESCR_ARGREGS

    Thread *pCurThread = GetThread();

#ifdef _DEBUG 
    LPCUTF8 pszMethodName;
    pszMethodName = m_pCliMD->GetName();
    LOG((LF_REMOTING, LL_INFO100, "MarshalAndCall. Marshalling arguments to method %s\n", pszMethodName));
#endif // _DEBUG

    // Collect all client domain GC references together in a single GC frame.
    // refReturnValue contains the returned object when its a value type and needs marshalling
    ZeroMemory(&args.ClientGC, sizeof(args.ClientGC));
    GCPROTECT_BEGIN(args.ClientGC);

    // When computing the method signature we need to take special care if the call is on a non-interface class with a
    // generic instantiation (since in that case we may have a representative method with a non-concrete signature).
    TypeHandle thDeclaringType;
    CDC_DETERMINE_DECLARING_TYPE(m_pCliMD, TypeHandle((MethodTable*)m_pFrame->GetThis()->GetPtrOffset(CTPMethodTable::GetOffsetOfMT())));
    MetaSig mSig(m_pCliMD, thDeclaringType);
    ArgIterator argit(m_pFrame, &mSig);
    BYTE   typ;
    UINT32 structSize;
    int    ofs;

    // NumFixedArgs() doesn't count the "this" object, but SizeOfFrameArgumentArray() does.
    dwNumArgs = mSig.NumFixedArgs();
    m_numStackSlotsToCopy = mSig.SizeOfFrameArgumentArray(m_pCliMD->IsStatic()) / sizeof(SIZE_T);

    // Ensure none of the following _alloca's are subject to integer overflow problems.
    DWORD dwMaxEntries = dwNumArgs > m_numStackSlotsToCopy ? dwNumArgs : m_numStackSlotsToCopy;
    DWORD dwResult;
    if (!ClrSafeInt<DWORD>::multiply(dwMaxEntries, sizeof(SIZE_T), dwResult))
        COMPlusThrowOM();

#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK 
    DWORD dwOffsetOfArg = m_numStackSlotsToCopy * sizeof(SIZE_T);
#else // !STACK_GROWS_DOWN_ON_ARGS_WALK
    DWORD dwOffsetOfArg = 0;
#endif // !STACK_GROWS_DOWN_ON_ARGS_WALK

    args.bHasRetBuffArg           = mSig.HasRetBuffArg();
    args.bHasNonStandardReturnVal = mSig.HasNonStandardByvalReturn();

#ifdef CALLDESCR_ARGREGS 
    BOOL  bArgumentRegisterUsed = FALSE;
    if (args.bHasRetBuffArg)
    {
        bArgumentRegisterUsed = TRUE;
    }
#endif // CALLDESCR_ARGREGS

    // pArgAttribs tell where the marshalled objects should go, where they need unboxing etc
    pArgAttribs = (DWORD*) _alloca(dwNumArgs * sizeof(DWORD));
    ZeroMemory(pArgAttribs, sizeof(DWORD) * dwNumArgs);
    // pThByRefs has the typehandles of the by-ref args
    pThByRefs = (TypeHandle *)_alloca(dwNumArgs * sizeof(TypeHandle));
    ZeroMemory(pThByRefs, sizeof(TypeHandle) *dwNumArgs);
    // pByRefArgAttribs tell where the by-ref args should go, after the call
    pByRefArgAttribs = (int*) _alloca(dwNumArgs * sizeof(int));
    ZeroMemory(pByRefArgAttribs, sizeof(int) * dwNumArgs);
    // bMarshalledArgs is a bunch of flags that tell which args were marshalled
    bMarshalledArgs = (BOOL*) _alloca(dwNumArgs * sizeof(BOOL));
    ZeroMemory(bMarshalledArgs, sizeof(BOOL) * dwNumArgs);

    // pArgArray contains marshalled objects on the client side
    OBJECTREF *pClientArgArray = NULL;
    pClientArgArray = (OBJECTREF *) _alloca(dwNumArgs * sizeof(OBJECTREF));
    ZeroMemory(pClientArgArray, sizeof(OBJECTREF) * dwNumArgs);
    GCPROTECT_ARRAY_BEGIN(pClientArgArray[0], dwNumArgs);

    // pStackArgs will finally contain the arguments that'll be fed to Dispatch call. The Marshalled objects
    // are not placed directly into pStackArgs because its not possible to GCPROTECT an array that can contain
    // both GC refs and primitives.
    pStackArgs = (SIZE_T*)_alloca(m_numStackSlotsToCopy * sizeof (SIZE_T));
    ZeroMemory(pStackArgs, m_numStackSlotsToCopy * sizeof(SIZE_T));

    UINT64 uRegTypeMap = 0;
    BYTE*  pMap         = (BYTE*)&uRegTypeMap;
#if defined(CALLDESCR_REGTYPEMAP) 
    int    regArgNum    = 0;

    if (!m_pCliMD->IsStatic())
    {
        regArgNum++;
        dwOffsetOfArg += sizeof(SIZE_T);
    }

#if !defined(RETBUF_ARG_SPECIAL_PARAM) 
    if (args.bHasRetBuffArg)
    {
        regArgNum++;
        dwOffsetOfArg += sizeof(SIZE_T);
    }
#endif // !defined(RETBUF_ARG_SPECIAL_PARAM)


#endif // defined(CALLDESCR_REGTYPEMAP)

    for (int argNum = 0;
         0 != (ofs = argit.GetNextOffset(&typ, &structSize));
         argNum++
#ifdef STACK_GROWS_UP_ON_ARGS_WALK 
         , dwOffsetOfArg += 
#if ENREGISTERED_PARAMTYPE_MAXSIZE
                                    (MetaSig::IsArgPassedByRef(structSize))?sizeof(PVOID):
#endif
/* ----read---->   */                                                    StackElemSize(structSize)

#endif
        )
    {
#if defined(CALLDESCR_REGTYPEMAP) 


        FillInRegTypeMap(&argit, pMap, &regArgNum, &typ, structSize);
#endif // defined(CALLDESCR_REGTYPEMAP)

        SIZE_T *pDestToCopy = NULL;
#ifdef CALLDESCR_ARGREGS 
        if (!bArgumentRegisterUsed && gElementTypeInfo[typ].m_enregister)
        {
            pDestToCopy = pRegArgs;
            bArgumentRegisterUsed = TRUE;
        }
        else
#endif // CALLDESCR_ARGREGS
        {
#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK 
            dwOffsetOfArg -= StackElemSize(structSize);
#endif
            _ASSERTE(dwOffsetOfArg < (m_numStackSlotsToCopy * sizeof(SIZE_T)));
            pDestToCopy = (SIZE_T*)((BYTE *)pStackArgs + dwOffsetOfArg);
        }

        CorElementType origTyp = (CorElementType) typ;
        // Get the signature type of the argument (For ex. enum will be E_T_VT, not E_T_I4 etc)
        SigPointer sp = mSig.GetArgProps();
        CorElementType eType;
        IfFailThrow(sp.GetElemType(&eType));
        typ = eType;
        
        if (typ == ELEMENT_TYPE_VAR ||
            typ == ELEMENT_TYPE_MVAR ||
            typ == ELEMENT_TYPE_GENERICINST)
            typ = origTyp;

        switch (typ)
        {
                case ELEMENT_TYPE_BOOLEAN:
                case ELEMENT_TYPE_I1:
                case ELEMENT_TYPE_U1:
                case ELEMENT_TYPE_I2:
                case ELEMENT_TYPE_U2:
                case ELEMENT_TYPE_CHAR:
                case ELEMENT_TYPE_I4:
                case ELEMENT_TYPE_U4:
                case ELEMENT_TYPE_R4:

#if defined(_X86_) 
                    *(pDestToCopy) = *((SIZE_T*)((BYTE*)m_pFrame + ofs));
#else // !defined(_WIN64)
                    PORTABILITY_ASSERT("MarshalAndCall() - NYI on this platform");
#endif // !defined(_WIN64)
                    break;

                case ELEMENT_TYPE_I:
                case ELEMENT_TYPE_U:
                case ELEMENT_TYPE_PTR:
                case ELEMENT_TYPE_FNPTR:

                    *((SIZE_T*)((BYTE *)pDestToCopy)) = *((SIZE_T*)((BYTE*)m_pFrame + ofs));
                    break;

                case ELEMENT_TYPE_I8:
                case ELEMENT_TYPE_U8:
                case ELEMENT_TYPE_R8:

                    *((INT64*)((BYTE *)pDestToCopy)) = *((INT64 *)((BYTE*)m_pFrame + ofs));
                    break;


                case ELEMENT_TYPE_BYREF:
                {
                    // Check if this is a by-ref primitive
                    OBJECTREF refTmpBox = NULL;
                    TypeHandle ty = TypeHandle();
                    CorElementType brType = mSig.GetByRefType(&ty);
                    if (CorIsPrimitiveType(brType) || ty.IsValueType())
                    {

                        // Needs marshalling
                        MethodTable *pMT = NULL;
                        if (CorIsPrimitiveType(brType))
                            pMT = g_Mscorlib.FetchElementType(brType);
                        else
                            pMT = ty.GetMethodTable();
                        refTmpBox = pMT->Box(*((SIZE_T**)((BYTE*)m_pFrame + ofs)), TRUE);
                        pArgAttribs[dwNumObjectsMarshalled] |= ARG_NEEDS_UNBOX;
                    }
                    else
                    {
                        OBJECTREF *refRefObj = *((OBJECTREF **)((BYTE*)m_pFrame + ofs));
                        refTmpBox = (refRefObj == NULL ? NULL : *refRefObj);
                    }

                    pByRefArgAttribs[dwNumObjectsMarshalled] = ofs;
                    pThByRefs[dwNumObjectsMarshalled] = ty;

                    // we should have stopped nullables before we got here in DoStaticAnalysis
                    _ASSERTE(ty.IsNull() || !Nullable::IsNullableType(ty));
                    pArgAttribs[dwNumObjectsMarshalled] |= ARG_IS_BYREF;

                    args.bHasByRefArgsToMarshal = TRUE;

                    pClientArgArray[dwNumObjectsMarshalled] = refTmpBox;
                    bMarshalledArgs[argNum] = TRUE;

#ifdef CALLDESCR_ARGREGS 
                    if (pDestToCopy == pRegArgs)
                    {
                        pArgAttribs[dwNumObjectsMarshalled] |= ARG_GOES_IN_EDX;    // Indicate that this goes in EDX
                    }
                    else
#endif // CALLDESCR_ARGREGS
                    {
                        _ASSERTE(dwOffsetOfArg < ARG_OFFSET_MASK);
                        pArgAttribs[dwNumObjectsMarshalled] |= dwOffsetOfArg;
                    }
                    dwNumObjectsMarshalled++;
                }
                break;

                case ELEMENT_TYPE_VALUETYPE:
                {

                    TypeHandle th = mSig.GetLastTypeHandleThrowing();

#ifdef _DEBUG 
                    {
                        DefineFullyQualifiedNameForClass()
                        LPCUTF8 szTypeName = GetFullyQualifiedNameForClassNestedAware(th.GetClass());
                        LOG((LF_REMOTING, LL_INFO100, "MarshalAndCall. Boxing a value type argument of type %s.\n", &szTypeName[0]));
                    }
#endif // _DEBUG

                    OBJECTREF refTmpBox;
#if defined(ENREGISTERED_PARAMTYPE_MAXSIZE) 
                    if (MetaSig::IsArgPassedByRef(structSize))
                    {
                        refTmpBox = th.GetMethodTable()->Box(*(LPVOID*)((BYTE*)m_pFrame + ofs), TRUE);

                        // we should have stopped nullables before we got here in DoStaticAnalysis
                        _ASSERTE(!Nullable::IsNullableType(th));
                        pArgAttribs[dwNumObjectsMarshalled] |= ARG_IS_BYREF;

                        pByRefArgAttribs[dwNumObjectsMarshalled] = ofs;
                        pThByRefs[dwNumObjectsMarshalled] = th;
                    }
                    else
#endif // defined(ENREGISTERED_PARAMTYPE_MAXSIZE)
                    {
                        refTmpBox = th.GetMethodTable()->Box(((BYTE*)m_pFrame + ofs), TRUE);
                    }
                    pClientArgArray[dwNumObjectsMarshalled] = refTmpBox;
                    bMarshalledArgs[argNum] = TRUE;

#if defined(CALLDESCR_ARGREGS) 
                    if (pDestToCopy == pRegArgs)
                    {
                        pArgAttribs[dwNumObjectsMarshalled]  |= ARG_GOES_IN_EDX;    // Indicate that this goes in EDX
                    }
                    else
#endif // defined(CALLDESCR_ARGREGS)
                    {
                        _ASSERTE(dwOffsetOfArg < ARG_OFFSET_MASK);
                        pArgAttribs[dwNumObjectsMarshalled] |= dwOffsetOfArg;
                    }
                    pArgAttribs[dwNumObjectsMarshalled] |= ARG_NEEDS_UNBOX; // Indicate that an unboxing is required
                    dwNumObjectsMarshalled++;
                }
                break;

                case ELEMENT_TYPE_SZARRAY:          // Single Dim
                case ELEMENT_TYPE_ARRAY:            // General Array
                case ELEMENT_TYPE_CLASS:            // Class
                case ELEMENT_TYPE_OBJECT:
                case ELEMENT_TYPE_STRING:           // System.String
                case ELEMENT_TYPE_VAR:
                {
                    OBJECTREF *refRefObj = (OBJECTREF *)((BYTE*)m_pFrame + ofs);
                    // The frame does protect this object, so mark it as such to avoid asserts
                    INDEBUG(Thread::ObjectRefNew(refRefObj);)
                    INDEBUG(Thread::ObjectRefProtected(refRefObj);)

                    pClientArgArray[dwNumObjectsMarshalled] = *refRefObj;
                    bMarshalledArgs[argNum] = TRUE;

#ifdef CALLDESCR_ARGREGS 
                    if (pDestToCopy == pRegArgs)
                    {
                        pArgAttribs[dwNumObjectsMarshalled] |= ARG_GOES_IN_EDX;    // Indicate that this goes in EDX
                    }
                    else
#endif // CALLDESCR_ARGREGS
                    {
                        _ASSERTE(dwOffsetOfArg < ARG_OFFSET_MASK);
                        pArgAttribs[dwNumObjectsMarshalled] |= dwOffsetOfArg;
                    }
                    dwNumObjectsMarshalled++;
                }
                break;

                default:
                    _ASSERTE(!"Unknown Element type in MarshalAndCall" );
        }
    }

    if (!RemotableMethodInfo::IsReturnBlittable(m_xret))
    {
        CorElementType retType = mSig.GetReturnType();
        if (retType == ELEMENT_TYPE_VALUETYPE)
        {
            args.retTh = mSig.GetRetTypeHandleThrowing();
            args.bHasValueTypeReturnValToMarshal = TRUE;
        }
        else
        {
            args.retTh = mSig.GetRetTypeHandleThrowing();
        }
    }

#if defined(CALLDESCR_RETBUF) 
    if (args.bHasRetBuffArg)
    {
        args.cbRetBuff = mSig.GetReturnTypeSize();
    }
    else if (args.bHasValueTypeReturnValToMarshal)
    {
        args.pvRetBuff = m_pFrame->GetReturnValuePtr();
        args.cbRetBuff = mSig.GetReturnTypeSize();
        _ASSERTE(args.cbRetBuff <= ENREGISTERED_RETURNTYPE_MAXSIZE);
    }

    if (args.bHasNonStandardReturnVal)
    {
        _ASSERTE(args.bHasValueTypeReturnValToMarshal);
    }
#endif // defined(CALLDESCR_RETBUF)

    // Check for any call context
    BOOL bMarshalCallContext = FALSE;
    args.bMarshalReturnCallContext = FALSE;
    if (pCurThread->IsExposedObjectSet())
    {
        THREADBASEREF ref = (THREADBASEREF) pCurThread->GetExposedObjectRaw();
        _ASSERTE(ref != NULL);

        if (ref->GetExecutionContext() != NULL)
        {
            args.ClientGC.refExecutionContext = ref->GetExecutionContext();
            bMarshalCallContext = TRUE;

            args.ClientGC.refPrincipal = ReadPrincipal();
        }
    }


    // Make the Cross-AppDomain call
    {
        args.pThis = this;

        args.bOptimizable = TRUE;

        args.mSig  = &mSig;
        args.argit = &argit;

        args.dwNumArgs = dwNumArgs;
        args.pStackArgs  = pStackArgs;
        args.pRegArgs    = pRegArgs;
        args.pArgAttribs = pArgAttribs;

        args.dwNumObjectsMarshalled = dwNumObjectsMarshalled;
        args.bMarshalledArgs = bMarshalledArgs;
        args.pClientArgArray = pClientArgArray;

        args.pByRefArgAttribs = pByRefArgAttribs;
        args.pThByRefs        = pThByRefs;

        args.bMarshalCallContext = bMarshalCallContext;

        args.uRegTypeMap = *(UINT64*)pMap;

        MakeCallWithPossibleAppDomainTransition(m_pSrvDomain, (FPAPPDOMAINCALLBACK) MarshalAndCall_Wrapper2, &args);
    }

    if (args.ClientGC.refException != NULL)
    {
        RestorePrincipal(&args.ClientGC.refPrincipal);
        COMPlusThrow(args.ClientGC.refException);
    }

    if (args.bMarshalReturnCallContext)
    {
        THREADBASEREF ref = (THREADBASEREF) pCurThread->GetExposedObjectRaw();
        _ASSERTE(ref != NULL);

        ref->SetExecutionContext(args.ClientGC.refExecutionContext);
    }

    RestorePrincipal(&args.ClientGC.refPrincipal);

    // If the return type is an object, take it out of the protected ref
    if (args.bHasObjRefReturnVal)
    {
        args.returnVal = (ARG_SLOT)(size_t)OBJECTREFToObject(args.ClientGC.refReturnValue);
    }

    GCPROTECT_END();    // pClientArgArray
    GCPROTECT_END();    // args.ClientGC

    args.Marshaller.RemoveGCFrames();

#if defined(CALLDESCR_RETBUF) 
    // The return value from DispatchCall() is bogus when we have a non-standard return value on IA64.
    if (!args.bHasNonStandardReturnVal)
#endif // defined(CALLDESCR_RETBUF)
    {
        *pReturn = args.returnVal;
    }
    return args.bOptimizable;
}

#endif // defined(_X86_) || defined(_WIN64)

