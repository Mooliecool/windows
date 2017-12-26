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
// genericdict.h
// 
// Definitions for "dictionaries" used to encapsulate generic instantiations
// and instantiation-specific information for shared-code generics
// ---------------------------------------------------------------------------

#ifndef _GENERICDICT_H
#define _GENERICDICT_H


// DICTIONARIES 
//
// A dictionary is a cache of handles associated with particular
// instantiations of generic classes and generic methods, containing
// - the instantiation itself (a list of TypeHandles)
// - handles created on demand at runtime when code shared between
//   multiple instantiations needs to lookup an instantiation-specific
//   handle (for example, in newobj C<!0> and castclass !!0[])
//
// DICTIONARY ENTRIES
//
// Dictionary entries (abstracted as the type DictionaryEntry) can be:
//   a TypeHandle (for type arguments and entries associated with a TypeSpec token)
//   a MethodDesc* (for entries associated with a method MemberRef or MethodSpec token)
//   a FieldDesc* (for entries associated with a field MemberRef token)
//   a code pointer (e.g. for entries associated with an CORINFO_ANNOT_ENTRYPOINT annotated token)
//   a dispatch stub address (for entries associated with an CORINFO_ANNOT_DISPATCH_STUBADDR annotated token)
//
// DICTIONARY LAYOUTS
// 
// A dictionary layout describes the layout of all dictionaries that can be
// accessed from the same shared code. For example, Hashtable<string,int> and 
// Hashtable<object,int> share one layout, and Hashtable<int,string> and Hashtable<int,object>
// share another layout. For generic types, the dictionary layout is stored in the EEClass
// that is shared across compatible instantiations. For generic methods, the layout
// is stored in the InstantiatedMethodDesc associated with the shared generic code itself.
//

class TypeHandleList;
class Module;
class BaseDomain;
class SigTypeContext;

enum DictionaryEntryKind 
{ 
    EmptySlot = 0,
    TypeHandleSlot = 1, 
    MethodDescSlot = 2, 
    MethodEntrySlot = 3, 
    DispatchStubAddrSlot = 4, 
    FieldDescSlot = 5,
    UncertainSlot = 6
};

class DictionaryEntryLayout
{
public:
    DictionaryEntryLayout(mdToken annotatedToken1, mdToken token2)
    { LEAF_CONTRACT; m_token1 = annotatedToken1; m_token2 = token2; }

    DictionaryEntryKind GetKind(Module *pModule);

    static BOOL Equal(DictionaryEntryLayout *pEntry1, DictionaryEntryLayout *pEntry2)
    {
        LEAF_CONTRACT;
        return (pEntry1->m_token1 == pEntry2->m_token1 &&
                pEntry1->m_token2 == pEntry2->m_token2);
    }

    // The lower bits of m_token1 are a metadata token. The high bits (CORINFO_ANNOT_MASK)
    // are used to encode the kind of entry for the given token.
    mdToken m_token1;
    // m_token2 is used for constrained calls to encode the constrained type-parameter
    mdToken m_token2;
};

// The type of dictionary layouts. We don't include the number of type
// arguments as this is obtained elsewhere
class DictionaryLayout
{
    friend class Dictionary;
private:
    // Next bucket of slots (only used to track entries that won't fit in the dictionary)
    DictionaryLayout* m_pNext;
    
    // Number of non-type-argument slots in this bucket
    WORD m_numSlots;          

    // m_numSlots of these
    DictionaryEntryLayout m_slots[1];   
     
public:
    // Create an initial dictionary layout with a single bucket containing numSlots slots
    static DictionaryLayout* Allocate(DWORD numSlots, BaseDomain *pDomain);

    // Bytes used for the first bucket of this dictionary, which might be stored inline in
    // another structure (e.g. MethodTable)
    static DWORD GetFirstDictionaryBucketSize(DWORD numGenericArgs, DictionaryLayout *pDictLayout);

    static WORD FindToken(BaseDomain *pDomain,
                          DWORD numGenericArgs, 
                          DictionaryLayout *pDictLayout, 
                          BOOL isMethodDict, 
                          Module *pModule, 
                          DictionaryEntryLayout *pEntryLayout,
                          WORD *offsets
#ifdef _DEBUG
                                                     , const char *debugName
#endif
                          );

    DWORD GetMaxSlots();
    DWORD GetNumUsedSlots();

    DictionaryEntryLayout *GetEntryLayout(DWORD i);

    DictionaryLayout* GetNextLayout() { LEAF_CONTRACT; return m_pNext; }


};


// The type of dictionaries. This is just an abstraction around an open-ended array
class Dictionary
{  
  private:
    DictionaryEntry m_pEntries[1];

    TADDR EntryAddr(ULONG32 idx)
    {
        LEAF_CONTRACT;
        return PTR_HOST_MEMBER_TADDR(Dictionary, this, m_pEntries) +
            idx * sizeof(m_pEntries[0]);
    }
    
  public:
    inline PTR_TypeHandle GetInstantiation() 
    {
        LEAF_CONTRACT;
        return PTR_TypeHandle(EntryAddr(0));
    }

#ifndef DACCESS_COMPILE
    inline void* AsPtr()
    {
        LEAF_CONTRACT;
        return (void*) m_pEntries;
    }
#endif // #ifndef DACCESS_COMPILE

  private:

#ifndef DACCESS_COMPILE

    inline TypeHandle GetTypeHandleSlot(DWORD numGenericArgs, DWORD i) 
    { 
        LEAF_CONTRACT; 
        return *GetTypeHandleSlotAddr(numGenericArgs, i);
    }
    inline MethodDesc *GetMethodDescSlot(DWORD numGenericArgs, DWORD i) 
    { 
        LEAF_CONTRACT; 
        return *GetMethodDescSlotAddr(numGenericArgs,i);
    }
    inline FieldDesc *GetFieldDescSlot(DWORD numGenericArgs, DWORD i) 
    { 
        LEAF_CONTRACT; 
        return *GetFieldDescSlotAddr(numGenericArgs,i);
    }
    inline TypeHandle *GetTypeHandleSlotAddr(DWORD numGenericArgs, DWORD i) 
    { 
        LEAF_CONTRACT; 
        return ((TypeHandle *) &m_pEntries[numGenericArgs + i]);
    }
    inline MethodDesc **GetMethodDescSlotAddr(DWORD numGenericArgs, DWORD i) 
    { 
        LEAF_CONTRACT; 
        return ((MethodDesc **) &m_pEntries[numGenericArgs + i]);
    }
    inline FieldDesc **GetFieldDescSlotAddr(DWORD numGenericArgs, DWORD i) 
    { 
        LEAF_CONTRACT; 
        return ((FieldDesc **) &m_pEntries[numGenericArgs + i]);
    }
    inline DictionaryEntry *GetSlotAddr(DWORD numGenericArgs, DWORD i) 
    { 
        LEAF_CONTRACT; 
        return ((void **) &m_pEntries[numGenericArgs + i]);
    }
    inline DictionaryEntry GetSlot(DWORD numGenericArgs, DWORD i) 
    { 
        LEAF_CONTRACT; 
        return *GetSlotAddr(numGenericArgs,i);
    }
    inline BOOL IsSlotEmpty(DWORD numGenericArgs, DWORD i) 
    { 
        LEAF_CONTRACT; 
        return GetSlot(numGenericArgs,i) == NULL;
    }
    inline Dictionary **GetNextDictionaryAddr(DWORD numGenericArgs, DictionaryLayout *pDictLayout) 
    { 
        LEAF_CONTRACT; 
        return ((Dictionary **) &m_pEntries[numGenericArgs + pDictLayout->GetMaxSlots()]);
    }

#endif // #ifndef DACCESS_COMPILE

  public:

#ifndef DACCESS_COMPILE

    inline Dictionary *GetNextDictionary(DWORD numGenericArgs, DictionaryLayout *pDictLayout) 
    { 
        LEAF_CONTRACT; 
        return *GetNextDictionaryAddr(numGenericArgs, pDictLayout);
    }
    static DictionaryEntry PopulateEntry(Module* module, 
                                         SigTypeContext *typeContext,
                                         DictionaryEntryLayout *pSlotLayout,
                                         BaseDomain *pDictDomain,
                                         BOOL nonExpansive, 
                                         DictionaryEntry *slotPtr);
    void PrepopulateDictionary(Module *module, 
                               SigTypeContext *typeContext,
                               DWORD numGenericArgs, 
                               DictionaryLayout *pDictLayout,
                               BaseDomain *pDictDomain,
                               BOOL nonExpansive);

#endif // #ifndef DACCESS_COMPILE

  private:
    DictionaryEntry PopulateSlot(Module* module, 
                                 SigTypeContext *typeContext,
                                 DWORD numGenericArgs, 
                                 DictionaryLayout *pDictLayout,
                                 BaseDomain *pDictDomain,
                                 DWORD i,
                                 BOOL nonExpansive);
  public:

};

#endif
