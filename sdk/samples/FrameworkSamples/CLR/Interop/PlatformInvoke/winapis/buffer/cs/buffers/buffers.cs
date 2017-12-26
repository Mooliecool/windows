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
// Buffers.cs

using System;
using System.Text;
using System.Runtime.InteropServices;

public class LibWrap
{
	// UINT GetSystemDirectory(LPTSTR lpBuffer, UINT uSize)
	[ DllImport( "Kernel32.dll", CharSet=CharSet.Auto )]
	public static extern int GetSystemDirectory( StringBuilder sysDirBuffer, int size );  
	
	// BOOL GetUserName(LPTSTR lpBuffer,LPDWORD nSize);
	[ DllImport( "Advapi32.dll", CharSet=CharSet.Auto )]
	public static extern bool GetUserName( StringBuilder userNameBuffer, ref int size );	
	
    // When marshaling strings as return types, the platform invoke marshaler assumes
    // it must free the memory the original LPTSTR pointed to. This isn't
    // quite valid when a function like this returns a pointer into the 
    // process's environment or similar kernel data structure.
    
	// LPTSTR GetCommandLine();
    [DllImport("Kernel32.dll", CharSet=CharSet.Auto)]
    public static extern IntPtr GetCommandLine();	
}

public class App
{
	public static void Main()
	{
		// call GetSystemDirectory
		StringBuilder sysDirBuffer = new StringBuilder( 256 );
		LibWrap.GetSystemDirectory( sysDirBuffer, sysDirBuffer.Capacity );
		Console.WriteLine( "SysDir: {0}", sysDirBuffer );
		
		// call GetUserName
		StringBuilder userNameBuffer = new StringBuilder( 128 );
		int size = userNameBuffer.Capacity;
		LibWrap.GetUserName( userNameBuffer, ref size );
		Console.WriteLine( "UserName: {0}", userNameBuffer );
		
        IntPtr cmdLineStr = LibWrap.GetCommandLine();
        String commandLine = Marshal.PtrToStringAuto( cmdLineStr );
        Console.WriteLine( "CommandLine: {0}", commandLine );			
	}
}


