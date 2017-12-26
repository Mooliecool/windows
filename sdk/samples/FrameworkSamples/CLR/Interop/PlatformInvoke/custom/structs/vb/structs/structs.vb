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

' Structs.vb

Imports System
Imports Microsoft.VisualBasic
Imports System.Runtime.InteropServices

'typedef struct _MYPERSON
'{
'	char* first; 
'	char* last; 
'} MYPERSON, *LP_MYPERSON;

< StructLayout( LayoutKind.Sequential, CharSet := CharSet.Ansi )> _
Public Structure MyPerson

	Public first As String
	Public last As String
	
End Structure 'MyPerson

'typedef struct _MYPERSON2
'{
'	MYPERSON* person;
'	int age; 
'} MYPERSON2, *LP_MYPERSON2;

< StructLayout( LayoutKind.Sequential )> _
Public Structure MyPerson2

	Public person As IntPtr
	Public age As Integer
	
End Structure 'MyPerson2

'typedef struct _MYPERSON3
'{
'	MYPERSON person;
'	int age; 
'} MYPERSON3;

< StructLayout( LayoutKind.Sequential )> _
Public Structure MyPerson3

	Public person As MyPerson
	Public age As Integer
	
End Structure 'MyPerson3

'typedef struct _MYARRAYSTRUCT
'{
'	bool flag;
'	int vals[ 3 ]; 
'} MYARRAYSTRUCT;

< StructLayout( LayoutKind.Sequential )> _
Public Structure MyArrayStruct 

	Public flag As Boolean
	< MarshalAs( UnmanagedType.ByValArray, SizeConst:=3 )> _
	public vals As Integer()
	
End Structure 'MyArrayStruct

Public Class LibWrap
   
	' int TestStructInStruct(MYPERSON2* pPerson2);
	
    Declare Function TestStructInStruct Lib "PinvokeLib.dll" ( _
  ByRef person2 As MyPerson2) As Integer
	
	' void TestStructInStruct3(MYPERSON3 person3)
	
    Declare Function TestStructInStruct3 Lib "PinvokeLib.dll" ( _
  ByVal person3 As MyPerson3) As Integer
		
	' void TestArrayInStruct( MYARRAYSTRUCT* pStruct );
	
    Declare Function TestArrayInStruct Lib "PinvokeLib.dll" ( _
  ByRef myStruct As MyArrayStruct) As Integer

End Class 'LibWrap

Public Class App
	Public Shared Sub Main()
   
		' ******************* structure with pointer to other structure ************
		Dim personName As MyPerson
		personName.first = "Mark"
		personName.last = "Lee"
		
		Dim personAll As MyPerson2
		personAll.age = 30
		
		Dim buffer As IntPtr = Marshal.AllocCoTaskMem( Marshal.SizeOf( personName ))
		Marshal.StructureToPtr( personName, buffer, False )
		
		personAll.person = buffer
		
		Console.WriteLine( ControlChars.CrLf & "Person before call:" )
		Console.WriteLine( "first = {0}, last = {1}, age = {2}", personName.first, _
			personName.last, personAll.age )
		
		Dim res As Integer = LibWrap.TestStructInStruct( personAll )
		
		Dim personRes As MyPerson = _
			CType( Marshal.PtrToStructure( personAll.person, GetType( MyPerson )), MyPerson )
		
		Marshal.FreeCoTaskMem( buffer )
		
		Console.WriteLine( "Person after call:" )
		Console.WriteLine( "first = {0}, last = {1}, age = {2}", personRes.first, _
			personRes.last, personAll.age )
		
		' ******************* structure with embedded structure ************	
		Dim person3 As New MyPerson3()
		person3.person.first = "John"
		person3.person.last = "Evans"
		person3.age = 27
		
		LibWrap.TestStructInStruct3( person3 )
		
		' ******************* structure with embedded array ************	
		Dim myStruct As New MyArrayStruct()
		
		myStruct.flag = False
		Dim array( 2 ) As Integer
		myStruct.vals = array
		myStruct.vals( 0 ) = 1
		myStruct.vals( 1 ) = 4
		myStruct.vals( 2 ) = 9
		
		Console.WriteLine( ControlChars.CrLf & "Structure with array before call:" )
		Console.WriteLine( myStruct.flag )
		Console.WriteLine( "{0} {1} {2}", myStruct.vals( 0 ), _
			myStruct.vals( 1 ), myStruct.vals( 2 ) )
		
		LibWrap.TestArrayInStruct( myStruct )
		
		Console.WriteLine( ControlChars.CrLf & "Structure with array after call:" )
		Console.WriteLine( myStruct.flag )
		Console.WriteLine( "{0} {1} {2}", myStruct.vals( 0 ), _
			myStruct.vals( 1 ), myStruct.vals( 2 ) )			
		
   End Sub 'Main
End Class 'App
