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
// File: typehash.h
//
#ifndef _TYPE_HASH_H
#define _TYPE_HASH_H

//========================================================================================
// This hash table is used by class loaders to look up constructed types:
// arrays, pointers and instantiations of user-defined generic types.
//
// Each persisted module structure has an EETypeHashTable used for constructed types that
// were ngen'ed into that module. See ceeload.hpp for more information about ngen modules.
//
// Types created at runtime are placed in an EETypeHashTable in BaseDomain.
//
// Keys are derivable from the data stored in the table (TypeHandle)
// - for an instantiated type, the typedef module, typedef token, and instantiation
// - for an array/pointer type, the CorElementType, rank, and type parameter
//
//========================================================================================

class ClassLoader;
class TypeKey;
class ZapMonitor;
class AllocMemTracker;
class ZapCodeMap;
class ICorCompilePreloader;

typedef DPTR(struct EETypeHashEntry) PTR_EETypeHashEntry;
typedef struct EETypeHashEntry
{
    PTR_EETypeHashEntry pNext;
    DWORD               dwHashValue;
    TypeHandle          data;
} EETypeHashEntry_t;


// The type hash table itself
typedef DPTR(class EETypeHashTable) PTR_EETypeHashTable;
class EETypeHashTable 
{
protected:
    DPTR(PTR_EETypeHashEntry) m_pBuckets;    // Pointer to first entry for each bucket
    DWORD           m_dwNumBuckets;
    DWORD           m_dwNumEntries;
    friend class ZapMonitor; // to get ranges etc.

public:
    // This is the zap module that owns the table.
    // NULL if this table is owned by a domain.
    PTR_Module      m_pModule;

    // This is the domain in which the hash table is allocated
    PTR_BaseDomain  m_pDomain;

#ifdef _DEBUG
    DWORD           m_dwDebugMemory; // how much memory is owned by this table

    volatile LONG  m_dwSealCount; // Can more types be added to the table?
    void            Seal()   { LEAF_CONTRACT; FastInterlockIncrement(&m_dwSealCount); }
    void            Unseal() { LEAF_CONTRACT; FastInterlockDecrement(&m_dwSealCount); }
#endif

private:
#ifndef DACCESS_COMPILE
    EETypeHashTable();
    ~EETypeHashTable();
#endif
public:
    static EETypeHashTable *EETypeHashTable::Create(BaseDomain *pDomain, Module *pModule, DWORD dwNumBuckets, AllocMemTracker *pamTracker);
private:
    void               operator delete(void *p);

public:
    // Insert a value in the hash table, key implicit in data
    // Value must not be present in the table already
    VOID InsertValue(TypeHandle data);

    // Look up a value in the hash table, key explicit in pKey
    // Return a null type handle if not found
    TypeHandle GetValue(TypeKey* pKey);

    BOOL ContainsValue(TypeHandle th);

    // An iterator for the table, currently used only by Module::Save
    struct Iterator
    {
        DPTR(PTR_EETypeHashEntry) m_pCurBucket;
        PTR_EETypeHashEntry     m_pCurEntry;
        EETypeHashTable       * m_pTable;

        // This iterator can be reused for walking different tables
        void Reset();
        Iterator();

        Iterator(EETypeHashTable * pTable);
        ~Iterator();
        
    protected:
        void Init();
    };

    BOOL FindNext(Iterator *it, EETypeHashEntry **ppEntry);

    DWORD GetCount() { LEAF_CONTRACT; return m_dwNumEntries; }


#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

private:
    EETypeHashEntry_t * FindItem(TypeKey* pKey);
    BOOL CompareInstantiatedType(TypeHandle t, Module *pModule, mdTypeDef token, DWORD numGenericArgs, TypeHandle *genericArgs);
    BOOL CompareFnPtrType(TypeHandle t, BYTE callConv, DWORD numArgs, TypeHandle *retAndArgTypes);
    BOOL GrowHashTable();
    BaseDomain* GetDomain();
};



#endif /* _TYPE_HASH_H */

