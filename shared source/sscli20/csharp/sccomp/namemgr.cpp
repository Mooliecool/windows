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
// File: namemgr.cpp
//
// Routines for storing and handling names.
// ===========================================================================


#include "stdafx.h"

// NOTE:  If you add a keyword and the assert in AddKeywords() fires, do this:
//  1) uncomment the following #define FIND_HASH_START (or FIND_HASH_START64 to do a cross-proc find)
//  2) rebuild
//  3) run the command line driver (no arguments).  IT WILL CRASH (bp) but not before reporting a new hash start value
//  4) replace the value of giHashStart below with the value reported above
//  5) comment this line back out and rebuild.

//#define FIND_HASH_START
//#define FIND_HASH_START64

#ifdef FIND_HASH_START64
#ifndef FIND_HASH_START
#define FIND_HASH_START
#endif
#define _WIN64
#endif

#ifndef FIND_HASH_START
const
#endif
unsigned    giHashStart = 0xB1635D64;

/*
 * Static array of the predefined names.
 */
#define PREDEFNAMEDEF(id, name)      L##name,

const PCWSTR predefNameInfo[] =
{
    #include "predefname.h"
};

/*
 *********************************************
 * NAMEMGR methods.
 */

#if defined(_MSC_VER)
#pragma warning(disable:4355)  // allow "this" in member initializer list
#endif  // defined(_MSC_VER)

/*
 * Constructor.
 */
NAMEMGR::NAMEMGR() :
    iRef(0),
    buckets(NULL),
    cBuckets(0),
    bucketMask(0),
    cNames(0),
    pStdHeap(NULL),
    pNRHeap(NULL),
    pPageHeap(NULL)
{
}

#if defined(_MSC_VER)
#pragma warning(default:4355)
#endif  // defined(_MSC_VER)

/*
 * Destructor.
 */
NAMEMGR::~NAMEMGR()
{
    //PrintStats();  // Uncomment to see name managed hashing statistics after the end of the run.
    ClearAll();
    if (pNRHeap)
        delete pNRHeap;
    if (pPageHeap)
        delete pPageHeap;
    if (pStdHeap)
        delete pStdHeap;
}


/*
 * Clear all names, leaving the bucket table allocated.
 */
void NAMEMGR::ClearNames()
{
    // Acquire the lock
    CTinyGate   gate (&lock);

    // Free all the allocated names and empty the bucket table.
    if (pNRHeap)
        pNRHeap->FreeHeap();
    pPageHeap->AllowWrite(ProtectedEntityFlags::Nametable, buckets, cBuckets * sizeof(PNAME));
    memset(buckets, 0, cBuckets * sizeof(PNAME));
    pPageHeap->ForbidWrite(ProtectedEntityFlags::Nametable, buckets, cBuckets * sizeof(PNAME));
    cNames = NULL;
}

/*
 * Clear all the names and free the bucket table.
 */
void NAMEMGR::ClearAll()
{
    // Acquire the lock
    CTinyGate   gate (&lock);

    ClearNames();

    // Free the bucket table.
    if (buckets) {
        size_t cb = cBuckets * sizeof(PNAME);
        if (cb < PAGEHEAP::pageSize)
            cb = PAGEHEAP::pageSize;
        pPageHeap->FreePages(ProtectedEntityFlags::Nametable, buckets, cb);
    }

    // Free the keyword table.
    if (kwds) {
        pPageHeap->FreePages(ProtectedEntityFlags::Nametable, kwds, PAGEHEAP::pageSize);
    }

    kwds = NULL;
    buckets = NULL;
    cBuckets = bucketMask = 0;
}

// HashUInt moved to namemgr.h, to be inlined

/*
 * Hash a string to return a 32-bit hash value.
 */

unsigned NAMEMGR::HashString(__in_ecount(length) PCWCH str, int length)
{
    PCWCH strEnd = str + length;
    PCWCH strDwEnd = str + (length & 0xFFFFFFFE);

    unsigned hash = giHashStart;


    ASSERT(sizeof(DWORD)/sizeof(WCHAR) == 2);

    while (str < strDwEnd) {
#if BIGENDIAN
        // There's an assumption that the hash values fit into certain
        // ranges so make sure it matches little endian format
        hash += *str | *(str+1) << 16;
#else
        hash += GET_UNALIGNED_32(str);
#endif
        hash += (hash << 10);
        hash ^= (hash >> 6);
        str += 2;
    }

    // Hash the last character, if any.
    if (str < strEnd) {
        hash += *str;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

#if defined(FIND_HASH_START64) && defined(_M_IX86)
    // Roll this inline instead of calling HashUInt because the x86 and Win64
    // routines are slightly different
    unsigned __int64 l = (unsigned __int64)hash * 0x000000007ff19519;  // this number is prime.
    return (unsigned)l + (unsigned)(l >> 32);
#else
    return HashUInt(hash);
#endif
}


/*
 * Lookup a string in the name table. If it's not there, then return NULL.
 */
PNAME NAMEMGR::LookupString(__in_ecount(length) PCWCH str, int length, unsigned hash)
{
    unsigned ibucket;
    PNAME name;

#ifdef DEBUG
    // Embedded NUL characters are NOT allowed in the string.
    for (int i = 0; i < length; ++i)
        ASSERT(str[i] != 0);
#endif //DEBUG

    // Acquire the lock
    CTinyGate   gate (&lock);

    if (buckets == NULL)
        return NULL;        // Nothing in the table.

    // Get the bucket.
    ibucket = (hash & bucketMask);

    // Search all names in the bucket for the string we want.
    name = buckets[ibucket];
    while (name) {
        if (name->hash == hash) {
            // Hash values match... pretty good sign. Check length
            // and contents.
            if ((int)wcslen(name->text) == length &&
                memcmp(name->text, str, length * sizeof(WCHAR)) == 0)
            {
                // Names match!
                return name;
            }
        }
        name = name->nextInBucket;
    }

    return NULL;
}

/*
 * Simple lookup string that takes just buffer and length.
 */
PNAME NAMEMGR::LookupString(__in_ecount(length) PCWCH str, int length)
{
    return LookupString(str, length, HashString(str, length));
}

/*
 * Simple lookup string the just takes zero-terminated string.
 */
PNAME NAMEMGR::LookupString(PCWSTR str)
{
    int length = (int)wcslen(str);
    return LookupString(str, length, HashString(str, length));
}



/*
 * Add a string to the name table. If it's already there
 * return it.
 */
PNAME NAMEMGR::AddString(__in_ecount(length) PCWCH str, int length, unsigned hash, bool * wasAdded)
{
    unsigned ibucket;
    PNAME name;
    PNAME * link; // link to place to add new name.
#ifdef DEBUG
    int cInBucket = 0;  // number of items in this bucket.
#endif //DEBUG

    // Acquire the lock
    CTinyGate   gate (&lock);

    if (buckets == NULL) {
        // Nothing in the table. Create a new empty table.
        ASSERT(cNames == 0);

        buckets = (PNAME *) pPageHeap->AllocPages(PAGEHEAP::pageSize);
        if (buckets == NULL)
            return NULL;

        ASSERT((PAGEHEAP::pageSize / sizeof(PNAME) < UINT_MAX));
        cBuckets = (unsigned)(PAGEHEAP::pageSize / sizeof(PNAME));

        memset(buckets, 0, cBuckets * sizeof(PNAME));
        bucketMask = cBuckets - 1;
        pPageHeap->ForbidWrite(ProtectedEntityFlags::Nametable, buckets, PAGEHEAP::pageSize);
    }

    // Get the bucket.
    ibucket = (hash & bucketMask);

    // Traverse all names in the bucket.
    link = & buckets[ibucket];
    for (;;) {
        name = *link;

        if (!name)
            break;  // End of list.

#ifdef DEBUG
        ++cInBucket;
#endif //DEBUG

        if (name->hash == hash) {
            // Hash values match... pretty good sign. Check contents.

            // Check one DWORD at a time, in an unrolled loop.
            const WCHAR * pLastChar = str + length;  // Point to beyond last char .
            const DWORD * pdw1 = (DWORD *) str;
            const DWORD * pdw2 = (DWORD *) name->text;
#ifdef ALIGN_ACCESS
            ASSERT(((ULONG_PTR)pdw2 & 0x03) == 0);  // The namemgr keeps it's strings pointer aligned
            if (((ULONG_PTR)pdw1 & 0x03) != 0) {
                // If the lookup string is not aligned, just use a string compare
                if (wcsncmp(str, name->text, length) == 0 &&
                    // Also check the length (in case the new string has the same hash and is a
                    // perfect substring of the existing one)
                    name->text[length] == L'\0')
                    goto MATCH;
                else
                    goto NOMATCH;
            }
#endif
            for (;;) {
                switch (pLastChar - (const WCHAR *)pdw1) {
                default:
                    if (*pdw1++ != *pdw2++)
                        goto NOMATCH;
                case 15:
                case 14:
                    if (*pdw1++ != *pdw2++)
                        goto NOMATCH;
                case 13:
                case 12:
                    if (*pdw1++ != *pdw2++)
                        goto NOMATCH;
                case 11:
                case 10:
                    if (*pdw1++ != *pdw2++)
                        goto NOMATCH;
                case 9:
                case 8:
                    if (*pdw1++ != *pdw2++)
                        goto NOMATCH;
                case 7:
                case 6:
                    if (*pdw1++ != *pdw2++)
                        goto NOMATCH;
                case 5:
                case 4:
                    if (*pdw1++ != *pdw2++)
                        goto NOMATCH;
                case 3:
                case 2:
                    if (*pdw1++ != *pdw2++)
                        goto NOMATCH;
                    break;

                case 1:
                    // check last character and presence of null terminator on pdw2
                    if (*(WCHAR*)pdw1 != *(WCHAR*)pdw2 || *((WCHAR*)pdw2 + 1) != 0)
                        goto NOMATCH;
                    goto MATCH;

                case 0:
                    if (*(WCHAR *)pdw2 != L'\0')
                        goto NOMATCH;
                    goto MATCH;
                }
            }

        MATCH:
            // Names match!
            *wasAdded = false;
            return name;

        NOMATCH:
            ; // Names didn't match.
        }
        link = & name->nextInBucket;
    }

    ASSERT(cInBucket < 12);      // If this triggers, our hash function is no good.

    // Name not found; create new and add it to the table.
    name = (NAME *) pNRHeap->Alloc(sizeof(NAME) + (length + 1) * sizeof(WCHAR));
    if (name == NULL)
        return NULL;

    bool fSetLinkToReadOnly = false;
    if (!pNRHeap->IsAddressInCurrentPage(link)) {
        pPageHeap->AllowWrite(ProtectedEntityFlags::Nametable, link, sizeof(NAME*));
        fSetLinkToReadOnly = true;
    }
    *link = name;

    // Fill in the new name structure and copy in name.
    name->hash = hash;
    name->nextInBucket = 0;

    memcpy((PWSTR)name->text, str, length * sizeof(WCHAR));
    *((WCHAR *)name->text + length) = L'\0';        // Must cast away constness of name->text

    pNRHeap->MakeCurrentPageReadOnly();
    if (fSetLinkToReadOnly && (pNRHeap->AllowingReadOnlyDirectives() || AddressIsInBucketsArray(link)))
        pPageHeap->ForbidWrite(ProtectedEntityFlags::Nametable, link, sizeof(NAME*));

    // If the name table is full, resize it.
    ++cNames;
    if (cNames >= cBuckets)
        GrowTable();

    *wasAdded = true;
    return name;
}

/*
 * Simple add string the just takes zero-terminated string.
 */
PNAME NAMEMGR::AddString(PCWSTR str)
{
    bool dummy;
    int length = (int)wcslen(str);
    return AddString(str, length, HashString(str, length), &dummy);
}

/*
 * Double the bucket size of the name table.
 */
void NAMEMGR::GrowTable()
{
    unsigned newSize;
    size_t cbNew;
    PNAME * newBuckets;
    unsigned iBucket;
    PNAME * linkLow, * linkHigh;
    PNAME name;

    // Must be locked!
    ASSERT (lock.LockedByMe());

    // Allocate the new bucket table.
    newSize = cBuckets * 2;
    cbNew = newSize * sizeof(PNAME);
    if (cbNew < PAGEHEAP::pageSize)
        cbNew = PAGEHEAP::pageSize;
    newBuckets = (PNAME *) pPageHeap->AllocPages(cbNew);
    if (newBuckets == NULL)
    {
        VSFAIL ("Out of memory");
        return;
    }
    size_t cb = cBuckets * sizeof(PNAME);
    if (cb < PAGEHEAP::pageSize)
        cb = PAGEHEAP::pageSize;
    pNRHeap->MakeAllHeapWriteable();

    // Go through each bucket in the old table, and link the names
    // into the new tables. Because the old table is a power of 2 in size,
    // and the new table is twice that size, names from bucket n in the
    // old table must map to either bucket n or bucket n + oldSize in the new
    // table. Which of these two is determined by (hash & oldSize).
    for (iBucket = 0; iBucket < cBuckets; ++iBucket)
    {
        // Get starting links for the two new buckets.
        linkLow = &newBuckets[iBucket];
        linkHigh = &newBuckets[iBucket + cBuckets];

        // Go through each name in the old bucket, redistributing
        // to the two new buckets.
        name = buckets[iBucket];
        while (name) {
            if (name->hash & cBuckets) {
                *linkHigh = name;
                linkHigh = & name->nextInBucket;
            }
            else {
                *linkLow = name;
                linkLow = & name->nextInBucket;
            }

            name = name->nextInBucket;
        }

        // Terminate both new linked lists.
        *linkLow = *linkHigh = NULL;
    }

    // Free the old bucket table.
    pPageHeap->ForbidWrite(ProtectedEntityFlags::Nametable, newBuckets, cbNew);
    pNRHeap->MakeAllHeapReadOnly();
    pPageHeap->FreePages(ProtectedEntityFlags::Nametable, buckets, cb);

    // Remember the new bucket table and size.
    buckets = newBuckets;
    cBuckets = newSize;
    bucketMask = cBuckets - 1;
}

/*
 * Checks if this name is contained within this name table
 */
HRESULT NAMEMGR::ValidateName(NAME* pName, BOOL* pfFound)
{
    if (buckets != NULL && 
        pName != NULL)
    {
        unsigned iBucket = (pName->hash & bucketMask);
        if (iBucket < cBuckets)
        {
            NAME* pCurrentName = buckets[iBucket];
            while (pCurrentName != NULL)
            {
                if (pCurrentName == pName)
                {
                    *pfFound = true;
                    return S_OK;
                }

                pCurrentName = pCurrentName->nextInBucket;
            }
        }
    }

    *pfFound = false;
    return S_OK;
}

/*
 * Add all the keywords to the name table. To enable fast determination
 * of whether a particular NAME is a keyword, a seperate keyword hash table
 * is used that is indexed by the hash value of the name. At most one keyword
 * has the same index, so we don't need a collision resolution scheme.
 */
HRESULT NAMEMGR::AddKeywords()
{
    PNAME name;
    unsigned sizeTable;
    unsigned iTable;
    int i;

    // Must be locked!
    ASSERT (lock.LockedByMe());

    // Allocate the keyword table, using one page of memory.
    ASSERT((PAGEHEAP::pageSize / sizeof(KWDENTRY) < UINT_MAX));
    sizeTable = (unsigned)(PAGEHEAP::pageSize / sizeof(KWDENTRY));
#ifdef FIND_HASH_START64
    // IA64 doubles the size of KWDENTRY and pageSize, so sizeTable is the
    // same as X86, but AMD64 doubles the size of KWDENTRY, but keeps the
    // same pageSize which results in a smaller table
    sizeTable /= 2;
#endif
    kwdMask = sizeTable - 1;
    kwds = (KWDENTRY *) pPageHeap->AllocPages(PAGEHEAP::pageSize);
    if (kwds == NULL)
        return E_OUTOFMEMORY;

#ifdef FIND_HASH_START
    unsigned    iHashStartOrg = giHashStart;
tryagain:
#endif
    memset(kwds, 0, PAGEHEAP::pageSize);

    // Add each keyword first to the name table, then record the
    // name and keyword id in the keyword table.
    for (i = 0; i < TID_IDENTIFIER; ++i) {
        const TOKINFO * tokinfo = CParser::GetTokenInfo((TOKENID)i);

        name = AddString(tokinfo->pszText);
        if (name == NULL)
            return E_OUTOFMEMORY;
        iTable = (name->hash & kwdMask);

#ifdef FIND_HASH_START
        if (kwds[iTable].name != NULL)
        {
            // Whack the table, adjust the starting hash value and try again
            ClearNames();
            if (++giHashStart == iHashStartOrg)
            {
                // No value works.  Either make a new hash function, or grow the array!
                printf ("ERROR:  HASH FUNCTION DOESN'T WORK!  (See %s(%d))\n", __FILE__, __LINE__);
                DebugBreak();
            }

            goto tryagain;
        }
#else
        // If this triggers, we have a collision.  See the top of
        // this file for how to fix...
        ASSERT(kwds[iTable].name == NULL);
#endif

        kwds[iTable].name = name;
        kwds[iTable].iKwd = (TOKENID)i;

    }

    pPageHeap->ForbidWrite(ProtectedEntityFlags::Nametable, kwds, PAGEHEAP::pageSize);

#ifdef FIND_HASH_START
    printf ("APPROPRIATE HASH START VALUE FOUND: %08X\n", giHashStart);
    DebugBreak(); // can't continue; name table is potentially missing some names now...
#endif

    // Initialize the preprocessor keywords
    #define PPKWD(n,id,d) n,
    const PCWSTR  rgszKwds[] = {
        #include "ppkwds.h"
        NULL };

    ASSERT (PPT_DEFINE == 0);
    for (i=PPT_DEFINE; i < PPT_IDENTIFIER; i++)
        m_rgpPreprocKwds[i] = AddString (rgszKwds[i]);

    // Initialize the attribute location table
    #define ATTRLOCDEF(id, value, str) str,
    const PCWSTR rgszAttrLocs[] = {
        #include "attrloc.h"
        };

    for (i = 0; i < ATTRLOC_COUNT; i++)
        m_rgpAttributeLocs[i] = AddString(rgszAttrLocs[i]);

    return S_OK;
}

bool NAMEMGR::IsPPKeyword (NAME *p, int *piPP)
{
    // NOTE:  There are currently only 8 pp keywords, so a linear search is okay...
    for (int i=0; i<PPT_IDENTIFIER; i++)
    {
        if (m_rgpPreprocKwds[i] == p)
        {
            *piPP = i;
            return true;
        }
    }

    return false;
}

bool NAMEMGR::IsAttrLoc(NAME *pName, int *piAttrLoc)
{
    for (int i = 0; i < ATTRLOC_COUNT; i ++)
    {
        if (m_rgpAttributeLocs[i] == pName)
        {
            *piAttrLoc = (1 << i);
            return true;
        }
    }

    return false;
}

PNAME NAMEMGR::GetAttrLoc(int index)
{
    ASSERT(index >=0 && index < ATTRLOC_COUNT);
    return m_rgpAttributeLocs[index];
}

/*
 * Get the name corresponding to a keyword.
 */
PNAME NAMEMGR::KeywordName(int iKwd)
{
    return LookupString (CParser::GetTokenInfo((TOKENID)iKwd)->pszText);
}

/*
 * Add all the predefined names to the name table. These are common
 * names we use a lot. We also add all the keywords.
 */
HRESULT NAMEMGR::AddPredefNames()
{
    HRESULT     hr;

    // Add the keywords.
    if (FAILED (hr = AddKeywords()))
        return hr;

    {
        NRHeapWriteAllower allowWrite(pNRHeap); 
    
        // Allocate space for the predefined names.
        predefNames = (PNAME *) pNRHeap->Alloc(PN_COUNT * sizeof(PNAME));
        if (predefNames == NULL)
            return E_OUTOFMEMORY;

        // Add all the predefined names to the name table, and remember their names.
        for (int i = 0; i < PN_COUNT; ++i) {
            if ((predefNames[i] = AddString(predefNameInfo[i])) == NULL)
                return E_OUTOFMEMORY;
        }
    }

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// NAMEMGR::Init

HRESULT NAMEMGR::Init ()
{
    CTinyGate   gate (&lock);

    if (pStdHeap != NULL)
        return E_FAIL;      // Already initialized.  What are you doing?

    pStdHeap = vs_new MEMHEAP (this, true);
    if (pStdHeap == NULL)
        return E_OUTOFMEMORY;

    pPageHeap = vs_new PAGEHEAP (this, true);
    if (pPageHeap == NULL)
        return E_OUTOFMEMORY;

    pNRHeap = vs_new NRHEAP (this, false, ProtectedEntityFlags::Nametable);
    if (pNRHeap == NULL)
        return E_OUTOFMEMORY;
    pNRHeap->AllowReadOnlyDirectives();

    return AddPredefNames ();
}

////////////////////////////////////////////////////////////////////////////////
// NAMEMGR::Lookup

STDMETHODIMP NAMEMGR::Lookup (PCWSTR pszName, NAME **ppName)
{
    *ppName = LookupString (pszName);
    return *ppName ? S_OK : S_FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// NAMEMGR::Lookup

STDMETHODIMP NAMEMGR::LookupLen (__in_ecount(iLen) PCWCH prgchName, long iLen, NAME **ppName)
{
    *ppName = LookupString (prgchName, iLen);
    return *ppName ? S_OK : S_FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// NAMEMGR::Add

STDMETHODIMP NAMEMGR::Add (PCWSTR pszName, NAME **ppName)
{
    *ppName = AddString (pszName);
    return *ppName ? S_OK : E_OUTOFMEMORY;
}

////////////////////////////////////////////////////////////////////////////////
// NAMEMGR::Add

STDMETHODIMP NAMEMGR::AddLen (__in_ecount(iLen) PCWCH prgchName, long iLen, NAME **ppName)
{
    *ppName = AddString (prgchName, iLen);
    return *ppName ? S_OK : E_OUTOFMEMORY;
}

////////////////////////////////////////////////////////////////////////////////
// NAMEMGR::IsValidIdentifier

// Checks for a valid identifier.  The definition of a valid identifier can change depending on what flags are passed.
bool NAMEMGR::IsValidIdentifier(__in_ecount(cchName) PCWCH prgchName, int cchName, CompatibilityMode eKeywordMode, CheckIdentifierFlagsEnum flags)
{
    ASSERT(prgchName || cchName == 0);
    ASSERT(cchName >= 0);

    bool atKeyword = false;

    if ((cchName <= 0) || (cchName >= MAX_IDENT_SIZE))
        return false;

    const WCHAR * pch = prgchName;
    const WCHAR * pchLim = pch + cchName;

    if (*pch == L'@')
    {
        if (!(flags & CheckIdentifierFlags::AllowAtIdentifiers))
            return false;

        atKeyword = true;

        if (++pch >= pchLim)
            return false;
    }

    ASSERT(pch < pchLim);
    if (!IsIdentifierChar(*pch++))
        return false;

    for ( ; pch < pchLim; ++pch)
    {
        if (!IsIdentifierCharOrDigit(*pch))
            return false;
    }

    if (!(flags & CheckIdentifierFlags::CheckKeywords) || atKeyword)
        return true;

    int kwd;
    NAME * name = LookupString(prgchName, cchName);

   return name == NULL || !IsNameKeyword(name, eKeywordMode, &kwd);
}

////////////////////////////////////////////////////////////////////////////////
// NAMEMGR::IsValidIdentifer

STDMETHODIMP NAMEMGR::IsValidIdentifier (PCWSTR pszName, CompatibilityMode eKeywordMode, VARIANT_BOOL *pfValid)
{
    ASSERT(pszName);
    *pfValid = VARIANT_FALSE;

    if (IsValidIdentifier(pszName, (int)wcslen(pszName), eKeywordMode, CheckIdentifierFlags::StandardSource))
    {
        *pfValid = VARIANT_TRUE;
    }
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// NAMEMGR::GetTokenText

STDMETHODIMP NAMEMGR::GetTokenText (long iTokenId, PCWSTR *ppszText)
{
    if (iTokenId < 0 || iTokenId >= TID_INVALID)
        return E_INVALIDARG;

    *ppszText = CParser::GetTokenInfo((TOKENID)iTokenId)->pszText;
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// NAMEMGR::GetPredefinedName

STDMETHODIMP NAMEMGR::GetPredefinedName (long iPredefName, NAME **ppName)
{
    if (iPredefName < 0 || iPredefName >= PN_COUNT)
        return E_FAIL;

    *ppName = GetPredefName ((PREDEFNAME)iPredefName);
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// NAMEMGR::GetPreprocessorDirective

STDMETHODIMP NAMEMGR::GetPreprocessorDirective (NAME *pName, long *piToken)
{
    int     i;

    if (IsPPKeyword (pName, &i))
    {
        *piToken = i;
        return S_OK;
    }

    return E_FAIL;
}

////////////////////////////////////////////////////////////////////////////////
// NAMEMGR::IsKeyword

STDMETHODIMP NAMEMGR::IsKeyword (NAME *pName, CompatibilityMode eKeywordMode, long *piToken)
{
    int     iKwd;

    if (IsNameKeyword (pName, eKeywordMode, &iKwd))
    {
        if (piToken != NULL)
            *piToken = (long)iKwd;
        return S_OK;
    }

    return S_FALSE;
}


bool NAMEMGR::IsNameKeyword(PNAME name, CompatibilityMode eKeywordMode, int * iKwd)
{
    ASSERT(name);

    // Find index into the keyword table.
    unsigned iTable = name->hash & kwdMask;

    if (kwds[iTable].name == name
        && (eKeywordMode != CompatibilityECMA1 || 0 == (CParser::GetTokenInfo(kwds[iTable].iKwd)->dwFlags & TFF_MSKEYWORD))) {
        // Yes, we are a keyword.
        *iKwd = kwds[iTable].iKwd;
        return true;
    }
    else {
        // No, not a keyword.
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////
// NAMEMGR::QueryInterface

STDMETHODIMP NAMEMGR::QueryInterface (REFIID riid, void **ppObj)
{
    if (riid == IID_IUnknown || riid == IID_ICSNameTable)
    {
        *ppObj = (ICSNameTable *)this;
        AddRef();
        return S_OK;
    }

    if (riid == IID_ICSPrivateNameTable)
    {
        *ppObj = this;
        AddRef();
        return S_OK;
    }

    *ppObj = NULL;
    return E_NOINTERFACE;
}

////////////////////////////////////////////////////////////////////////////////
// NAMEMGR::AddRef

STDMETHODIMP_(ULONG) NAMEMGR::AddRef ()
{
    return InterlockedIncrement (&iRef);
}

////////////////////////////////////////////////////////////////////////////////
// NAMEMGR::Release

STDMETHODIMP_(ULONG) NAMEMGR::Release ()
{
    ULONG   i = InterlockedDecrement (&iRef);

    if (i == 0)
        delete this;

    return i;
}

STDMETHODIMP NAMEMGR::CheckCompilerVersion (HINSTANCE hInstance, DWORD dwVersion, BSTR *pbstrError)
{
    return CFactory::StaticCheckCompilerVersion(hInstance, dwVersion, pbstrError);
}


////////////////////////////////////////////////////////////////////////////////
// NAMETAB::~NAMETAB

NAMETAB::~NAMETAB ()
{
    if (m_ppNames != NULL)
        m_pAllocHost->GetStandardHeap()->Free (m_ppNames);
}

////////////////////////////////////////////////////////////////////////////////
// NAMETAB::Define

void NAMETAB::Define (NAME *pName)
{
    // Make sure it's not already there
    for (long i=0; i<m_iCount; i++)
        if (m_ppNames[i] == pName)
            return;

    // Make sure there's room
    if ((m_iCount & 7) == 0)
    {
        long    iSize = (m_iCount + 8) * sizeof (NAME *);
        NAME    **ppNew = (NAME **)(m_ppNames == NULL ? m_pAllocHost->GetStandardHeap()->Alloc (iSize) :
                                                        m_pAllocHost->GetStandardHeap()->Realloc (m_ppNames, iSize));

        if (ppNew == NULL)
            return;

        m_ppNames = ppNew;
    }

    m_ppNames[m_iCount++] = pName;
}

////////////////////////////////////////////////////////////////////////////////
// NAMETAB::Undef

void NAMETAB::Undef (NAME *pName)
{
    // Find it
    for (long i=0; i<m_iCount; i++)
    {
        if (m_ppNames[i] == pName)
        {
            // Remove it.  Replace this slot with the one at the end, if this
            // isn't the one on the end...
            if (i < m_iCount - 1)
                m_ppNames[i] = m_ppNames[m_iCount - 1];

            m_iCount--;
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// NAMETAB::IsDefined

BOOL NAMETAB::IsDefined (NAME *pName)
{
    for (long i=0; i<m_iCount; i++)
        if (m_ppNames[i] == pName)
            return TRUE;

    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// NAMETAB::ClearAll

void NAMETAB::ClearAll (BOOL fFree)
{
    m_iCount = 0;
    if (fFree && (m_ppNames != NULL))
    {
        m_pAllocHost->GetStandardHeap()->Free (m_ppNames);
        m_ppNames = NULL;
    }
}



#ifdef DEBUG
/*
 * Print statistics on the hash table. This can be used to
 * discover whether the hash function is performing adequately.
 */
void NAMEMGR::PrintStats()
{
    unsigned iBucket;
    int cEmpty = 0, cOneEntry = 0, cTwoEntry = 0, cManyEntries = 0, maxEntries = 0;;

    // Acquire the lock
    CTinyGate   gate (&lock);

    for (iBucket = 0; iBucket < cBuckets; ++iBucket) {
        int cEntries = 0;
        PNAME name;

        // Count entries in the bucket.
        name = buckets[iBucket];
        while (name != NULL) {
            ++ cEntries;
            name = name->nextInBucket;
        }

        // Classify.
        if (cEntries == 0)
            ++cEmpty;
        else if (cEntries == 1)
            ++cOneEntry;
        else if (cEntries == 2)
            ++cTwoEntry;
        else
            ++cManyEntries;

        if (cEntries > maxEntries)
            maxEntries = cEntries;
    }

    // Print stats.
    printf("Total buckets:                %d\n", cBuckets);
    printf("Total names:                  %d\n", cNames);
    printf("\n");
    printf("Empty buckets:                %d (%.2f%%)\n", cEmpty, (double)cEmpty/cBuckets*100.0);
    printf("Buckets with one entry:       %d (%.2f%%)\n", cOneEntry, (double)cOneEntry/cBuckets*100.0);
    printf("Buckets with two entries:     %d (%.2f%%)\n", cTwoEntry, (double)cTwoEntry/cBuckets*100.0);
    printf("Buckets with 3+ entries:      %d (%.2f%%)\n", cManyEntries, (double)cManyEntries/cBuckets*100.0);
    printf("Max bucket size:              %d\n", maxEntries);
}
#endif //DEBUG

/*
//                                                                                      

long CTinyLock::m_iSpins = 0;
long CTinyLock::m_iSleeps = 0;
*/
