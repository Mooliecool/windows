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
// File: genmeth.cpp
//
// Most functionality for generic methods is put here
//

#include "common.h"
#include "method.hpp"
#include "field.h"
#include "eeconfig.h"
#include "prettyprintsig.h"
#include "perfcounters.h"
#include "crst.h"
#include "generics.h"
#include "genericdict.h"
#include "instmethhash.h"
#include "typestring.h"
#include "typedesc.h"

// Instantiated generic methods
//
// Method descriptors for instantiated generic methods are allocated on demand and inserted
// into the InstMethodHashTable for the LoaderModule of the descriptor. (See ceeload.h for more
// information about loader modules).
//
// For non-shared instantiations, entering the pretub for such a method descriptor causes the method to
// be JIT-compiled, specialized to that instantiation.
//
// For shared instantiations, entering the prestub generates a piece of stub code that passes the
// method descriptor as an extra argument and then jumps to code shared between compatible
// instantiations. This code has its own method descriptor whose instantiation is *canonical*
// (with reference-type type parameters replaced by Object).
//
// Thus for example the shared method descriptor for m<object> is different to the
// exact-instantiation method descriptor for m<object>.
//
// Complete example:
//
// class C<T> { public void m<S>(S x, T y) { ... } }
//
// Suppose that code sharing is turned on.
//
// Upon compiling calls to C<string>.m<string>, C<string>.m<Type>, C<Type>.m<string> and C<Type>.m<Type>

// Given a generic method descriptor and an instantiation, create a new instantiated method
// descriptor and chain it into the list attached to the generic method descriptor
//
// pMT is the owner method table.  If looking for a shared MD this should be
// the MT for the shared class.
//
// pGenericMD is the generic method descriptor (owner may be instantiated)
// pWrappedMD is the corresponding shared  md for use when creating stubs
// nGenericMethodArgs/genericMethodArgs is the instantiation
// getWrappedCode=TRUE if you want a shared instantiated md whose code expects an extra argument.  In this
// case pWrappedMD should be NULL.
//
// The result is put in ppMD
//
// If getWrappedCode.  In thise case the genericMethodArgs
// should be the normalized representative genericMethodArgs (see typehandle.h)
//


// Helper method that creates a method-desc off a template method desc
static MethodDesc* CreateMethodDesc(BaseDomain *pDomain,
                                    MethodTable *pMT,
                                    MethodDesc *pTemplateMD,
                                    DWORD classification,
                                    AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pDomain));
        PRECONDITION(CheckPointer(pMT));
        PRECONDITION(CheckPointer(pTemplateMD));
        PRECONDITION(pTemplateMD->IsRestored());
        PRECONDITION(pMT->IsRestored());
        PRECONDITION(pTemplateMD->GetMethodTable()->GetCanonicalMethodTable() == pMT->GetCanonicalMethodTable());
    }
    CONTRACTL_END

    mdMethodDef token = pTemplateMD->GetMemberDef();

    // Create a singleton chunk for the method desc
    MethodDescChunk *pChunk =
        MethodDescChunk::CreateChunk(pDomain->GetHighFrequencyHeap(),
                                     1,
                                     classification,
                                     ::GetTokenRange(token),
                                     pMT,
                                     pamTracker);


    // Now initialize the MDesc at the single method descriptor in
    // the new chunk
    MethodDesc *pMD = pChunk->GetFirstMethodDesc();

    //We copy over the flags one by one.  This is fragile w.r.t. adding
    // new flags, but other techniques are also fragile.  <NICE>We should move
    // to using constructors on MethodDesc</NICE>
    if (pTemplateMD->IsStatic())
    {
        pMD->SetStatic();
    }
     if (pTemplateMD->IsNotInline())
     {
         pMD->SetNotInline(true);
     }
     if (pTemplateMD->IsSynchronized())
     {
         pMD->SetSynchronized();
     }
     if (pTemplateMD->RequiresLinktimeCheck())
     {
         pMD->SetRequiresLinktimeCheck();
     }
     if (pTemplateMD->RequiresInheritanceCheck())
     {
         pMD->SetRequiresInheritanceCheck();
     }
     if (pTemplateMD->ParentRequiresInheritanceCheck())
     {
         pMD->SetParentRequiresInheritanceCheck();
     }
     if (pTemplateMD->ParentRequiresInheritanceCheck())
     {
         pMD->SetParentRequiresInheritanceCheck();
     }
     if (pTemplateMD->IsInterceptedForDeclSecurity())
     {
         pMD->SetInterceptedForDeclSecurity(TRUE);
     }
     if (pTemplateMD->IsInterceptedForDeclSecurityCASDemandsOnly())
     {
         pMD->SetInterceptedForDeclSecurityCASDemandsOnly(TRUE);
     }

    pMD->SetMemberDef(token);
    pMD->SetSlot(pTemplateMD->GetSlot());


#ifdef _DEBUG 
    pMD->m_pszDebugMethodName = pTemplateMD->m_pszDebugMethodName;
    pMD->InitPrestubCallChecking();
    //<NICE> more info here</NICE>
    pMD->m_pszDebugMethodSignature = "<generic method signature>";
    pMD->m_pszDebugClassName  = "<generic method class name>";
    pMD->m_pszDebugMethodName = "<generic method name>";
    pMD->m_pDebugMethodTable  = pMT;
#endif // _DEBUG

    return pMD;
}

/* static */
InstantiatedMethodDesc *
InstantiatedMethodDesc::NewInstantiatedMethodDesc(MethodTable *pExactMT,
                                                  MethodDesc* pGenericMDescInRepMT,
                                                  MethodDesc* pWrappedMD,
                                                  DWORD nGenericMethodArgs,
                                                  TypeHandle *genericMethodArgs,
                                                  BOOL getWrappedCode)
{
    CONTRACT(InstantiatedMethodDesc*)
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pExactMT));
        PRECONDITION(CheckPointer(pGenericMDescInRepMT));
        PRECONDITION(pGenericMDescInRepMT->IsRestored());
        PRECONDITION(pWrappedMD == NULL || pWrappedMD->IsRestored());
        PRECONDITION(nGenericMethodArgs == 0 || pGenericMDescInRepMT->IsGenericMethodDefinition());
        PRECONDITION(nGenericMethodArgs == pGenericMDescInRepMT->GetNumGenericMethodArgs());
        PRECONDITION(nGenericMethodArgs == 0 || genericMethodArgs != NULL);
        POSTCONDITION(CheckPointer(RETVAL));
        POSTCONDITION(RETVAL->IsRestored());
        POSTCONDITION(getWrappedCode == RETVAL->IsSharedByGenericInstantiations());
        POSTCONDITION(nGenericMethodArgs == 0 || RETVAL->HasMethodInstantiation());
    }
    CONTRACT_END;

    // All instantiated method descs live off the RepMT for the
    // instantiated class they live in.
    INDEBUG(MethodTable *pCanonMT = pExactMT->GetCanonicalMethodTable();)

    _ASSERTE(pGenericMDescInRepMT->GetMethodTable() == pCanonMT);

    if (getWrappedCode)
    {
        _ASSERTE(pWrappedMD == NULL);
        _ASSERTE(pExactMT->IsCanonicalMethodTable());
        _ASSERTE(pCanonMT == pExactMT);
        // This always holds but we no longer check it
		//_ASSERTE(TypeHandle::CheckInstantiationIsCanonical(nGenericMethodArgs,genericMethodArgs));
        _ASSERTE(pExactMT->IsSharedByGenericInstantiations() || TypeHandle::IsSharableInstantiation(nGenericMethodArgs, genericMethodArgs));

    }

    InstantiatedMethodDesc *pNewMD;
    Module *pExactMDLoaderModule = ClassLoader::ComputeLoaderModule(pExactMT, pGenericMDescInRepMT->GetMemberDef(), genericMethodArgs, nGenericMethodArgs);

    BaseDomain *pDomain = pExactMDLoaderModule->GetDomain();

    {
        // Acquire crst to prevent tripping up other threads searching in the same hashtable
        CrstHolder ch(&pExactMDLoaderModule->m_InstMethodHashTableCrst);

        // Check whether another thread beat us to it!
        pNewMD = FindLoadedInstantiatedMethodDesc(pExactMT,
                                                  pGenericMDescInRepMT->GetMemberDef(),
                                                  nGenericMethodArgs,
                                                  genericMethodArgs,
                                                  getWrappedCode);

        // Crst goes out of scope here
        // We don't need to hold the crst while we build the MethodDesc, but we reacquire it later
    }
    

    if (pNewMD != NULL)
    {
        pNewMD->CheckRestore();
    }
    else
    {
        TypeHandle *pInstOrPerInstInfo = NULL;
        DWORD infoSize = 0;
        if (nGenericMethodArgs != 0)
        {
            BOOL hasDict = pWrappedMD && pWrappedMD->IsSharedByGenericMethodInstantiations();
            DictionaryLayout *pDL = hasDict ? pWrappedMD->AsInstantiatedMethodDesc()->m_pDictLayout : NULL;
            // Allocate space for the instantiation and dictionary
            infoSize = DictionaryLayout::GetFirstDictionaryBucketSize(nGenericMethodArgs, pDL);
            pInstOrPerInstInfo = (TypeHandle *) (void*) pDomain->GetHighFrequencyHeap()->AllocMem(infoSize);
            memcpy(pInstOrPerInstInfo, genericMethodArgs, sizeof(TypeHandle) * nGenericMethodArgs);
        }

        AllocMemTracker amt;


        // Create a new singleton chunk for the new instantiated method descriptor
        // Notice that we've passed in the method table pointer; this gets
        // used in some of the subsequent setup methods for method descs.
        //
        pNewMD = (InstantiatedMethodDesc*) (CreateMethodDesc(pDomain,
                                                             pExactMT,
                                                             pGenericMDescInRepMT,
                                                             mcInstantiated,
                                                             &amt));

        // Initialize the MD the way it needs to be
        if (pWrappedMD)
        {
            pNewMD->SetupWrapperStubWithInstantiations(pWrappedMD, nGenericMethodArgs, pInstOrPerInstInfo);
            _ASSERTE(pNewMD->IsInstantiatingStub());
        }
        else if (getWrappedCode)
        {
            pNewMD->SetupSharedMethodInstantiation(nGenericMethodArgs, pInstOrPerInstInfo);
            _ASSERTE(!pNewMD->IsInstantiatingStub());
        }
        else
        {
            pNewMD->SetupUnsharedMethodInstantiation(nGenericMethodArgs, pInstOrPerInstInfo);
        }

        // Check that whichever field holds the inst. got setup correctly
        _ASSERTE(pNewMD->GetMethodInstantiation() == pInstOrPerInstInfo);

        if (pNewMD->ComputeMayHaveNativeCode())
            pNewMD->SetMayHaveNativeCode();

        pNewMD->SetTemporaryEntryPoint(pDomain, &amt);

        {
            // The canonical instantiation is exempt from constraint checks. It's used as the basis
            // for all other reference instantiations so we can't not load it. The Canon type is
            // not visible to users so it can't be abused.
            BOOL fExempt = FALSE;
            for (DWORD arg = 0; arg < nGenericMethodArgs; arg++)
            {
                if (pInstOrPerInstInfo[arg].GetTypeArgumentSharingInfo() == TypeHandle::CanonicalShared)
                {
                    fExempt = TRUE;
                    break;
                }
            }

            TypeHandle *classInst = pNewMD->GetClassInstantiation();
            DWORD numGenericClassArgs = pNewMD->GetNumGenericClassArgs();
            for (DWORD classarg = 0; classarg < numGenericClassArgs; classarg++)
            {
                if (classInst[classarg].GetTypeArgumentSharingInfo() == TypeHandle::CanonicalShared)
                {
                    fExempt = TRUE;
                    break;
                }
            }
            if (!fExempt)
            {
                pNewMD->SatisfiesMethodConstraints(TypeHandle(pExactMT), TRUE);
            }
        }

        // OK, now we have a candidate MethodDesc.
        {
            CrstHolder ch(&pExactMDLoaderModule->m_InstMethodHashTableCrst);

            // We checked before, but make sure again that another thread didn't beat us to it!
            InstantiatedMethodDesc *pOldMD = FindLoadedInstantiatedMethodDesc(pExactMT,
                                                      pGenericMDescInRepMT->GetMemberDef(),
                                                      nGenericMethodArgs,
                                                      genericMethodArgs,
                                                      getWrappedCode);

            if (pOldMD == NULL)
            {
                // No one else got there first, our MethodDesc wins.
                amt.SuppressRelease();

#ifdef _DEBUG 
                SString name(SString::Utf8);
                TypeString::AppendMethodDebug(name, pNewMD);
                StackScratchBuffer buff;
                const char* pDebugNameUTF8 = name.GetUTF8(buff);
                const char* verb = "Created";
                if (pWrappedMD)
                    LOG((LF_CLASSLOADER, LL_INFO1000,
                         "GENERICS: %s instantiating-stub method desc %s with dictionary size %d\n",
                         verb, pDebugNameUTF8, infoSize));
                else
                    LOG((LF_CLASSLOADER, LL_INFO1000,
                         "GENERICS: %s instantiated method desc %s\n",
                         verb, pDebugNameUTF8));

                size_t len = strlen(pDebugNameUTF8)+1;
                pNewMD->m_pszDebugMethodName = (char*) (void*)pDomain->GetLowFrequencyHeap()->AllocMem(len);
                _ASSERTE(pNewMD->m_pszDebugMethodName);
                strcpy_s((char *) pNewMD->m_pszDebugMethodName, len, pDebugNameUTF8);
                pNewMD->m_pszDebugClassName = pExactMT->GetDebugClassName();
                pNewMD->m_pszDebugMethodSignature = (LPUTF8)pNewMD->m_pszDebugMethodName;
                pNewMD->InitPrestubCallChecking();
#endif // _DEBUG

                // This check need comes after as calling IsVarArg can't be done until the chunk is linked in
                if (pNewMD->IsVarArg())
                {
                     COMPlusThrow(kInvalidProgramException, BFA_GENCODE_NOT_BE_VARARG);
                }

                // The method desc is fully set up; now add to the table
                InstMethodHashTable* pTable = pExactMDLoaderModule->GetInstMethodHashTable();
                pTable->InsertMethodDesc(pNewMD);

            }
            else
                pNewMD = pOldMD;
            // CrstHolder goes out of scope here
        }
        
    }

    RETURN pNewMD;
}

// N.B. it is not guarantee that the returned InstantiatedMethodDesc is restored.
// It is the caller's responsibility to call CheckRestore on the returned value.
/* static */
InstantiatedMethodDesc*
InstantiatedMethodDesc::FindLoadedInstantiatedMethodDesc(MethodTable *pExactOrRepMT,
                                                         mdMethodDef methodDef,
                                                         DWORD nGenericMethodArgs,
                                                         TypeHandle *genericMethodArgs,
                                                         BOOL getWrappedCode)
{
    CONTRACT(InstantiatedMethodDesc *)
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        PRECONDITION(CheckPointer(pExactOrRepMT));
        PRECONDITION(nGenericMethodArgs == 0 || CheckPointer(genericMethodArgs));

        // All wrapped method descriptors (except BoxedEntryPointStubs, which don't use this path) are
        // canonical and exhibit some kind of code sharing.
        PRECONDITION(!getWrappedCode || pExactOrRepMT->IsCanonicalMethodTable());
        PRECONDITION(!getWrappedCode || pExactOrRepMT->IsSharedByGenericInstantiations() || TypeHandle::IsSharableInstantiation(nGenericMethodArgs, genericMethodArgs));

        // Unboxing stubs are dealt with separately in FindOrCreateAssociatedMethodDesc.  This should
        // probably be streamlined...
        POSTCONDITION(!RETVAL || !RETVAL->IsUnboxingStub());

        // All wrapped method descriptors (except BoxedEntryPointStubs, which don't use this path) take an inst arg.
        // The only ones that don't should have been found in the type's meth table.
        POSTCONDITION(!getWrappedCode || !RETVAL || !RETVAL->IsRestored() || RETVAL->RequiresInstArg());
    }
    CONTRACT_END


    // First look in the table for the runtime loader module in case someone created it before any
    // zap modules got loaded
    Module *pLoaderModule = ClassLoader::ComputeLoaderModule(pExactOrRepMT, methodDef, genericMethodArgs, nGenericMethodArgs);

    InstMethodHashTable* pTable = pLoaderModule->GetInstMethodHashTable();
    MethodDesc *resultMD = pTable->FindMethodDesc(TypeHandle(pExactOrRepMT),
                                                  methodDef,
                                                  FALSE /* not forceBoxedEntryPoint */,
                                                  nGenericMethodArgs,
                                                  genericMethodArgs,
                                                  getWrappedCode);

    if (resultMD != NULL)
       RETURN((InstantiatedMethodDesc*) resultMD);


    RETURN(NULL);
}


//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//

/* static */
MethodDesc*
MethodDesc::FindOrCreateAssociatedMethodDesc(MethodDesc* pDefMD,
                                             MethodTable *pExactMT,
                                             BOOL forceBoxedEntryPoint,
                                             DWORD nGenericMethodArgs,
                                             TypeHandle *genericMethodArgs,
                                             BOOL allowInstParam,
                                             BOOL forceRemotableMethod,
                                             BOOL allowCreate,
                                             ClassLoadLevel level)
{
    CONTRACT(MethodDesc*)
    {
        THROWS;
        if (allowCreate) { GC_TRIGGERS; } else { GC_NOTRIGGER; }
        INJECT_FAULT(COMPlusThrowOM(););

        PRECONDITION(CheckPointer(pDefMD));
        PRECONDITION(CheckPointer(pExactMT));
        PRECONDITION(pDefMD->IsRestored());
        PRECONDITION(pExactMT->IsRestored());

        // If the method descriptor belongs to a generic type then
        // the input exact type must be an instantiation of that type.
        // DISABLED PRECONDITION - too strict - see akenn or crusso the classes may be in
        // a subtype relation to each other.
        //
        // PRECONDITION(!pDefMD->HasClassInstantiation() || pDefMD->GetMethodTable()->HasSameTypeDefAs(pExactMT));

        // You may only request an BoxedEntryPointStub for an instance method on a value type
        PRECONDITION(!forceBoxedEntryPoint || pExactMT->IsValueClass());
        PRECONDITION(!forceBoxedEntryPoint || !pDefMD->IsStatic());

        // For remotable methods we better not be allowing instantiation parameters.
        PRECONDITION(!forceRemotableMethod || !allowInstParam);

        POSTCONDITION(((RETVAL == NULL) && !allowCreate) || CheckPointer(RETVAL));
        POSTCONDITION(((RETVAL == NULL) && !allowCreate) || RETVAL->IsRestored());
        POSTCONDITION(((RETVAL == NULL) && !allowCreate) || forceBoxedEntryPoint || !RETVAL->IsUnboxingStub());
        POSTCONDITION(((RETVAL == NULL) && !allowCreate) || allowInstParam || !RETVAL->RequiresInstArg());
    }
    CONTRACT_END;

    // Quick exit for the common cases where the result is the same as the primary MD we are given
    DWORD dwNumGenericMethodArgs = pDefMD->GetNumGenericMethodArgs();
    if (!pDefMD->HasClassOrMethodInstantiation() &&
        dwNumGenericMethodArgs == 0 &&
        (allowInstParam || !pDefMD->RequiresInstArg()) &&
        !forceBoxedEntryPoint &&
        !pDefMD->IsUnboxingStub())
        RETURN pDefMD;

    // Get the version of the method desc. for the instantiated shared class, e.g.
    //  e.g. if pDefMD == List<T>.m()
    //          pExactMT = List<string>
    //     then pMDescInCanonMT = List<object>.m()
    // or
    //  e.g. if pDefMD == List<T>.m<U>()
    //          pExactMT = List<string>
    //     then pMDescInCanonMT = List<object>.m<U>()

    MethodDesc * pMDescInCanonMT = pDefMD;

    // Some callers pass a pExactMT that is a subtype of a parent type of pDefMD.
    // GetMethodDeclaringType finds the right parent of pDefMD.
    pExactMT = Generics::GetMethodDeclaringType(TypeHandle(pExactMT), pDefMD).GetMethodTable();
    CONSISTENCY_CHECK(pExactMT->HasSameTypeDefAs(pDefMD->GetMethodTable()));

    if (pDefMD->HasClassOrMethodInstantiation() || (nGenericMethodArgs != 0))
    {
        // General checks related to generics: arity (if any) must match and generic method
        // instantiation (if any) must be well-formed.
        if (pDefMD->GetNumGenericMethodArgs() != nGenericMethodArgs ||
            !Generics::CheckInstantiation(nGenericMethodArgs, genericMethodArgs))
        {
            COMPlusThrowHR(COR_E_BADIMAGEFORMAT);
        }

        pMDescInCanonMT = pExactMT->GetCanonicalMethodTable()->GetMethodDescForSlot(pDefMD->GetSlot());

        if (!allowCreate && (!pMDescInCanonMT->IsRestored() || 
                              !pMDescInCanonMT->GetMethodTable()->IsFullyLoaded()))

        {
            RETURN(NULL);
        }

        pMDescInCanonMT->CheckRestore(level);
    }

    // This case covers nearly all "normal" (i.e. non-associate) MethodDescs.  Just return
    // the MethodDesc in the canonical method table.
    //
    // Also, it will be taken for methods which acquire their type context from the "this" parameter
    // - we don't need instantiating stubs for these.
    if (   (nGenericMethodArgs == 0)
        && (allowInstParam || !pMDescInCanonMT->RequiresInstArg())
        && (forceBoxedEntryPoint == pMDescInCanonMT->IsUnboxingStub())
        && (!forceRemotableMethod || !pMDescInCanonMT->IsInterface()))
    {
        RETURN(pMDescInCanonMT);
    }

    // Unboxing stubs
    else if (forceBoxedEntryPoint)
    {

        //

        //
        MethodDesc *pResultMD;
        if (nGenericMethodArgs == 0)
        {
            MethodTable *pRepMT = pMDescInCanonMT->GetMethodTable();
            mdMethodDef methodDef = pDefMD->GetMemberDef();
            Module *pModule = pDefMD->GetModule();

            // First search for the unboxing MD in the shared vtable for the value type
            MethodTable::MethodIterator it(pRepMT);
            it.MoveToEnd();
            for (; it.IsValid(); it.Prev()) {
                if (it.IsVirtual()) {
                    MethodDesc* pCurMethod = it.GetMethodDesc();
                    if (pCurMethod && pCurMethod->IsUnboxingStub()) {
                        if ((pCurMethod->GetMemberDef() == methodDef) &&
                            (pCurMethod->GetModule() == pModule)) {
                            RETURN(pCurMethod);
                        }
                    }
                }
            }

            Module *pLoaderModule = ClassLoader::ComputeLoaderModule(pRepMT, methodDef, genericMethodArgs, nGenericMethodArgs);

            InstMethodHashTable* pTable = pLoaderModule->GetInstMethodHashTable();
            // If we didn't find it there then go to the hash table
            pResultMD = pTable->FindMethodDesc(TypeHandle(pRepMT),
                                               methodDef,
                                               TRUE /* forceBoxedEntryPoint */,
                                               0,
                                               NULL,
                                               FALSE /* no inst param */);

            // If we didn't find it then create it...
            if (!pResultMD)
            {
                CrstHolder ch(&pLoaderModule->m_InstMethodHashTableCrst);

                // Check whether another thread beat us to it!
                pResultMD = pTable->FindMethodDesc(TypeHandle(pRepMT),
                                                   methodDef,
                                                   TRUE,
                                                   0,
                                                   NULL,
                                                   FALSE);
                if (pResultMD == NULL)
                {
                    if (!allowCreate)
                    {
                        RETURN(NULL);
                    }

                    AllocMemTracker amt;

                    pResultMD = CreateMethodDesc(pLoaderModule->GetDomain(),
                                                 pRepMT,
                                                 pMDescInCanonMT,
                                                 mcInstantiated,
                                                 &amt);

                    // Indicate that this is a stub method which takes a BOXed this pointer.
                    // An BoxedEntryPointStub may still be an InstantiatedMethodDesc
                    pResultMD->SetIsUnboxingStub();
                    pResultMD->AsInstantiatedMethodDesc()->SetupWrapperStubWithInstantiations(pMDescInCanonMT, NULL, NULL);

                    _ASSERTE(!pResultMD->ComputeMayHaveNativeCode());

                    pResultMD->SetTemporaryEntryPoint(pLoaderModule->GetDomain(), &amt);

                    amt.SuppressRelease();

                    // Add it to the table
                    pTable->InsertMethodDesc(pResultMD);
                }

                // CrstHolder goes out of scope here
            }

        }
        else
        {
            mdMethodDef methodDef = pDefMD->GetMemberDef();

            Module *pLoaderModule = ClassLoader::ComputeLoaderModule(pExactMT, methodDef, genericMethodArgs, nGenericMethodArgs);

            InstMethodHashTable* pTable = pLoaderModule->GetInstMethodHashTable();
            // First check the hash table...
            pResultMD = pTable->FindMethodDesc(TypeHandle(pExactMT),
                                               methodDef,
                                               TRUE, /* forceBoxedEntryPoint */
                                               nGenericMethodArgs,
                                               genericMethodArgs,
                                               FALSE /* no inst param */);

            if (!pResultMD)
            {
                // Enter the critical section *after* we've found or created the non-unboxing instantiating stub (else we'd have a race)
                CrstHolder ch(&pLoaderModule->m_InstMethodHashTableCrst);

                // Check whether another thread beat us to it!
                pResultMD = pTable->FindMethodDesc(TypeHandle(pExactMT),
                                                   methodDef,
                                                   TRUE, /* forceBoxedEntryPoint */
                                                   nGenericMethodArgs,
                                                   genericMethodArgs,
                                                   FALSE /* no inst param */);

                if (pResultMD == NULL)
                {
                    if (!allowCreate)
                    {
                        RETURN(NULL);
                    }

                    // Recursively get the non-unboxing instantiating stub.  Thus we chain an unboxing
                    // stub with an instantiating stub.
                    MethodDesc* pNonUnboxingStub=
                        MethodDesc::FindOrCreateAssociatedMethodDesc(pDefMD,
                                                                     pExactMT,
                                                                     FALSE /* not Unboxing */,
                                                                     nGenericMethodArgs,
                                                                     genericMethodArgs,
                                                                     FALSE);

                    _ASSERTE(pNonUnboxingStub->GetClassification() == mcInstantiated);
                    _ASSERTE(!pNonUnboxingStub->RequiresInstArg());
                    _ASSERTE(!pNonUnboxingStub->IsUnboxingStub());

                    AllocMemTracker amt;

                    _ASSERTE(pDefMD->GetClassification() == mcInstantiated);

                    pResultMD = CreateMethodDesc(pLoaderModule->GetDomain(),
                                                 pExactMT,
                                                 pNonUnboxingStub,
                                                 mcInstantiated,
                                                 &amt);

                    pResultMD->SetIsUnboxingStub();
                    pResultMD->AsInstantiatedMethodDesc()->SetupWrapperStubWithInstantiations(pNonUnboxingStub,
                                                                                              pNonUnboxingStub->GetNumGenericMethodArgs(),
                                                                                              pNonUnboxingStub->GetMethodInstantiation());

                    _ASSERTE(!pResultMD->ComputeMayHaveNativeCode());

                    pResultMD->SetTemporaryEntryPoint(pLoaderModule->GetDomain(), &amt);

                    amt.SuppressRelease();

                    pTable->InsertMethodDesc(pResultMD);
                }

                // CrstHolder goes out of scope here
            }
        }
        _ASSERTE(pResultMD);

        if (!allowCreate && (!pResultMD->IsRestored() || !pResultMD->GetMethodTable()->IsFullyLoaded()))
        {
            RETURN(NULL);
        }

        pResultMD->CheckRestore(level);
        _ASSERTE(pResultMD->IsUnboxingStub());
        _ASSERTE(!pResultMD->IsInstantiatingStub());
        RETURN(pResultMD);
    }


    // Now all generic method instantiations and static/shared-struct-instance-method wrappers...
    else
    {
        _ASSERTE(!forceBoxedEntryPoint);

        mdMethodDef methodDef = pDefMD->GetMemberDef();
        Module *pModule = pDefMD->GetModule();

        // Some unboxed entry points are attached to canonical method tables.  This is because
        // we have to fill in vtables and/or dispatch maps at load time,
        // and boxed entry points are created to do this. (note vtables and dispatch maps
        // are only created for canonical instantiations).  These boxed entry points
        // in turn refer to unboxed entry points.

        if (// Check if we're looking for something at the canonical instantiation
            (allowInstParam || pExactMT->IsCanonicalMethodTable()) &&
            // Only value types have BoxedEntryPointStubs in the canonical method table
            pExactMT->IsValueType() &&
            // The only generic methods whose BoxedEntryPointStubs are in the canonical method table
            // are those open MethodDescs at the "typical" isntantiation, e.g.
            // VC<int>.m<T>
            // <NICE> This is probably actually not needed </NICE>
            ((nGenericMethodArgs == 0) || ClassLoader::IsTypicalInstantiation(pModule, methodDef, nGenericMethodArgs, genericMethodArgs))

            )
        {
            MethodTable::MethodIterator it(pExactMT->GetCanonicalMethodTable());
            it.MoveToEnd();
            for (; it.IsValid(); it.Prev()) {
                if (!it.IsVirtual()) {
                    // Get the MethodDesc for current method
                    MethodDesc* pCurMethod = it.GetMethodDesc();

                    // When collecting profile information, note that this method desc
                    // will be accessed at run-time (as it is accessed currently)
                    // <NICE> It would be good if we didn't have to touch all of these </NICE>
                    g_IBCLogger.LogMethodDescAccess(pCurMethod);

                    if (pCurMethod && !pCurMethod->IsUnboxingStub()) {
                        if ((pCurMethod->GetMemberDef() == methodDef)  &&
                            (pCurMethod->GetModule() == pModule) &&
                            (allowInstParam || !pCurMethod->RequiresInstArg()))
                        {
                            if (!allowCreate && (!pCurMethod->IsRestored() || !pCurMethod->GetMethodTable()->IsFullyLoaded()))
                            {
                                RETURN(NULL);
                            }

                            pCurMethod->CheckRestore(level);
                            RETURN(pCurMethod);
                        }
                    }
                }
            }
        }

        // Is it the "typical" instantiation in the correct type?
        if (pDefMD->GetMethodTable()->IsTypicalTypeDefinition() &&
            pDefMD->GetMethodTable() == pExactMT &&
            ClassLoader::IsTypicalInstantiation(pModule, methodDef, nGenericMethodArgs, genericMethodArgs))
        {
            _ASSERTE(pDefMD->IsTypicalMethodDefinition());
            _ASSERTE(!pDefMD->IsUnboxingStub());
            RETURN(pDefMD);
        }

        // OK, so we now know the thing we're looking for can only be found in the MethodDesc table.

        // Are either the generic type arguments or the generic method arguments shared?
        BOOL sharedInst =
            pExactMT->GetClass()->IsSharedByGenericInstantiations()
            || TypeHandle::IsSharableInstantiation(nGenericMethodArgs,genericMethodArgs);

        // If getWrappedCode == true, we are looking for a wrapped MethodDesc

        BOOL getWrappedCode = allowInstParam && sharedInst;
        BOOL getWrappedThenStub = !allowInstParam && sharedInst;

        CQuickBytes qbRepInst;
        TypeHandle *repInst = NULL;
        if (getWrappedCode || getWrappedThenStub)
        {
            // Canonicalize the type arguments.
            DWORD cbAllocaSize = 0;
            if (!ClrSafeInt<DWORD>::multiply(nGenericMethodArgs, sizeof(TypeHandle), cbAllocaSize))
                ThrowHR(COR_E_OVERFLOW);

            repInst = reinterpret_cast<TypeHandle *>(qbRepInst.AllocThrows(cbAllocaSize));

            for (DWORD i = 0; i < nGenericMethodArgs; i++)
            {
                repInst[i] = genericMethodArgs[i].LoadCanonicalGenericArg();
            }
        }


        // <NICE> These paths can probably be merged together more nicely, and the lookup-lock-lookup pattern made much
        // more obvious </NICE>
        InstantiatedMethodDesc *pInstMD;
        if (getWrappedCode)
        {
            // Get the underlying shared code using the canonical instantiations
            pInstMD =
                InstantiatedMethodDesc::FindLoadedInstantiatedMethodDesc(pExactMT->GetCanonicalMethodTable(),
                                                                         methodDef,
                                                                         nGenericMethodArgs,
                                                                         repInst,
                                                                         TRUE);

            // No - so create one.
            if (pInstMD == NULL)
            {
                if (!allowCreate)
                {
                    RETURN(NULL);
                }

                pInstMD = InstantiatedMethodDesc::NewInstantiatedMethodDesc(pExactMT->GetCanonicalMethodTable(),
                                                                            pMDescInCanonMT,
                                                                            NULL,
                                                                            nGenericMethodArgs,
                                                                            repInst,
                                                                            TRUE);
            }
        }
        else if (getWrappedThenStub)
        {
            // See if we've already got the instantiated method desc for this one.
            pInstMD =
                InstantiatedMethodDesc::FindLoadedInstantiatedMethodDesc(pExactMT,
                                                                         methodDef,
                                                                         nGenericMethodArgs,
                                                                         genericMethodArgs,
                                                                         FALSE);

            // No - so create one.  Go fetch the shared one first
            if (pInstMD == NULL)
            {
                if (!allowCreate)
                {
                    RETURN(NULL);
                }

                // This always returns the shared code.  Repeat the original call except with
                // approximate params and allowInstParam=true
                MethodDesc* pWrappedMD = FindOrCreateAssociatedMethodDesc(pDefMD,
                                                                          pExactMT->GetCanonicalMethodTable(),
                                                                          FALSE,
                                                                          nGenericMethodArgs,
                                                                          repInst,
                                                                          TRUE);

                _ASSERTE(pWrappedMD->IsSharedByGenericInstantiations());
                _ASSERTE(nGenericMethodArgs || !pWrappedMD->IsSharedByGenericMethodInstantiations());

                pInstMD = InstantiatedMethodDesc::NewInstantiatedMethodDesc(pExactMT,
                                                                            pMDescInCanonMT,
                                                                            pWrappedMD,
                                                                            nGenericMethodArgs,
                                                                            genericMethodArgs,
                                                                            FALSE);
            }
        }
        else
        {
            // See if we've already got the instantiated method desc for this one.
            // If looking for shared code use the representative inst.
            pInstMD =
                InstantiatedMethodDesc::FindLoadedInstantiatedMethodDesc(pExactMT,
                                                                         methodDef,
                                                                         nGenericMethodArgs,
                                                                         genericMethodArgs,
                                                                         FALSE);

            // No - so create one.
            if (pInstMD == NULL)
            {
                if (!allowCreate)
                {
                    RETURN(NULL);
                }

                pInstMD = InstantiatedMethodDesc::NewInstantiatedMethodDesc(pExactMT,
                                                                            pMDescInCanonMT,
                                                                            NULL,
                                                                            nGenericMethodArgs,
                                                                            genericMethodArgs,
                                                                            FALSE);
            }
        }
        _ASSERTE(pInstMD);

        if (!allowCreate && (!pInstMD->IsRestored() || !pInstMD->GetMethodTable()->IsFullyLoaded()))
        {
            RETURN(NULL);
        }

        pInstMD->CheckRestore(level);

        RETURN(pInstMD);
    }
}


// Given a typical method desc (i.e. instantiated at formal type
// parameters if it is a generic method or lives in a generic class),
// instantiate any type parameters at <object>
//
// NOTE: If allowCreate is FALSE, typically you must also set ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE()
// allowCreate may be set to FALSE to enforce that the method searched
// should already be in existence - thus preventing creation and GCs during 
// inappropriate times.
//
MethodDesc * MethodDesc::FindOrCreateInstantiationAtObject(BOOL allowCreate /* = TRUE */)
{
    CONTRACT(MethodDesc*)
    {
        THROWS;
        if (allowCreate) { GC_TRIGGERS; } else { GC_NOTRIGGER; }
        PRECONDITION(IsTypicalMethodDefinition());
        POSTCONDITION(CheckPointer(RETVAL));
        POSTCONDITION(!RETVAL->HasNonObjectClassOrMethodInstantiation());
    }
    CONTRACT_END

    MethodDesc *pMD = this;
    MethodTable *pMT = pMD->GetMethodTable();

    // First instantiate the declaring type at <object,...,object>
    DWORD nGenericClassArgs = pMT->GetNumGenericArgs();
    DWORD dwAllocSize = 0;
    if (!ClrSafeInt<DWORD>::multiply(sizeof(TypeHandle), nGenericClassArgs, dwAllocSize))
        ThrowHR(COR_E_OVERFLOW);

    CQuickBytes qbGenericClassArgs;
    TypeHandle* pGenericClassArgs = reinterpret_cast<TypeHandle*>(qbGenericClassArgs.AllocThrows(dwAllocSize));

    for (DWORD i = 0; i < nGenericClassArgs; i++)
    {
        pGenericClassArgs[i] = TypeHandle(g_pHiddenMethodTableClass);
    }

    pMT = ClassLoader::LoadGenericInstantiationThrowing(pMT->GetModule(),
                                                        pMT->GetCl(),
                                                        nGenericClassArgs,
                                                        pGenericClassArgs,
                                                        allowCreate ? ClassLoader::LoadTypes : ClassLoader::DontLoadTypes
                                                        ).GetMethodTable();

    // Now instantiate the method at <object,...,object>, creating the shared code.
    // This will not create an instantiating stub just yet.
    DWORD nGenericMethodArgs = pMD->GetNumGenericMethodArgs();
    CQuickBytes qbGenericMethodArgs;
    TypeHandle *genericMethodArgs = NULL;

    // The rest of this method instantiates a generic method
    // Instantiate at "object" if a NULL "genericMethodArgs" is given
    if (nGenericMethodArgs)
    {
        dwAllocSize = 0;
        if (!ClrSafeInt<DWORD>::multiply(sizeof(TypeHandle), nGenericMethodArgs, dwAllocSize))
            ThrowHR(COR_E_OVERFLOW);
        
        genericMethodArgs = reinterpret_cast<TypeHandle*>(qbGenericMethodArgs.AllocThrows(dwAllocSize));

        for (DWORD i =0; i < nGenericMethodArgs; i++)
            genericMethodArgs[i] = TypeHandle(g_pHiddenMethodTableClass);
    }

    RETURN(MethodDesc::FindOrCreateAssociatedMethodDesc(pMD, 
                                                        pMT,
                                                        FALSE, /* don't get unboxing entry point */
                                                        nGenericMethodArgs, 
                                                        genericMethodArgs, 
                                                        TRUE,
                                                        FALSE,
                                                        allowCreate));
}


// Given the shared MethodDesc for an instantiated method, search its
// dictionary layout for a particular (annotated) token.
// If there's no space in the current layout, grow the layout.
// Return a sequence of offsets into dictionaries
// IMPORTANT: dictionaries themselves will not be created until the
// layout is fully know (when the method has been JIT-compiled).
//
// The dictionary is allocated in the Domain of the InstantiatedMethodDesc.
WORD InstantiatedMethodDesc::FindDictionaryToken(DictionaryEntryLayout *pEntryLayout, WORD *offsets)
{

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(offsets));
        PRECONDITION(HasMethodInstantiation());
        PRECONDITION(IsSharedByGenericInstantiations());
        PRECONDITION(RequiresInstMethodDescArg());
        // This always holds but we no longer check it, because the contact for CheckInstantiationIsCanonical was permitting type loads
        //PRECONDITION(TypeHandle::CheckInstantiationIsCanonical(GetNumGenericMethodArgs(),GetMethodInstantiation()));
    }
    CONTRACTL_END

    WORD nindirections = DictionaryLayout::FindToken(GetDomain(),
                                                     GetNumGenericMethodArgs(),
                                                     m_pDictLayout,
                                                     TRUE/*isMethodDict*/,
                                                     GetModule(),
                                                     pEntryLayout,
                                                     offsets
#ifdef _DEBUG 
                                                     , m_pszDebugMethodName
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
    if (LoggingOn(LF_JIT, LL_INFO10000))
    {
        LOG((LF_JIT, LL_INFO10000,
             "GENERICS: Found/allocated slot at position %S to token %x/%x in method dictionary for %S\n",
             slotstr.GetUnicode(), pEntryLayout->m_token1, pEntryLayout->m_token2, m_pszDebugMethodName));
    }
#endif // _DEBUG
    return nindirections;
}


//@GENERICSVER: Set the typical (ie. formal) instantiation
void InstantiatedMethodDesc::SetupGenericMethodDefinition(IMDInternalImport *pIMDII,
                                                          BaseDomain *bmtDomain,
                                                          Module *pModule,
                                                          mdMethodDef tok,
                                                          BOOL newTypeParams)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(CheckPointer(pIMDII));
        PRECONDITION(CheckPointer(bmtDomain));
    }
    CONTRACTL_END;

    // The first field is never used
    m_wFlags2 = GenericMethodDefinition;

    //@GENERICSVER: allocate space for and initialize the typical instantiation
    //we share the typical instantiation among all instantiations by placing it in the generic method desc
    LOG((LF_JIT, LL_INFO1000, "GENERICSVER: Initializing typical method instantiation with type handles\n"));
    mdGenericParam    tkTyPar;
    HENUMInternal hEnumTyPars; //todo: Use HENUMInternalHolder instead?
    HRESULT hr = pIMDII->EnumInit(mdtGenericParam, tok, &hEnumTyPars);


    // Initialize the typical instantiation
    if (!FAILED(hr))
    {
        DWORD numTyPars = pIMDII->EnumGetCount(&hEnumTyPars);
        m_wNumGenericArgs = numTyPars;
        if (newTypeParams)
        {
            DWORD dwAllocSize = 0;
            if (!ClrSafeInt<DWORD>::multiply(numTyPars, sizeof(TypeHandle), dwAllocSize))
                ThrowHR(E_INVALIDARG);
            m_pMethInst = (TypeHandle *) (void*)bmtDomain->GetLowFrequencyHeap()->AllocMem(dwAllocSize);
            for(unsigned int i = 0; i < numTyPars; i++)
            {
                pIMDII->EnumNext(&hEnumTyPars, &tkTyPar);
                void *mem = (void*)bmtDomain->GetLowFrequencyHeap()->AllocMem(sizeof(TypeVarTypeDesc));
                m_pMethInst[i] = TypeHandle(new (mem) TypeVarTypeDesc(pModule, tok, i, tkTyPar));
            }
            LOG((LF_JIT, LL_INFO1000, "GENERICSVER: Initialized typical  method instantiation with %d type handles\n",numTyPars));
        }
        else
            m_pMethInst = NULL;
        pIMDII->EnumClose(&hEnumTyPars);
    };
}

void InstantiatedMethodDesc::SetupWrapperStubWithInstantiations(MethodDesc* wrappedMD,DWORD numGenericArgs, TypeHandle *pInst)
{
    WRAPPER_CONTRACT;

    //_ASSERTE(sharedMD->IMD_IsSharedByGenericMethodInstantiations());

    m_pWrappedMethodDesc = wrappedMD;
    m_wFlags2 = WrapperStubWithInstantiations | HasNonVtableSlot;
    m_pMethInst = pInst;
    m_wNumGenericArgs = numGenericArgs;

    _ASSERTE(IMD_IsWrapperStubWithInstantiations());
    _ASSERTE(((MethodDesc *) this)->IsInstantiatingStub() || ((MethodDesc *) this)->IsUnboxingStub());
}


// Set the instantiation in the per-inst section (this is actually a dictionary)
void InstantiatedMethodDesc::SetupSharedMethodInstantiation(DWORD numGenericArgs, TypeHandle *pPerInstInfo)
{
    WRAPPER_CONTRACT;

    _ASSERTE(numGenericArgs != 0);
    // Initially the dictionary layout is empty
    m_wFlags2 = SharedMethodInstantiation | HasNonVtableSlot;
    m_pPerInstInfo = pPerInstInfo;
    m_wNumGenericArgs = numGenericArgs;

    // 4 seems like a good number
    m_pDictLayout = DictionaryLayout::Allocate(4, GetDomain());

#ifdef _DEBUG 
    {
        SString name;
        TypeString::AppendMethodDebug(name, this);
        LOG((LF_JIT, LL_INFO1000, "GENERICS: Created new dictionary layout for dictionary of size %d for %S\n",
             DictionaryLayout::GetFirstDictionaryBucketSize(GetNumGenericMethodArgs(), m_pDictLayout), name.GetUnicode()));
    }
#endif // _DEBUG

    _ASSERTE(IMD_IsSharedByGenericMethodInstantiations());
}

// Set the instantiation in the per-inst section (this is actually a dictionary)
void InstantiatedMethodDesc::SetupUnsharedMethodInstantiation(DWORD numGenericArgs, TypeHandle *pInst)
{
    LEAF_CONTRACT;

    // The first field is never used
    m_wFlags2 = UnsharedMethodInstantiation | HasNonVtableSlot;
    m_pMethInst = pInst;
    m_wNumGenericArgs = numGenericArgs;

    _ASSERTE(!IsUnboxingStub());
    _ASSERTE(!IsInstantiatingStub());
    _ASSERTE(!IMD_IsWrapperStubWithInstantiations());
    _ASSERTE(!IMD_IsSharedByGenericMethodInstantiations());
    _ASSERTE(!IMD_IsGenericMethodDefinition());
}


// A type variable is bounded to some depth iff it
// has no chain of type variable bounds of that depth.
// We use this is a simple test for circularity among class and method type parameter constraints:
// the constraints on a set of n variables are well-founded iff every variable is bounded by n.
// The test is cheap for the common case that few, if any, constraints are variables.
BOOL Bounded(TypeVarTypeDesc *tyvar, DWORD depth) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(tyvar));
    } CONTRACTL_END;

    if (depth == 0)
    {
        return FALSE;
    }

    DWORD numConstraints;
    TypeHandle *constraints = tyvar->GetConstraints(&numConstraints);
    for (unsigned i = 0; i < numConstraints; i++)
    {
        TypeHandle constraint = constraints[i];
        if (constraint.IsGenericVariable())
        {
            TypeVarTypeDesc* constraintVar = (TypeVarTypeDesc*) constraint.AsTypeDesc();
            if (tyvar->GetInternalCorElementType() == constraintVar->GetInternalCorElementType())
            {
                if (!Bounded(constraintVar, depth - 1))
                    return FALSE;
            }
        }
    }
    return TRUE;
}

void MethodDesc::LoadConstraintsForTypicalMethodDefinition(BOOL *pfHasCircularClassConstraints, BOOL *pfHasCircularMethodConstraints, ClassLoadLevel level/* = CLASS_LOADED*/)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(IsTypicalMethodDefinition());
        PRECONDITION(CheckPointer(pfHasCircularClassConstraints));
        PRECONDITION(CheckPointer(pfHasCircularMethodConstraints));
    } CONTRACTL_END;

    *pfHasCircularClassConstraints = FALSE;
    *pfHasCircularMethodConstraints = FALSE;

    // Force a load of the constraints on the type parameters
    TypeHandle *classInst = GetClassInstantiation();
    DWORD numGenericClassArgs = GetNumGenericClassArgs();
    for (DWORD i = 0; i < numGenericClassArgs; i++)
    {
        TypeVarTypeDesc* tyvar = (TypeVarTypeDesc*) (classInst[i].AsTypeDesc());
        _ASSERTE(tyvar != NULL);
        tyvar->LoadConstraints(level);
    }

    TypeHandle *methodInst = GetMethodInstantiation();
    DWORD numGenericMethodArgs = GetNumGenericMethodArgs();
    for (DWORD i = 0; i < numGenericMethodArgs; i++)
    {
        TypeVarTypeDesc* tyvar = (TypeVarTypeDesc*) (methodInst[i].AsTypeDesc());
        _ASSERTE(tyvar != NULL);
        tyvar->LoadConstraints(level);

        VOID DoAccessibilityCheckForConstraints(MethodTable *pAskingMT, TypeVarTypeDesc *pTyVar);
        DoAccessibilityCheckForConstraints(GetMethodTable(), tyvar);
    }

    // reject circular class constraints
    for (DWORD i = 0; i < numGenericClassArgs; i++)
    {
        TypeVarTypeDesc* tyvar = (TypeVarTypeDesc*) (classInst[i].AsTypeDesc());
        _ASSERTE(tyvar != NULL);
        if(!Bounded(tyvar, numGenericClassArgs))
        {
            *pfHasCircularClassConstraints = TRUE;
        }
    }

    // reject circular method constraints
    for (DWORD i = 0; i < numGenericMethodArgs; i++)
    {
        TypeVarTypeDesc* tyvar = (TypeVarTypeDesc*) (methodInst[i].AsTypeDesc());
        _ASSERTE(tyvar != NULL);
        if(!Bounded(tyvar, numGenericMethodArgs))
        {
            *pfHasCircularMethodConstraints = TRUE;
        }
    }

    return;
}



#ifndef DACCESS_COMPILE 

BOOL MethodDesc::SatisfiesMethodConstraints(TypeHandle thParent, BOOL fThrowIfNotSatisfied/* = FALSE*/)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    // nice: cache (positive?) result in (instantiated) methoddesc
    // caveat: this would be unsafe for instantiated method desc living in generic,
    // hence possibly shared classes (with varying class instantiations).

    if (!HasMethodInstantiation())
       return TRUE;

    DWORD numGenericArgs = GetNumGenericMethodArgs();
    TypeHandle* methodInst = LoadMethodInstantiation();
    PREFIX_ASSUME(methodInst != NULL);
    TypeHandle* typicalInst = LoadTypicalMethodDefinition()->GetMethodInstantiation();
    PREFIX_ASSUME(typicalInst != NULL); // It would have thrown if the load had failed.

    //NB: according to the constructor's signature, thParent should be the declaring type,
    // but the code appears to admit derived types too.
    SigTypeContext typeContext(this,thParent);

    for (DWORD i = 0; i < numGenericArgs; i++)
    {
        TypeHandle thArg = methodInst[i];
        _ASSERTE(!thArg.IsNull());

        TypeVarTypeDesc* tyvar = (TypeVarTypeDesc*) (typicalInst[i].AsTypeDesc());
        _ASSERTE(tyvar != NULL);
        _ASSERTE(TypeFromToken(tyvar->GetTypeOrMethodDef()) == mdtMethodDef);

        tyvar->LoadConstraints(); //TODO: is this necessary for anything but the typical method?




        if (!tyvar->SatisfiesConstraints(&typeContext,thArg))
        {
            if (fThrowIfNotSatisfied)
            {
                SString sParentName;
                TypeString::AppendType(sParentName, thParent);
    
                SString sMethodName(SString::Utf8, GetName());
    
                SString sActualParamName;
                TypeString::AppendType(sActualParamName, methodInst[i]);
    
                SString sFormalParamName;
                TypeString::AppendType(sFormalParamName, typicalInst[i]);
               
                COMPlusThrow(kVerificationException,
                             IDS_EE_METHOD_CONSTRAINTS_VIOLATION,
                             sParentName.GetUnicode(),
                             sMethodName.GetUnicode(),
                             sActualParamName.GetUnicode(),
                             sFormalParamName.GetUnicode()
                            );
      
                
            }
            return FALSE;
        }

    }
    return TRUE;
}

#endif // !DACCESS_COMPILE
