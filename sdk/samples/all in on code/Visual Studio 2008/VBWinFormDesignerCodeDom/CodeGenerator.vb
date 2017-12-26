'************************************* Module Header **************************************\
' Module Name:	CodeGenerator.vb
' Project:		VBWinFormDesignerCodeDom
' Copyright (c) Microsoft Corporation.
' 
' 
' This sample demonstrates how to add your own customized generate code for 
' control.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Imports System.ComponentModel.Design.Serialization
Imports System.ComponentModel
Imports System.CodeDom

#Region "MyComponent"
<DesignerSerializer(GetType(MyCodeGenerator), GetType(CodeDomSerializer))> _
Public Class MyComponent
    Inherits Component

    Private _myList As List(Of String) = New List(Of String)

    Public Sub New()
        Me._myList.Add("string1")
        Me._myList.Add("string2")
        Me._myList.Add("string3")
        Me._myList.Add("string4")
        Me._myList.Add("string5")
    End Sub

    Public Property MyList() As List(Of String)
        Get
            Return Me._myList
        End Get
        Set(ByVal value As List(Of String))
            Me._myList = value
        End Set
    End Property
End Class
#End Region

#Region "MyCodeGenerator"
Public Class MyCodeGenerator
    Inherits CodeDomSerializer

    Public Overrides Function Deserialize(ByVal manager As IDesignerSerializationManager, _
                                          ByVal codeObject As Object) As Object
        Dim baseClassSerializer As CodeDomSerializer = _
        DirectCast(manager.GetSerializer(GetType(MyComponent).BaseType, _
                                GetType(CodeDomSerializer)), CodeDomSerializer)
        Return baseClassSerializer.Deserialize(manager, codeObject)
    End Function

    Public Overrides Function Serialize(ByVal manager As IDesignerSerializationManager, _
                                        ByVal value As Object) As Object
        Dim codeObject As Object = DirectCast(manager.GetSerializer(GetType(MyComponent).BaseType, _
                            GetType(CodeDomSerializer)), CodeDomSerializer).Serialize(manager, value)
        If TypeOf codeObject Is CodeStatementCollection Then
            Dim statements As CodeStatementCollection = _
               DirectCast(codeObject, CodeStatementCollection)

            ' Initial MyList
            ' Generate "myComponent1.MyList = new System.Collections.Generic.List(Of String)();"
            Dim objectCreate1 As New  _
                     CodeObjectCreateExpression("System.Collections.Generic.List(Of String)", _
                            New CodeExpression(0 - 1) {})
            Dim as2 As New CodeAssignStatement(New  _
              CodeVariableReferenceExpression((manager.GetName(value) & ".MyList")), objectCreate1)
            statements.Insert(0, as2)

            ' Add my generated code comment
            Dim commentText As String = "MyList generation code"
            Dim comment As New CodeCommentStatement(commentText)
            statements.Insert(1, comment)

            ' Add items to MyList
            ' Generate the following code
            ' Me.myComponent1.MyList.Add("string5")
            ' Me.myComponent1.MyList.Add("string4")
            ' Me.myComponent1.MyList.Add("string3")
            ' Me.myComponent1.MyList.Add("string2")
            ' Me.myComponent1.MyList.Add("string1")
            Dim myCom As MyComponent = DirectCast(manager.GetInstance(manager.GetName(value)), MyComponent)
            Dim i As Integer
            For i = 0 To myCom.MyList.Count - 1
                ' targetObject that contains the method to invoke.
                Dim methodInvoke As New CodeMethodInvokeExpression(New CodeThisReferenceExpression, _
                     (manager.GetName(value) & ".MyList.Add"), _
                     New CodeExpression() {New CodePrimitiveExpression(myCom.MyList.Item(i))})
                Dim expressionStatement As New CodeExpressionStatement(methodInvoke)
                statements.Insert(2, expressionStatement)
            Next i
            statements.RemoveAt((statements.Count - 1))
        End If
        Return codeObject
    End Function

End Class
#End Region

