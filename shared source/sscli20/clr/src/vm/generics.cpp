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
// File: generics.cpp
//
// Helper functions for generics prototype
//
#include "common.h"
#include "method.hpp"
#include "field.h"
#include "eeconfig.h"
#include "prettyprintsig.h"
#include "generics.h"
#include "genericdict.h"
#include "stackprobe.h"
#include "typestring.h"
#include "typekey.h"

// An arbitrary limit for class instantiations, used only to help detect
// recursive generic type definitions and avoid a stack overflow.
#define MAX_GENERIC_INSTANTIATION_DEPTH 100

#ifndef DACCESS_COMPILE 


/* static */ BOOL Generics::CheckInstantiation(DWORD nGenericArgs, TypeHandle *pGenericArgs)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END

#ifndef DACCESS_COMPILE 
    for (DWORD i = 0; i < nGenericArgs; i++)
    {
        TypeHandle th = pGenericArgs[i];
        if (th.IsNull())
        {
            return FALSE;
        }

        CorElementType type = th.GetSignatureCorElementType();
        if (type == ELEMENT_TYPE_BYREF
            || type == ELEMENT_TYPE_TYPEDBYREF
            || type == ELEMENT_TYPE_VOID
            || type == ELEMENT_TYPE_PTR
            || type == ELEMENT_TYPE_FNPTR)
        {
            return FALSE;
        }

        MethodTable* pMT = th.GetMethodTable();
        if (pMT != NULL && pMT->ContainsStackPtr())
        {
            return FALSE;
        }
    }
#endif // !DACCESS_COMPILE
    return TRUE;
}


BOOL Generics::CheckInstantiationForRecursion(const unsigned int nGenericClassArgs, const TypeHandle pGenericArgs[])
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    if (nGenericClassArgs == 0)
        return TRUE;

    _ASSERTE(pGenericArgs);

    struct PerIterationData {
        const TypeHandle * genArgs;
        int index;
        int numGenArgs;
    };
    
    PerIterationData stack[MAX_GENERIC_INSTANTIATION_DEPTH];
    stack[0].genArgs = pGenericArgs;
    stack[0].numGenArgs = nGenericClassArgs;
    stack[0].index = 0;
    int curDepth = 0;

    // Walk over each instantiation, doing a depth-first search looking for any
    // instantiation with a depth of over 100, in an attempt at flagging 
    // recursive type definitions.  We're doing this to help avoid a stack 
    // overflow in the loader.  
    // Avoid recursion here, to avoid a stack overflow.  Also, this code
    // doesn't allocate memory.
    while(curDepth >= 0) {
        PerIterationData * cur = &stack[curDepth];
        if (cur->index == cur->numGenArgs) {
            // Pop
            curDepth--;
            if (curDepth >= 0)
                stack[curDepth].index++;
            continue;
        }
        if (cur->genArgs[cur->index].HasInstantiation()) {
            // Push
            curDepth++;
            if (curDepth >= MAX_GENERIC_INSTANTIATION_DEPTH)
                return FALSE;
            stack[curDepth].genArgs = cur->genArgs[cur->index].GetInstantiation();
            stack[curDepth].numGenArgs = cur->genArgs[cur->index].GetNumGenericArgs();
            stack[curDepth].index = 0;
            continue;
        }
        
        // Continue to the next item
        cur->index++;
    }
    return TRUE;
}


// Create a non-canonical instantiation of a generic type, by
// copying the method table of the canonical instantiation
//
/* static */
TypeHandle ClassLoader::CreateTypeHandleForNonCanonicalGenericInstantiation(TypeKey *pTypeKey,
                                                                            AllocMemTracker *pamTracker)
{
    CONTRACT(TypeHandle)
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pTypeKey));
        PRECONDITION(CheckPointer(pamTracker));
        PRECONDITION(pTypeKey->HasInstantiation());
        PRECONDITION(TypeHandle::GetInstantiationSharingInfo(pTypeKey->GetNumGenericArgs(),
                                                             pTypeKey->GetInstantiation()) == TypeHandle::NonCanonicalShared);
        POSTCONDITION(CheckPointer(RETVAL));
        POSTCONDITION(RETVAL.CheckMatchesKey(pTypeKey));
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACT_END

    Module *pLoaderModule = ClassLoader::ComputeLoaderModule(pTypeKey);
    BaseDomain *pDomain = pLoaderModule->GetDomain();

    DWORD ntypars = pTypeKey->GetNumGenericArgs();
    TypeHandle *inst = pTypeKey->GetInstantiation();
    Module *pModule = pTypeKey->GetModule();
    mdTypeDef typeDef = pTypeKey->GetTypeToken();

    // Either we are NGEN'ing or we're in the right domain.
    _ASSERTE(pDomain->IsAppDomain() && pDomain->AsAppDomain()->IsCompilationDomain() ||
             pDomain == BaseDomain::ComputeBaseDomain(pModule->GetDomain(), ntypars, inst));

    MEMORY_REPORT_ASSEMBLY_SCOPE(pLoaderModule);

#ifdef _DEBUG 
    if (LoggingOn(LF_CLASSLOADER, LL_INFO1000))
    {
        StackSString debugTypeKeyName;
        TypeString::AppendTypeKeyDebug(debugTypeKeyName, pTypeKey);
        LOG((LF_CLASSLOADER, LL_INFO1000, "GENERICS: New instantiation requested: %S\n", debugTypeKeyName.GetUnicode()));
    }
#endif // _DEBUG

    // Canonicalize the type arguments.
    DWORD dwAllocSize = 0;
    if (!ClrSafeInt<DWORD>::multiply(ntypars, sizeof(TypeHandle), dwAllocSize))
        ThrowHR(COR_E_OVERFLOW);

    CQuickBytes qbRepInst;

    TypeHandle *repInst = reinterpret_cast<TypeHandle*>(qbRepInst.AllocThrows(dwAllocSize));
    for (DWORD i = 0; i < ntypars; i++)
    {
        repInst[i] = ClassLoader::LoadCanonicalGenericArg(inst[i], ClassLoader::LoadTypes, CLASS_LOAD_APPROXPARENTS);
    }

    // Load the canonical instantiation
    TypeKey canonKey(pModule, typeDef, ntypars, repInst);
    TypeHandle canonType = ClassLoader::LoadConstructedTypeThrowing(&canonKey, ClassLoader::LoadTypes, CLASS_LOAD_APPROXPARENTS);

    // Now fabricate a method table
    MethodTable* pOldMT = canonType.AsMethodTable();

    // We only need true vtable entries as the rest can be found in the representative method table
    DWORD cSlots = pOldMT->GetNumVirtuals();

    BOOL fDomainNeutral = pLoaderModule->GetAssembly()->IsDomainNeutral();
    BOOL fContainsGenericVariables = MethodTable::ComputeContainsGenericVariables(ntypars,inst);

    // These are all copied across from the old MT, i.e. don't depend on the
    // instantiation.
    BOOL fDynamicStatics = pOldMT->IsDynamicStatics();
#ifdef _DEBUG
    BOOL fHasRemotableMethodInfo = pOldMT->HasRemotableMethodInfo();
#endif // DEBUG
    BOOL fHasRemotingVtsInfo = pOldMT->HasRemotingVtsInfo();
    BOOL fHasCriticalFinalizer = pOldMT->HasCriticalFinalizer();
    BOOL fHasGenericsStaticsInfo = pOldMT->HasGenericsStaticsInfo();
    BOOL fHasClassConstructor = pOldMT->HasClassConstructor();
    BOOL fHasStaticOrContextStatics = pOldMT->HasThreadOrContextStatics();

    if (pOldMT->IsZapped())
    {
        for (DWORD j=0; j<cSlots; j++)
        {
            pOldMT->GetRestoredSlot(j);
        }
    }

    // The number of bytes used for GC info
    size_t cbGC = pOldMT->GetNumGCPointerSeries() ? ((CGCDesc*) pOldMT)->GetSize() : 0;

    // Bytes are required for the vtable itself
    S_SIZE_T safe_cbMT = S_SIZE_T( cbGC ) + S_SIZE_T( sizeof(MethodTable) );
    safe_cbMT += S_SIZE_T( cSlots ) * S_SIZE_T( sizeof(SLOT) );
    if ( safe_cbMT.IsOverflow() )
    {
        ThrowHR(COR_E_OVERFLOW);
    }
    const size_t cbMT = safe_cbMT.Value();

    // After the optional members (see below) comes the duplicated interface map.
    // For dynamic interfaces the interface map area begins one word
    // before the location returned by GetInterfaceMap()
    DWORD dwNumInterfaces = pOldMT->GetNumInterfaces();
    DWORD cbIMap = pOldMT->GetInterfaceMapSize();
    BYTE* pOldIMapMem = (BYTE*) pOldMT->GetInterfaceMap();


    // We need space for the optional members.
    DWORD cbOptional = pOldMT->GetOptionalMembersSize();

    // We need space for the PerInstInfo, i.e. the generic dictionary pointers...
    DWORD cbPerInst = pOldMT->GetPerInstInfoSize();

    // Finally we need space for the instantiation/dictionary for this type
    DWORD cbInstAndDict = pOldMT->GetInstAndDictSize();

    // Allocate from the high frequence heap of the correct domain
    S_SIZE_T allocSize = safe_cbMT;
    allocSize += cbOptional;
    allocSize += cbPerInst;
    allocSize += cbIMap;
    allocSize += cbInstAndDict;
    if( allocSize.IsOverflow() )
    {
        ThrowHR(COR_E_OVERFLOW);
    }
    BYTE* pMemory = (BYTE *) pamTracker->Track(pDomain->GetHighFrequencyHeap()->AllocMem( allocSize.Value() ));
    
    // Head of MethodTable memory
    MethodTable *pMT = (MethodTable*) (pMemory + cbGC);

    // Copy of GC
    memcpy((BYTE*)pMT - cbGC, (BYTE*) pOldMT - cbGC, cbGC);

    pMT->InitializeFlags(pOldMT->m_wFlags);


    DWORD wFlags2 = pOldMT->m_wFlags2;

    // Mark it as non-canonical
    wFlags2 &= ~MethodTable::enum_flag2_GenericsMask;
    wFlags2 |= MethodTable::enum_flag2_GenericsMask_NonCanonInst;

    // Freshly allocated - does not need restore
    wFlags2 &= ~(MethodTable::enum_flag2_IsZapped | MethodTable::enum_flag2_MayNeedRestore);

    pMT->InitializeFlags2(wFlags2);


    pMT->SetBaseSize(pOldMT->GetBaseSize());
    pMT->SetParentMethodTable(pOldMT->GetParentMethodTable());
    pMT->SetClass(pOldMT->GetClass());

    pMT->m_wNumInterfaces = pOldMT->m_wNumInterfaces;
    pMT->m_pDispatchMap = pOldMT->m_pDispatchMap;

    // Number of slots only includes vtable slots
    pMT->SetNumMethods (cSlots);
    pMT->SetNumVirtuals(cSlots);

    // Copy of vtable slots
    memcpy((BYTE*)pMT + sizeof(MethodTable), (BYTE*) pOldMT + sizeof(MethodTable), cSlots * sizeof(SLOT));
   
    // Allocate the private data block ("private" during runtime in the ngen'ed case)
    BYTE* pMTWriteableData = (BYTE *) pamTracker->Track(pDomain->GetHighFrequencyHeap()->AllocMem(sizeof(MethodTableWriteableData)));
    // Note: Memory allocated on loader heap is zero filled
    pMT->SetWriteableData((PTR_MethodTableWriteableData)pMTWriteableData);

    // All flags on m_pNgenPrivateData data apart
    // are initially false for a dynamically generated instantiation.
    //
    // Last time this was checked this included
    //    enum_flag_RemotingConfigChecked
    //    enum_flag_RequiresManagedActivation
    //    enum_flag_Unrestored
    //    enum_flag_CriticalTypePrepared

#ifdef _DEBUG 
    if (pOldMT->IsContextful() || pOldMT->GetClass()->HasRemotingProxyAttribute())
    {
        _ASSERTE(pOldMT->RequiresManagedActivation());
    }
#endif // _DEBUG
    if (pOldMT->RequiresManagedActivation())
    {
        // Will also set enum_flag_RemotingConfigChecked
        pMT->SetRequiresManagedActivation();
    }

    pMT->SetCompiledDomainNeutral(fDomainNeutral);

    if (fContainsGenericVariables)
        pMT->SetContainsGenericVariables();

    if (fHasGenericsStaticsInfo)
        pMT->SetHasGenericsStaticsInfo();

    if (fHasRemotingVtsInfo)
        pMT->SetHasRemotingVtsInfo();

    if (fHasStaticOrContextStatics)
        pMT->SetHasThreadOrContextStatics();

    // Set the ClassInited flag to its correct initial value.
    if (MethodTable::ComputeIsPreInit(fContainsGenericVariables,
                                      fHasClassConstructor,
                                      (pOldMT->GetNumBoxedStatics() > 0),
                                      fDynamicStatics))
    {
        pMT->SetClassPreInited();
    }

    // Since we are fabricating a new MT based on an existing one, the per-inst info should
    // be non-null
    _ASSERTE(pOldMT->GetPerInstInfo());

    // Fill in per-inst map pointer (which points to the array of generic dictionary pointers)
    pMT->SetPerInstInfo ((Dictionary**) (pMemory + cbMT + cbOptional));
    pMT->SetDictInfo(pOldMT->GetNumDicts(), pOldMT->GetNumGenericArgs());
    pMT->SetIsNotFullyLoaded();
    pMT->SetHasApproxParent();

    // Fill in the last entry in the array of generic dictionary pointers ("per inst info")
    // The others are filled in by LoadExactParents which copied down any inherited generic
    // dictionary pointers.
    *(pMT->GetPerInstInfo() + (pOldMT->GetNumDicts()-1)) = (Dictionary*) (pMemory + cbMT + cbOptional + cbPerInst + cbIMap);



    // Fill in the instantiation section of the generic dictionary.  The remainder of the
    // generic dictionary will be zeroed, which is the correct initial state.
    memcpy(pMT->GetInstantiation(), inst, ntypars * sizeof(TypeHandle));

    // Restore interface map, as it might contain pointers that are encoded relative to another zap module
    MethodTable::InterfaceMapIterator intIt = pOldMT->IterateInterfaceMap();
    while (intIt.Next()) {
        intIt.GetInterface();
    }

    // Copy interface map across
    memcpy(pMemory + cbMT + cbOptional + cbPerInst, pOldIMapMem, cbIMap);


    // Set the interface map pointer stored in the main section of the vtable (actually
    // an optional member) to point to the correct region within the newly
    // allocated method table.
    BYTE *pInterfaceMap = pMemory + cbMT + cbOptional + cbPerInst;
    // Fill in interface map pointer
    pMT->SetInterfaceMap(dwNumInterfaces, (InterfaceInfo_t *) pInterfaceMap);

    // Zero out guid info (regenerate on demand)
    if (pOldMT->IsInterface())
        pMT->SetGuidInfo (NULL);

    // Set up the other optional members
    if (fHasCriticalFinalizer)
        pMT->SetHasCriticalFinalizer();

    pMT->SetLoaderModule(pLoaderModule);

    // Name for debugging
#ifdef _DEBUG 
    StackSString debugName;
    TypeString::AppendTypeKey(debugName, pTypeKey, TypeString::FormatNamespace | TypeString::FormatAngleBrackets | TypeString::FormatFullInst);
    StackScratchBuffer buff;
    const char* pDebugNameUTF8 = debugName.GetUTF8(buff);
    size_t len = strlen(pDebugNameUTF8)+1;
    char *name = (char*) pamTracker->Track(pDomain->GetLowFrequencyHeap()->AllocMem(len));
    strcpy_s(name, len, pDebugNameUTF8);
    pMT->SetDebugClassName(name);
#endif // _DEBUG

    // <NICE>This logic is identical to logic in class.cpp.  Factor these out.</NICE>
    // No need to generate IDs for open types.   However
    // we still leave the optional member in the MethodTable holding the value -1 for the ID.
    if (fHasGenericsStaticsInfo)
    {
        FieldDesc* pStaticFieldDescs = NULL;

        if (pOldMT->GetNumStaticFields() != 0)
        {
            pStaticFieldDescs = (FieldDesc*) pamTracker->Track(pDomain->GetLowFrequencyHeap()->AllocMem(sizeof(FieldDesc) * pOldMT->GetNumStaticFields()));
            FieldDesc* pOldFD = pOldMT->GetGenericsStaticFieldDescs();

            for (DWORD i = 0; i < pOldMT->GetNumStaticFields(); i++)
            {
                pStaticFieldDescs[i] = pOldFD[i];
                pStaticFieldDescs[i].SetMethodTable(pMT);
            }
        }

        pMT->SetupGenericsStaticsInfo(pStaticFieldDescs);

    }

    // Make fresh RMI: the results of RMI analysis depend on the exact instantiation.
    if (pOldMT->HasRemotableMethodInfo())
    {
        pMT->SetupRemotableMethodInfo(pDomain, pamTracker);
    }

    // VTS info doesn't depend on the exact instantiation but we make a copy
    // anyway since we can't currently deal with the possibility of having a
    // cross module pointer to the data block. Eventually we might be able to
    // tokenize this reference, but determine first whether there's enough
    // performance degradation to justify the extra complexity.
    if (fHasRemotingVtsInfo)
    {
        RemotingVtsInfo *pOldInfo = pOldMT->GetRemotingVtsInfo();
        DWORD            cbInfo   = RemotingVtsInfo::GetSize(pOldMT->GetNumIntroducedInstanceFields());
        RemotingVtsInfo *pNewInfo = (RemotingVtsInfo*)pamTracker->Track(pDomain->GetLowFrequencyHeap()->AllocMem(cbInfo));

        memcpyNoGCRefs(pNewInfo, pOldInfo, cbInfo);

        *(pMT->GetRemotingVtsInfoPtr()) = pNewInfo;
    }

    // if there are thread or context static make room for them there is no sharing with the other MethodTable
    if (fHasStaticOrContextStatics)
    {
        // this is responsible for setting the flag and allocation in the loader heap
        pMT->SetupThreadOrContextStatics(pamTracker, pOldMT->GetThreadStaticsSize(), pOldMT->GetContextStaticsSize());
    }

    // Check we've set up the flags correctly on the new method table
    _ASSERTE(!fContainsGenericVariables == !pMT->ContainsGenericVariables());
    _ASSERTE(!fDynamicStatics == !pMT->IsDynamicStatics());
    _ASSERTE(!fHasCriticalFinalizer == !pMT->HasCriticalFinalizer());
    _ASSERTE(!fHasGenericsStaticsInfo == !pMT->HasGenericsStaticsInfo());
    _ASSERTE(!fDomainNeutral == !pMT->IsDomainNeutral());
    _ASSERTE(!fHasRemotableMethodInfo == !pMT->HasRemotableMethodInfo());
    _ASSERTE(!fHasRemotingVtsInfo == !pMT->HasRemotingVtsInfo());
    _ASSERTE(!fHasClassConstructor == !pMT->HasClassConstructor());

    LOG((LF_CLASSLOADER, LL_INFO1000, "GENERICS: Replicated methodtable to create type %s\n", pMT->GetDebugClassName()));

    RETURN(TypeHandle(pMT));
}

// Find the exact enclosing type for MethodDescs that are potentially
// shared between various generic instantiations.
//
// Given the exact type owning a method (which might be subclass of the
// type where the method is declared), return the corresponding exact type
// for the class where  method is "declared".
//
// Example 1: Non-generic method descriptor:
//    class C { void m(); }
//    class D
// Here GetMethodDeclaringType(D,C::m) will just return C.
//
// Example 2: Non-shared method descriptor in a generic class:
//    class C<T> { void m(); }
//    class D : C<int>
// then GetMethodDeclaringType(D,C<int>::m) will just return C<int.
//
// Example 3a: Shared method descriptor in a generic class:
//    class C<T> { void m(); }
//    class D : C<string>
// then GetMethodDeclaringType(D,C<object>::m) will return C<string>.
//
// Example 3b: Shared method descriptor in a generic class:
//    class C<T> { void m(); }
//    class D<T> : C<T>
// then GetMethodDeclaringType(D<string>,C<object>::m) will return C<string>.
//
// allowCreate may be set to FALSE to enforce that the type to return
// should already be in existence - thus preventing creation and GCs during 
// inappropriate times.
TypeHandle Generics::GetMethodDeclaringType(TypeHandle owner, MethodDesc *pMD, BOOL allowCreate /* = TRUE */)
{

    CONTRACT(TypeHandle)
    {
        WRAPPER(THROWS);
        if (allowCreate) { WRAPPER(GC_TRIGGERS); } else { GC_NOTRIGGER; }
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, (allowCreate ? NULL_NOT_OK : NULL_OK)));
    }
    CONTRACT_END

    if (!pMD->HasClassInstantiation())
    {
        RETURN(TypeHandle(pMD->GetMethodTable()));
    }
    else if (pMD->GetMethodTable()->HasSameTypeDefAs(owner.GetMethodTable()))
    {
        RETURN(owner);
    }
    else
    {
        TypeHandle *inst = pMD->GetExactClassInstantiation(owner);
        RETURN(ClassLoader::LoadGenericInstantiationThrowing(
                        pMD->GetModule(), 
                        pMD->GetMethodTable()->GetCl(),
                        pMD->GetNumGenericClassArgs(),
                        inst,
                        allowCreate ? ClassLoader::LoadTypes : ClassLoader::DontLoadTypes));
    }
}

// Similarly for field descriptors.
TypeHandle Generics::GetFieldDeclaringType(TypeHandle owner, FieldDesc *pFD)
{

    CONTRACT(TypeHandle)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pFD));
        PRECONDITION(CheckPointer(owner, NULL_OK));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END

    MethodTable *pMT = pFD->GetApproxEnclosingMethodTable();
    if (!pMT->HasInstantiation())
        RETURN(TypeHandle(pMT));
    else
    {
        TypeHandle *inst = pFD->GetExactClassInstantiation(owner);

        RETURN(ClassLoader::LoadGenericInstantiationThrowing(pMT->GetModule(), pMT->GetCl(), pMT->GetNumGenericArgs(), inst));
    }
}


// Given the shared EEClass for an instantiated type, search its dictionary layout
// for a particular (annotated) token.
// If there's no space in the current layout, grow the layout and the actual dictionaries
// of all compatible types.
// Return a sequence of offsets into dictionaries
WORD Generics::FindClassDictionaryToken(EEClass *pClass, DictionaryEntryLayout *pEntryLayout, WORD *offsets)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pClass));
        PRECONDITION(pClass->HasInstantiation());
        PRECONDITION(CheckPointer(offsets));
    }
    CONTRACTL_END

    WORD nindirections = DictionaryLayout::FindToken(pClass->GetDomain(),
                                                     pClass->GetNumGenericArgs(),
                                                     pClass->GetDictionaryLayout(),
                                                     FALSE/*isMethodDict*/,
                                                     pClass->GetModule(),
                                                     pEntryLayout,
                                                     offsets
#ifdef _DEBUG 
                                                     , pClass->m_szDebugClassName
#endif
                                                     );

#ifdef _DEBUG 
    InlineSString<100> slotstr;
    if (nindirections == CORINFO_USEHELPER)
        slotstr.AppendASCII("<use-helper>");
    else
    {
        for (DWORD j = 0; j < nindirections; j++)
            slotstr.AppendPrintf("%d", offsets[j]);
    }
    LOG((LF_JIT, LL_INFO1000, "GENERICS: Found/allocated slot at position %S for token %x/%x in class dictionary for %s\n",
        slotstr.GetUnicode(),
        pEntryLayout->m_token1,
        pEntryLayout->m_token2,
        pClass->GetDebugClassName()));
#endif // _DEBUG

    return nindirections;
}

#endif // !DACCESS_COMPILE

/*
 * GetExactInstantiationsFromCallInformation
 *
 * This routine takes in the various pieces of information of a call site to managed code
 * and returns the exact instatiations for the class and method.
 *
 * Parameters:
 *    pRepMethod - A MethodDesc to the representative instantiation method.
 *    pThis - The OBJECTREF that is being passed to pRepMethod.
 *    paramTypeArg - The extra argument passed to pRepMethod when pRepMethod is either
 *       RequiresInstMethodTableArg() or RequiresInstMethodDescArg().
 *    pSpecificClass - A pointer to a TypeHandle for storing the result.
 *    pSpecificMethod - A pointer to a MethodDesc* for storing 
 *
 * Returns:
 *    TRUE if successful.
 *    FALSE if could not get the TypeHandles for the method type parameters.  Note that in
 *      this case the SpecificClass may be correct, iff the class is not a generic class.
 *
 */
BOOL Generics::GetExactInstantiationsFromCallInformation(
    /* in */  MethodDesc *pRepMethod,
    /* in */  OBJECTREF pThis,
    /* in */  void *paramTypeArg,
    /* out*/  TypeHandle *pSpecificClass,
    /* out*/  MethodDesc** pSpecificMethod
    )
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pRepMethod));
    }
    CONTRACTL_END;
     
    void* pExactGenericArgsToken = NULL;

    if (pRepMethod->AcquiresInstMethodTableFromThis())
    {
        if (pThis != NULL)
            pExactGenericArgsToken = pThis->GetMethodTable();
    }
    else        
    {
        pExactGenericArgsToken = paramTypeArg;
    }

    return GetExactInstantiationsFromCallInformation(pRepMethod, pExactGenericArgsToken, 
        pSpecificClass, pSpecificMethod);      
}

BOOL Generics::GetExactInstantiationsFromCallInformation(
    /* in */  MethodDesc *pRepMethod,
    /* in */  void *pExactGenericArgsToken,
    /* out*/  TypeHandle *pSpecificClass,
    /* out*/  MethodDesc** pSpecificMethod
    )
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pRepMethod));
    }
    CONTRACTL_END;

    //
    // Start with some decent default values.
    //
    *pSpecificClass = TypeHandle(pRepMethod->GetMethodTable());
    *pSpecificMethod = pRepMethod;

    if (!pRepMethod->IsSharedByGenericInstantiations())
    {
        return TRUE;
    }

    if (!pExactGenericArgsToken)
    {
        return FALSE;
    }

    if (pRepMethod->RequiresInstMethodTableArg())
    {
        MethodTable *mt = (MethodTable *) pExactGenericArgsToken;
        *pSpecificClass = TypeHandle(mt);
        return TRUE;
    }

    if (pRepMethod->RequiresInstMethodDescArg())
    {
        MethodDesc *md = (MethodDesc *) pExactGenericArgsToken;
        *pSpecificClass = TypeHandle(md->GetMethodTable());
        *pSpecificMethod = md;
        return TRUE;
    }

    if (pRepMethod->AcquiresInstMethodTableFromThis())
    {
        MethodTable *mt = (MethodTable *) pExactGenericArgsToken;
        *pSpecificClass = TypeHandle(mt);
        return TRUE;
    }

    _ASSERTE(!"Should not happen.");
    return TRUE;
}
