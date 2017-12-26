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

#ifndef __SOURCELINE_H__
#define __SOURCELINE_H__


#define CComPtr(x) x*

class SourceLine
{
	bool initialized_;


public:
	SourceLine( LPWSTR pszFileName );

	bool IsInitialized() { return initialized_; }

	//
	// Given function token (methoddef) and offset, return filename and line number
	//
	HRESULT GetSourceLine( DWORD dwFunctionToken, DWORD dwOffset, __out_ecount(dwFileNameMaxLen) __out_z LPWSTR wszFileName, DWORD dwFileNameMaxLen, PDWORD pdwLineNumber );
	
	//
	// Given function token (methoddef) and slot, return name of the local variable
	//
	HRESULT GetLocalName( DWORD dwFunctionToken, DWORD dwSlot, __out_ecount(dwNameMaxLen) __out_z LPWSTR wszName, DWORD dwNameMaxLen );
};

#endif // __SOURCELINE_H__
