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

' Arrays.vb

Imports System
Imports Microsoft.VisualBasic
Imports System.Runtime.InteropServices

Namespace Microsoft.Samples
    <StructLayout(LayoutKind.Sequential)> _
    Public Structure MyPoint
        Implements IComparable
        Private _x As Integer
        Private _y As Integer

        Public Sub New(ByVal x As Integer, ByVal y As Integer)
            _x = x
            _y = y
        End Sub 'New

        Public Property X() As Integer
            Get
                Return _x
            End Get
            Set(ByVal value As Integer)
                _x = value
            End Set
        End Property

        Public Property Y() As Integer
            Get
                Return _y
            End Get
            Set(ByVal value As Integer)
                _y = value
            End Set
        End Property

        Public Overrides Function Equals(ByVal obj As Object) As Boolean
            Return CompareTo(obj) = 0
        End Function

        Public Function CompareTo(ByVal obj As Object) As Integer Implements IComparable.CompareTo
            Dim mp As MyPoint = CType(obj, MyPoint)
            Dim result As Integer = _x.CompareTo(mp.X)
            If result <> 0 Then
                Return result
            End If

            Return _y.CompareTo(mp.Y)
        End Function

        ' Omitting getHashCode violates FxCop rule: EqualsOverridesRequireGetHashCodeOverride.
        Public Overrides Function GetHashCode() As Integer
            Return _x.GetHashCode() Xor _y.GetHashCode()
        End Function

    End Structure 'MyPoint

    'typedef struct _MYPERSON
    '{
    '	char* first; 
    '	char* last; 
    '} MYPERSON;

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi)> _
    Public Structure MyPerson
        Implements IComparable
        Private _firstName As String
        Private _lastName As String

        Public Sub New(ByVal firstName As String, ByVal lastName As String)
            Me._firstName = firstName
            Me._lastName = lastName
        End Sub 'New

        Public Property FirstName() As String
            Get
                Return _firstName
            End Get
            Set(ByVal value As String)
                _firstName = value
            End Set
        End Property

        Public Property LastName() As String
            Get
                Return _lastName
            End Get
            Set(ByVal value As String)
                _lastName = value
            End Set
        End Property

        Public Overrides Function Equals(ByVal obj As Object) As Boolean
            Return CompareTo(obj) = 0
        End Function

        Public Function CompareTo(ByVal obj As Object) As Integer Implements IComparable.CompareTo
            Dim mp As MyPerson = CType(obj, MyPerson)
            Dim result As Integer = _firstName.CompareTo(mp.FirstName)
            If result <> 0 Then
                Return result
            End If

            Return _lastName.CompareTo(mp.LastName)
        End Function

        ' Omitting getHashCode violates FxCop rule: EqualsOverridesRequireGetHashCodeOverride.
        Public Overrides Function GetHashCode() As Integer
            Return _firstName.GetHashCode() Xor _lastName.GetHashCode()
        End Function
    End Structure 'MyPerson


    Friend NotInheritable Class NativeMethods

        Private Sub New()
        End Sub
        ' this way array size can't be changed and array can be copied back
        'int TestArrayOfInts(int* pArray, int pSize)

        Declare Function TestArrayOfInts Lib "PinvokeLib.dll" ( _
      <[In](), Out()> ByVal myArray() As Integer, ByVal size As Integer) As Integer

        'this way we could change array size, but array can't be copied back
        'since marshaler doesn't know resulting size, we must do this manually
        'int TestRefArrayOfInts(int** ppArray, int* pSize)

        Declare Function TestRefArrayOfInts Lib "PinvokeLib.dll" ( _
      ByRef myArray As IntPtr, ByRef size As Integer) As Integer

        'int TestMatrixOfInts(int pMatrix[][COL_DIM], int row)

        Declare Function TestMatrixOfInts Lib "PinvokeLib.dll" ( _
      <[In](), Out()> ByVal matrix(,) As Integer, ByVal row As Integer) As Integer

        'int TestArrayOfStrings(char** ppStrArray, int size)

        Declare Function TestArrayOfStrings Lib "PinvokeLib.dll" ( _
      <[In](), Out()> ByVal strArray() As String, ByVal size As Integer) As Integer

        'int TestArrayOfStructs(MYPOINT* pPointArray, int size)

        Declare Function TestArrayOfStructs Lib "PinvokeLib.dll" ( _
      <[In](), Out()> ByVal pointArray() As MyPoint, ByVal size As Integer) As Integer

        'without [In, Out] strings will not be copied out
        'int TestArrayOfStructs2(MYPERSON* pPersonArray, int size)

        Declare Function TestArrayOfStructs2 Lib "PinvokeLib.dll" ( _
      <[In](), Out()> ByVal personArray() As MyPerson, ByVal size As Integer) As Integer

    End Class 'LibWrap


    Public NotInheritable Class App
        Private Sub New()
        End Sub
        Public Shared Sub Main()

            ' *************** array ByVal ********************************
            Dim array1(9) As Integer

            Console.WriteLine("Integer array passed ByVal before call:")
            Dim i As Integer

            For i = 0 To array1.Length - 1
                array1(i) = i
                Console.Write(" " & array1(i))
            Next i

            Dim sum1 As Integer = NativeMethods.TestArrayOfInts(array1, array1.Length)
            Console.WriteLine(ControlChars.CrLf & "Sum of elements:" & sum1)

            Console.WriteLine(ControlChars.CrLf & "Integer array passed ByVal after call:")

            For Each i In array1
                Console.Write(" " & i)
            Next i

            ' *************** array ByRef ********************************
            Dim array2(9) As Integer
            Dim arraySize As Integer = array2.Length

            Console.WriteLine(ControlChars.CrLf & ControlChars.CrLf & _
             "Integer array passed ByRef before call:")

            For i = 0 To array2.Length - 1
                array2(i) = i
                Console.Write(" " & array2(i))
            Next i

            Dim buffer As IntPtr = Marshal.AllocCoTaskMem(Marshal.SizeOf(arraySize) * array2.Length)
            Marshal.Copy(array2, 0, buffer, array2.Length)

            Dim sum2 As Integer = NativeMethods.TestRefArrayOfInts(buffer, arraySize)
            Console.WriteLine(ControlChars.CrLf & "Sum of elements:" & sum2)

            If arraySize > 0 Then
                Dim arrayRes(arraySize - 1) As Integer
                Marshal.Copy(buffer, arrayRes, 0, arraySize)
                Marshal.FreeCoTaskMem(buffer)

                Console.WriteLine(ControlChars.CrLf & "Integer array passed ByRef after call:")
                For Each i In arrayRes
                    Console.Write(" " & i)
                Next i
            Else
                Console.WriteLine(ControlChars.CrLf & "Array after call is empty")
            End If

            ' *************** matrix ByVal ********************************
            Const [DIM] As Integer = 4
            Dim matrix([DIM], [DIM]) As Integer

            Console.WriteLine(ControlChars.CrLf & ControlChars.CrLf & "Matrix before call:")
            For i = 0 To [DIM]
                Dim j As Integer
                For j = 0 To [DIM]
                    matrix(i, j) = j
                    Console.Write(" " & matrix(i, j))
                Next j
                Console.WriteLine("")
            Next i

            Dim sum3 As Integer = NativeMethods.TestMatrixOfInts(matrix, [DIM] + 1)
            Console.WriteLine(ControlChars.CrLf & "Sum of elements:" & sum3)

            Console.WriteLine(ControlChars.CrLf & "Matrix after call:")
            For i = 0 To [DIM]
                Dim j As Integer
                For j = 0 To [DIM]
                    Console.Write(" " & matrix(i, j))
                Next j
                Console.WriteLine("")
            Next i

            ' *************** string array ByVal ********************************
            Dim strArray As String() = {"one", "two", "three", "four", "five"}

            Console.WriteLine(ControlChars.CrLf & ControlChars.CrLf & "String array before call:")
            Dim s As String
            For Each s In strArray
                Console.Write(" " & s)
            Next s

            Dim lenSum As Integer = NativeMethods.TestArrayOfStrings(strArray, strArray.Length)
            Console.WriteLine(ControlChars.CrLf & "Sum of string lengths:" & lenSum)

            Console.WriteLine(ControlChars.CrLf & "String array after call:")
            For Each s In strArray
                Console.Write(" " & s)
            Next s

            ' *************** struct array ByVal ********************************
            Dim points As MyPoint() = {New MyPoint(1, 1), New MyPoint(2, 2), New MyPoint(3, 3)}

            Console.WriteLine(ControlChars.CrLf & ControlChars.CrLf & "Points array before call:")
            Dim p As MyPoint
            For Each p In points
                Console.WriteLine("x = {0}, y = {1}", p.X, p.Y)
            Next p

            Dim allSum As Integer = NativeMethods.TestArrayOfStructs(points, points.Length)
            Console.WriteLine(ControlChars.CrLf & "Sum of points:" & allSum)

            Console.WriteLine(ControlChars.CrLf & "Points array after call:")
            For Each p In points
                Console.WriteLine("x = {0}, y = {1}", p.X, p.Y)
            Next p

            ' *************** struct with strings array ByVal ********************************
            Dim persons As MyPerson() = {New MyPerson("Kim", "Akers"), _
                    New MyPerson("Adam", "Barr"), _
                    New MyPerson("Jo", "Brown")}

            Console.WriteLine(ControlChars.CrLf & ControlChars.CrLf & "Persons array before call:")
            Dim pe As MyPerson
            For Each pe In persons
                Console.WriteLine("first = {0}, last = {1}", pe.FirstName, pe.LastName)
            Next pe

            Dim namesSum As Integer = NativeMethods.TestArrayOfStructs2(persons, persons.Length)
            Console.WriteLine(ControlChars.CrLf & "Sum of name lengths:" & namesSum)

            Console.WriteLine(ControlChars.CrLf & ControlChars.CrLf & "Persons array after call:")
            For Each pe In persons
                Console.WriteLine("first = {0}, last = {1}", pe.FirstName, pe.LastName)
            Next pe

        End Sub 'Main
    End Class 'App 
End Namespace