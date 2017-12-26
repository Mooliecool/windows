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

// Helpers.h
//
// WARNING: This .DLL is supposed to be the same in managed and native code,
// starting with the Longhorn & Whidbey releases.  Please make sure they stay in sync.
//
// This file contains nifty helper methods that can help us process unicode characters.
// Things like Hangul and surrogate character parsing.
//
// Many of them are implimented here so they can be inlined.
//

#ifndef HELPERS_H_
#define HELPERS_H_

//
// Surrogate Helper Stuff
//

// GetSurrogateHigh
//
// For an input 21 bit character, return the 16 bit high surrogate
// (first word of the surrogate pair)
//
// WARNING: We assume the input character is from
// 0x010000 - 0x10FFFF and don't do any bounds checking
inline wchar_t GetSurrogateHigh(lchar_t cChar) 
{
    _ASSERTE(0x10000 <= cChar && cChar <= 0x10FFFF);

    return (wchar_t)(((cChar - 0x10000) / 0x400) + 0xD800);
}

// GetSurrogateLow
//
// For an input 21 bit character, return the 16 bit low surrogate
// (second word of the surrogate pair)
//
// WARNING: We assume the input character is from
// 0x010000 - 0x10FFFF and don't do any bounds checking
inline wchar_t GetSurrogateLow(lchar_t cChar)
{
    _ASSERTE(0x10000 <= cChar && cChar <= 0x10FFFF);

    return (wchar_t)(((cChar - 0x10000) % 0x400) + 0xDC00);
}

// GetSurrogate
//
// For two 21 bit high and low surrogate characters, return the 21
// 21 bit high surrogate formed by those characters.
//
// WARNING: We assume the input characters are actually high & low
// surrogates from 0xD800-0xDBFF and 0xDC00-0xDFFF respectively.
inline lchar_t GetSurrogate(lchar_t cHigh, lchar_t cLow)
{
    _ASSERTE(0xD800 <= cHigh && cHigh <= 0xDBFF);
    _ASSERTE(0xDC00 <= cLow && cLow <= 0xDFFF);

    return 0x10000 + (cHigh - 0xD800) * 0x400 + (cLow - 0xDC00);
}

//
// Hangul Helper Stuff
//

// Hangul Constants
const int SBase = 0xAC00;           // Start of Combined Form
const int LBase = 0x1100;           // Start of Leading Consonants
const int VBase = 0x1161;           // Start of Vowels
const int TBase = 0x11A7;           // Start of Trailing Consonants
const int LCount = 19;              // Leading Consonants
const int VCount = 21;              // Vowels
const int TCount = 28;              // Trailing Consonants
const int NCount = VCount * TCount; // 588
const int SCount = LCount * NCount; // 11172

//
// Hangul Helpers
//

// IsHangulS
//
// For the input 21 bit character, return true if it is a
// Hangul S character (combined character)
inline bool IsHangulS(lchar_t cTest)
{
    return (cTest >= SBase && cTest < SBase + SCount);
}

// IsHangulL
//
// For the input 21 bit character, return true if its a 
// Hangul L (leading) consonant.
inline bool IsHangulL(lchar_t cTest)
{
    return (cTest >= LBase && cTest < LBase + LCount);
}

// IsHangulL
//
// For the input 21 bit character, return true if its a 
// Hangul V (vowel).
inline bool IsHangulV(lchar_t cTest)
{
    return (cTest >= VBase && cTest < VBase + VCount);
}

// IsHangulT
//
// For the input 21 bit character, return true if its a 
// Hangul T (trailing) consonant.
inline bool IsHangulT(lchar_t cTest)
{
    return (cTest >= TBase && cTest < TBase + TCount);
}

// IsHangulLV
//
// For the input 21 bit character, return true if its a 
// Hangul L (leading) + Hangul V (vowel) combined character.
inline bool IsHangulLV(lchar_t cTest)
{
    // LVs must be S, and SIndex % TCount must be 0
    return IsHangulS(cTest) &&
           ((cTest - SBase) % TCount) == 0;
}

// These presume we already know our chacter's a
// combined Hangul syllable.

// GetHangulL
// Get the L (Leading) consotant from 
// a combined LV or LVT character.
// We use this for decomposition and assume we already
// know that its a hangul S (combined) character
inline wchar_t GetHangulL(lchar_t cTest)
{
    _ASSERTE(IsHangulS(cTest) == true);
    // Get the L part of our character
    return (wchar_t)(((cTest - SBase) / NCount) + LBase);
}

// GetHangulV
// Get the V (vowel) from 
// a combined LV or LVT character.
// We use this for decomposition and assume we already
// know that its a hangul S (combined) character
inline wchar_t GetHangulV(lchar_t cTest)
{
    _ASSERTE(IsHangulS(cTest) == true);
    // Get the V part of our character
    return (wchar_t)((((cTest - SBase) % NCount) / TCount) + VBase);
}

// GetHangulT
// Get the T (trailing) consonant from 
// a combined LVT character.
//
// Returns 0 if it was an LV instead of an LVT character.
//
// We use this for decomposition and assume we already
// know that its a hangul S (combined) character
inline wchar_t GetHangulT(lchar_t cTest)
{
    _ASSERTE(IsHangulS(cTest) == true);
    // Get the T part of our character (or null if cTest is just LV)
    wchar_t T = (wchar_t)((cTest - SBase) % TCount);
    if (T == 0) return 0;

    return (T + TBase);
}

// CanDecomposeHangul
// Takes a 32 bit (21 bit) unicode character and returns true
// if its a decomposable hangul character.
inline bool CanDecomposeHangul(lchar_t cTest)
{
    return IsHangulS(cTest);
}

// CanComposeHangul
// Takes a 32 bit (21 bit) first character and 32 bit (21 bit) second character and
// returns true if they combine.
//
// So L + V returns true and so does LV + t.
//
// Note that if the 1st 2 characters combine, then 2 or 3 character sequences
// will be reduced even if the 3rd isn't Hangul or not a Hangul Trailing Consonant.
// So for IsNormalized() we just need to call the first version here.
inline bool CanComposeHangul(lchar_t cFirst, lchar_t cSecond)
{
    return ((IsHangulL(cFirst) && IsHangulV(cSecond)) ||
            (IsHangulLV(cFirst) && IsHangulT(cSecond)));
} // CanComposeHangul(wchar_t, wchar_t)

// ComposeHangulLV
//
// Given 2 input 32 bit (21 bit) characters, return the combined
// hangul LV character if the input characters are L + V.
// Otherwise return 0.
//
inline lchar_t ComposeHangulLV(lchar_t cL, lchar_t cV)
{
    int iL = cL - LBase;
    if (iL < 0 || iL >= LCount)
    {
        // Not an L, doesn't compose
        return 0;
    }

    int iV = cV - VBase;
    if ( iV < 0 || iV >= VCount)
    {
        // Not a V, doesn't compose
        return 0;
    }

    return SBase + (iL * VCount +iV) * TCount;
} // ComposeHangulLV

// ComposeHangulLVT
//
// Given 2 input 32 bit (21 bit) characters, return the combined
// hangul LVT character if the input characters are LV + T.
// Otherwise return 0.
//
// (So you have to call ComposeHangulLV above to get the LV
// character first)
//
inline lchar_t ComposeHangulLVT(lchar_t cLV, lchar_t cT)
{
    int iT = cT - TBase;
    if (iT < 0 || iT >= TCount || !IsHangulLV(cLV))
    {
        // Not an T, or not an LV, doesn't compose
        return 0;
    }

    return cLV + iT;
} // ComposeHangulLVT

#endif  // HELPERS_H_
