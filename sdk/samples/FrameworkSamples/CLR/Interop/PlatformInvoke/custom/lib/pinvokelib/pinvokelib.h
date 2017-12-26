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

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PINVOKELIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PINVOKELIB_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef PINVOKELIB_EXPORTS
#define PINVOKELIB_API __declspec(dllexport)
#else
#define PINVOKELIB_API __declspec(dllimport)
#endif

// This class is exported from the PinvokeLib.dll
class PINVOKELIB_API CTestClass 
{
public:
	CTestClass( void );
	int DoSomething( int i );
private:
	int m_member;
};

extern "C" PINVOKELIB_API CTestClass* CreateTestClass();
extern "C" PINVOKELIB_API void DeleteTestClass( CTestClass* instance );

//*************************************************************************************

extern "C" PINVOKELIB_API int TestArrayOfInts( int* pArray, int size );

extern "C" PINVOKELIB_API int TestRefArrayOfInts( int** ppArray, int* pSize );

const int COL_DIM = 5;
extern "C" PINVOKELIB_API int TestMatrixOfInts( int pMatrix[][COL_DIM], int row );

extern "C" PINVOKELIB_API int TestArrayOfStrings( char* ppStrArray[], int size );
//*************************************************************************************

typedef struct _MYPOINT
{
	int x; 
	int y; 
} MYPOINT;

extern "C" PINVOKELIB_API int TestArrayOfStructs( MYPOINT* pPointArray, int size );
//*************************************************************************************

typedef struct _MYPERSON
{
	char* first; 
	char* last; 
} MYPERSON;

extern "C" PINVOKELIB_API int TestArrayOfStructs2( MYPERSON* pPersonArray, int size );
//*************************************************************************************

typedef struct _MYPERSON2
{
	MYPERSON* person;
	int age; 
} MYPERSON2;

extern "C" PINVOKELIB_API int TestStructInStruct( MYPERSON2* pPerson2 );
//*************************************************************************************

typedef struct _MYPERSON3
{
	MYPERSON person;
	int age; 
} MYPERSON3;

extern "C" PINVOKELIB_API void TestStructInStruct3( MYPERSON3 person3 );
//*************************************************************************************

union MYUNION
{
    int i;
    double d;
};

extern "C" PINVOKELIB_API void TestUnion( MYUNION u, int type );

union MYUNION2
{
    int i;
    char str[128];
};

extern "C" PINVOKELIB_API void TestUnion2( MYUNION2 u, int type );
//*************************************************************************************

typedef bool (CALLBACK *FPTR)( int i );
extern "C" PINVOKELIB_API void TestCallBack( FPTR pf, int value );

typedef bool (CALLBACK *FPTR2)( char* str );
extern "C" PINVOKELIB_API void TestCallBack2( FPTR2 pf2, char* value );

//*************************************************************************************

typedef struct _MYSTRSTRUCT
{
	wchar_t* buffer;
	UINT size; 
} MYSTRSTRUCT;

// buffer is an in/out param
extern "C" PINVOKELIB_API void TestStringInStruct( MYSTRSTRUCT* pStruct );

typedef struct _MYSTRSTRUCT2
{
	char* buffer;
	UINT size; 
} MYSTRSTRUCT2;

// buffer is in/out param
extern "C" PINVOKELIB_API void TestStringInStructAnsi( MYSTRSTRUCT2* pStruct );

extern "C" PINVOKELIB_API void TestOutArrayOfStructs( int* pSize, MYSTRSTRUCT2** ppStruct );

//*************************************************************************************

extern "C" PINVOKELIB_API char* TestStringAsResult();

//*************************************************************************************
enum DataType 
{
	DT_I2 = 1,
	DT_I4,
	DT_R4,
	DT_R8,
	DT_STR
};

extern "C" PINVOKELIB_API void SetData( DataType typ, void* object );

//*************************************************************************************

typedef struct _MYARRAYSTRUCT
{
	bool flag;
	int vals[ 3 ]; 
} MYARRAYSTRUCT;

extern "C" PINVOKELIB_API void TestArrayInStruct( MYARRAYSTRUCT* pStruct );

//*************************************************************************************