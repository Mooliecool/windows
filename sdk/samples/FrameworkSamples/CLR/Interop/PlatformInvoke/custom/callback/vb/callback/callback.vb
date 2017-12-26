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

' Callback.vb

Imports System
Imports Microsoft.VisualBasic
Imports System.Runtime.InteropServices

Public Delegate Function FPtr( ByVal value As Integer ) As Boolean
Public Delegate Function FPtr2( ByVal value As String ) As Boolean

Public Class LibWrap
   
	'void TestCallBack(FPTR pf, int value)
	
    Declare Sub TestCallBack Lib "PinvokeLib.dll" (ByVal cb As FPtr, ByVal value As Integer)
	
	'void TestCallBack2(FPTR2 pf2, char* value)
	
    Declare Sub TestCallBack2 Lib "PinvokeLib.dll" (ByVal cb2 As FPtr2, ByVal value As String)

End Class 'LibWrap

Public Class App
   
	Public Shared Sub Main()
	
		' If delagate will be used just inside one call, this syntax
		' is ok since platform invoke will protect delegate from GC
		' until call ends. If unmanaged side could store delegate and
		' try to use it later than caller should protect delegate until
		' unmanaged side is finished.
			
		Dim cb As FPtr
		cb = AddressOf App.DoSomething
		
		Dim cb2 As FPtr2
		cb2 = AddressOf App.DoSomething2		
			
		LibWrap.TestCallBack( cb, 99 )
		LibWrap.TestCallBack2( cb2, "abc" )
		
	End Sub 'Main
   
	Public Shared Function DoSomething( ByVal value As Integer ) As Boolean
	
		Console.WriteLine( ControlChars.CrLf + "Callback called with param: {0}", value )
		If value < 0 Then
			Return False
		Else
			Return True
		End If
		
	End Function 'DoSomething
    
	Public Shared Function DoSomething2( ByVal value As String ) As Boolean
	
		Console.WriteLine( ControlChars.CrLf + "Callback called with param: {0}", value )
		If value.Length < 99 Then
			Return False
		Else
			Return True
		End If
		
	End Function 'DoSomething2
	
End Class 'App 
