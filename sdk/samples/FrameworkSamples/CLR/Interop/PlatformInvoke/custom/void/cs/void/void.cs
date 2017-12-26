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

// Void.cs

using System;
using System.Text;
using System.Runtime.InteropServices;

public class LibWrap
{
	public enum DataType 
	{
		DT_I2 = 1,
		DT_I4,
		DT_R4,
		DT_R8,
		DT_STR
	}
	
	// void SetData(DataType typ, void* object)
	// using AsAny when void* is expected
	
	[ DllImport( "PinvokeLib.dll" )]
	public static extern void SetData( DataType t, 
		[ MarshalAs( UnmanagedType.AsAny )] Object o );
	
	// using overloading when void* is expected
	
	[ DllImport( "PinvokeLib.dll", EntryPoint="SetData" )]
	public static extern void SetData2( DataType t, ref double i );			
	
	[ DllImport( "PinvokeLib.dll", EntryPoint="SetData" )]
	public static extern void SetData2( DataType t, String s );	
	
}

public class App
{
	public static void Main()
	{
		Console.WriteLine( "Calling SetData using AsAny... \n" );
		
		LibWrap.SetData( LibWrap.DataType.DT_I2, (short)12 );
		LibWrap.SetData( LibWrap.DataType.DT_I4, (long)12 );
		LibWrap.SetData( LibWrap.DataType.DT_R4, (float)12 );
		LibWrap.SetData( LibWrap.DataType.DT_R8, (double)12 );
		LibWrap.SetData( LibWrap.DataType.DT_STR, "abcd" );	
		
		Console.WriteLine( "\nCalling SetData using overloading... \n" );	
			
		double d = 12;
		LibWrap.SetData2( LibWrap.DataType.DT_R8, ref d );
		LibWrap.SetData2( LibWrap.DataType.DT_STR, "abcd" );
	}
}


