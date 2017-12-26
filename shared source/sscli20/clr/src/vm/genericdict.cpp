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
// ---------------------------------------------------------------------------
// genericdict.cpp
//
// WARNING: Do NOT turn try to save dictionary slots except in the
// hardbind case.  Saving further dictionary slots can lead
// to ComputeNeedsRestore returning TRUE for the dictionary and
// the associated method table (though of course only if some
// entries in the dictionary are prepopulated).  However at
// earlier stages in the NGEN, code may have been compiled
// under the assumption that ComputeNeedsRestore was
// FALSE for the assocaited method table, and indeed this result
// may have been cached in the ComputeNeedsRestore
// for the MethodTable.  Thus the combination of populating
// the dictionary and saving further dictionary slots could lead
// to inconsistencies and unsoundnesses in compilation.

// ---------------------------------------------------------------------------

#include "common.h"
#include "genericdict.h"
#include "typestring.h"
#include "field.h"
#include "typectxt.h"
#include "virtualcallstub.h"


#ifndef DACCESS_COMPILE 

/*static*/ DictionaryLayout* DictionaryLayout::Allocate(DWORD numSlots, BaseDomain *pDomain)
{
    CONTRACT(DictionaryLayout*)
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pDomain));
        PRECONDITION(numSlots > 0);
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END

    DWORD bytes = sizeof(DictionaryLayout) + sizeof(DictionaryEntryLayout) * (numSlots-1);

    DictionaryLayout *pD = (DictionaryLayout*) (void*) pDomain->GetLowFrequencyHeap()->AllocMem(bytes);

    // When bucket spills we'll allocate another layout structure
    pD->m_pNext = NULL;

    // This is the number of slots excluding the type parameters
    pD->m_numSlots = numSlots;

    RETURN pD;
}

#endif // !DACCESS_COMPILE

// Count the number of bytes that are required by the first bucket in a dictionary with the specified layout
/*static*/
DWORD DictionaryLayout::GetFirstDictionaryBucketSize(DWORD numGenericArgs, DictionaryLayout *pDictLayout)
{
    LEAF_CONTRACT;
    PRECONDITION(numGenericArgs > 0);
    PRECONDITION(CheckPointer(pDictLayout, NULL_OK));

    DWORD bytes = numGenericArgs * sizeof(TypeHandle);
    if (pDictLayout != NULL)
        bytes += pDictLayout->m_numSlots * sizeof(void*);

    return bytes;
}

#ifndef DACCESS_COMPILE 

// Find a token in the dictionary layout and return the offsets of indirections
// required to get to its slot in the actual dictionary
//
// NOTE: We will currently never return more than one indirection. We don't
// cascade dictionaries but we will record overflows in the dictionary layout
// (and cascade that accordingly) so we can prepopulate the overflow hash in
// reliability scenarios.
//
// Optimize the case of a token being !i (for class dictionaries) or !!i (for method dictionaries)
/*static*/
WORD DictionaryLayout::FindToken(BaseDomain *pDomain,
                                 DWORD numGenericArgs,
                                 DictionaryLayout *pDictLayout,
                                 BOOL isMethodDict,
                                 Module *pModule,
                                 DictionaryEntryLayout *pEntryLayout,
                                 WORD *offsets
#ifdef _DEBUG 
                                 , const char *debugString
#endif
                                 )
{
    CONTRACT(WORD)
    {
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(numGenericArgs > 0);
        PRECONDITION(CheckPointer(pDictLayout));
        PRECONDITION(CheckPointer(offsets));
        PRECONDITION(CheckPointer(pEntryLayout));
        POSTCONDITION(RETVAL == CORINFO_USEHELPER || RETVAL >= 0 && RETVAL <= CORINFO_MAXINDIRECTIONS);
    }
    CONTRACT_END


    BOOL isFirstBucket = TRUE;

    // Special case: TypeSpec for !i or !!i
    if (TypeFromToken(pEntryLayout->m_token1) == mdtTypeSpec)
    {
        PCCOR_SIGNATURE pSig;
        ULONG cSig;
        if (FAILED(pModule->GetMDImport()->GetTypeSpecFromToken(pEntryLayout->m_token1, &pSig, &cSig)))
            ThrowHR(COR_E_BADIMAGEFORMAT);
        SigPointer sigptr = SigPointer(pSig);
        CorElementType type;
        IfFailThrow(sigptr.GetElemType(&type));
        
        if (type == ELEMENT_TYPE_VAR && !isMethodDict || type == ELEMENT_TYPE_MVAR && isMethodDict)
        {
            ULONG data;
            IfFailThrow(sigptr.GetData(&data));
        
            offsets[0] = sizeof(TypeHandle) * data;
            RETURN 1;
        }
    }

    // First bucket also contains type parameters
    WORD slot = numGenericArgs;
    while (pDictLayout)
    {
        for (DWORD i = 0; i < pDictLayout->m_numSlots; i++)
        {
            // We've found it
            if (pDictLayout->m_slots[i].m_token1 == pEntryLayout->m_token1
                && pDictLayout->m_slots[i].m_token2 == pEntryLayout->m_token2)
            {
                // We don't store entries outside the first bucket in the layout in the dictionary (they'll be cached in a hash
                // instead).
                if (!isFirstBucket)
                    RETURN (WORD) -1;
                offsets[0] = slot * sizeof(DictionaryEntry);
                RETURN 1;
            }

            // If we hit an empty slot then there's no more so use it
            else if (pDictLayout->m_slots[i].m_token1 == mdTokenNil)
            {
                _ASSERTE(pDictLayout->m_slots[i].m_token2 == mdTokenNil);
                LOG((LF_JIT, LL_INFO10000, "GENERICS: Used slot %d in dictionary layout for annotated token 0x%x/0x%x\n", i, pEntryLayout->m_token1, pEntryLayout->m_token2));
                // printf("DICTSLOTS: %s %d\n", debugString, i);

                // we perform a compare exchange to verify the value has not been assigned yet, 
                // otherwise we loop around again to test next slot
                // InterlockedCompareExchange performs always a 32bit value exchange so we should be ok
                _ASSERTE(sizeof(LONG) == sizeof(ULONG32));
                if (FastInterlockCompareExchange((LONG*)&(pDictLayout->m_slots[i].m_token1), *(LONG*)&(pEntryLayout->m_token1), 0) == 0) 
                {
                    // copy the whole struct, this copies the value interlocked exchanged as well but that's ok
                    pDictLayout->m_slots[i] = *pEntryLayout;

                    // Again, we only store entries in the first layout bucket in the dictionary.
                    if (!isFirstBucket)
                        RETURN (WORD) -1;

                    offsets[0] = slot * sizeof(DictionaryEntry);
                    RETURN 1;
                }
            }
            slot++;
        }

        // If we've reached the end of the chain we need to allocate another bucket. Make the pointer update carefully to avoid
        // orphaning a bucket in a race. We leak the loser in such a race (since the allocation comes from the loader heap) but both
        // the race and the overflow should be very rare.
        if (pDictLayout->m_pNext == NULL)
            FastInterlockCompareExchangePointer((PVOID*)&(pDictLayout->m_pNext), Allocate(4, pDomain), 0);

        pDictLayout = pDictLayout->m_pNext;
        isFirstBucket = FALSE;
    }

    // We shouldn't ever get here (we'll record the lookup above or fail to allocate a new bucket in the chain instead).
    _ASSERTE(FALSE);
    RETURN (WORD) -1;
}

DWORD DictionaryLayout::GetMaxSlots()
{
    LEAF_CONTRACT;
    return m_numSlots;
}

DWORD DictionaryLayout::GetNumUsedSlots()
{
    LEAF_CONTRACT;

    DWORD numUsedSlots = 0;
    for (DWORD i = 0; i < m_numSlots; i++)
    {
        if (m_slots[i].m_token1)
            numUsedSlots++;
    }
    return numUsedSlots;
}


DictionaryEntryLayout *DictionaryLayout::GetEntryLayout(DWORD i)
{
    LEAF_CONTRACT;
    _ASSERTE(i >= 0 && i < GetMaxSlots());
    return &m_slots[i];
}


DictionaryEntryKind DictionaryEntryLayout::GetKind(Module *pModule)
{
    LEAF_CONTRACT;
    mdToken token = m_token1 & ~CORINFO_ANNOT_MASK;
    mdToken annotation = m_token1 & CORINFO_ANNOT_MASK;
    if (token == mdTokenNil)
        return EmptySlot;
    if (TypeFromToken(token) == mdtTypeSpec || TypeFromToken(token) == mdtTypeDef || TypeFromToken(token) == mdtTypeRef)
        return TypeHandleSlot;
    BOOL isMemberRefFld = FALSE;
    if (TypeFromToken(token) == mdtMemberRef)
    {
        // We have to look at the metadata to see if it's a field or method
        PCCOR_SIGNATURE pSig;
        ULONG cSig;
        pModule->GetMDImport()->GetNameAndSigOfMemberRef(token, &pSig, &cSig);

        isMemberRefFld = isCallConv(MetaSig::GetCallingConventionInfo(pModule, pSig), IMAGE_CEE_CS_CALLCONV_FIELD);
    }
    if ((TypeFromToken(token) == mdtMethodSpec) ||
        (TypeFromToken(token) == mdtMethodDef) ||
        ((TypeFromToken(token) == mdtMemberRef) && !isMemberRefFld) )
    {
        if (annotation == CORINFO_ANNOT_ENTRYPOINT)
            return MethodEntrySlot;
        else if (annotation == CORINFO_ANNOT_DISPATCH_STUBADDR)
            return DispatchStubAddrSlot;
        else
            return MethodDescSlot;
    }
    if ((TypeFromToken(token) == mdtFieldDef) ||
        ((TypeFromToken(token) == mdtMemberRef) && isMemberRefFld) )
        return FieldDescSlot;

    _ASSERTE(!"Unknown dictionary slot entry kind!");
    return UncertainSlot;

}






extern unsigned GetMemberParent(Module *scopeHnd, unsigned metaTOK);


DictionaryEntry Dictionary::PopulateEntry(Module* module,
                                          SigTypeContext *typeContext,
                                          DictionaryEntryLayout *pEntryLayout,
                                          BaseDomain *pDictDomain,
                                          BOOL nonExpansive,
                                          DictionaryEntry *slotPtr)
{
     CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    DictionaryEntry result = NULL;

    // Strip off annotation
    unsigned metaTOK = pEntryLayout->m_token1 & ~CORINFO_ANNOT_MASK;
    unsigned annotation = pEntryLayout->m_token1 & CORINFO_ANNOT_MASK;

    // We're expecting a type or method, as nothing else is needed at run-time
    switch (TypeFromToken(metaTOK))
    {
    case mdtTypeRef:
    case mdtTypeDef:
    case mdtTypeSpec :
      {
        _ASSERTE(pEntryLayout->m_token2 == NULL);
        TypeHandle clsHnd = ClassLoader::LoadTypeDefOrRefOrSpecThrowing(module, metaTOK, typeContext,
                                                                        (nonExpansive ? ClassLoader::ReturnNullIfNotFound : ClassLoader::ThrowIfNotFound),
                                                                        ClassLoader::FailIfUninstDefOrRef,
                                                                        // == nonExpansive ? FailIfNotLoadedOrNotRestored
                                                                        // Note: Can change this to FailIfNotLoaded + CheckRestore without cost except later on
                                                                        // we make sure we can PrerestoreHardBind to the thing so it's not really worth it.
                                                                        (nonExpansive ? ClassLoader::DontLoadTypes : ClassLoader::LoadTypes));

        if (!clsHnd.IsNull())
        {
            if (annotation == CORINFO_ANNOT_ARRAY)
                clsHnd = ClassLoader::LoadArrayTypeThrowing(clsHnd, ELEMENT_TYPE_SZARRAY, 0,
                    (nonExpansive ? ClassLoader::DontLoadTypes : ClassLoader::LoadTypes));

#ifdef _DEBUG 
            if (LoggingOn(LF_JIT, LL_INFO10000))
            {
                if (slotPtr && *slotPtr)
                {
                    TypeHandle clsHnd2 = TypeHandle::FromPtr(*slotPtr);
                    SString name2;
                    TypeString::AppendTypeDebug(name2, clsHnd2);
                    LOG((LF_JIT, LL_INFO10000, "GENERICS: slot was assigned by another thread 0x%x = %S\n", clsHnd2.AsPtr(), name2.GetUnicode()));
                    _ASSERTE(clsHnd == clsHnd2);
                }
                else
                {
                    SString name;
                    TypeString::AppendTypeDebug(name, clsHnd);
                    LOG((LF_JIT, LL_INFO10000,
                        "GENERICS: Populated slot with type 0x%x = %S for typeSpec token 0x%x/0x%x\n", clsHnd.AsPtr(), name.GetUnicode(), pEntryLayout->m_token1, pEntryLayout->m_token2));
                }
            }
#endif // _DEBUG

            result = (DictionaryEntry) clsHnd.AsPtr();
        }

        break;
      }

      case mdtMemberRef :
      case mdtMethodSpec :
      case mdtMethodDef:
      case mdtFieldDef:
      {
          // <NICE> wsperf: Create a path that doesn't load types or create new handles if nonExpansive is set </NICE>
          if (!nonExpansive)
          {
              BOOL isFld = FALSE;
              if (TypeFromToken(metaTOK) == mdtMemberRef)
              {
                  // OK, we have to look at the metadata to see if it's a field or method
                  PCCOR_SIGNATURE pSig;
                  ULONG cSig;
                  module->GetMDImport()->GetNameAndSigOfMemberRef(metaTOK, &pSig, &cSig);

                  isFld = isCallConv(MetaSig::GetCallingConventionInfo(module, pSig), IMAGE_CEE_CS_CALLCONV_FIELD);
              }

              if (isFld)
              {
                  _ASSERTE(pEntryLayout->m_token2 == NULL);
                  FieldDesc *fd = MemberLoader::GetFieldDescFromMemberRefThrowing(module, (mdMemberRef) metaTOK, typeContext);
                  if (fd != NULL)
                  {
                      result = (DictionaryEntry) fd;

#ifdef _DEBUG 
                      LOG((LF_JIT, LL_INFO10000,
                          "GENERICS: Populated slot with %s::%s for field-ref token 0x%x/0x%x\n", fd->GetApproxEnclosingMethodTable()->GetDebugClassName(), fd->GetName(), pEntryLayout->m_token1, pEntryLayout->m_token2));
                      if (slotPtr && *slotPtr)
                      {
                          LOG((LF_JIT, LL_INFO10000, "GENERICS: slot was already assigned by another thread 0x%x\n", *slotPtr));
                      }
#endif // _DEBUG
                  }
                  else
                      COMPlusThrowHR(E_UNEXPECTED);
              }
              else
              {
                  _ASSERTE(pEntryLayout->m_token2 == NULL || annotation == CORINFO_ANNOT_ENTRYPOINT);
                  MethodDesc *md = MemberLoader::GetMethodDescFromMemberDefOrRefOrSpecThrowing(module,
                      (mdMemberRef) metaTOK,
                      typeContext,
                      TRUE /* strictMetadataChecks */,
                      (annotation == CORINFO_ANNOT_ALLOWINSTPARAM));


                  if (md != NULL)
                  {
                      if (annotation == CORINFO_ANNOT_ENTRYPOINT)
                      {
                          if (pEntryLayout->m_token2)
                          {

                              // OK, we have a constraint call....  We need to find both the exact type being
                              // constrained, the exact ownere of the method being called and then
                              // actually resolve the constraint call.  All such calls should be resolvable.
                              TypeHandle constraintType =
                                  ClassLoader::LoadTypeDefOrRefOrSpecThrowing(module,
                                                                              pEntryLayout->m_token2,
                                                                              typeContext);
                              TypeHandle ownerType =
                                  ClassLoader::LoadTypeDefOrRefOrSpecThrowing(module, GetMemberParent(module,metaTOK), typeContext);

                              MethodDesc *pResolvedMD = constraintType.GetMethodTable()->TryResolveConstraintMethodApprox(ownerType, md);

                              // All such calls should be resolvable.  If not then for now just throw an error.
                              _ASSERTE(pResolvedMD);
                              INDEBUG(if (!pResolvedMD) constraintType.GetMethodTable()->TryResolveConstraintMethodApprox(ownerType, md);)
                              if (!pResolvedMD)
                                  COMPlusThrowHR(COR_E_BADIMAGEFORMAT);

                              md = pResolvedMD;

                          }
                          result = (DictionaryEntry) md->GetMultiCallableAddrOfCode();
                      }
                      else if (annotation == CORINFO_ANNOT_DISPATCH_STUBADDR)
                      {
                          CONSISTENCY_CHECK(md->IsInterface());

                          // Get the more precise parent...
                          TypeHandle ownerHnd =
                              ClassLoader::LoadTypeDefOrRefOrSpecThrowing(module, GetMemberParent(module,metaTOK), typeContext);
                          if (!ownerHnd.IsNull())
                          {
                              // Generate a dispatch stub and store it in the dictionary.
                              //
                              // We generate an indirection so we don't have to write to the dictionary
                              // when we do updates, and to simplify stub indirect callsites.  Stubs stored in
                              // dictionaries use "RegisterIndirect" stub calling, e.g. "call [eax]",
                              // i.e. here the register "eax" would contain the value fetched from the dictionary,
                              // which in turn points to the stub indirection which holds the value the current stub
                              // address itself. If we just used "call eax" then we wouldn't know which stub indirection
                              // to update.  If we really wanted to avoid the extra indirection we could return the _address_ of the
                              // dictionary entry to the  caller, still using "call [eax]", and then the
                              // stub dispatch mechanism can update the dicitonary itself and we don't
                              // need an indirection.
                              VirtualCallStubManager *pMgr = pDictDomain->GetVirtualCallStubManager();

                              // We indirect through a cell so that updates can take place atomically.
                              // The call stub and the indirection cell have the same lifetime as the dictionary itself, i.e.
                              // are allocated in the domain of the dicitonary.
                              //
                              // In the case of overflow (where there is no dictionary, just a global hash table) then
                              // the entry will be placed in the overflow hash table (JitGenericHandleCache).  This
                              // is partitioned according to domain, i.e. is scraped each time an AppDomain gets unloaded.
                              BYTE *addr = pMgr->GetCallStub(ownerHnd, md);
                              result = (DictionaryEntry) pMgr->GenerateStubIndirection(addr);
                          }
                      }
                      else
                      {
                          result = (DictionaryEntry) md;
                      }

#ifdef _DEBUG 
                      if (LoggingOn(LF_JIT, LL_INFO10000))
                      {
                          SString name;
                          TypeString::AppendMethodDebug(name, md);
                          LOG((LF_JIT, LL_INFO10000,
                               "GENERICS: Populated slot with 0x%x = %S for method-ref token 0x%x/0x%x\n", md, name.GetUnicode(), pEntryLayout->m_token1, pEntryLayout->m_token2));
                      }
#endif // _DEBUG
                  }
                  else
                      COMPlusThrowHR(E_UNEXPECTED);
              }
          }
        break;
      }

      default :
        _ASSERTE(!"Invalid token type");
    }

    // Update the slot
    // No harm is done if another thread has already updated it but we expect it to be the same!
    if (slotPtr)
    {
      *slotPtr = result;
    }

    // Return the handle
    return result;
}

DictionaryEntry Dictionary::PopulateSlot(Module* module,
                                         SigTypeContext *typeContext,
                                         DWORD numGenericArgs,
                                         DictionaryLayout *pDictLayout,
                                         BaseDomain *pDictDomain,
                                         DWORD i,
                                         BOOL nonExpansive)
{
    WRAPPER_CONTRACT;
    return  Dictionary::PopulateEntry(module,
                                      typeContext,
                                      pDictLayout->GetEntryLayout(i),
                                      pDictDomain,
                                      nonExpansive,
                                      GetSlotAddr(numGenericArgs, i));
}

void Dictionary::PrepopulateDictionary(Module *module,
                                       SigTypeContext *typeContext,
                                       DWORD numGenericArgs,
                                       DictionaryLayout *pDictLayout,
                                       BaseDomain *pDictDomain,
                                       BOOL nonExpansive)
{
     CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    if (pDictLayout)
    {
        for (DWORD i = 0; i < pDictLayout->GetNumUsedSlots(); i++)
        {
            if (IsSlotEmpty(numGenericArgs,i))
            {
                PopulateSlot(module, typeContext, numGenericArgs, pDictLayout, pDictDomain, i, nonExpansive);
            }
        }
    }
}


#endif // !DACCESS_COMPILE
