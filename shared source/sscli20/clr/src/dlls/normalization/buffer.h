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

// Buffer.cpp
//
// WARNING: This .DLL is supposed to be the same in managed and native code,
// starting with the Longhorn & Whidbey releases.  Please make sure they stay in sync.
//
// This file contains a helper class to help us read/output our
// input and output strings.
//
// The biggest reason to have a buffer class is to provide a simple
// way of including extra buffer space in case our output string
// is temporarily longer than the string the user called us with.
// But we got rid of that idea because we're guessing at string lengths anyway
//
// A case where that would happen would be if a caller ended with a 
// composed then non-spacing character where the non-spacing character
// was preferred to combine with the composed character, yet the composed character
// has extra diacritics that may recombine with the newly created character.
// In that case we overrun our buffer, but by the time we're finished normalization,
// we get back down to our legal buffer size.
//
// 16 Aug 2002     Shawn Steele    Initial Implementation

#ifndef BUFFER_H_
#define BUFFER_H_

#include "normalizationhelp.h"
#include "helpers.h"

class CBuffer
{
    // CBuffer is a friend of CNormalization

private:
    //
    // Buffer pointers & counters
    //
    wchar_t*    pszInputBuffer;                 // Beginning of input buffer
    wchar_t*    pszInputBufferEnd;              // End of input buffer.  (start + size)
    wchar_t*    pszNextIn;                      // Next input character
    wchar_t*    pszOutputBuffer;                // Beginning of output buffer
    wchar_t*    pszOutputBufferEnd;             // Location after end of our output buffer
    wchar_t*    pszNextOut;                     // location of next output character
                                                // This could be pszOutputBufferEnd or pszExtraBuffer-1
                                                // during the transition between legal and buffer space
    int         cchOutputBufferSize;            // Our total output buffer size
                                                // (pszOutputBufferEnd == pszOutputBuffer + cchOutputBufferSize)
    int         cchInputBufferSize;             // Total input buffer size

    //
    // Already Known information
    //
    // Last known character:
    // We set this information when we've looked up the class and flags of a previous character
    // (so we don't set this for quick skipped characters)
    lchar_t     cLastChar;                      // Our last known character (combined if surrogate)
    wchar_t*    pszLastChar;                    // Last word of last known character (low surrogate if surrogate pair)
                                                // So if pszLastChar == pszNextOut-1 then last character information is valid
    BYTE        bLastClass;                     // Class of last character
    BYTE        bLastFlags;                     // Flags for the last character

    // Last known start character
    // We set this information when the last known character is a start character.  We check this
    // the first time we find a non-spacing character after a start character
    wchar_t*    pszAfterLastStart;              // First word (high surrogate if pair) of character after last known start character
                                                // So if pszNextOut == pszAfterLastStart then last character was start character
    lchar_t     cLastStart;                     // Last known start character
    BYTE        bLastStartClass;                // Class of last known start character
    BYTE        bLastStartFlags;                // Flags for last known start character
    // Base character information for last known start character:
    // We clear these the first time we find a non-spacing character after a start character, and then
    // set them if needed (first time something could combine and need them.)  These are only used when
    // the last character can decompose and recombine as something else.
    lchar_t     cLastStartBase;                 // Base character of last known start character
    lchar_t     cLastStartBasePair;             // First 2 character combination of last known start character

    // Used when trying to figure out where to insert a non-spacing character
    // We'll want to insert the current non-spacing character right before this character.
    wchar_t*    pszFirstBigger;                 // First character with a class larger than our current character

    // The normalization that's using us, so we can call back for a couple of interesting cases.
    CNormalization* pNormalization;

public:
    // Constructor
    // Sets our I/O Buffers and sizes.
    inline CBuffer(__in_ecount(cchUseInputBufferSize) wchar_t *pszUseInputBuffer, int cchUseInputBufferSize,
                   __out_ecount_opt(cchUseOutputBufferSize) wchar_t *pszUseOutputBuffer, int cchUseOutputBufferSize,
                   CNormalization *pUseNormalization)
    {
        // Must have an output buffer, can't have negative length
        _ASSERTE(pszUseInputBuffer != NULL);
        _ASSERTE(cchUseInputBufferSize >= 0);
        _ASSERTE(pszUseOutputBuffer != NULL);
        _ASSERTE(cchUseOutputBufferSize >= 0);

        // Remember input buffer
        this->pszInputBuffer = pszUseInputBuffer;
        this->cchInputBufferSize = cchUseInputBufferSize;
        this->pszInputBufferEnd = this->pszInputBuffer + this->cchInputBufferSize;

        // Remember our buffer and size
        this->pszOutputBuffer = pszUseOutputBuffer;
        this->cchOutputBufferSize = cchUseOutputBufferSize;
        this->pszOutputBufferEnd = this->pszOutputBuffer + this->cchOutputBufferSize;

        // Check for overflow
        _ASSERTE(this->pszInputBufferEnd >= this->pszInputBuffer);
        _ASSERTE(this->pszOutputBufferEnd >= this->pszOutputBuffer);

        // So far we don't have anything stored in our output buffer
        this->pszNextOut = this->pszOutputBuffer;
        this->pszNextIn = this->pszInputBuffer;

        // Set up our start helper thingys
        // Pretend our last character and last start character
        // were just before the start of the string
        pszLastChar = this->pszNextOut - 1;
        cLastChar = 0;
        bLastClass = bLastFlags = 0;
        this->pszAfterLastStart = this->pszNextOut;
        cLastStart = 0;
        bLastStartClass = bLastStartFlags = 0;

        // The normalization form that we're attached to.
        pNormalization = pUseNormalization;
    };

    // GetNextInputChar
    //
    // Return the next character from our input string
    inline wchar_t GetNextInputChar()
    {
        // Get the current code point (not a whole character)
        _ASSERTE(this->pszNextIn >= this->pszInputBuffer && this->pszNextIn < this->pszInputBufferEnd);
        wchar_t cTemp = *(this->pszNextIn);

        // Advance input pointer to the next character
        // Note that this could now be pszOutputBufferEnd, which isn't legal, but
        // we'll figure that out in CheckBufferSpace and move it to pszExtraBuffer
        this->pszNextIn++;

        return cTemp;
    } // GetNextInputChar

    // RewindInputWord
    //
    // Back up one input word (as opposed to character because a word could be half of a surrogate pair).
    // We use this when we used GetNextInputChar to peek at the next input character and decided it wasn't
    // interesting.  For example, Hangul L + V + V, for the 2nd V we have to check if its a T because LVT
    // combines, yet if it isn't we'll want to rewind and process the 2nd V by itself.
    inline void RewindInputWord()
    {
        this->pszNextIn--;
        _ASSERTE(this->pszNextIn >= this->pszInputBuffer);
    } // RewindInputChar

    // RewindOutputCharacter
    //
    // Back up last output character.  We use this mostly when we realized that our 
    // current character combines with the previous one.  In that case we rewind here
    // and then append the new character.
    inline void RewindOutputCharacter()
    {
        // Back up our buffer
        // See if we slipped back into our input buffer
        pszNextOut--;

        // Surrogates need to rewind another word, be careful of the extra buffer boundary
        if (*(this->pszNextOut) > 0xDC00 && *(this->pszNextOut) < 0xDFFF)
        {
            // Back up our buffer
            pszNextOut--;
            _ASSERTE(*(this->pszNextOut) > 0xD800 && *(this->pszNextOut) < 0xDBFF);
        }
        _ASSERTE(this->pszNextOut >= this->pszOutputBuffer);
    }

    // EndOfInput
    //
    // Return true if we've reached the end of our input string.
    //
    inline bool EndOfInput()
    {
        return (this->pszNextIn == this->pszInputBufferEnd);
    }

    // ReadIn
    //
    // Returns the number of characters that have been read in.  This is used
    // to estimate how much larger of a buffer we need if we've run out of buffer
    // space.
    inline int ReadIn()
    {
        return (int)(this->pszNextIn - this->pszInputBuffer);
    }

    // LeftToRead
    //
    // Returns the number of characters we have left to read.  We use this
    // when guessing how much larger of a buffer we'll need if we've overrun
    // the output buffer.
    inline int LeftToRead()
    {      
        // Note that if this is after failure then it is likely we didn't
        // finish processing that char, so the caller should do a +1.
        return (int)(this->pszInputBufferEnd - this->pszNextIn);
    }

    // Length
    //
    // Return the total length of the string.  We'll need to count anything
    // used in the extra buffer.  (In case we're trying to guess how much
    // more space we'll need to tell them to allocate before trying again.)
    inline int Length()
    {
        // Include the output buffer in our count!
        int iLength;

        // Return how much of the buffer we used
        iLength = (int)(this->pszNextOut - this->pszOutputBuffer);

        // Return how much of the buffer we used
        _ASSERTE(iLength >= 0);
        return iLength;
    } // Length();

    // Append
    //
    // Append a 21 bit character to our output string.  Returns false if we
    // run out of buffer + extra buffer space.
    //
    // We have to be careful to check our output buffer space (CheckBufferSpace())
    // and we need to add 2 characters if this is a supplimentary character.
    inline bool Append(lchar_t cAppend)
    {
        // All characters should be in the Unicode range.
        _ASSERTE(cAppend <= 0x10FFFF);

        // Do surrogates, remembering they could straddle our buffer end.
        if (cAppend >= 0x10000)
        {
            // Make sure we have room and add the high surrogate
            if (!CheckBufferSpace()) return false;
            *(this->pszNextOut) = GetSurrogateHigh(cAppend);
            this->pszNextOut++;

            // Get the low character, we'll drop out to the normal path 
            // to add it.
            cAppend = GetSurrogateLow(cAppend);
        }

        // Check our space
        if (!CheckBufferSpace()) return false;

        // Add our character
        *(this->pszNextOut) = (wchar_t)cAppend;

        // Increment next out pointer.  This could now be at the end,
        // which is why CheckBufferSpace is important to move it to
        // the extra buffer if necessary.
        this->pszNextOut++;

        return true;
    } // Append

    // Append
    //
    // Append a character, but remember all of the extra info (Class & Flags)
    // as our last character.  Return false if we couldn't add it (no buffer space)
    //
    inline bool Append(lchar_t cAppend, BYTE bClass, BYTE bFlags)
    {
        // Append it
        if (!Append(cAppend)) return false;

        // Remember appended character is on end
        this->cLastChar = cAppend;
        this->bLastClass = bClass;
        this->bLastFlags = bFlags;
        this->pszLastChar = this->pszNextOut - 1;
        return true;
    }
   
    //
    //
    //
    inline bool Insert( lchar_t cInsert, __in wchar_t* pszInsertLocation )
    {
        // All characters should be in the Unicode range.
        _ASSERTE(cInsert <= 0x10FFFF);
        _ASSERTE(pszInsertLocation >= this->pszOutputBuffer &&
                 pszInsertLocation < this->pszOutputBufferEnd);
        _ASSERTE(this->pszAfterLastStart <= pszInsertLocation);

        // May get to remember our last character.  (It moves, but its still last character)
        bool bRemember = (this->pszLastChar == this->pszNextOut - 1);

        // Surrogates need inserted twice
        if (cInsert >= 0x10000)
        {
            // Insert them out of order because we're inserting them into the same place.
            return ( this->Insert(GetSurrogateLow(cInsert), pszInsertLocation) &&
                     this->Insert(GetSurrogateHigh(cInsert), pszInsertLocation) );
        }

        // Check our space (this puts pszNextOut at pszExtraBuffer if its past OutputBuffer)
        if (!CheckBufferSpace()) return false;
        
        // See if insert location is in main output string
        wchar_t cTemp = (wchar_t) cInsert;

        // Keep inserting until we get to pszNextOut
        // remember that we have to watch out for the extra buffer boundary
        _ASSERTE(pszInsertLocation <= this->pszNextOut);
        _ASSERTE(this->pszNextOut < this->pszOutputBufferEnd);
        while (pszInsertLocation != this->pszNextOut)
        {
            wchar_t cTemp2 = *(pszInsertLocation);
            *(pszInsertLocation) = cTemp;
            cTemp = cTemp2;

            pszInsertLocation++;
        }

        // Append our new last character
        *(this->pszNextOut) = cTemp;
        this->pszNextOut++;

        // If we knew our last character we still know it
        if (bRemember)
            this->pszLastChar = this->pszNextOut - 1;
        return true;
    } // Insert

    // RemoveCharacter
    //
    // Opposite of Insert.  We need to remove a character from pszRemoveLocation, so all following
    // characters will move 1 word to the left.
    //
    // We call this when removing non-spacing characters from a sequence because they do combine
    // with a start character.  If we get here its because we're recombining all sorts of wierd 
    // things with the start character.  D + dot above + dot below will eventually get here I think.
    //
    // Note that combining characters always are non-supplimentary, so the removed character
    // should never be a surrogate character.  (We'll assert to make sure)
    //
    // We assume that we're after the last start char.
    inline void RemoveCharacter( __in wchar_t* pszRemoveLocation )
    {
        // We should never do this to a surrogate, they don't combine, so we shouldn't get here
        // with them
        _ASSERTE( *pszRemoveLocation < 0xD800 || *pszRemoveLocation > 0xDFFF);
        _ASSERTE(pszRemoveLocation >= this->pszOutputBuffer &&
                 pszRemoveLocation < this->pszOutputBufferEnd);

        // We never remove the last start character or characters before it.
        _ASSERTE(this->pszAfterLastStart <= pszRemoveLocation);

        // Have to move the end down 1
        this->pszNextOut--;

        // If our this->cLastChar is here it'll need moved down 1 too
        if (this->pszLastChar == this->pszNextOut)
        {
            if (pszRemoveLocation < this->pszLastChar )
                this->pszLastChar--;
            else
            {
                // We removed the last character, so its invalid now.
                this->pszLastChar = this->pszOutputBuffer - 1;
                this->cLastChar = 0;
                this->bLastClass = this->bLastFlags = 0;
            }
        }

        // Ok, we need to move character down until we reach the end
        _ASSERTE(pszRemoveLocation <= this->pszNextOut);
        _ASSERTE(this->pszNextOut >= this->pszOutputBuffer);
        while (pszRemoveLocation != this->pszNextOut)
        {
            // Slide the character down 1
            *(pszRemoveLocation) = *(pszRemoveLocation + 1);
            pszRemoveLocation++;
        }
    } // RemoveCharacter

    // CheckBufferSpace
    //
    // Return true if we have enough room in our buffer for another character.
    // Make sure that pszNextOut is pointing to pszExtraBuffer instead of pszOutputBufferEnd
    inline bool CheckBufferSpace()
    {
        // Are we in the output buffer?
        _ASSERTE(this->pszNextOut >= this->pszOutputBuffer);

        // True if we're before the end
        return (this->pszNextOut < this->pszOutputBufferEnd);
    } // CheckBufferSpace

    // GetLastChar
    //
    // Return our last 21 bit character.  We'll see if we remember it first,
    // if not we relookup all of the information in the tables and combine
    // surrogate pairs.
    //
    // When the last character is returned we'll know that last class and last
    // info are also set appropriately.
    inline lchar_t GetLastChar()
    {
        // Do we know it already?
        if (this->pszLastChar == this->pszNextOut - 1)
        {
             return this->cLastChar;
        }

        // Have to figure out where our last character ended...
        this->pszLastChar = this->pszNextOut - 1;

        // Get our last char
        _ASSERTE(this->pszLastChar >= this->pszOutputBuffer);
        this->cLastChar = *(this->pszLastChar);

        // If its a surrogate we'll need more, assert we know where we are in surrogate pair.
        _ASSERTE(this->cLastChar < 0xD800 || this->cLastChar > 0xDBFF);
        if (this->cLastChar > 0xDC00 && this->cLastChar <= 0xDFFF)
        {
            // Get 1st part
            lchar_t cTemp;
            
            // Get Last Char
            _ASSERTE(this->pszLastChar > this->pszOutputBuffer);            
            cTemp = *(this->pszLastChar - 1);

            // Build our surrogate into a normal character
            this->cLastChar = GetSurrogate(cTemp, this->cLastChar);
        }

        // Need to get cTemp's details
        // Get our info
        BYTE bLastInfo = pNormalization->PageLookup(this->cLastChar);

        // See if it was boring and we can just set class and flags to 0
        if (bLastInfo == 0 || bLastInfo >= NORM_PAGE_STARTFLAGS)
        {
            // Special page cases are all uninteresting for last characters
            //      0 is boring start char
            //      NORM_PAGE_INVALID (should've err'd by now)
            //      NORM_PAGE_CHANGED (should've been changed by now)
            //      NORM_PAGE_SPECIAL_SURROGATE (not possible here)
            //      NORM_PAGE_SURROGATE (not possible here)
            //      NORM_PAGE_HANGUL_JAMO (normalized already, just a start)
            //      NORM_PAGE_HANGUL_SYLLABLE (normalized already, just a start)
            this->bLastClass = this->bLastFlags = 0;
        }
        else
        {
            // Need to look up info in table.
            // Only character we care about is NORM_CHAR_START_MAY_COMBINE_START
            bLastInfo = pNormalization->TableLookup(this->cLastChar, bLastInfo);
            this->bLastClass = bLastInfo & NORM_CLASS_MASK;
            this->bLastFlags = bLastInfo & NORM_FLAGS_MASK;
        }

        return this->cLastChar;
    } // GetLastChar

    // MarkStart
    //
    // Flag that our last character was a start character, update
    // all of the start information.
    inline void MarkStart(lchar_t cStart, BYTE bClass, BYTE bFlags)
    {
        // Remember some start info
        pszAfterLastStart = pszNextOut;
        cLastStart = cStart;
        bLastStartClass = bClass;
        bLastStartFlags = bFlags;
    }

    // LastChar
    //
    // Accessor for cLastChar.  If you don't know if it is really
    // the last character or not you should call GetLastChar instead.
    inline lchar_t LastChar()
    {
        return this->cLastChar;
    }

    // LastFlags
    //
    // Return flags for last character.  Call GetLastChar() first
    // to set the flags if you don't know if last flags are correct or not.
    inline BYTE LastFlags()
    {
        return this->bLastFlags;
    }

    // LastClass
    //
    // Return class of last character.  Call GetLastChar() first
    // to set the class if you don't know if last class is correct or not.
    inline BYTE LastClass()
    {
        return this->bLastClass;
    }

    // LastInfo
    //
    // Return info for last character.  Call GetLastChar() first
    // to set the info if you don't know if last info is correct or not.
    //
    // Info only makes sense for special class 63 and class 0 flags.
    inline BYTE LastInfo()
    {
        return (this->bLastClass | this->bLastFlags);
    }

    // LastStart
    //
    // Return the last start character.  We assume that its correct.
    inline lchar_t LastStart()
    {
        return this->cLastStart;
    }

    // LastStartFlags
    //
    // Return flags for the last start character.  We assume that its correct.
    inline BYTE LastStartFlags()
    {
        return this->bLastStartFlags;
    }

    // LastStartClass
    //
    // Return the flags for the last start character.  We assume that its correct.
    inline BYTE LastStartClass()
    {
        return this->bLastStartClass;
    }

    // LastInfo
    //
    // Return last start character info.  We assume that its correct.
    // Info only makes sense for class 0 and class 63 special cases.
    inline BYTE LastStartInfo()
    {
        return (this->bLastStartClass | this->bLastStartFlags);
    }

    // LastStartBase
    //
    // Return the base character for our last start character.
    // If it isn't known yet, figure it out before returning in.
    //
    // We clear base characters when we reach the first non-space character
    // after a start character, and this is the first place it gets set.
    inline lchar_t LastStartBase()
    {
        // See if we need to calculate it
        if (this->cLastStartBase == 0)
        {
            // Last base character is first character of the last start character's decomposition
            this->cLastStartBase = this->pNormalization->GetFirstDecomposedCharPlane0(this->cLastStart);
        }

        return this->cLastStartBase;
    }

    // LastStartBasePair
    //
    // Return the 2 base pair characters for our last start character.
    // If it isn't known yet, figure it out before returning in.
    //
    // We clear base characters when we reach the first non-space character
    // after a start character, and this is the first place it gets set.
    inline lchar_t LastStartBasePair()
    {
        // See if we know it yet
        if (this->cLastStartBasePair == 0)
        {
            // Get the second character of our last decomposition
            lchar_t cStartSecond = this->pNormalization->GetSecondDecomposedCharPlane0(this->cLastStart);
            // Try to recompose the first and second characters.  Use LastStartBase() so 
            // that the last start base can be found if it isn't already.
            this->cLastStartBasePair = this->pNormalization->CanCombinableCharactersCombine(
                this->LastStartBase(), cStartSecond);
            // This should only be called on special characters where we're pretty sure it has
            // a base pair that's interesting to us.
            _ASSERTE(this->cLastStartBasePair != 0);
        }

        return this->cLastStartBasePair;
    }

    // SetLastStartChar
    // 
    // The last start character is changing to this new character.  The location of the character
    // stays the same.  What this means is that we added a non-spacing character to it.
    //
    // We can only compose these more for plane 0 characters, so we don't have to worry
    // about surrogate pairs tripping us up.
    inline void SetLastStart(lchar_t cNewStart, BYTE bNewClass, BYTE bNewFlags, bool bBaseChanged = true)
    {
        // Change the last start character to the new combined character.
        // Note: This only happens when combining start characters, which only happens in plane 0
        _ASSERTE( cNewStart <= 0xFFFF && cLastStart <= 0xFFFF);

        // Set the character
        // It should be after start of output buffer (because prev char was the one we're combining with)
        // and it should be before the end of the output buffer 
        _ASSERTE(this->pszAfterLastStart > this->pszOutputBuffer &&
                 this->pszAfterLastStart <= this->pszOutputBufferEnd);
        *(this->pszAfterLastStart - 1) = (wchar_t) cNewStart;

        // Remember the info
        this->cLastStart = cNewStart;
        this->bLastStartClass = bNewClass;
        this->bLastStartFlags = bNewFlags;

        // Last start base info is now unknown to us
        if (bBaseChanged)
            this->cLastStartBase = this->cLastStartBasePair = 0;
    }

    // LastWasStart
    //
    // We know the last character was also a start character.  This is called the
    // first time we find a non-spacing character after a start character, so we
    // also want to clear our LastStartBase info so it can be computed if needed.
    inline void LastWasStart()
    {
        this->pszAfterLastStart = this->pszLastChar + 1;
        this->cLastStart = this->cLastChar;
        this->bLastStartClass = this->bLastClass;
        this->bLastStartFlags = this->bLastFlags;

        // Last start base info is unknown to us
        this->cLastStartBase = this->cLastStartBasePair = 0;
    }

    // GetCurrentOutputChar
    //
    // Get the 21 bit character at the specified location.  If its
    // a surrogate, get the rest of the pair and update our location
    // appropriately.
    //
    // If they find the 2nd surrogate, someone's iterating down/back,
    // so the location will have to be decremented.
    // 
    // If we find a 1st surrogate, they're iterating up, so the
    // location will have to be incremented.
    inline lchar_t GetCurrentOutputChar(__in wchar_t* &pszLocation)
    {
        _ASSERTE(pszLocation >= this->pszOutputBuffer &&
                 pszLocation < this->pszOutputBufferEnd);
        
        // Get our info
        lchar_t cTemp = *(pszLocation);

        // Was it a surrogate?
        if (cTemp >= 0xD800 && cTemp <= 0xDFFF)
        {
            // Are we first char going up or 2nd char going down?
            if (cTemp >= 0xDC00)
            {
                // Surrogate, update our pointer, 2nd char going down
                pszLocation--;

                // Go ahead and get rest of surrogate
                _ASSERTE(pszLocation >= this->pszOutputBuffer);
                cTemp = GetSurrogate(*pszLocation, cTemp);
            }
            else
            {
                // on first char going up
                pszLocation++;

                // Get the rest of the surrogate
                _ASSERTE(pszLocation < this->pszOutputBufferEnd);
                cTemp = GetSurrogate(cTemp, *pszLocation);
            }
        }

        return cTemp;
    } // GetCurrentOutputChar

    // IsBlocked
    //
    // See if we are "blocked" from the start character by another character.
    // We are blocked if there is another non-start character with our class
    // between the last location in the output string and the last start character.
    //
    // In addition set pszFirstBigger to the location of the earliest character
    // with a class bigger than ours so we know where to insert our character
    // later if needed.
    inline bool IsBlocked(BYTE bTestClass)
    {
        // Assert our assumptions
        // Note that GetCurrentOutputChar is the only place we dereference the pointer
        // and it has its own asserts.
        _ASSERTE(this->pszAfterLastStart != NULL);
        _ASSERTE(bTestClass != NULL);
  
        // Remember the biggest class one that we have to insert before.
        this->pszFirstBigger = this->pszNextOut;

        // If we started right after start char, then stop
        if (this->pszNextOut == this->pszAfterLastStart)
            return false;

        // Start with the character at the end
        wchar_t*    pszBeforeTemp = this->pszNextOut-1;

        // Loop 'til last start.  We set pszLastStart to last character of the start
        // character, which'll be the first char pszBeforeTemp encounters as it decrements
        wchar_t*    pszLastStart = this->pszAfterLastStart - 1;

        _ASSERTE(pszBeforeTemp >= pszLastStart);
        // If we're in extra buffer, need to check it first
        // Loop until the start char (don't process the start char)
        while (pszBeforeTemp != pszLastStart)
        {
            // Get our character info.  GetCurrentOutputChar combines surrogates for us if needed
            lchar_t cBefore = GetCurrentOutputChar(pszBeforeTemp);            
            BYTE bBeforeInfo = this->pNormalization->PageLookup(cBefore);

            _ASSERTE(bBeforeInfo != 0 && bBeforeInfo < NORM_PAGE_STARTFLAGS);

            // Lookup the gory details
            bBeforeInfo = this->pNormalization->TableLookup(cBefore, bBeforeInfo);
            BYTE bBeforeClass = bBeforeInfo & NORM_CLASS_MASK;            // Flags are irrelevent, class 0 or 63 are like starts

            // Shouldn't be class 0 or class 63, double check its not a start character
            _ASSERTE(bBeforeInfo != 0);
            _ASSERTE(bBeforeClass != 0);
            _ASSERTE(bBeforeClass != 63);

            // Is it bigger than our class?
            if (bBeforeClass > bTestClass)
            {
                this->pszFirstBigger = pszBeforeTemp;
            }
            // About time, are they the same?
            else if (bBeforeClass == bTestClass)
            {
                // Blocked
                return true;
            }
            // Or maybe we're in order.
            else // (bBeforeClass < bTestClass)
            {
                // They're in order, we can stop.
                return false;
            }

            // They're still out of order, keep looking...
            pszBeforeTemp--;
        }

        // Not blocked, just out of order
        return false;
    } // IsBlocked

    // SameBeforeSameClass
    //
    // Set pszFirstBigger to the appropriate insertion point if we
    // want to be inserted BEFORE other characters of the same class.
    //
    // This is needed when we are a character that decomposes from
    // a composed character and needs to be inserted before all others
    // of the same class.
    inline void SortBeforeSameClass(BYTE bTestClass)
    {
        // We have to have had one start character already because we're decomposing one!
        // GetCurrentOutputChar asserts our pointer dereferences
        _ASSERTE(this->pszAfterLastStart != NULL);
        _ASSERTE(bTestClass != NULL);

        // Remember the biggest class one that we have to insert before.
        this->pszFirstBigger = this->pszNextOut;

        // If we started right after start char, then stop
        if (this->pszNextOut == this->pszAfterLastStart)
            return;

        // Start with the character at the end
        wchar_t*    pszBeforeTemp = this->pszNextOut-1;
        lchar_t     cBefore = GetCurrentOutputChar(pszBeforeTemp);

        // Loop 'til last start
        wchar_t*    pszLastStart = this->pszAfterLastStart - 1;
        if (this->cLastStart > 0xFFFF)
            pszLastStart--;

        _ASSERTE(pszBeforeTemp >= pszLastStart);
        // If we're in extra buffer, need to check it first
        // Loop until the start char (don't process the start char)
        while (pszBeforeTemp != pszLastStart)
        {
            // Get our character info
            BYTE bBeforeInfo = this->pNormalization->PageLookup(cBefore);

            _ASSERTE(bBeforeInfo != 0 && bBeforeInfo < NORM_PAGE_STARTFLAGS);

            // Lookup the gory details
            bBeforeInfo = this->pNormalization->TableLookup(cBefore, bBeforeInfo);
            BYTE bBeforeClass = bBeforeInfo & NORM_CLASS_MASK;            // Flags are irrelevent, class 0 or 63 are like starts

            // Shouldn't be class 0 or class 63, double check its not a start character
            _ASSERTE(bBeforeInfo != 0);
            _ASSERTE(bBeforeClass != 0);
            _ASSERTE(bBeforeClass != 63);

            // Is it bigger or the same as our test class?
            if (bBeforeClass >= bTestClass)
            {
                this->pszFirstBigger = pszBeforeTemp;
            }
            // Or maybe we're in order.
            else
            {
                // They're in order, we can stop.
                return;
            }

            // They're still out of order, keep looking...

            // Next character
            pszBeforeTemp--;
            cBefore = GetCurrentOutputChar(pszBeforeTemp);
        }

        // Got to the start character
        return;
    } // SortBeforeSameClass

    // InsertAtBlockedLocation
    //
    // Inserts a character just before pszBigger that we found in IsBlocked, last character will remain the same
    // return false if there's no buffer space left.
    //
    // We take info because if blocked location happens to be at end, then we'll just append
    inline bool InsertAtBlockedLocation(lchar_t cInsert, BYTE bClass, BYTE bFlags)
    {
        // If it goes at the end just append it.
        if (this->pszFirstBigger == this->pszNextOut)
            return this->Append(cInsert, bClass, bFlags);

        // It doesn't go at end, so next character will remain the same
        bool bResult = this->Insert(cInsert, this->pszFirstBigger);

        // Remember that last char didn't change.  Redundant, Insert did this for us
//        this->pszLastChar = this->pszNextOut - 1;

        return bResult;
    }

    // AppendNonStart
    //
    // Figure out where a character goes and insert it, last character will remain the same
    inline bool AppendAndSortNonStart( lchar_t cAppend, BYTE bClass, BYTE bFlags)
    {
        // Call IsBlocked to figure out where we get appended
        this->IsBlocked(bClass);

        // Insert us at the blocked location
        return this->InsertAtBlockedLocation(cAppend, bClass, bFlags);
    }

    // AppendAndSortDecomposed
    //
    // This appends a character that we just decomposed, which is like inserting any
    // other non-start, except that this one should be BEFORE any other characters of the
    // same class, not after.  So we call SortBeforeClass not IsBlocked.
    inline bool AppendAndSortDecomposed( lchar_t cAppend )
    {
        // Get class info (in case we get appended to very end)
        BYTE bClass;
        BYTE bFlags;
        this->pNormalization->GetCharacterInfo(cAppend, bClass, bFlags);

        // Sort and insert it.  It goes BEFORE others of its same class
        this->SortBeforeSameClass( bClass );

        // Go ahead and insert us at our blocked location
        return this->InsertAtBlockedLocation(cAppend, bClass, bFlags);
    }

    // Make sure we checked to see if last character was a start character
    inline void VerifyLastStart()
    {
        if ( this->LastClass() == 0 || this->LastClass() == 63 ||
             this->LastFlags() == NORM_FLAG_START_MAY_DECOMPOSE ||
             this->LastFlags() == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE )
        {
            // Last character was a start character, so set the start info
            this->LastWasStart();
        }
    }

    // ReplaceLastStartBase
    //
    // Replace the last start base character with a new character.  We'll need
    // to reappend the extra non-spacing characters after we readd our new start character.
    // Return false if no buffer space available.
    //
    // Something changed the last start base with a new character.  Now we 
    // need to use that character as our start character and then readd the
    // extra 1 or 2 (or 0) diacritics that we need.
    inline bool ReplaceLastStartBase( lchar_t cNewStart, BYTE bClass, BYTE bFlags )
    {
        // After combining we may have 1 or 2 extra diacritics to check.
        // The only case with 4 classes is 0 230 230 240, so something < 230 would have
        // to combine with the base character, which is literally all greek so nothing
        // else combines with it, its only combinable classes are 230 or 240.

        // Remember the current start char so we can find the later pieces later
        lchar_t cPreviousStart = this->LastStart();

        // Replace our start base character, base changed
        SetLastStart(cNewStart, bClass, bFlags, true);

        // Need to get our replacement characters
        lchar_t cSecond;
        lchar_t cThird;
        this->pNormalization->GetSecondAndThirdDecomposedCharPlane0(cPreviousStart, cSecond, cThird);

        // Since we insert them BEFORE characters of the same class, we need
        // to insert the third character first, then the 2nd.  Then if they
        // happen to be the same class, they'll be reordered correctly by
        // the insertion

        // Do third character first
        if (cThird != 0)
        {
            if (!AppendAndSortDecomposed(cThird))
                return false;
        }

        // Then do 2nd character
        if (cSecond != 0)
        {
            return AppendAndSortDecomposed(cSecond);
        }

        // It worked
        return true;
    } // ReplaceLastStartBase

    // ReplaceLastStartBasePair
    //
    // Something changed the last start base pair with a new character.  Now we 
    // need to use that character as our start character and then readd the
    // extra diacritics that we need.  (This used to be a 0 216 230 or 0 230 230 character,
    // So only the third character needs replaced.)
    //
    // return false if no buffer space available.
    inline bool ReplaceLastStartBasePair( lchar_t cNewStart, BYTE bClass, BYTE bFlags )
    {
        // After combining have 1 extra diacritics to check.
        // The only cases with base pairs are characters of class 0 216 230 or 0 230 240,
        // so we'll need to readd the last part after replacing the character

        // Remember the current start char third part so we can find the later pieces later
        lchar_t cThird = this->pNormalization->GetThirdAndLastDecomposedCharPlane0(this->LastStart());

        // Replace our start base character, base pair changed
        SetLastStart(cNewStart, bClass, bFlags, true);

        // If no cThird just return
        if (cThird == 0) return true;

        // Add the third character
        return AppendAndSortDecomposed(cThird);
    }

    // RecheckStartCombinations
    //
    // Double check our start combinations after changing the base character in case
    // some of the further dislocated non-spacing characters combine with the start character
    //
    // Buffer space won't overflow, but it could get smaller if these recombine.
    inline void RecheckStartCombinations()
    {
        _ASSERTE(this->pszAfterLastStart >= this->pszOutputBuffer &&
                 this->pszAfterLastStart < this->pszOutputBufferEnd);
        
        // How annoying.  For each character after our start character we need to check if it
        // could combine with that start character.  The only good thing about this is that
        // we know any left here are already in order.
        wchar_t*    pszTemp = this->pszAfterLastStart;
        
        // This only happens if something combined with our start character (so we know they may decompose)
        _ASSERTE(this->bLastStartFlags == NORM_FLAG_START_MAY_DECOMPOSE ||
                 this->bLastStartFlags == NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE);

        // Can't combine them if the start character can't combine with them.
        if (this->bLastStartFlags != NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE)
        {
            return;
        }

        // Iterate through all characters from pszTemp to pszNextOut
        BYTE bLastClass = 0;
        _ASSERTE(pszTemp <= this->pszNextOut);
        while (pszTemp != this->pszNextOut)
        {
            // Get the character to test.
            lchar_t cTest = this->GetCurrentOutputChar(pszTemp);

            // Need its class
            BYTE bClass;
            BYTE bFlags;
            this->pNormalization->GetCharacterInfo(cTest, bClass, bFlags);

            // We have to be a non-start.
            _ASSERTE(bFlags == NORM_FLAG_NON_START ||
                     bFlags == NORM_FLAG_NON_START_COMBINES_WITH_PREVIOUS);
            _ASSERTE(bClass > 0 && bClass < 63);

            // If its class is different last class we're not blocked
            if (bClass != bLastClass)
            {
                // See if we combine, we have to be a normal
                if (bFlags == NORM_FLAG_NON_START_COMBINES_WITH_PREVIOUS)
                {
                    lchar_t cCombined = this->pNormalization->CanCombinableCharactersCombine(this->LastStart(), cTest);

                    if (cCombined)
                    {
                        // They combined.  Ick.
                        // If we were a surrogate we'll have to -- our pointer. (surrogates don't combine)
                        _ASSERTE(cTest <= 0xFFFF);

                        // Update the last start character, base didn't change
                        this->pNormalization->GetCharacterInfo(cCombined, bClass, bFlags);
                        this->SetLastStart(cCombined, bClass, bFlags, false);

                        // Get rid of our current character (it combined);
                        this->RemoveCharacter(pszTemp);

                        // Can't combine them if the start character can't combine with anything else.
                        if (this->bLastStartFlags != NORM_FLAG_START_MAY_DECOMPOSE_AND_COMBINE)
                        {
                            return;
                        }
  
                        // Don't need to increment us because we are where we want to be already
                        continue;
                    }
                }

                // Didn't combine.  Update our last class
                bLastClass = bClass;
            }

            // Need to carefully move to next character.
            pszTemp++;
        }
    }
};

#endif  // BUFFER_H_
