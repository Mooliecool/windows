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
// File: instmethhash.h
//
#ifndef _INSTMETHHASH_H
#define _INSTMETHHASH_H

class ZapMonitor;
class AllocMemTracker;
class ZapCodeMap;
class ICorCompilePreloader;

//
//
//
//
//

typedef DPTR(struct InstMethodHashEntry) PTR_InstMethodHashEntry;
typedef struct InstMethodHashEntry
{
    PTR_InstMethodHashEntry pNext;
    DWORD               dwHashValue;
    PTR_MethodDesc      data;

    // <NICE> @dsyme @akenn Put these flags into one of the other words, e.g. bottom two bits of pNext, or data </NICE>
    WORD                wKeyFlags;

    enum
    {
        UnboxingStub    = 0x01,
        RequiresInstArg = 0x02
    };
} InstMethodHashEntry_t;


// The method-desc hash table itself
typedef DPTR(class InstMethodHashTable) PTR_InstMethodHashTable;
class InstMethodHashTable 
{
protected:
    DPTR(PTR_InstMethodHashEntry) m_pBuckets;  // Pointer to first entry for each bucket
    DWORD           m_dwNumBuckets;
    DWORD           m_dwNumEntries;
    friend class ZapMonitor;
public:
    // This is the zap module that owns the table
    // NULL if this table is owned by a domain
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
    InstMethodHashTable();
    ~InstMethodHashTable();

public:
    static InstMethodHashTable* Create(BaseDomain *pDomain, Module *pModule, DWORD dwNumBuckets, AllocMemTracker *pamTracker);

private:
    void               operator delete(void *p);

public:
    // Add a method desc to the hash table
    void InsertMethodDesc(MethodDesc *pMD);

    // Look up a method in the hash table
    MethodDesc *FindMethodDesc(TypeHandle declaringType, 
                               mdMethodDef token, 
                               BOOL unboxingStub, 
                               DWORD numGenericArgs, 
                               TypeHandle *genericArgs, 
                               BOOL getSharedNotStub);

    BOOL ContainsMethodDesc(MethodDesc* pMD);


    // An iterator for the table, currently used only by Module::Save
    struct Iterator
    {
        DPTR(PTR_InstMethodHashEntry) m_pCurBucket;
        PTR_InstMethodHashEntry     m_pCurEntry;
        InstMethodHashTable       * m_pTable;

        // This iterator can be reused for walking different tables
        void Reset();
        Iterator();

        Iterator(InstMethodHashTable * pTable);
        ~Iterator();

    protected:
        void Init();
    };

    BOOL FindNext(Iterator *it, InstMethodHashEntry **ppEntry);

    DWORD GetCount() { LEAF_CONTRACT; return m_dwNumEntries; }

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

private:
    void            GrowHashTable();
    BaseDomain* GetDomain();
};


#endif /* _INSTMETHHASH_H */

