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

#include "common.h"

#include "sourceline.h"

//////////////////////////////////////////////////////////////////

SourceLine::SourceLine( LPWSTR pszFileName ) 
{
    LEAF_CONTRACT;
    initialized_ = false;
}

HRESULT SourceLine::GetSourceLine( DWORD dwFunctionToken, DWORD dwOffset, __out_ecount(dwFileNameMaxLen) __out_z LPWSTR pszFileName, DWORD dwFileNameMaxLen, PDWORD pdwLineNumber )
{
    LEAF_CONTRACT;
    return E_NOTIMPL;
}

HRESULT SourceLine::GetLocalName( DWORD dwFunctionToken, DWORD dwSlot, __out_ecount(dwNameMaxLen) __out_z LPWSTR pszName, DWORD dwNameMaxLen )
{
    LEAF_CONTRACT;
    return E_NOTIMPL;
}

