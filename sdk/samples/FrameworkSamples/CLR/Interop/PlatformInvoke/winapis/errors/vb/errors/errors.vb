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
' Errors.vb

Imports System
Imports System.Text
Imports System.Runtime.InteropServices

Public Class LibWrap
   
	' int MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
	' SetLastError := True is default in VB.NET
	
	Declare Auto Function MessageBox Lib "User32.dll" ( _
		ByVal hWnd As IntPtr, ByVal txt As String, ByVal caption As String, ByVal typ As Integer ) _
	As Integer
   
   
	' DWORD FormatMessage(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, 
	'					DWORD dwLanguageId, LPTSTR lpBuffer, DWORD nSize, 
	'					va_list *Arguments)
	
	Declare Auto Function FormatMessage Lib "Kernel32.dll" ( _
		ByVal flags As Integer, ByVal src As IntPtr, ByVal messageId As Integer, _
		ByVal languageId As Integer, ByVal buffer As StringBuilder, _
		ByVal size As Integer, ByVal arguments As IntPtr ) _
	As Integer 
   
   Public Shared FORMAT_MESSAGE_FROM_SYSTEM As Integer = &H1000
   
End Class 'LibWrap

Public Class App
   
   Public Shared Sub Main()
   
      Dim errCode As Integer = Marshal.GetLastWin32Error()
      Console.WriteLine( "The last error before MessageBox is called: {0}", errCode )
      
      Console.WriteLine( "Calling MessageBox with wrong parameters..." )
      Dim res As Integer = LibWrap.MessageBox( IntPtr.Zero, "Correct text", "PInvoke MsgBox Sample", 999 )
      Console.WriteLine( "Call result: {0}", res )
      
      errCode = Marshal.GetLastWin32Error()
      Console.WriteLine( "The last error after MessageBox is called: {0}", errCode )
      
      Dim buffer As New StringBuilder( 256 )
      LibWrap.FormatMessage( LibWrap.FORMAT_MESSAGE_FROM_SYSTEM, IntPtr.Zero, errCode, 0, _
      	buffer, buffer.Capacity, IntPtr.Zero )
      Console.WriteLine( "Error message: {0}", buffer )
      
   End Sub 'Main
End Class 'App

