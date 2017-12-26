//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//-----------------------------------------------------------------------

// PinvokeLib.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "PinvokeLib.h"
#include <objbase.h>
#include <stdio.h>

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

// This is the constructor of a class that has been exported.
CTestClass::CTestClass()
{ 
	m_member = 1; 
}

int CTestClass::DoSomething( int i )
{
	return i*i + m_member;
}

extern "C" PINVOKELIB_API CTestClass* CreateTestClass()
{
	return new CTestClass();
}

extern "C" PINVOKELIB_API void DeleteTestClass( CTestClass* instance )
{
	delete instance;
}

//******************************************************************

extern "C" PINVOKELIB_API int TestArrayOfInts( int* pArray, int size )
{
	int result = 0;

	for( int i = 0; i < size; i++ )
	{
		result += pArray[ i ];
		pArray[ i ] += 100;
	}
	return result;
}
//******************************************************************

extern "C" PINVOKELIB_API int TestRefArrayOfInts( int** ppArray, int* pSize )
{
	int result = 0;

	// CoTaskMemAlloc must be used instead of new operator
	// since code on managed side will call Marshal.FreeCoTaskMem 
	// to free this memory
	
	int* newArray = (int*)CoTaskMemAlloc( sizeof(int) * 5 );

	for( int i = 0; i < *pSize; i++ )
	{
		result += (*ppArray)[ i ];
	}

	for( int j = 0; j < 5; j++ )
	{
		newArray[ j ] = (*ppArray)[ j ] + 100;
	}

	CoTaskMemFree( *ppArray );
	*ppArray = newArray;
	*pSize = 5;

	return result;
}
//******************************************************************

extern "C" PINVOKELIB_API int TestMatrixOfInts( int pMatrix[][COL_DIM], int row )
{
	int result = 0;

	for( int i = 0; i < row; i++ )
	{
		for( int j = 0; j < COL_DIM; j++ )
		{
			result += pMatrix[ i ][ j ];
			pMatrix[ i ][ j ] += 100;
		}
	}
	return result;
}
//******************************************************************

extern "C" PINVOKELIB_API int TestArrayOfStrings( char* ppStrArray[], int size )
{
	int result = 0;
	char* temp;

	for( int i = 0; i < size; i++ )
	{
		result += (int)strlen( ppStrArray[ i ] );

		temp = (char*)CoTaskMemAlloc( sizeof(char) * 10 );
		strcpy_s( temp, sizeof(char) * 10, "123456789" );

		// CoTaskMemFree must be used instead of delete to free memory

		CoTaskMemFree( ppStrArray[ i ] );
		ppStrArray[ i ] = temp;
	}
	return result;
}
//******************************************************************

extern "C" PINVOKELIB_API int TestArrayOfStructs( MYPOINT* pPointArray, int size )
{
	int result = 0;
	MYPOINT* pCur = pPointArray;

	for( int i = 0; i < size; i++ )
	{
		result += pCur->x + pCur->y;
		pCur->y = 0;
		pCur++;
	}
	return result;
}
//******************************************************************

extern "C" PINVOKELIB_API int TestArrayOfStructs2( MYPERSON* pPersonArray, int size )
{
	int result = 0;
	MYPERSON* pCur = pPersonArray;
	char* temp;

	for( int i = 0; i < size; i++ )
	{
		result += (int)(strlen( pCur->first ) + strlen( pCur->last ));

		temp = (char*)CoTaskMemAlloc( sizeof(char) * ( strlen( pCur->last ) + 6 ));
		strcpy_s( temp, sizeof(char) * ( strlen( pCur->last ) + 6 ), "last:" );
		strcat_s( temp, sizeof(char) * ( strlen( pCur->last ) + 6 ), pCur->last );

		CoTaskMemFree( pCur->last );
		pCur->last = temp;

		pCur++;
	}
	return result;
}

//******************************************************************
extern "C" PINVOKELIB_API int TestStructInStruct( MYPERSON2* pPerson2 )
{
	char* temp = (char*)CoTaskMemAlloc( sizeof(char) * ( strlen( pPerson2->person->last ) + 6 ));
	strcpy_s( temp, sizeof(char) * ( strlen( pPerson2->person->last ) + 6 ), "last:" );
	strcat_s( temp, sizeof(char) * ( strlen( pPerson2->person->last ) + 6 ), pPerson2->person->last );

	CoTaskMemFree( pPerson2->person->last );
	pPerson2->person->last = temp;

	return pPerson2->age;
}

//******************************************************************

extern "C" PINVOKELIB_API void TestStructInStruct3( MYPERSON3 person3 )
{
	printf( "\n\nperson passed by value:\n" );
	printf( "first = %s last = %s age = %i\n\n", 
		person3.person.first, person3.person.last, person3.age );
}

//*********************************************************************

extern "C" PINVOKELIB_API void TestUnion( MYUNION u, int type )
{
	if(( type != 1 ) && ( type != 2 )) return;
	if( type == 1 )
		printf( "\n\ninteger passed: %i", u.i );
	else if( type == 2 )
		printf( "\n\ndouble passed: %f", u.d );
}
//******************************************************************

extern "C" PINVOKELIB_API void TestUnion2( MYUNION2 u, int type )
{
	if(( type != 1 ) && ( type != 2 )) return;
	if( type == 1 )
		printf( "\n\ninteger passed: %i", u.i );
	else if( type == 2 )
		printf( "\n\nstring passed: %s", u.str );
}
//******************************************************************

extern "C" PINVOKELIB_API void TestCallBack( FPTR pf, int value )
{
	printf( "\nReceived value: %i", value );
	printf( "\nPassing to callback..." );
	bool res = (*pf)(value);
	
	if( res )
		printf( "Callback returned true.\n" );
	else
		printf( "Callback returned false.\n" );
}
//******************************************************************

extern "C" PINVOKELIB_API void TestCallBack2( FPTR2 pf2, char* value )
{
	printf( "\nReceived value: %s", value );
	printf( "\nPassing to callback..." );
	bool res = (*pf2)(value);
	
	if( res )
		printf( "Callback2 returned true.\n" );
	else
		printf( "Callback2 returned false.\n" );
}

//******************************************************************

extern "C" PINVOKELIB_API void TestStringInStruct( MYSTRSTRUCT* pStruct )
{
	wprintf( L"\nUnicode buffer content: %s\n", pStruct->buffer );

	// assuming that buffer is big enough
	wcscat_s( pStruct->buffer, sizeof(pStruct->buffer), L"++" );
}

//******************************************************************

extern "C" PINVOKELIB_API void TestStringInStructAnsi( MYSTRSTRUCT2* pStruct )
{
	printf( "\nAnsi buffer content: %s\n", pStruct->buffer );

	// assuming that buffer is big enough
	strcat_s( pStruct->buffer, sizeof(pStruct->buffer), "++" );
}

//******************************************************************

extern "C" PINVOKELIB_API void TestOutArrayOfStructs( int* pSize, MYSTRSTRUCT2** ppStruct )
{
	const int cArraySize = 5;
	*pSize = cArraySize;
	*ppStruct = (MYSTRSTRUCT2*)CoTaskMemAlloc( cArraySize * sizeof( MYSTRSTRUCT2 ));

	MYSTRSTRUCT2* pCurStruct = *ppStruct;
	char* buffer;
	for( int i = 0; i < cArraySize; i++, pCurStruct++ )
	{
		pCurStruct->size = i;
		buffer = (char*)CoTaskMemAlloc( 4 );
		strcpy_s( buffer, 4, "***" );
		pCurStruct->buffer = buffer;
	}
}
//*************************************************************************************

extern "C" PINVOKELIB_API char* TestStringAsResult()
{
	char* result = (char*)CoTaskMemAlloc( 64 );
	strcpy_s( result, 64, "This is return value" );
	return result;
}

//*************************************************************************************

extern "C" PINVOKELIB_API void SetData( DataType typ, void* object )
{
	switch( typ )
	{
		case DT_I2: printf( "Short %i\n", *((short*)object) ); break;
		case DT_I4: printf( "Long %i\n", *((long*)object) ); break;
		case DT_R4: printf( "Float %f\n", *((float*)object) ); break;
		case DT_R8: printf( "Double %f\n", *((double*)object) ); break;
		case DT_STR: printf( "String %s\n", (char*)object ); break;
		default: printf( "Unknown type" ); break;
	}
}

//*************************************************************************************

extern "C" PINVOKELIB_API void TestArrayInStruct( MYARRAYSTRUCT* pStruct )
{
	pStruct->flag = true;
	pStruct->vals[ 0 ] += 100;
	pStruct->vals[ 1 ] += 100;
	pStruct->vals[ 2 ] += 100;
}
//*************************************************************************************
