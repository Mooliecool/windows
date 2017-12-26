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

// NormalizationPrivate.h
//
//

#ifndef NORMALIZATIONPRIVATE_H_
#define NORMALIZATIONPRIVATE_H_

#include <crtdbg.h>
#include <windows.h>
#include "normalization.h"

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))
#endif

typedef long lchar_t;
class CBuffer;

// Like our enumeration stuff
const int NORMALIZATION_K   =   0x04;
const DWORD ERROR_FALSE = 0xffff;

typedef BYTE TABLESEGMENT[128];

enum INTERNAL_NORM_FORM
{
    NormOther  = 0,
    NormC      = 0x1,
    NormD      = 0x2,
    NormKC     = 0x5,
    NormKD     = 0x6,
    NormIdna   = 0xd,
    DisallowUnassigned         = 0x100,
    NormCDisallowUnassigned    = 0x101,
    NormDDisallowUnassigned    = 0x102,
    NormKCDisallowUnassigned   = 0x105,
    NormKDDisallowUnassigned   = 0x106,
    NormIdnaDisallowUnassigned = 0x10d,
};

struct Tables
{
    // Header
    WORD    signature[16];                  // file signature
    WORD    version[4];                     // version.  (unicode major, minor, minor revision, minor revision)
    // General Data
    WORD    normalizationForm;              // Could be byte right now
    WORD    maxReplacementSize;             // Could be byte
    WORD    quickSkip;                      // Has to be word
    WORD    replacementHash;                // Has to be word
    WORD    compositionHash;                // Has to be word
    WORD    flags;                          // Only 1 bit right now
    // Table offsets
    WORD    offsetClassMap;
    WORD    offsetMainIndex;
    WORD    offsetCharInfo;
    WORD    offsetReplacementHashTable;      // Maps to index table (or directly to data table)
    WORD    offsetReplacementIndex;
    WORD    offsetReplacementData;           // Replacement Data
    WORD    offsetCompositionIndex;
    WORD    offsetCompositionData;
    BYTE    firstTableByte;                 // Beginning of the first table...
};

class CNormalization
{
    friend class CBuffer;

       // Global normalization classes
    private:
        static CNormalization  *pFormC;
        static CNormalization  *pFormD;
        static CNormalization  *pFormKC;
        static CNormalization  *pFormKD;
        static CNormalization  *pFormIdna;
        static CNormalization  *pFormCNoUnassigned;
        static CNormalization  *pFormDNoUnassigned;
        static CNormalization  *pFormKCNoUnassigned;
        static CNormalization  *pFormKDNoUnassigned;
        static CNormalization  *pFormIdnaNoUnassigned;

        // Private for our normalization info
    protected:
        NORM_FORM       eNormalizationForm;

        // Tables Pointer
        Tables          *pTablesPointer;

        // Character Data lookup info
        int             iMaxReplacementSize;
        int             iQuickLower;
        BYTE            *pMainIndex;
        TABLESEGMENT    *pCharInfo;

        // Character Class Mapping Table
        BYTE            *pClassMap;

        // Decomposition info
        int             iReplacementHash;
        WORD            *pReplacementHashTable;
        WORD            *pReplacementIndex;
        WORD            *pReplacementData;

        // Composition info
        int             iCompositionHash;
        WORD            *pCompositionIndex;
        wchar_t         *pCompositionData;

        // Hangul decomposition?
        bool            bDecomposeHangul;

        // Does this form allow unassigned code points?
        bool            bAllowUnassignedCodePoints;

        // Class 221 and class 231 exceptions
        BYTE            bClass216;
        BYTE            bClass220;
        BYTE            bClass221;
        BYTE            bClass230;
        BYTE            bClass231;

    // Construction/Destruction
    protected:
        CNormalization(Tables* pTables, NORM_FORM eNormalizationForm);
        ~CNormalization();

    protected:
        bool    LoadTables(Tables*    pTables);
        void    LoadClassMapExceptions();

    // Initialize our global stuff
    public:
        static void    CleanUp();

    // Static Methods
    public:
        static CNormalization*  GetNormalization(NORM_FORM eNormalizationForm);
        static CNormalization*  GetNormalization(NORM_FORM eNormalizationForm, Tables* pTables);
        static bool             IsValidForm(NORM_FORM eNormalizationForm);

    private:
        static CNormalization*  GetNormalization(NORM_FORM eNormalizationForm, Tables* pTables,
                                                    CNormalization** ppUseForm);

    // Public Methods
    public:
        bool    IsFormC()   { return eNormalizationForm & NormalizationC; };
        bool    IsFormD()   { return (eNormalizationForm & NormalizationD) == NormalizationD; };
        bool    IsFormK()   { return (eNormalizationForm & NORMALIZATION_K) == NORMALIZATION_K; };
        bool    IsFormKC()  { return (eNormalizationForm & NormalizationKC) == NormalizationKC; };
        bool    IsFormKD()  { return (eNormalizationForm & NormalizationKD) == NormalizationKD; };

    private:
        bool    IsFormIDNA(){ return (eNormalizationForm & NormIdna) == NormIdna; };

    public:
        DWORD Normalize( __in_ecount(cchIn) const wchar_t* pszStringIn, const unsigned int cchIn,
                                   __out_ecount_opt(cchOut) wchar_t* pszStringOut, const unsigned int cchOut,
                                   int* pcchOut);
        DWORD IsNormalized( __in_ecount(cchMaxIn) const wchar_t* pszStringIn, int cchMaxIn );
        DWORD GuessCharCount(  __in_ecount(cchMaxIn) const wchar_t* pszStringIn, const int cchMaxIn, int *pcchOut);
        int GuessCharCount( const int iInputSize);

    // Helper Methods
    protected:
        int GuessBetterCharCount(int iCharCounted, int iCharLeft, int iCurrentCount);
        inline BYTE PageLookup(lchar_t cTest);
        inline BYTE TableLookup(lchar_t cTest, BYTE bPage);
        DWORD NormalizeCharacter(lchar_t cTest, BYTE bPage, CBuffer& cBuffer);

    private:
        lchar_t GetFirstDecomposedCharPlane0(lchar_t cDecompose);
        lchar_t GetSecondDecomposedCharPlane0(lchar_t cDecompose);
        lchar_t GetThirdAndLastDecomposedCharPlane0(lchar_t cDecompose);
        void    GetSecondAndThirdDecomposedCharPlane0(lchar_t cDecompose, lchar_t &cSecond, lchar_t &cThird);

        DWORD AppendDecomposedChar(lchar_t cDecompose, CBuffer &cBuffer);
        inline lchar_t  CanCombinableCharactersCombine(lchar_t cFirst, lchar_t cSecond);
        inline lchar_t  CanCombineWithStartBase(lchar_t &cStartBase, lchar_t cStart, lchar_t cTest);
        inline lchar_t  CanCombineWithStartFirstPair(lchar_t &cStartBasePair, lchar_t &cStartBase, lchar_t Start, lchar_t cTest);
        inline lchar_t  GetLastChar(__in wchar_t* pszString, __in wchar_t* &pszLast, lchar_t &cLastTest,
                                    BYTE& bLastClass, BYTE& bLastFlags);
        inline void     GetCharacterInfo(lchar_t cTest, BYTE& bClass, BYTE& bFlags );
};

#endif  // NORMALIZATIONPRIVATE_H_
