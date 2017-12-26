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
//*****************************************************************************
// NamespaceUtil.cpp
//
// Helpers for converting namespace separators.
//
//*****************************************************************************
#include "stdafx.h" 
#include "corhdr.h"
#include "corhlpr.h"
#include "sstring.h"
#include "utilcode.h"

#ifndef _ASSERTE
#define _ASSERTE(foo)
#endif

#include "nsutilpriv.h"

// This macro expands into the list of inalid chars to case on.
// You must define _T_TYPE to handle wide vs narrow chars.
#define INVALID_CHAR_LIST() \
        case _T_TYPE('/') : \
        case _T_TYPE('\\') :


//*****************************************************************************
// Determine how many chars large a fully qualified name would be given the
// two parts of the name.  The return value includes room for every character
// in both names, as well as room for the separator and a final terminator.
//*****************************************************************************
int ns::GetFullLength(                  // Number of chars in full name.
    const WCHAR *szNameSpace,           // Namspace for value.
    const WCHAR *szName)                // Name of value.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    int iLen = 1;                       // Null terminator.
    if (szNameSpace)
        iLen += (int)wcslen(szNameSpace);
    if (szName)
        iLen += (int)wcslen(szName);
    if (szNameSpace && *szNameSpace && szName && *szName)
        ++iLen;
    return iLen;
}   //int ns::GetFullLength()

int ns::GetFullLength(                  // Number of chars in full name.
    LPCUTF8     szNameSpace,            // Namspace for value.
    LPCUTF8     szName)                 // Name of value.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;


    int iLen = 1;
    if (szNameSpace)
        iLen += (int)strlen(szNameSpace);
    if (szName)
        iLen += (int)strlen(szName);
    if (szNameSpace && *szNameSpace && szName && *szName)
        ++iLen;
    return iLen;
}   //int ns::GetFullLength()


//*****************************************************************************
// Scan the given string to see if the name contains any invalid characters
// that are not allowed.
//*****************************************************************************
#undef _T_TYPE
#define _T_TYPE(x) L ## x
int ns::IsValidName(                    // true if valid, false invalid.
    const WCHAR *szName)                // Name to parse.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    for (const WCHAR *str=szName; *str;  str++)
    {
        switch (*str)
        {
        INVALID_CHAR_LIST();
        return false;
        }
    }
    return true;
}   //int ns::IsValidName()

#undef _T_TYPE
#define _T_TYPE
int ns::IsValidName(                    // true if valid, false invalid.
    LPCUTF8     szName)                 // Name to parse.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;


    for (LPCUTF8 str=szName; *str;  str++)
    {
        switch (*str)
        {
        INVALID_CHAR_LIST();
        return false;
        }
    }
    return true;
}   //int ns::IsValidName()


//*****************************************************************************
// Scan the string from the rear looking for the first valid separator.  If
// found, return a pointer to it.  Else return null.  This code is smart enough
// to skip over special sequences, such as:
//      a.b..ctor
//         ^
//         |
// The ".ctor" is considered one token.
//*****************************************************************************
WCHAR *ns::FindSep(                     // Pointer to separator or null.
    const WCHAR *szPath)                // The path to look in.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    _ASSERTE(szPath);
    WCHAR *ptr = (WCHAR*)wcsrchr(szPath, NAMESPACE_SEPARATOR_WCHAR);
    if((ptr == NULL) || (ptr == szPath)) return NULL;
    if(*(ptr - 1) == NAMESPACE_SEPARATOR_WCHAR) // here ptr is at least szPath+1
        --ptr;
    return ptr;
}   //WCHAR *ns::FindSep()

LPUTF8 ns::FindSep(                     // Pointer to separator or null.
    LPCUTF8     szPath)                 // The path to look in.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    _ASSERTE(szPath);
    LPUTF8 ptr = const_cast<LPUTF8>(strrchr(szPath, NAMESPACE_SEPARATOR_CHAR));
    if((ptr == NULL) || (ptr == szPath)) return NULL;
    if(*(ptr - 1) == NAMESPACE_SEPARATOR_CHAR) // here ptr is at least szPath+1
        --ptr;
    return ptr;
}   //LPUTF8 ns::FindSep()



//*****************************************************************************
// Take a path and find the last separator (nsFindSep), and then replace the
// separator with a '\0' and return a pointer to the name.  So for example:
//      a.b.c
// becomes two strings "a.b" and "c" and the return value points to "c".
//*****************************************************************************
WCHAR *ns::SplitInline(                 // Pointer to name portion.
    __inout __inout_z WCHAR       *szPath)           // The path to split.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    WCHAR *ptr = ns::FindSep(szPath);
    if (ptr)
    {
        *ptr = 0;
        ++ptr;
    }
    return ptr;
}   // WCHAR *ns::SplitInline()

LPUTF8 ns::SplitInline(                 // Pointer to name portion.
    __inout __inout_z LPUTF8  szPath)                 // The path to split.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    LPUTF8 ptr = ns::FindSep(szPath);
    if (ptr)
    {
        *ptr = 0;
        ++ptr;
    }
    return ptr;
}   // LPUTF8 ns::SplitInline()

void ns::SplitInline(
    __inout __inout_z LPWSTR  szPath,                 // Path to split.
    LPCWSTR     &szNameSpace,           // Return pointer to namespace.
    LPCWSTR     &szName)                // Return pointer to name.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    WCHAR *ptr = SplitInline(szPath);
    if (ptr)
    {
        szNameSpace = szPath;
        szName = ptr;
    }
    else
    {
        szNameSpace = 0;
        szName = szPath;
    }
}   // void ns::SplitInline()

void ns::SplitInline(
    __inout __inout_z LPUTF8  szPath,                 // Path to split.
    LPCUTF8     &szNameSpace,           // Return pointer to namespace.
    LPCUTF8     &szName)                // Return pointer to name.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    LPUTF8 ptr = SplitInline(szPath);
    if (ptr)
    {
        szNameSpace = szPath;
        szName = ptr;
    }
    else
    {
        szNameSpace = 0;
        szName = szPath;
    }
}   // void ns::SplitInline()


//*****************************************************************************
// Split the last parsable element from the end of the string as the name,
// the first part as the namespace.
//*****************************************************************************
int ns::SplitPath(                      // true ok, false trunction.
    const WCHAR *szPath,                // Path to split.
    __out_ecount(cchNameSpace) WCHAR *szNameSpace,           // Output for namespace value.
    int         cchNameSpace,           // Max chars for output.
    __out_ecount(cchName)      WCHAR *szName,                // Output for name.
    int         cchName)                // Max chars for output.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    const WCHAR *ptr = ns::FindSep(szPath);
    size_t iLen = (ptr) ? ptr - szPath : 0;
    size_t iCopyMax;
    int brtn = true;
    if (szNameSpace && cchNameSpace)
    {
        _ASSERTE(cchNameSpace > 1);    
        iCopyMax = cchNameSpace - 1;
        iCopyMax = min(iCopyMax, iLen);
        wcsncpy_s(szNameSpace, cchNameSpace, szPath, iCopyMax);
        szNameSpace[iCopyMax] = 0;
        
        if (iLen >= (size_t)cchNameSpace)
            brtn = false;
    }

    if (szName && cchName)
    {
        _ASSERTE(cchName > 1);    
        iCopyMax = cchName - 1;
        if (ptr)
            ++ptr;
        else
            ptr = szPath;
        iLen = (int)wcslen(ptr);
        iCopyMax = min(iCopyMax, iLen);
        wcsncpy_s(szName, cchName, ptr, iCopyMax);
        szName[iCopyMax] = 0;
    
        if (iLen >= (size_t)cchName)
            brtn = false;
    }
    return brtn;
}   // int ns::SplitPath()


int ns::SplitPath(                      // true ok, false trunction.
    LPCUTF8     szPath,                 // Path to split.
    __out_ecount_opt (cchNameSpace) LPUTF8      szNameSpace,            // Output for namespace value.
    int         cchNameSpace,           // Max chars for output.
    __out_ecount_opt (cchName) LPUTF8      szName,                 // Output for name.
    int         cchName)                // Max chars for output.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    LPCUTF8 ptr = ns::FindSep(szPath);
    size_t iLen = (ptr) ? ptr - szPath : 0;
    size_t iCopyMax;
    int brtn = true;
    if (szNameSpace && cchNameSpace)
    {
        _ASSERTE(cchNameSpace > 1);    
        iCopyMax = cchNameSpace-1;
        iCopyMax = min(iCopyMax, iLen);
        strncpy_s(szNameSpace, cchNameSpace, szPath, iCopyMax);
        szNameSpace[iCopyMax] = 0;
        
        if (iLen >= (size_t)cchNameSpace)
            brtn = false;
    }

    if (szName && cchName)
    {
        _ASSERTE(cchName > 1);    
        iCopyMax = cchName-1;
        if (ptr)
            ++ptr;
        else
            ptr = szPath;
        iLen = (int)strlen(ptr);
        iCopyMax = min(iCopyMax, iLen);
        strncpy_s(szName, cchName, ptr, iCopyMax);
        szName[iCopyMax] = 0;
    
        if (iLen >= (size_t)cchName)
            brtn = false;
    }
    return brtn;
}   // int ns::SplitPath()


//*****************************************************************************
// Take two values and put them together in a fully qualified path using the
// correct separator.
//*****************************************************************************
int ns::MakePath(                       // true ok, false truncation.
    __out_ecount(cchChars) WCHAR       *szOut,                 // output path for name.
    int         cchChars,               // max chars for output path.
    const WCHAR *szNameSpace,           // Namespace.
    const WCHAR *szName)                // Name.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    if (cchChars < 1)
        return false;

    if (szOut)
        *szOut = 0;
    else
        return false;
        
    if (szNameSpace && *szNameSpace != L'\0')
    {
        if (wcsncpy_s(szOut, cchChars, szNameSpace, _TRUNCATE) == STRUNCATE)
            return false;

        // Add namespace separator if a non-empty name was supplied
        if (szName && *szName != L'\0')
        {
            if (wcsncat_s(szOut, cchChars, NAMESPACE_SEPARATOR_WSTR, _TRUNCATE) == STRUNCATE)
            {
                return false;
            }
        }
    }
    
    if (szName && *szName)
    {
        if (wcsncat_s(szOut, cchChars, szName, _TRUNCATE) == STRUNCATE)
            return false;
    }
    
    return true;
}   // int ns::MakePath()

int ns::MakePath(                       // true ok, false truncation.
    __out_ecount(cchChars) LPUTF8      szOut,                  // output path for name.
    int         cchChars,               // max chars for output path.
    LPCUTF8     szNameSpace,            // Namespace.
    LPCUTF8     szName)                 // Name.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    if (cchChars < 1)
        return false;

    if (szOut)
        *szOut = 0;
    else
        return false;
        
    if (szNameSpace && *szNameSpace != L'\0')
    {
        if (strncpy_s(szOut, cchChars, szNameSpace, _TRUNCATE) == STRUNCATE)
            return false;

        // Add namespace separator if a non-empty name was supplied
        if (szName && *szName != L'\0')
        {
            if (strncat_s(szOut, cchChars, NAMESPACE_SEPARATOR_STR, _TRUNCATE) == STRUNCATE)
            {
                return false;
            }
        }
    }
    
    if (szName && *szName)
    {
        if (strncat_s(szOut, cchChars, szName, _TRUNCATE) == STRUNCATE)
            return false;
    }
    
    return true;

}   // int ns::MakePath()

int ns::MakePath(                       // true ok, false truncation.
    __out_ecount(cchChars) WCHAR       *szOut,                 // output path for name.
    int         cchChars,               // max chars for output path.
    LPCUTF8     szNamespace,            // Namespace.
    LPCUTF8     szName)                 // Name.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    if (cchChars < 1)
        return false;

    if (szOut)
        *szOut = 0;
    else
        return false;
    
    if (szNamespace != NULL && *szNamespace != '\0')
    {
        if (cchChars < 2)
            return false;

        int count;

        // We use cBuffer - 2 to account for the '.' and at least a 1 character name below.
        count = WszMultiByteToWideChar(CP_UTF8, 0, szNamespace, -1, szOut, cchChars-2);
        if (count == 0)
            return false; // Supply a bigger buffer!

        szOut[count-1] = NAMESPACE_SEPARATOR_WCHAR;
        szOut += count;
        cchChars -= count;
    }

    if (((cchChars == 0) && (szName != NULL) && (*szName != '\0')) || 
        (WszMultiByteToWideChar(CP_UTF8, 0, szName, -1, szOut, cchChars) == 0))
        return false; // supply a bigger buffer!
    return true;
}   // int ns::MakePath()

int ns::MakePath(                       // true ok, false out of memory
    CQuickBytes &qb,                    // Where to put results.
    LPCUTF8     szNameSpace,            // Namespace for name.
    LPCUTF8     szName)                 // Final part of name.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FAULT;

    int iLen = 2;
    if (szNameSpace)
        iLen += (int)strlen(szNameSpace);
    if (szName)
        iLen += (int)strlen(szName);
    LPUTF8 szOut = (LPUTF8) qb.AllocNoThrow(iLen);
    if (!szOut)
        return false;
    return ns::MakePath(szOut, iLen, szNameSpace, szName);
}   // int ns::MakePath()

int ns::MakePath(                       // true ok, false out of memory
    CQuickArray<WCHAR> &qa,             // Where to put results.
    LPCUTF8            szNameSpace,     // Namespace for name.
    LPCUTF8            szName)          // Final part of name.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FAULT;

    int iLen = 2;
    if (szNameSpace)
        iLen += (int)strlen(szNameSpace);
    if (szName)
        iLen += (int)strlen(szName);
    WCHAR *szOut = (WCHAR *) qa.AllocNoThrow(iLen);
    if (!szOut)
        return false;
    return ns::MakePath(szOut, iLen, szNameSpace, szName);
}   // int ns::MakePath()

int ns::MakePath(                       // true ok, false out of memory
    CQuickBytes &qb,                    // Where to put results.
    const WCHAR *szNameSpace,           // Namespace for name.
    const WCHAR *szName)                // Final part of name.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FAULT;

    int iLen = 2;
    if (szNameSpace)
        iLen += (int)wcslen(szNameSpace);
    if (szName)
        iLen += (int)wcslen(szName);
    WCHAR *szOut = (WCHAR *) qb.AllocNoThrow(iLen * sizeof(WCHAR));
    if (!szOut)
        return false;
    return ns::MakePath(szOut, iLen, szNameSpace, szName);
}   // int ns::MakePath()

void ns::MakePath(                      // throws on out of memory
    SString       &ssBuf,               // Where to put results.
    const SString &ssNameSpace,         // Namespace for name.
    const SString &ssName)              // Final part of name.
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FAULT;

    ssBuf.Clear();

    if (!ssNameSpace.IsEmpty())
    {
        if (ssName.IsEmpty())
        {
            ssBuf.Set(ssNameSpace);
        }
        else
        {
            ssBuf.Set(ssNameSpace, SL(NAMESPACE_SEPARATOR_WSTR));
        }
    }

    if (!ssName.IsEmpty())
    {
        ssBuf.Append(ssName);
    }
}

bool ns::MakeAssemblyQualifiedName(                                        // true ok, false truncation
                                   __out_ecount(dwBuffer) WCHAR* pBuffer,  // Buffer to recieve the results
                                   int    dwBuffer,                        // Number of characters total in buffer
                                   const WCHAR *szTypeName,                // Namespace for name.
                                   int   dwTypeName,                       // Number of characters (not including null)
                                   const WCHAR *szAssemblyName,            // Final part of name.
                                   int   dwAssemblyName)                   // Number of characters (not including null)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    if (dwBuffer < 2)
        return false;

    int iCopyMax = 0;
    _ASSERTE(pBuffer);
    *pBuffer = NULL;
    
    if (szTypeName && *szTypeName != L'\0')
    {
        _ASSERTE(dwTypeName > 0);
        iCopyMax = min(dwBuffer-1, dwTypeName);
        wcsncpy_s(pBuffer, dwBuffer, szTypeName, iCopyMax);
        dwBuffer -= iCopyMax;
    }
    
    if (szAssemblyName && *szAssemblyName != L'\0')
    {
        
        if(dwBuffer < ASSEMBLY_SEPARATOR_LEN) 
            return false;

        for(DWORD i = 0; i < ASSEMBLY_SEPARATOR_LEN; i++)
            pBuffer[iCopyMax+i] = ASSEMBLY_SEPARATOR_WSTR[i];

        dwBuffer -= ASSEMBLY_SEPARATOR_LEN;
        if(dwBuffer == 0) 
            return false;

        int iCur = iCopyMax + ASSEMBLY_SEPARATOR_LEN;
        _ASSERTE(dwAssemblyName > 0);
        iCopyMax = min(dwBuffer-1, dwAssemblyName);
        wcsncpy_s(pBuffer + iCur, dwBuffer, szAssemblyName, iCopyMax);
        pBuffer[iCur + iCopyMax] = L'\0';
        
        if (iCopyMax < dwAssemblyName)
            return false;
    }
    else {
        if(dwBuffer == 0) {
            PREFIX_ASSUME(iCopyMax > 0);
            pBuffer[iCopyMax-1] = L'\0';
            return false;
        }
        else
            pBuffer[iCopyMax] = L'\0';
    }
    
    return true;
}   // int ns::MakePath()

bool ns::MakeAssemblyQualifiedName(                                        // true ok, false out of memory
                                   CQuickBytes &qb,                        // Where to put results.
                                   const WCHAR *szTypeName,                // Namespace for name.
                                   const WCHAR *szAssemblyName)            // Final part of name.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FAULT;

    int iTypeName = 0;
    int iAssemblyName = 0;
    if (szTypeName)
        iTypeName = (int)wcslen(szTypeName);
    if (szAssemblyName)
        iAssemblyName = (int)wcslen(szAssemblyName);

    int iLen = ASSEMBLY_SEPARATOR_LEN + iTypeName + iAssemblyName + 1; // Space for null terminator
    WCHAR *szOut = (WCHAR *) qb.AllocNoThrow(iLen * sizeof(WCHAR));
    if (!szOut)
        return false;

    bool ret;
    ret = ns::MakeAssemblyQualifiedName(szOut, iLen, szTypeName, iTypeName, szAssemblyName, iAssemblyName);
    _ASSERTE(ret);
    return true;
}   

int ns::MakeNestedTypeName(             // true ok, false out of memory
    CQuickBytes &qb,                    // Where to put results.
    LPCUTF8     szEnclosingName,        // Full name for enclosing type
    LPCUTF8     szNestedName)           // Full name for nested type
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FAULT;

    _ASSERTE(szEnclosingName && szNestedName);
    int iLen = 2;
    iLen += (int)strlen(szEnclosingName);
    iLen += (int)strlen(szNestedName);
    LPUTF8 szOut = (LPUTF8) qb.AllocNoThrow(iLen);
    if (!szOut)
        return false;
    return ns::MakeNestedTypeName(szOut, iLen, szEnclosingName, szNestedName);
}   // int ns::MakeNestedTypeName()

int ns::MakeNestedTypeName(             // true ok, false truncation.
    __out_ecount (cchChars) LPUTF8      szOut,                  // output path for name.
    int         cchChars,               // max chars for output path.
    LPCUTF8     szEnclosingName,        // Full name for enclosing type
    LPCUTF8     szNestedName)           // Full name for nested type
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    if (cchChars < 1)
        return false;

    int iCopyMax = 0, iLen;
    int brtn = true;
    *szOut = 0;
    
    iLen = (int)strlen(szEnclosingName);
    iCopyMax = min(cchChars-1, iLen);
    strncpy_s(szOut, cchChars, szEnclosingName, iCopyMax);
    
    if (iLen >= cchChars)
        brtn =  false;

    szOut[iCopyMax] = NESTED_SEPARATOR_CHAR;
    int iCur = iCopyMax+1; // iCopyMax characters + nested_separator_char
    cchChars -= iCur;
    if(cchChars == 0) 
        return false;

    iLen = (int)strlen(szNestedName);
    iCopyMax = min(cchChars-1, iLen);
    strncpy_s(&szOut[iCur], cchChars, szNestedName, iCopyMax);
    szOut[iCur + iCopyMax] = 0;
    
    if (iLen >= cchChars)
        brtn = false;
    
    return brtn;
}   // int ns::MakeNestedTypeName()

void ns::MakeNestedTypeName(            // throws on out of memory
    SString        &ssBuf,              // output path for name.
    const SString  &ssEnclosingName,    // Full name for enclosing type
    const SString  &ssNestedName)       // Full name for nested type
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;

    ssBuf.Clear();

    ssBuf.Append(ssEnclosingName);
    ssBuf.Append(NESTED_SEPARATOR_WCHAR);
    ssBuf.Append(ssNestedName);
}

//*****************************************************************************
// Given a buffer that already contains a namespace, this function appends a
// name onto that buffer with the inclusion of the separator between the two.
// The return value is a pointer to where the separator was written.
//*****************************************************************************
const WCHAR *ns::AppendPath(            // Pointer to start of appended data.
    __inout_ecount(cchMax) WCHAR       *szBasePath, // Current path to append to.
    int         cchMax,                 // Max chars for output buffer, including existing data.
    const WCHAR *szAppend)              // Value to append to existing path.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    _ASSERTE(0 && "nyi");
    return false;
}   // const WCHAR *ns::AppendPath()

LPCUTF8     ns::AppendPath(             // Pointer to start of appended data.
    __inout_ecount(cchMax) LPUTF8      szBasePath,             // Current path to append to.
    int         cchMax,                 // Max chars for output buffer, including existing data.
    LPCUTF8     szAppend)               // Value to append to existing path.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    _ASSERTE(0 && "nyi");
    return false;
}   // LPCUTF8     ns::AppendPath()


//*****************************************************************************
// Given a two sets of name and namespace, this function, compares if the
// concatenation of each sets gives the same full-qualified name.  Instead of
// actually doing the concatenation and making the comparison, this does the
// comparison in a more optimized manner that avoid any kind of allocations.
//*****************************************************************************
bool ns::FullQualNameCmp(               // true if identical, false otherwise.
    LPCUTF8     szNameSpace1,           // NameSpace 1.
    LPCUTF8     szName1,                // Name 1.
    LPCUTF8     szNameSpace2,           // NameSpace 2.
    LPCUTF8     szName2)                // Name 2.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    LPCUTF8     rszType1[3];            // Array of components of Type1.
    LPCUTF8     rszType2[3];            // Array of components of Type2.
    ULONG       ulCurIx1 = 0;           // Current index into Array1.
    ULONG       ulCurIx2 = 0;           // Current index into Array2.
    LPCUTF8     szType1;                // Current index into current string1.
    LPCUTF8     szType2;                // Current index into current string2.
    ULONG       ulFullQualLen1;         // Length of full qualified name1.
    ULONG       ulFullQualLen2;         // Length of full qualified name2.

    // Replace each of the NULLs passed in with empty strings.
    rszType1[0] = szNameSpace1 ? szNameSpace1 : EMPTY_STR;
    rszType1[2] = szName1 ? szName1 : EMPTY_STR;
    rszType2[0] = szNameSpace2 ? szNameSpace2 : EMPTY_STR;
    rszType2[2] = szName2 ? szName2 : EMPTY_STR;

    // Set namespace separators as needed.  Set it to the empty string where
    // not needed.
    rszType1[1] = (*rszType1[0] && *rszType1[2]) ? NAMESPACE_SEPARATOR_STR : EMPTY_STR;
    rszType2[1] = (*rszType2[0] && *rszType2[2]) ? NAMESPACE_SEPARATOR_STR : EMPTY_STR;

    // Compute the full qualified lengths for each Type.
    ulFullQualLen1 = (int)(strlen(rszType1[0]) + strlen(rszType1[1]) + strlen(rszType1[2]));
    ulFullQualLen2 = (int)(strlen(rszType2[0]) + strlen(rszType2[1]) + strlen(rszType2[2]));

    // Fast path, compare Name length.
    if (ulFullQualLen1 != ulFullQualLen2)
        return false;

    // Get the first component of the second type.
    szType2 = rszType2[ulCurIx2];

    // Compare the names.  The logic below assumes that the lengths of the full
    // qualified name are equal for the two names.
    for (ulCurIx1 = 0; ulCurIx1 < 3; ulCurIx1++)
    {
        // Get the current component of the name of the first Type.
        szType1 = rszType1[ulCurIx1];
        // Compare the current component to the second type, grabbing as much
        // of as many components as needed.
        while (*szType1)
        {
            // Get the next non-empty component of the second type.
            while (! *szType2)
                szType2 = rszType2[++ulCurIx2];
            // Compare the current character.
            if (*szType1++ != *szType2++)
                return false;
        }
    }
    return true;
}   // bool ns::FullQualNameCmp()


// Change old namespace separator to new in a string.
void SlashesToDots(__inout __inout_z char* pStr, int ilen)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    if (pStr)
    {
        for (char *pChar = pStr; *pChar; pChar++)
        {
            if (*pChar == '/')
                *pChar = NAMESPACE_SEPARATOR_CHAR;
        }
    }
}   // void SlashesToDots()

// Change old namespace separator to new in a string.
void SlashesToDots(__inout __inout_z WCHAR* pStr, int ilen)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    if (pStr)
    {
        for (WCHAR *pChar = pStr; *pChar; pChar++)
        {
            if (*pChar == L'/')
                *pChar = NAMESPACE_SEPARATOR_WCHAR;
        }
    }
}   // void SlashesToDots()

// Change new namespace separator back to old in a string.
// (Yes, this is actually done)
void DotsToSlashes(__inout __inout_z char* pStr, int ilen)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    if (pStr)
    {
        for (char *pChar = pStr; *pChar; pChar++)
        {
            if (*pChar == NAMESPACE_SEPARATOR_CHAR)
            {
                *pChar = '/';

                // Skip the second character of double occurrences
                if (*(pChar + 1) == NAMESPACE_SEPARATOR_CHAR)
                    pChar++;
            }
        }
    }
}   // void DotsToSlashes()

// Change new namespace separator back to old in a string.
// (Yes, this is actually done)
void DotsToSlashes(__inout __inout_z WCHAR* pStr, int ilen)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    if (pStr)
    {
        for (WCHAR *pChar = pStr; *pChar; pChar++)
        {
            if (*pChar == NAMESPACE_SEPARATOR_WCHAR)
            {
                *pChar = L'/';

                // Skip the second character of double occurrences
                if (*(pChar + 1) == NAMESPACE_SEPARATOR_WCHAR)
                    pChar++;
            }
        }
    }
}   // void DotsToSlashes()
