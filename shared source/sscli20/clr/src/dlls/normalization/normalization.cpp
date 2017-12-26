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

// Normalization.cpp
//
// This file contains the normalization class and implementation
//
// WARNING: This .DLL is supposed to be the same in managed and native code,
// starting with the Longhorn & Whidbey releases.  Please make sure they stay in sync.
//
// ERROR_SUCCESS                (no real error, mostly useful if IsNormalized returns false or NormalizeString returns 0 for 0 length string)
// ERROR_INVALID_PARAMETER      Bad input pointers or bad normalization form or other bad input
// ERROR_INSUFFICIENT_BUFFER    NormalizeString didn't have a big enough output buffer.  Try again using the opposite of the return value for the buffer size.
// ERROR_NO_UNICODE_TRANSLATION The input string had messed up Unicode data (bad surrogate pair sequences) or contained characters that are not valid
//                              for that normalization form.  The opposite of the return value is the index to the bad character.

#include "normalizationprivate.h"
#include "buffer.h"
#include "normalizationhelp.h"


// Keep some of them around
CNormalization  *CNormalization::pFormC = NULL;
CNormalization  *CNormalization::pFormD = NULL;
CNormalization  *CNormalization::pFormKC = NULL;
CNormalization  *CNormalization::pFormKD = NULL;
CNormalization  *CNormalization::pFormIdna = NULL;
CNormalization  *CNormalization::pFormCNoUnassigned = NULL;
CNormalization  *CNormalization::pFormDNoUnassigned = NULL;
CNormalization  *CNormalization::pFormKCNoUnassigned = NULL;
CNormalization  *CNormalization::pFormKDNoUnassigned = NULL;
CNormalization  *CNormalization::pFormIdnaNoUnassigned = NULL;


// Construction
CNormalization::CNormalization(Tables* pTables, NORM_FORM eNormalizationForm)
{
    _ASSERTE(pTables);

    // Load our data
    LoadTables(pTables);

    _ASSERTE(this->eNormalizationForm == (eNormalizationForm & ~DisallowUnassigned));

    // Set allow unassigned flag
    this->bAllowUnassignedCodePoints = (eNormalizationForm & DisallowUnassigned) == 0;
}

CNormalization::~CNormalization()
{
    // Note that managed view won't have this mapped.  Of course managed view
    // also won't call the destuctor because it just stops the program.
    if (this->pTablesPointer) UnmapViewOfFile(this->pTablesPointer);
}

void CNormalization::CleanUp()
{
    if (pFormC) delete pFormC;
    if (pFormD) delete pFormD;
    if (pFormKC) delete pFormKC;
    if (pFormKD) delete pFormKD;
    if (pFormIdna) delete pFormIdna;
    if (pFormCNoUnassigned) delete pFormCNoUnassigned;
    if (pFormDNoUnassigned) delete pFormDNoUnassigned;
    if (pFormKCNoUnassigned) delete pFormKCNoUnassigned;
    if (pFormKDNoUnassigned) delete pFormKDNoUnassigned;
    if (pFormIdnaNoUnassigned) delete pFormIdnaNoUnassigned;
}

// GetNormalization
CNormalization* CNormalization::GetNormalization(NORM_FORM eNormalizationForm)
{
    Tables*          pTables = NULL;
    CNormalization** ppUseForm = NULL;

    switch ((INTERNAL_NORM_FORM)eNormalizationForm)
    {
        case NormC:
            ppUseForm = &CNormalization::pFormC;
            break;
        case NormCDisallowUnassigned:
            ppUseForm = &CNormalization::pFormCNoUnassigned;
            break;
        case NormD:
            ppUseForm = &CNormalization::pFormD;
            break;
        case NormDDisallowUnassigned:
            ppUseForm = &CNormalization::pFormDNoUnassigned;
            break;
        case NormKC:
            ppUseForm = &CNormalization::pFormKC;
            break;
        case NormKCDisallowUnassigned:
            ppUseForm = &CNormalization::pFormKCNoUnassigned;
            break;
        case NormKD:
            ppUseForm = &CNormalization::pFormKD;
            break;
        case NormKDDisallowUnassigned:
            ppUseForm = &CNormalization::pFormKDNoUnassigned;
            break;
        case NormIdna:
            ppUseForm = &CNormalization::pFormIdna;
            break;
        case NormIdnaDisallowUnassigned:
            ppUseForm = &CNormalization::pFormIdnaNoUnassigned;
            break;

        default:
            // Have no clue what it is, return null
            return NULL;
    }

    // If we have it already, use it
    if (*ppUseForm) return *ppUseForm;

    // Don't have it, get table name
    _ASSERTE("Managed version expected to provide its own table pointer");

    // Should have pTables by now
    _ASSERTE(pTables);
    if (pTables == NULL)
        return NULL;

    // Go ahead and try to load our tables and make one
    return GetNormalization(eNormalizationForm, pTables, ppUseForm);
}

CNormalization* CNormalization::GetNormalization(NORM_FORM eNormalizationForm, Tables* pTables)
{
    _ASSERTE(pTables);

    if (pTables == NULL)
        return NULL;

    CNormalization** ppUseForm = NULL;

    switch ((INTERNAL_NORM_FORM)eNormalizationForm)
    {
        case NormC:
            ppUseForm = &CNormalization::pFormC;
            break;
        case NormCDisallowUnassigned:
            ppUseForm = &CNormalization::pFormCNoUnassigned;
            break;
        case NormD:
            ppUseForm = &CNormalization::pFormD;
            break;
        case NormDDisallowUnassigned:
            ppUseForm = &CNormalization::pFormDNoUnassigned;
            break;
        case NormKC:
            ppUseForm = &CNormalization::pFormKC;
            break;
        case NormKCDisallowUnassigned:
            ppUseForm = &CNormalization::pFormKCNoUnassigned;
            break;
        case NormKD:
            ppUseForm = &CNormalization::pFormKD;
            break;
        case NormKDDisallowUnassigned:
            ppUseForm = &CNormalization::pFormKDNoUnassigned;
            break;
        case NormIdna:
            ppUseForm = &CNormalization::pFormIdna;
            break;
        case NormIdnaDisallowUnassigned:
            ppUseForm = &CNormalization::pFormIdnaNoUnassigned;
            break;

        default:
            // Have no clue what it is, return null
            return NULL;
    }

    // Should have something now
    _ASSERTE(ppUseForm);

    // If we already have one, return it
    if (*ppUseForm) return *ppUseForm;

    // If we don't have one, build one
    return GetNormalization(eNormalizationForm, pTables, ppUseForm);
}

CNormalization* CNormalization::GetNormalization(
    NORM_FORM eNormalizationForm, Tables* pTables, CNormalization **ppUseForm)
{
    // It was missing, build it and exchange it
    CNormalization* pTemp = new CNormalization(pTables, eNormalizationForm);
    if (pTemp)
    {
        // Exchange it
        PVOID pResult = InterlockedCompareExchangePointer(
            (PVOID*)ppUseForm, pTemp, NULL);

        // We may have been beaten to it, so double check & delete it if necessary
        if (pResult) delete pTemp;
    }

    // Don't know what it is, return NULL
    return *ppUseForm;
}

bool CNormalization::IsValidForm(NORM_FORM eNormalizationForm)
{
    // Is it one we know about?
    switch((int)eNormalizationForm)
    {
        case NormalizationC:
        case NormalizationD:
        case NormalizationKC:
        case NormalizationKD:
        case NormIdna:
            return true;
        default:
            break;
    }

    // We don't know this form
    return false;
}

// This is basically simple stuff.  Nothings constructed.
bool CNormalization::LoadTables(Tables* pTables)
{
    _ASSERTE(pTables);

    // Get our tables
    this->pTablesPointer = pTables;
    BYTE*   pBytePointer = (BYTE*)this->pTablesPointer;

    // Basic info
    this->eNormalizationForm = (NORM_FORM)(this->pTablesPointer->normalizationForm);
    this->iMaxReplacementSize = this->pTablesPointer->maxReplacementSize;

    // Quick Lower Bounds Test
    this->iQuickLower = pTables->quickSkip;

    // Flags (right now we only have 1 flag, and its quite bogus so we don't use it)
    //   (pTables->flags != 0);

    // Get our table info
    this->pMainIndex    = (pBytePointer + pTables->offsetMainIndex);
    this->pCharInfo     = (TABLESEGMENT*)(pBytePointer + pTables->offsetCharInfo);

    // Remember our replacement character tables
    this->iReplacementHash = pTables->replacementHash;
    this->pReplacementHashTable =
        (WORD*)(pBytePointer + pTables->offsetReplacementHashTable);
    this->pReplacementIndex =
        (WORD*)(pBytePointer + pTables->offsetReplacementIndex);    // This idea is embedded in composition data table.
    this->pReplacementData =
        (WORD*)(pBytePointer + pTables->offsetReplacementData);

    // Remember our composition tables
    this->iCompositionHash = pTables->compositionHash;
    if (this->iCompositionHash == 0)
    {
        this->bDecomposeHangul = true;
        this->pCompositionIndex = 0;
        this->pCompositionData = 0;
    }
    else
    {
        this->bDecomposeHangul = false;
        this->pCompositionIndex = (WORD*)(pBytePointer + pTables->offsetCompositionIndex);
        this->pCompositionData  = (wchar_t*)(pBytePointer + pTables->offsetCompositionData);
    }

    // Don't forget our class map
    this->pClassMap = (pBytePointer + pTables->offsetClassMap);
    LoadClassMapExceptions();

    return true;
} // LoadTables

void CNormalization::LoadClassMapExceptions()
{
    // Make sure they're 0xff if not preset
    this->bClass216 = 0xff;
    this->bClass220 = 0xff;
    this->bClass221 = 0xff;
    this->bClass230 = 0xff;
    this->bClass231 = 0xff;

    BYTE bLastOne = 0;
    BYTE* pTemp = this->pClassMap;
    BYTE bTemp = 0;
    BYTE iIndex;
    for (iIndex = 0; (bTemp = *pTemp) >= bLastOne; iIndex++, pTemp++)
    {
        bLastOne = bTemp;
        if (bTemp == 216) this->bClass216 = iIndex;
        else if (bTemp == 220) this->bClass220 = iIndex;
        else if (bTemp == 221) this->bClass221 = iIndex;
        else if (bTemp == 230) this->bClass230 = iIndex;
        else if (bTemp == 231) this->bClass231 = iIndex;
    }
} // LoadClassMapExceptions


// Look up an actual page for our char.
BYTE CNormalization::TableLookup(lchar_t cTest, BYTE bPage)
{
    // We should make sure that whatever code gets here has already checked.  (Maybe in the normalize or normalizecount functions)
    // NOTE: Right now page 0 is special class 0 chars, so we have to do bPage-1
    // We shouldn't call this with page 0 (we should always test that case first).
    _ASSERTE(bPage != 0);
    _ASSERTE(cTest < NORM_FIRST_UNKNOWN_CHARACTER);
    _ASSERTE(this->pCharInfo);

    return this->pCharInfo[bPage-1][cTest & 0x7f];
} // TableLookup

// Look up our full character info in the table.
// If returns 0 can skip fast.
BYTE CNormalization::PageLookup(lchar_t cTest)
{
    // Look it up in the main table
    // We should make sure that whatever code gets here has already checked.  (Maybe in the normalize or normalizecount functions)
    _ASSERTE(this->pMainIndex);

    // Make sure we don't overrun our table, (Currently main table supports entire unicode range)
    _ASSERTE(cTest < NORM_FIRST_UNKNOWN_CHARACTER);

    return this->pMainIndex[cTest >> 7];
} // PageLookup

// Normalize
//
// This does most of the work of our Normalization.
//
// pszStringin  - Input String
// cchIn        - Input String Length (>=0)
// pszStringOut - Output String
// cchOut       - Size our output buffer
// pcchOut      - Receives actual # of output characters, or better buffer guess, or location of error.
//
// Return is windows error code:
//  ERROR_SUCCESS (0) if no problems
//  ERROR_INSUFFICIENT_BUFFER if buffer is too small.  pcchOut would then contain a better buffer size guess.
//  ERROR_NO_UNICODE_TRANSLATION if there's a bad unicode character.  pcchOut points to the location of the error in the input string.
//  ERROR_INVALID_PARAMETER if you pass null pointers or input string length < 0
DWORD CNormalization::Normalize( __in_ecount(cchIn) const wchar_t* pszStringIn, const unsigned int cchIn,
                                 __out_ecount_opt(cchOut) wchar_t* pszStringOut, const unsigned int cchOut, int* pcchOut)
{
    // Verify input arguments
    _ASSERTE(pszStringIn != NULL);
    _ASSERTE(pcchOut != NULL);
    _ASSERTE(pszStringOut != NULL);
    _ASSERTE(cchIn >= 0);           // Input length must be >= 0

    if (pszStringIn == NULL || pszStringOut == NULL || pcchOut == NULL) return ERROR_INVALID_PARAMETER;

    // Need our working buffer stuff
    CBuffer cBuffer((wchar_t*) pszStringIn, cchIn, pszStringOut, cchOut, this);

    // Just for officialiness
    *pcchOut = 0;

    // Positive thinking
    DWORD error = ERROR_SUCCESS;

    // Loop until we run out of string (either \0 or length (cchMaxIn) terminated)
    // Note that cchCountDownIn postdecrements, so in our loop it'll be 1 on the last character
    // Note that cchCountDownOut can be zero because all remaining input characters could be trimmed,
    //      so we test cchCountDownOut's space right before assigning to it.
    BYTE bPage;
    while (!cBuffer.EndOfInput() && error == ERROR_SUCCESS)
    {
        // Get our character
        lchar_t cTest = cBuffer.GetNextInputChar();

        //
        //  Quick Skip Test
        //

        // See if we can skip it quick.  In other words, non-decomposable start chars.
        // Do our quick bounds test.
        if (cTest < this->iQuickLower || (bPage = PageLookup(cTest)) == NORM_PAGE_NORMAL ||
            (bPage == NORM_PAGE_UNASSIGNED && this->bAllowUnassignedCodePoints))
        {
            // Append our character.
            if (!cBuffer.Append(cTest))
            {
                error = ERROR_INSUFFICIENT_BUFFER;
                break;
            }

            // Go to next character
            continue;
        }

        //
        // Do the harder processing
        //

        error = NormalizeCharacter(cTest, bPage, cBuffer);
    }

    // Did it work?
    if (error == ERROR_SUCCESS)
    {
        // Well, if we get here the string's been normalized.  Yippee.  Get our count
        // Presumably they wont have a string longer than 2^31 !!!
        *pcchOut = cBuffer.Length();
    }
    // If they had ERROR_INSUFFICIENT_BUFFER, give them a new buffer size guess
    else if (error == ERROR_INSUFFICIENT_BUFFER)
    {
        // Get our count and remaining.  Add 1 to remaining because its likely that
        // we ran out of space without completing the processing of the current char.
        int iCounted = cBuffer.ReadIn();
        int iLeft = cBuffer.LeftToRead() + 1;
        *pcchOut = this->GuessBetterCharCount(iCounted, iLeft, cBuffer.Length());
    }
    else
    {
        // Some other sort of error, tell them how far we got
        // Do -1 so the result is 0 indexed
        *pcchOut = cBuffer.ReadIn() - 1;
    }

    return error;
} // Normalize

// Process our character
DWORD CNormalization::NormalizeCharacter(lchar_t cTest, BYTE bPage, CBuffer& cBuffer)
{
    // We come back here if we have to correct the character before trying again.
TestPageInfo:

    if (bPage == NORM_PAGE_NORMAL)
    {
        if (!cBuffer.Append(cTest))
            return ERROR_INSUFFICIENT_BUFFER;

        return ERROR_SUCCESS;
    }

    // Try quick test for Hangul and surrogate pairs
    switch(bPage)
    {
        // We have special case information
        case NORM_PAGE_HANGUL:
            // Check if we're in composed or decomposed area
            if (cTest >= SBase)
            {
                // We may need to compose from Hangul LV to Hangul LVT...
                if (IsHangulS(cTest))
                {
                    // Decomposed? (Forms D & KD)
                    if (this->bDecomposeHangul == true)
                    {
                        // Add Hangul L & V
                        if (!cBuffer.Append(GetHangulL(cTest),0,0))
                            return ERROR_INSUFFICIENT_BUFFER;

                        wchar_t cStart = GetHangulV(cTest);
                        if (!cBuffer.Append(cStart,0,0))
                            return ERROR_INSUFFICIENT_BUFFER;

                        // See if we need to add Hangul T
                        wchar_t t = GetHangulT(cTest);
                        if (t != 0)
                        {
                            // It has a T, do we have room?
                            if (!cBuffer.Append(t,0,0))
                                return ERROR_INSUFFICIENT_BUFFER;

                            cStart = t;
                        }

                        // Decomposed Hangul are all start
                        cBuffer.MarkStart(cStart, 0, 0);
                        return ERROR_SUCCESS;
                    }

                    // Composed Hangul (C, KC & IDNA)
                    // Its at least LV, but do we need LV+T?
                    if (IsHangulLV(cTest))
                    {
                        // Its an LV, do we need LV+T?
                        // We want to compose our Jamos, so see if we can
                        if (!cBuffer.EndOfInput())
                        {
                            // Get next char and see if it composes
                            lchar_t cTestNext = cBuffer.GetNextInputChar();
                            cTestNext = ComposeHangulLVT(cTest, cTestNext);

                            // If it composes, use it instead of our current character
                            if (cTestNext)
                            {
                                // We need to replace the previous Hangul LV with the new LVT
                                // Its a start char, so readd it
                                cTest = cTestNext;
                            }
                            else
                            {
                                // Oops, it didn't combine. rewind
                                cBuffer.RewindInputWord();
                            }
                        }
                    }

                    // In any case its a start char, so jump back & add it
                    bPage = NORM_PAGE_NORMAL;
                    goto TestPageInfo;

                }

                bPage = NORM_PAGE_UNASSIGNED;
                goto TestPageInfo;
            }
            else
            {
                // Decomposed tables mark these as start, so decomposed should be false
                _ASSERTE(this->bDecomposeHangul == FALSE);

                // Can only compose if we're starting with Hangul L.
                if (IsHangulL(cTest))
                {
                    // Its an L, can we get an LV?
                    // We want to compose our Jamos, so see if we can
                    if (!cBuffer.EndOfInput())
                    {
                        // Get next char and see if it composes
                        lchar_t cTestNext = cBuffer.GetNextInputChar();
                        cTestNext = ComposeHangulLV(cTest, cTestNext);

                        // If it composes, use it instead of our current character
                        if (cTestNext)
                        {
                            // We need to replace the previous Hangul L with the new LV
                            cTest = cTestNext;

                            // We have an LV, can we get an LVT?
                            if (!cBuffer.EndOfInput())
                            {
                                // Get next char and see if it composes
                                cTestNext = cBuffer.GetNextInputChar();
                                cTestNext = ComposeHangulLVT(cTest, cTestNext);

                                // If it composes, use it instead of our current character
                                if (cTestNext)
                                {
                                    // We need to replace the previous Hangul LV with the new LVT
                                    cTest = cTestNext;
                                }
                                else
                                {
                                    // Didn't combine further, rewind
                                    cBuffer.RewindInputWord();
                                }
                            }
                        }
                        else
                        {
                            // Didn't compose, rewind
                            cBuffer.RewindInputWord();
                        }
                    }

                    // In any case we're going to add it as a start char
                    bPage = 0;
                    goto TestPageInfo;
                }


                if ((cTest >= 0x115A && cTest <= 0x115E) || (cTest >= 0x11A3 && cTest <= 0x11A7) ||
                    (cTest >= 0x11FA /*&& cTest <= 0x11FF*/)) // (always less than 11FF because of tables and we aren't base
                    bPage = NORM_PAGE_UNASSIGNED;
                else
                    bPage = NORM_PAGE_NORMAL;
                goto TestPageInfo;
            }
        case NORM_PAGE_HIGH_SURROGATE:
        {
            // Check surrogates first to make sure they're valid
            // Make sure we have input buffer space.  (ending nulls will be caught down below)
            if (cBuffer.EndOfInput())
            {
                // No characters left in buffer, it fails
                return ERROR_NO_UNICODE_TRANSLATION;
            }
            // See if our high surrogate's followed by a low surrogate (& update our pointers)
            // Have to preincrement pszTestString
            wchar_t cNext = cBuffer.GetNextInputChar();

            // This'll catch end of string stuff (\0) too.
            if (cNext < 0xDC00 || cNext > 0xDFFF)
            {
                // Invalid surrogate pair
                return ERROR_NO_UNICODE_TRANSLATION;
            }

            // Get our surrogate value:
            cTest = GetSurrogate(cTest, cNext);

            // Make sure its a surrogate
            _ASSERTE(cTest >= 0x10000);

            // Make sure its in unicode range
            _ASSERTE(cTest < NORM_FIRST_UNKNOWN_CHARACTER);  // Redundent, we know all of them right now

            // Look up new char info
            bPage = PageLookup(cTest);

            // Try again with new char
            goto TestPageInfo;
        }
        case NORM_PAGE_CHANGED:
            // This page of characters all changes, so we have to lookup their decomposition
            // Add it and clean it up
            return AppendDecomposedChar(cTest, cBuffer);

        case NORM_PAGE_UNASSIGNED:
            // Are unassigned allowed?
            if (!this->bAllowUnassignedCodePoints)
                return ERROR_NO_UNICODE_TRANSLATION;

            // They're allowed, fall through to normal
        case NORM_PAGE_NORMAL:
            // Append our character.
            if (!cBuffer.Append(cTest))
                return ERROR_INSUFFICIENT_BUFFER;

            cBuffer.MarkStart(cTest,0,0);
            return ERROR_SUCCESS;

        case NORM_PAGE_INVALID:
            // Invalid character
            return ERROR_NO_UNICODE_TRANSLATION;

        default:
            // If we get here we should just be a class
            _ASSERTE(bPage < NORM_PAGE_STARTFLAGS);
    }

    // If it wasn't a QuickSkip we'll have to try slower version, still want to skip if we can.
    // Note that bInfo will be a different format than up above.
    BYTE bInfo = TableLookup(cTest, bPage);
    if (bInfo == 0)
    {
        // Append our character.  0 is no characters appended (buffer overflow)
        if (!cBuffer.Append(cTest, 0, 0))
            return ERROR_INSUFFICIENT_BUFFER;

        // This'll be a start char
        cBuffer.MarkStart(cTest, 0, 0);
        return ERROR_SUCCESS;
    }

    // Get its interesting flags
    // Now we have all of this character's interesting info, so we want to remember
    // this in cLastTest, etc. so that we don't have to look it up again.  Since this
    // Isn't a boring start character, its likely the next one isn't either.
    // We remember it at return ERROR_SUCCESS;:  so you'll see goto return ERROR_SUCCESS;:s after this.
    BYTE bFlags = bInfo & NORM_FLAGS_MASK;
    BYTE bClass = bInfo & NORM_CLASS_MASK;

    if ( bClass == 0 || bClass == 63 )
    {
        // No real class, so its a special case character
        switch (bInfo)
        {
            case NORM_CHAR_START_MAY_COMBINE_NON_START:
            case NORM_CHAR_START_MAY_COMBINE_START:
                // We don't care about these until we get to next character, just add it
                break;

            case NORM_CHAR_START_MAY_COMBINE_PREVIOUS:
            {
                // Need to see if our previous character composes with this one.
                lchar_t cTemp = this->CanCombinableCharactersCombine(
                    cBuffer.GetLastChar(), cTest);

                if (cTemp != 0)
                {
                    _ASSERTE(cTemp != cBuffer.GetLastChar());

                    // This combined, stick it in
                    cBuffer.RewindOutputCharacter();

                    // Go back to TestPageInfo to add it.
                    cTest = cTemp;
                    bInfo = PageLookup(cTest);
                    goto TestPageInfo;
                }

                // Didn't combine, just add it
                break;
            }

            case NORM_CHAR_DIFFERENT:
                // Add it and clean it up
                return AppendDecomposedChar(cTest, cBuffer);

            case NORM_CHAR_ILLEGAL:
                // illegal character
                // We allow a single \0 at the end of a string even if its marked as illegal
                // (this is important for IDNA)
                if (cTest == 0 && cBuffer.EndOfInput())
                {
                    // This one's OK, break to add it
                    break;
                }
                return ERROR_NO_UNICODE_TRANSLATION;

            case NORM_CHAR_UNASSIGNED:
                // unassigned code point, are we allowing them?
                if (!this->bAllowUnassignedCodePoints)
                    return ERROR_NO_UNICODE_TRANSLATION;

                // Append our character.  0 is no characters appended (buffer overflow)
                if (!cBuffer.Append(cTest, 0, 0))
                    return ERROR_INSUFFICIENT_BUFFER;

                // This'll be a start char
                cBuffer.MarkStart(cTest, 0, 0);
                return ERROR_SUCCESS;

            default:
                // Shouldn't ever happen.
                _ASSERTE(!"Unknown Character attributes Found");
                break;
        }

        // Go ahead and add it.  Its a start character
        if (!cBuffer.Append(cTest, bClass, bFlags))
            return ERROR_INSUFFICIENT_BUFFER;
        return ERROR_SUCCESS;
    }

    //
    // Not a pseudo class 0 or 63 special case
    // Character has a real class.
    //

    // Start characters return ERROR_SUCCESS, but remember them for next time.
    if (bFlags == NORM_FLAG_START_MAY_DECOMPOSE ||
        bFlags == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE)
    {
        // These could decompose and/or combine if needed.  Won't know until our
        // next character.  Go ahead and add our character to our string
        if (!cBuffer.Append(cTest, bClass, bFlags))
            return ERROR_INSUFFICIENT_BUFFER;

        // If we got here it was a start character
        cBuffer.MarkStart(cTest, bClass, bFlags);
        return ERROR_SUCCESS;
    }

    // We're a non-start char, which may or may not combine with previous characters
    _ASSERTE( bFlags == NORM_FLAG_NON_START ||
              bFlags == NORM_FLAG_NON_START_COMBINES_WITH_PREVIOUS );

    // We need to know stuff about our last character, we can tell if it was some form
    // of start character or if it was also a non-start character.  If its a start character
    // we'll need to remember its info for our remaining non-start character tests.

    // See if we know about the previous character
    cBuffer.GetLastChar();

    // If Last char wasn't non-start, then remember its start info
    cBuffer.VerifyLastStart();

    // If we're a NORM_FLAG_NON_START we can't combine, so we just need to add ourselves in order,
    // also can't combine if we were just start or combine previous.  (Can't be Illegal or Different)
    if ( bFlags == NORM_FLAG_NON_START ||
            cBuffer.LastStartInfo() == NORM_CHAR_START ||
            cBuffer.LastStartInfo() == NORM_CHAR_START_MAY_COMBINE_PREVIOUS )
    {
        // Just add it and make sure its sorted OK.
        if (!cBuffer.AppendAndSortNonStart( cTest, bClass, bFlags ))
            return ERROR_INSUFFICIENT_BUFFER;
        return ERROR_SUCCESS;
    }

    // Now we have all of our interesting information, so we can tell if our last
    // character was a start character.  If it is we'll want to set the start info.
    if ( cBuffer.LastClass() == 0 || cBuffer.LastClass() == 63 )
    {
        // These are class 0 or 63 flags and all have start info to remember
        // NORM_CHAR_START                          (Should've been shortcut in last if)
        // NORM_CHAR_START_MAY_COMBINE_NON_START
        // NORM_CHAR_START_MAY_COMBINE_START
        // NORM_CHAR_START_MAY_COMBINE_PREVIOUS     (Should've been shortcut in last if)
        // NORM_CHAR_ILLEGAL                        (Should've been error before now)
        // NORM_CHAR_DIFFERENT                      (Should've been replaced before now)
        _ASSERTE(cBuffer.LastFlags() == NORM_CHAR_START_MAY_COMBINE_NON_START ||
                    cBuffer.LastFlags() == NORM_CHAR_START_MAY_COMBINE_START);

        // Ok, this is the first combinable non-start following a start that could combine
        // So we have to try to combine, but we don't have to worry about reducing or
        // any other non-starts between us and the start.

        // If it is combinable we can check if it combines with us
        // Remember this is 1st combinable character after start char, so
        // we don't have to worry about class order.

        // See if we combine with it (its a boring class 0 start if its a start)
        lchar_t cTemp = CanCombinableCharactersCombine(cBuffer.LastStart(), cTest);
        if (cTemp)
        {
            _ASSERTE(cTemp != cBuffer.LastStart());

            // Go ahead and stick it in our old place, we should've already added
            // at least one character to our string
            cBuffer.RewindOutputCharacter();
            cTest = cTemp;
            GetCharacterInfo(cTest, bClass, bFlags );

            // Drop through a couple lines to readd it and flag it as start.
        }

        // Didn't combine, add our character and return ERROR_SUCCESS.
        // Next time (if any) will call LastWasStart() if appropriate.
        if (!cBuffer.Append(cTest, bClass, bFlags))
            return ERROR_INSUFFICIENT_BUFFER;

        return ERROR_SUCCESS;
    }
    // We're combinable non-start, last was decomposable start?
    else if (cBuffer.LastFlags() == NORM_FLAG_START_MAY_DECOMPOSE ||
             cBuffer.LastFlags() == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE)
    {
        // Ok, last character was a decomposible start, that may or may not combine anyway

        // Could combine with it as it is, which would be easiest
        if (cBuffer.LastFlags() == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE)
        {
            // May combine with previous char.  1st check easy combine
            // See if we combine with it
            lchar_t cTemp = CanCombinableCharactersCombine(cBuffer.LastStart(), cTest);
            if (cTemp)
            {
                _ASSERTE(cTemp != cBuffer.LastStart());

                // Go ahead and stick it in our old place, we should've already added
                // at least one character to our string
                cTest = cTemp;
                cBuffer.RewindOutputCharacter();

                // We'll need its new info
                GetCharacterInfo(cTest, bClass, bFlags );

                // Append it again
                if (!cBuffer.Append(cTest, bClass, bFlags))
                    return ERROR_INSUFFICIENT_BUFFER;

                // Flag it as start, can return ERROR_SUCCESS since we added it
                cBuffer.LastWasStart();
                return ERROR_SUCCESS;
            }
        }

        // Didn't combine easily, have to check base character, if our
        // characters class is lower (otherwise existing composition is
        // preferred)
        if (cBuffer.LastClass() > bClass)
        {
            bool bMayCombineStartBase = true;
            lchar_t cCombinedStartPair = 0;

            // Special case for "class 221" combinable characters.
            // If we're class 220 and our start char is class 221,
            // then our start char's classes are 0 216 230 and we
            // might create a 0 216 220 instead
            if (cBuffer.LastClass() == this->bClass221)
            {
                // We're really class 216
                if (bClass >= this->bClass216)
                {
                    // We're class 218 or 220, but despite the 221, our
                    // start class is actually class 216 and we can't combine
                    bMayCombineStartBase = false;

                    // If we'e a 220, then we may combine with 1st pair
                    if (bClass == this->bClass220)
                    {
                        // We may combine with the start first pair
                        cCombinedStartPair = CanCombinableCharactersCombine( cBuffer.LastStartBasePair(), cTest);
                    }
                }
            }
            // Similarly if our start is "class 231" then it is really
            // class 0 230 240 and it may combine to 0 230 230 instead
            else if (cBuffer.LastClass() == this->bClass231)
            {
                // We're really class 230
                if (bClass == this->bClass230)   // (can't be higher)
                {
                    // We're class 230, we can't combine with the start base,
                    // but we might combine with the first pair.
                    bMayCombineStartBase = false;

                    // We may combine with the start first pair
                    cCombinedStartPair = CanCombinableCharactersCombine( cBuffer.LastStartBasePair(), cTest);
                }
            }

            // See if we combined somehow
            if (cCombinedStartPair != 0)
            {
                // This should be a plane 0 character
                _ASSERTE(cCombinedStartPair != cBuffer.LastStart());
                _ASSERTE(cCombinedStartPair <= 0xFFFF);

                // We need to merge with the start pair (0 216 230) -> (0 216 220) 230
                // or (0 230 240) -> (0 230 230) 240 ?-> (0 230 230 240)

                // Replace our character with our new start character (easy because we know we're plane 0)
                // Get info for our new character
                GetCharacterInfo(cCombinedStartPair, bClass, bFlags);

                // Update our last start character
                if (!cBuffer.ReplaceLastStartBasePair(cCombinedStartPair, bClass, bFlags))
                    return ERROR_INSUFFICIENT_BUFFER;

                // Ok, we're done, may need to call resort in case one of the others combines with us
                _ASSERTE(bFlags == NORM_FLAG_START_MAY_DECOMPOSE ||
                         bFlags == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE);
                _ASSERTE(bClass > 0 && bClass < 63);
                if (bFlags == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE)
                {
                    cBuffer.RecheckStartCombinations();
                }
                return ERROR_SUCCESS;
            }

            // See if we're allowed to combine with the start base character
            if (bMayCombineStartBase)
            {
                // Wasn't fancy, see if they combine
                lchar_t cTemp = CanCombinableCharactersCombine( cBuffer.LastStartBase(), cTest);
                if (cTemp)
                {
                     _ASSERTE(cTemp != cBuffer.LastStart());

                    // Oops, they combine, figure out how to combine them
                    // After combining we may have 1 or 2 extra diacritics to check.
                    // The only case with 4 classes is 0 230 230 240, so something < 230 would have
                    // to combine with the base character, which is literally all greek so nothing
                    // else combines with it, its only combinable classes are 230 or 240.

                    // Get our character info (since its a start it'll want that.)
                    cTest = cTemp;
                    GetCharacterInfo(cTest, bClass, bFlags);

                    if (!cBuffer.ReplaceLastStartBase(cTemp, bClass, bFlags))
                        return ERROR_INSUFFICIENT_BUFFER;

                    // We need to retest from Last Start to make sure they don't
                    // combine with us now that we combined more.  They won't combine
                    // with the base (because they already tested that) but they may
                    // combine with the new character.  0397 0345 0314 -> 1F89 would
                    // be an example of similar behavior.  This would need some
                    // (0 230) 220 -> (0 220) 230 -> (0 220 230).  Like probably
                    // (Latin Char + Dot Above) Dot Below.

                    // We only have to do this if the new character is combinable.
                    // Also we know it has to be decomposable (because we just composed it)
                    _ASSERTE(bFlags == NORM_FLAG_START_MAY_DECOMPOSE ||
                             bFlags == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE);
                    _ASSERTE(bClass > 0 && bClass < 63);
                    if (bFlags == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE)
                    {
                        cBuffer.RecheckStartCombinations();
                    }

                    return ERROR_SUCCESS;
                }
            }
        }

        // Couldn't combine, Last character was start so we have to be in order
        // so just append us
        if (!cBuffer.Append(cTest, bClass, bFlags))
            return ERROR_INSUFFICIENT_BUFFER;
        return ERROR_SUCCESS;
    }

    // Current and last characters are both non-start.  Last start had some combinable form.

    //
    // Last character was non-start (as is this character)
    // This character may combine whether or not previous one did
    //
    //      NORM_FLAG_NON_START
    //      NORM_FLAG_NON_START_COMBINES_WITH_PREVIOUS
    //
    _ASSERTE(cBuffer.LastFlags() == NORM_FLAG_NON_START ||
             cBuffer.LastFlags() == NORM_FLAG_NON_START_COMBINES_WITH_PREVIOUS );

    // If we're blocked we can't do anything but insert ourselves into the appropriate place
    if (cBuffer.IsBlocked(bClass))
    {
        // We're blocked, insert us in our last unblocked place.
        if (!cBuffer.InsertAtBlockedLocation(cTest, bClass, bFlags))
            return ERROR_INSUFFICIENT_BUFFER;
        return ERROR_SUCCESS;
    }

    // Now we know we aren't blocked and we may combine with the last start or part of
    // the last start.

    // We may have simple combinations with our start character, so try that first
    if ( cBuffer.LastStartClass() == 0 || cBuffer.LastStartClass() == 63 ||
         cBuffer.LastStartFlags() == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE )
    {
        // These are class 0 or 63 flags and all have start info to remember
        // NORM_CHAR_START                          (Should've been shortcut in last if)
        // NORM_CHAR_START_MAY_COMBINE_NON_START
        // NORM_CHAR_START_MAY_COMBINE_START
        // NORM_CHAR_START_MAY_COMBINE_PREVIOUS     (Should've been shortcut in last if)
        // NORM_CHAR_ILLEGAL                        (Should've been error before now)
        // NORM_CHAR_DIFFERENT                      (Should've been replaced before now)
        _ASSERTE( cBuffer.LastStartFlags() == NORM_CHAR_START_MAY_COMBINE_NON_START ||
                  cBuffer.LastStartFlags() == NORM_CHAR_START_MAY_COMBINE_START ||
                  ( cBuffer.LastStartClass() > 0 && cBuffer.LastStartClass() < 63 &&
                    cBuffer.LastStartFlags() == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE ) );

        // Ok, this is the first combinable non-start following a start that could combine
        // So we have to try to combine, but we don't have to worry about reducing or
        // any other non-starts between us and the start.

        // If it is combinable we can check if it combines with us
        // Remember this is 1st combinable character after start char, so
        // we don't have to worry about class order.

        // See if we combine with it (its a boring class 0 start if its a start)
        lchar_t cTemp = CanCombinableCharactersCombine(cBuffer.LastStart(), cTest);
        if (cTemp)
        {
            // They can combine, and so far we're allowed to combine with it
            // Combine it with the start character and return ERROR_SUCCESS
            _ASSERTE((cBuffer.LastStart() <= 0xFFFF) && (cTemp <= 0xFFFF));
            _ASSERTE(cTemp != cBuffer.LastStart());

            // Get info for our new character
            cTest = cTemp;
            GetCharacterInfo(cTest, bClass, bFlags);

            // Update our last start character - base didn't change
            cBuffer.SetLastStart(cTest, bClass, bFlags, false);

            // We need to retest from Last Start to make sure they don't
            // combine with us now that we combined more.  They won't combine
            // with the base (because they already tested that) but they may
            // combine with the new character.  0397 0345 0314 -> 1F89 would
            // be an example of that behavior.

            // We only have to do this if the new character is combinable.
            // Also we know it has to be decomposable (because we just composed it)
            _ASSERTE(bFlags == NORM_FLAG_START_MAY_DECOMPOSE ||
                     bFlags == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE);
            _ASSERTE(bClass > 0 && bClass < 63);
            if (bFlags == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE)
            {
                cBuffer.RecheckStartCombinations();
            }

            // Don't add pointer, we merged with start char, return ERROR_SUCCESS
            // Also didn't displace any other non-starts
            return ERROR_SUCCESS;
        }

        // Didn't combine, may need to add our character and return ERROR_SUCCESS, remembering our start info
        // We only add it for NORM_CHAR_START_MAY_COMBINE_NON_START and NORM_CHAR_START_MAY_COMBINE_START,
        // NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE may have decompositions to help
        if ( cBuffer.LastStartClass() == 0 || cBuffer.LastStartClass() == 63 )
        {
            if (!cBuffer.InsertAtBlockedLocation(cTest, bClass, bFlags))
                return ERROR_INSUFFICIENT_BUFFER;
            return ERROR_SUCCESS;
        }
    }

    // Our last class should now be a decompose or decompose and combine.  We should be
    // a NORM_FLAG_NON_START_COMBINES_WITH_PREVIOUS.
    _ASSERTE(cBuffer.LastStartFlags() == NORM_FLAG_START_MAY_DECOMPOSE ||
             cBuffer.LastStartFlags() == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE);
    _ASSERTE(bFlags == NORM_FLAG_NON_START_COMBINES_WITH_PREVIOUS &&
             bClass > 0 && bClass < 63);

    // Ok, now we know we might be able to combine with part of a decomposition, if
    // we combined with the whole character we'd have done that by now.

    // If we're a higher class or same than the start character we can't combine (because its current
    // composition would be preferrable)
    if (cBuffer.LastStartClass() <= bClass)
    {
        // Current composition would be preferrable anyway, insert our character
        if (!cBuffer.InsertAtBlockedLocation(cTest, bClass, bFlags))
            return ERROR_INSUFFICIENT_BUFFER;
        return ERROR_SUCCESS;
    }

    // First we might combine with the base pair, which would be best
    bool bMayCombineStartBase = true;
    lchar_t cCombinedStartPair = 0;

    // Special case for "class 221" combinable characters.
    // If we're class 220 and our start char is class 221,
    // then our start char's classes are 0 216 230 and we
    // might create a 0 216 220 instead
    if (cBuffer.LastStartClass() == this->bClass221)
    {
        // We're really class 216
        if (bClass >= this->bClass216)
        {
            // We're class 218 or 220, but despite the 221, our
            // start class is actually class 216 and we can't combine
            bMayCombineStartBase = false;

            // If we'e a 220, then we may combine with 1st pair
            if (bClass == this->bClass220)
            {
                // We may combine with the start first pair
                cCombinedStartPair = CanCombinableCharactersCombine( cBuffer.LastStartBasePair(), cTest);
            }
        }
    }
    // Similarly if our start is "class 231" then it is really
    // class 0 230 240 and it may combine to 0 230 230 instead
    else if (cBuffer.LastStartClass() == this->bClass231)
    {
        // We're really class 230
        if (bClass == this->bClass230)   // (can't be higher)
        {
            // We're class 230, we can't combine with the start base,
            // but we might combine with the first pair.
            bMayCombineStartBase = false;

            // We may combine with the start first pair
            cCombinedStartPair = CanCombinableCharactersCombine( cBuffer.LastStartBasePair(), cTest);
        }
    }

    // Did we combine with the start pair? (special case for 0 216 230 & 0 230 240 classes only)
    if (cCombinedStartPair)
    {
        // This should be a plane 0 character
        _ASSERTE(cCombinedStartPair <= 0xFFFF);
        _ASSERTE(cCombinedStartPair != cBuffer.LastStart());

        // We need to merge with the start pair (0 216 230) -> (0 216 220) 230
        // or (0 230 240) -> (0 230 230) 240 ?-> (0 230 230 240)

        // Replace our character with our new start character (easy because we know we're plane 0)
        // Get info for our new character
        GetCharacterInfo(cCombinedStartPair, bClass, bFlags);

        // Update our last start character, this'll have to add the extra 3rd pair to
        // our decomposed string.  Only cases are classes (0 216 230) and (0 230 230),
        // so the extra 230 will need remembered, this does that for us:
        if (!cBuffer.ReplaceLastStartBasePair(cCombinedStartPair, bClass, bFlags))
            return ERROR_INSUFFICIENT_BUFFER;

        // We need to retest from Last Start to make sure they don't
        // combine with us now that we combined more.  They won't combine
        // with the base (because they already tested that) but they may
        // combine with the new character.  0397 0345 0314 -> 1F89 would
        // be an example of that behavior.

        // We only have to do this if the new character is combinable.
        // Also we know it has to be decomposable (because we just composed it)
        _ASSERTE(bFlags == NORM_FLAG_START_MAY_DECOMPOSE ||
                 bFlags == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE);
        _ASSERTE(bClass > 0 && bClass < 63);
        if (bFlags == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE)
        {
            cBuffer.RecheckStartCombinations();
        }

        // Ok, we're done with this one
        return ERROR_SUCCESS;
    }

    // We may be able to combine with the start character's base character
    // An example would be 0041 0302 0300 0323 changing to 1EAC 0300 should hit this.
    // (Actually more like 0041 0232 0302 0300 0323 because it has to have non-starts to get here)
    // (1EA6 0323 would be an intermediary step when we get here)
    if (bMayCombineStartBase)
    {
        // Wasn't fancy, see if they combine
        lchar_t cTemp = CanCombinableCharactersCombine( cBuffer.LastStartBase(), cTest);
        if (cTemp)
        {
            _ASSERTE(cTemp != cBuffer.LastStart());

            // Oops, it combines with the start base character
            // Worst case is something like our example where 2 characters need to be
            // reappended after the current character gets added.  We could also merely
            // be adding a single character. (it was 0 230 - 220 and now 0 220 - 230)
            // After combining we may have 1 or 2 extra diacritics to check.
            // The only case with 4 classes is 0 230 230 240, so something < 230 would have
            // to combine with the base character, which is literally all greek so nothing
            // else combines with it, its only combinable classes are 230 or 240.
            cTest = cTemp;
            GetCharacterInfo(cTest, bClass, bFlags);

            if (!cBuffer.ReplaceLastStartBase(cTemp, bClass, bFlags))
                return ERROR_INSUFFICIENT_BUFFER;

            // We need to retest from Last Start to make sure they don't
            // combine with us now that we combined more.  They won't combine
            // with the base (because they already tested that) but they may
            // combine with the new character.  0397 0345 0314 -> 1F89 would
            // be an example of that behavior.

            // We only have to do this if the new character is combinable.
            // Also we know it has to be decomposable (because we just composed it)
            _ASSERTE(bFlags == NORM_FLAG_START_MAY_DECOMPOSE ||
                     bFlags == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE);
            _ASSERTE(bClass > 0 && bClass < 63);
            if (bFlags == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE)
            {
                cBuffer.RecheckStartCombinations();
            }

            return ERROR_SUCCESS;
        }
    }

    // Couldn't find anywhere to combine, insert our character
    if (!cBuffer.InsertAtBlockedLocation(cTest, bClass, bFlags))
        return ERROR_INSUFFICIENT_BUFFER;
    return ERROR_SUCCESS;

} // NormalizeCharacter (CNormalization)

// Need to figure out our error codes
// Note that we probably don't usually have to sort class orders to count these.
DWORD CNormalization::GuessCharCount( __in_ecount(cchMaxIn) const wchar_t* pszStringIn, const int cchMaxIn, int* pcchOut)
{
    // Verify input arguments
    _ASSERTE(pszStringIn != NULL);
    _ASSERTE(pcchOut != NULL);

    if (pszStringIn == NULL || pcchOut == NULL) return ERROR_INVALID_PARAMETER;
    if (cchMaxIn < 0) return ERROR_INVALID_PARAMETER;

    // Start with nothing
    *pcchOut = 0;

    // Get our string length
    int iInCount = (int)cchMaxIn;

    // So for a buffer size of iInCount, what do we recommend?
    *pcchOut = GuessCharCount(iInCount);

    return ERROR_SUCCESS;
} // GuessCharCount ( pszStringIn, cchMaxIn, pcchOut)

// Guess a character count for our size.
// Instances of Length of Replacement Strings For Form C:
//      1 characters - 801 instances
//      2 characters - 856 instances
//      3 characters - 220 instances
//      4 characters - 40 instances
//      5 characters - 0 instances
//      6 characters - 9 instances
// Remember that many of the 2, 3 & 4 character replacement strings
// only decompose if they'll recombine with at least one other character
int CNormalization::GuessCharCount(const int iInputSize)
{
    // return 112.5% of the current size.
    int iGuess = iInputSize + (iInputSize >> 3);

    // pick a minimum of 64 chars (really short strings could vary wildly)
    // some short strings could be really confused
    if (iGuess < 64)
    {
        iGuess = this->iMaxReplacementSize * iInputSize;

        if (iGuess > 64)
        {
            iGuess = 64;
        }
    }

    return iGuess;
} // GuessCharCount

// Guess a better character count for our size
int CNormalization::GuessBetterCharCount(int iCharCounted, int iCharLeft, int iCurrentCount)
{
    _ASSERTE(iCharCounted >= 0);
    _ASSERTE(iCharLeft > 0);
    _ASSERTE(iCurrentCount >= 0);

    // Don't wanna divide by zero
    if (iCharCounted == 0)
    {
        // Assume we had 1 of the largest character then.
        iCharCounted = 1;
        iCurrentCount += this->iMaxReplacementSize;
    }

    // Pick a value for the remainder that's either our average so far
    // or the guesscharcount, whichever's higher.
    int iRemainingGuess = GuessCharCount(iCharLeft);
    long iNewRemainingGuess = (iCurrentCount * iCharLeft) / iCharCounted;

    // Really, this should almost always be false.  Averages are expected to be lower than
    // the guess, so if this isn't true their string is really wierd (probably a stress tester
    // trying all maximum replacement character strings).
    if (iNewRemainingGuess > iRemainingGuess) iRemainingGuess = iNewRemainingGuess;

    // We really don't have to want to do this again, so pick a guess that's bigger
    // than we'd expect in case something wierd happens at the end of the string.
    // So pick 112.5% of what we expect the new part to be.
    iRemainingGuess += iRemainingGuess >> 3;
    int iNewGuess = iCurrentCount + iRemainingGuess;

    return iNewGuess;
} // GuessBetterCharCount

// Need to figure out our error codes
// Note that we probably don't usually have to sort class orders to count these.
// Returns ERROR_SUCCESS if normalized, -1 if not normalized or Windows Error if appropriate:
// ERROR_SUCCESS = true;
// -1 = false = ERROR_FAIL
// ERROR_INVALID_PARAMETER - bad inputs
// ERROR_NO_UNICODE_TRANSLATION - bad data in input string
DWORD CNormalization::IsNormalized( __in_ecount(cchMaxIn) const wchar_t* pszStringIn, int cchMaxIn )
{
    // Verify input arguments
    if (pszStringIn == NULL) return ERROR_INVALID_PARAMETER;

    // Temporary variables
    wchar_t*        pszTestString = (wchar_t*) pszStringIn;
    lchar_t         cTest;

    // Info about last char
    lchar_t         cLastTest = 0;
    BYTE            bLastClass = 0;
    BYTE            bLastFlags = 0;
    wchar_t*        pszLast = pszTestString - 1;        // Last character was at before beginning of string
                                                    // (Don't dereference because of this behavior)
                                                    // But we know chars after this are always valid
                                                    // so if pszTestString > pszLast + 1, then *(pszTestString-1) is valid
    // Info about last known start char
    lchar_t         cLastStart = 0;
    lchar_t         cLastStartBase = 0;
    lchar_t         cLastStartBasePair = 0;
    BYTE            bLastStartClass = 0;
    BYTE            bLastStartFlags = 0;
    wchar_t*        pszLastStart = pszLast;

    // Loop until we run out of string (either \0 or length (cchMaxIn) terminated)
    // Note that cchMaxIn postdecrements, so in our loop it'll be 1 on the last character
    for (   ; cchMaxIn > 0;
            pszTestString++, cchMaxIn--)
    {
        // Get our character
        cTest = *(pszTestString);

        // See if we can skip it quick
        // Do our quick bounds test.
        if (cTest < this->iQuickLower) continue;

        // Look in 1st table and see if we can skip this
        BYTE bPage;
        if ((bPage = PageLookup(cTest)) == NORM_PAGE_NORMAL ||
            (bPage == NORM_PAGE_UNASSIGNED && this->bAllowUnassignedCodePoints)) continue;

        // Try quick test for Hangul and surrogate pairs
TestPageInfo:
        switch(bPage)
        {
            // We have special case information
            case NORM_PAGE_HANGUL:
                if (cTest >= SBase)
                {
                    // Are we trying to decompose Hangul, and if so, is this
                    // actually a decomposable Hangul syllable?  (We have a few
                    // undefined characters at the end of this range)
                    // Also, if we compose Hangul, make sure we can't compose
                    // it further
                    //
                    if (IsHangulS( cTest))
                    {
                        // Composed or decomposed Hangul?
                        if (this->bDecomposeHangul == true)
                        {
                            // Form D or KD is supposed to decompose hangul S characters, not normalized
                            return ERROR_FALSE;
                        }

                        // Form C or KC or IDNA Normalized

                        // If we require composed hangul we need to check if its an LV+T combination
                        // Any more characters in string?
                        if (cchMaxIn > 1)
                        {
                            // Get next char and see if it composes
                            wchar_t cTestNext = *(pszTestString + 1);
                            if (CanComposeHangul(cTest, cTestNext))
                            {
                                // Oops, it composes further (it was an LV + T combination)
                                return ERROR_FALSE;
                            }
                        }

                        cLastTest = cTest;
                        bLastClass = 0;
                        bLastFlags = 0;
                        pszLast = pszTestString;
                        continue;
                    }

                    bPage = NORM_PAGE_UNASSIGNED;
                    goto TestPageInfo;
                }
                else
                {
                    // decomposed Jamos
                    // Decomposed tables mark these as start, so decomposed should be false
                    _ASSERTE(this->bDecomposeHangul == FALSE);

                    // This block contains hangul L jamos, but still need to look
                    // up misses in main table because there are invalid an non-L characters here.

                    // We're form C or KC or IDNA, so we want to compose our Jamos, so see if we can
                    if (cchMaxIn > 1)
                    {
                        // Get next char and see if it composes
                        wchar_t cTestNext = *(pszTestString + 1);
                        if (CanComposeHangul(cTest, cTestNext))
                        {
                            // Oops, it composes further (it was an L + V combination)
                            return ERROR_FALSE;
                        }

                        // Its boring composed
                        bPage = NORM_PAGE_NORMAL;
                        goto TestPageInfo;
                    }

                    // There're holes in this block, some are unassigned, rest are NORMAL

                    if ((cTest >= 0x115A && cTest <= 0x115E) || (cTest >= 0x11A3 && cTest <= 0x11A7) ||
                        (cTest >= 0x11FA /*&& cTest <= 0x11FF*/)) // (always less than 11FF because of tables and we aren't base
                        bPage = NORM_PAGE_UNASSIGNED;
                    else
                        bPage = NORM_PAGE_NORMAL;
                    goto TestPageInfo;
                }

            case NORM_PAGE_HIGH_SURROGATE:
            {
                // Check surrogates first to make sure they're valid
                // Make sure we have buffer space.  (ending nulls will be caught down below)
                if (cchMaxIn <= 1)
                {
                    // No characters left in buffer, it fails
                    return ERROR_NO_UNICODE_TRANSLATION;
                }
                // See if our high surrogate's followed by a low surrogate (& update our pointers)
                // Have to preincrement pszTestString
                ++pszTestString;
                wchar_t cNext = *(pszTestString);
                cchMaxIn--;
                // This'll catch end of string stuff too.
                if (cNext < 0xDC00 || cNext > 0xDFFF)
                {
                    // Invalid surrogate pair
                    return ERROR_NO_UNICODE_TRANSLATION;
                }
                // Get our surrogate value:
                cTest = GetSurrogate(cTest, cNext);

                // Should be in unicode range
                _ASSERTE(cTest < NORM_FIRST_UNKNOWN_CHARACTER);  // Redundent, we know all of them right now

                // Look up surrogate info
                bPage = PageLookup(cTest);


                goto TestPageInfo;
            }
            case NORM_PAGE_CHANGED:
                // If it changes its not normalized
                return ERROR_FALSE;

            case NORM_PAGE_UNASSIGNED:
                // Are unassigned allowed?
                if (!this->bAllowUnassignedCodePoints)
                    return ERROR_NO_UNICODE_TRANSLATION;

                // They're allowed, fall through to normal
            case NORM_PAGE_NORMAL:
                cLastTest = cTest;
                bLastClass = 0;
                bLastFlags = 0;
                pszLast = pszTestString;
                continue;

            case NORM_PAGE_INVALID:
                // Invalid character
                return ERROR_NO_UNICODE_TRANSLATION;

            default:
                // If we get here we should just be a class
                _ASSERTE(bPage < NORM_PAGE_STARTFLAGS);
        }

        // If it wasn't a QuickSkip we'll have to try slower version, still want to skip if we can.
        // Note that bInfo will be a different format than up above.
        BYTE bInfo;
        if ((bInfo = TableLookup(cTest, bPage)) == 0)
        {
            cLastTest = cTest;
            pszLast = pszTestString;
            bLastClass = 0;
            bLastFlags = 0;
            continue;
        }

        // Get its interesting flags
        // Now we have all of this character's interesting info, so we want to remember
        // this in cLastTest, etc. so that we don't have to look it up again.  Since this
        // Isn't a boring start character, its likely the next one isn't either.
        // We remember it at CONTINUE:  so you'll see goto CONTINUE:s after this.
        BYTE bClass = bInfo & NORM_CLASS_MASK;
        BYTE bFlags = bInfo & NORM_FLAGS_MASK;

        if ( bClass == 0 || bClass == 63 )
        {
            // No real class, so its a special case character
            switch (bInfo)
            {
                case NORM_CHAR_START_MAY_COMBINE_NON_START:
                case NORM_CHAR_START_MAY_COMBINE_START:
                    // Start characters can continue unless they combine with previous start char
                    // Remember our info in last tested char.
                    goto CONTINUE_START;

                case NORM_CHAR_START_MAY_COMBINE_PREVIOUS:
                    // Could combine with a previous start character
                    this->GetLastChar(pszTestString, pszLast, cLastTest, bLastClass, bLastFlags);
                    // GetLastChar updated pszLast, but it shouldn't have backed up before the char before our string
                    _ASSERTE(pszLast >= pszStringIn - 1);

                    // Now our lastclass & lastflags are valid, so we should have something.
                    if ((bLastClass | bLastFlags) != NORM_CHAR_START_MAY_COMBINE_START)
                    {
                        // Can't combine anyway, we can continue.
                        goto CONTINUE_START;
                    }

                    // Start characters May Combine, do they?
                    if (CanCombinableCharactersCombine(cLastTest, cTest))
                    {
                        // Can Combine, strings change
                        return ERROR_FALSE;
                    }

                    // Well, it doesn't do anything special, but it is a start character
                    goto CONTINUE_START;

                case NORM_CHAR_DIFFERENT:
                    // This character changes to a different character(s), not a normalized string
                    return ERROR_FALSE;

                case NORM_CHAR_ILLEGAL:
                    // for IDNA char 0 is legal if its the last character.
                    if (cTest == 0 && cchMaxIn <= 1)
                    {
                        // Last character is okay if its 0
                        // Since we know its the last character, just return ERROR_SUCCESS
                        return ERROR_SUCCESS;
                    }

                    // illegal character
                    return ERROR_NO_UNICODE_TRANSLATION;


                case NORM_CHAR_UNASSIGNED:
                    // unassigned code point, are we allowing them?
                    if (!this->bAllowUnassignedCodePoints)
                        return ERROR_NO_UNICODE_TRANSLATION;
                    // This'll be a start char
                    cLastTest = cTest;
                    bLastClass = 0;
                    bLastFlags = 0;
                    pszLast = pszTestString;
                    continue;
            }

            // Done with special cases
            _ASSERTE(!"We shouldn't get here either");
        }

        // Character has a real class.  Start characters may continue, but
        // remember them in lasttest
        if (bFlags == NORM_FLAG_START_MAY_DECOMPOSE ||
            bFlags == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE)
        {
            // Flag that our last start char hasn't been decoded yet,
            // really only an issue if we're recombinable, but this is easier than
            // another if statement
            cLastStartBase = 0;

            // Continue if start, we'll catch them next time.
            goto CONTINUE_START;
        }

        //
        // Our only choices left are:
        //
        //      NORM_FLAG_NON_START
        //      NORM_FLAG_NON_START_COMBINES_WITH_PREVIOUS
        //
        // This starts processing of non-start characters, which may
        // combine with previous start characters
        //
        _ASSERTE( bFlags == NORM_FLAG_NON_START ||
                  bFlags == NORM_FLAG_NON_START_COMBINES_WITH_PREVIOUS );

        // We need to know stuff about our last character, we can tell if it was some form
        // of start character or if it was also a non-start character.  If its a start character
        // we'll need to remember its info for our remaining non-start character tests.

        // See if we know about the previous character
        this->GetLastChar(pszTestString, pszLast, cLastTest, bLastClass, bLastFlags);
        // GetLastChar updated pszLast, but it shouldn't have backed up before the char before our string
        _ASSERTE(pszLast >= pszStringIn - 1);

        // Now we have all of our interesting information, so we can tell if our last
        // character was a start character.  If it is we'll want to set the start info.
        if ( bLastClass == 0 || bLastClass == 63 )
        {
            // These are class 0 or 63 flags and all have start info to remember.
            // NORM_CHAR_START
            // NORM_CHAR_START_MAY_COMBINE_NON_START
            // NORM_CHAR_START_MAY_COMBINE_START
            // NORM_CHAR_START_MAY_COMBINE_PREVIOUS
            // NORM_CHAR_ILLEGAL
            // NORM_CHAR_DIFFERENT
            cLastStart = cLastTest;
            bLastStartClass = bLastClass;
            bLastStartFlags = bLastFlags;
            pszLastStart = pszLast;

            // If it is combinable we can check if it combines with us
            if ((bLastFlags == NORM_CHAR_START_MAY_COMBINE_NON_START ||
                 bLastFlags == NORM_CHAR_START_MAY_COMBINE_START ) &&
                 bFlags == NORM_FLAG_NON_START_COMBINES_WITH_PREVIOUS)
            {
                // See if we combine with it (its a boring class 0 start if its a start)
                if (CanCombinableCharactersCombine(cLastStart, cTest))
                {
                    // Not normalized, they can combine.  This character could
                    // ALSO be out of order, but in either case we aren't normalized.
                    return ERROR_FALSE;
                }
            }

            // Continue, remembering our last character information
            goto CONTINUE;
        }

        if (bLastFlags == NORM_FLAG_START_MAY_DECOMPOSE ||
            bLastFlags == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE)
        {
            // We don't know the start base (if its recombinable)
            cLastStartBase = 0;
            cLastStartBasePair = 0;

            // Could combine with it as it is
            if (bFlags == NORM_FLAG_NON_START_COMBINES_WITH_PREVIOUS)
            {
                // They may simply combine, which is easiest to check
                if (bLastFlags == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE)
                {
                    // May combine with previous char.  1st check easy combine
                    if (CanCombinableCharactersCombine(cLastTest, cTest))
                    {
                        // They combined the easy way
                        return ERROR_FALSE;
                    }
                }

                // Didn't combine easily, have to check base character, if our
                // characters class is lower (otherwise existing composition is
                // preferred)
                if (bLastClass > bClass)
                {
                    bool bMayCombineStartBase = true;

                    // Special case for "class 221" combinable characters.
                    // If we're class 220 and our start char is class 221,
                    // then our start char's classes are 0 216 230 and we
                    // might create a 0 216 220 instead
                    if (bLastClass == this->bClass221)
                    {
                        // We're really class 216
                        if (bClass >= this->bClass216)
                        {
                            // We're class 218 or 220, but despite the 221, our
                            // start class is actually class 216 and we can't combine
                            bMayCombineStartBase = false;

                            // If we'e a 220, then we may combine with 1st pair
                            if (bClass == this->bClass220)
                            {
                                // We may combine with the start first pair
                                if (CanCombineWithStartFirstPair(
                                    cLastStartBasePair, cLastStartBase, cLastTest, cTest))
                                {
                                    return ERROR_FALSE;
                                }
                            }

                        }
                    }
                    // Similarly if our start is "class 231" then it is really
                    // class 0 230 240 and it may combine to 0 230 230 instead
                    else if (bLastClass == this->bClass231)
                    {
                        // We're really class 230
                        if (bClass == this->bClass230)   // (can't be higher)
                        {
                            // We're class 230, we can't combine with the start base,
                            // but we might combine with the first pair.
                            bMayCombineStartBase = false;

                            // We may combine with the start first pair
                            if (CanCombineWithStartFirstPair(
                                cLastStartBasePair, cLastStartBase, cLastTest, cTest))
                            {
                                return ERROR_FALSE;
                            }
                        }
                    }

                    if (bMayCombineStartBase)
                    {
                        // Wasn't fancy, see if they combine
                        if (CanCombineWithStartBase(cLastStartBase, cLastTest, cTest))
                        {
                            // Oops, they combine, return false
                            return ERROR_FALSE;
                        }
                    }
                }
            }

            // We don't combine, but next character may.
            // previous char was some start or start-like character, remember its info.
            cLastStart = cLastTest;
            bLastStartClass = bLastClass;
            bLastStartFlags = bLastFlags;
            pszLastStart = pszLast;
        }
        else
        {
            //
            // Last character was non-start (as is this character)
            //
            //      NORM_FLAG_NON_START
            //      NORM_FLAG_NON_START_COMBINES_WITH_PREVIOUS
            //
            _ASSERTE(bLastFlags == NORM_FLAG_NON_START ||
                     bLastFlags == NORM_FLAG_NON_START_COMBINES_WITH_PREVIOUS );

            // This character should be in order
            if (bLastClass > bClass)
            {
                // Out of order.  This character could still combine with the
                // start character, but in either case its not normalized
                return ERROR_FALSE;
            }

            // If its combinable, then it may yet combine with the start character,
            // but only if we aren't blocked by a same class non-start character.
            if (bFlags == NORM_FLAG_NON_START_COMBINES_WITH_PREVIOUS &&
                bLastClass != bClass)
            {
                // If last start class was combinable we'll have to check that.
                // Note that (as always) may_combine_start also can compbine with non-start
                if ( ( ( bLastStartFlags | bLastStartClass) == NORM_CHAR_START_MAY_COMBINE_NON_START ) ||
                     ( ( bLastStartFlags | bLastStartClass) == NORM_CHAR_START_MAY_COMBINE_START ) ||
                     ( bLastStartFlags == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE &&
                       ( bLastStartClass == 0 || bLastStartClass == 63 )))
                {
                    // We're one of these types, so we might just combine
                    //      NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE
                    //      NORM_CHAR_START_MAY_COMBINE_NON_START
                    //      NORM_CHAR_START_MAY_COMBINE_START
                    if (CanCombinableCharactersCombine(cLastStart, cTest))
                    {
                        // Not normalized, they can combine.  This character could
                        // ALSO be out of order, but in either case we aren't normalized.
                        return ERROR_FALSE;
                    }
                }

                // If the start character is decomposable and if our class is lower,
                // then we may combine with it instead of its current composition
                if ((bClass < bLastStartClass) &&
                    (bLastStartFlags == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE ||
                     bLastStartFlags == NORM_FLAG_START_MAY_DECOMPOSE) &&
                     (bLastStartClass > 0 && bLastStartClass < 63))
                {
                    // Special cases may prevent us from combining
                    bool bMayCombineStartBase = true;

                    // Special case for "class 221" combinable characters.
                    // If we're class 220 and our start char is class 221,
                    // then our start char's classes are 0 216 230 and we
                    // might create a 0 216 220 instead
                    if (bLastStartClass == this->bClass221)
                    {
                        // We're really class 216
                        if (bClass >= this->bClass216)
                        {
                            // We're class 218 or 220, but despite the 221, our
                            // start class is actually class 216 and we can't combine
                            bMayCombineStartBase = false;

                            // If we'e a 220, then we may combine with 1st pair
                            if (bClass == this->bClass220)
                            {
                                // We may combine with the start first pair
                                if (CanCombineWithStartFirstPair(
                                    cLastStartBasePair, cLastStartBase, cLastStart, cTest))
                                {
                                    return ERROR_FALSE;
                                }
                            }

                        }
                    }
                    // Similarly if our start is "class 231" then it is really
                    // class 0 230 240 and it may combine to 0 230 230 instead
                    else if (bLastStartClass == this->bClass231)
                    {
                        // We're really class 230
                        if (bClass == this->bClass230)   // (can't be higher)
                        {
                            // We're class 230, we can't combine with the start base,
                            // but we might combine with the first pair.
                            bMayCombineStartBase = false;

                            // We may combine with the start first pair
                            if (CanCombineWithStartFirstPair(
                                cLastStartBasePair, cLastStartBase, cLastStart, cTest))
                            {
                                return ERROR_FALSE;
                            }
                        }
                    }

                    if (bMayCombineStartBase)
                    {
                        // Wasn't fancy, see if they combine
                        if (CanCombineWithStartBase(cLastStartBase, cLastStart, cTest))
                        {
                            // Oops, they combine, return false
                            return ERROR_FALSE;
                        }
                    }
                }
            }
        }


        // Remember our info.  We did breaks instead of continues up above so we could remember this stuff.
        goto CONTINUE;
CONTINUE_START:
        cLastStart = cTest;
        bLastStartClass = bClass;
        bLastStartFlags = bFlags;
        pszLastStart = pszTestString;

CONTINUE:
        // Remember our last character
        cLastTest = cTest;
        bLastClass = bClass;
        bLastFlags = bFlags;
        pszLast = pszTestString;
    }

    // Well, if we get here the string's normalized.  Yippee.
    return ERROR_SUCCESS;
} // IsNormalized (CNormalization)

// Note that pszLast should always point to last word of last character, even for surrogate pairs.
// GetLastChar takes our pointers and checks to see if we know of the last character already or if we
// have to look it up.  For surrogates we should already know that it was a start character.
inline lchar_t  CNormalization::GetLastChar(__in wchar_t* pszString, __out wchar_t* &pszLast, lchar_t &cLastTest,
                                            BYTE& bLastClass, BYTE& bLastFlags)
{
    // Last character must be prior to input string
    _ASSERTE(pszString > pszLast);

    // Easy do we know it already
    // IMPORTANT: pszLast could be just before input buffer, in which
    // case we need to make sure we're valid before reading that word!  Since we
    // set pszLast for character 0, this will do that for us.

    // Move to previous character and test if that's the last character
    pszString--;
    if (pszLast == pszString)
        return cLastTest;

    // If the previous character is a high surrogate then pszString pointed to a 
    // surrogate and it needs to back up one more char.
    if (*pszString >= 0xD800 && *pszString <= 0xDFFF)
    {
        // Its a surrogate.  It cannot have been a low surrogate
        _ASSERTE(*pszString < 0xDC00);

        // Now we should be pointing at low surrogate of previous character
        pszString--;
        if (pszLast == pszString)
            return cLastTest;    
    }

    // pszString is now pointing to last word of last character
    pszLast = pszString;
    cLastTest = *(pszLast);

    // If last char is a surrogate we need the whole thing
    if (cLastTest >= 0xD800 && cLastTest <= 0xDFFF)
    {
        // It should be a low surrogate
        _ASSERTE(cLastTest >= 0xDC00 && cLastTest <= 0xDFFF);

        // Get our surrogate from the previous character and this one
        cLastTest = GetSurrogate(*(pszLast - 1), cLastTest);
    }

    // Get our info
    BYTE bLastInfo = PageLookup(cLastTest);

    //      NORM_PAGE_INVALID (should've err'd by now)
    //      NORM_PAGE_CHANGED (should've been changed by now)
    //      NORM_PAGE_SPECIAL_SURROGATE (not possible here)
    //      NORM_PAGE_SURROGATE (not possible here)
    //      NORM_PAGE_HANGUL_JAMO           // Can't have this for page lookup
    //      NORM_PAGE_HANGUL_SYLLABLE       // Can't have this for page lookup
    _ASSERTE(bLastInfo < NORM_PAGE_STARTFLAGS);

    // See if it was boring and we can just set class and flags to 0
    if (bLastInfo == 0)
    {
        // Special page cases are all uninteresting for last characters
        //      0 is boring start char
        bLastClass = bLastFlags = 0;
    }
    else
    {
        // Need to look up info in table.
        // Only character we care about is NORM_CHAR_START_MAY_COMBINE_START
        bLastInfo = TableLookup(cLastTest, bLastInfo);
        bLastClass = bLastInfo & NORM_CLASS_MASK;
        bLastFlags = bLastInfo & NORM_FLAGS_MASK;
    }

    // Return our character
    return cLastTest;
} // GetLastChar

// Get the dirt on this character
void    CNormalization::GetCharacterInfo(lchar_t cTest, BYTE& bClass, BYTE& bFlags )
{
    // We only do this after combining characters, so it should be plane 0.
    _ASSERTE(cTest < 0x30000);

    // Look up our new character
    BYTE bInfo = PageLookup (cTest);

    // bInfo should be > 0 because we only call this after combining characters,
    // so this should have some interesting properties.
    _ASSERTE(bInfo != 0);

    // Including all the gory details
    bInfo = TableLookup(cTest, bInfo);
    bFlags = bInfo & NORM_FLAGS_MASK;
    bClass = bInfo & NORM_CLASS_MASK;
} // GetCharacterInfo

lchar_t CNormalization::CanCombineWithStartFirstPair(
    lchar_t &cStartBasePair, lchar_t &cStartBase, lchar_t cStart, lchar_t cTest)
{
    // See if we know our base pair
    if (cStartBasePair == 0)
    {
        // See if we know our base
        if (cStartBase == 0)
            cStartBase = GetFirstDecomposedCharPlane0(cStart);

        lchar_t cStartSecond = GetSecondDecomposedCharPlane0(cStart);
        cStartBasePair = CanCombinableCharactersCombine(cStartBase, cStartSecond);
        _ASSERTE(cStartBasePair != 0);
    }

    // See if it combines with the base pair
    return CanCombinableCharactersCombine(cStartBasePair, cTest);
} // CanCombineWithStartFirstPair

lchar_t CNormalization::CanCombineWithStartBase(lchar_t &cStartBase, lchar_t cStart, lchar_t cTest)
{
    // See if we know our base
    if (cStartBase == 0)
       cStartBase = GetFirstDecomposedCharPlane0(cStart);

    // See if it combines with the base
    return CanCombinableCharactersCombine(cStartBase, cTest);
} // CanCombineWithStartBase


// This method allows us to test if our combinable characters can combine.
// We assume that we know they are combinable first (actually that doesn't matter much)
// Combinable characters are all in plane 0.
lchar_t CNormalization::CanCombinableCharactersCombine(lchar_t cFirst, lchar_t cSecond)
{
    // Should be in plane 0
    _ASSERTE(cFirst <= 0xFFFF && cSecond <= 0xFFFF);

    // If we don't combine then no, they can't combine (forms D & KD)
    if (this->iCompositionHash == NULL) return 0;

    _ASSERTE(this->pCompositionIndex);
    _ASSERTE(this->pCompositionData);

    // Well, look it up
    unsigned short sHash = (unsigned short)((cFirst + cSecond * 12541) % this->iCompositionHash);

    // Where we should start looking
    unsigned short sIndex = this->pCompositionIndex[sHash];
    unsigned short sLast = this->pCompositionIndex[sHash+1];

    // Our indexes go up by 3 and we should only have a few of them.  Last should be after index,
    // but not "too" far, and the difference should be a multiple of 3.  Could be the same if
    // there aren't any items of this type.
    _ASSERTE(sLast >= sIndex && (sIndex + 90) > sLast && ((sIndex - sLast) % 3 == 0));

    for ( ; sIndex != sLast; sIndex += 3)
    {
        // See if the first & second chars match.
        if (( cFirst == this->pCompositionData[sIndex] ) &&
            ( cSecond == this->pCompositionData[sIndex + 1] ))
        {
            // We found it, return the 3rd char (the combined char)
            return this->pCompositionData[sIndex + 2];
        }
    }

    // Reached table end, no replacement found
    return 0;
} // CanCombinableCharactersCombine

// This is used internally for getting the base of a start character that
// may decompose and recompose.  We know already that this character may
// decompose, and the only characters of this type are on plane 0.
lchar_t CNormalization::GetFirstDecomposedCharPlane0(lchar_t cDecompose)
{
    _ASSERTE(this->pReplacementHashTable && this->pReplacementIndex && this->pReplacementData);

    // Get our hash value
    int             iHash = cDecompose % this->iReplacementHash;
    unsigned short  iIndex = this->pReplacementHashTable[iHash];

    // See if we need to look it up in index table
    if ((iIndex & 0xE000) == 0)
    {
        // Yup, didn't have a length, so its an index
        unsigned short cTestChar;

        // Indexes index pairs, so we only stored 1/2 their value
        iIndex <<= 1;

        // Find our entry in the hashed table
        for (;
             (cTestChar = this->pReplacementIndex[iIndex]) != 0 && cTestChar != cDecompose ;
             iIndex += 2)
        {
            // Actually we just did everything
        }

        _ASSERTE( cTestChar != 0 );

        // Get our replacement character
        iIndex = this->pReplacementIndex[iIndex + 1];
    }

    // Need to chop off count part (we don't care because we only want 1st character anyway)
    iIndex &= 0x1FFF;

    // Look us up in the ReplacementIndex table, all we have to do is grab the first character we find
    _ASSERTE(this->pReplacementData[iIndex] != 0);

    return this->pReplacementData[iIndex];
} // GetFirstDecomposedCharPlane0

// This is used internally for getting the 2bnd character of a start character that
// may decompose and recompose.  We know already that this character may
// decompose, and the only characters of this type are on plane 0.
lchar_t CNormalization::GetSecondDecomposedCharPlane0(lchar_t cDecompose)
{
    _ASSERTE(this->pReplacementHashTable && this->pReplacementIndex && this->pReplacementData);

    // Get our hash value
    int             iHash = cDecompose % this->iReplacementHash;
    unsigned short  iIndex = this->pReplacementHashTable[iHash];

    // See if we need to look it up in index table
    if ((iIndex & 0xE000) == 0)
    {
        // Yup, didn't have a length, so its an index
        unsigned short cTestChar;

        // Indexes index pairs, so we only stored 1/2 their value
        iIndex <<= 1;

        // Find our entry in the hashed table
        for (;
             (cTestChar = this->pReplacementIndex[iIndex]) != 0 && cTestChar != cDecompose ;
             iIndex += 2)
        {
            // Actually we just did everything
        }

        _ASSERTE( cTestChar != 0 );

        // Get our replacement character
        iIndex = this->pReplacementIndex[iIndex + 1];
    }

    // This replacement should be more than 1 character.  It has to have a 2nd part because it
    // can decompose and recompose.  (I.e: its not angstrom changing to a-ring)
    _ASSERTE((iIndex / 0x2000) > 1);

    // Need to chop off count part (we don't care because we only want 2nd character anyway)
    iIndex &= 0x1FFF;

    // We want second character
    iIndex++;

    // Look us up in the ReplacementIndex table, all we have to do is grab the first character we find
    _ASSERTE(this->pReplacementData[iIndex] != 0);

    return this->pReplacementData[iIndex];
} // GetSecondDecomposedCharPlane0

// This is used internally for getting the 3rd character of a start character that
// decomposes to 3 characters.  We know already that this character may
// decompose and its 3 characters, and the only characters of this type are on plane 0.
// We call this for figuring out combinations with the first pair of class
// 0 216 230 and class 0 230 240 characters.
lchar_t CNormalization::GetThirdAndLastDecomposedCharPlane0(lchar_t cDecompose)
{
    _ASSERTE(this->pReplacementHashTable && this->pReplacementIndex && this->pReplacementData);

    // Get our hash value
    int             iHash = cDecompose % this->iReplacementHash;
    unsigned short  iIndex = this->pReplacementHashTable[iHash];

    // See if we need to look it up in index table
    if ((iIndex & 0xE000) == 0)
    {
        // Yup, didn't have a length, so its an index
        unsigned short cTestChar;

        // Indexes index pairs, so we only stored 1/2 their value
        iIndex <<= 1;

        // Find our entry in the hashed table
        for (;
             (cTestChar = this->pReplacementIndex[iIndex]) != 0 && cTestChar != cDecompose ;
             iIndex += 2)
        {
            // Actually we just did everything
        }

        _ASSERTE( cTestChar != 0 );

        // Get our replacement character
        iIndex = this->pReplacementIndex[iIndex + 1];
    }

    // This replacement should be 3 characters, because its a 0 216 230 or 0 230 240
    _ASSERTE((iIndex / 0x2000) == 3);

    // Need to chop off count part (we don't care because we only want 3rd character anyway)
    iIndex &= 0x1FFF;

    // We want third character
    iIndex+=2;

    // Look us up in the ReplacementIndex table, all we have to do is grab the first character we find
    _ASSERTE(this->pReplacementData[iIndex] != 0);

    return this->pReplacementData[iIndex];
} // GetThirdAndLastDecomposedCharPlane0

// Gets the 2nd & 3rd decomposed characters (if present) we use this when decomposing base characters
// when trying to make ourselves behave.
void CNormalization::GetSecondAndThirdDecomposedCharPlane0(lchar_t cDecompose, lchar_t &cSecond, lchar_t &cThird)
{
    _ASSERTE(this->pReplacementHashTable && this->pReplacementIndex && this->pReplacementData);

    // First we didn't find anything
    cSecond = cThird = 0;

    // Get our hash value
    int             iHash = cDecompose % this->iReplacementHash;
    unsigned short  iIndex = this->pReplacementHashTable[iHash];

    // See if we need to look it up in index table
    if ((iIndex & 0xE000) == 0)
    {
        // Yup, didn't have a length, so its an index
        unsigned short cTestChar;

        // Indexes index pairs, so we only stored 1/2 their value
        iIndex <<= 1;

        // Find our entry in the hashed table
        for (;
             (cTestChar = this->pReplacementIndex[iIndex]) != 0 && cTestChar != cDecompose ;
             iIndex += 2)
        {
            // Actually we just did everything
        }

        _ASSERTE( cTestChar != 0 );

        // Get our replacement character
        iIndex = this->pReplacementIndex[iIndex + 1];
    }

    // This replacement should be <= 3 characters
    int iLength = iIndex / 0x2000;
    _ASSERTE(iLength <= 3);

    // If 1 return nothing (really shouldn't happen, we wouldn't be testing bases if there weren't other characters attached)
    if (iLength <= 1) return;

    // Need to chop off count part
    iIndex &= 0x1FFF;

    // Get the 2nd character
    iIndex++;
    _ASSERTE(this->pReplacementData[iIndex] != 0);
    cSecond = this->pReplacementData[iIndex];

    // If 2 return nothing for 3rd
    if (iLength == 2) return;

    // Get the 3rd character
    iIndex++;
    _ASSERTE(this->pReplacementData[iIndex] != 0);
    cThird = this->pReplacementData[iIndex];
} // GetSecondAndThirdDecomposedCharPlane0


// This method calls NormalizeCharacter to attach each character of the decomposition,
// so any wierd combinations or out of order stuff should be dealt with then.
// Note that the hash includes the character plane, but the tables themselves only
// include the last 16 bits.
DWORD CNormalization::AppendDecomposedChar(lchar_t cDecompose, CBuffer& cBuffer)
{
    // Just double check we have tables
    _ASSERTE(this->pReplacementHashTable && this->pReplacementIndex && this->pReplacementData);

    // Get our hash value
    int             iHash = cDecompose % this->iReplacementHash;
    unsigned short  iIndex = this->pReplacementHashTable[iHash];

    // See if we need to look it up in index table
    if ((iIndex & 0xE000) == 0)
    {
        // Yup, didn't have a length, so its an index
        unsigned short cTestChar;

        // Our table ignores high bits of non bmp chars, so we only use 0xFFFF here.
        unsigned short cDecomposeChar = (unsigned short)(cDecompose & 0xFFFF);

        // Indexes index pairs, so we only stored 1/2 their value
        iIndex <<= 1;

        // Find our entry in the hashed table
        for (;
             (cTestChar = this->pReplacementIndex[iIndex]) != 0 && cTestChar != cDecomposeChar ;
             iIndex += 2)
        {
            // Actually we just did everything
        }

        _ASSERTE( cTestChar != 0 );

        // Get our replacement character
        iIndex = this->pReplacementIndex[iIndex + 1];
    }

    // See what the count is.
    int iLength = iIndex / 0x2000;

    // If iLength < 7 then we can count, otherwise we look for \0
    // Note this limits us to a hard coded length of 100 characters in our replacements tring
    if (iLength == 7) iLength = 100;

    // Need to chop off count part so we can count it.
    iIndex &= 0x1FFF;
    for (; (iLength) > 0 && (this->pReplacementData[iIndex] != 0); iLength--, iIndex ++)
    {
        // Get our character
        lchar_t cChar = this->pReplacementData[iIndex];

        // If its a surrogate, then we need to compose it so we don't mess ourselves up
        if (cChar >= 0xD800 && cChar <= 0xDFFF)
        {
            // Should only have the first char now
            _ASSERTE(cChar <= 0xDBFF);

            iIndex++; iLength--;
            _ASSERTE(iLength > 0);
            _ASSERTE(this->pReplacementData[iIndex] >= 0xDC00 &&
                     this->pReplacementData[iIndex] <= 0xDFFF);

            cChar = GetSurrogate(cChar, this->pReplacementData[iIndex]);
        }

        // We can add our character now, use page info so that Hangul, etc funky stuff isn't reprocessed.
        BYTE bPage = this->PageLookup(cChar);
        DWORD error = this->NormalizeCharacter(cChar, bPage, cBuffer);
        if (error != ERROR_SUCCESS) return error;
    }

    // We made it!
    return ERROR_SUCCESS;
} // AppendDecomposedChar
