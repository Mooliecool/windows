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

// Unions.cs

using System;
using System.Runtime.InteropServices;

/*
union MYUNION
{
    int i;
    double d;
};
*/

[ StructLayout( LayoutKind.Explicit )]
public struct MyUnion 
{
	[ FieldOffset( 0 )]
	public int i;
	[ FieldOffset( 0 )]
	public double d;
}

/*
union MYUNION2
{
    int i;
    char str[128];
};
*/

[ StructLayout( LayoutKind.Explicit, Size=128 )]
public struct MyUnion2_1 
{	
	[ FieldOffset( 0 )]
	public int i;
}

[ StructLayout( LayoutKind.Sequential )]
public struct MyUnion2_2 
{	
	[ MarshalAs( UnmanagedType.ByValTStr, SizeConst=128 )] 
	public String str;
}

public class LibWrap
{
	// void TestUnion(MYUNION u, int type)
	
	[ DllImport( "PinvokeLib.dll" )]
	public static extern void TestUnion( MyUnion u, int type );
	
	// void TestUnion( MYUNION u, int type )
	
	[ DllImport( "PinvokeLib.dll" )]
	public static extern void TestUnion2( MyUnion2_1 u, int type );
	
	// void TestUnion(MYUNION u, int type)
	
	[ DllImport( "PinvokeLib.dll" )]
	public static extern void TestUnion2( MyUnion2_2 u, int type );		
}

public class App
{
	public static void Main()
	{
		MyUnion mu = new MyUnion();
		mu.i = 99;
		LibWrap.TestUnion( mu, 1 );
		
		mu.d = 99.99;
		LibWrap.TestUnion( mu, 2 );
		
		MyUnion2_1 mu2_1 = new MyUnion2_1();
		mu2_1.i = 99;
		LibWrap.TestUnion2( mu2_1, 1 );
		
		MyUnion2_2 mu2_2 = new MyUnion2_2();
		mu2_2.str = "*** string ***";
		LibWrap.TestUnion2( mu2_2, 2 );		
	}
}


