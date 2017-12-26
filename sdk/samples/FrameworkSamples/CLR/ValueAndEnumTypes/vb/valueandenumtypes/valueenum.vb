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
'/*=====================================================================
'  File:      ValueEnum.vb
'  Summary:   Demonstrates things you can do with ValueType/Enum types.
'=====================================================================*/

Option Explicit On 
Option Strict On


' Add the classes in the following namespaces to our namespace
Imports System
Imports System.ValueType
Imports System.Enum
Imports System.Object
Imports Microsoft.VisualBasic.Strings

Public Module App
    ' "Main" is application's entry point
    Sub Main()
        DemoValueTypes()
        DemoReferenceTypes()
        DemoEnums()
        DemoFlags()        
    End Sub

    'This is a value type because of "structure"
    Structure Point
        Dim x, y As Integer
                
        Sub New(ByVal x As Integer, ByVal y As Integer)
            Me.x = x
            Me.y = y
        End Sub


        Overrides Public Function ToString() As String  
          ToString = "(" & x & "," & y & ")"
        End Function
        
    End Structure

    Private Sub DemoValueTypes()
        Console.WriteLine("Demo start: Demo of value types.")

        Dim p1 As Point = New Point(5, 10)
        Dim p2 As Point = New Point(5, 10)
        Dim p3 As Point = New Point(3, 4)

        ' What type is this valuetype & what is it derived from
        Console.WriteLine("   The " & (p1.GetType().ToString) & " type is derived from " & (p1.GetType().BaseType.ToString))

        ' Value types compare for equality by comparing the fields
        Console.WriteLine("   Does p1 equal p1: " & p1.Equals(p1)) ' True
        Console.WriteLine("   Does p1 equal p2: " & p1.Equals(p2)) ' True
        Console.WriteLine("   Does p1 equal p3: " & p1.Equals(p3)) ' False
        Console.WriteLine("   p1=" & p1.ToString & ", p3=" & p3.ToString())        

        Console.WriteLine("Demo stop: Demo of value types.")

    End Sub

    Class Rectangle
        Dim x, y, width, height As Integer

        Sub New(ByRef x As Integer, ByRef y As Integer, ByRef width As Integer, ByRef height As Integer)
            Me.x = x
            Me.y = y
            Me.width = width
            Me.height = height
        End Sub

        Overrides Public Function ToString() As String
            ToString = "(" & x & "," & y & ")x(" & width & "," & height & ")"
        End Function

        Public Overloads Overrides Function Equals(ByVal o As Object) As Boolean
            ' Change the symantics of this reference type so that it is
            ' equal to the same type of object if the fields are equal.
            Console.WriteLine("   In Rectangle.Equals method")
            Dim r As Rectangle = CType(o, Rectangle)

            If (r.x = x And r.y = y And r.width = width And r.height = height) Then
                Equals = True
            Else
                Equals = False
            End If

        End Function

    End Class

    Private Sub DemoReferenceTypes()
        
        console.WriteLine()
        console.WriteLine()
        Console.WriteLine("Demo start: Demo of reference types.")
        Dim r As Rectangle = New Rectangle(1, 2, 3, 4)
  
        ' What type is this reference type & what is it derived from
        Console.WriteLine("   The " & r.GetType().ToString & " type is derived from " & r.GetType().BaseType.ToString)
        Console.WriteLine("   " & r.ToString)
        
        ' Reference types are equal if they refer to the same object
        Console.WriteLine("   Is r equivalent to (1, 2, 3, 4): " & (r Is New Rectangle(1, 2, 3, 4))) ' False
        Console.WriteLine("   Is r equal to (1, 2, 3, 4): " & (r.Equals(New Rectangle(1, 2, 3, 4)))) ' True
        Console.WriteLine("   Is r equivalent to (1, 1, 1, 1): " & (r Is New Rectangle(1, 1, 1, 1))) ' False
        Console.WriteLine("   Is r equal to (1, 1, 1, 1): " & (r.Equals(New Rectangle(1, 1, 1, 1)))) ' False

        Console.WriteLine("Demo stop: Demo of reference types.")
    End Sub

    ' This is an enumerated type because of 'enum'
    Enum Color
        Red = 111
        Green = 222
        Blue = 333
    End Enum

    Private Sub DemoEnums()
        
        Console.WriteLine()
        Console.Writeline()
        Console.WriteLine("Demo start: Demo of enumerated types.")
        Dim c As Color = CType(Color.Red, Color)

        ' What type is this enum & what is it derived from
        Console.WriteLine("   The " + c.GetType().ToString + " type is derived from " + c.GetType().BaseType.ToString)

        ' What is the underlying type used for the Enum's value
        Console.WriteLine("   Underlying type: " + GetUnderlyingType(c.GetType()).ToString)

        ' Display the set of legal enum values
        Dim o() As Color = CType(GetValues(c.GetType()), Color())
        Console.WriteLine()
        Console.WriteLine("   Number of valid enum values: " & o.length)
        
        Dim x As Integer
        For x = 0 To (o.Length - 1)
            Dim cc As Color = CType(o(x), Color)
            Console.WriteLine("   " & x & ": Name=" & chr(9) & cc.ToString() & Chr(9) & "     Number=" & System.Enum.Format(GetType(Color), cc, "d"))
        Next
       
        ' Check if a value is legal for this enum
        Console.WriteLine()
        Console.WriteLine("   111 is a valid enum value: " & IsDefined(c.GetType(), 111)) ' True
        Console.WriteLine("   112 is a valid enum value: " & IsDefined(c.GetType(), 112)) ' False

        ' Check if two enums are equal
        console.WriteLine()
        Console.WriteLine("   Is c equal to Red: " & (c.Equals(Color.Red))) ' True
        Console.WriteLine("   Is c equal to Blue: " & (c.Equals(Color.Blue))) ' False

        ' Display the enum's value as a string
        console.WriteLine()
        Console.WriteLine("   c's value as a string: " & c.ToString()) ' Red
        Console.WriteLine("   c's value as a number: " & System.Enum.Format(GetType(Color), c, "d")) ' 111

        ' Convert a string to an enum's value
        c = CType(Parse(GetType(Color), "Blue"), color)
        Try
            c = CType(Parse(GetType(Color), "NotAColor"), Color) ' Not valid, raises exception        
        Catch 
            Console.WriteLine("   'NotAColor' is not a valid value for this enum.")
        End Try

        ' Display the enum's value as a string
        console.WriteLine()
        Console.WriteLine("   c's value as a string: " & c.ToString()) ' Blue
        Console.WriteLine("   c's value as a number: " & System.Enum.Format(GetType(Color), c, "d")) ' 333

        Console.WriteLine("Demo stop: Demo of enumerated types.")
    End Sub

    
    <Flags()> _
    Enum ActionAttributes
        Read = 1
        Write = 2
        Delete = 4
        Query = 8
        Sync = 16
    End Enum

    Private Sub DemoFlags()
        Console.WriteLine()
        Console.WriteLine()
        Console.WriteLine("Demo start: Demo of enumerated flags types.")

        Dim aa As ActionAttributes = CType((ActionAttributes.Read Or ActionAttributes.Write Or ActionAttributes.Query), ActionAttributes)

        ' What type is this enum & what is it derived from
        Console.WriteLine("   The " & aa.GetType.ToString & " type is derived from " & aa.GetType.BaseType.ToString)

        ' What is the underlying type used for the Enum's value
        Console.WriteLine("   Underlying type: " & GetUnderlyingType(aa.GetType()).ToString)

        ' Display the set of legal enum values
        Dim o() As ActionAttributes = CType(GetValues(aa.GetType()), ActionAttributes())
        Console.WriteLine()
        Console.WriteLine("   Number of valid enum values: " & o.Length)

        Dim x As Integer
        For x = 0 To (o.Length - 1)
            Dim aax As ActionAttributes = CType(o(x), ActionAttributes)
            Console.WriteLine("   " & x & ": Name=" & Chr(9) & aax.ToString() & Chr(9) & Chr(9) & "Number=" & System.Enum.Format(GetType(ActionAttributes), aax, "d"))
        Next

        ' Check if a value is legal for this enum
        Console.WriteLine()
        Console.WriteLine("   8 is a valid enum value: " & IsDefined(aa.GetType(), 8)) ' True
        Console.WriteLine("   6 is a valid enum value: " & IsDefined(aa.GetType(), 6)) ' False

        ' Display the enum's value as a string
        Console.WriteLine()
        Console.WriteLine("   aa's value as a string: " & aa.ToString()) ' Read|Write|Query
        Console.WriteLine("   aa's value as a number: " & System.Enum.Format(GetType(ActionAttributes), aa, "d")) ' 11

        ' Convert a string to an enum's value
        aa = CType(Parse(GetType(ActionAttributes), "Write"), ActionAttributes)
        aa = aa Or CType(Parse(GetType(ActionAttributes), "Sync"), ActionAttributes)
        Console.WriteLine()
        Console.WriteLine("   aa's value as a string: " & aa.ToString()) ' Write|Sync
        Console.WriteLine("   aa's value as a number: " & System.Enum.Format(GetType(ActionAttributes), aa, "d")) ' 18

        Console.WriteLine("Demo stop: Demo of enumerated flags types.")
    End Sub

End Module

'End of File
