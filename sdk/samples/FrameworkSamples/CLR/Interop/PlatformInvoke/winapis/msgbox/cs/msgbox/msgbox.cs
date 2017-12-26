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
// MsgBox.cs

using System;
using System.Runtime.InteropServices;

public class LibWrap 
{
	// int MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
	
	[ DllImport( "User32.dll", EntryPoint="MessageBox", CharSet=CharSet.Auto )]
	public static extern int MsgBox( int hWnd, String text, String caption, uint type );
	
	// this will cause incorrect output in message window
	
	[ DllImport( "User32.dll", EntryPoint="MessageBoxW", CharSet=CharSet.Ansi )]
	public static extern int MsgBox2( int hWnd, String text, String caption, uint type );	
	
	// this will cause an exception
	
	[ DllImport( "User32.dll", EntryPoint="MessageBox", CharSet=CharSet.Ansi, ExactSpelling=true )]
	public static extern int MsgBox3( int hWnd, String text, String caption, uint type );	
}

public class MsgBoxSample
{
	public static void Main()
	{
		LibWrap.MsgBox( 0, "Correct text", "MsgBox Sample", 0 );
		LibWrap.MsgBox2( 0, "Incorrect text", "MsgBox Sample", 0 );
		try
		{
			LibWrap.MsgBox3( 0, "No such function", "MsgBox Sample", 0 );
		}
		catch( EntryPointNotFoundException )
		{
			Console.WriteLine( "EntryPointNotFoundException thrown as expected!" );
		}
	}
}


