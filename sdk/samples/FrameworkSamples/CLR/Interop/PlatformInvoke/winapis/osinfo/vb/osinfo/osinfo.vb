'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'-----------------------------------------------------------------------
' OSInfo.vb

Imports System
Imports Microsoft.VisualBasic
Imports System.Runtime.InteropServices

'typedef struct _OSVERSIONINFO
'{ 
'  DWORD dwOSVersionInfoSize; 
'  DWORD dwMajorVersion; 
'  DWORD dwMinorVersion; 
'  DWORD dwBuildNumber; 
'  DWORD dwPlatformId; 
'  TCHAR szCSDVersion[ 128 ]; 
'} OSVERSIONINFO; 

< StructLayout( LayoutKind.Sequential )> _
Public Class OSVersionInfo

   Public OSVersionInfoSize As Integer
   Public majorVersion As Integer
   Public minorVersion As Integer
   Public buildNumber As Integer
   Public platformId As Integer
   
   < MarshalAs( UnmanagedType.ByValTStr, SizeConst := 128 )> _
   Public versionString As String
   
End Class 'OSVersionInfo

< StructLayout( LayoutKind.Sequential )> _
Public Structure OSVersionInfo2

   Public OSVersionInfoSize As Integer
   Public majorVersion As Integer
   Public minorVersion As Integer
   Public buildNumber As Integer
   Public platformId As Integer
   
   < MarshalAs( UnmanagedType.ByValTStr, SizeConst := 128 )> _
   Public versionString As String
   
End Structure 'OSVersionInfo2

Public Class LibWrap
   
	'BOOL GetVersionEx(LPOSVERSIONINFO lpVersionInfo);
	
	Declare Ansi Function GetVersionEx Lib "kernel32" Alias "GetVersionExA" ( _
		<[In], Out> ByVal osvi As OSVersionInfo ) As Boolean
	
	Declare Ansi Function GetVersionEx2 Lib "kernel32" Alias "GetVersionExA" ( _
		ByRef osvi As OSVersionInfo2 ) As Boolean
		
End Class 'LibWrap


Public Class App
	Public Shared Sub Main()
	
		Console.WriteLine( ControlChars.CrLf + "Passing OSVersionInfo as class" )
		
		Dim osvi As New OSVersionInfo()
		osvi.OSVersionInfoSize = Marshal.SizeOf( osvi )
		
		LibWrap.GetVersionEx( osvi )
		
		Console.WriteLine( "Class size:    {0}", osvi.OSVersionInfoSize )
		Console.WriteLine( "Major version: {0}", osvi.majorVersion )
		Console.WriteLine( "Minor version: {0}", osvi.minorVersion )
		Console.WriteLine( "Build number:  {0}", osvi.buildNumber )
		Console.WriteLine( "Platform ID:   {0}", osvi.platformId )
		Console.WriteLine( "Version:       {0}", osvi.versionString )
		
		
		Console.WriteLine( ControlChars.CrLf + "Passing OSVersionInfo as struct" )
		
		Dim osvi2 As New OSVersionInfo2()
		osvi2.OSVersionInfoSize = Marshal.SizeOf( osvi2 )
		
		LibWrap.GetVersionEx2( osvi2 )
		
		Console.WriteLine( "Struct size:   {0}", osvi2.OSVersionInfoSize )
		Console.WriteLine( "Major version: {0}", osvi2.majorVersion )
		Console.WriteLine( "Minor version: {0}", osvi2.minorVersion )
		Console.WriteLine( "Build number:  {0}", osvi2.buildNumber )
		Console.WriteLine( "Platform ID:   {0}", osvi2.platformId )
		Console.WriteLine( "Version:       {0}", osvi2.versionString )
		
	End Sub 'Main
End Class 'App
