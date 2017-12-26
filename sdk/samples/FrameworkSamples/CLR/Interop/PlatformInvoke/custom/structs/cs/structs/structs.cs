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

// Structs.cs

using System;
using System.Runtime.InteropServices;

/*
typedef struct _MYPERSON
{
	char* first; 
	char* last; 
} MYPERSON, *LP_MYPERSON;
*/

[ StructLayout( LayoutKind.Sequential, CharSet=CharSet.Ansi )]
public struct MyPerson 
{
	public String first; 
	public String last;
}

/*
typedef struct _MYPERSON2
{
	MYPERSON* person;
	int age; 
} MYPERSON2, *LP_MYPERSON2;
*/

[ StructLayout( LayoutKind.Sequential )]
public struct MyPerson2 
{
	public IntPtr person;
	public int age;
}

/*
typedef struct _MYPERSON3
{
	MYPERSON person;
	int age; 
} MYPERSON3;
*/

[ StructLayout( LayoutKind.Sequential )]
public struct MyPerson3 
{
	public MyPerson person;
	public int age;
}

/*
typedef struct _MYARRAYSTRUCT
{
	bool flag;
	int vals[ 3 ]; 
} MYARRAYSTRUCT;
*/

[ StructLayout( LayoutKind.Sequential )]
public struct MyArrayStruct 
{
	public bool flag;
	[ MarshalAs( UnmanagedType.ByValArray, SizeConst=3 )] 
	public int[] vals;
}

public class LibWrap
{
	// int TestStructInStruct(MYPERSON2* pPerson2);
	
	[ DllImport( "PinvokeLib.dll" )]
	public static extern int TestStructInStruct( ref MyPerson2 person2 );
	
	// void TestStructInStruct3(MYPERSON3 person3)
	
	[ DllImport( "PinvokeLib.dll" )]
	public static extern int TestStructInStruct3( MyPerson3 person3 );	
	
	// void TestArrayInStruct( MYARRAYSTRUCT* pStruct );
	
	[ DllImport( "PinvokeLib.dll" )]
	public static extern int TestArrayInStruct( ref MyArrayStruct myStruct );	
}

public class App
{
	public static void Main()
	{
		// ******************* structure with pointer to other structure ************
		MyPerson personName;
		personName.first = "Mark";
		personName.last = "Lee";
		
		MyPerson2 personAll;
		personAll.age = 30;
		
		IntPtr buffer = Marshal.AllocCoTaskMem( Marshal.SizeOf( personName ));
		Marshal.StructureToPtr( personName, buffer, false );
		
		personAll.person = buffer;
		
		Console.WriteLine( "\nPerson before call:" );
		Console.WriteLine( "first = {0}, last = {1}, age = {2}", 
			personName.first, personName.last, personAll.age ); 
		
		int res = LibWrap.TestStructInStruct( ref personAll );
		
		MyPerson personRes = 
			(MyPerson)Marshal.PtrToStructure( personAll.person, typeof( MyPerson ));
		
        Marshal.DestroyStructure(buffer, typeof(MyPerson));
		
		Console.WriteLine( "Person after call:" );
		Console.WriteLine( "first = {0}, last = {1}, age = {2}", 
			personRes.first, personRes.last, personAll.age );
		
		// ******************* structure with embedded structure ************	
		MyPerson3 person3 = new MyPerson3();
		person3.person.first = "John";
		person3.person.last = "Evans";
		person3.age = 27;
		
		LibWrap.TestStructInStruct3( person3 );
		
		// ******************* structure with embedded array ************	
		MyArrayStruct myStruct = new MyArrayStruct();
		
		myStruct.flag = false;
		myStruct.vals = new int[ 3 ];
		myStruct.vals[ 0 ] = 1;
		myStruct.vals[ 1 ] = 4;
		myStruct.vals[ 2 ] = 9;
		
		Console.WriteLine( "\nStructure with array before call:" );
		Console.WriteLine( myStruct.flag );
		Console.WriteLine( "{0} {1} {2}", myStruct.vals[ 0 ], 
			myStruct.vals[ 1 ], myStruct.vals[ 2 ] );
		
		LibWrap.TestArrayInStruct( ref myStruct );
		
		Console.WriteLine( "\nStructure with array after call:" );
		Console.WriteLine( myStruct.flag );
		Console.WriteLine( "{0} {1} {2}", myStruct.vals[ 0 ], 
			myStruct.vals[ 1 ], myStruct.vals[ 2 ] );		
	}
}


