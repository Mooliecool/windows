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

// ClassMethods.cs

using System;
using System.Text;
using System.Runtime.InteropServices;

public class LibWrap
{
	/*
	class PINVOKELIB_API CTestClass 
	{
	public:
		CTestClass( void );
		int DoSomething( int i );
	private:
		int m_member;
	};	
	*/
	 
	[ DllImport( "PinvokeLib.dll", 
	EntryPoint="?DoSomething@CTestClass@@QEAAHH@Z", 
	CallingConvention=CallingConvention.ThisCall )]
	public static extern int TestThisCalling( IntPtr ths, int i );	
	
	// CTestClass* CreateTestClass();
	[ DllImport( "PinvokeLib.dll" )]
	public static extern IntPtr CreateTestClass();	
	
	// void DeleteTestClass( CTestClass* instance )
	[ DllImport( "PinvokeLib.dll" )]
	public static extern void DeleteTestClass( IntPtr instance );	
}

public class App
{
	public static void Main()
	{
		IntPtr instancePtr = LibWrap.CreateTestClass();
		
		int res = LibWrap.TestThisCalling( instancePtr, 9 );	
		Console.WriteLine( "\nResult: {0} \n", res );
		
		LibWrap.DeleteTestClass( instancePtr );
	}
}


