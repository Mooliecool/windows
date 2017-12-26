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
/*============================================================
**
** Header:  Map used for interning of string literals.
**         
                                                   </STRIP>
===========================================================*/

#ifndef _STRINGLITERALMAP_H
#define _STRINGLITERALMAP_H

#include "vars.hpp"
#include "appdomain.hpp"
#include "eehash.h"
#include "comstring.h"
#include "eeconfig.h" // For OS pages size
#include "memorypool.h"


class StringLiteralEntry;
// Allocate 16 entries (approx size sizeof(StringLiteralEntry)*16)
#define MAX_ENTRIES_PER_CHUNK 16

STRINGREF AllocateStringObject(EEStringData *pStringData);

// AppDomain specific string literal map.
class AppDomainStringLiteralMap
{
public:
    // Constructor and destructor.
    AppDomainStringLiteralMap(BaseDomain *pDomain);
    ~AppDomainStringLiteralMap();

    // Initialization method.
    void  Init();

    size_t GetSize()
    {
        LEAF_CONTRACT;
        return m_MemoryPool?m_MemoryPool->GetSize():0;
    }

    // Method to retrieve a string from the map.
    STRINGREF *GetStringLiteral(EEStringData *pStringData, BOOL bAddIfNotFound, BOOL bAppDomainWontUnload);

    // Method to explicitly intern a string object.
    STRINGREF *GetInternedString(STRINGREF *pString, BOOL bAddIfNotFound, BOOL bAppDomainWontUnload);

private:
    // Hash tables that maps a Unicode string to a COM+ string handle.
    EEUnicodeStringLiteralHashTable    *m_StringToEntryHashTable;

    // The memorypool for hash entries for this hash table.
    MemoryPool                  *m_MemoryPool;

    BaseDomain                  *m_pDomain;
};

// Global string literal map.
class GlobalStringLiteralMap
{
    friend class StringLiteralEntry;

    // These methods need to acquire the crst of the global string literal map.
    // We can't acquire the lock in GlobalStringLiteralMap::RemoveStringLiteralEntry() because
    // this method may be called during app domain unload, at which time we'll already have
    // acquired the thread store lock, and so we would violate the crst order.
    friend void AppDomainStringLiteralMap::Init();
    friend AppDomainStringLiteralMap::~AppDomainStringLiteralMap();
    friend STRINGREF *AppDomainStringLiteralMap::GetStringLiteral(EEStringData *pStringData, BOOL bAddIfNotFound, BOOL bAppDomainWontUnload);
    friend STRINGREF *AppDomainStringLiteralMap::GetInternedString(STRINGREF *pString, BOOL bAddIfNotFound, BOOL bAppDomainWontUnload);

public:
    // Constructor and destructor.
    GlobalStringLiteralMap();
    ~GlobalStringLiteralMap();

    // Initialization method.
    void Init();

    // Method to retrieve a string from the map. Takes a precomputed hash (for perf).
    StringLiteralEntry *GetStringLiteral(EEStringData *pStringData, DWORD dwHash, BOOL bAddIfNotFound);

    // Method to explicitly intern a string object. Takes a precomputed hash (for perf).
    StringLiteralEntry *GetInternedString(STRINGREF *pString, DWORD dwHash, BOOL bAddIfNotFound);

private:    
    // Helper method to add a string to the global string literal map.
    StringLiteralEntry *AddStringLiteral(EEStringData *pStringData);

    // Helper method to add an interned string.
    StringLiteralEntry *AddInternedString(STRINGREF *pString);

    // Called by StringLiteralEntry when its RefCount falls to 0.
    void RemoveStringLiteralEntry(StringLiteralEntry *pEntry);
    
    // Hash tables that maps a Unicode string to a LiteralStringEntry.
    EEUnicodeStringLiteralHashTable    *m_StringToEntryHashTable;

    // The memorypool for hash entries for this hash table.
    MemoryPool                  *m_MemoryPool;

    // The hash table table critical section.  
    // (the Global suffix is so that it is clear in context whether the global table is being locked 
    // or the per app domain table is being locked.  Sometimes there was confusion in the code
    // changing the name of the global one will avoid this problem and prevent copy/paste errors)
    
    Crst                        m_HashTableCrstGlobal;

    // The large heap handle table.
    LargeHeapHandleTable        m_LargeHeapHandleTable;

};

class StringLiteralEntryArray;

// Ref counted entry representing a string literal.
class StringLiteralEntry
{
private:
    StringLiteralEntry(EEStringData *pStringData, STRINGREF *pStringObj)
    : m_pStringObj(pStringObj), m_dwRefCount(1)
#ifdef _DEBUG
      , m_bDeleted(FALSE)
#endif
    {
        LEAF_CONTRACT;
    }
protected:
    ~StringLiteralEntry()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            PRECONDITION(CheckPointer<void>(this));
        }
        CONTRACTL_END;
    }

public:
    void AddRef()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            PRECONDITION(CheckPointer<void>(this));
            PRECONDITION((LONG)m_dwRefCount > 0);            
            PRECONDITION(SystemDomain::GetGlobalStringLiteralMapNoCreate()->m_HashTableCrstGlobal.OwnedByCurrentThread());            
        }
        CONTRACTL_END;

        _ASSERTE (!m_bDeleted);
        m_dwRefCount++;
    }
#ifndef DACCESS_COMPILE
    FORCEINLINE static void StaticRelease(StringLiteralEntry* pEntry)
    {        
        CONTRACTL
        {
            PRECONDITION(SystemDomain::GetGlobalStringLiteralMapNoCreate()->m_HashTableCrstGlobal.OwnedByCurrentThread());            
        }
        CONTRACTL_END;
        
        pEntry->Release();
    }
#else
    FORCEINLINE static void StaticRelease(StringLiteralEntry* /* pEntry */)
    {
        WRAPPER_CONTRACT;
        DacNotImpl();
    }
#endif // DACCESS_COMPILE

#ifndef DACCESS_COMPILE
    void Release()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            PRECONDITION(CheckPointer<void>(this));
            PRECONDITION(m_dwRefCount > 0);
            PRECONDITION(SystemDomain::GetGlobalStringLiteralMapNoCreate()->m_HashTableCrstGlobal.OwnedByCurrentThread());
        }
        CONTRACTL_END;

        // Safety check for retail builds in case our asserts above didn't 
        // catch the refcount==0 case
        if (m_dwRefCount == 0)
            return;

        m_dwRefCount--;
        if (m_dwRefCount == 0)
        {
            _ASSERTE(SystemDomain::GetGlobalStringLiteralMapNoCreate());
            SystemDomain::GetGlobalStringLiteralMapNoCreate()->RemoveStringLiteralEntry(this);
            // Puts this entry in the free list
            DeleteEntry (this);             
        }
    }
#endif // DACCESS_COMPILE
    
    LONG GetRefCount()
    {
        CONTRACTL
        {
            NOTHROW;
            if(GetThread()){GC_NOTRIGGER;}else{DISABLED(GC_TRIGGERS);};
            PRECONDITION(CheckPointer(this));
        }
        CONTRACTL_END;

        _ASSERTE (!m_bDeleted);

        return (m_dwRefCount);
    }

    STRINGREF* GetStringObject()
    {
        CONTRACTL
        {
            NOTHROW;
            if(GetThread()){GC_NOTRIGGER;}else{DISABLED(GC_TRIGGERS);};
            PRECONDITION(CheckPointer(this));
        }
        CONTRACTL_END;
        return m_pStringObj;
    }

    void GetStringData(EEStringData *pStringData)
    {
        CONTRACTL
        {
            NOTHROW;
            if(GetThread()){GC_NOTRIGGER;}else{DISABLED(GC_TRIGGERS);};
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(this));
            PRECONDITION(CheckPointer(pStringData));
        }
        CONTRACTL_END;
            
        WCHAR *thisChars;
        int thisLength;

        RefInterpretGetStringValuesDangerousForGC(ObjectToSTRINGREF(*(StringObject**)m_pStringObj), &thisChars, &thisLength);
        pStringData->SetCharCount (thisLength); // thisLength is in WCHARs and that's what EEStringData's char count wants
        pStringData->SetStringBuffer (thisChars);
    }

    static StringLiteralEntry *AllocateEntry(EEStringData *pStringData, STRINGREF *pStringObj);
    static void DeleteEntry (StringLiteralEntry *pEntry);

private:
    STRINGREF*                  m_pStringObj;
    union
    {
        volatile DWORD              m_dwRefCount;
        StringLiteralEntry         *m_pNext;
    };

#ifdef _DEBUG
    BOOL m_bDeleted;       
#endif

    // The static lists below are protected by GetGlobalStringLiteralMap()->m_HashTableCrstGlobal
    static StringLiteralEntryArray *s_EntryList; // always the first entry array in the chain. 
    static DWORD                    s_UsedEntries;   // number of entries used up in the first array
    static StringLiteralEntry      *s_FreeEntryList; // free list chained thru the arrays.
};

typedef Wrapper<StringLiteralEntry*,DoNothing,StringLiteralEntry::StaticRelease> StringLiteralEntryHolder; 

class StringLiteralEntryArray
{
public:
    StringLiteralEntryArray *m_pNext;
    BYTE                     m_Entries[MAX_ENTRIES_PER_CHUNK*sizeof(StringLiteralEntry)];
};

#endif // _STRINGLITERALMAP_H

