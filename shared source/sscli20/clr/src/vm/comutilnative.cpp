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
** File:  COMUtilNative
**
**                                        
**
** Purpose: A dumping ground for classes which aren't large
** enough to get their own file in the EE.
**
** Date:  April 8, 1998
**
===========================================================*/
#include "common.h"
#include "object.h"
#include "excep.h"
#include "vars.hpp"
#include "comstring.h"
#include "comutilnative.h"
#include "comstringcommon.h"
#include "comobject.h"

#include "utilcode.h"
#include "frames.h"
#include "field.h"
#include "gcscan.h"
#include "winwrap.h"
#include "gc.h"
#include "fcall.h"
#include "invokeutil.h"
#include "eeconfig.h"
#include "typestring.h"
#include "ndpversion.h"


#define MANAGED_LOGGING_ENABLE   L"LogEnable"
#define MANAGED_LOGGING_CONSOLE  L"LogToConsole"
#define MANAGED_LOGGING_FACILITY L"ManagedLogFacility"
#define MANAGED_LOGGING_LEVEL    L"LogLevel"
#define MANAGED_PERF_WARNINGS    L"BCLPerfWarnings"
#define MANAGED_CORRECTNESS_WARNINGS  L"BCLCorrectnessWarnings"
#define MANAGED_SAFEHANDLE_STACKTRACES  L"SafeHandleStackTraces"
#define STACK_OVERFLOW_MESSAGE   L"StackOverflowException"

//These are defined in System.ParseNumbers and should be kept in sync.
#define PARSE_TREATASUNSIGNED 0x200
#define PARSE_TREATASI1 0x400
#define PARSE_TREATASI2 0x800
#define PARSE_ISTIGHT 0x1000
#define PARSE_NOSPACE 0x2000

#define IS_WHITESPACE(__c) ((__c)=='\t' || (__c)==' ' || (__c)=='\r' || (__c)=='\n')

const WCHAR BitConverter::base64[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/','='};


// Prototype for m_memmove, which is defined in COMSystem.cpp and used here
// by Buffer's BlockCopy & InternalBlockCopy methods.
#if defined(_X86_)
void m_memmove(BYTE* dmem, BYTE* smem, int size);
#else
    // On WIN64 the CRT implementation of memmove is actually faster than the CLR implementation of m_memmove().
    #define m_memmove(a, b, c) memmove((a), (b), (c))
#endif


//
//
// COMCharacter and Helper functions
//
//


/*============================GetCharacterInfoHelper============================
**Determines character type info (digit, whitespace, etc) for the given char.
**Args:   c is the character on which to operate.
**        CharInfoType is one of CT_CTYPE1, CT_CTYPE2, CT_CTYPE3 and specifies the type
**        of information being requested.
**Returns: The bitmask returned by GetStringTypeEx.  The caller needs to know
**         how to interpret this.
**Exceptions: ArgumentException if GetStringTypeEx fails.
==============================================================================*/
INT32 GetCharacterInfoHelper(WCHAR c, INT32 CharInfoType)
{
    WRAPPER_CONTRACT;

    unsigned short result=0;

    {
        if (!GetStringTypeEx(LOCALE_USER_DEFAULT, CharInfoType, &(c), 1, &result)) {
            _ASSERTE(!"This should not happen, verify the arguments passed to GetStringTypeEx()");
        }
    }
    return(INT32)result;
}


/*==============================nativeIsWhiteSpace==============================
**The locally available version of IsWhiteSpace.  Designed to be called by other
**native methods.  The work is mostly done by GetCharacterInfoHelper
**Args:  c -- the character to check.
**Returns: true if c is whitespace, false otherwise.
**Exceptions:  Only those thrown by GetCharacterInfoHelper.
==============================================================================*/
BOOL COMCharacter::nativeIsWhiteSpace(WCHAR c)
{
    WRAPPER_CONTRACT;
    return((GetCharacterInfoHelper(c, CT_CTYPE1) & C1_SPACE)!=0);
}

/*================================nativeIsDigit=================================
**The locally available version of IsDigit.  Designed to be called by other
**native methods.  The work is mostly done by GetCharacterInfoHelper
**Args:  c -- the character to check.
**Returns: true if c is whitespace, false otherwise.
**Exceptions:  Only those thrown by GetCharacterInfoHelper.
==============================================================================*/
BOOL COMCharacter::nativeIsDigit(WCHAR c)
{
    WRAPPER_CONTRACT;
    return((GetCharacterInfoHelper(c, CT_CTYPE1) & C1_DIGIT)!=0);
}

//
//
// PARSENUMBERS (and helper functions)
//
//

/*===================================IsDigit====================================
**Returns a bool indicating whether the character passed in represents a   **
**digit.
==============================================================================*/
bool IsDigit(WCHAR c, int radix, int *result)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(result));
    }
    CONTRACTL_END;

    if (IS_DIGIT(c)) {
        *result = DIGIT_TO_INT(c);
    }
    else if (c>='A' && c<='Z') {
        //+10 is necessary because A is actually 10, etc.
        *result = c-'A'+10;
    }
    else if (c>='a' && c<='z') {
        //+10 is necessary because a is actually 10, etc.
        *result = c-'a'+10;
    }
    else {
        *result = -1;
    }

    if ((*result >=0) && (*result < radix))
        return true;

    return false;
}

INT32 wtoi(__in_ecount(length) WCHAR* wstr, DWORD length)
{  
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(wstr));
        PRECONDITION(length >= 0);
    }
    CONTRACTL_END;

    DWORD i = 0;
    int value;
    INT32 result = 0;

    while ( (i < length) && (IsDigit(wstr[i], 10 ,&value)) ) {
        //Read all of the digits and convert to a number
        result = result*10 + value;
        i++;
    }

    return result;
}

INT32 ParseNumbers::GrabInts(const INT32 radix, __in_ecount(length) WCHAR *buffer, const int length, int *i, BOOL isUnsigned)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(buffer));
        PRECONDITION(CheckPointer(i));
        PRECONDITION(*i >= 0);
        PRECONDITION(length >= 0);
        PRECONDITION( radix==2 || radix==8 || radix==10 || radix==16 );
    }
    CONTRACTL_END;

    UINT32 result=0;
    int value;
    UINT32 maxVal;

    // Allow all non-decimal numbers to set the sign bit.
    if (radix==10 && !isUnsigned) {
        maxVal = (0x7FFFFFFF / 10);

        //Read all of the digits and convert to a number
        while (*i<length&&(IsDigit(buffer[*i],radix,&value))) {
            // Check for overflows - this is sufficient & correct.
            if (result > maxVal || ((INT32)result)<0)
                COMPlusThrow(kOverflowException, L"Overflow_Int32");
            result = result*radix + value;
            (*i)++;
        }
        if ((INT32)result<0 && result!=0x80000000)
            COMPlusThrow(kOverflowException, L"Overflow_Int32");

    }
    else {
        maxVal = ((UINT32) -1) / radix;

        //Read all of the digits and convert to a number
        while (*i<length&&(IsDigit(buffer[*i],radix,&value))) {
            // Check for overflows - this is sufficient & correct.
            if (result > maxVal)
                COMPlusThrow(kOverflowException, L"Overflow_UInt32");
            // the above check won't cover 4294967296 to 4294967299
            UINT32 temp = result*radix + value;
            if( temp < result) { // this means overflow as well
                COMPlusThrow(kOverflowException, L"Overflow_UInt32");
            }

            result = temp;
            (*i)++;
        }
    }
    return(INT32) result;
}

INT64 ParseNumbers::GrabLongs(const INT32 radix, __in_ecount(length) WCHAR *buffer, const int length, int *i, BOOL isUnsigned)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(buffer));
        PRECONDITION(CheckPointer(i));
        PRECONDITION(*i >= 0);
        PRECONDITION(length >= 0);
    }
    CONTRACTL_END;

    UINT64 result=0;
    int value;
    UINT64 maxVal;

    // Allow all non-decimal numbers to set the sign bit.
    if (radix==10 && !isUnsigned) {
        maxVal = (UI64(0x7FFFFFFFFFFFFFFF) / 10);

        //Read all of the digits and convert to a number
        while (*i<length&&(IsDigit(buffer[*i],radix,&value))) {
            // Check for overflows - this is sufficient & correct.
            if (result > maxVal || ((INT64)result)<0)
                COMPlusThrow(kOverflowException, L"Overflow_Int64");
            result = result*radix + value;
            (*i)++;
        }
        if ((INT64)result<0 && result!=UI64(0x8000000000000000))
            COMPlusThrow(kOverflowException, L"Overflow_Int64");

    }
    else {
        maxVal = ((UINT64) -1L) / radix;

        //Read all of the digits and convert to a number
        while (*i<length&&(IsDigit(buffer[*i],radix,&value))) {
            // Check for overflows - this is sufficient & correct.
            if (result > maxVal)
                COMPlusThrow(kOverflowException, L"Overflow_UInt64");

            UINT64 temp = result*radix + value;
            if( temp < result) { // this means overflow as well
                COMPlusThrow(kOverflowException, L"Overflow_UInt64");
            }
            result = temp;

            (*i)++;
        }
    }
    return(INT64) result;
}

void EatWhiteSpace(__in_ecount(length) WCHAR *buffer, int length, int *i)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(buffer));
        PRECONDITION(CheckPointer(i));
        PRECONDITION(length >= 0);
    }
    CONTRACTL_END;

    for (; *i<length && COMCharacter::nativeIsWhiteSpace(buffer[*i]); (*i)++);
}

FCIMPL5_VII(LPVOID, ParseNumbers::LongToString, INT64 n, INT32 radix, INT32 width, CLR_CHAR paddingChar, INT32 flags)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    LPVOID rv = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_0();

    bool isNegative = false;
    int index=0;
    int charVal;
    UINT64 l;
    INT32 i;
    INT32 buffLength=0;
    WCHAR buffer[67];//Longest possible string length for an integer in binary notation with prefix

    if (radix<MinRadix || radix>MaxRadix)
        COMPlusThrowArgumentException(L"radix", L"Arg_InvalidBase");

    //If the number is negative, make it positive and remember the sign.
    if (n<0) {
        isNegative=true;

        // For base 10, write out -num, but other bases write out the
        // 2's complement bit pattern
        if (10==radix)
            l = (UINT64)(-n);
        else
            l = (UINT64)n;
    }
    else {
        l=(UINT64)n;
    }

    if (flags&PrintAsI1)
        l = l&0xFF;
    else if (flags&PrintAsI2)
        l = l&0xFFFF;
    else if (flags&PrintAsI4)
        l=l&0xFFFFFFFF;

    //Special case the 0.
    if (0==l) {
        buffer[0]='0';
        index=1;
    }
    else {
        //Pull apart the number and put the digits (in reverse order) into the buffer.
        for (index=0; l>0; l=l/radix, index++) {
            if ((charVal=(int)(l%radix))<10)
                buffer[index] = (WCHAR)(charVal + '0');
            else
                buffer[index] = (WCHAR)(charVal + 'a' - 10);
        }
    }

    //If they want the base, append that to the string (in reverse order)
    if (radix!=10 && ((flags&PrintBase)!=0)) {
        if (16==radix) {
            buffer[index++]='x';
            buffer[index++]='0';
        }
        else if (8==radix) {
            buffer[index++]='0';
        }
        else if ((flags&PrintRadixBase)!=0) {
            buffer[index++]='#';
            buffer[index++]=((radix%10)+'0');
            buffer[index++]=((radix/10)+'0');
        }
    }

    if (10==radix) {
        //If it was negative, append the sign.
        if (isNegative) {
            buffer[index++]='-';
        }

        //else if they requested, add the '+';
        else if ((flags&PrintSign)!=0) {
            buffer[index++]='+';
        }

        //If they requested a leading space, put it on.
        else if ((flags&PrefixSpace)!=0) {
            buffer[index++]=' ';
        }
    }

    //Figure out the size of our string.
    if (width<=index)
        buffLength=index;
    else
        buffLength=width;

    STRINGREF Local = COMString::NewString(buffLength);
    WCHAR *LocalBuffer = Local->GetBuffer();

    //Put the characters into the String in reverse order
    //Fill the remaining space -- if there is any --
    //with the correct padding character.
    if ((flags&LeftAlign)!=0) {
        for (i=0; i<index; i++) {
            LocalBuffer[i]=buffer[index-i-1];
        }
        for (;i<buffLength; i++) {
            LocalBuffer[i]=paddingChar;
        }
    }
    else {
        for (i=0; i<index; i++) {
            LocalBuffer[buffLength-i-1]=buffer[i];
        }
        for (int j=buffLength-i-1; j>=0; j--) {
            LocalBuffer[j]=paddingChar;
        }
    }

    *((STRINGREF *)&rv)=Local;

    HELPER_METHOD_FRAME_END();

    return rv;
}
FCIMPLEND


FCIMPL5(LPVOID, ParseNumbers::IntToString, INT32 n, INT32 radix, INT32 width, CLR_CHAR paddingChar, INT32 flags)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    LPVOID rv = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_0();

    bool isNegative = false;
    int index=0;
    int charVal;
    int buffLength;
    int i;
    UINT32 l;
    WCHAR buffer[66];  //Longest possible string length for an integer in binary notation with prefix

    if (radix<MinRadix || radix>MaxRadix)
        COMPlusThrowArgumentException(L"radix", L"Arg_InvalidBase");

    //If the number is negative, make it positive and remember the sign.
    //If the number is MIN_VALUE, this will still be negative, so we'll have to
    //special case this later.
    if (n<0) {
        isNegative=true;
        // For base 10, write out -num, but other bases write out the
        // 2's complement bit pattern
        if (10==radix)
            l = (UINT32)(-n);
        else
            l = (UINT32)n;
    }
    else {
        l=(UINT32)n;
    }

    //The conversion to a UINT will sign extend the number.  In order to ensure
    //that we only get as many bits as we expect, we chop the number.
    if (flags&PrintAsI1) {
        l = l&0xFF;
    }
    else if (flags&PrintAsI2) {
        l = l&0xFFFF;
    }
    else if (flags&PrintAsI4) {
        l=l&0xFFFFFFFF;
    }

    //Special case the 0.
    if (0==l) {
        buffer[0]='0';
        index=1;
    }
    else {
        do {
            charVal = l%radix;
            l=l/radix;
            if (charVal<10) {
                buffer[index++] = (WCHAR)(charVal + '0');
            }
            else {
                buffer[index++] = (WCHAR)(charVal + 'a' - 10);
            }
        }
        while (l!=0);
    }

    //If they want the base, append that to the string (in reverse order)
    if (radix!=10 && ((flags&PrintBase)!=0)) {
        if (16==radix) {
            buffer[index++]='x';
            buffer[index++]='0';
        }
        else if (8==radix) {
            buffer[index++]='0';
        }
    }

    if (10==radix) {
        //If it was negative, append the sign.
        if (isNegative) {
            buffer[index++]='-';
        }

        //else if they requested, add the '+';
        else if ((flags&PrintSign)!=0) {
            buffer[index++]='+';
        }

        //If they requested a leading space, put it on.
        else if ((flags&PrefixSpace)!=0) {
            buffer[index++]=' ';
        }
    }

    //Figure out the size of our string.
    if (width<=index) {
        buffLength=index;
    }
    else {
        buffLength=width;
    }

    STRINGREF Local = COMString::NewString(buffLength);
    WCHAR *LocalBuffer = Local->GetBuffer();

    //Put the characters into the String in reverse order
    //Fill the remaining space -- if there is any --
    //with the correct padding character.
    if ((flags&LeftAlign)!=0) {
        for (i=0; i<index; i++) {
            LocalBuffer[i]=buffer[index-i-1];
        }
        for (;i<buffLength; i++) {
            LocalBuffer[i]=paddingChar;
        }
    }
    else {
        for (i=0; i<index; i++) {
            LocalBuffer[buffLength-i-1]=buffer[i];
        }
        for (int j=buffLength-i-1; j>=0; j--) {
            LocalBuffer[j]=paddingChar;
        }
    }

    *((STRINGREF *)&rv)=Local;

    HELPER_METHOD_FRAME_END();

    return rv;
}
FCIMPLEND


/*===================================FixRadix===================================
**It's possible that we parsed the radix in a base other than 10 by accident.
**This method will take that number, verify that it only contained valid base 10
**digits, and then do the conversion to base 10.  If it contained invalid digits,
**they tried to pass us a radix such as 1A, so we throw a FormatException.
**
**Args: oldVal: The value that we had actually parsed in some arbitrary base.
**      oldBase: The base in which we actually did the parsing.
**
**Returns:  oldVal as if it had been parsed as a base-10 number.
**Exceptions: FormatException if either of the digits in the radix aren't
**            valid base-10 numbers.
==============================================================================*/
int FixRadix(int oldVal, int oldBase)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    int firstDigit = (oldVal/oldBase);
    int secondDigit = (oldVal%oldBase);

    if ((firstDigit>=10) || (secondDigit>=10))
        COMPlusThrow(kFormatException, L"Format_BadBase");

    return(firstDigit*10)+secondDigit;
}

/*=================================StringToLong=================================
**Action:
**Returns:
**Exceptions:
==============================================================================*/
FCIMPL4(INT64, ParseNumbers::StringToLong, StringObject * s, INT32 radix, INT32 flags, INT32 *currPos)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    INT64 result = 0;

    HELPER_METHOD_FRAME_BEGIN_RET_1(s);

    int sign = 1;
    WCHAR *input;
    int length;
    int i;
    int grabNumbersStart=0;
    INT32 r;

    _ASSERTE((flags & PARSE_TREATASI1) == 0 && (flags & PARSE_TREATASI2) == 0);

    if (s) {
        i = currPos ? *currPos : 0;

        //Do some radix checking.
        //A radix of -1 says to use whatever base is spec'd on the number.
        //Parse in Base10 until we figure out what the base actually is.
        r = (-1==radix)?10:radix;

        if (r!=2 && r!=10 && r!=8 && r!=16)
            COMPlusThrow(kArgumentException, L"Arg_InvalidBase");

        RefInterpretGetStringValuesDangerousForGC(s, &input, &length);

        if (i<0 || i>=length)
            COMPlusThrowArgumentOutOfRange(L"startIndex", L"ArgumentOutOfRange_Index");

        //Get rid of the whitespace and then check that we've still got some digits to parse.
        if (!(flags & PARSE_ISTIGHT) && !(flags & PARSE_NOSPACE)) {
            EatWhiteSpace(input,length,&i);
            if (i==length)
                COMPlusThrow(kFormatException, L"Format_EmptyInputString");
        }

        //Check for a sign
        if (input[i]=='-') {
            if (r != 10)
                COMPlusThrow(kArgumentException, L"Arg_CannotHaveNegativeValue");

            if (flags & PARSE_TREATASUNSIGNED)
                COMPlusThrow(kOverflowException, L"Overflow_NegativeUnsigned");

            sign = -1;
            i++;
        }
        else if (input[i]=='+') {
            i++;
        }

        if ((radix==-1 || radix==16) && (i+1<length) && input[i]=='0') {
            if (input[i+1]=='x' || input [i+1]=='X') {
                r=16;
                i+=2;
            }
        }

        grabNumbersStart=i;
        result = GrabLongs(r,input,length,&i, (flags & PARSE_TREATASUNSIGNED));

        //Check if they passed us a string with no parsable digits.
        if (i==grabNumbersStart)
            COMPlusThrow(kFormatException, L"Format_NoParsibleDigits");

        if (flags & PARSE_ISTIGHT) {
            //If we've got effluvia left at the end of the string, complain.
            if (i<length)
                COMPlusThrow(kFormatException, L"Format_ExtraJunkAtEnd");
        }

        //Put the current index back into the correct place.
        if (currPos != NULL) *currPos = i;

        //Return the value properly signed.
        if ((UINT64) result==UI64(0x8000000000000000) && sign==1 && r==10)
            COMPlusThrow(kOverflowException, L"Overflow_Int64");

        if (r == 10)
            result *= sign;
    }
    else {
        result = 0;
    }

    HELPER_METHOD_FRAME_END();

    return result;
}
FCIMPLEND

FCIMPL4(INT32, ParseNumbers::StringToInt, StringObject * s, INT32 radix, INT32 flags, INT32* currPos)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    INT32 result = 0;

    HELPER_METHOD_FRAME_BEGIN_RET_1(s);

    int sign = 1;
    WCHAR *input;
    int length;
    int i;
    int grabNumbersStart=0;
    INT32 r;

    // TreatAsI1 and TreatAsI2 are mutually exclusive.
    _ASSERTE(!((flags & PARSE_TREATASI1) != 0 && (flags & PARSE_TREATASI2) != 0));

    if (s) {
        //They're requied to tell me where to start parsing.
        i = currPos ? (*currPos) : 0;

        //Do some radix checking.
        //A radix of -1 says to use whatever base is spec'd on the number.
        //Parse in Base10 until we figure out what the base actually is.
        r = (-1==radix)?10:radix;

        if (r!=2 && r!=10 && r!=8 && r!=16)
            COMPlusThrow(kArgumentException, L"Arg_InvalidBase");

        RefInterpretGetStringValuesDangerousForGC(s, &input, &length);

        if (i<0 || i>=length)
            COMPlusThrowArgumentOutOfRange(L"startIndex", L"ArgumentOutOfRange_Index");

        //Get rid of the whitespace and then check that we've still got some digits to parse.
        if (!(flags & PARSE_ISTIGHT) && !(flags & PARSE_NOSPACE)) {
            EatWhiteSpace(input,length,&i);
            if (i==length)
                COMPlusThrow(kFormatException, L"Format_EmptyInputString");
        }

        //Check for a sign
        if (input[i]=='-') {
            if (r != 10)
                COMPlusThrow(kArgumentException, L"Arg_CannotHaveNegativeValue");

            if (flags & PARSE_TREATASUNSIGNED)
                COMPlusThrow(kOverflowException, L"Overflow_NegativeUnsigned");

            sign = -1;
            i++;
        }
        else if (input[i]=='+') {
            i++;
        }

        //Consume the 0x if we're in an unknown base or in base-16.
        if ((radix==-1||radix==16) && (i+1<length) && input[i]=='0') {
            if (input[i+1]=='x' || input [i+1]=='X') {
                r=16;
                i+=2;
            }
        }

        grabNumbersStart=i;
        result = GrabInts(r,input,length,&i, (flags & PARSE_TREATASUNSIGNED));

        //Check if they passed us a string with no parsable digits.
        if (i==grabNumbersStart)
            COMPlusThrow(kFormatException, L"Format_NoParsibleDigits");

        if (flags & PARSE_ISTIGHT) {
            //If we've got effluvia left at the end of the string, complain.
            if (i<(length))
                COMPlusThrow(kFormatException, L"Format_ExtraJunkAtEnd");
        }

        //Put the current index back into the correct place.
        if (currPos != NULL) *currPos = i;

        //Return the value properly signed.
        if (flags & PARSE_TREATASI1) {
            if ((UINT32)result > 0xFF)
                COMPlusThrow(kOverflowException, L"Overflow_SByte");

            // result looks positive when parsed as an I4
            _ASSERTE(sign==1 || r==10);
        }
        else if (flags & PARSE_TREATASI2) {
            if ((UINT32)result > 0xFFFF)
                COMPlusThrow(kOverflowException, L"Overflow_Int16");

            // result looks positive when parsed as an I4
            _ASSERTE(sign==1 || r==10);
        }
        else if ((UINT32) result==0x80000000U && sign==1 && r==10) {
            COMPlusThrow(kOverflowException, L"Overflow_Int32");
        }

        if (r == 10)
            result *= sign;
    }
    else {
        result = 0;
    }

    HELPER_METHOD_FRAME_END();

    return result;
}
FCIMPLEND

//
//
// EXCEPTION NATIVE
//
//
FCIMPL1(Object*, ExceptionNative::GetClassName, Object* pThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    ASSERT(pThisUNSAFE != NULL);

    STRINGREF   s       = NULL;
    OBJECTREF   pThis   = (OBJECTREF) pThisUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, pThis);

    TypeHandle t = pThis->GetTypeHandle();
    StackSString ss;
    TypeString::AppendType(ss, t);
    s = COMString::NewString(ss);

    HELPER_METHOD_FRAME_END();

    // force the stringref into an LPVOID
    return OBJECTREFToObject(s);
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, ExceptionNative::IsImmutableAgileException, Object* pExceptionUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    ASSERT(pExceptionUNSAFE != NULL);

    OBJECTREF pException = (OBJECTREF) pExceptionUNSAFE;
    BOOL      fIsImmutableAgile = FALSE;

    HELPER_METHOD_FRAME_BEGIN_RET_1(pException);

    // The preallocated exception objects may be used from multiple AppDomains
    // and therefore must remain immutable from the application's perspective.
    fIsImmutableAgile = CLRException::IsPreallocatedExceptionObject(pException);

    HELPER_METHOD_FRAME_END();

    FC_RETURN_BOOL(fIsImmutableAgile);
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, ExceptionNative::IsTransient, INT32 hresult)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    BOOL fIsTransient = FALSE;

    HELPER_METHOD_FRAME_BEGIN_RET_0();

    fIsTransient = Exception::IsTransient(hresult);

    HELPER_METHOD_FRAME_END();

    FC_RETURN_BOOL(fIsTransient);
}
FCIMPLEND

BSTR BStrFromString(STRINGREF s)
{
    CONTRACTL
    {
        THROWS;
    }
    CONTRACTL_END;

    WCHAR *wz;
    int cch;
    BSTR bstr;

    if (s == NULL)
        return NULL;

    RefInterpretGetStringValuesDangerousForGC(s, &wz, &cch);

    bstr = SysAllocString(wz);
    if (bstr == NULL)
        COMPlusThrowOM();

    return bstr;
}

static BSTR GetExceptionDescription(OBJECTREF objException)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION( ExceptionNative::IsException(objException->GetMethodTable()) );
    }
    CONTRACTL_END;

    BSTR bstrDescription;

    STRINGREF MessageString = NULL;
    GCPROTECT_BEGIN(MessageString)
    GCPROTECT_BEGIN(objException)
    {
        // read Exception.Message property
        MethodDescCallSite getMessage(METHOD__EXCEPTION__GET_MESSAGE, &objException);

        ARG_SLOT GetMessageArgs[] = { ObjToArgSlot(objException)};
        MessageString = getMessage.Call_RetSTRINGREF(GetMessageArgs);

        // if the message string is empty then use the exception classname.
        if (MessageString == NULL || MessageString->GetStringLength() == 0) {
            // call GetClassName
            MethodDescCallSite getClassName(METHOD__EXCEPTION__GET_CLASS_NAME, &objException);
            ARG_SLOT GetClassNameArgs[] = { ObjToArgSlot(objException)};
            MessageString = getClassName.Call_RetSTRINGREF(GetClassNameArgs);
            _ASSERTE(MessageString != NULL && MessageString->GetStringLength() != 0);
        }

        // Allocate the description BSTR.
        int DescriptionLen = MessageString->GetStringLength();
        bstrDescription = SysAllocStringLen(MessageString->GetBuffer(), DescriptionLen);
    }
    GCPROTECT_END();
    GCPROTECT_END();

    return bstrDescription;
}

static BSTR GetExceptionSource(OBJECTREF objException)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION( ExceptionNative::IsException(objException->GetMethodTable()) );
    }
    CONTRACTL_END;

    STRINGREF refRetVal;
    GCPROTECT_BEGIN(objException)

    // read Exception.Source property
    MethodDescCallSite getSource(METHOD__EXCEPTION__GET_SOURCE, &objException);

    ARG_SLOT GetSourceArgs[] = { ObjToArgSlot(objException)};

    refRetVal = getSource.Call_RetSTRINGREF(GetSourceArgs);

    GCPROTECT_END();
    return BStrFromString(refRetVal);
}

static void GetExceptionHelp(OBJECTREF objException, BSTR *pbstrHelpFile, DWORD *pdwHelpContext)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(ExceptionNative::IsException(objException->GetMethodTable()));
        PRECONDITION(CheckPointer(pbstrHelpFile));
        PRECONDITION(CheckPointer(pdwHelpContext));
    }
    CONTRACTL_END;

    *pdwHelpContext = 0;

    GCPROTECT_BEGIN(objException);

    // read Exception.HelpLink property
    MethodDescCallSite getHelpLink(METHOD__EXCEPTION__GET_HELP_LINK, &objException);

    ARG_SLOT GetHelpLinkArgs[] = { ObjToArgSlot(objException)};
    *pbstrHelpFile = BStrFromString(getHelpLink.Call_RetSTRINGREF(GetHelpLinkArgs));

    GCPROTECT_END();

    // parse the help file to check for the presence of helpcontext
    int len = SysStringLen(*pbstrHelpFile);
    int pos = len;
    WCHAR *pwstr = *pbstrHelpFile;
    if (pwstr) {
        BOOL fFoundPound = FALSE;

        for (pos = len - 1; pos >= 0; pos--) {
            if (pwstr[pos] == L'#') {
                fFoundPound = TRUE;
                break;
            }
        }

        if (fFoundPound) {
            int PoundPos = pos;
            int NumberStartPos = -1;
            BOOL bNumberStarted = FALSE;
            BOOL bNumberFinished = FALSE;
            BOOL bInvalidDigitsFound = FALSE;

            _ASSERTE(pwstr[pos] == L'#');

            // Check to see if the string to the right of the pound a valid number.
            for (pos++; pos < len; pos++) {
                if (bNumberFinished) {
                    if (!COMCharacter::nativeIsWhiteSpace(pwstr[pos])) {
                        bInvalidDigitsFound = TRUE;
                        break;
                    }
                }
                else if (bNumberStarted) {
                    if (COMCharacter::nativeIsWhiteSpace(pwstr[pos])) {
                        bNumberFinished = TRUE;
                    }
                    else if (!COMCharacter::nativeIsDigit(pwstr[pos])) {
                        bInvalidDigitsFound = TRUE;
                        break;
                    }
                }
                else {
                    if (COMCharacter::nativeIsDigit(pwstr[pos])) {
                        NumberStartPos = pos;
                        bNumberStarted = TRUE;
                    }
                    else if (!COMCharacter::nativeIsWhiteSpace(pwstr[pos])) {
                        bInvalidDigitsFound = TRUE;
                        break;
                    }
                }
            }

            if (bNumberStarted && !bInvalidDigitsFound) {
                // Grab the help context and remove it from the help file.
                *pdwHelpContext = (DWORD)wtoi(&pwstr[NumberStartPos], len - NumberStartPos);

                // Allocate a new help file string of the right length.
                BSTR strOld = *pbstrHelpFile;
                *pbstrHelpFile = SysAllocStringLen(strOld, PoundPos);
                SysFreeString(strOld);
                if (!*pbstrHelpFile)
                    COMPlusThrowOM();
            }
        }
    }
}

// NOTE: caller cleans up any partially initialized BSTRs in pED
void ExceptionNative::GetExceptionData(OBJECTREF objException, ExceptionData *pED)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(ExceptionNative::IsException(objException->GetMethodTable()));
        PRECONDITION(CheckPointer(pED));
    }
    CONTRACTL_END;

    ZeroMemory(pED, sizeof(ExceptionData));

    if (objException->GetMethodTable() == g_pStackOverflowExceptionClass) {
        // In a low stack situation, most everything else in here will fail.
        pED->hr = COR_E_STACKOVERFLOW;
        pED->bstrDescription = SysAllocString(STACK_OVERFLOW_MESSAGE);
        return;
    }

    GCPROTECT_BEGIN(objException);
    pED->hr = GetExceptionHResult(objException);
    pED->bstrDescription = GetExceptionDescription(objException);
    pED->bstrSource = GetExceptionSource(objException);
    GetExceptionHelp(objException, &pED->bstrHelpFile, &pED->dwHelpContext);
    GCPROTECT_END();
    return;
}



BOOL ExceptionNative::IsException(MethodTable* pMT)
{
    WRAPPER_CONTRACT;

    ASSERT(g_pExceptionClass != NULL);

    while (pMT != NULL && pMT != g_pExceptionClass) {
        pMT = pMT->GetParentMethodTable();
    }

    return pMT != NULL;
}

FCIMPL0(EXCEPTION_POINTERS*, ExceptionNative::GetExceptionPointers)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    EXCEPTION_POINTERS* retVal = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_0();

    Thread *pThread = GetThread();
    _ASSERTE(pThread);

    if (pThread->IsExceptionInProgress())
    {
        retVal = pThread->GetExceptionState()->GetExceptionPointers();
    }

    HELPER_METHOD_FRAME_END();

    return retVal;
}
FCIMPLEND

FCIMPL0(INT32, ExceptionNative::GetExceptionCode)
{
    WRAPPER_CONTRACT;
    INT32 retVal = 0;
    HELPER_METHOD_FRAME_BEGIN_RET_0();

    Thread *pThread = GetThread();
    _ASSERTE(pThread);

    if (pThread->IsExceptionInProgress())
    {
        retVal = pThread->GetExceptionState()->GetExceptionCode();
    }

    HELPER_METHOD_FRAME_END();
    return retVal;
}
FCIMPLEND


//
// This must be implemented as an FCALL because managed code cannot
// swallow a thread abort exception without resetting the abort,
// which we don't want to do.  Additionally, we can run into deadlocks
// if we use the ResourceManager to do resource lookups - it requires
// taking managed locks when initializing Globalization & Security,
// but a thread abort on a separate thread initializing those same
// systems would also do a resource lookup via the ResourceManager.
// We've deadlocked in CompareInfo.GetCompareInfo &
// Environment.GetResourceString.  It's not practical to take all of
// our locks within CER's to avoid this problem - just use the CLR's
// unmanaged resources.
//
FCIMPL1(StringObject*, ExceptionNative::GetMessageFromNativeResources, ExceptionMessageKind kind)
{
    CONTRACTL {
        DISABLED(GC_TRIGGERS);    // GC_TRIGGERS is not allowed in FCall yet.
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    STRINGREF msg = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_1(msg);

    SString buffer;
    HRESULT hr = S_OK;
    WCHAR * wszFallbackString = NULL;

    switch(kind) {
    case ThreadAbort:
        hr = buffer.LoadResourceAndReturnHR(IDS_EE_THREAD_ABORT, false);
        if (FAILED(hr)) {
            wszFallbackString = L"Thread was being aborted.";
        }
        break;

    case ThreadInterrupted:
        hr = buffer.LoadResourceAndReturnHR(IDS_EE_THREAD_INTERRUPTED, false);
        if (FAILED(hr)) {
            wszFallbackString = L"Thread was interrupted from a waiting state.";
        }
        break;

    case OutOfMemory:
        hr = buffer.LoadResourceAndReturnHR(IDS_EE_OUT_OF_MEMORY, false);
        if (FAILED(hr)) {
            wszFallbackString = L"Insufficient memory to continue the execution of the program.";
        }
        break;

    default:
        _ASSERTE(!"Unknown ExceptionMessageKind value!");
    }
    if (FAILED(hr)) {       
        STRESS_LOG1(LF_BCL, LL_ALWAYS, "LoadResource error: %x", hr);
        _ASSERTE(wszFallbackString != NULL);
        msg = COMString::NewString(wszFallbackString);
    }
    else {
        msg = COMString::NewString(buffer.GetUnicode());
    }

    HELPER_METHOD_FRAME_END();

    return STRINGREFToObject(msg);
}
FCIMPLEND


//
//
// GUID NATIVE
//
//

FCIMPL1(void, GuidNative::CompleteGuid, GUID* thisPtr)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_0();
    HRESULT hr;

    _ASSERTE(thisPtr != NULL);

    hr = CoCreateGuid(thisPtr);
    if (FAILED(hr)) {
        COMPlusThrowHR(hr);
    }

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


FCIMPL1(Object*, BitConverter::Base64StringToByteArray, StringObject* pInString)
{
    CONTRACTL
{
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
}
    CONTRACTL_END;

    STRINGREF inString(pInString);
    U1ARRAYREF bArray = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_1(inString);

    if (inString==NULL)
        COMPlusThrowArgumentNull(L"InString");

    INT32 inStringLength = (INT32)inString->GetStringLength();
    // empty string should be a valid case, return an empty array for it
    if ( inStringLength == 0) {
        bArray = (U1ARRAYREF)AllocatePrimitiveArray(ELEMENT_TYPE_U1, inStringLength);
    }
    else {
        WCHAR *c = inString->GetBuffer();

        CQuickBytes valueHolder;
        INT32 *value = (INT32 *)(valueHolder.AllocThrows(inStringLength * sizeof(INT32)));

        // Convert the characters in the string into an array of integers in the range [0-63].
        // returns the number of extra padded characters that we will discard.
        UINT trueLength=0; //Length ignoring whitespace
        int iend = ConvertBase64ToByteArray(value,c,0,inStringLength, &trueLength);

        if(trueLength == 0) {
            bArray = (U1ARRAYREF)AllocatePrimitiveArray(ELEMENT_TYPE_U1, 0);
        }
        else {
            if(trueLength %4 >0) {
            COMPlusThrow(kFormatException, L"Format_BadBase64CharArrayLength");
            }

        //Create the new byte array.  We can determine the size from the chars we read
        //out of the string.
        int blength = ( ( trueLength / 4 - 1 ) * 3 ) + ( 3 - iend );

        bArray = (U1ARRAYREF)AllocatePrimitiveArray(ELEMENT_TYPE_U1, blength);
        U1 *b = (U1*)bArray->GetDataPtr();

        //Walk the byte array and convert the int's into bytes in the proper base-64 notation.
        ConvertByteArrayToByteStream(value,b,blength);
    }
    }
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(bArray);
}
FCIMPLEND

FCIMPL3(Object*, BitConverter::Base64CharArrayToByteArray, CHARArray* pInCharArray, INT32 offset, INT32 length)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    CHARARRAYREF inCharArray(pInCharArray);
    U1ARRAYREF bArray = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_1(inCharArray);

    if (inCharArray==NULL)
        COMPlusThrowArgumentNull(L"InArray");

    if (length<0)
        COMPlusThrowArgumentOutOfRange(L"length", L"ArgumentOutOfRange_Index");

    if (offset<0)
        COMPlusThrowArgumentOutOfRange(L"offset", L"ArgumentOutOfRange_GenericPositive");

    UINT32     inArrayLength = inCharArray->GetNumComponents();

    if (offset > (INT32)(inArrayLength - length))
        COMPlusThrowArgumentOutOfRange(L"offset", L"ArgumentOutOfRange_OffsetLength");

    if ((length<4) /*|| ((length%4)>0)*/)
        COMPlusThrow(kFormatException, L"Format_BadBase64CharArrayLength");

    CQuickBytes valueHolder;
    INT32 *value = (INT32 *)(valueHolder.AllocThrows(length * sizeof(INT32)));

    WCHAR *c = (WCHAR *)inCharArray->GetDataPtr();
    UINT trueLength=0; //Length excluding whitespace
    int iend = ConvertBase64ToByteArray(value,c,offset,length, &trueLength);

    if (trueLength%4>0)
        COMPlusThrow(kFormatException, L"Format_BadBase64CharArrayLength");

    //Create the new byte array.  We can determine the size from the chars we read
    //out of the string.
    int blength = (trueLength > 0) ? ( ( ( trueLength / 4 - 1 ) * 3 ) + ( 3 - iend ) ) : 0;

    bArray = (U1ARRAYREF)AllocatePrimitiveArray(ELEMENT_TYPE_U1, blength);
    U1 *b = (U1*)bArray->GetDataPtr();

    ConvertByteArrayToByteStream(value,b,blength);

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(bArray);
}
FCIMPLEND

//Convert the characters on the stream into an array of integers in the range [0-63].
INT32 BitConverter::ConvertBase64ToByteArray(__out_ecount(length) INT32 *value, __in_ecount(length + offset) WCHAR *c,UINT offset,UINT length, UINT *nonWhiteSpaceChars)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(value));
        PRECONDITION(CheckPointer(c));
        PRECONDITION(CheckPointer(nonWhiteSpaceChars));
    }
    CONTRACTL_END;
    PREFIX_ASSUME(value != NULL);
    int iend = 0;
    int intA = (int)'A';
    int intZ = (int)'Z';
    int inta = (int)'a';
    int intz = (int)'z';
    int int0 = (int)'0';
    int int9 = (int)'9';

    int currBytePos = 0;


    //Convert the characters on the stream into an array of integers in the range [0-63].
    for (UINT i=offset; i<length+offset; i++) {
        int ichar = (int)c[i];
        if ((ichar >= intA)&&(ichar <= intZ))
            value[currBytePos++] = ichar - intA;
        else if ((ichar >= inta)&&(ichar <= intz))
            value[currBytePos++] = ichar - inta + 26;
        else if ((ichar >= int0)&&(ichar <= int9))
            value[currBytePos++] = ichar - int0 + 52;
        else if (c[i] == '+')
            value[currBytePos++] = 62;
        else if (c[i] == '/')
            value[currBytePos++] = 63;
        else if (IS_WHITESPACE(c[i]))
            continue;
        else if (c[i] == '=') {
            // throw for bad inputs like
            // ====, a===, ab=c
            // valid inputs are ab==,abc=
            int temp = (currBytePos - offset) % 4;
            if ( ( temp == 3 ) || ( (temp == 2) && ( i + 1 < length + offset ) && ( c[i+1] == '=' ) ) ) {
                value[currBytePos++] = 0;
                iend++;
            }
            else {
                //We may have whitespace in the trailing characters, so take a slightly more expensive path
                //to determine this.
                //This presupposes that these characters can only occur at the end of the string.  Verify this assumption.
                bool foundEquals=false;
                for (UINT j = i+1; j<(length+offset); j++) {
                    if (IS_WHITESPACE(c[j])) {
                        continue;
                    }
                    else if (c[j]=='=') {
                        if (foundEquals)
                            COMPlusThrow(kFormatException, L"Format_BadBase64Char");

                        foundEquals=true;
                    }
                    else {
                        COMPlusThrow(kFormatException, L"Format_BadBase64Char");
                    }
                }
                value[currBytePos++] = 0;
                iend++;
            }

            // We are done looking at a group of 4, only valid characters after this are whitespaces
            if ((currBytePos % 4) == 0) {
                for (UINT j = i+1; j<(length+offset); j++) {
                    if (IS_WHITESPACE(c[j])) {
                        continue;
                    }
                    else {
                        COMPlusThrow(kFormatException, L"Format_BadBase64Char");
                    }
                }
            }
        }
        else
            COMPlusThrow(kFormatException, L"Format_BadBase64Char");
    }

    *nonWhiteSpaceChars = currBytePos;
    return iend;
}

//Walk the byte array and convert the int's into bytes in the proper base-64 notation.
INT32 BitConverter::ConvertByteArrayToByteStream(INT32 *value, __out_ecount(length) U1 *b,UINT length)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(value));
        PRECONDITION(CheckPointer(b));
    }
    CONTRACTL_END;

    int j = 0;
    int b1;
    int b2;
    int b3;

    //Walk the byte array and convert the int's into bytes in the proper base-64 notation.
    for (UINT i=0; i<(length); i+=3) {
        b1 = (UINT8)((value[j]<<2)&0xfc);
        b1 = (UINT8)(b1|((value[j+1]>>4)&0x03));
        b2 = (UINT8)((value[j+1]<<4)&0xf0);
        b2 = (UINT8)(b2|((value[j+2]>>2)&0x0f));
        b3 = (UINT8)((value[j+2]<<6)&0xc0);
        b3 = (UINT8)(b3|(value[j+3]));
        j+=4;
        b[i] = (UINT8)b1;
        if ((i+1)<length)
            b[i+1] = (UINT8)b2;
        if ((i+2)<length)
            b[i+2] = (UINT8)b3;
    }
    return j;
}

// BlockCopy
// This method from one primitive array to another based
//  upon an offset into each an a byte count.
FCIMPL5(VOID, Buffer::BlockCopy, ArrayBase *src, int srcOffset, ArrayBase *dst, int dstOffset, int count)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);    // GC_TRIGGERS is not allowed in FCall yet.
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // Verify that both the src and dst are Arrays of primitive
    //  types.
    if (src==NULL || dst==NULL)
        FCThrowArgumentNullVoid((src==NULL) ? L"src" : L"dst");

    // Size of the Arrays in bytes
    int srcLen = src->GetNumComponents() * src->GetMethodTable()->GetComponentSize();
    int dstLen = srcLen;

    // We only want to allow arrays of primitives, no Objects.
    const CorElementType srcET = src->GetArrayClass()->GetArrayElementType();
    if (!CorTypeInfo::IsPrimitiveType(srcET) || ELEMENT_TYPE_STRING == srcET)
        FCThrowArgumentVoid(L"src", L"Arg_MustBePrimArray");

    if (src != dst) {
        const CorElementType dstET = dst->GetArrayClass()->GetArrayElementType();
        if (!CorTypeInfo::IsPrimitiveType(dstET) || ELEMENT_TYPE_STRING == dstET)
            FCThrowArgumentVoid(L"dest", L"Arg_MustBePrimArray");
        dstLen = dst->GetNumComponents() * dst->GetMethodTable()->GetComponentSize();
    }

    if (srcOffset < 0 || dstOffset < 0 || count < 0) {
        const wchar_t* str = L"srcOffset";
        if (dstOffset < 0) str = L"dstOffset";
        if (count < 0) str = L"count";
        FCThrowArgumentOutOfRangeVoid(str, L"ArgumentOutOfRange_NeedNonNegNum");
    }

    if (srcLen - srcOffset < count || dstLen - dstOffset < count) {
        FCThrowArgumentVoid(NULL, L"Argument_InvalidOffLen");
    }

    if (count > 0) {
        // Call our faster version of memmove, not the CRT one.
        m_memmove(dst->GetDataPtr() + dstOffset, src->GetDataPtr() + srcOffset, count);
    }

    FC_GC_POLL();
}
FCIMPLEND


// InternalBlockCopy
// This method from one primitive array to another based
//  upon an offset into each an a byte count.
FCIMPL5(VOID, Buffer::InternalBlockCopy, ArrayBase *src, int srcOffset, ArrayBase *dst, int dstOffset, int count)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);    // GC_TRIGGERS is not allowed in FCall yet.
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // @TODO: We should consider writing this in managed code.  We probably
    // cannot easily do this though - how do we get at the array's data?

    // Unfortunately, we must do a check to make sure we're writing within
    // the bounds of the array.  This will ensure that we don't overwrite
    // memory elsewhere in the system nor do we write out junk.  This can
    // happen if multiple threads screw with our IO classes simultaneously
    // without being threadsafe.  Throw here.                                                
    // Unfortunately this even applies to setting our internal buffers to
    // null.  We don't want to debug races between Close and Read or Write.
    if (src == NULL || dst == NULL)
        FCThrowResVoid(kIndexOutOfRangeException, L"IndexOutOfRange_IORaceCondition");

    int srcLen = src->GetNumComponents() * src->GetMethodTable()->GetComponentSize();
    if (srcOffset < 0 || dstOffset < 0 || count < 0 || srcOffset > srcLen - count)
        FCThrowResVoid(kIndexOutOfRangeException, L"IndexOutOfRange_IORaceCondition");
    int destLen = srcLen;
    if (src != dst)
        destLen = dst->GetNumComponents() * dst->GetMethodTable()->GetComponentSize();
    if (dstOffset > destLen - count)
        FCThrowResVoid(kIndexOutOfRangeException, L"IndexOutOfRange_IORaceCondition");


    _ASSERTE(srcOffset >= 0);
    _ASSERTE((src->GetNumComponents() * src->GetMethodTable()->GetComponentSize()) - (unsigned) srcOffset >= (unsigned) count);
    _ASSERTE((dst->GetNumComponents() * dst->GetMethodTable()->GetComponentSize()) - (unsigned) dstOffset >= (unsigned) count);
    _ASSERTE(dstOffset >= 0);
    _ASSERTE(count >= 0);

    // Copy the data.
    // Call our faster version of memmove, not the CRT one.
    m_memmove(dst->GetDataPtr() + dstOffset, src->GetDataPtr() + srcOffset, count);

    FC_GC_POLL();
}
FCIMPLEND


// Gets a particular byte out of the array.  The array can't be an array of Objects - it
// must be a primitive array.
FCIMPL2(FC_UINT8_RET, Buffer::GetByte, ArrayBase* arrayUNSAFE, INT32 index)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    BYTE            RetVal = 0;
    BASEARRAYREF    array = (BASEARRAYREF) arrayUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_1(array);

    if (array == NULL)
        COMPlusThrowArgumentNull(L"array");

    TypeHandle elementTH = array->GetArrayElementTypeHandle();

    if (!CorTypeInfo::IsPrimitiveType(elementTH.GetVerifierCorElementType()))
        COMPlusThrow(kArgumentException, L"Arg_MustBePrimArray");

    const int elementSize = elementTH.GetMethodTable()->GetNumInstanceFieldBytes();
    _ASSERTE(elementSize > 0);

    if (index < 0 || index >= (int)array->GetNumComponents()*elementSize)
        COMPlusThrowArgumentOutOfRange(L"index", L"ArgumentOutOfRange_Index");

    RetVal = *((BYTE*)array->GetDataPtr() + index);

    HELPER_METHOD_FRAME_END();
    return RetVal;
}
FCIMPLEND

// Sets a particular byte in an array.  The array can't be an array of Objects - it
// must be a primitive array.
FCIMPL3(void, Buffer::SetByte, ArrayBase* arrayUNSAFE, INT32 index, UINT8 value)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    BASEARRAYREF array = (BASEARRAYREF) arrayUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_1(array);

    if (array == NULL)
        COMPlusThrowArgumentNull(L"array");

    TypeHandle elementTH = array->GetArrayElementTypeHandle();

    if (!CorTypeInfo::IsPrimitiveType(elementTH.GetVerifierCorElementType()))
        COMPlusThrow(kArgumentException, L"Arg_MustBePrimArray");

    const int elementSize = elementTH.GetMethodTable()->GetNumInstanceFieldBytes();
    _ASSERTE(elementSize > 0);

    if (index < 0 || index >= (int)array->GetNumComponents()*elementSize)
        COMPlusThrowArgumentOutOfRange(L"index", L"ArgumentOutOfRange_Index");

    *((BYTE*)array->GetDataPtr() + index) = value;

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


// Finds the length of an array in bytes.  Must be a primitive array.
FCIMPL1(INT32, Buffer::ByteLength, ArrayBase* arrayUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    INT32           iRetVal = 0;
    BASEARRAYREF    array   = (BASEARRAYREF) arrayUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_1(array);

    if (array == NULL)
        COMPlusThrowArgumentNull(L"array");

    TypeHandle elementTH = array->GetArrayElementTypeHandle();

    if (!CorTypeInfo::IsPrimitiveType(elementTH.GetVerifierCorElementType()))
        COMPlusThrow(kArgumentException, L"Arg_MustBePrimArray");

    const int elementSize = elementTH.GetMethodTable()->GetNumInstanceFieldBytes();
    _ASSERTE(elementSize > 0);

    iRetVal = array->GetNumComponents() * elementSize;

    HELPER_METHOD_FRAME_END();
    return iRetVal;
}
FCIMPLEND

//
// GCInterface
//
BOOL GCInterface::m_cacheCleanupRequired=FALSE;
MethodDesc *GCInterface::m_pCacheMethod=NULL;

UINT64   GCInterface::m_ulMemPressure = 0;
UINT64   GCInterface::m_ulThreshold = MIN_GC_MEMORYPRESSURE_THRESHOLD;
INT32    GCInterface::m_gc_gen = 0; 
INT32    GCInterface::m_gc_counts[3] = {0,0,0};
INT32    GCInterface::m_induced_gc_counts[3] = {0,0,0};
CrstStatic GCInterface::m_MemoryPressureLock;

/*============================IsCacheCleanupRequired============================
**Action: Called by Thread::HaveExtraWorkForFinalizer to determine if we have
**        managed caches which should be cleared as a part of the finalizer thread
**        finishing it's work.
**Returns: BOOL.  True if the cache needs to be cleared.
**Arguments: None
**Exceptions: None
==============================================================================*/
BOOL GCInterface::IsCacheCleanupRequired()
{
    LEAF_CONTRACT;
    return m_cacheCleanupRequired;
}

/*===========================SetCacheCleanupRequired============================
**Action: Sets the bit as to whether cache cleanup is required.
**Returns: void
**Arguments: None
**Exceptions: None
==============================================================================*/
void GCInterface::SetCacheCleanupRequired(BOOL bCleanup)
{
    LEAF_CONTRACT;
    m_cacheCleanupRequired = bCleanup;
}


/*=================================CleanupCache=================================
**Action: Call the managed code in GC.FireCacheEvent to tell all of the managed
**        caches to go clean themselves up.
**Returns:    Void
**Arguments:  None
**Exceptions: We don't care if exceptions happen.  We'll trap, log, and
**            discard them.  We will rethrow terminals.
==============================================================================*/
void GCInterface::CleanupCache()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    //Let's set the bit to false.  This means that if any cache gets
    //created while we're clearing caches, it will set the bit again
    //and we'll remember to go clean it up.
    SetCacheCleanupRequired(FALSE);

    //The EE shouldn't enter shutdown phase while the finalizer thread is active.
    //If this isn't true, I'll need some more complicated logic here.
    if (g_fEEShutDown)
        return;

    EX_TRY
    {
        //If we don't have the method already, let's try to go get it.
        if (!m_pCacheMethod) {
            m_pCacheMethod = g_Mscorlib.GetMethod(METHOD__GC__FIRE_CACHE_EVENT);
            _ASSERTE(m_pCacheMethod);
        }

        //If we have the method let's call it and catch any errors.  We don't do anything
        //other than log these because we don't care.  If the cache clear fails, then either
        //we're shutting down or the failure will be propped up to user code the next
        //time that they try to access the cache.
        if (m_pCacheMethod) {
            MethodDescCallSite fireCacheEvent(m_pCacheMethod);
            //Static method has no arguments;
            fireCacheEvent.Call((ARG_SLOT*)NULL);
            LOG((LF_BCL, LL_INFO10, "Called cache cleanup method."));
        }
        else {
            LOG((LF_BCL, LL_INFO10, "Unable to get MethodDesc for cleanup"));
        }

    }
    EX_CATCH
    {
        if (!m_pCacheMethod) {
            LOG((LF_BCL, LL_INFO10, "Caught an exception while trying to get the MethodDesc"));
        }
        else {
            LOG((LF_BCL, LL_INFO10, "Got an exception while calling cache method"));
        }
    }
    EX_END_CATCH(RethrowTerminalExceptions);
}


/*============================NativeSetCleanupCache=============================
**Action: Sets the bit to say to clear the cache.  This is merely the wrapper
**        for managed code to call.
**Returns: void
**Arguments: None
**Exceptions: None
==============================================================================*/
FCIMPL0(void, GCInterface::NativeSetCleanupCache)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    SetCacheCleanupRequired(TRUE);
}
FCIMPLEND

/*================================GetGeneration=================================
**Action: Returns the generation in which args->obj is found.
**Returns: The generation in which args->obj is found.
**Arguments: args->obj -- The object to locate.
**Exceptions: ArgumentException if args->obj is null.
==============================================================================*/
FCIMPL1(int, GCInterface::GetGeneration, Object* objUNSAFE)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (objUNSAFE == NULL)
        FCThrowArgumentNull(L"obj");

    int result = (INT32)GCHeap::GetGCHeap()->WhichGeneration(objUNSAFE);
    FC_GC_POLL_RET();
    return result;
}
FCIMPLEND

/*================================CollectionCount=================================
**Action: Returns the number of collections for this generation since the begining of the life of the process
**Returns: The collection count.
**Arguments: args->generation -- The generation
**Exceptions: Argument exception if args->generation is < 0 or > GetMaxGeneration();
==============================================================================*/
FCIMPL1(int, GCInterface::CollectionCount, INT32 generation)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    //We've already checked this in GC.cs, so we'll just assert it here.
    _ASSERTE(generation >= 0);

    //We don't need to check the top end because the GC will take care of that.
    int result = (INT32)GCHeap::GetGCHeap()->CollectionCount(generation);
    FC_GC_POLL_RET();
    return result;
}
FCIMPLEND

// This method is called by the GetMethod function and will crawl backward
//  up the stack for integer methods.
StackWalkAction GCInterface::SkipMethods(CrawlFrame* frame, VOID* data)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(frame));
        PRECONDITION(CheckPointer(data));
    }
    CONTRACTL_END;

    SkipStruct* pSkip = (SkipStruct*) data;

    MethodDesc *pFunc = frame->GetFunction();

    /* We asked to be called back only for functions */
    _ASSERTE(pFunc);

    // First check if the walk has skipped the required frames. The check
    // here is between the address of a local variable (the stack mark) and a
    // pointer to the EIP for a frame (which is actually the pointer to the
    // return address to the function from the previous frame). So we'll
    // actually notice which frame the stack mark was in one frame later. This
    // is fine for our purposes since we're always looking for the frame of the
    // caller of the method that actually created the stack mark.
    _ASSERTE((pSkip->stackMark == NULL) || (*pSkip->stackMark == LookForMyCaller));

    if ((pSkip->stackMark != NULL) && !IsInCalleesFrames(frame->GetRegisterSet(), pSkip->stackMark))
        return SWA_CONTINUE;

    pSkip->pMeth = static_cast<MethodDesc*>(pFunc);

    return SWA_ABORT;
}


/*==================================KeepAlive===================================
**Action: A helper to extend the lifetime of an object to this call.  Note
**        that calling this method forces a reference to the object to remain
**        valid until this call happens, preventing some destructive premature
**        finalization problems.
==============================================================================*/
FCIMPL1 (void, GCInterface::KeepAlive, Object *obj)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    FCUnique(0x83);

    FC_GC_POLL();
}
FCIMPLEND

/*===============================GetGenerationWR================================
**Action: Returns the generation in which the object pointed to by a WeakReference is found.
**Returns:
**Arguments: args->handle -- the OBJECTHANDLE to the object which we're locating.
**Exceptions: ArgumentException if handle points to an object which is not accessible.
==============================================================================*/
FCIMPL1(int, GCInterface::GetGenerationWR, LPVOID handle)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    int iRetVal = 0;

    HELPER_METHOD_FRAME_BEGIN_RET_0();

    OBJECTREF temp;
    temp = ObjectFromHandle((OBJECTHANDLE) handle);
    if (temp == NULL)
        COMPlusThrowArgumentNull(L"weak handle");

    iRetVal = (INT32)GCHeap::GetGCHeap()->WhichGeneration(OBJECTREFToObject(temp));

    HELPER_METHOD_FRAME_END();

    return iRetVal;
}
FCIMPLEND


/*================================GetTotalMemory================================
**Action: Returns the total number of bytes in use
**Returns: The total number of bytes in use
**Arguments: None
**Exceptions: None
==============================================================================*/
FCIMPL0(INT64, GCInterface::GetTotalMemory)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    INT64 iRetVal = 0;
    HELPER_METHOD_FRAME_BEGIN_RET_0();

    iRetVal = (INT64) GCHeap::GetGCHeap()->GetTotalBytesInUse();
    Thread *pThread = GetThread();
    if (pThread->CatchAtSafePoint())
    {
        CommonTripThread();
    }
    HELPER_METHOD_FRAME_END();
    return iRetVal;
}
FCIMPLEND

/*==============================CollectGeneration===============================
**Action: Collects all generations <= args->generation
**Returns: void
**Arguments: args->generation:  The maximum generation to collect
**Exceptions: Argument exception if args->generation is < 0 or > GetMaxGeneration();
==============================================================================*/
FCIMPL1(void, GCInterface::CollectGeneration, INT32 generation)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    //We've already checked this in GC.cs, so we'll just assert it here.
    _ASSERTE(generation >= -1);

    //We don't need to check the top end because the GC will take care of that.
    HELPER_METHOD_FRAME_BEGIN_0();

    GCHeap::GetGCHeap()->GarbageCollect(generation);

    if (g_TrapReturningThreads)
    {
        GetThread()->PulseGCMode();
    }
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


/*===============================GetMaxGeneration===============================
**Action: Returns the largest GC generation
**Returns: The largest GC Generation
**Arguments: None
**Exceptions: None
==============================================================================*/
FCIMPL0(int, GCInterface::GetMaxGeneration)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return(INT32)GCHeap::GetGCHeap()->GetMaxGeneration();
}
FCIMPLEND


/*================================RunFinalizers=================================
**Action: Run all Finalizers that haven't been run.
**Arguments: None
**Exceptions: None
==============================================================================*/
FCIMPL0(void, GCInterface::RunFinalizers)
{
    STATIC_CONTRACT_SO_TOLERANT;
    HELPER_METHOD_FRAME_BEGIN_0();

    TRIGGERSGC();

    GCHeap::GetGCHeap()->FinalizerThreadWait();

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


/*==============================SuppressFinalize================================
**Action: Indicate that an object's finalizer should not be run by the system
**Arguments: Object of interest
**Exceptions: None
==============================================================================*/
FCIMPL1(void, GCInterface::FCSuppressFinalize, Object *obj)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (obj == 0)
        FCThrowVoid(kArgumentNullException);

    if (!obj->GetMethodTable ()->HasFinalizer())
        return;

    GCHeap::GetGCHeap()->SetFinalizationRun(obj);
    FC_GC_POLL();
}
FCIMPLEND


/*============================ReRegisterForFinalize==============================
**Action: Indicate that an object's finalizer should be run by the system.
**Arguments: Object of interest
**Exceptions: None
==============================================================================*/
FCIMPL1(void, GCInterface::FCReRegisterForFinalize, Object *obj)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    if (obj == 0)
        FCThrowVoid(kArgumentNullException);

    BEGIN_SO_INTOLERANT_CODE(GetThread());
    if (obj->GetMethodTable()->HasFinalizer())
    {
        HELPER_METHOD_FRAME_BEGIN_1(obj);
        GCHeap::GetGCHeap()->RegisterForFinalization(-1, obj);
        HELPER_METHOD_FRAME_END();
    }
    END_SO_INTOLERANT_CODE;
}
FCIMPLEND

UINT64 GCInterface::InterlockedAdd (UINT64 addend) {
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    UINT64 oldMemValue;
    UINT64 newMemValue;

    do {
        oldMemValue = m_ulMemPressure;
        newMemValue = oldMemValue + addend;

        // check for overflow
        if (newMemValue < oldMemValue)
        {
            newMemValue = UINT64_MAX;
        }
    } while (InterlockedCompareExchange64((LONGLONG*) &m_ulMemPressure, (LONGLONG) newMemValue, (LONGLONG) oldMemValue) != (LONGLONG) oldMemValue);

    return newMemValue;
}

UINT64 GCInterface::InterlockedSub(UINT64 subtrahend) {
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    UINT64 oldMemValue;
    UINT64 newMemValue;

    do {
        oldMemValue = m_ulMemPressure;
        newMemValue = oldMemValue - subtrahend;

        // check for underflow
        if (newMemValue > oldMemValue)
            newMemValue = 0;
        
    } while (InterlockedCompareExchange64((LONGLONG*) &m_ulMemPressure, (LONGLONG) newMemValue, (LONGLONG) oldMemValue) != (LONGLONG) oldMemValue);

    return newMemValue;
}

FCIMPL1_V(void, GCInterface::NativeAddMemoryPressure, UINT64 bytesAllocated) 
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    // AddMemoryPressure could cause a GC, so we need a frame 
    HELPER_METHOD_FRAME_BEGIN_0()
    AddMemoryPressure(bytesAllocated);
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

void GCInterface::AddMemoryPressure(UINT64 bytesAllocated) {
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;
    
    GCX_PREEMP();
    INT32 gen_collect = -1;               
        
    UINT64 newMemValue = InterlockedAdd(bytesAllocated);

    if (newMemValue > m_ulThreshold) {
        CrstPreempHolder holder(&m_MemoryPressureLock);

        // to avoid collecting too often, take the max threshold of the linear and geometric growth 
        // heuristics.          
        UINT64 addMethod;
        UINT64 multMethod;
	UINT64 bytesAllocatedMax = (UINT64_MAX - m_ulThreshold) / 8;

        if (bytesAllocated >= bytesAllocatedMax) // overflow check
        {
            addMethod = UINT64_MAX;
        }
        else
        {
            addMethod = m_ulThreshold + bytesAllocated * 8;
        }

        multMethod = newMemValue + newMemValue / 10;
        if (multMethod < newMemValue) // overflow check
        {
            multMethod = UINT64_MAX;
        }

        m_ulThreshold = (addMethod > multMethod) ? addMethod : multMethod;
        gen_collect = m_gc_gen;
        if ((m_gc_gen < 2) &&
             (m_induced_gc_counts[m_gc_gen] >= (m_induced_gc_counts[m_gc_gen+1]*4)))
        {
            m_gc_gen++;
        }
    }

    if ((gen_collect >= 0) && ((gen_collect == 0) || (m_gc_counts[gen_collect] == GCHeap::GetGCHeap()->CollectionCount(gen_collect)))) {
        for(int j = 0; j < gen_collect; j++)
        {
            m_induced_gc_counts[j]++;
        }
        GCX_COOP();
        GCHeap::GetGCHeap()->GarbageCollect(gen_collect);
        if (g_TrapReturningThreads)
        {
            GetThread()->PulseGCMode();
        }
    }
    
    //don't bother with gen0. 
    for (int i = 1; i < 3; i++) {
        m_gc_counts [i] = GCHeap::GetGCHeap()->CollectionCount(i);
    }
}


FCIMPL1_V(void, GCInterface::NativeRemoveMemoryPressure, UINT64 bytesAllocated) 
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    HELPER_METHOD_FRAME_BEGIN_0()
    RemoveMemoryPressure(bytesAllocated);
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

void GCInterface::RemoveMemoryPressure(UINT64 bytesAllocated) {
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;    
    }
    CONTRACTL_END;

    GCX_PREEMP();
    
    UINT64 newMemValue = InterlockedSub(bytesAllocated);
    UINT64 new_th;  
    UINT64 bytesAllocatedMax = (m_ulThreshold / 4);
    UINT64 addMethod;
    UINT64 multMethod = (m_ulThreshold - m_ulThreshold / 20); // can never underflow
    if (bytesAllocated >= bytesAllocatedMax) // protect against underflow
    {
        addMethod = 0;
    }
    else
    {
        addMethod = m_ulThreshold - bytesAllocated * 4;
    }

    new_th = (addMethod < multMethod) ? addMethod : multMethod;
 
    if (newMemValue <= new_th) {
        CrstPreempHolder holder(&m_MemoryPressureLock);
        if (new_th > MIN_GC_MEMORYPRESSURE_THRESHOLD)
            m_ulThreshold = new_th;
        else
            m_ulThreshold = MIN_GC_MEMORYPRESSURE_THRESHOLD;
        m_gc_gen = 0;
    }

    //should this be done inside of the threshold code?
    for (int i = 1; i < 3; i++) {
        m_gc_counts[i] = GCHeap::GetGCHeap()->CollectionCount(i);
    }
}

//
// COMInterlocked
//

FCIMPL1(INT32,COMInterlocked::Increment32, INT32 *location)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    if( NULL == location) {
        FCThrow(kNullReferenceException);
    }

    return FastInterlockIncrement((LONG *) location);
}
FCIMPLEND

FCIMPL1(INT32,COMInterlocked::Decrement32, INT32 *location)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    if( NULL == location) {
        FCThrow(kNullReferenceException);
    }

    return FastInterlockDecrement((LONG *) location);
}
FCIMPLEND

FCIMPL1(INT64,COMInterlocked::Increment64, INT64 *location)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    if( NULL == location) {
        FCThrow(kNullReferenceException);
    }

    return FastInterlockIncrementLong((UINT64 *) location);
}
FCIMPLEND

FCIMPL1(INT64,COMInterlocked::Decrement64, INT64 *location)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    if( NULL == location) {
        FCThrow(kNullReferenceException);
    }

    return FastInterlockDecrementLong((UINT64 *) location);
}
FCIMPLEND

FCIMPL2(INT32,COMInterlocked::Exchange, INT32 *location, INT32 value)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    if( NULL == location) {
        FCThrow(kNullReferenceException);
    }

    return FastInterlockExchange((LONG *) location, value);
}
FCIMPLEND

FCIMPL2_IV(INT64,COMInterlocked::Exchange64, INT64 *location, INT64 value)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    if( NULL == location) {
        FCThrow(kNullReferenceException);
    }

    return FastInterlockExchangeLong((INT64 *) location, value);
}
FCIMPLEND

FCIMPL2(LPVOID,COMInterlocked::ExchangePointer, LPVOID *location, LPVOID value)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    if( NULL == location) {
        FCThrow(kNullReferenceException);
    }

    FCUnique(0x15);
    return FastInterlockExchangePointer(location, value);
}
FCIMPLEND

FCIMPL3(INT32, COMInterlocked::CompareExchange, INT32* location, INT32 value, INT32 comparand)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    if( NULL == location) {
        FCThrow(kNullReferenceException);
    }

    return FastInterlockCompareExchange((LONG*)location, value, comparand);
}
FCIMPLEND

FCIMPL3_IVV(INT64, COMInterlocked::CompareExchange64, INT64* location, INT64 value, INT64 comparand)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    if( NULL == location) {
        FCThrow(kNullReferenceException);
    }

    return FastInterlockCompareExchangeLong((INT64*)location, value, comparand);
}
FCIMPLEND

FCIMPL3(LPVOID,COMInterlocked::CompareExchangePointer, LPVOID *location, LPVOID value, LPVOID comparand)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    if( NULL == location) {
        FCThrow(kNullReferenceException);
    }

    FCUnique(0x59);
    return FastInterlockCompareExchangePointer(location, value, comparand);
}
FCIMPLEND

FCIMPL2_IV(float,COMInterlocked::ExchangeFloat, float *location, float value)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    if( NULL == location) {
        FCThrow(kNullReferenceException);
    }


    LONG ret = FastInterlockExchange((LONG *) location, *(LONG*)&value);
    return *(float*)&ret;
}
FCIMPLEND

FCIMPL2_IV(double,COMInterlocked::ExchangeDouble, double *location, double value)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    if( NULL == location) {
        FCThrow(kNullReferenceException);
    }


    INT64 ret = FastInterlockExchangeLong((INT64 *) location, *(INT64*)&value);
    return *(double*)&ret;
}
FCIMPLEND

FCIMPL3_IVV(float,COMInterlocked::CompareExchangeFloat, float *location, float value, float comparand)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    if( NULL == location) {
        FCThrow(kNullReferenceException);
    }

    LONG ret = (LONG)FastInterlockCompareExchange((LONG*) location, *(LONG*)&value, *(LONG*)&comparand);
    return *(float*)&ret;
}
FCIMPLEND

FCIMPL3_IVV(double,COMInterlocked::CompareExchangeDouble, double *location, double value, double comparand)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    if( NULL == location) {
        FCThrow(kNullReferenceException);
    }

    INT64 ret = (INT64)FastInterlockCompareExchangeLong((INT64*) location, *(INT64*)&value, *(INT64*)&comparand);
    return *(double*)&ret;
}
FCIMPLEND

FCIMPL2(LPVOID,COMInterlocked::ExchangeObject, LPVOID*location, LPVOID value)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    if( NULL == location) {
        FCThrow(kNullReferenceException);
    }

    LPVOID ret = FastInterlockExchangePointer(location, value);
#ifdef _DEBUG
    Thread::ObjectRefAssign((OBJECTREF *)location);
#endif
    ErectWriteBarrier((OBJECTREF*) location, ObjectToOBJECTREF((Object*) value));
    return ret;
}
FCIMPLEND

FCIMPL2_VV(void,COMInterlocked::ExchangeGeneric, TypedByRef location, TypedByRef value)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    LPVOID* loc = (LPVOID*)location.data;
    if( NULL == loc) {
        FCThrowVoid(kNullReferenceException);
    }

    LPVOID val = *(LPVOID*)value.data;
    *(LPVOID*)value.data = FastInterlockExchangePointer(loc, val);
#ifdef _DEBUG
    Thread::ObjectRefAssign((OBJECTREF *)loc);
#endif
    ErectWriteBarrier((OBJECTREF*) loc, ObjectToOBJECTREF((Object*) val));
}
FCIMPLEND

FCIMPL3_VVI(void,COMInterlocked::CompareExchangeGeneric, TypedByRef location, TypedByRef value, LPVOID comparand)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    LPVOID* loc = (LPVOID*)location.data;
    LPVOID val = *(LPVOID*)value.data;
    if( NULL == loc) {
        FCThrowVoid(kNullReferenceException);
    }

    LPVOID ret = FastInterlockCompareExchangePointer(loc, val, comparand);
    *(LPVOID*)value.data = ret;
    if(ret == comparand)
    {
#ifdef _DEBUG
        Thread::ObjectRefAssign((OBJECTREF *)loc);
#endif
        ErectWriteBarrier((OBJECTREF*) loc, ObjectToOBJECTREF((Object*) val));
    }
}
FCIMPLEND


FCIMPL3(LPVOID,COMInterlocked::CompareExchangeObject, LPVOID *location, LPVOID value, LPVOID comparand)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    if( NULL == location) {
        FCThrow(kNullReferenceException);
    }

    LPVOID ret = FastInterlockCompareExchangePointer(location, value, comparand);
    if (ret == comparand) {
#ifdef _DEBUG
        Thread::ObjectRefAssign((OBJECTREF *)location);
#endif
        ErectWriteBarrier((OBJECTREF*) location, ObjectToOBJECTREF((Object*) value));
    }
    return ret;
}
FCIMPLEND

FCIMPL2(INT32,COMInterlocked::ExchangeAdd32, INT32 *location, INT32 value)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    if( NULL == location) {
        FCThrow(kNullReferenceException);
    }

    return FastInterlockExchangeAdd((LONG *) location, value);
}
FCIMPLEND

FCIMPL2_IV(INT64,COMInterlocked::ExchangeAdd64, INT64 *location, INT64 value)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    if( NULL == location) {
        FCThrow(kNullReferenceException);
    }

    return FastInterlockExchangeAddLong((INT64 *) location, value);
}
FCIMPLEND


FCIMPL6(INT32, ManagedLoggingHelper::GetRegistryLoggingValues, CLR_BOOL* bLoggingEnabled, CLR_BOOL* bLogToConsole, INT32 *iLogLevel, CLR_BOOL* bPerfWarnings, CLR_BOOL* bCorrectnessWarnings, CLR_BOOL* bSafeHandleStackTraces)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);    // GC_TRIGGERS is not allowed in FCall yet.
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    INT32 logFacility = 0;

    HELPER_METHOD_FRAME_BEGIN_RET_0();

    *bLoggingEnabled         = (bool)(g_pConfig->GetConfigDWORD(MANAGED_LOGGING_ENABLE, 0)!=0);
    *bLogToConsole           = (bool)(g_pConfig->GetConfigDWORD(MANAGED_LOGGING_CONSOLE, 0)!=0);
    *iLogLevel               = (INT32)(g_pConfig->GetConfigDWORD(MANAGED_LOGGING_LEVEL, 0));
    logFacility              = (INT32)(g_pConfig->GetConfigDWORD(MANAGED_LOGGING_FACILITY, 0));
    *bPerfWarnings           = (bool)(g_pConfig->GetConfigDWORD(MANAGED_PERF_WARNINGS, 0)!=0);
    *bCorrectnessWarnings    = (bool)(g_pConfig->GetConfigDWORD(MANAGED_CORRECTNESS_WARNINGS, 0)!=0);
    *bSafeHandleStackTraces  = (bool)(g_pConfig->GetConfigDWORD(MANAGED_SAFEHANDLE_STACKTRACES, 0)!=0);

    HELPER_METHOD_FRAME_END();                              \

    return logFacility;
}
FCIMPLEND

// Return true if the valuetype does not contain pointer and is tightly packed
FCIMPL1(FC_BOOL_RET, ValueTypeHelper::CanCompareBits, Object* obj)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    _ASSERTE(obj != NULL);
    MethodTable* mt = obj->GetMethodTable();
    FC_RETURN_BOOL(!mt->ContainsPointers() && !mt->IsNotTightlyPacked());
}
FCIMPLEND

FCIMPL2(FC_BOOL_RET, ValueTypeHelper::FastEqualsCheck, Object* obj1, Object* obj2)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    _ASSERTE(obj1 != NULL);
    _ASSERTE(obj2 != NULL);
    _ASSERTE(!obj1->GetMethodTable()->ContainsPointers());
    _ASSERTE(obj1->GetSize() == obj2->GetSize());

    TypeHandle pTh = obj1->GetTypeHandle();

    FC_RETURN_BOOL(memcmp(obj1->GetData(),obj2->GetData(),pTh.GetSize()) == 0);
}
FCIMPLEND

static BOOL CanUseFastGetHashCodeHelper(MethodTable *mt)
{
    return !mt->ContainsPointers() && !mt->IsNotTightlyPacked();
}

static INT32 FastGetValueTypeHashCodeHelper(MethodTable *mt, void *pObjRef)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CanUseFastGetHashCodeHelper(mt));
    } CONTRACTL_END;

    INT32 hashCode = 0;
    INT32 *pObj = (INT32*)pObjRef;
            
    // this is a struct with no refs and no "strange" offsets, just go through the obj and xor the bits
    INT32 size = mt->GetNumInstanceFieldBytes();
    for (INT32 i = 0; i < (INT32)(size / sizeof(INT32)); i++)
        hashCode ^= *pObj++;

    return hashCode;
}

static INT32 RegularGetValueTypeHashCode(MethodTable *mt, void *pObjRef)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    INT32 hashCode = 0;
    INT32 *pObj = (INT32*)pObjRef;

    // While we shouln't get here directly from ValueTypeHelper::GetHashCode, if we recurse we need to 
    // be able to handle getting the hashcode for an embedded structure whose hashcode is computed by the fast path.
    if (CanUseFastGetHashCodeHelper(mt))
    {
        return FastGetValueTypeHashCodeHelper(mt, pObjRef);
    }
    else
    {
        // it's looking ugly so we'll use the old behavior in managed code. Grab the first non-null
        // field and return its hash code or 'it' as hash code
        //
        ApproxFieldDescIterator fdIterator(mt, ApproxFieldDescIterator::INSTANCE_FIELDS);
        INT32 count = (INT32)fdIterator.Count();

        if (count != 0)
        {
            for (INT32 i = 0; i < count; i++)
            {
                FieldDesc *field = fdIterator.Next();
                _ASSERTE(!field->IsRVA());
                void *pFieldValue = field->GetAddressNoThrowNoGC(pObj);
                if (field->IsObjRef())
                {
                    // if we get an object reference we get the hash code out of that
                    if (*(Object**)pFieldValue != NULL)
                    {

                        OBJECTREF fieldObjRef = ObjectToOBJECTREF(*(Object **) pFieldValue);
                        GCPROTECT_BEGIN(fieldObjRef);

                        MethodDescCallSite getHashCode(METHOD__OBJECT__GET_HASH_CODE, &fieldObjRef);

                        // Make the call.
                        ARG_SLOT arg[1] = {ObjToArgSlot(fieldObjRef)};
                        hashCode = getHashCode.Call_RetI4(arg);

                        GCPROTECT_END();
                    }
                    else
                    {
                        // null object reference, try next
                        continue;
                    }
                }
                else
                {
                    UINT fieldSize = field->LoadSize();
                    INT32 *pValue = (INT32*)pFieldValue;
                    CorElementType fieldType = field->GetFieldType();
                    if (fieldType != ELEMENT_TYPE_VALUETYPE)
                    {
                        for (INT32 j = 0; j < (INT32)(fieldSize / sizeof(INT32)); j++)
                            hashCode ^= *pValue++;
                    }
                    else
                    {
                        // got another value type. Get the type
                        TypeHandle fieldTH = field->LookupFieldTypeHandle(); // the type was loaded already
                        _ASSERTE(!fieldTH.IsNull());
                        hashCode = RegularGetValueTypeHashCode(fieldTH.GetMethodTable(), pValue);
                    }
                }
                break;
            }
        }
    }
    return hashCode;
}

FCIMPL1(INT32, ValueTypeHelper::GetHashCode, Object* objUNSAFE)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    if (objUNSAFE == NULL)
        FCThrow(kNullReferenceException);
    INT32 hashCode = 0;
    MethodTable *pMT = objUNSAFE->GetMethodTable();

    INT32 *pObj = (INT32*)objUNSAFE;
    for (INT32 i = 0; i < (INT32)(sizeof(MethodTable*) / sizeof(INT32)); i++)
        hashCode ^= *pObj++;
    
    if (CanUseFastGetHashCodeHelper(pMT))
    {
        hashCode ^= FastGetValueTypeHashCodeHelper(pMT, objUNSAFE->UnBox());
    }
    else
    {
        OBJECTREF obj = ObjectToOBJECTREF(objUNSAFE);
        VALIDATEOBJECTREF(obj);
        HELPER_METHOD_FRAME_BEGIN_RET_1(obj);        
        hashCode ^= RegularGetValueTypeHashCode(pMT, obj->UnBox());
        HELPER_METHOD_FRAME_END();
    }
    
    return hashCode;
}
FCIMPLEND

