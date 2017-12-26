/****************************** Module Header ******************************\
Module Name:  CppDIADumpSymbol.cpp
Project:      CppDIADumpSymbol
Copyright (c) Microsoft Corporation.



This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes
#include <stdio.h>
#include <tchar.h>
#include <dia2.h>	// Requires the setup of Visual Studio SDK.
#pragma endregion


int _tmain(int argc, _TCHAR* argv[])
{
	HRESULT hr = CoInitialize(NULL);


	/////////////////////////////////////////////////////////////////////////
	// Acquire a data source by creating an IDiaDataSource interface and 
	// opening a program database.
	// 

	IDiaDataSource* pSource;
	hr = CoCreateInstance(__uuidof(DiaSource), NULL, CLSCTX_INPROC_SERVER, 
		__uuidof(IDiaDataSource), (void **)&pSource);
	if (FAILED(hr))
	{
		_tprintf(_T("CoCreateInstance failed w/err 0x%08lx\n"), hr);
		return 1;
	}

	// Open and prepare a program database (.pdb) file as a debug data source
	TCHAR szPdbFile[] = _T("xxx.pdb");
	hr = pSource->loadDataFromPdb(szPdbFile);
	if (FAILED(hr))
	{
		_tprintf(_T("loadDataFromPdb failed w/err 0x%08lx\n"), hr);
		return 1;
	}


	/////////////////////////////////////////////////////////////////////////
	// Call IDiaDataSource::openSession to open an IDiaSession. 
	// 

	// Once the PDB is open, a session is required to be opened, to provide a 
	// query context for debug symbols. 
	IDiaSession* pSession;
	hr = pSource->openSession(&pSession);
	if (FAILED(hr))
	{
		_tprintf(_T("openSession failed w/err 0x%08lx\n"), hr);
		return 1;
	}


	/////////////////////////////////////////////////////////////////////////
	// Use the methods in IDiaSession to query for the symbols (IDiaSymbol) 
	// in the data source.
	// 

	// Find symbols by some conditions
	/*ULONGLONG va = 0x...;
	IDiaSymbol* pFunc;
	pSession->findSymbolByVA(va, SymTagFunction, &pFunc);*/

	// [-or-]

	// Retrieve a reference to the global scope
	IDiaSymbol* pGlobal;
	hr = pSession->get_globalScope(&pGlobal);
	if (FAILED(hr))
	{
		_tprintf(_T("get_globalScope failed w/err 0x%08lx\n"), hr);
		return 1;
	}


	/////////////////////////////////////////////////////////////////////////
	// Use the IDiaEnum* interfaces to enumerate and scan through the symbols 
	// or other elements of debug information.
	// 

	IDiaEnumSymbols pEnumSymbols;
	hr = pGlobal->findChildren(SymTagUDT, NULL, nsNone, &pEnumSymbols);
	if (FAILED(hr))
	{
		_tprintf(_T("findChildren failed w/err 0x%08lx\n"), hr);
		return 1;
	}

	return 0;
}

