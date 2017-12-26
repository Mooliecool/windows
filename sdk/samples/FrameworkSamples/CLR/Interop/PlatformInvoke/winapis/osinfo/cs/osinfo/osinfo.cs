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
// OSInfo.cs

using System;
using System.Runtime.InteropServices;

/*
typedef struct _OSVERSIONINFO
{ 
  DWORD dwOSVersionInfoSize; 
  DWORD dwMajorVersion; 
  DWORD dwMinorVersion; 
  DWORD dwBuildNumber; 
  DWORD dwPlatformId; 
  TCHAR szCSDVersion[ 128 ]; 
} OSVERSIONINFO; 
*/

[ StructLayout( LayoutKind.Sequential )]   
public class OSVersionInfo 
{			
    public int OSVersionInfoSize;
    public int majorVersion; 
    public int minorVersion;
    public int buildNumber;
    public int platformId;

	[ MarshalAs( UnmanagedType.ByValTStr, SizeConst=128 )]    
    public String versionString;
}

[ StructLayout( LayoutKind.Sequential )]  
public struct OSVersionInfo2 
{
    public int OSVersionInfoSize;
    public int majorVersion; 
    public int minorVersion;
    public int buildNumber;
    public int platformId;

	[ MarshalAs( UnmanagedType.ByValTStr, SizeConst=128 )]    
    public String versionString;
}


public class LibWrap 
{
	// BOOL GetVersionEx(LPOSVERSIONINFO lpVersionInfo);
	
	[ DllImport( "kernel32" )]				
	public static extern bool GetVersionEx( [In, Out] OSVersionInfo osvi );

	[ DllImport( "kernel32", EntryPoint="GetVersionEx" )] 
	public static extern bool GetVersionEx2( ref OSVersionInfo2 osvi );  
}

public class App
{
	public static void Main()
	{
		Console.WriteLine( "\nPassing OSVersionInfo as class" );

		OSVersionInfo osvi = new OSVersionInfo();
		osvi.OSVersionInfoSize = Marshal.SizeOf( osvi );

		LibWrap.GetVersionEx( osvi );
		
		Console.WriteLine( "Class size:    {0}", osvi.OSVersionInfoSize );
		Console.WriteLine( "Major version: {0}", osvi.majorVersion );
		Console.WriteLine( "Minor version: {0}", osvi.minorVersion );
		Console.WriteLine( "Build number:  {0}", osvi.buildNumber );
		Console.WriteLine( "Platform ID:   {0}", osvi.platformId );
		Console.WriteLine( "Version:       {0}", osvi.versionString );

		Console.WriteLine( "\nPassing OSVersionInfo as struct" );

		OSVersionInfo2 osvi2 = new OSVersionInfo2();
		osvi2.OSVersionInfoSize = Marshal.SizeOf( osvi2 );

		LibWrap.GetVersionEx2( ref osvi2 );

		Console.WriteLine( "Struct size:   {0}", osvi2.OSVersionInfoSize );
		Console.WriteLine( "Major version: {0}", osvi2.majorVersion );
		Console.WriteLine( "Minor version: {0}", osvi2.minorVersion );
		Console.WriteLine( "Build number:  {0}", osvi2.buildNumber );
		Console.WriteLine( "Platform ID:   {0}", osvi2.platformId );
		Console.WriteLine( "Version:       {0}", osvi2.versionString );
	}
}



