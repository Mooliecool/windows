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
' FindFile.vb

Imports System
Imports System.Runtime.InteropServices

'typedef struct _WIN32_FIND_DATA 
'{
'  DWORD    dwFileAttributes; 
'  FILETIME ftCreationTime; 
'  FILETIME ftLastAccessTime; 
'  FILETIME ftLastWriteTime; 
'  DWORD    nFileSizeHigh; 
'  DWORD    nFileSizeLow; 
'  DWORD    dwReserved0; 
'  DWORD    dwReserved1; 
'  TCHAR    cFileName[ MAX_PATH ]; 
'  TCHAR    cAlternateFileName[ 14 ]; 
'} WIN32_FIND_DATA, *PWIN32_FIND_DATA; 

< StructLayout( LayoutKind.Sequential, CharSet := CharSet.Auto )> _
Public Class FindData

   Public fileAttributes As Integer = 0
   
   ' creationTime was a by-value FILETIME structure
   Public creationTime_lowDateTime As Integer = 0
   Public creationTime_highDateTime As Integer = 0
   
   ' lastAccessTime was a by-value FILETIME structure
   Public lastAccessTime_lowDateTime As Integer = 0
   Public lastAccessTime_highDateTime As Integer = 0
   
   ' lastWriteTime was a by-value FILETIME structure
   Public lastWriteTime_lowDateTime As Integer = 0
   Public lastWriteTime_highDateTime As Integer = 0
   
   Public nFileSizeHigh As Integer = 0
   Public nFileSizeLow As Integer = 0
   Public dwReserved0 As Integer = 0
   Public dwReserved1 As Integer = 0
   
   < MarshalAs( UnmanagedType.ByValTStr, SizeConst := 256 )> _
   Public fileName As String = Nothing
   
   < MarshalAs( UnmanagedType.ByValTStr, SizeConst := 14 )> _
   Public alternateFileName As String = Nothing
   
End Class 'FindData

Public Class LibWrap
   
	' HANDLE FindFirstFile(LPCTSTR lpFileName, LPWIN32_FIND_DATA lpFindFileData)
	
	Declare Auto Function FindFirstFile Lib "Kernel32.dll" _
		( ByVal fileName As String, <[In], Out> ByVal findFileData As FindData ) As IntPtr
   
End Class 'LibWrap

Public Class App
   Public Shared Sub Main()
   
      Dim fd As New FindData()
      LibWrap.FindFirstFile( "C:\*", fd )
      Console.WriteLine( "The first file: {0}", fd.fileName )
      
   End Sub 'Main
End Class 'App
