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

' ClassMethods.vb

Imports System
Imports System.Text
Imports Microsoft.VisualBasic
Imports System.Runtime.InteropServices

Public Class LibWrap
	
	' class PINVOKELIB_API CTestClass 
	' {
	' public:
	'	CTestClass( void );
	'	int DoSomething( int i );
	' private:
	'	int m_member;
	' };	
	 
    <DllImport("PinvokeLib.dll", _
 EntryPoint:="?DoSomething@CTestClass@@QEAAHH@Z", _
 CallingConvention:=CallingConvention.ThisCall)> _
 Shared Function TestThisCalling( _
  ByVal ths As IntPtr, ByVal i As Integer) As Integer
    End Function
	
	' CTestClass* CreateTestClass();
	
    Declare Function CreateTestClass Lib "PinvokeLib.dll" () As IntPtr
	
	' void DeleteTestClass( CTestClass* instance )
	
    Declare Sub DeleteTestClass Lib "PInvokeLib.dll" (ByVal instance As IntPtr)
	
End Class 'LibWrap

Public Class App
	Public Shared Sub Main()
	
		Dim instancePtr As IntPtr = LibWrap.CreateTestClass()
		
		Dim res As Integer = LibWrap.TestThisCalling( instancePtr, 9 )
		Console.WriteLine( ControlChars.CrLf + "Result: {0}", res )
		
		LibWrap.DeleteTestClass( instancePtr )
		
	End Sub 'Main
End Class 'App


