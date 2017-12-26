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
' Printf.vb

Imports System
Imports Microsoft.VisualBasic
Imports System.Runtime.InteropServices

Public Class LibWrap
   
	' VB doesn't support varargs so all arguments must be explicitly defined
	' CallingConvention.Cdecl must be used since the stack is cleaned up by the caller 
	
	' int printf( const char *format [, argument]... )
	
	< DllImport( "msvcrt.dll", CharSet:=CharSet.Ansi, CallingConvention := CallingConvention.Cdecl )> _
	Overloads Shared Function printf ( _
		ByVal format As String, ByVal i As Integer, ByVal d As Double ) As Integer
	End Function
	
	< DllImport( "msvcrt.dll", CharSet:=CharSet.Ansi, CallingConvention := CallingConvention.Cdecl )> _	
	Overloads Shared Function printf ( _
		ByVal format As String, ByVal i As Integer, ByVal s As String ) As Integer
	End Function
	
End Class 'LibWrap


Public Class App
	Public Shared Sub Main()
	
		LibWrap.printf( ControlChars.CrLf + "Print params: %i %f", 99, 99.99 )
		LibWrap.printf( ControlChars.CrLf + "Print params: %i %s", 99, "abcd" )
		
	End Sub 'Main
End Class 'App
