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

' Void.vb
 
Imports System
Imports System.Text
Imports Microsoft.VisualBasic
Imports System.Runtime.InteropServices

Public Class LibWrap
   
	Public Enum DataType
		DT_I2 = 1
		DT_I4
		DT_R4
		DT_R8
		DT_STR
	End Enum 'DataType
   
   
	' void SetData(DataType typ, void* object)
	' using AsAny when void* is expected
	
    Declare Sub SetData Lib "PinvokeLib.dll" ( _
  ByVal t As DataType, <MarshalAs(UnmanagedType.AsAny)> ByVal o As Object)
		
	' using overloading when void* is expected
	
    Overloads Declare Sub SetData2 Lib "PinvokeLib.dll" Alias "SetData" ( _
  ByVal t As DataType, ByRef d As Double)

    Overloads Declare Sub SetData2 Lib "PinvokeLib.dll" Alias "SetData" ( _
  ByVal t As DataType, ByVal s As String)
							
End Class 'LibWrap


Public Class App
	Public Shared Sub Main()
	
		Console.WriteLine( "Calling SetData using AsAny..." + ControlChars.CrLf )
		
		LibWrap.SetData( LibWrap.DataType.DT_I2, CShort(12) )
		LibWrap.SetData( LibWrap.DataType.DT_I4, CLng(12) )
		LibWrap.SetData( LibWrap.DataType.DT_R4, CSng(12) )
		LibWrap.SetData( LibWrap.DataType.DT_R8, CDbl(12) )
		LibWrap.SetData( LibWrap.DataType.DT_STR, "abcd" )
		
		Console.WriteLine( ControlChars.CrLf + "Calling SetData using overloading..." )
		Console.WriteLine( ControlChars.CrLf )	
		
		Dim d As Double	= 12
		LibWrap.SetData2( LibWrap.DataType.DT_R8, d )
		LibWrap.SetData2( LibWrap.DataType.DT_STR, "abcd" )			
		
	End Sub 'Main
End Class 'App



