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
// File: common.h
//
// standard header for ALink front end, that has common definitions.
// ===========================================================================

#ifndef __common_h__
#define __common_h__

#define MESSAGE_DLLW L"alink.satellite"
#define MESSAGE_DLLA  "alink.satellite"

#define CLIENT_IS_ALINK
#include "cscommon.h"
#undef CLIENT_IS_ALINK
#include "file_can.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
#include <strsafe.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

inline LPWSTR VSAllocStr(LPCWSTR str) {
    size_t cchStr = wcslen(str) + 1;
    WCHAR * p = new WCHAR[cchStr];
    if (p != NULL) {
        StringCchCopyW (p, cchStr, str);
        
    }
    return p;
}

#ifdef _DEBUG
const DWORD * CheckFlags(LPCSTR env_name, LPCSTR name);

#define DEBUG_BEGIN_IF_NZ(sz) \
    { \
        const DWORD * __f = CheckFlags( "ALINK_" sz, sz); \
        if (__f != NULL && *__f != 0) {

#define DEBUG_BEGIN_IF_EQ(sz, comp) \
    { \
        const DWORD * f = CheckFlags( "ALINK_" sz, sz); \
        if (__f != NULL && *__f == comp) {

#define DEBUG_BEGIN_IF_BIT(sz, comp) \
    { \
        const DWORD * f = CheckFlags( "ALINK_" sz, sz); \
        if (__f != NULL && (*__f & comp) == comp) {

#define DEBUG_ELSE } else {

#define DEBUG_END_IF \
        } \
    }

#endif

#endif //__common_h__

