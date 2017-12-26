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
' ActiveDir.vb

Imports System
Imports System.Text
Imports System.Runtime.InteropServices
Imports System.Threading

'typedef struct 
'{
'	DWORD           cbStruct;
'	HWND            hwndOwner;
'	LPCWSTR         pszCaption;
'	LPCWSTR         pszTitle;
'	LPCWSTR         pszRoot; 
'	LPWStr          pszPath;
'	ULONG           cchPath;
'	DWORD           dwFlags;
'	BFFCALLBACK     pfnCallback;
'	LPARAM          lParam;
'	DWORD           dwReturnFormat;
'	LPCWSTR         pUserName;
'	LPCWSTR         pPassword;
'	LPWStr          pszObjectClass;
'	ULONG           cchObjectClass;
'} DSBROWSEINFOW, *PDSBROWSEINFOW;

' all strings in structure will be marshaled as LPWStr
< StructLayout( LayoutKind.Sequential, CharSet := CharSet.Unicode )> _
Public Structure DSBrowseInfo

	Public structSize As Integer
    Private dlgOwner As IntPtr
	
	Public dlgCaption As String
	Public treeViewTitle As String
	Public rootPath As String
	
	' in/out string must be declared as String in struct/class, 
	' not as StringBuilder
	Public path As String
	Public pathSize As Integer 
	
	Public flags As Integer 
    Private callback As IntPtr
    Private lParam As IntPtr
	Public returnFormat As Integer 
	
	Public userName As String
	Public password As String
	
	Public objectClass As String
	Public objectClassSize As Integer
	
End Structure 'DSBrowseInfo

Public Class LibWrap
   
	'int DsBrowseForContainer(PDSBROWSEINFO pInfo)
	
	Declare Unicode Function DsBrowseForContainerW Lib "dsuiext.dll" ( _
		ByRef info As DSBrowseInfo ) As Integer
   
	Public Shared DSBI_ENTIREDIRECTORY As Integer = &H90000
   
End Class 'LibWrap

Class App

	Public Shared MAX_PATH As Integer = 256
	
	' DsBrowseForContainerW should be called from STA, but STA is default in VB.NET
	' so we don't need to specify it explicitly like in C#
	Public Shared Sub Main()
	
		' initialize all members
		Dim dsbi As New DSBrowseInfo()
		
		dsbi.structSize = Marshal.SizeOf( dsbi )
		dsbi.dlgCaption = "The container picker"
		dsbi.treeViewTitle = "Pick a container for this example."
		
		dsbi.path = New String( New Char( MAX_PATH ) {}) 
		dsbi.pathSize = dsbi.path.Length 
		dsbi.flags = LibWrap.DSBI_ENTIREDIRECTORY
		
		dsbi.objectClass = New String( New Char( MAX_PATH ) {})
		dsbi.objectClassSize = dsbi.objectClass.Length 
		
		Console.WriteLine( "Call DSBrowseForContainer..." )
		
		Dim status As Integer = LibWrap.DsBrowseForContainerW( dsbi )
		
		Console.WriteLine( "The status is " & status )
		
		If status = 1 Then
			Console.WriteLine( "The path is " + dsbi.path )
		Else
			Console.WriteLine( "Call failed!" )
		End If
		
	End Sub 'Main
	
End Class 'App
