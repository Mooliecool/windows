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

' OutArrayOfStructs.vb

Imports System
Imports Microsoft.VisualBasic
Imports System.Runtime.InteropServices

'typedef struct _MYSTRSTRUCT2
'{
'	char* buffer;
'	UINT size; 
'} MYSTRSTRUCT2;

< StructLayout( LayoutKind.Sequential, CharSet:=CharSet.Ansi )> _
Public Class MyStruct 

	Public buffer As String 
	Public someSize As Integer
	
End Class 'MyStruct

Public Class LibWrap

	' void TestOutArrayOfStructs(int* pSize, MYSTRSTRUCT2** ppStruct);	
	
    Declare Sub TestOutArrayOfStructs Lib "PinvokeLib.dll" ( _
  ByRef arrSize As Integer, ByRef outArray As IntPtr)
		
End Class 'LibWrap

Public Class App

	Public Shared Sub Main()
	
		Console.WriteLine( ControlChars.CrLf & "Using marshal class" & ControlChars.CrLf )
		UsingMarshal()
		
		'VB.NET can't use unsafe code
		
	End Sub 'Main
	
	Public Shared Sub UsingMarshal()	
	
		Dim arrSize As Integer
		Dim outArray As IntPtr
		
		LibWrap.TestOutArrayOfStructs( arrSize, outArray )
		
		Dim manArray(arrSize - 1) As MyStruct
		
		Dim current As IntPtr = outArray
		Dim i As Integer
		
		For i = 0 To arrSize - 1
		
			manArray(i) = New MyStruct()
			Marshal.PtrToStructure( current, manArray(i))
			
			Marshal.DestroyStructure( current, GetType( MyStruct )) 
			current = IntPtr.op_explicit( current.ToInt32() + Marshal.SizeOf( manArray(i) ))
			
			Console.WriteLine( "Element {0}: {1} {2}", i, manArray(i).buffer, manArray(i).someSize )
		Next i
		Marshal.FreeCoTaskMem( outArray )
		
	End Sub 'UsingMarshal
	
End Class 'App


