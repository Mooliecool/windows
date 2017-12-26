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
//  Purpose:  This module implements the methods of the COMNlsInfo
//            class.  These methods are the helper functions for the
//            Locale class.
//
//  Date:     August 12, 1998
//
////////////////////////////////////////////////////////////////////////////

//
//  Include Files.
//
#include "common.h"
#include "object.h"
#include "excep.h"
#include "vars.hpp"
#include "comstring.h"
#include "interoputil.h"
#include "corhost.h"

#include <winnls.h>

#include "utilcode.h"
#include "frames.h"
#include "field.h"
#include "metasig.h"
#include "comnls.h"
#include "gcscan.h"
#include "comnlsinfo.h"
#include "nlstable.h"
#include "nativetextinfo.h"
#include "casingtable.h"        // class CasingTable
#include "globalizationassembly.h"
#include "sortingtablefile.h"
#include "sortingtable.h"
#include "memoryreport.h"


#include "unicodecattable.h"


//
//  Constant Declarations.
//
#ifndef COMPARE_OPTIONS_ORDINAL
#define COMPARE_OPTIONS_ORDINAL            0x40000000
#endif

#ifndef COMPARE_OPTIONS_IGNORECASE
#define COMPARE_OPTIONS_IGNORECASE            0x00000001
#endif

#define MAX_STRING_VALUE        512

// Language ID for Traditional Chinese (CHT)
#define LANGID_ZH_CHT           0x7c04
// Language ID for CHT (Taiwan)
#define LANGID_ZH_TW            0x0404
// Language ID for CHT (Hong-Kong)
#define LANGID_ZH_HK            0x0c04
#define REGION_NAME_0404 L"\x53f0\x7063"

CasingTable*    COMNlsInfo::m_pCasingTable       = NULL;
NativeTextInfo* COMNlsInfo::m_pInvariantTextInfo = NULL;

const WCHAR* CasingTableFileName = L"\\l_intl.nls";

const PlaneOffset NullPlaneOffset[16] = 
{
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, 
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}
};




//
// Normalization Implementation
//
#define NORMALIZATION_DLL       MAKEDLLNAME(L"normalization")
HMODULE COMNlsInfo::m_hNormalization = NULL;
PFN_NORMALIZATION_IS_NORMALIZED_STRING COMNlsInfo::m_pfnNormalizationIsNormalizedStringFunc = NULL;
PFN_NORMALIZATION_NORMALIZE_STRING COMNlsInfo::m_pfnNormalizationNormalizeStringFunc = NULL;
PFN_NORMALIZATION_INIT_NORMALIZATION COMNlsInfo::m_pfnNormalizationInitNormalizationFunc = NULL;

//
BOOL COMNlsInfo::InitializeNLS() {
    LEAF_CONTRACT;
    return TRUE; //Made a boolean in case we have further initialization in the future.
}


/*============================nativeCreateGlobalizationAssembly============================
**Action: Create NativeGlobalizationAssembly instance for the specified Assembly.
**Returns:
**  void.
**  The side effect is to allocate the NativeCompareInfo cache.
**Arguments:  None
**Exceptions: OutOfMemoryException if we run out of memory.
**
** LOCK CONTRACT: NEED PROCESS-WIDE LOCK
**
**NOTE NOTE: This is a synchronized operation.  The required synchronization is
**           provided by the fact that we only call this in the class initializer
**           for CompareInfo.  If this invariant ever changes, guarantee
**           synchronization.
==============================================================================*/

FCIMPL1(LPVOID, COMNlsInfo::nativeCreateGlobalizationAssembly, AssemblyBaseObject* pAssemblyRef)
{
    CONTRACTL
    {
        THROWS;
        INJECT_FAULT(COMPlusThrowOM());
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pAssemblyRef));
    } CONTRACTL_END;

    NativeGlobalizationAssembly* pNGA = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_0();

    Assembly *pAssembly = pAssemblyRef->GetAssembly();

    if ((pNGA = NativeGlobalizationAssembly::FindGlobalizationAssembly(pAssembly))==NULL)
    {
        // Get the native pointer to Assembly from the ASSEMBLYREF, and use the pointer
        // to construct NativeGlobalizationAssembly.

        NewHolder<NativeGlobalizationAssembly> pTemp (new NativeGlobalizationAssembly(pAssembly));

        // Always add the newly created NGA to the static linked list of NativeGlobalizationAssembly.
        // This step is necessary so that we can shut down the SortingTable correctly.
        pTemp.SuppressRelease();
        pNGA = pTemp;
        NativeGlobalizationAssembly::AddToList(pNGA);
    }
    HELPER_METHOD_FRAME_END();

    return pNGA;
}
FCIMPLEND

/*============================nativeCreateOpenFileMapping============================
**Action: Create or open a named memory file mapping.
**Returns: Pointer to named section, or NULL if failed
**Arguments:
**  StringObject*   inSectionName  - name of section to open/create
**  int             inBytesToAllocate - desired size of memory section in bytes
**                      We use the last 4 bytes (must be aligned, so only choose
**                      inBytesToAllocate in multiples of 4) to indicate if the
**                      section is set or not.  AFTER section is initialized, set
**                      those 4 bytes to non-0, otherwise you'll get get new
**                      heap memory all the time.
**  HANDLE*         mappedFile - is the handle of the memory mapped file. this is  
**                      out parameter.
**
** NOTE: We'll try to open the same object, so we can share names.  We don't lock
**       though, so 2 thread could get the same object, but thread 1 might not
**       have initialized it yet.
**
** NOTE: For NT you should add a Global\ to the beginning of the name if you
**       want to share it machine wide.
**
==============================================================================*/
FCIMPL3(LPVOID, COMNlsInfo::nativeCreateOpenFileMapping,
            StringObject* inSectionName, int inBytesToAllocate, HANDLE *mappedFile)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(inSectionName));
        PRECONDITION(inBytesToAllocate % 4 == 0);
        PRECONDITION(inBytesToAllocate > 0);
        PRECONDITION(CheckPointer(mappedFile));
    } CONTRACTL_END;

    // Need a place for our result
    LPVOID pResult = NULL;

    //copy the string to stack to avoid GC hole
    STRINGREF inString(inSectionName);    
    
    // Need Frame because OpenOrCreate can throw, also for worst case newholder below.
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_NONE, inString);

    _ASSERTE(inSectionName != NULL);        // Must have a string name.
    _ASSERTE(inBytesToAllocate % 4 == 0);   // Expected 4 bytes boundaries so we don't get unaligned
    _ASSERTE(inBytesToAllocate > 0);        // Pointless to have <=0 allocation
    
    StackSString inNameStackBuffer (inString->GetBuffer());
    pResult = NLSTable::OpenOrCreateMemoryMapping((LPCWSTR)inNameStackBuffer, inBytesToAllocate, mappedFile);

    // Worst case allocate some memory, use holder
    //    if (pResult == NULL) pResult = new BYTE[inBytesToAllocate];
    if (pResult == NULL)
    {
        MEMORY_REPORT_CONTEXT_SCOPE("NLS");

        // Need to use a NewHolder
        NewArrayHolder<BYTE> holder (new BYTE[inBytesToAllocate]);
        pResult = holder;
        // Zero out the mapCodePageCached field (an int value, and it's used to check if the section is initialized or not.)
        BYTE* pByte = (BYTE*)pResult;
        FillMemory(pByte + inBytesToAllocate - sizeof(int), sizeof(int), 0);
        holder.SuppressRelease();
    }

    HELPER_METHOD_FRAME_END();

    return pResult;
}
FCIMPLEND

/*=============================InitializeNativeCompareInfo==============================
**Action: A very thin wrapper on top of the NativeCompareInfo class that prevents us
**        from having to include SortingTable.h in ecall.
**Returns: The LPVOID pointer to the constructed NativeCompareInfo for the specified sort ID.
**        The side effect is to allocate a particular sorting table
**Arguments:
**        pAssembly the NativeGlobalizationAssembly instance used to load the sorting data tables.
**        sortID    the sort ID.
**Exceptions: OutOfMemoryException if we run out of memory.
**
** LOCK CONTRACT: NEED PROCESS-WIDE LOCK
**
**NOTE NOTE: 
**           If you call this method from anyplace else, ensure
**           that synchronization remains intact.
==============================================================================*/

FCIMPL2(LPVOID, COMNlsInfo::InitializeNativeCompareInfo, INT_PTR pNativeGlobalizationAssembly, INT32 sortID)
{
    CONTRACTL
    {
        THROWS;
        INJECT_FAULT(COMPlusThrowOM());
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer((LPVOID)pNativeGlobalizationAssembly));
    } CONTRACTL_END;

    NativeCompareInfo*              pNativeCompareInfo = NULL;
    
    HELPER_METHOD_FRAME_BEGIN_RET_0();

    // Ask the SortingTable instance in pNativeGlobalizationAssembly to get back the
    // NativeCompareInfo object for the specified LCID.
    NativeGlobalizationAssembly*    pNGA = (NativeGlobalizationAssembly*)pNativeGlobalizationAssembly;

    pNativeCompareInfo = pNGA->m_pSortingTable->InitializeNativeCompareInfo(sortID);
    HELPER_METHOD_FRAME_END();

    if (pNativeCompareInfo == NULL) {
        FCThrow(kOutOfMemoryException);
    }

    return pNativeCompareInfo;
}
FCIMPLEND

/*==============================nativeIsSortable==============================
**Action: A wrapper for calling the IsSortable instance method. In theory this could be
**        called on any locale but in practice the locale should be irrelevant (and only
**        the invariant should ever be used). It also does prevent us from having to 
**        include SortingTable.h in ecall.
**Returns: TRUE if the entire string is sortable. FALSE if even one character is not
           considered sortable.
**Arguments:
**        pNativeCompareInfo - pointer to the NativeCompareInfo object
**        pString            - the string to test
**Exceptions: OutOfMemoryException if we run out of memory.
**
**NOTENOTENOTE: That property is only an instance property to make sure that a default
**              table exists. Since the public method is static, the instance depenendency
**              is less than ideal but we use invariant so it should already exist.
==============================================================================*/

FCIMPL2(FC_BOOL_RET, COMNlsInfo::nativeIsSortable, INT_PTR pNativeCompareInfo, StringObject* pString)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer((LPVOID)pNativeCompareInfo));
        PRECONDITION(CheckPointer(pString));
    } CONTRACTL_END;

    STRINGREF refString(pString);
    LPCWSTR buffer = refString->GetBuffer();
    DWORD length = refString->GetStringLength();

    FC_RETURN_BOOL(((NativeCompareInfo*)pNativeCompareInfo)->IsSortable(buffer, length));
}
FCIMPLEND



FCIMPL2(FC_BOOL_RET, COMNlsInfo::IsWin9xInstalledCulture, StringObject *cultureKey, INT32 lcid)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(cultureKey));
    } CONTRACTL_END;


    FC_RETURN_BOOL(::IsValidLocale(lcid, LCID_INSTALLED));
}
FCIMPLEND

FCIMPL2(FC_BOOL_RET, COMNlsInfo::IsValidLCID, INT32 lcid, INT32 flag)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    FC_RETURN_BOOL(::IsValidLocale(lcid, flag));
}
FCIMPLEND


//
//
//
#ifndef LOCALE_SNAME
#define LOCALE_SNAME                0x0000005c
#endif

#ifndef LOCALE_SNAN
#define LOCALE_SNAN                 0x00000069
#endif

#ifndef LOCALE_SPOSINFINITY
#define LOCALE_SPOSINFINITY         0x0000006a
#endif

#ifndef LOCALE_SNEGINFINITY
#define LOCALE_SNEGINFINITY         0x0000006b
#endif

#ifndef LOCALE_SISO3166CTRYNAME2
#define LOCALE_SISO3166CTRYNAME2    0x00000068
#endif

#ifndef LOCALE_SISO639LANGNAME2    
#define LOCALE_SISO639LANGNAME2     0x00000067
#endif

#ifndef LOCALE_SIETFLANGUAGE     
#define LOCALE_SIETFLANGUAGE        0x0000005f
#endif

#ifndef LOCALE_SSHORTESTDAYNAME1
#define LOCALE_SSHORTESTDAYNAME1    0x00000060
#endif

FCIMPL2(Object*, COMNlsInfo::nativeGetUserDefaultLCID, INT32* LCID, INT32 langType)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        PRECONDITION(CheckPointer(LCID));
        PRECONDITION((langType == LOCALE_SYSTEM_DEFAULT) || (langType == LOCALE_USER_DEFAULT));
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    STRINGREF refRetVal = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);

    WCHAR strName[80];
    int size = 0;

    *LCID = ::GetUserDefaultLCID();


    // Not found, either not longhorn (no LOCALE_SNAME) or not a valid name </STRIP>
    if (size == 0)
    {
        // Return an empty string.
        refRetVal = COMString::GetEmptyString();
    }
    else
    {
        refRetVal = COMString::NewString(strName,size-1);
    }

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

/*       
 */
FCIMPL1(Object*, COMNlsInfo::nativeGetUserDefaultUILanguage, INT32* LCID)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        PRECONDITION(CheckPointer(LCID));
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    *LCID = 0;
    STRINGREF refRetVal = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);


    if (*LCID == 0) {
        *LCID = GetUserDefaultLangID();
    }

    // See if we can turn this LCID into a name
    WCHAR strName[80];
    int size = 0;

    if (size == 0)
    {
        // Didn't find string, return an empty string.
        refRetVal = COMString::GetEmptyString();
    }
    else
    {
        refRetVal = COMString::NewString(strName,size-1);
    }

    // Return the found language name.  LCID should be found one already.
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND




//
FCIMPL1(Object*, COMNlsInfo::nativeGetSystemDefaultUILanguage, INT32* LCID)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        PRECONDITION(CheckPointer(LCID));
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    *LCID = 0;
    STRINGREF refRetVal = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);


    if (*LCID == 0) {
        *LCID = ::GetSystemDefaultLangID();
    }

    // Now try to get a name
    WCHAR strName[80];
    int size = 0;
    

    // Not found, either not longhorn (no LOCALE_SNAME) or not a valid name </STRIP>
    if (size == 0)
    {
        // No LCID, return an empty string.
        refRetVal = COMString::GetEmptyString();
    }
    else
    {
        refRetVal = COMString::NewString(strName, size-1);
    }
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND


/*=================================GetMultiStringValues==========================
**Action:
**Returns:
**Arguments:
**Exceptions:
============================================================================*/

PTRARRAYREF COMNlsInfo::GetMultiStringValues(__in_z LPWSTR pInfoStr)
{
    CONTRACTL {
        THROWS;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pInfoStr, NULL_OK));
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    //
    // Get the first string.
    //
    if (pInfoStr == NULL) {
        return (NULL);
    }

    MEMORY_REPORT_CONTEXT_SCOPE("NLS");

    typedef CUnorderedArray<WCHAR *, CULTUREINFO_OPTIONS_SIZE> MyType;
    //
    // Create a dynamic array to store multiple strings.
    //

    // This will throw exception when OOM happens.
    NewHolder<MyType> pStringArray(new CUnorderedArray<WCHAR *, CULTUREINFO_OPTIONS_SIZE>());


    //
    // We can't store STRINGREFs in an unordered array because the GC won't track
    // them properly.  To work around this, we'll count the number of strings
    // which we need to allocate and store a wchar* for the beginning of each string.
    // In the loop below, we'll walk this array of wchar*'s and allocate a managed
    // string for each one.
    //
    while (*pInfoStr != NULL) {
        *(pStringArray->Append()) = pInfoStr;
        //
        // Advance to next string.
        //
        pInfoStr += (Wszlstrlen(pInfoStr) + 1);
    }


    //
    // Allocate the array of STRINGREFs.  We don't need to check for null because the GC will throw
    // an OutOfMemoryException if there's not enough memory.
    //
    PTRARRAYREF ResultArray = (PTRARRAYREF)AllocateObjectArray(pStringArray->Count(), g_pStringClass);

//    LPVOID lpvReturn;
    STRINGREF pString;
    INT32 stringCount = pStringArray->Count();

    //
    // Walk the wchar*'s and allocate a string for each one which we put into the result array.
    //
    GCPROTECT_BEGIN(ResultArray);
    for (int i = 0; i < stringCount; i++) {
        pString = COMString::NewString(pStringArray->m_pTable[i]);
        ResultArray->SetAt(i, (OBJECTREF)pString);
    }
//    *((PTRARRAYREF *)(&lpvReturn))=ResultArray;
    GCPROTECT_END();

    return (ResultArray);
}


//
// This method is only called by Taiwan localized build.
//
FCIMPL2(Object*, COMNlsInfo::nativeGetEraName, INT32 culture, INT32 nValue2)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    STRINGREF refRetVal = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);


    int size;
    WCHAR eraName[64];
    size = WszGetDateFormat(culture, DATE_USE_ALT_CALENDAR , NULL, L"gg", eraName, sizeof(eraName)/sizeof(WCHAR));
    if (size == 0) {
        goto lEmptyString;
    }

    refRetVal = COMString::NewString(eraName, size-1);
    goto lExit;

lEmptyString:
    // Return an empty string.
    refRetVal = COMString::GetEmptyString();

lExit: ;
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND


FCIMPL1(VOID, COMNlsInfo::nativeInitUnicodeCatTable, INT_PTR pBytePtr)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrowVoid(kStackOverflowException));

    CharacterInfoTable::CreateInstance((LPBYTE)pBytePtr);

    END_SO_INTOLERANT_CODE;
}
FCIMPLEND


BYTE COMNlsInfo::GetUnicodeCategory(WCHAR wch) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;
    if (CharacterInfoTable::GetInstance() == NULL) {
        g_Mscorlib.GetClass(CLASS__CHAR_UNICODE_INFO)->CheckRunClassInitThrowing();
    }
    _ASSERTE(CharacterInfoTable::GetInstance() != NULL);
    return (CharacterInfoTable::GetInstance()->GetUnicodeCategory(wch));
}

BOOL COMNlsInfo::nativeIsWhiteSpace(WCHAR c) {
    WRAPPER_CONTRACT;
    // This is the native equivalence of CharacterInfo.IsWhiteSpace().
    switch (c) {
        case ' ':
        case '\x0009' :
        case '\x000a' :
        case '\x000b' :
        case '\x000c' :
        case '\x000d' :
        case '\x0085' :
            return (TRUE);
    }

    BYTE uc = GetUnicodeCategory(c);
    switch (uc) {
        case (11):      // UnicodeCategory.SpaceSeparator
        case (12):      // UnicodeCategory.LineSeparator
        case (13):      // UnicodeCategory.ParagraphSeparator
            return (TRUE);
    }
    return (FALSE);
}


FCIMPL1(FC_BOOL_RET, COMNlsInfo::nativeSetThreadLocale, INT32 lcid)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    BOOL result = TRUE;

    if (IsUnicodeSystem()) {
        IHostTaskManager *manager = CorHost2::GetHostTaskManager();
        if (manager)
        {
            BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
            result = (manager->SetLocale(lcid) == S_OK);
            END_SO_TOLERANT_CODE_CALLING_HOST;
        }
        else
        {
            result = ::SetThreadLocale(lcid);
        }        
    }

    FC_RETURN_BOOL(result);
}
FCIMPLEND

FCIMPL2(Object*, COMNlsInfo::nativeGetLocaleInfo, INT32 lcid, INT32 lcType) 
{
    STATIC_CONTRACT_SO_TOLERANT;

    // The maximum allowed string length in GetLocaleInfo is 80 WCHARs.
    WCHAR buffer[80];
    STRINGREF refRetVal = NULL;
    int result;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);

    {
        result = GetLocaleInfoW(lcid, lcType, buffer, sizeof(buffer)/sizeof(buffer[0]));
    }

    if (result != 0) {
        // Exclude the NULL char at the end.
        refRetVal = COMString::NewString(buffer, result-1);
    }
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND


////////////////////////////////////////////////////////////////////////
//
// Call the Win32 GetLocaleInfo() using the specified lcid to retrive
// the native digits, probably from the registry override. The return
// indicates whether the call was successful.
//
// Parameters:
//       IN lcid            the LCID to make the Win32 call with
//      OUT pOutputStrAry   The output managed string array.
//
////////////////////////////////////////////////////////////////////////

BOOL COMNlsInfo::GetNativeDigitsFromWin32(INT32 lcid, PTRARRAYREF * pOutputStrAry) {
    // The maximum allowed string length for the native digits is 11 WCHARs (10 plus the NULL).
    // For FEATURE_PAL this gets copied below
    WCHAR* buffer = L"0123456789";
    int result = 11;

    STATIC_CONTRACT_SO_TOLERANT;
    
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), return (result==11); )


    //
    // Be very unforgiving and only support strings of size 10 plus the NULL
    //
    if (result == 11) {
        //
        // Break up the unmanaged ten-character ZLS into what NFI wants (a managed
        // ten-string array).
        //

        //
        // Allocate the array of STRINGREFs.  We don't need to check for null because the GC will throw
        // an OutOfMemoryException if there's not enough memory.
        //
        
        PTRARRAYREF DigitArray = (PTRARRAYREF) AllocateObjectArray(10, g_pStringClass);
            
        GCPROTECT_BEGIN(DigitArray);
        for(DWORD i = 0;  i < 10; i++) {
            OBJECTREF o = (OBJECTREF) COMString::NewString(buffer + i, 1);
            DigitArray->SetAt(i, o);
        }
        GCPROTECT_END();
         
        _ASSERTE(pOutputStrAry != NULL);
        *pOutputStrAry = DigitArray;
    }

    END_SO_INTOLERANT_CODE

    return (result == 11);
}


////////////////////////////////////////////////////////////////////////
//
// Call the Win32 GetLocaleInfo() using the specified lcid and LCTYPE.
// The return value can be INT32 or an allocated managed string object.
//
// Parameters:
//      OUT pOutputInt32    The output int32 value.
//      OUT pOutputRef      The output string value.
//
////////////////////////////////////////////////////////////////////////


struct DTFIUserOverrideValues {
     // DTFI values that are affected by calendar setttings.
     STRINGREF shortDatePattern;                    
     STRINGREF longDatePattern;
     STRINGREF yearMonthPattern;                            

     // DTFI values that will not be affected by calendar settings.
     STRINGREF amDesignator;
     STRINGREF pmDesignator;
     STRINGREF longTimePattern;
     INT32 firstDayOfWeek;
     INT32 padding1;  // Add padding to make sure that we are aligned in DWORD. This is important for 64-bit platforms
     INT32 calendarWeekRule;
     INT32 padding2;  // Add padding to make sure that we are aligned in DWORD. This is important for 64-bit platforms
};

//
//
//
//


FCIMPL2(FC_BOOL_RET, COMNlsInfo::nativeGetDTFIUserValues, INT32 lcid, LPVOID pvValue) 
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;



    BOOL ret = FALSE;
    // Do nothing here.
    // In a ROTOR build, we will always use the built-in value from our table.
    FCUnique(0x60);
    FC_RETURN_BOOL(ret);
}
FCIMPLEND

////////////////////////////////////////////////////////////////////////
//
// Implementation of CultureInfo.nativeGetNFIUserValues.  
//
// //  Retrieve NFI properties that can be overridden by users.
//
// Parameters:
//      IN/OUT pNumfmtUNSAFE   
//                  The pointer of the managed NumberFormatInfo passed
//                  from the managed side.
//                  Note that the native NumberFormatInfo* is defined
//                  in COMNumber.h
// Note:
// Managed string will be allocated and assign to the string fields in
//      the managed NumberFormatInfo passed in pNumftUNSAFE
//
////////////////////////////////////////////////////////////////////////


/*
    This is the list of the data members in the managed NumberFormatInfo and their
    corresponding LCTYPE().
    
    Win32 GetLocaleInfo() constatns             Data members in NumberFormatInfo in the defined order.
    LOCALE_SPOSITIVE                            // String positiveSign
    LOCALE_SNEGATIVE                            // String negativeSign
    LOCALE_SDECIMAL                             // String numberDecimalSeparator
    LOCALE_SGROUPING                            // String numberGroupSeparator
    LOCALE_SMONGROUPING                         // String currencyGroupSeparator
    LOCALE_SMONDECIMALSEP                       // String currencyDecimalSeparator
    LOCALE_SCURRENCY                            // String currencySymbol
    N/A                                         // String ansiCurrencySymbol
    N/A                                         // String nanSymbol
    N/A                                         // String positiveInfinitySymbol
    N/A                                         // String negativeInfinitySymbol
    N/A                                         // String percentDecimalSeparator
    N/A                                         // String percentGroupSeparator
    N/A                                         // String percentSymbol
    N/A                                         // String perMilleSymbol

    N/A                                         // int m_dataItem

    LOCALE_IDIGITS | LOCALE_RETURN_NUMBER,      // int numberDecimalDigits
    LOCALE_ICURRDIGITS | LOCALE_RETURN_NUMBER,  // int currencyDecimalDigits
    LOCALE_ICURRENCY | LOCALE_RETURN_NUMBER,    // int currencyPositivePattern
    LOCALE_INEGCURR | LOCALE_RETURN_NUMBER,      // int currencyNegativePattern
    LOCALE_INEGNUMBER| LOCALE_RETURN_NUMBER,    // int numberNegativePattern
    N/A                                         // int percentPositivePattern
    N/A                                         // int percentNegativePattern
    N/A                                         // int percentDecimalDigits
    N/A                                         // bool isReadOnly=false;
    N/A                                         // internal bool m_useUserOverride;      
*/

FCIMPL2(FC_BOOL_RET, COMNlsInfo::nativeGetNFIUserValues, INT32 lcid, NumberFormatInfo* pNumfmtUNSAFE) {
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    // The maximum allowed string length in GetLocaleInfo is 80 WCHARs.
    BOOL ret = FALSE;

    // Do nothing here.
    // In a ROTOR build, we will always use the built-in value from our table.
    FCUnique(0x16);
    FC_RETURN_BOOL(ret);

}
FCIMPLEND

//
// nativeCompareString is used in the managed side to handle the synthetic CompareInfo methods (IndexOf, LastIndexOf, IsPrfix, and IsSuffix)
//
// <SyntheticSupport/>
FCIMPL8(INT32, COMNlsInfo::nativeCompareString, INT32 lcid, StringObject* string1, INT32 offset1, INT32 length1, StringObject* string2, INT32 offset2, INT32 length2, INT32 flags)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(string1));
        PRECONDITION(CheckPointer(string2));
    } CONTRACTL_END;

    FCUnique(0x19);
    return 1;
}
FCIMPLEND


//
// nativeGetCultureName is getting the culture name in the form en-US to be used during the synthetic cultures cache. 
// if the getDayName is true then the method will return the native day name instead to be used to get the sort key.
//
// if useSNameLCType is set the we'll use LOCALE_SNAME to get the culture name. otherwise we'll use LOCALE_SISO639LANGNAME
// and LOCALE_SISO3166CTRYNAME to construct the name.
// note that this method just try LOCALE_SNAME or the other LC types according to useSNameLCType flag but not both and that
// to have a way to detect if the platform is supporting LOCALE_SNAME or not.
//
// <SyntheticSupport/>

FCIMPL3(Object*, COMNlsInfo::nativeGetCultureName, INT32 lcid, CLR_BOOL useSNameLCType, CLR_BOOL getDayName)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;
    
    FCUnique(0x20);
    return NULL;
}
FCIMPLEND

//
// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
// nativeEnumSystemLocales should be called inside locks as it uses the static 
// variables. So we call it from the managed code inside a lock.
//
//
// LOCK CONTRACT: NEED PROCESS-WIDE LOCK
//
// nativeEnumSystemLocales is used to collect the culture data need for synthetic 
// cultures creation.
// <SyntheticSupport/>

FCIMPL1(FC_BOOL_RET, COMNlsInfo::nativeEnumSystemLocales, I4ARRAYREF *localesArray)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    FCUnique(0x21);
    FC_RETURN_BOOL(FALSE);

}
FCIMPLEND

//
// nativeGetCultureData is the method to collect all needed culture data through 
// GetLocaleInfo and EnumCalendarInfo. the data will be used to construct the 
// synthetic cultures.
//
// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
//
// LOCK CONTRACT: NEED PROCESS-WIDE LOCK
//
// nativeGetCultureData should be called inside locks as it uses the static 
// variables. So we call it from the managed code inside a lock.
//
// <SyntheticSupport/>

FCIMPL2(FC_BOOL_RET, COMNlsInfo::nativeGetCultureData, INT32 lcid, CultureData *data)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(data));
    } CONTRACTL_END;

    FCUnique(0x22);
    FC_RETURN_BOOL(FALSE);
}
FCIMPLEND


/*============================ConvertStringCaseFast=============================
**Action:
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/
void COMNlsInfo::ConvertStringCaseFast(__in_ecount(length) WCHAR *inBuff, __out_ecount(length) WCHAR *outBuff, INT32 length, BOOL bIsToUpper) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(inBuff));
        PRECONDITION(CheckPointer(outBuff));
    } CONTRACTL_END

    if (bIsToUpper) {
        for (int i=0; i<length; i++) {
            _ASSERTE(inBuff[i]<0x80);
            outBuff[i]=ToUpperMapping[inBuff[i]];
        }
    } else {
        for (int i=0; i<length; i++) {
            _ASSERTE(inBuff[i]<0x80);
            outBuff[i]=ToLowerMapping[inBuff[i]];
        }
    }
}


/*==============================DoComparisonLookup==============================
**Action:
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/
INT32 COMNlsInfo::DoComparisonLookup(wchar_t charA, wchar_t charB) {
    LEAF_CONTRACT;
    if ((charA ^ charB) & 0x20) {
        //We may be talking about a special case
        if (charA>='A' && charA<='Z') {
            return 1;
        }

        if (charA>='a' && charA<='z') {
            return -1;
        }
    }

    if (charA==charB) {
        return 0;
    }

    return ((charA>charB)?1:-1);
}


/*================================DoCompareChars================================
**Action:
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/
FORCEINLINE INT32 COMNlsInfo::DoCompareChars(WCHAR charA, WCHAR charB, BOOL *bDifferInCaseOnly) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(bDifferInCaseOnly));
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    INT32 result;
    WCHAR temp;

    //The ComparisonTable is a 0x80 by 0x80 table of all of the characters in which we're interested
    //and their sorting value relative to each other.  We can do a straight lookup to get this info.
    result = ComparisonTable[(int)(charA)][(int)(charB)];

    //This is the tricky part of doing locale-aware sorting.  Case-only differences only matter in the
    //event that they're the only difference in the string.  We mark characters that differ only in case
    //and deal with the rest of the logic in CompareFast.
    *bDifferInCaseOnly = (((charA ^ 0x20)==charB) && (((temp=(charA | 0x20))>='a') && (temp<='z')));
    return result;
}


/*=================================CompareFast==================================
**Action:
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/
INT32 COMNlsInfo::CompareFast(STRINGREF strA, STRINGREF strB, BOOL *pbDifferInCaseOnly) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pbDifferInCaseOnly));
        PRECONDITION(strA != NULL);
        PRECONDITION(strB != NULL);
    } CONTRACTL_END

    _ASSERTE(strA != NULL);
    _ASSERTE(strB != NULL);
    WCHAR *charA, *charB;
    DWORD *dwAChars, *dwBChars;
    INT32 strALength, strBLength;
    BOOL bDifferInCaseOnly=false;
    BOOL bDifferTemp;
    INT32 caseOnlyDifference=0;
    INT32 result;

    RefInterpretGetStringValuesDangerousForGC(strA, (WCHAR **) &dwAChars, &strALength);
    RefInterpretGetStringValuesDangerousForGC(strB, (WCHAR **) &dwBChars, &strBLength);
    _ASSERTE(IS_ALIGNED((size_t)dwAChars, sizeof(DWORD)) && IS_ALIGNED((size_t)dwAChars, sizeof(DWORD)));

    *pbDifferInCaseOnly = false;

    // If the strings are the same length, compare exactly the right # of chars.
    // If they are different, compare the shortest # + 1 (the '\0').
    int count = strALength;
    if (count > strBLength)
        count = strBLength;

    ptrdiff_t diff = (char *)dwAChars - (char *)dwBChars;

    int c;


    while ((count-=2)>=0) {
        if ((c = *((DWORD* )((char *)dwBChars + diff)) - *dwBChars) != 0) {
            charB = (WCHAR *)dwBChars;
            charA = ((WCHAR* )((char *)dwBChars + diff));
            if (*charA!=*charB) {
                result = DoCompareChars(*charA, *charB, &bDifferTemp);
                //We know that the two characters are different because of the check that we did before calling DoCompareChars.
                //If they don't differ in just case, we've found the difference, so we can return that.
                if (!bDifferTemp) {
                    return result;
                }

                //We only note the difference the first time that they differ in case only.  If we haven't seen a case-only
                //difference before, we'll record the difference and set bDifferInCaseOnly to true and record the difference.
                if (!bDifferInCaseOnly) {
                    bDifferInCaseOnly = true;
                    caseOnlyDifference=result;
                }
            }
            // Two cases will get us here: The first chars are the same or
            // they differ in case only.
            // The logic here is identical to the logic described above.
            charA++; charB++;
            if (*charA!=*charB) {
                result = DoCompareChars(*charA, *charB, &bDifferTemp);
                if (!bDifferTemp) {
                    return result;
                }
                if (!bDifferInCaseOnly) {
                    bDifferInCaseOnly = true;
                    caseOnlyDifference=result;
                }
            }
        }
        ++dwBChars;
    }

    //We'll only get here if we had an odd number of characters.  If we did, repeat the logic from above for the last
    //character in the string.
    if (count == -1) {
        charB = (WCHAR *)dwBChars;
        charA = ((WCHAR* )((char *)dwBChars + diff));
        if (*charA!=*charB) {
            result = DoCompareChars(*charA, *charB, &bDifferTemp);
            if (!bDifferTemp) {
                return result;
            }
            if (!bDifferInCaseOnly) {
                bDifferInCaseOnly = true;
                caseOnlyDifference=result;
            }
        }
    }

    //If the lengths are the same, return the case-only difference (if such a thing exists).
    //Otherwise just return the longer string.
    if (strALength==strBLength) {
        if (bDifferInCaseOnly) {
            *pbDifferInCaseOnly = true;
            return caseOnlyDifference;
        }
        return 0;
    }

    return (strALength>strBLength)?1:-1;
}

FCIMPL3(INT32, COMNlsInfo::CompareOrdinalIgnoreCase,
     INT_PTR     ptr,
     StringObject*   pString1UNSAFE,
     StringObject*   pString2UNSAFE)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(ptr != NULL);
        PRECONDITION(pString1UNSAFE != NULL);
        PRECONDITION(pString2UNSAFE != NULL);
    } CONTRACTL_END;

     INT32 result = 0;
     STRINGREF   pString1 = ObjectToSTRINGREF(pString1UNSAFE);
     STRINGREF   pString2 = ObjectToSTRINGREF(pString2UNSAFE);

     HELPER_METHOD_FRAME_BEGIN_RET_2(pString1, pString2);

     VALIDATEOBJECTREF(pString1);
     VALIDATEOBJECTREF(pString2);

     NativeTextInfo* pNativeCompareInfo = (NativeTextInfo*)ptr;
     result = pNativeCompareInfo->CompareOrdinalIgnoreCase(
         pString1->GetBuffer(),
         pString1->GetStringLength(),
         pString2->GetBuffer(),
         pString2->GetStringLength());

     HELPER_METHOD_FRAME_END();
     return (result);
}
FCIMPLEND

// we do argument check on managed code, so no checking here.

FCIMPL6(INT32, COMNlsInfo::CompareOrdinalIgnoreCaseEx,
     INT_PTR     ptr,
     StringObject*   pString1UNSAFE,
     INT32 indexA,
     StringObject*   pString2UNSAFE,
     INT32 indexB,
     INT32 count)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(ptr!= NULL);
        PRECONDITION(pString1UNSAFE != NULL);
        PRECONDITION(pString2UNSAFE != NULL);
        PRECONDITION(count > 0);
        PRECONDITION(indexA >= 0);
        PRECONDITION(indexB >= 0);
    } CONTRACTL_END;

     INT32 result = 0;


     STRINGREF   pString1 = ObjectToSTRINGREF(pString1UNSAFE);
     STRINGREF   pString2 = ObjectToSTRINGREF(pString2UNSAFE);
     VALIDATEOBJECTREF(pString1);
     VALIDATEOBJECTREF(pString2);

     HELPER_METHOD_FRAME_BEGIN_RET_2(pString1, pString2);
     
     //If any of our indices are negative throw an exception.

     WCHAR *strAChars, *strBChars;
     int strALength, strBLength;

     RefInterpretGetStringValuesDangerousForGC(pString1, (WCHAR **) &strAChars, &strALength);
     RefInterpretGetStringValuesDangerousForGC(pString2, (WCHAR **) &strBChars, &strBLength);

     int countA = strALength - indexA;
     _ASSERTE(countA >= 0);

     int countB = strBLength - indexB;
     _ASSERTE(countB >= 0);

     NativeTextInfo* pNativeTextInfo = (NativeTextInfo*)ptr;

     result = pNativeTextInfo->CompareOrdinalIgnoreCaseEx(
         strAChars + indexA,
         countA,
         strBChars + indexB,
         countB,
         count);

     HELPER_METHOD_FRAME_END();
     return (result);
}
FCIMPLEND



////////////////////////////////////////////////////////////////////////////
//
//  Compare
//
////////////////////////////////////////////////////////////////////////////

FCIMPL5(INT32, COMNlsInfo::Compare,
    INT_PTR         pNativeCompareInfo,
    INT32           LCID,
    StringObject*   pString1UNSAFE,
    StringObject*   pString2UNSAFE,
    INT32           dwFlags
    )
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION((dwFlags & COMPARE_OPTIONS_ORDINAL) == 0);     // This should be handled in the managed side.
        PRECONDITION(CheckPointer((LPVOID)pNativeCompareInfo));
        PRECONDITION(CheckPointer(pString1UNSAFE));                 // NULL string is handled in the managed code.
        PRECONDITION(CheckPointer(pString2UNSAFE));                 // NULL string is handled in the managed code.
    } CONTRACTL_END;


    INT32       dwRetVal = 0;
    STRINGREF   pString1 = ObjectToSTRINGREF(pString1UNSAFE);
    STRINGREF   pString2 = ObjectToSTRINGREF(pString2UNSAFE);

    HELPER_METHOD_FRAME_BEGIN_RET_2(pString1, pString2);


    //
    //  Checking the parameters are done in the managed side.
    //


    //
    // Check if we can use the highly optimized comparisons
    //

    bool bFastCompareLocale = IS_FAST_COMPARE_LOCALE(LCID);

    if (bFastCompareLocale)
    {
        //If we've never before looked at whether this string has high chars, do so now.
        if (IS_STRING_STATE_UNDETERMINED(pString1->GetHighCharState()))
        {
            COMString::InternalCheckHighChars(pString1);
        }

        //If we've never before looked at whether this string has high chars, do so now.
        if (IS_STRING_STATE_UNDETERMINED(pString2->GetHighCharState()))
        {
            COMString::InternalCheckHighChars(pString2);
        }
    }

    if ((bFastCompareLocale) &&
        (IS_FAST_SORT(pString1->GetHighCharState())) &&
        (IS_FAST_SORT(pString2->GetHighCharState())) &&
        (dwFlags<=COMPARE_OPTIONS_IGNORECASE))  // The value of COMPARE_OPTIONS_IGNORECASE is 1, so we can do this check.
    {
        //0 is no flags.  1 is ignore case.  We can handle both here.
        BOOL bDifferInCaseOnly;
        dwRetVal = CompareFast(pString1, pString2, &bDifferInCaseOnly);

        if (dwFlags != 0) //If we're looking to do a case-sensitive comparison
        {
            //The remainder of this block deals with instances where we're ignoring case.
            if (bDifferInCaseOnly)
            {
                dwRetVal = 0;
            }
        }
    }
    else
    {

        // The return value of NativeCompareInfo::CompareString() is Win32-style value (1=less, 2=equal, 3=larger).
        // So substract by two to get the NLS+ value.
        // Will change NativeCompareInfo to return the correct value later s.t. we don't have
        // to subtract 2.

        // NativeCompareInfo::CompareString() won't take -1 as the end of string anymore.  Therefore,
        // pass the correct string length.
        // The change is for adding the null-embeded string support in CompareString().
        //
        dwRetVal = (((NativeCompareInfo*)(pNativeCompareInfo))->CompareString(
            dwFlags,
            pString1->GetBuffer(),
            pString1->GetStringLength(),
            pString2->GetBuffer(),
            pString2->GetStringLength()) - 2);
    }

    HELPER_METHOD_FRAME_END();

    return dwRetVal;
}
FCIMPLEND

////////////////////////////////////////////////////////////////////////////
//
//  CompareRegion
//
////////////////////////////////////////////////////////////////////////////

FCIMPL9(INT32, COMNlsInfo::CompareRegion,
                    INT_PTR         pNativeCompareInfo,
                    INT32           LCID,
                    StringObject*   pString1UNSAFE,
                    INT32           Offset1,
                    INT32           Length1,
                    StringObject*   pString2UNSAFE,
                    INT32           Offset2,
                    INT32           Length2,
                    INT32           dwFlags)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer((LPVOID)pNativeCompareInfo, NULL_OK));
        PRECONDITION(CheckPointer(pString1UNSAFE));    // Null case is handled in the managed code.
        PRECONDITION(CheckPointer(pString2UNSAFE));    // Null case is handled in the managed code.
        PRECONDITION(Length1 >= 0);
        PRECONDITION(Length2 >= 0);
        PRECONDITION(Offset1 >= 0);
        PRECONDITION(Offset2 >= 0);
    } CONTRACTL_END;

    STRINGREF   pString1 = ObjectToSTRINGREF(pString1UNSAFE);
    STRINGREF   pString2 = ObjectToSTRINGREF(pString2UNSAFE);
    INT32       dwRetVal = 0;
    HELPER_METHOD_FRAME_BEGIN_RET_2(pString1, pString2);

    _ASSERTE(Offset1 <= (INT32)pString1->GetStringLength() - Length1);
    _ASSERTE(Offset2 <= (INT32)pString2->GetStringLength() - Length2);

    dwRetVal = (((NativeCompareInfo*)(pNativeCompareInfo))->CompareString(
        dwFlags,
        pString1->GetBuffer() + Offset1,
        Length1,
        pString2->GetBuffer() + Offset2,
        Length2) - 2);
    HELPER_METHOD_FRAME_END();

    return dwRetVal;

}
FCIMPLEND


FCIMPL2(INT32, COMNlsInfo::GetHashCodeOrdinalIgnoreCase, INT_PTR ptr, StringObject* pStringUNSAFE) 
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(ptr != NULL);
        PRECONDITION(pStringUNSAFE != NULL);
    } CONTRACTL_END;

    INT32 result = 0;
    STRINGREF   pString = ObjectToSTRINGREF(pStringUNSAFE);

    VALIDATEOBJECTREF(pString);

    NativeTextInfo* pNativeCompareInfo = (NativeTextInfo*)ptr;
    result = pNativeCompareInfo->GetHashCodeOrdinalIgnoreCase(pString->GetBuffer());

    return (result);
}
FCIMPLEND


////////////////////////////////////////////////////////////////////////////
//
//  IndexOfChar
//
////////////////////////////////////////////////////////////////////////////

FCIMPL7(INT32, COMNlsInfo::IndexOfChar, INT_PTR pNativeCompareInfo, INT32 myLCID, StringObject* pStringUNSAFE,  CLR_CHAR ch, INT32 StartIndex, INT32 Count, INT32 dwFlags)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer((LPVOID)pNativeCompareInfo));
        PRECONDITION(CheckPointer(pStringUNSAFE));
    } CONTRACTL_END;

    INT32     iRetVal = -1;
    STRINGREF pString = (STRINGREF) pStringUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_1(pString)

    WCHAR *buffer = NULL;
    BOOL bASCII = FALSE;

    //
    //  Get the arguments.
    //
    int StringLength = pString->GetStringLength();

    _ASSERTE(StartIndex >= 0 && StartIndex <= StringLength);

    int EndIndex = StartIndex - 1 + Count;

    _ASSERTE(Count >= 0 && EndIndex < StringLength);

    //
    //  Check the ranges.
    //
    if (StringLength == 0)
    {
        goto lExit;
    }

    //
    //  Search for the character in the string starting at StartIndex.

    buffer = pString->GetBuffer();
    int ctr;

    if (dwFlags!=COMPARE_OPTIONS_ORDINAL) {
        //
        // Check if we can use the highly optimized comparisons
        //

        //If we've never before looked at whether this string has high chars, do so now.
        if (IS_STRING_STATE_UNDETERMINED(pString->GetHighCharState())) {
            COMString::InternalCheckHighChars(pString);
        }

        bASCII = ((IS_FAST_INDEX(pString->GetHighCharState())) && ch < 0x7f) || (ch == 0);
    }

    if ((bASCII && dwFlags == 0) || (dwFlags == COMPARE_OPTIONS_ORDINAL))
    {
        for (ctr = StartIndex; ctr <= EndIndex; ctr++)
        {
            if (buffer[ctr] == ch)
            {
                iRetVal = ctr;
                goto lExit;
            }
        }
        goto lExit;
    }
    else if (bASCII && dwFlags == COMPARE_OPTIONS_IGNORECASE)
    {
        WCHAR chctr= 0;
        WCHAR UpperValue = (ch>='A' && ch<='Z')?(ch|0x20):ch;

        for (ctr = StartIndex; ctr <= EndIndex; ctr++)
        {
            chctr = buffer[ctr];
            chctr = (chctr>='A' && chctr<='Z')?(chctr|0x20):chctr;

            if (UpperValue == chctr) {
                iRetVal = ctr;
                goto lExit;
            }
        }
        goto lExit;
    }
    iRetVal = ((NativeCompareInfo*)(pNativeCompareInfo))->IndexOfString(
        pString->GetBuffer(), &ch, StartIndex, EndIndex, 1, dwFlags, FALSE);

    // We checked flags in managed code, so this shouldn't happen.
    _ASSERTE(iRetVal != INDEXOF_INVALID_FLAGS);
//    if (iRetVal == INDEXOF_INVALID_FLAGS) {
//        COMPlusThrowArgumentException(L"flags", L"Argument_InvalidFlag");
//    }

lExit: ;
    HELPER_METHOD_FRAME_END();
    return iRetVal;
}
FCIMPLEND

////////////////////////////////////////////////////////////////////////////
//
//  LastIndexOfChar
//
////////////////////////////////////////////////////////////////////////////

FCIMPL7(INT32, COMNlsInfo::LastIndexOfChar, INT_PTR pNativeCompareInfo, INT32 myLCID, StringObject* pStringUNSAFE,  CLR_CHAR ch, INT32 StartIndex, INT32 Count, INT32 dwFlags)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer((LPVOID)pNativeCompareInfo));
        PRECONDITION(CheckPointer(pStringUNSAFE));
    } CONTRACTL_END;

    INT32     iRetVal = -1;
    STRINGREF pString = (STRINGREF) pStringUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_1(pString);

    LCID Locale = 0;
    WCHAR *buffer = NULL;
    BOOL bASCII = FALSE;

    //
    //  Get the arguments.
    //
    int StringLength = pString->GetStringLength();

    //
    //  Check the ranges.
    //

    _ASSERTE(StartIndex >= 0 && StartIndex <= StringLength);

    int EndIndex = StartIndex - Count + 1;

    // This also catches startIndex == MAXINT, and MAXINT + 1 == -1 when count == 0.
    _ASSERT(Count >= 0 && EndIndex >= 0);

    //
    //  Empty string is trivial
    //
    if (StringLength == 0)
    {
        goto lExit;
    }

    //
    //  Search for the character in the string starting at EndIndex.
    Locale = myLCID;
    buffer = pString->GetBuffer();
    int ctr;

    //If they haven't asked for exact comparison, we may still be able to do a
    //fast comparison if the string is all less than 0x80.
    if (dwFlags!=COMPARE_OPTIONS_ORDINAL) {
        //If we've never before looked at whether this string has high chars, do so now.
        if (IS_STRING_STATE_UNDETERMINED(pString->GetHighCharState())) {
            COMString::InternalCheckHighChars(pString);
        }

        bASCII = (IS_FAST_INDEX(pString->GetHighCharState()) && ch < 0x7f) || (ch == 0);
    }

    if ((bASCII && dwFlags == 0) || (dwFlags == COMPARE_OPTIONS_ORDINAL))
    {
        for (ctr = StartIndex; ctr >= EndIndex; ctr--)
        {
            if (buffer[ctr] == ch)
            {
                iRetVal = ctr;
                goto lExit;
            }
        }
        goto lExit;
    }
    else if (bASCII && dwFlags == COMPARE_OPTIONS_IGNORECASE)
    {
        WCHAR UpperValue = (ch>='A' && ch<='Z')?(ch|0x20):ch;
        WCHAR chctr;

        for (ctr = StartIndex; ctr >= EndIndex; ctr--)
        {
            chctr = buffer[ctr];
            chctr = (chctr>='A' && chctr<='Z')?(chctr|0x20):chctr;

            if (UpperValue == chctr) {
                iRetVal = ctr;
                goto lExit;
            }
        }
        goto lExit;
    }
    int nMatchEndIndex;
    iRetVal = ((NativeCompareInfo*)(pNativeCompareInfo))->LastIndexOfString(
        pString->GetBuffer(), &ch, StartIndex, EndIndex, 1, dwFlags, &nMatchEndIndex);

    // We checked flags in managed code, so this shouldn't happen.
    _ASSERTE(iRetVal != INDEXOF_INVALID_FLAGS);
//    if (iRetVal == INDEXOF_INVALID_FLAGS) {
//        COMPlusThrowArgumentException(L"flags", L"Argument_InvalidFlag");
//    }

lExit: ;
    HELPER_METHOD_FRAME_END();
    return iRetVal;
}
FCIMPLEND

INT32 COMNlsInfo::FastIndexOfString(__in WCHAR *source, INT32 startIndex, INT32 endIndex, __in_ecount(patternLength) WCHAR *pattern, INT32 patternLength)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(source));
        PRECONDITION(CheckPointer(pattern));
        PRECONDITION(startIndex >= 0);
        PRECONDITION(endIndex >= 0);
        PRECONDITION(patternLength>= 0);
    } CONTRACTL_END

    int endPattern = endIndex - patternLength + 1;

    if (endPattern<0) {
        return -1;
    }

    if (patternLength <= 0) {
        return startIndex;
    }

    WCHAR patternChar0 = pattern[0];
    for (int ctrSrc = startIndex; ctrSrc<=endPattern; ctrSrc++) {
        if (source[ctrSrc] != patternChar0)
            continue;
        int ctrPat;
        for (ctrPat = 1; (ctrPat < patternLength) && (source[ctrSrc + ctrPat] == pattern[ctrPat]); ctrPat++) {
            ;
        }
        if (ctrPat == patternLength) {
            return (ctrSrc);
        }
    }

    return (-1);
}

INT32 COMNlsInfo::FastIndexOfStringInsensitive(__in WCHAR *source, INT32 startIndex, INT32 endIndex, __in_ecount(patternLength) WCHAR *pattern, INT32 patternLength) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(source));
        PRECONDITION(CheckPointer(pattern));
        PRECONDITION(startIndex >= 0);
        PRECONDITION(endIndex >= 0);
        PRECONDITION(patternLength>= 0);
    } CONTRACTL_END;
    WCHAR srcChar;
    WCHAR patChar;

    int endPattern = endIndex - patternLength + 1;

    if (endPattern<0) {
        return -1;
    }

    for (int ctrSrc = startIndex; ctrSrc<=endPattern; ctrSrc++) {
        int ctrPat;
        for (ctrPat = 0; (ctrPat < patternLength); ctrPat++) {
            srcChar = source[ctrSrc + ctrPat];
            if (srcChar>='A' && srcChar<='Z') {
                srcChar|=0x20;
            }
            patChar = pattern[ctrPat];
            if (patChar>='A' && patChar<='Z') {
                patChar|=0x20;
            }
            if (srcChar!=patChar) {
                break;
            }
        }

        if (ctrPat == patternLength) {
            return (ctrSrc);
        }
    }

    return (-1);
}

////////////////////////////////////////////////////////////////////////////
//
//  IndexOfString
//
////////////////////////////////////////////////////////////////////////////

FCIMPL7(INT32, COMNlsInfo::IndexOfString,
                    INT_PTR pNativeCompareInfo,
                    INT32 LCID,
                    StringObject* pString1UNSAFE,   // String to search in
                    StringObject* pString2UNSAFE,   // String we're looking for
                    INT32 StartIndex,               // Index to start at in search string
                    INT32 Count,                    // # of chars to search
                    INT32 dwFlags)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer((LPVOID)pNativeCompareInfo));
        PRECONDITION(CheckPointer(pString1UNSAFE));
        PRECONDITION(CheckPointer(pString2UNSAFE));
    } CONTRACTL_END;

    INT32       iRetVal = -1;
    STRINGREF   pString1 = (STRINGREF) pString1UNSAFE;
    STRINGREF   pString2 = (STRINGREF) pString2UNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_2(pString1, pString2);

    WCHAR *Buffer1 = NULL;
    WCHAR *Buffer2 = NULL;

    //
    //  Get the arguments.
    //
    int StringLength1;
    int StringLength2;
    StringLength1 = pString1->GetStringLength();
    StringLength2 = pString2->GetStringLength();

    //
    //  Get the arguments.
    //
    _ASSERTE(StartIndex >= 0 && StartIndex <= StringLength1);

    int EndIndex = StartIndex - 1 + Count;

    _ASSERTE(Count >= 0 && EndIndex < StringLength1);

    //
    //  Check the ranges.
    //
    if (StringLength1 == 0)
    {
        if (StringLength2 == 0)
            iRetVal = 0;
        // else iRetVal = -1 (not found)
        goto lExit;
    }

    //
    //  See if we have an empty string 2.
    //
    if (StringLength2 == 0)
    {
        iRetVal = StartIndex;
        goto lExit;
    }

    //
    //  Search for the character in the string.
    //
    Buffer1 = pString1->GetBuffer();
    Buffer2 = pString2->GetBuffer();

    if (dwFlags == COMPARE_OPTIONS_ORDINAL)
    {
        iRetVal = FastIndexOfString(Buffer1, StartIndex, EndIndex, Buffer2, StringLength2);
        goto lExit;
    }

    //For dwFlags, 0 is the default, 1 is ignore case, we can handle both.
    if (dwFlags<=1 && IS_FAST_COMPARE_LOCALE(LCID))
    {
        //If we've never before looked at whether this string has high chars, do so now.
        if (IS_STRING_STATE_UNDETERMINED(pString1->GetHighCharState()))
        {
            COMString::InternalCheckHighChars(pString1);
        }

        //If we've never before looked at whether this string has high chars, do so now.
        if (IS_STRING_STATE_UNDETERMINED(pString2->GetHighCharState()))
        {
            COMString::InternalCheckHighChars(pString2);
        }

        //If neither string has high chars, we can use a much faster comparison algorithm.
        if (IS_FAST_INDEX(pString1->GetHighCharState()) && IS_FAST_INDEX(pString2->GetHighCharState()))
        {
            if (dwFlags==0)
                iRetVal = FastIndexOfString(Buffer1, StartIndex, EndIndex, Buffer2, StringLength2);
            else
                iRetVal = FastIndexOfStringInsensitive(Buffer1, StartIndex, EndIndex, Buffer2, StringLength2);
            goto lExit;
        }
    }

    iRetVal = ((NativeCompareInfo*)(pNativeCompareInfo))->IndexOfString(
        Buffer1, Buffer2, StartIndex, EndIndex, StringLength2, dwFlags, FALSE);

    // We checked flags in managed code, so this shouldn't happen.
    _ASSERTE(iRetVal != INDEXOF_INVALID_FLAGS);
//    if (iRetVal == INDEXOF_INVALID_FLAGS) {
//        COMPlusThrowArgumentException(L"flags", L"Argument_InvalidFlag");
//    }

lExit: ;
    HELPER_METHOD_FRAME_END();
    return iRetVal;

}
FCIMPLEND

FCIMPL5(INT32, COMNlsInfo::IndexOfStringOrdinalIgnoreCase,
                    INT_PTR     ptr,
                    StringObject* pString1UNSAFE,   // String to search in
                    StringObject* pString2UNSAFE,   // String we're looking for
                    INT32 StartIndex,               // Index to start at in search string
                    INT32 Count)                    // # of chars to search
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(ptr != NULL);
        PRECONDITION(CheckPointer(pString1UNSAFE));
        PRECONDITION(CheckPointer(pString2UNSAFE));
    } CONTRACTL_END;

    INT32       iRetVal = -1;
    STRINGREF   pString1 = (STRINGREF) pString1UNSAFE;
    STRINGREF   pString2 = (STRINGREF) pString2UNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_2(pString1, pString2);

    WCHAR *Buffer1 = NULL;
    WCHAR *Buffer2 = NULL;

    //
    //  Get the arguments.
    //
    int StringLength1;
    int StringLength2;
    StringLength1 = pString1->GetStringLength();
    StringLength2 = pString2->GetStringLength();

    //
    //  Get the arguments.
    //
    _ASSERTE(StartIndex >= 0 && StartIndex <= StringLength1);

    int EndIndex = StartIndex - 1 + Count;

    _ASSERTE(Count >= 0 && EndIndex < StringLength1);

    //
    //  Check the ranges.
    //
    if (StringLength1 == 0)
    {
        if (StringLength2 == 0)
            iRetVal = 0;
        // else iRetVal = -1 (not found)
        goto lExit;
    }

    //
    //  See if we have an empty string 2.
    //
    if (StringLength2 == 0)
    {
        iRetVal = StartIndex;
        goto lExit;
    }

    //
    //  Search for the character in the string.
    //
    Buffer1 = pString1->GetBuffer();
    Buffer2 = pString2->GetBuffer();

    //If we've never before looked at whether this string has high chars, do so now.
    if (IS_STRING_STATE_UNDETERMINED(pString1->GetHighCharState()))
    {
        COMString::InternalCheckHighChars(pString1);
    }
    
    //If we've never before looked at whether this string has high chars, do so now.
    if (IS_STRING_STATE_UNDETERMINED(pString2->GetHighCharState()))
    {
        COMString::InternalCheckHighChars(pString2);
    }
    
    //If neither string has high chars, we can use a much faster comparison algorithm.
    if (IS_FAST_INDEX(pString1->GetHighCharState()) && IS_FAST_INDEX(pString2->GetHighCharState())) {
        iRetVal = FastIndexOfStringInsensitive(Buffer1, StartIndex, EndIndex, Buffer2, StringLength2);
    }
    else {
        NativeTextInfo* pNativeTextInfo = (NativeTextInfo*)ptr;
        iRetVal = pNativeTextInfo->IndexOfStringOrdinalIgnoreCase(Buffer1, StartIndex, EndIndex, Buffer2, StringLength2);
    }

lExit: ;
    HELPER_METHOD_FRAME_END();
    return iRetVal;

}
FCIMPLEND


FCIMPL5(INT32, COMNlsInfo::IndexOfCharOrdinalIgnoreCase,
                    INT_PTR     ptr,
                    StringObject* pString1UNSAFE,   // String to search in
                    CLR_CHAR value,                 // character we're looking for
                    INT32 StartIndex,               // Index to start at in search string
                    INT32 Count)                    // # of chars to search
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(ptr != NULL);
        PRECONDITION(CheckPointer(pString1UNSAFE));
    } CONTRACTL_END;

    INT32           iRetVal = -1;
    STRINGREF       pString1 = (STRINGREF) pString1UNSAFE;
    NativeTextInfo* pNativeTextInfo;
    int             charToFind;

    HELPER_METHOD_FRAME_BEGIN_RET_1(pString1);

    WCHAR *Buffer1 = NULL;

    int StringLength1 = pString1->GetStringLength();

    _ASSERTE(StartIndex >= 0 && StartIndex <= StringLength1);

    int EndIndex = StartIndex - 1 + Count;

    _ASSERTE(Count >= 0 && EndIndex < StringLength1);

    if (StringLength1 == 0)
    {
        goto lExit;
    }

    Buffer1 = pString1->GetBuffer();

    pNativeTextInfo = (NativeTextInfo*)ptr;
   
    charToFind = pNativeTextInfo->ChangeCaseChar(TRUE, (int) value);
    for (int i=StartIndex; i<=EndIndex; i++)
    {
        int charToCheck = pNativeTextInfo->ChangeCaseChar(TRUE, (int) Buffer1[i]);
        if (charToCheck == charToFind)
        {
            iRetVal = i;
            break;
        }
    }

lExit: ;
    HELPER_METHOD_FRAME_END();
    return iRetVal;
}
FCIMPLEND


FCIMPL5(INT32, COMNlsInfo::LastIndexOfCharOrdinalIgnoreCase,
                    INT_PTR ptr,
                    StringObject* pString1UNSAFE,       // String to search in
                    CLR_CHAR value,                     // String to find
                    INT32 StartIndex,                   // Index to start at
                    INT32 Count)                        // # of chars to search
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(ptr != NULL);
        PRECONDITION(CheckPointer((LPVOID)ptr));
        PRECONDITION(CheckPointer(pString1UNSAFE));
    } CONTRACTL_END;

    INT32           iRetVal = -1;
    STRINGREF       pString1 = (STRINGREF) pString1UNSAFE;
    NativeTextInfo* pNativeTextInfo;
                    int charToFind;

    HELPER_METHOD_FRAME_BEGIN_RET_1(pString1);

    int startIndex = StartIndex;
    int count = Count;
    int stringLength1 = pString1->GetStringLength();

    WCHAR *buffString = NULL;

    _ASSERTE(startIndex >= 0 && startIndex <= stringLength1);

    int endIndex = startIndex - count + 1;

    _ASSERT(count >= 0 && endIndex >= 0);

    if (stringLength1 == 0)
    {
        goto lExit;
    }

    buffString = pString1->GetBuffer();

    pNativeTextInfo = (NativeTextInfo*)ptr;

    charToFind = pNativeTextInfo->ChangeCaseChar(TRUE, (int) value);
    for (int i=startIndex; i>=endIndex; i--)
    {
        int charToCheck = pNativeTextInfo->ChangeCaseChar(TRUE, (int) buffString[i]);
        if (charToCheck == charToFind)
        {
            iRetVal = i;
            break;
        }
    }

lExit: ;
    HELPER_METHOD_FRAME_END();
    return iRetVal;

}
FCIMPLEND




INT32 COMNlsInfo::FastLastIndexOfString(__in WCHAR *source, INT32 startIndex, INT32 endIndex, __in_ecount(patternLength) WCHAR *pattern, INT32 patternLength)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(source));
        PRECONDITION(CheckPointer(pattern));
        PRECONDITION(startIndex >= 0);
        PRECONDITION(endIndex >= 0);
        PRECONDITION(patternLength>= 0);
    } CONTRACTL_END;
    //startIndex is the greatest index into the string.
    int startPattern = startIndex - patternLength + 1;

    if (startPattern < 0) {
        return (-1);
    }

    for (int ctrSrc = startPattern; ctrSrc >= endIndex; ctrSrc--) {
        int ctrPat;
        for (ctrPat = 0; (ctrPat<patternLength) && (source[ctrSrc+ctrPat] == pattern[ctrPat]); ctrPat++) {
            //Deliberately empty.
        }
        if (ctrPat == patternLength) {
            return (ctrSrc);
        }
    }

    return (-1);
}

INT32 COMNlsInfo::FastLastIndexOfStringInsensitive(__in WCHAR *source, INT32 startIndex, INT32 endIndex, __in_ecount(patternLength) WCHAR *pattern, INT32 patternLength) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(source));
        PRECONDITION(CheckPointer(pattern));
        PRECONDITION(startIndex >= 0);
        PRECONDITION(endIndex >= 0);
        PRECONDITION(patternLength>= 0);
    } CONTRACTL_END;
    //startIndex is the greatest index into the string.
    int startPattern = startIndex - patternLength + 1;
    WCHAR srcChar;
    WCHAR patChar;

    if (startPattern < 0) {
        return (-1);
    }

    for (int ctrSrc = startPattern; ctrSrc >= endIndex; ctrSrc--) {
        int ctrPat;
        for (ctrPat = 0; (ctrPat<patternLength); ctrPat++) {
            srcChar = source[ctrSrc+ctrPat];
            if (srcChar>='A' && srcChar<='Z') {
                srcChar|=0x20;
            }
            patChar = pattern[ctrPat];
            if (patChar>='A' && patChar<='Z') {
                patChar|=0x20;
            }
            if (srcChar!=patChar) {
                break;
            }
        }
        if (ctrPat == patternLength) {
            return (ctrSrc);
        }
    }

    return (-1);
}

// The parameter verfication is done in the managed side.
FCIMPL5(FC_BOOL_RET, COMNlsInfo::nativeIsPrefix, INT_PTR pNativeCompareInfo, INT32 LCID, StringObject* pString1UNSAFE, StringObject* pString2UNSAFE, INT32 dwFlags)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer((LPVOID)pNativeCompareInfo));
        PRECONDITION(CheckPointer(pString1UNSAFE));
        PRECONDITION(CheckPointer(pString2UNSAFE));
    } CONTRACTL_END;

    BOOL bReturn = FALSE;
    STRINGREF pString1 = (STRINGREF) pString1UNSAFE;
    STRINGREF pString2 = (STRINGREF) pString2UNSAFE;

    int SourceLength = pString1->GetStringLength();
    int PrefixLength = pString2->GetStringLength();

    WCHAR *SourceBuffer = pString1->GetBuffer();
    WCHAR *PrefixBuffer = pString2->GetBuffer();

    if (dwFlags == COMPARE_OPTIONS_ORDINAL) {
        if (PrefixLength > SourceLength) {
            FC_RETURN_BOOL(FALSE);
        }
        FC_RETURN_BOOL(memcmp(SourceBuffer, PrefixBuffer, PrefixLength * sizeof(WCHAR)) == 0);
    }

    HELPER_METHOD_FRAME_BEGIN_RET_2(pString1, pString2);

    //For dwFlags, 0 is the default, 1 is ignore case, we can handle both.
    if (dwFlags<=1 && IS_FAST_COMPARE_LOCALE(LCID)) {
        //If we've never before looked at whether this string has high chars, do so now.
        if (IS_STRING_STATE_UNDETERMINED(pString1->GetHighCharState())) {
            COMString::InternalCheckHighChars(pString1);
        }

        //If we've never before looked at whether this string has high chars, do so now.
        if (IS_STRING_STATE_UNDETERMINED(pString2->GetHighCharState())) {
            COMString::InternalCheckHighChars(pString2);
        }

        //If neither string has high chars, we can use a much faster comparison algorithm.
        if (IS_FAST_INDEX(pString1->GetHighCharState()) && IS_FAST_INDEX(pString2->GetHighCharState())) {
            if (SourceLength < PrefixLength) {
                goto EXIT;  // bReturn = false
            }
            if (dwFlags==0) {
                bReturn = (memcmp(SourceBuffer, PrefixBuffer, PrefixLength * sizeof(WCHAR)) == 0);
                goto EXIT;
            } else {
                LPCWSTR SourceEnd = SourceBuffer + PrefixLength;
                while (SourceBuffer < SourceEnd) {
                    WCHAR srcChar = *SourceBuffer;
                    if (srcChar>='A' && srcChar<='Z') {
                        srcChar|=0x20;
                    }
                    WCHAR prefixChar = *PrefixBuffer;
                    if (prefixChar>='A' && prefixChar<='Z') {
                        prefixChar|=0x20;
                    }
                    if (srcChar!=prefixChar) {
                        goto EXIT;  // bReturn == FALSE;
                    }
                    SourceBuffer++; PrefixBuffer++;
                }
                bReturn = TRUE;
                goto EXIT;
            }
        }
    }

    bReturn = ((NativeCompareInfo*)pNativeCompareInfo)->IsPrefix(SourceBuffer, SourceLength, PrefixBuffer, PrefixLength, dwFlags);

EXIT:;
    HELPER_METHOD_FRAME_END();

    FC_RETURN_BOOL(bReturn);
}
FCIMPLEND

// The parameter verfication is done in the managed side.
FCIMPL5(FC_BOOL_RET, COMNlsInfo::nativeIsSuffix, INT_PTR pNativeCompareInfo, INT32 LCID, StringObject* pString1UNSAFE, StringObject* pString2UNSAFE, INT32 dwFlags)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer((LPVOID)pNativeCompareInfo));
        PRECONDITION(CheckPointer(pString1UNSAFE));
        PRECONDITION(CheckPointer(pString2UNSAFE));
    } CONTRACTL_END;

    BOOL bReturn = FALSE;
    STRINGREF pString1 = (STRINGREF) pString1UNSAFE;
    STRINGREF pString2 = (STRINGREF) pString2UNSAFE;

    int SourceLength = pString1->GetStringLength();
    int SuffixLength = pString2->GetStringLength();

    WCHAR *SourceBuffer = pString1->GetBuffer();
    WCHAR *SuffixBuffer = pString2->GetBuffer();

    if (dwFlags == COMPARE_OPTIONS_ORDINAL) {
        if (SuffixLength > SourceLength) {
            FC_RETURN_BOOL(FALSE);
        }
        FC_RETURN_BOOL(memcmp(SourceBuffer + SourceLength - SuffixLength, SuffixBuffer, SuffixLength * sizeof(WCHAR)) == 0);
    }

    HELPER_METHOD_FRAME_BEGIN_RET_2(pString1, pString2);

    //For dwFlags, 0 is the default, 1 is ignore case, we can handle both.
    if (dwFlags<=1 && IS_FAST_COMPARE_LOCALE(LCID)) {
        //If we've never before looked at whether this string has high chars, do so now.
        if (IS_STRING_STATE_UNDETERMINED(pString1->GetHighCharState())) {
            COMString::InternalCheckHighChars(pString1);
        }

        //If we've never before looked at whether this string has high chars, do so now.
        if (IS_STRING_STATE_UNDETERMINED(pString2->GetHighCharState())) {
            COMString::InternalCheckHighChars(pString2);
        }

        //If neither string has high chars, we can use a much faster comparison algorithm.
        if (IS_FAST_INDEX(pString1->GetHighCharState()) && IS_FAST_INDEX(pString2->GetHighCharState())) {
            int nSourceStart = SourceLength - SuffixLength;
            if (nSourceStart < 0) {
                goto EXIT; // bReturn == FALSE;
            }
            if (dwFlags==0) {
                bReturn = (memcmp(SourceBuffer + nSourceStart, SuffixBuffer, SuffixLength * sizeof(WCHAR)) == 0);
                goto EXIT;
            } else {
                LPCWSTR SourceEnd = SourceBuffer + SourceLength;
                SourceBuffer += nSourceStart;
                while (SourceBuffer < SourceEnd) {
                    WCHAR srcChar = *SourceBuffer;
                    if (srcChar>='A' && srcChar<='Z') {
                        srcChar|=0x20;
                    }
                    WCHAR suffixChar = *SuffixBuffer;
                    if (suffixChar>='A' && suffixChar<='Z') {
                        suffixChar|=0x20;
                    }
                    if (srcChar!=suffixChar) {
                        goto EXIT; // bReturn == FALSE;
                    }
                    SourceBuffer++; SuffixBuffer++;
                }
                bReturn = TRUE;
                goto EXIT;
            }
        }
    }

    bReturn = ((NativeCompareInfo*)pNativeCompareInfo)->IsSuffix(SourceBuffer, SourceLength, SuffixBuffer, SuffixLength, dwFlags);

EXIT:;
    HELPER_METHOD_FRAME_END();

    FC_RETURN_BOOL(bReturn);
}
FCIMPLEND

////////////////////////////////////////////////////////////////////////////
//
//  LastIndexOfString
//
////////////////////////////////////////////////////////////////////////////

FCIMPL7(INT32, COMNlsInfo::LastIndexOfString,
                    INT_PTR pNativeCompareInfo,
                    INT32 myLCID,
                    StringObject* pString1UNSAFE,       // String to search in
                    StringObject* pString2UNSAFE,       // String to find
                    INT32 StartIndex,                   // Index to start at
                    INT32 Count,                        // # of chars to search
                    INT32 dwFlags)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer((LPVOID)pNativeCompareInfo));
        PRECONDITION(CheckPointer(pString1UNSAFE));
        PRECONDITION(CheckPointer(pString2UNSAFE));
    } CONTRACTL_END;

    INT32       iRetVal = -1;
    STRINGREF   pString1 = (STRINGREF) pString1UNSAFE;
    STRINGREF   pString2 = (STRINGREF) pString2UNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_2(pString1, pString2);

    //
    //  Get the arguments.
    //
    int startIndex = StartIndex;
    int count = Count;
    int stringLength1 = pString1->GetStringLength();
    int findLength = pString2->GetStringLength();

    WCHAR *buffString = NULL;
    WCHAR *buffFind = NULL;

    //
    //  Check the ranges.
    //

    _ASSERTE(startIndex >= 0 && startIndex <= stringLength1);

    int endIndex = startIndex - count + 1;

    // This also catches startIndex == MAXINT, and MAXINT + 1 == -1 when count == 0.
    _ASSERT(count >= 0 && endIndex >= 0);

    // Check for empty strings
    if (stringLength1 == 0)
    {
        if (findLength == 0)
            iRetVal = 0;
        // else iRetVal = -1 (not found)
        goto lExit;
    }

    //
    //  See if we have an empty string 2.
    //
    if (findLength == 0)
    {
        iRetVal = startIndex;
        goto lExit;
    }

    //
    //  Search for the character in the string.
    buffString = pString1->GetBuffer();
    buffFind = pString2->GetBuffer();

    if (dwFlags == COMPARE_OPTIONS_ORDINAL) {
        iRetVal = FastLastIndexOfString(buffString, startIndex, endIndex, buffFind, findLength);
        goto lExit;
    }

    //For dwFlags, 0 is the default, 1 is ignore case, we can handle both.
    if (dwFlags<=1 && IS_FAST_COMPARE_LOCALE(myLCID)) {
        //If we've never before looked at whether this string has high chars, do so now.
        if (IS_STRING_STATE_UNDETERMINED(pString1->GetHighCharState())) {
            COMString::InternalCheckHighChars(pString1);
        }

        //If we've never before looked at whether this string has high chars, do so now.
        if (IS_STRING_STATE_UNDETERMINED(pString2->GetHighCharState())) {
            COMString::InternalCheckHighChars(pString2);
        }

        //If neither string has high chars, we can use a much faster comparison algorithm.
        if (IS_FAST_INDEX(pString1->GetHighCharState()) && IS_FAST_INDEX(pString2->GetHighCharState()))
        {
            if (dwFlags==0)
                iRetVal = FastLastIndexOfString(buffString, startIndex, endIndex, buffFind, findLength);
            else
                iRetVal = FastLastIndexOfStringInsensitive(buffString, startIndex, endIndex, buffFind, findLength);
            goto lExit;
        }
    }

    int nMatchEndIndex;
    iRetVal = ((NativeCompareInfo*)(pNativeCompareInfo))->LastIndexOfString(
        buffString, buffFind, startIndex, endIndex, findLength, dwFlags, &nMatchEndIndex);

    // We checked flags in managed code, so this shouldn't happen.
    _ASSERTE(iRetVal != INDEXOF_INVALID_FLAGS);
//    if (iRetVal == INDEXOF_INVALID_FLAGS) {
//        COMPlusThrowArgumentException(L"flags", L"Argument_InvalidFlag");
//    }

lExit: ;
    HELPER_METHOD_FRAME_END();
    return iRetVal;

}
FCIMPLEND

FCIMPL5(INT32, COMNlsInfo::LastIndexOfStringOrdinalIgnoreCase,
                    INT_PTR ptr,
                    StringObject* pString1UNSAFE,       // String to search in
                    StringObject* pString2UNSAFE,       // String to find
                    INT32 StartIndex,                   // Index to start at
                    INT32 Count)                        // # of chars to search
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(ptr != NULL);
        PRECONDITION(CheckPointer((LPVOID)ptr));
        PRECONDITION(CheckPointer(pString1UNSAFE));
        PRECONDITION(CheckPointer(pString2UNSAFE));
    } CONTRACTL_END;

    INT32       iRetVal = -1;
    STRINGREF   pString1 = (STRINGREF) pString1UNSAFE;
    STRINGREF   pString2 = (STRINGREF) pString2UNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_2(pString1, pString2);

    //
    //  Get the arguments.
    //
    int startIndex = StartIndex;
    int count = Count;
    int stringLength1 = pString1->GetStringLength();
    int findLength = pString2->GetStringLength();

    WCHAR *buffString = NULL;
    WCHAR *buffFind = NULL;

    //
    //  Check the ranges.
    //

    _ASSERTE(startIndex >= 0 && startIndex <= stringLength1);

    int endIndex = startIndex - count + 1;

    // This also catches startIndex == MAXINT, and MAXINT + 1 == -1 when count == 0.
    _ASSERT(count >= 0 && endIndex >= 0);

    // Check for empty strings
    if (stringLength1 == 0)
    {
        if (findLength == 0)
            iRetVal = 0;
        // else iRetVal = -1 (not found)
        goto lExit;
    }

    //
    //  See if we have an empty string 2.
    //
    if (findLength == 0)
    {
        iRetVal = startIndex;
        goto lExit;
    }

    //
    //  Search for the character in the string.
    buffString = pString1->GetBuffer();
    buffFind = pString2->GetBuffer();

    //If we've never before looked at whether this string has high chars, do so now.
    if (IS_STRING_STATE_UNDETERMINED(pString1->GetHighCharState())) {
        COMString::InternalCheckHighChars(pString1);
    }

    //If we've never before looked at whether this string has high chars, do so now.
    if (IS_STRING_STATE_UNDETERMINED(pString2->GetHighCharState())) {
        COMString::InternalCheckHighChars(pString2);
    }

    //If neither string has high chars, we can use a much faster comparison algorithm.
    if (IS_FAST_INDEX(pString1->GetHighCharState()) && IS_FAST_INDEX(pString2->GetHighCharState()))
    {
        iRetVal = FastLastIndexOfStringInsensitive(buffString, startIndex, endIndex, buffFind, findLength);
    }
    else {
        NativeTextInfo* pNativeTextInfo = (NativeTextInfo*)ptr;
        iRetVal = pNativeTextInfo->LastIndexOfStringOrdinalIgnoreCase(buffString, startIndex, endIndex, buffFind, findLength);
    }

lExit: ;
    HELPER_METHOD_FRAME_END();
    return iRetVal;

}
FCIMPLEND

////////////////////////////////////////////////////////////////////////////
//
//  nativeCreateSortKey
//
////////////////////////////////////////////////////////////////////////////

FCIMPL4(Object*, COMNlsInfo::nativeCreateSortKey, INT_PTR pNativeCompareInfo, StringObject* pStringUNSAFE, INT32 dwFlagsIn, INT32 SortId)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer((LPVOID)pNativeCompareInfo));
        PRECONDITION(CheckPointer(pStringUNSAFE, NULL_OK));
    } CONTRACTL_END;

    STRINGREF   pString     = (STRINGREF) pStringUNSAFE;
    U1ARRAYREF  ResultArray = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, pString);

    //
    //  Make sure there is a string.
    //
    if (!pString) {
        COMPlusThrowArgumentNull(L"string",L"ArgumentNull_String");
    }

    SString ssCultureKey;
    WCHAR* wstr;
    int Length;
    DWORD dwFlags = (LCMAP_SORTKEY | dwFlagsIn);
    int ByteCount = 0;
    NewArrayHolder<BYTE> pByte;

    Length = pString->GetStringLength();

    if (Length==0) {
        //If they gave us an empty string, we're going to create an empty array.
        //This will serve to be less than any other compare string when we call sortkey_compare.
        ResultArray = (U1ARRAYREF)AllocatePrimitiveArray(ELEMENT_TYPE_U1,0);
        goto lExit;
    }

    pString->GetSString(ssCultureKey);
    wstr = (WCHAR *) ssCultureKey.GetUnicode();

    //This just gets the correct size for the table.
    ByteCount = ((NativeCompareInfo*)(pNativeCompareInfo))->MapSortKey(dwFlags, wstr, Length, NULL, 0);

    //A count of 0 indicates that we either had an error or had a zero length string originally.
    if (ByteCount==0) {
        COMPlusThrow(kArgumentException, L"Argument_MustBeString");
    }

    pByte = new BYTE[ByteCount];

#ifdef _DEBUG
    _ASSERTE(((NativeCompareInfo*)(pNativeCompareInfo))->MapSortKey(dwFlags, wstr, Length, pByte, ByteCount) != 0);
#else
    ((NativeCompareInfo*)(pNativeCompareInfo))->MapSortKey(dwFlags, wstr, Length, pByte, ByteCount);
#endif

    ResultArray = (U1ARRAYREF)AllocatePrimitiveArray(ELEMENT_TYPE_U1, ByteCount);
    memcpyNoGCRefs(ResultArray->GetDirectPointerToNonObjectElements(), pByte, ByteCount);

lExit: ;
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(ResultArray);
}
FCIMPLEND

////////////////////////////////////////////////////////////////////////////
//
//  nativeGetGlobalizedHashCode
//
////////////////////////////////////////////////////////////////////////////

FCIMPL4(INT32, COMNlsInfo::nativeGetGlobalizedHashCode, INT_PTR pNativeCompareInfo, StringObject* pStringUNSAFE, INT32 dwFlagsIn, INT32 SortId)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer((LPVOID)pNativeCompareInfo));
        PRECONDITION(CheckPointer(pStringUNSAFE, NULL_OK));
    } CONTRACTL_END;

    INT32     iReturnHash = 0;
    STRINGREF pString     = (STRINGREF) pStringUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_1(pString);

    //
    //  Make sure there is a string.
    //
    if (!pString) {
        COMPlusThrowArgumentNull(L"string",L"ArgumentNull_String");
    }

    WCHAR* wstr;
    int Length;
    DWORD dwFlags = (LCMAP_SORTKEY | dwFlagsIn);
    int ByteCount = 0;

    //
    // Caller has already verified that the string is not of zero length
    //
    Length = pString->GetStringLength();

    //This just gets the correct size for the table.
    ByteCount = ((NativeCompareInfo*)(pNativeCompareInfo))->MapSortKey(dwFlags, (wstr = pString->GetBuffer()), Length, NULL, 0);

    //A count of 0 indicates that we either had an error or had a zero length string originally.
    if (ByteCount==0) {
        COMPlusThrow(kArgumentException, L"Argument_MustBeString");
    }

    // We used to use a NewArrayHolder here, but it turns out that hurts our large # process
    // scalability in ASP.Net hosting scenarios, using the quick bytes instead mostly stack 
    // allocates and ups throughput by 8% in 100 process case, 5% in 1000 process case
    CQuickBytesSpecifySize<MAX_STRING_VALUE * sizeof(WCHAR)> qbBuffer;
    BYTE* pByte = (BYTE*)qbBuffer.AllocThrows(ByteCount);

    //MapSortKey doesn't do anything that could cause GC to occur.
#ifdef _DEBUG
    _ASSERTE(((NativeCompareInfo*)(pNativeCompareInfo))->MapSortKey(dwFlags, wstr, Length, pByte, ByteCount) != 0);
#else
    ((NativeCompareInfo*)(pNativeCompareInfo))->MapSortKey(dwFlags, wstr, Length, pByte, ByteCount);
#endif

    // Ok, lets build the hashcode -- mostly lifted from GetHashCode() in String.cs, for strings.
    int hash1 = 5381;
    int hash2 = hash1;
    LPBYTE pB = pByte;
    BYTE    c;

    while (pB != 0 && *pB != 0) {
        hash1 = ((hash1 << 5) + hash1) ^ *pB;
        c = pB[1];
        if (c == 0)
            break;
        hash2 = ((hash2 << 5) + hash2) ^ c;
        pB += 2;
    }

    iReturnHash = hash1 + (hash2 * 1566083941);

    HELPER_METHOD_FRAME_END();

    return(iReturnHash);
}
FCIMPLEND

FCIMPL1(INT32, COMNlsInfo::nativeGetTwoDigitYearMax, INT32 nValue)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    DWORD dwTwoDigitYearMax = (DWORD) -1;

    HELPER_METHOD_FRAME_BEGIN_RET_0();


#ifndef CAL_ITWODIGITYEARMAX
        #define CAL_ITWODIGITYEARMAX    0x00000030  // two digit year max
#endif // CAL_ITWODIGITYEARMAX
#ifndef CAL_RETURN_NUMBER
        #define CAL_RETURN_NUMBER       0x20000000   // return number instead of string
#endif // CAL_RETURN_NUMBER

    if (WszGetCalendarInfo(
                    LOCALE_USER_DEFAULT,
                    nValue,
                    CAL_ITWODIGITYEARMAX | CAL_RETURN_NUMBER,
                    NULL,
                    0,
                    &dwTwoDigitYearMax) == 0) {
        dwTwoDigitYearMax = (DWORD) -1;
        goto lExit;
    }

lExit: ;
    HELPER_METHOD_FRAME_END();

    return (dwTwoDigitYearMax);
}
FCIMPLEND

FCIMPL0(LONG, COMNlsInfo::nativeGetTimeZoneMinuteOffset)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    TIME_ZONE_INFORMATION timeZoneInfo;

    GetTimeZoneInformation(&timeZoneInfo);

    //
    // In Win32, UTC = local + offset.  So for Pacific Standard Time, offset = 8.
    // In NLS+, Local time = UTC + offset. So for PST, offset = -8.
    // So we have to reverse the sign here.
    //
    return (timeZoneInfo.Bias * -1);
}
FCIMPLEND

FCIMPL0(Object*, COMNlsInfo::nativeGetStandardName)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    STRINGREF refRetVal = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);

    TIME_ZONE_INFORMATION timeZoneInfo;
    GetTimeZoneInformation(&timeZoneInfo);

    refRetVal = COMString::NewString(timeZoneInfo.StandardName);

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL0(Object*, COMNlsInfo::nativeGetDaylightName)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    STRINGREF refRetVal = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);

    TIME_ZONE_INFORMATION timeZoneInfo;
    GetTimeZoneInformation(&timeZoneInfo);
    // Instead of returning null when daylight saving is not used, now we return the same result as the OS.
    //In this case, if daylight saving time is used, the standard name is returned.


    refRetVal = COMString::NewString(timeZoneInfo.DaylightName);

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL0(Object*, COMNlsInfo::nativeGetDaylightChanges)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    I2ARRAYREF pResultArray = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, pResultArray);

    TIME_ZONE_INFORMATION timeZoneInfo;
    DWORD result = GetTimeZoneInformation(&timeZoneInfo);

    if (result == TIME_ZONE_ID_UNKNOWN || timeZoneInfo.DaylightBias == 0
        ) {
        // If daylight saving time is not used in this timezone, return null.
        //
        // Windows NT/2000: TIME_ZONE_ID_UNKNOWN is returned if daylight saving time is not used in
        // the current time zone, because there are no transition dates.
        // If the current timezone uses daylight saving rule, but user unchekced the
        // "Automatically adjust clock for daylight saving changes", the value
        // for DaylightBias will be 0.
        goto lExit;
    }

    pResultArray = (I2ARRAYREF)AllocatePrimitiveArray(ELEMENT_TYPE_I2, 17);

    //
    // The content of timeZoneInfo.StandardDate is 8 words, which
    // contains year, month, day, dayOfWeek, hour, minute, second, millisecond.
    //
    memcpyNoGCRefs(pResultArray->m_Array,
            (LPVOID)&timeZoneInfo.DaylightDate,
            8 * sizeof(INT16));

    //
    // The content of timeZoneInfo.DaylightDate is 8 words, which
    // contains year, month, day, dayOfWeek, hour, minute, second, millisecond.
    //
    memcpyNoGCRefs(((INT16*)pResultArray->m_Array) + 8,
            (LPVOID)&timeZoneInfo.StandardDate,
            8 * sizeof(INT16));

    ((INT16*)pResultArray->m_Array)[16] = (INT16)timeZoneInfo.DaylightBias * -1;

lExit: ;
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(pResultArray);
}
FCIMPLEND

FCIMPL4(FC_CHAR_RET, COMNlsInfo::nativeChangeCaseChar,
    INT32 nLCID, INT_PTR pNativeTextInfo, CLR_CHAR wch, CLR_BOOL bIsToUpper)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer((LPVOID)pNativeTextInfo));
    } CONTRACTL_END;

    //
    // If our character is less than 0x80 and we're in US English locale, we can make certain
    // assumptions that allow us to do this a lot faster.  US English is 0x0409.  The "Invariant
    // Locale" is 0x0.
    //
    if ((nLCID == 0x0409 || nLCID==0x0) && wch < 0x7f) {
        return (bIsToUpper ? ToUpperMapping[wch] : ToLowerMapping[wch]);
    }

    NativeTextInfo* pNativeTextInfoPtr = (NativeTextInfo*)pNativeTextInfo;
    return (pNativeTextInfoPtr->ChangeCaseChar(bIsToUpper, wch));
}
FCIMPLEND

FCIMPL6(void, COMNlsInfo::nativeChangeCaseSurrogate,
    INT_PTR pNativeTextInfo, CLR_CHAR highSurrogate, CLR_CHAR lowSurrogate, __out/*_ecount(1)*/ WCHAR* resultHighSurrogate, __out/*_ecount(1)*/ WCHAR* resultLowSurrogate, CLR_BOOL bIsToUpper)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer((LPVOID)pNativeTextInfo));
        PRECONDITION(CheckPointer(resultHighSurrogate));
        PRECONDITION(CheckPointer(resultLowSurrogate));
    } CONTRACTL_END;

    NativeTextInfo* pNativeTextInfoPtr = (NativeTextInfo*)pNativeTextInfo;
    pNativeTextInfoPtr->ChangeCaseSurrogate(bIsToUpper, highSurrogate, lowSurrogate, resultHighSurrogate, resultLowSurrogate);
}
FCIMPLEND

FCIMPL4(Object*, COMNlsInfo::nativeChangeCaseString, INT32 nLCID, INT_PTR pNativeTextInfo, StringObject* pStringUNSAFE, CLR_BOOL bIsToUpper)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pStringUNSAFE));
        PRECONDITION(CheckPointer((LPVOID)pNativeTextInfo));
    } CONTRACTL_END;

    STRINGREF pResult = NULL;
    STRINGREF pString = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(Frame::FRAME_ATTR_RETURNOBJ, pString, pResult)
        
    pString = ObjectToSTRINGREF(pStringUNSAFE);

    //
    //  Get the length of the string.
    //
    int nLength = pString->GetStringLength();

    //
    //  Check if we have the empty string.
    //
    if (nLength == 0)
    {
        pResult = ObjectToSTRINGREF(pString);
    }
    else
    {
        //
        //  Create the result string.
        //
        pResult = COMString::NewString(nLength);
        LPWSTR pResultStr = pResult->GetBuffer();

        //
        // If we've never before looked at whether this string has high chars, do so now.
        //
        if (IS_STRING_STATE_UNDETERMINED(pString->GetHighCharState()))
        {
            COMString::InternalCheckHighChars(pString);
        }

        //
        // If all of our characters are less than 0x80 and in a USEnglish or Invariant Locale, we can make certain
        // assumptions that allow us to do this a lot faster.
        //

        if ((nLCID == 0x0409 || nLCID==0x0) && IS_FAST_CASING(pString->GetHighCharState()))
        {
            ConvertStringCaseFast(pString->GetBuffer(), pResultStr, nLength, bIsToUpper);
        }
        else
        {
            NativeTextInfo* pNativeTextInfoPtr = (NativeTextInfo*)pNativeTextInfo;
            pNativeTextInfoPtr->ChangeCaseString(bIsToUpper, nLength, pString->GetBuffer(), pResultStr);
        }
        pResult->SetStringLength(nLength);
        pResultStr[nLength] = 0;
    }

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(pResult);
}
FCIMPLEND

FCIMPL2(FC_CHAR_RET, COMNlsInfo::nativeGetTitleCaseChar,
    INT_PTR pNativeTextInfo, CLR_CHAR wch) {
    CONTRACTL
    {
        NOTHROW;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer((LPVOID)pNativeTextInfo));
    } CONTRACTL_END;

    NativeTextInfo* pNativeTextInfoPtr = (NativeTextInfo*)pNativeTextInfo;
    return (pNativeTextInfoPtr->GetTitleCaseChar(wch));
}
FCIMPLEND


//
// nativeGetInvariantTextInfo get or create the invariant text info.
// it initialize the upper and lower casing tables from the OS file l_intl.nls
// initialize the Unicode plane pointers into NULL (no surrogate support).
// and initialize the rest of teh pointers from the default text info object 
// returned from the call m_pCasingTable->GetDefaultNativeTextInfo.
//

FCIMPL0(LPVOID, COMNlsInfo::nativeGetInvariantTextInfo)
{
    CONTRACTL
    {
        THROWS;
        INJECT_FAULT(COMPlusThrowOM());
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    if (!IsUnicodeSystem())
        return NULL;

    PVOID returnValue = m_pInvariantTextInfo;

    
    return returnValue; 
}
FCIMPLEND


/*==========================AllocateDefaultCasingTable==========================
**Action:  A thin wrapper for the casing table functionality.
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/
FCIMPL1(LPVOID, COMNlsInfo::AllocateDefaultCasingTable, INT_PTR byteptr)
{
    CONTRACTL
    {
        THROWS;
        INJECT_FAULT(COMPlusThrowOM());
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer((LPVOID)byteptr));
    } CONTRACTL_END;

    LPVOID rv = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_0();

    if (m_pCasingTable == NULL) {
        MEMORY_REPORT_CONTEXT_SCOPE("NLS");

        // An exception will be throw if OOM happens in the following line.
        NewHolder<CasingTable> pTemp(new CasingTable());

        // Create the default NativeTextInfo.
        if (pTemp->AllocateDefaultTable((LPBYTE)byteptr) == NULL) {
            COMPlusThrowOM();
        }

        // Check if m_pDefaultInstance has been set by another thread before the current thread.
        PVOID result = FastInterlockCompareExchangePointer((LPVOID*)&m_pCasingTable, (LPVOID)pTemp, (LPVOID)NULL);
        if (result == NULL) {
            // Returning NULL means that m_pDefuaultTable was NULL before the FastInterlockCompareExchangePointer().
            // So we beat other threads to get here.  Tell holder to keep the pointer.
            pTemp.SuppressRelease();
        } else {
            // Do nothing here.
            // The holder will clean up the object when it goes out the scope.
        }
    }
    rv = (LPVOID)m_pCasingTable->GetDefaultNativeTextInfo();
    HELPER_METHOD_FRAME_END();

    return rv;
}
FCIMPLEND

/*=============================AllocateCasingTable==============================
**Action: This is a thin wrapper for the CasingTable that allows us not to have
**        additional .h files.
**Returns:
**Arguments:
**Exceptions:
**
** LOCK CONTRACT: NOT NEEDED.
**
**  The NativeTextInfo is cached in a table indexed by the exceptionIndex.
**  The method uses InterlockCompareExchange() to prevent multiple threads
**  to store in the same slot.
**
==============================================================================*/

FCIMPL2(LPVOID, COMNlsInfo::AllocateCasingTable, INT_PTR bytePtr, INT32 exceptionIndex)
{
    CONTRACTL
    {
        THROWS;
        INJECT_FAULT(COMPlusThrowOM());
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(m_pCasingTable));
    } CONTRACTL_END;

    NativeTextInfo* pNativeTextInfo = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_0();

    _ASSERTE(m_pCasingTable != NULL);
    pNativeTextInfo = m_pCasingTable->InitializeNativeTextInfo((LPBYTE)bytePtr, exceptionIndex);

    if (pNativeTextInfo == NULL)
    {
        COMPlusThrowOM();
    }

    HELPER_METHOD_FRAME_END();

    return pNativeTextInfo;
}
FCIMPLEND

/*================================GetCaseInsHash================================
**Action:
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/
FCIMPL2(INT32, COMNlsInfo::GetCaseInsHash, LPVOID pvStrA, void *pNativeTextInfoPtr)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pNativeTextInfoPtr));
        PRECONDITION(CheckPointer(pvStrA));
    } CONTRACTL_END;

    _ASSERTE(pvStrA!=NULL);
    STRINGREF strA;
    INT32 highCharState;
    INT32 result;

    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), FCThrow(kStackOverflowException))
 
    *((LPVOID *)&strA)=pvStrA;
    LPCWSTR pCurrStr = (LPCWSTR)strA->GetBuffer();

    //
    // Check the high-char state of the string.  Mark this on the String for
    // future use.
    //
    if (IS_STRING_STATE_UNDETERMINED(highCharState=strA->GetHighCharState())) {
        COMString::InternalCheckHighChars(strA);
        highCharState=strA->GetHighCharState();
    }

    //
    // If we know that we don't have any high characters (the common case) we can
    // call a hash function that knows how to do a very fast case conversion.  If
    // we find characters above 0x80, it's much faster to convert the entire string
    // to Uppercase and then call the standard hash function on it.
    //
    if (IS_FAST_CASING((UINT32) highCharState)) {
        result = HashiStringKnownLower80(pCurrStr);
    } else {
        MEMORY_REPORT_CONTEXT_SCOPE("NLS");

        CQuickBytes newBuffer;
        INT32 length = strA->GetStringLength();
        WCHAR *pNewStr = (WCHAR *)newBuffer.AllocThrows((length + 1) * sizeof(WCHAR));
        ((NativeTextInfo*)pNativeTextInfoPtr)->ChangeCaseString(true, length, (LPWSTR)pCurrStr, pNewStr);
        pNewStr[length]='\0';
        result = HashString(pNewStr);
    }
     
    END_SO_INTOLERANT_CODE
    
    return result;
}
FCIMPLEND

/**
 * This function returns a pointer to this table that we use in System.Globalization.EncodingTable.
 * No error checking of any sort is performed.  Range checking is entirely the responsiblity of the managed
 * code.
 */
FCIMPL0(EncodingDataItem *, COMNlsInfo::nativeGetEncodingTableDataPointer)
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    
    return (EncodingDataItem *)EncodingDataTable;
}
FCIMPLEND

/**
 * This function returns a pointer to this table that we use in System.Globalization.EncodingTable.
 * No error checking of any sort is performed.  Range checking is entirely the responsiblity of the managed
 * code.
 */
FCIMPL0(CodePageDataItem *, COMNlsInfo::nativeGetCodePageTableDataPointer)
{
    LEAF_CONTRACT;
    
    STATIC_CONTRACT_SO_TOLERANT;

    return ((CodePageDataItem*) CodePageDataTable);
}
FCIMPLEND

//
// CreateOSCasingTableMemorySection open and map the file %windir%\system32\l_intl.nlp to the memory.
// the file l_intl.nlp contains the OS upper and lower casing tables. these tables get used in the ordinal 
// ignore case string comparison.
//
// Note, we use here the default DACL as it'll be inheritted from the %windir%\system32 folder DACL.
//       Also we open and map the file as read only.
// 
PVOID COMNlsInfo::CreateOSCasingTableMemorySection(DWORD *pFileSize)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        SO_TOLERANT;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    LPVOID mappedData = NULL;
    

    return mappedData;
}


//
// IsUnicodeSystem is a wrapper for OnUnicodeSystem so we can probe for the stack before 
// calling OnUnicodeSystem so the tolerant callers shouldn't care about the probe setting
//
BOOL COMNlsInfo::IsUnicodeSystem()
{
    STATIC_CONTRACT_SO_TOLERANT;

    bool result = FALSE;
    result = OnUnicodeSystem();

    return result;
}

//
// Casing Table Helpers for use in the EE.
//

NativeTextInfo *InternalCasingHelper::pNativeTextInfo=NULL;
void InternalCasingHelper::InitTable() {

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        SO_TOLERANT;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), return; )

    if (!pNativeTextInfo) {
        g_Mscorlib.GetClass(CLASS__TEXT_INFO)->CheckRunClassInitThrowing();
        pNativeTextInfo = COMNlsInfo::m_pCasingTable->GetDefaultNativeTextInfo();
        _ASSERTE(pNativeTextInfo || "Unable to get a casing table for 0x0409.");
    }

    _ASSERTE(pNativeTextInfo || "Somebody has nulled the casing table required for case-insensitive type lookups.");
    
    END_SO_INTOLERANT_CODE
}

// Convert szIn to lower case in the Invariant locale.
INT32 InternalCasingHelper::InvariantToLower(LPUTF8 szOut, int cMaxBytes, LPCUTF8 szIn)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        SO_TOLERANT;
        INJECT_FAULT(COMPlusThrowOM());
    } CONTRACTL_END

    return InvariantToLowerHelper(szOut, cMaxBytes, szIn, TRUE /*fAllowThrow*/);
}

// Convert szIn to lower case in the Invariant locale.
INT32 InternalCasingHelper::InvariantToLowerNoThrow(LPUTF8 szOut, int cMaxBytes, LPCUTF8 szIn)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
        INJECT_FAULT(return 0;);
    } CONTRACTL_END


    return InvariantToLowerHelper(szOut, cMaxBytes, szIn, FALSE /*fAllowThrow*/);
}

// Convert szIn to lower case in the Invariant locale.
INT32 InternalCasingHelper::InvariantToLowerHelper(LPUTF8 szOut, int cMaxBytes, LPCUTF8 szIn, BOOL fAllowThrow)
{

    CONTRACTL {
        // This fcn can trigger a lazy load of the TextInfo class.
        if (fAllowThrow) THROWS; else NOTHROW;
        if (fAllowThrow) GC_TRIGGERS; else GC_NOTRIGGER;
        if (fAllowThrow) {INJECT_FAULT(COMPlusThrowOM());} else {INJECT_FAULT(return 0);}
        MODE_ANY;
        SO_TOLERANT;

        // If the function isn't allowed to throw, the caller is obliged to have
        // called InitTables() already.
        PRECONDITION(fAllowThrow || pNativeTextInfo);

        PRECONDITION((cMaxBytes == 0) || CheckPointer(szOut));
        PRECONDITION(CheckPointer(szIn));
    } CONTRACTL_END

    int inLength = (int)(strlen(szIn)+1);
    INT32 result = 0;

    LPCUTF8 szInSave = szIn;
    LPUTF8 szOutSave = szOut;
    BOOL bFoundHighChars=FALSE;
    //Compute our end point.
    LPCUTF8 szEnd;
    INT32 wideCopyLen;
    
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), ; )
        
    CQuickBytes qbOut;
    LPWSTR szWideOut;

    if (cMaxBytes) {
        szEnd = szOut + min(inLength, cMaxBytes);
        //Walk the string copying the characters.  Change the case on
        //any character between A-Z.
        for (; szOut<szEnd; szOut++, szIn++) {
            if (*szIn>='A' && *szIn<='Z') {
                *szOut = *szIn | 0x20;
            }
            else {
                if (((UINT32)(*szIn))>((UINT32)0x80)) {
                    bFoundHighChars = TRUE;
                    break;
                }
                *szOut = *szIn;
            }
        }

        if (!bFoundHighChars) {
            //If we copied everything, tell them how many bytes we copied,
            //and arrange it so that the original position of the string + the returned
            //length gives us the position of the null (useful if we're appending).
            if (--inLength > cMaxBytes) {
                if (fAllowThrow) {
                    COMPlusThrowHR(HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER));
                }
                SetLastError(ERROR_INSUFFICIENT_BUFFER);
                result = 0;
                goto Exit;
            }

            result = inLength;
            goto Exit;
        }
    }
    else {
        szEnd = szIn + inLength;
        for (; szIn<szEnd; szIn++) {
            if (((UINT32)(*szIn))>((UINT32)0x80)) {
                bFoundHighChars = TRUE;
                break;
            }
        }

        if (!bFoundHighChars) {
            result = inLength;
            goto Exit;
        }
    }

    szOut = szOutSave;

    //convert the UTF8 to Unicode
    //MAKE_WIDEPTR_FROMUTF8(szInWide, szInSave);

    int __lszInWide; 
    LPWSTR szInWide; 
    __lszInWide = WszMultiByteToWideChar(CP_UTF8, 0, szInSave, -1, 0, 0); 
    if (__lszInWide > MAKE_MAX_LENGTH) 
         RaiseException(EXCEPTION_INT_OVERFLOW, EXCEPTION_NONCONTINUABLE, 0, 0); 
    szInWide = (LPWSTR) alloca(__lszInWide*sizeof(WCHAR));  
    if (szInWide == NULL) {
        if (fAllowThrow) {
            COMPlusThrowOM();
        } else {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            result = 0;
            goto Exit;
        }
    }
    if (0==WszMultiByteToWideChar(CP_UTF8, 0, szInSave, -1, szInWide, __lszInWide)) { 
        RaiseException(ERROR_NO_UNICODE_TRANSLATION, EXCEPTION_NONCONTINUABLE, 0, 0); 
    } 

    
    wideCopyLen = (INT32)wcslen(szInWide)+1;
    if (fAllowThrow) {
        szWideOut = (LPWSTR)qbOut.AllocThrows(wideCopyLen * sizeof(WCHAR));
    }
    else {
        szWideOut = (LPWSTR)qbOut.AllocNoThrow(wideCopyLen * sizeof(WCHAR));
        if (!szWideOut) {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            result = 0;
            goto Exit;
        }
    }

    if (fAllowThrow) {
        InitTable();
    }

    //Do the casing operation
    pNativeTextInfo->ChangeCaseString(FALSE/*IsToUpper*/, wideCopyLen, szInWide, szWideOut);

    //Convert the Unicode back to UTF8
    result = WszWideCharToMultiByte(CP_UTF8, 0, szWideOut, wideCopyLen, szOut, cMaxBytes, NULL, NULL);

    if ((result == 0) && fAllowThrow) {
        COMPlusThrowWin32();
    }

Exit: {}
    
    END_SO_INTOLERANT_CODE;

    return result;
}

// Compare case insensitive in the Invariant locale.
INT32 InternalCasingHelper::InvariantCaseInsensitiveCompare(__in_ecount(Length1) const WCHAR *string1, int Length1, __in_ecount(Length2) const WCHAR *string2, int Length2)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
        INJECT_FAULT(return 0;);

        // Must have called Init()
        PRECONDITION(pNativeTextInfo != NULL);

    } CONTRACTL_END

    return pNativeTextInfo->CompareOrdinalIgnoreCase(const_cast<WCHAR*>(string1), Length1, 
                                                     const_cast<WCHAR*>(string2), Length2);
}


//
// Normalization
//

FCIMPL0(FC_BOOL_RET, COMNlsInfo::nativeLoadNormalizationDLL)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    // Initialize all statics one by one so that eventual race with different thread will be harmless

    if (m_hNormalization == NULL)
    {
        HMODULE hNormalization = NULL;
        if (FAILED(LoadLibraryShim(NORMALIZATION_DLL, NULL, NULL, &hNormalization)))
        {
            FC_RETURN_BOOL(FALSE);
        }
        _ASSERTE(hNormalization != NULL);
        m_hNormalization = hNormalization;
    }

    if (m_pfnNormalizationIsNormalizedStringFunc == NULL)
    {
        FARPROC pfn = GetProcAddress(m_hNormalization, "IsNormalizedString");
        if (pfn == NULL)
        {
            FC_RETURN_BOOL(FALSE);
        }
        m_pfnNormalizationIsNormalizedStringFunc = (PFN_NORMALIZATION_IS_NORMALIZED_STRING)pfn;
    }

    if (m_pfnNormalizationNormalizeStringFunc == NULL)
    {
        FARPROC pfn = GetProcAddress(m_hNormalization, "NormalizeString");
        if (pfn == NULL)
        {
            FC_RETURN_BOOL(FALSE);
        }
        m_pfnNormalizationNormalizeStringFunc = (PFN_NORMALIZATION_NORMALIZE_STRING)pfn;
    }

    if (m_pfnNormalizationInitNormalizationFunc == NULL)
    {
        FARPROC pfn = GetProcAddress(m_hNormalization, "InitNormalization");
        if (pfn == NULL)
        {
            FC_RETURN_BOOL(FALSE);
        }
        m_pfnNormalizationInitNormalizationFunc = (PFN_NORMALIZATION_INIT_NORMALIZATION)pfn;
    }

    FC_RETURN_BOOL(TRUE);
}
FCIMPLEND

FCIMPL6(int, COMNlsInfo::nativeNormalizationNormalizeString,
            int NormForm, int& iError,
            StringObject* inChars, int inLength,
            CHARArray* outChars, int outLength )
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(inChars));
        PRECONDITION(CheckPointer(outChars, NULL_OK));
    } CONTRACTL_END;

    // Dereference our string
    STRINGREF inString(inChars);
    LPWSTR inCharsBuffer = inString->GetBuffer();

    // We'll want to return something
    int iResult = 0;

    if (outChars != NULL)
    {
        CHARARRAYREF outCharArray(outChars);
        LPWSTR outCharsBuffer =
            (LPWSTR) (outCharArray->GetDirectPointerToNonObjectElements());

        iResult = m_pfnNormalizationNormalizeStringFunc(
            NormForm, inCharsBuffer, inLength, outCharsBuffer, outLength);
    }
    else
    {
        iResult = m_pfnNormalizationNormalizeStringFunc(
            NormForm, inCharsBuffer, inLength, NULL, 0);
    }

    // Get our error if necessary
    if (iResult <= 0)
    {
        // if the length is <= 0 there was an error
        iError = GetLastError();

        // Go ahead and return positive lengths/indexes so we don't get confused
        iResult = -iResult;
    }
    else
    {
        iError = 0; // ERROR_SUCCESS
    }

    return iResult;
}
FCIMPLEND

FCIMPL4( int, COMNlsInfo::nativeNormalizationIsNormalizedString,
            int NormForm, int& iError,
            StringObject* chars, int inLength )
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(chars));
    } CONTRACTL_END;

    STRINGREF inString(chars);
    LPWSTR charsBuffer = inString->GetBuffer();

    // Ask if its normalized
    bool bResult = m_pfnNormalizationIsNormalizedStringFunc( NormForm, charsBuffer, inLength);

    // May need an error
    if (bResult == false)
    {
        // If its false there may have been an error
        iError = GetLastError();
    }
    else
    {
        iError = 0; // ERROR_SUCCESS
    }

    return (bResult == true) ? -1 : 0;
}
FCIMPLEND

FCIMPL2( BYTE*, COMNlsInfo::nativeNormalizationInitNormalization,
            int NormForm, BYTE* pTableData )
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pTableData));
    } CONTRACTL_END;

    BYTE* pResult = m_pfnNormalizationInitNormalizationFunc( NormForm, pTableData);

    return (pResult);
}
FCIMPLEND

FCIMPL0(INT32, COMNlsInfo::nativeGetCurrentCalendar)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    FCUnique(0x18);
    // Do nothing here.
    // In a ROTOR build, we will always use the built-in value from our table.
    return (0);
}
FCIMPLEND


//
// This table should be sorted using case-insensitive ordinal order.
// In the managed code, String.CompareStringOrdinalWC() is used to sort this.


/**
 * This function returns the number of items in EncodingDataTable.
 */
FCIMPL0(INT32, COMNlsInfo::nativeGetNumEncodingItems)
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    
    return (m_nEncodingDataTableItems);
}
FCIMPLEND

////////////////////////////////////////////////////////////////////////
//
// Return a String object which contains the path for Windows directory.
//
////////////////////////////////////////////////////////////////////////

FCIMPL0(Object*, COMNlsInfo::nativeGetWindowsDirectory)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;


    STRINGREF refRetVal = NULL;

    // ROTOR does not need this function since replacment culture is not supported in ROTOR.
    FCUnique(0x61);

    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

////////////////////////////////////////////////////////////////////////
//
// Check if a file exists.  A folder is not considered to be a file.
//
//  Returns:
//      TRUE if the specified file exists.
//      Otherwise, FALSE is returned.
//
////////////////////////////////////////////////////////////////////////

FCIMPL1(FC_BOOL_RET, COMNlsInfo::nativeFileExists, StringObject* pFileNameUNSAFE)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        PRECONDITION(CheckPointer(pFileNameUNSAFE));
        SO_TOLERANT;
    } CONTRACTL_END;

    // ROTOR does not need this function since replacment culture is not supported in ROTOR.
    FCUnique(0x62);
    FC_RETURN_BOOL(FALSE);
    
}
FCIMPLEND

////////////////////////////////////////////////////////////////////////
//
// Get the static int array data used by globalization classes.
//
// Parameters:
//      dataTableType: The type of the table to be retrieved.  The available types
//            are defined in COMNlsInfo.h. Search "INT32TABLE_".
// Return:
//      The pointer to the int array.
//
////////////////////////////////////////////////////////////////////////

FCIMPL2(CONST INT32 *, COMNlsInfo::nativeGetStaticInt32DataTable, INT32 dataTableType, INT* pDataTableSize)
{
    CONTRACTL
    {
        NOTHROW;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        PRECONDITION(dataTableType >= INT32TABLE_FIRST_TABLE_ITEM && dataTableType <= INT32TABLE_LAST_TABLE_ITEM);
        PRECONDITION(CheckPointer(pDataTableSize));
        SO_TOLERANT;
    } CONTRACTL_END;

    CONST INT* ptr = NULL;
    switch (dataTableType)
    {
        case INT32TABLE_EVERETT_REGION_DATA_ITEM_MAPPINGS:
            ptr = (m_nEverettRegionDataItemMappings);
            *pDataTableSize = m_nEverettRegionDataItemMappingsSize;
            break;
            
        case INT32TABLE_EVERETT_CULTURE_DATA_ITEM_MAPPINGS:
            ptr = (m_nEverettCultureDataItemMappings);
            *pDataTableSize = m_nEverettCultureDataItemMappingsSize;
            break;
            
        case INT32TABLE_EVERETT_DATA_ITEM_TO_LCID_MAPPINGS:
            ptr = (m_nEverettDataItemToLCIDMappings);
            *pDataTableSize = m_nEverettCultureDataItemMappingsSize;
            break;

        case INT32TABLE_EVERETT_REGION_DATA_ITEM_TO_LCID_MAPPINGS:
            ptr = (m_nEverettRegionInfoDataItemToLCIDMappings);
            *pDataTableSize = m_nEverettRegionInfoDataItemToLCIDMappingsSize;
            break;
            
        default:
            _ASSERTE(!"Invalid dataTableType");
            *pDataTableSize = 0;
            break;
    }
    return (ptr);
}
FCIMPLEND

