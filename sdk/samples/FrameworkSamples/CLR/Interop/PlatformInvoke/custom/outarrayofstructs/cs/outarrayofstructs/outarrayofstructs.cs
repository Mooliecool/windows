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

// OutArrayOfStructs.cs

using System;
using System.Runtime.InteropServices;

/*
typedef struct _MYSTRSTRUCT2
{
	char* buffer;
	UINT size; 
} MYSTRSTRUCT2;
*/
[ StructLayout( LayoutKind.Sequential, CharSet=CharSet.Ansi )]
public class MyStruct 
{
	public String buffer;
	public int size;
}

[ StructLayout( LayoutKind.Sequential )]
public struct MyUnsafeStruct 
{
	public IntPtr buffer;
	public int size;
}

public unsafe class LibWrap
{
	// void TestOutArrayOfStructs(int* pSize, MYSTRSTRUCT2** ppStruct);	
	
	[ DllImport( "PinvokeLib.dll" )]
	public static extern void TestOutArrayOfStructs( out int size, out IntPtr outArray );
	
	[CLSCompliant(false)]
	[ DllImport( "PinvokeLib.dll" )]
	public static extern void TestOutArrayOfStructs( out int size, MyUnsafeStruct** outArray );	
	
}

public class App
{
	public static void Main()
	{
		Console.WriteLine( "\nUsing marshal class\n" );
		UsingMarshal();
		
		Console.WriteLine( "\nUsing unsafe code\n" );
		UsingUnsafe();
	}
	
	public static void UsingMarshal()	
	{
		int size;
		IntPtr outArray;
		
		LibWrap.TestOutArrayOfStructs( out size, out outArray );
		
		MyStruct[] manArray = new MyStruct[ size ];
		
		IntPtr current = outArray;
		for( int i = 0; i < size; i++ )
		{
			manArray[ i ] = new MyStruct();
			Marshal.PtrToStructure( current, manArray[ i ]);
			
			//Marshal.FreeCoTaskMem( (IntPtr)Marshal.ReadInt32( current ));
			Marshal.DestroyStructure( current, typeof(MyStruct) );
			current = (IntPtr)((int)current + Marshal.SizeOf( manArray[ i ] ));
			
			Console.WriteLine( "Element {0}: {1} {2}", i, manArray[ i ].buffer, manArray[ i ].size );
		}
		Marshal.FreeCoTaskMem( outArray );	
	}
	
	public static unsafe void UsingUnsafe()
	{
		int size;
		MyUnsafeStruct* pResult;		
		LibWrap.TestOutArrayOfStructs( out size, &pResult );
		
		MyUnsafeStruct* pCurrent = pResult;
		for( int i = 0; i < size; i++, pCurrent++ )
		{
			Console.WriteLine( "Element {0}: {1} {2}", i, 
				Marshal.PtrToStringAnsi( pCurrent->buffer ), pCurrent->size );
			Marshal.FreeCoTaskMem( pCurrent->buffer );
		}
		
		Marshal.FreeCoTaskMem( (IntPtr)pResult );		
	}
}


