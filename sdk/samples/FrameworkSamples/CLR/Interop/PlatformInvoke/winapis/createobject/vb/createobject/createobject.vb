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
' CreateObject.vb

Imports System
Imports System.Runtime.InteropServices
Imports System.Threading
Imports Microsoft.VisualBasic

Public Class LibWrap

	'STDAPI CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext,  
	'						  REFIID riid, LPVOID * ppv);
	
	Declare Auto Function CoCreateInstance Lib "Ole32.dll" ( _
		<[In]> ByRef clsid As Guid, _
		<MarshalAs( UnmanagedType.IUnknown )> ByVal punkOuter As Object, _
		ByVal context As Integer, _
		<[In]> ByRef iid As Guid, _
		<MarshalAs( UnmanagedType.IUnknown )> ByRef o As Object ) _
	As Integer
	
	' using DllImport syntax since PreserveSig can't be specified in Declare
	
	<DllImport( "Ole32.dll", CharSet:=CharSet.Auto, _
	EntryPoint:="CoCreateInstance", PreserveSig := False )> _	
	Shared Function CoCreateInstance2 ( _
	   <[In]> ByRef clsid As Guid, _
	   <MarshalAs( UnmanagedType.IUnknown )> ByVal punkOuter As Object, _
	   ByVal context As Integer, _
	   <[In]> ByRef iid As Guid ) As <MarshalAs( UnmanagedType.IUnknown )> Object 
	End Function
	
	Public Shared InprocServer As Integer = 1
	Public Shared LocalServer As Integer = 4
	
End Class 'LibWrap

Public Class App
	Public Shared Sub Main()

		Dim hr As Integer = 0
		Dim syncMgr As Object = Nothing
		Dim CLSID_BadID As New Guid( "6295DF27-35EE-11d1-8707-00C04FD93326" )
		Dim CLSID_SyncMgr As New Guid( "6295DF27-35EE-11d1-8707-00C04FD93327" )
		Dim IID_ISyncMgrReg As New Guid( "6295DF42-35EE-11d1-8707-00C04FD93327" )
		
		' create the real SyncMgr object using CoCreateInstance 
		' that returns an HR
		
		Console.WriteLine( ControlChars.CrLf + "Creating Sync Mgr with CoCreateInstance" )
		hr = LibWrap.CoCreateInstance( CLSID_SyncMgr, Nothing, _
			LibWrap.InprocServer Or LibWrap.LocalServer, IID_ISyncMgrReg, syncMgr )
		Console.WriteLine( "CoCreateInstance return HRESULT = {0:x}", hr )
		
		' create the real SyncMgr object using CoCreateInstance 
		' that returns an Object and throws exception
		
		Console.WriteLine( ControlChars.CrLf + "Creating Sync Mgr with CoCreateInstance2" )
		syncMgr = LibWrap.CoCreateInstance2( CLSID_SyncMgr, Nothing, _
			LibWrap.InprocServer Or LibWrap.LocalServer, IID_ISyncMgrReg )
		Console.WriteLine( syncMgr.ToString() )
		
		
		' try to create an object that doesn't exist.  HRESULT returned should be 0x80040154
		
		Console.WriteLine( ControlChars.CrLf + "Creating Sync Mgr with CoCreateInstance" )
		hr = LibWrap.CoCreateInstance( CLSID_BadID, Nothing, _
			LibWrap.InprocServer Or LibWrap.LocalServer, IID_ISyncMgrReg, syncMgr )
			
		If hr <> 0 Then
			If hr = &H80040154 Then
				Console.WriteLine( "CoCreateInstance failed as expected" )
			Else
				Console.WriteLine( "CoCreateInstance failed unexpectedly with HRESULT = {0:x}", hr )
			End If
		End If
		
		' try to create an object that doesn't exist. COMException is thrown.
		
		Console.WriteLine( ControlChars.CrLf + "Creating Sync Mgr with CoCreateInstance2" )
		Try
			syncMgr = LibWrap.CoCreateInstance2( CLSID_BadID, Nothing, _
				LibWrap.InprocServer Or LibWrap.LocalServer, IID_ISyncMgrReg )
		Catch e As COMException
			If e.ErrorCode = &H80040154 Then
				Console.WriteLine( "CoCreateInstance failed as expected" )
			Else
				Console.WriteLine( "CoCreateInstance failed unexpectedly with HRESULT = {0:x}", e.ErrorCode )
			End If
		End Try
		
	End Sub 'Main	
End Class 'App
