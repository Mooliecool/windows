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
' OpenFileDlg.vb

Imports System
Imports System.Text
Imports Microsoft.VisualBasic
Imports System.Runtime.InteropServices

'typedef struct tagOFN { 
'  DWORD         lStructSize; 
'  HWND          hwndOwner; 
'  HINSTANCE     hInstance; 
'  LPCTSTR       lpstrFilter; 
'  LPTSTR        lpstrCustomFilter; 
'  DWORD         nMaxCustFilter; 
'  DWORD         nFilterIndex; 
'  LPTSTR        lpstrFile; 
'  DWORD         nMaxFile; 
'  LPTSTR        lpstrFileTitle; 
'  DWORD         nMaxFileTitle; 
'  LPCTSTR       lpstrInitialDir; 
'  LPCTSTR       lpstrTitle; 
'  DWORD         Flags; 
'  WORD          nFileOffset; 
'  WORD          nFileExtension; 
'  LPCTSTR       lpstrDefExt; 
'  LPARAM        lCustData; 
'  LPOFNHOOKPROC lpfnHook; 
'  LPCTSTR       lpTemplateName; 
'#if (_WIN32_WINNT >= 0x0500)
'  void *        pvReserved;
'  DWORD         dwReserved;
'  DWORD         FlagsEx;
'#endif // (_WIN32_WINNT >= 0x0500)
'} OPENFILENAME, *LPOPENFILENAME; 

< StructLayout( LayoutKind.Sequential, CharSet:=CharSet.Auto )> _
Public Class OpenFileName

	Public structSize As Integer = 0
    Private dlgOwner As IntPtr = IntPtr.Zero
    Private instance As IntPtr = IntPtr.Zero
	
	Public filter As String = Nothing
	Public customFilter As String = Nothing
	Public maxCustFilter As Integer = 0
	Public filterIndex As Integer = 0
	
	Public file As String = Nothing
	Public maxFile As Integer = 0
	
	Public fileTitle As String = Nothing
	Public maxFileTitle As Integer = 0
	
	Public initialDir As String = Nothing
	
	Public title As String = Nothing
	
	Public flags As Integer = 0
	Public fileOffset As Short = 0
	Public fileExtension As Short = 0
	
	Public defExt As String = Nothing
	
    Private custData As IntPtr = IntPtr.Zero
    Private hook As IntPtr = IntPtr.Zero
	
	Public templateName As String = Nothing
	
    Private reservedPtr As IntPtr = IntPtr.Zero
	Public reservedInt As Integer = 0
	Public flagsEx As Integer = 0
   
End Class 'OpenFileName


Public Class LibWrap
   
	' BOOL GetOpenFileName(LPOPENFILENAME lpofn);
	
	Declare Auto Function GetOpenFileName Lib "Comdlg32.dll" ( _
		<[In], Out> ByVal ofn As OpenFileName ) As Boolean

End Class 'LibWrap

Public Class App
	Public Shared Sub Main()
	
		Dim ofn As New OpenFileName()
		
		ofn.structSize = Marshal.SizeOf( ofn )
		
		ofn.filter = "Log files" & ChrW(0) & "*.log" & ChrW(0) & _
			"Batch files" & ChrW(0) & "*.bat" & ChrW(0)
		
		ofn.file = New String( New Char( 256 ) {})
		ofn.maxFile = ofn.file.Length
		
		ofn.fileTitle = New String( New Char( 64 ) {})
		ofn.maxFileTitle = ofn.fileTitle.Length
		
		ofn.initialDir = "C:\"
		ofn.title = "Open file called using platform invoke..."
		ofn.defExt = "txt"
		
		If LibWrap.GetOpenFileName( ofn ) Then
			Console.WriteLine( "Selected file with full path: {0}", ofn.file )
			Console.WriteLine( "Selected file name: {0}", ofn.fileTitle )
			Console.WriteLine( "Offset from file name: {0}", ofn.fileOffset )
			Console.WriteLine( "Offset from file extension: {0}", ofn.fileExtension )
		End If
      
	End Sub 'Main
End Class 'OpenFileDlgSample
