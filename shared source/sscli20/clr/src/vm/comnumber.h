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
#ifndef _COMNUMBER_H_
#define _COMNUMBER_H_

#include "common.h"

#include <pshpack1.h>

class NumberFormatInfo: public Object
{
public:
    // C++ data members                 // Corresponding data member in NumberFormat.cs
                                        // Also update mscorlib.h when you add/remove fields

    I4ARRAYREF cNumberGroup;        // numberGroupSize
    I4ARRAYREF cCurrencyGroup;      // currencyGroupSize
    I4ARRAYREF cPercentGroup;       // percentGroupSize
    
    STRINGREF sPositive;            // positiveSign
    STRINGREF sNegative;            // negativeSign
    STRINGREF sNumberDecimal;       // numberDecimalSeparator
    STRINGREF sNumberGroup;         // numberGroupSeparator
    STRINGREF sCurrencyGroup;       // currencyDecimalSeparator
    STRINGREF sCurrencyDecimal;     // currencyGroupSeparator
    STRINGREF sCurrency;            // currencySymbol
    STRINGREF sAnsiCurrency;        // ansiCurrencySymbol
    STRINGREF sNaN;                 // nanSymbol
    STRINGREF sPositiveInfinity;    // positiveInfinitySymbol
    STRINGREF sNegativeInfinity;    // negativeInfinitySymbol
    STRINGREF sPercentDecimal;      // percentDecimalSeparator
    STRINGREF sPercentGroup;        // percentGroupSeparator
    STRINGREF sPercent;             // percentSymbol
    STRINGREF sPerMille;            // perMilleSymbol

    PTRARRAYREF sNativeDigits;      // nativeDigits (a string array)
    
    INT32 iDataItem;                // Index into the CultureInfo Table.  Only used from managed code.
    INT32 cNumberDecimals;          // numberDecimalDigits
    INT32 cCurrencyDecimals;        // currencyDecimalDigits
    INT32 cPosCurrencyFormat;       // positiveCurrencyFormat
    INT32 cNegCurrencyFormat;       // negativeCurrencyFormat
    INT32 cNegativeNumberFormat;    // negativeNumberFormat
    INT32 cPositivePercentFormat;   // positivePercentFormat
    INT32 cNegativePercentFormat;   // negativePercentFormat
    INT32 cPercentDecimals;         // percentDecimalDigits
    INT32 iDigitSubstitution;       // digitSubstitution

    CLR_BOOL bIsReadOnly;              // Is this NumberFormatInfo ReadOnly?
    CLR_BOOL bUseUserOverride;         // Flag to use user override. Only used from managed code.
    
};

typedef NumberFormatInfo * NUMFMTREF;

#define NUMBER_MAXDIGITS 50

struct NUMBER {
    int precision;
    int scale;
    int sign;
    wchar_t digits[NUMBER_MAXDIGITS + 1];
    NUMBER() : precision(0), scale(0), sign(0) {}
};

class COMNumber
{
public:
    static FCDECL3_VII(Object*, FormatDecimal, DECIMAL value, StringObject* formatUNSAFE, NumberFormatInfo* numfmtUNSAFE);
    static FCDECL3_VII(Object*, FormatDouble,  double  value, StringObject* formatUNSAFE, NumberFormatInfo* numfmtUNSAFE);
    static FCDECL3_VII(Object*, FormatSingle,  float   value, StringObject* formatUNSAFE, NumberFormatInfo* numfmtUNSAFE);
    static FCDECL3(Object*, FormatInt32,   INT32      value, StringObject* formatUNSAFE, NumberFormatInfo* numfmtUNSAFE);
    static FCDECL3(Object*, FormatUInt32,  UINT32     value, StringObject* formatUNSAFE, NumberFormatInfo* numfmtUNSAFE);
    static FCDECL3_VII(Object*, FormatInt64,   INT64  value, StringObject* formatUNSAFE, NumberFormatInfo* numfmtUNSAFE);
    static FCDECL3_VII(Object*, FormatUInt64,  UINT64 value, StringObject* formatUNSAFE, NumberFormatInfo* numfmtUNSAFE);

    static FCDECL2(FC_BOOL_RET, NumberBufferToDecimal, BYTE* number, DECIMAL* value);
    static FCDECL2(FC_BOOL_RET, NumberBufferToDouble, BYTE* number, double* value);
    
    static wchar_t* Int32ToDecChars(wchar_t* p, unsigned int value, int digits);
};

#include <poppack.h>

#endif // _COMNUMBER_H_
