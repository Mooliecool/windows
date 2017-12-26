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
'=====================================================================
'  File:      CustAttr.vb
'
'  Summary:   Demonstrates how to create and use custom attributes.
'
'=====================================================================*/

Option Explicit On 
Option Strict On


Imports System
Imports System.Reflection
Imports Microsoft.VisualBasic.Strings

' This type defines a custom attribute

' The attributes on this attribute describe where the attribute is legal
' and how often the attribute can be applied to a single target
<AttributeUsageAttribute(AttributeTargets.All, AllowMultiple:=False)> _
Public Class MyAttribute
    Inherits System.Attribute  ' Attributes must derive from System.Attribute

    Public y As Integer = 111  'This is a public attribute
    Public s As String         'This is a public attribute
    Public x As Integer        'This is a public attribute

    ' This attribute requires the 2 positional parameters.
    Public Sub New(ByVal s As String, ByVal x As Integer)
        MyBase.New()

        Me.s = s
        Me.x = x
    End Sub

    Shared Sub DisplayAttrInfo(ByVal n As Integer, ByVal a As Attribute)

        If TypeOf (a) Is MyAttribute Then
            ' Refer to the one of the custom attributes
            Dim myAttribute As MyAttribute = CType(a, MyAttribute)

            Console.WriteLine("{0}-""{1}"": {2}", n, a, "X: " & myAttribute.x.ToString() & ", Y: " & myAttribute.y.ToString() & ", S: " & myAttribute.s)
        Else
            Console.WriteLine("{0}-""{1}""", n, a)

        End If
    End Sub
End Class


<Obsolete("Ignore this warning -- just testing the Obsolete attribute ")> _
Class OldType
End Class



' This type has our custom attribute applied to it.

' Apply our attribute to our type. the optional named parameter is used.
<MyAttribute("AttribOnType", 111, y:=222)> _
Class SampAttrApp
    ' Apply our attribute to the constructor. the optional parameter is NOT used.
    ' NOTE: For convenience, the .NET Framework allows "Attribute" to be omitted from "MyAttribute"
    <My("AttribOnMethod", 333)> _
    Sub New()
        MyBase.New()

        Console.WriteLine("In Application constructor")
    End Sub

    Shared Sub Main()

        ' Get the set of custom attributes associated with the type
        Dim TypeAttrs() As Object = GetType(SampAttrApp).GetCustomAttributes(False)

        Console.WriteLine("Number of custom attributes on Application type: " & TypeAttrs.Length)
        Dim n As Integer

        For n = 0 To TypeAttrs.Length - 1
            MyAttribute.DisplayAttrInfo(n, CType(TypeAttrs(n), Attribute))
        Next n

        ' Get the set of methods associated with the type
        Dim mi() As MemberInfo = GetType(SampAttrApp).FindMembers(MemberTypes.Constructor Or MemberTypes.Method, BindingFlags.Instance Or BindingFlags.Static Or BindingFlags.Public Or BindingFlags.NonPublic Or BindingFlags.DeclaredOnly, Type.FilterName, "*")
        Console.WriteLine("Number of methods (including constructors): " & mi.Length)

        Dim x As Integer
        For x = 0 To mi.Length - 1
            ' Get the set of custom attributes associated with this method
            Dim MethodAttrs() As Object = mi(x).GetCustomAttributes(False)

            Console.WriteLine("Method name: " & mi(x).Name & Chr(9) & "(" & MethodAttrs.Length & " attributes)")

            For n = 0 To MethodAttrs.Length - 1
                Console.Write("   ")
                MyAttribute.DisplayAttrInfo(n, CType(MethodAttrs(n), Attribute))
            Next n

        Next x

    End Sub

End Class

