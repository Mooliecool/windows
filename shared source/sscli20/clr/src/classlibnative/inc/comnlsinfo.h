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
////////////////////////////////////////////////////////////////////////////
//
//  Class:    COMNlsInfo
//  Purpose:  This module defines the methods of the COMNlsInfo
//            class.  These methods are the helper functions for the
//            managed NLS+ classes.
//
//  Date:     August 12, 1998
//
////////////////////////////////////////////////////////////////////////////

#ifndef _COMNLSINFO_H
#define _COMNLSINFO_H

#include "comnumber.h"

//
//This structure must map 1-for-1 with the InternalDataItem structure in
//System.Globalization.EncodingTable.
//
struct EncodingDataItem {
    WCHAR* webName;
    int    codePage;
};

//
//This structure must map 1-for-1 with the InternalCodePageDataItem structure in
//System.Globalization.EncodingTable.
//
struct CodePageDataItem {
    int    codePage;
    int    uiFamilyCodePage;
    WCHAR* webName;
    WCHAR* headerName;
    WCHAR* bodyName;
    DWORD dwFlags;
};

// Normalization
typedef BYTE (*PFN_NORMALIZATION_IS_NORMALIZED_STRING)
    ( int NormForm, LPWSTR lpInString, int cchInString);

typedef int (*PFN_NORMALIZATION_NORMALIZE_STRING)
    ( int NormForm, LPWSTR lpInString, int cchInString, LPWSTR lpOutString, int cchOutString);

typedef BYTE* (*PFN_NORMALIZATION_INIT_NORMALIZATION)
    ( int NormForm, BYTE* pTableData);

//
// CultureData has a cloned strucure in the managed side. we receive this struct from the managed side to be filled 
// by the native APIs (mostly GetLocaleInfo) to load the synthetic cultures data.
//
// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
// any change in this struct require a change in the cloned one in the managed side.
//
// <SyntheticSupport/>

#define LOCALE_GEOID                 0x0000005B   // geographical location id

struct CultureData
{
    STRINGREF   sIso639Language;               // LOCALE_SISO639LANGNAME       (TwoLetterISOLanguageName)
    STRINGREF   sIso3166CountryName;           // LOCALE_SISO3166CTRYNAME      (TwoLetterISORegionName)
    STRINGREF   sListSeperator;                // LOCALE_SLIST                 (ListSeparator)
    STRINGREF   sDecimalSeperator;             // LOCALE_SDECIMAL              (NumberDecimalSeparator)
    STRINGREF   sThousandSeperator;            // LOCALE_STHOUSAND             (NumberGroupSeparator)
    STRINGREF   sCurrency;                     // LOCALE_SCURRENCY             (CurrencySymbol)
    STRINGREF   sMonetaryDecimal;              // LOCALE_SMONDECIMALSEP        (CurrencyDecimalSeparator)
    STRINGREF   sMonetaryThousand;             // LOCALE_SMONTHOUSANDSEP       (CurrencyGroupSeparator)
    STRINGREF   sNegativeSign;                 // LOCALE_SNEGATIVESIGN         (NegativeSign)
    STRINGREF   sAM1159;                       // LOCALE_S1159                 (AMDesignator)
    STRINGREF   sPM2359;                       // LOCALE_S2359                 (PMDesignator)
    STRINGREF   sAbbrevLang;                   // LOCALE_SABBREVLANGNAME       (ThreeLetterWindowsLanguageName)
    STRINGREF   sEnglishLanguage;              // LOCALE_SENGLANGUAGE          (Part of EnglishName)
    STRINGREF   sEnglishCountry;               // LOCALE_SENGCOUNTRY           (Part of EnglishName)
    STRINGREF   sNativeLanguage;               // LOCALE_SNATIVELANGNAME       (Part of NativeName)
    STRINGREF   sNativeCountry;                // LOCALE_SNATIVECTRYNAME       (Part of NativeName)
    STRINGREF   sAbbrevCountry;                // LOCALE_SABBREVCTRYNAME       (ThreeLetterWindowsRegionName)
    STRINGREF   sIntlMonetarySymbol;           // LOCALE_SINTLSYMBOL           (ISOCurrencySymbol)
    STRINGREF   sEnglishCurrency;              // LOCALE_SENGCURRNAME          (CurrencyEnglishName)
    STRINGREF   sNativeCurrency;               // LOCALE_SNATIVECURRNAME       (CurrencyNativeName)
    STRINGREF   saAltSortID;                   // LOCALE_SSORTNAME             (SortName)
    
    // _sPositiveSign in NLS always return empty string
    STRINGREF   sPositiveSign;                 // LOCALE_SPOSITIVESIGN         (PositiveSign)

    // saNativeDigits should be converted to array of string instead of array of characters later. 
    STRINGREF   saNativeDigits;                // LOCALE_SNATIVEDIGITS         (NativeDigits)

    STRINGREF   waGrouping;                    // LOCALE_SGROUPING             (NumberGroupSizes)
    STRINGREF   waMonetaryGrouping;            // LOCALE_SMONGROUPING          (CurrencyGroupSizes)
    STRINGREF   waFontSignature;               // LOCALE_FONTSIGNATURE         (No API for it) 

    // Some fields defined only post XP
    STRINGREF   sNaN;                           // LOCALE_SNAN                  (NaNSymbol)
    STRINGREF   sPositiveInfinity;              // LOCALE_SPOSINFINITY          (PositiveInfinitySymbol)
    STRINGREF   sNegativeInfinity;              // LOCALE_SNEGINFINITY          (NegativeInfinitySymbol)
    STRINGREF   sISO3166CountryName2;           // LOCALE_SISO3166CTRYNAME2     (ThreeLetterISORegionName)
    STRINGREF   sISO639Language2;               // LOCALE_SISO639LANGNAME2      (ThreeLetterISOLanguageName)
    STRINGREF   sIetfLanguage;                  // LOCALE_SIETFLANGUAGE         (IetfLanguageTag)
    
    PTRARRAYREF         saSuperShortDayNames;   // LOCALE_SSHORTESTDAYNAME1..LOCALE_SSHORTESTDAYNAME7 (ShortestDayNames)
    // End of the fields defined only post XP

    PTRARRAYREF saTimeFormat;                  // EnumTimeFormats              (GetAllDateTimePatterns('T'))

    PTRARRAYREF saShortDate;                   // EnumDateFormatsEx            (GetAllDateTimePatterns('d'))
    PTRARRAYREF saLongDate;                    // EnumDateFormatsEx            (GetAllDateTimePatterns('D'))
    PTRARRAYREF saYearMonth;                   // EnumDateFormatsEx            (GetAllDateTimePatterns('Y'))
    
    PTRARRAYREF saMonthNames;                  // LOCALE_SMONTHNAME(1~13)      (MonthNames)

    // LOCALE_SDAYNAME1 means Monday in NLS (need conversion in NLS+
    PTRARRAYREF saDayNames;                    // LOCALE_SDAYNAME(1~7)         (GetDayOfWeekNames)
    // LOCALE_SABBREVDAYNAME means Monday in NLS (need conversion in NLS+
    PTRARRAYREF saAbbrevDayNames;              // LOCALE_SABBREVDAYNAME(1~7)   (GetAbbreviatedDayOfWeekNames/SuperShortDayNames)
    PTRARRAYREF saAbbrevMonthNames;            // LOCALE_SABBREVMONTHNAME(1~13)(AbbreviatedMonthNames)
    PTRARRAYREF saNativeCalendarNames;         // GetCalendarInfo/CAL_SCALNAME (NativeCalendarName)

    PTRARRAYREF saGenitiveMonthNames;          // GetDateFormat with "dd MMMM" (MonthGenitiveNames)
    PTRARRAYREF saAbbrevGenitiveMonthNames;    // GetDateFormat with "d MMM"   (AbbreviatedMonthGenitiveNames)

    // use also EnumCalendarInfo/CAL_ICALINTVALUE
    U2ARRAYREF  waCalendars;                   // LOCALE_IOPTIONALCALENDAR     (OptionalCalendars)

    // _iFirstDayOfWeek (0 is Monday for NLS and is Sunday in NLS+)
    INT32       iFirstDayOfWeek;               // LOCALE_IFIRSTDAYOFWEEK       (FirstDayOfWeek) 
    INT32       iDigits;                       // LOCALE_IDIGITS               (NumberDecimalDigits)
    INT32       iNegativeNumber;               // LOCALE_INEGNUMBER            (NumberNegativePattern)
    INT32       iCurrencyDigits;               // LOCALE_ICURRDIGITS           (CurrencyDecimalDigits)
    INT32       iCurrency;                     // LOCALE_ICURRENCY             (CurrencyPositivePattern)
    INT32       iNegativeCurrency;             // LOCALE_INEGCURR              (CurrencyNegativePattern)
    INT32       iFirstWeekOfYear;              // LOCALE_IFIRSTWEEKOFYEAR      (CalendarWeekRule)
    INT32       iMeasure;                      // LOCALE_IMEASURE              (IsMetric)
    INT32       iDigitSubstitution;            // LOCALE_IDIGITSUBSTITUTION    (DigitSubstitution)
    INT32       iDefaultAnsiCodePage;          // LOCALE_IDEFAULTANSICODEPAGE  (ANSICodePage)
    INT32       iDefaultOemCodePage;           // LOCALE_IDEFAULTCODEPAGE      (OEMCodePage)
    INT32       iDefaultMacCodePage;           // LOCALE_IDEFAULTMACCODEPAGE   (MacCodePage)
    INT32       iDefaultEbcdicCodePage;        // LOCALE_IDEFAULTEBCDICCODEPAGE(EBCDICCodePage)
    INT32       iCountry;                      // LOCALE_ICOUNTRY              (No API for this field)
    INT32       iPaperSize;                    // LOCALE_IPAPERSIZE            (No API for this field)
    INT32       iLeadingZeros;                 // LOCALE_IDAYLZERO             (No API for this field)
    INT32       iIntlCurrencyDigits;           // LOCALE_IINTLCURRDIGITS       (No API for this field)
    INT32       iGeoId;                        // EnumSystemGeoID/GetGeoInfo   (RegionInfo.GeoId)
    INT32       iDefaultCalender;              // LOCALE_ICALENDARTYPE         (No API for this field)
};

////////////////////////////////////////////////////////////////////////////
//
// Forward declarations
//
////////////////////////////////////////////////////////////////////////////

class CharTypeTable;
class CasingTable;
class SortingTable;
class NativeTextInfo;


class COMNlsInfo {
public:
    static BOOL InitializeNLS();


public:

    //
    //  Native helper functions for methods in CultureInfo.
    //
    static FCDECL2(FC_BOOL_RET, IsValidLCID, INT32 lcid, INT32 flag);
    static FCDECL2(FC_BOOL_RET, IsWin9xInstalledCulture, StringObject *cultureKey, INT32 lcid);
    

    static FCDECL2(Object*, nativeGetUserDefaultLCID, INT32* LCID, INT32 langType);
    static FCDECL1(Object*, nativeGetUserDefaultUILanguage, INT32* LCID);
    static FCDECL1(Object*, nativeGetSystemDefaultUILanguage, INT32* LCID);

    //
    // Native helper functions for methods in DateTimeFormatInfo
    //
    static FCDECL2(INT32, GetCaseInsHash, LPVOID strA, void *pNativeTextInfoPtr);


    static FCDECL2(Object*, nativeGetEraName, INT32 nValue1, INT32 nValue2);

    static FCDECL1(VOID,   nativeInitUnicodeCatTable, INT_PTR);
    static BYTE GetUnicodeCategory(WCHAR wch);
    static BOOL nativeIsWhiteSpace(WCHAR c);

    static FCDECL1(FC_BOOL_RET,  nativeSetThreadLocale, INT32 lcid);
    static FCDECL2(Object*, nativeGetLocaleInfo, INT32 lcid, INT32 lcType);
    static FCDECL2(FC_BOOL_RET, nativeGetDTFIUserValues, INT32 lcid, LPVOID value);
    static FCDECL2(FC_BOOL_RET, nativeGetNFIUserValues, INT32 lcid, NumberFormatInfo* nfi);

    static FCDECL2(FC_BOOL_RET, nativeGetCultureData, INT32 lcid, CultureData *data);      // <SyntheticSupport/>
    static FCDECL1(FC_BOOL_RET, nativeEnumSystemLocales, I4ARRAYREF *localesArray);        // <SyntheticSupport/>
    static FCDECL3(Object*, nativeGetCultureName, INT32 lcid, CLR_BOOL useSNameLCType, CLR_BOOL getMonthName);   // <SyntheticSupport/>

    //
    //  Native helper functions for methods in CompareInfo.
    //
    static FCDECL5(INT32, Compare,           INT_PTR pNativeCompareInfo, INT32 LCID, StringObject* pString1UNSAFE, StringObject* pString2UNSAFE, INT32 dwFlags);
    static FCDECL9(INT32, CompareRegion,     INT_PTR pNativeCompareInfo, INT32 LCID, StringObject* pString1, INT32 Offset1, INT32 Length1, StringObject* pString2, INT32 Offset2, INT32 Length2, INT32 dwFlags);
    static FCDECL7(INT32, IndexOfChar,       INT_PTR pNativeCompareInfo, INT32 LCID, StringObject* pStringUNSAFE,  CLR_CHAR ch, INT32 StartIndex, INT32 Count, INT32 dwFlags);
    static FCDECL7(INT32, IndexOfString,     INT_PTR pNativeCompareInfo, INT32 LCID, StringObject* pString1UNSAFE, StringObject* pString2UNSAFE, INT32 StartIndex, INT32 Count, INT32 dwFlags);
    static FCDECL7(INT32, LastIndexOfChar,   INT_PTR pNativeCompareInfo, INT32 LCID, StringObject* pStringUNSAFE,  CLR_CHAR ch, INT32 StartIndex, INT32 Count, INT32 dwFlags);
    static FCDECL7(INT32, LastIndexOfString, INT_PTR pNativeCompareInfo, INT32 LCID, StringObject* pString1UNSAFE, StringObject* pString2UNSAFE, INT32 StartIndex, INT32 Count, INT32 dwFlags);
    static FCDECL5(FC_BOOL_RET, nativeIsPrefix,    INT_PTR pNativeCompareInfo, INT32 LCID, StringObject* pString1, StringObject* pString2, INT32 dwFlags);
    static FCDECL5(FC_BOOL_RET, nativeIsSuffix,    INT_PTR pNativeCompareInfo, INT32 LCID, StringObject* pString1, StringObject* pString2, INT32 dwFlags);

    static FCDECL5(INT32, IndexOfStringOrdinalIgnoreCase,       INT_PTR ptr, StringObject* pString1UNSAFE, StringObject * pString2UNSAFE, INT32 startIndex, INT32 count);
    static FCDECL5(INT32, LastIndexOfStringOrdinalIgnoreCase,   INT_PTR ptr, StringObject* pString1UNSAFE, StringObject * pString2UNSAFE, INT32 startIndex, INT32 count);

    static FCDECL5(INT32, IndexOfCharOrdinalIgnoreCase,       INT_PTR ptr, StringObject* pString1UNSAFE, CLR_CHAR value, INT32 startIndex, INT32 count);
    static FCDECL5(INT32, LastIndexOfCharOrdinalIgnoreCase,   INT_PTR ptr, StringObject* pString1UNSAFE, CLR_CHAR value, INT32 startIndex, INT32 count);

    // <SyntheticSupport/>
    static FCDECL8(INT32, nativeCompareString, INT32 lcid, StringObject* string1, INT32 offset1, INT32 length1, StringObject* string2, INT32 offset2, INT32 length2, INT32 flags);

    //
    //  Native helper functions for methods in SortKey.
    //
    static FCDECL4(Object*, nativeCreateSortKey, INT_PTR pNativeCompareInfo, StringObject* pStringUNSAFE, INT32 dwFlags, INT32 SortId);

    static FCDECL4(INT32, nativeGetGlobalizedHashCode, INT_PTR pNativeCompareInfo, StringObject* pStringUNSAFE, INT32 dwFlagsIn, INT32 SortId);

    //
    //  Native helper functions for methods in NLSDataTable
    //
    //    static INT32 __stdcall GetLCIDFromCultureName(NLSDataTable_GetLCIDFromCultureNameArgs* pargs);

    //
    //  Native helper function for methods in Calendar
    //
    static FCDECL1(INT32, nativeGetTwoDigitYearMax, INT32 nValue);

    //
    //  Native helper function for methods in TimeZone
    //
    static FCDECL0(LONG, nativeGetTimeZoneMinuteOffset);
    static FCDECL0(Object*, nativeGetStandardName);
    static FCDECL0(Object*, nativeGetDaylightName);
    static FCDECL0(Object*, nativeGetDaylightChanges);

    //
    //  Native helper function for methods in TextInfo
    //
    static FCDECL0(INT32, nativeGetNumEncodingItems);
    static FCDECL0(EncodingDataItem *, nativeGetEncodingTableDataPointer);
    static FCDECL0(CodePageDataItem *, nativeGetCodePageTableDataPointer);

    //
    //  Native helper function for methods in CharacterInfo
    //
    static FCDECL0(void, AllocateCharTypeTable);

    //
    //  Native helper functions for methods in GlobalizationAssembly.
    //
    static FCDECL1(LPVOID, nativeCreateGlobalizationAssembly, AssemblyBaseObject* pAssembly);
    static FCDECL3(LPVOID, nativeCreateOpenFileMapping, 
                       StringObject* inSectionName, int inBytesToAllocate, HANDLE *mappedFile);
    //
    //  Native helper functions for methods in CompareInfo.
    //
    static FCDECL2(LPVOID, InitializeNativeCompareInfo, INT_PTR pNativeGlobalizationAssembly, INT32 sortID);
    static FCDECL2(FC_BOOL_RET, nativeIsSortable, INT_PTR pNativeCompareInfo, StringObject* pString);

    //
    //  Native helper function for methods in TextInfo
    //
    static FCDECL4(FC_CHAR_RET, nativeChangeCaseChar, INT32, INT_PTR, CLR_CHAR, CLR_BOOL);
    static FCDECL6(void, nativeChangeCaseSurrogate, INT_PTR, CLR_CHAR, CLR_CHAR, __out/*_ecount(1)*/ WCHAR*, __out/*_ecount(1)*/ WCHAR*, CLR_BOOL);
    static FCDECL2(FC_CHAR_RET, nativeGetTitleCaseChar, INT_PTR , CLR_CHAR);
    static FCDECL3(INT32, CompareOrdinalIgnoreCase, INT_PTR , StringObject*   pString1UNSAFE,  StringObject*   pString2UNSAFE);
    static FCDECL6(INT32, CompareOrdinalIgnoreCaseEx, INT_PTR , StringObject*   pString1UNSAFE,  INT32 index1, 
                   StringObject*   pString2UNSAFE, INT32 index2, INT32 length);

    static FCDECL2(INT32, GetHashCodeOrdinalIgnoreCase, INT_PTR, StringObject*   pString);
    static FCDECL4(Object*, nativeChangeCaseString, INT32 nLCID, INT_PTR pNativeTextInfo, StringObject* pString, CLR_BOOL bIsToUpper);
    static FCDECL0(LPVOID, nativeGetInvariantTextInfo);
    static FCDECL1(LPVOID, AllocateDefaultCasingTable, INT_PTR);
    static FCDECL2(LPVOID, AllocateCasingTable, INT_PTR bytePtr, INT32 exceptionIndex);

    static CasingTable* m_pCasingTable;
    static NativeTextInfo* m_pInvariantTextInfo;


    //
    // Native helper function for methods in Normalization
    //
    static FCDECL0(FC_BOOL_RET, nativeLoadNormalizationDLL);
    static FCDECL6(int, nativeNormalizationNormalizeString,
        int NormForm, int& iError,
        StringObject* inString, int inLength,
        CHARArray* outChars, int outLength);
    static FCDECL4(int, nativeNormalizationIsNormalizedString,
        int NormForm, int& iError,
        StringObject* inString, int cwLength);
    static FCDECL2(BYTE*, nativeNormalizationInitNormalization,
        int NormForm, BYTE* pTableData);
    static FCDECL0(INT32, nativeGetCurrentCalendar);        

    static FCDECL0(Object*, nativeGetWindowsDirectory);
    static FCDECL1(FC_BOOL_RET, nativeFileExists, StringObject* fileName);
    static FCDECL2(CONST INT32* , nativeGetStaticInt32DataTable, INT32 dataTableType, INT32* pDataTableSize);

private:

    //
    //  Internal helper functions.
    //
    static LPVOID internalEnumSystemLocales(DWORD dwFlags);
    static PTRARRAYREF GetMultiStringValues(__in LPWSTR pInfoStr);
    static INT32  CompareFast(STRINGREF strA, STRINGREF strB, BOOL *pbDifferInCaseOnly);
    static INT32 CompareOrdinal(__in_ecount(Length1) WCHAR* strAChars, int Length1, __in_ecount(Length2) WCHAR* strBChars, int Length2 );
    static INT32 __stdcall  DoCompareChars(WCHAR charA, WCHAR charB, BOOL *bDifferInCaseOnly);
    static inline INT32  DoComparisonLookup(wchar_t charA, wchar_t charB);
    static void   ConvertStringCaseFast(__in_ecount(length) WCHAR *inBuff, __out_ecount(length) WCHAR* outBuff, INT32 length, BOOL bIsToUpper);
    static INT32  FastIndexOfString(__in WCHAR *sourceString, INT32 startIndex, INT32 endIndex, __in_ecount(patternLength) WCHAR *pattern, INT32 patternLength);
    static INT32  FastIndexOfStringInsensitive(__in WCHAR *sourceString, INT32 startIndex, INT32 endIndex, __in_ecount(patternLength) WCHAR *pattern, INT32 patternLength);
    static INT32  FastLastIndexOfString(__in WCHAR *sourceString, INT32 startIndex, INT32 endIndex, __in_ecount(patternLength) WCHAR *pattern, INT32 patternLength);
    static INT32  FastLastIndexOfStringInsensitive(__in WCHAR *sourceString, INT32 startIndex, INT32 endIndex, __in_ecount(patternLength) WCHAR *pattern, INT32 patternLength);

    static INT32 CallGetLocaleInfo(INT32 lcid, int lcType, INT32* pOutputInt32, STRINGREF* pOutputStrRef);
    static BOOL GetNativeDigitsFromWin32(INT32 lcid, PTRARRAYREF* pOutputStrAry);

    static PVOID CreateOSCasingTableMemorySection(DWORD *pFileSize);
    
    static BOOL IsUnicodeSystem();

    //
    //  Definitions.
    //

    #define CULTUREINFO_OPTIONS_SIZE 32



    // Normalization
    static HMODULE m_hNormalization;
    static PFN_NORMALIZATION_IS_NORMALIZED_STRING m_pfnNormalizationIsNormalizedStringFunc;
    static PFN_NORMALIZATION_NORMALIZE_STRING m_pfnNormalizationNormalizeStringFunc;
    static PFN_NORMALIZATION_INIT_NORMALIZATION m_pfnNormalizationInitNormalizationFunc;


private:
    //
    // Internal encoding data tables.
    //
    const static int m_nEncodingDataTableItems;
    const static EncodingDataItem EncodingDataTable[];

    const static int m_nCodePageTableItems;
    const static CodePageDataItem CodePageDataTable[];

    // LCTYPEs for calling GetLocaleInfo() for date/time formatting information.  Used in nativeGetDTFIValues().
    const static INT32 dtfiLCTypes[];


    ////////////////////////////////////////////////////////////////////////////
    //
    // Add tables in COMNlsInfoData.cpp here.
    //

    // Static INT32 tables.  You can extend COMNlsInfo::nativeGetStaticInt32DataTable() to return different types of
    // INT32 tables.
    //

    // Constatnts used in COMNlsInfo::nativeGetStaticInt32DataTable
    #define INT32TABLE_FIRST_TABLE_ITEM                     0

    #define INT32TABLE_EVERETT_REGION_DATA_ITEM_MAPPINGS    0

    #define INT32TABLE_EVERETT_CULTURE_DATA_ITEM_MAPPINGS   1

    #define INT32TABLE_EVERETT_DATA_ITEM_TO_LCID_MAPPINGS   2

    #define INT32TABLE_EVERETT_REGION_DATA_ITEM_TO_LCID_MAPPINGS   3
   
    
    // UPDATE THE LAST ITEM IF YOU EXTEND THE TABLES IN nativeGetStaticInt32DataTable()
    // UPDATE THE LAST ITEM IF YOU EXTEND THE TABLES IN nativeGetStaticInt32DataTable()
    // UPDATE THE LAST ITEM IF YOU EXTEND THE TABLES IN nativeGetStaticInt32DataTable()
    #define INT32TABLE_LAST_TABLE_ITEM                      3

    const static INT32 m_nEverettRegionDataItemMappings[];
    const static INT32 m_nEverettRegionDataItemMappingsSize;    // The size of the table.
    
    const static INT32 m_nEverettCultureDataItemMappings[];
    const static INT32 m_nEverettCultureDataItemMappingsSize;   // The size of the table.
    
    const static INT32 m_nEverettDataItemToLCIDMappings[];
    const static INT32 m_nEverettDataItemToLCIDMappingsSize;    // The size of the table.

    const static INT32 m_nEverettRegionInfoDataItemToLCIDMappings[];
    const static INT32 m_nEverettRegionInfoDataItemToLCIDMappingsSize; // The size of the table.

    //
    // Other tables.
    //
    const static WCHAR ToUpperMapping[];
    const static WCHAR ToLowerMapping[];
    const static INT8 ComparisonTable[0x80][0x80];

    //
    // End of tables in COMNlsInfoData.cpp
    //
    ////////////////////////////////////////////////////////////////////////////

};


class NativeTextInfo; //Defined in clr\src\ClassLibNative\NLS;

class InternalCasingHelper {
    private:
    static NativeTextInfo* pNativeTextInfo;

    public:
    //
    // Native helper functions to do correct casing operations in
    // runtime native code.
    //

    // Convert szIn to lower case in the Invariant locale. (WARNING: May throw.)
    static INT32 InvariantToLower(LPUTF8 szOut, int cMaxBytes, LPCUTF8 szIn);

    // Convert szIn to lower case in the Invariant locale. (WARNING: This version
    // won't throw but you must have called InternalCasingHelper::InitTable()
    // already and it will use stack space as an intermediary (so don't
    // use for ridiculously long strings.)
    static INT32 InvariantToLowerNoThrow(LPUTF8 szOut, int cMaxBytes, LPCUTF8 szIn);

    // Convert szIn to lower case in the Invariant locale.
    static INT32 InvariantToLowerHelper(LPUTF8 szOut, int cMaxBytes, LPCUTF8 szIn, BOOL fAllowThrow);

    // Compare two unicode strings in the Invariant locale. (WARNING: May throw.)
    static INT32 InvariantCaseInsensitiveCompare(__in_ecount(Length1) const WCHAR *string1, int Length1, 
                                                 __in_ecount(Length2) const WCHAR *string2, int Length2);

    static void InitTable();
};
#endif  // _COMNLSINFO_H
