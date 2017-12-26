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
// ===========================================================================
// File: namemgr.h
//
// Defines the structures used to store and manage names.
// ===========================================================================

#ifndef __namemgr_h__
#define __namemgr_h__

#include "enum.h"
#include "name.h"
#include "locks.h"
#include "alloc.h"

#ifndef UInt32x32To64
#define UInt32x32To64(a, b) ((DWORDLONG)((DWORD)(a)) * (DWORDLONG)((DWORD)(b)))
#endif


/*
 * A NAMEMGR stores names. An hash table is used with
 * buckets that lead to a linked list of names.
 */

// flags to use when checking IsValidIdentifier
namespace CheckIdentifierFlags { enum _Enum {
    Simple             = 0x00,   // No at-identifiers allowed and no keyword checking
    AllowAtIdentifiers = 0x01,   // allow at identifiers
    CheckKeywords      = 0x02,   // check if identifier is a keyword (and return false if it is)

    StandardSource = AllowAtIdentifiers | CheckKeywords    // Standard source identifier
}; };
DECLARE_FLAGS_TYPE(CheckIdentifierFlags)

class NAMEMGR : public ICSNameTable, public ALLOCHOST
{
public:
    NAMEMGR();
    ~NAMEMGR();

    //void * operator new(size_t size) { return VSAlloc (size); }
    //void operator delete(void * p) { VSFree (p); }

    static unsigned HashString(__in_ecount(length) PCWCH str, int length);

    HRESULT     Init ();

    PNAME AddString(__in_ecount(length) PCWCH str, int length, unsigned hash, bool * wasAdded);
    PNAME AddString(__in_ecount(length) PCWCH str, int length);
    PNAME AddString(PCWSTR str);
    PNAME LookupString(__in_ecount(length) PCWCH str, int length, unsigned hash);
    PNAME LookupString(__in_ecount(length) PCWCH str, int length);
    PNAME LookupString(PCWSTR str);
    bool IsNameKeyword(PNAME name,  CompatibilityMode eKeywordMode, int * iKwd);
    bool IsPPKeyword(NAME *pName, int *piPPKwd);
    bool IsAttrLoc(NAME *pName, int *piAttrLoc);
    PNAME GetAttrLoc(int index);
    PNAME KeywordName(int iKwd);

    PNAME GetPredefName(PREDEFNAME pn)      { return predefNames[pn]; }

    void ClearNames();
    void ClearAll();
    void Term();

    bool IsValidIdentifier(__in_ecount(cchName) PCWCH prgchName, int cchName, CompatibilityMode eKeywordMode, CheckIdentifierFlagsEnum flags);

    // IUnknown
    STDMETHOD(QueryInterface)(REFIID riid, void **ppvObject);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();

    // ICSNameTable
    STDMETHOD(Lookup)(PCWSTR pszName, NAME **ppName);
    STDMETHOD(LookupLen)(__in_ecount(iLen) PCWCH pszName, long iLen, NAME **ppName);
    STDMETHOD(Add)(PCWSTR pszName, NAME **ppName);
    STDMETHOD(AddLen)(__in_ecount(iLen) PCWCH pszName, long iLen, NAME **ppName);
    STDMETHOD(IsValidIdentifier)(PCWSTR pszName, CompatibilityMode eKeywordMode, VARIANT_BOOL *pfValid);
    STDMETHOD(GetTokenText)(long iTokenId, PCWSTR *ppszText);
    STDMETHOD(GetPredefinedName)(long iPredefName, NAME **ppName);
    STDMETHOD(GetPreprocessorDirective)(NAME *pName, long *piToken);
    STDMETHOD(IsKeyword)(NAME *pName, CompatibilityMode eKeywordMode, long *piToken);
    STDMETHOD(CheckCompilerVersion)(HINSTANCE hInstance, DWORD dwVersion, BSTR *pbstrError);
    STDMETHOD(ValidateName)(NAME* pName, BOOL* pfFound);

    // ALLOCHOST
    _declspec(noreturn) void NoMemory () { ALLOCHOST::ThrowOutOfMemoryException(); }
    MEMHEAP     *GetStandardHeap () { ASSERT (pStdHeap != NULL); return pStdHeap; }
    PAGEHEAP    *GetPageHeap () { ASSERT (pPageHeap != NULL); return pPageHeap; }

#ifdef DEBUG
    void PrintStats();
#endif //DEBUG

private:
    bool AddressIsInBucketsArray(void * p)
    {
        return buckets && p >= buckets && p < (buckets + cBuckets);
    }

    HRESULT AddPredefNames();
    HRESULT AddKeywords();
    void GrowTable();

    LONG       iRef;            // Ref count

    CTinyLock lock;             // This is the lock mechanism for thread safety.  NOTE:  All public operations on the name table
                                // currently acquire this lock throughout the entire operation.  If a more granular solution is
                                // necessary, we could store an additional lock per bucket, and release this 'global' lock as soon
                                // as the desired bucket head is determined (locking that bucket first of course).  We'll not do
                                // this unless performance numbers indicate the need for it, however.

    PNAME * buckets;            // the buckets of the name table.
    unsigned cBuckets;          // number of buckets
    unsigned bucketMask;        // mask, always cBuckets - 1.
    unsigned cNames;            // number of names added.

    MEMHEAP     *pStdHeap;      // allocator for general use
    NRHEAP      *pNRHeap;       // allocator to allocate NAME structures from
    PAGEHEAP    *pPageHeap;     // allocator to allocate pages from

    struct KWDENTRY {           // NOTE:  This structure's size must remain a power of 2!
        PNAME name;
        TOKENID iKwd;
    };
    KWDENTRY * kwds;            // table of all keywords.
    unsigned kwdMask;           // mask to use to look up in keyword table.

    PNAME * predefNames;        // array with all the predefined names.

    NAME    *m_rgpPreprocKwds[TID_IDENTIFIER];// Names of preprocessor keywords
    NAME    *m_rgpAttributeLocs[ATTRLOC_COUNT]; // Names of attribute locations

};


// A generally useful hashing function. Uses no state.
/*
 * Hash a 32-bit value to get a much more randomized
 * 32-bit value. (Uses 64-bit values on Win64)
 * Inlined so that
 */
__forceinline unsigned HashUInt(UINT_PTR u)
{

    // Equivalent portable code.
    unsigned __int64 l = UInt32x32To64(u, 0x7ff19519U);  // this number is prime.
    return (unsigned)l + (unsigned)(l >> 32);

}



////////////////////////////////////////////////////////////////////////////////
// NAMETAB
//

class NAMETAB
{
private:
    NAME        **m_ppNames;        // The defined names
    long        m_iCount;           // Number of defined names
    ALLOCHOST   *m_pAllocHost;      // Allocator host

public:
    NAMETAB (ALLOCHOST *p) : m_ppNames(NULL), m_iCount(0), m_pAllocHost(p) {}
    ~NAMETAB();

    void    Define (NAME *pName);
    void    Undef (NAME *pName);
    BOOL    IsDefined (NAME *pName);
    void    ClearAll (BOOL fFree = FALSE);
    long    GetCount () { return m_iCount; }
    NAME    *GetAt (long iIndex) { return m_ppNames[iIndex]; }
};

// Various inlines based on profiles.

/*
 * Simple add string that takes just buffer and length.
 */
__forceinline PNAME NAMEMGR::AddString(__in_ecount(length) PCWCH str, int length)
{
    bool dummy;
    return AddString(str, length, HashString(str, length), &dummy);
}

#endif //__namemgr_h__

