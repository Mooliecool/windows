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
// Printf.cs

using System;
using System.Runtime.InteropServices;

public class LibWrap
{
	// C# doesn't support varargs so all arguments must be explicitly defined
	// CallingConvention.Cdecl must be used since the stack is cleaned up by the caller 
	
	// int printf( const char *format [, argument]... )
	
	[ DllImport( "msvcrt.dll", CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl )]
	public static extern int printf( String format, int i, double d );	
	
	[ DllImport( "msvcrt.dll", CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl )]
	public static extern int printf( String format, int i, String s );	
	
}

public class App
{
	public static void Main()
	{
		LibWrap.printf( "\nPrint params: %i %f", 99, 99.99 );
		LibWrap.printf( "\nPrint params: %i %s", 99, "abcd" );
	}
}


