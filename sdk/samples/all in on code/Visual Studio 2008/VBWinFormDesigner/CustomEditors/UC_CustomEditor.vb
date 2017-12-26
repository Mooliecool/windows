'************************************* Module Header **************************************\
' Module Name:	UC_CustomEditor.vb
' Project:		VBWinFormDesigner
' Copyright (c) Microsoft Corporation.
' 
' 
' The CustomEditor sample demonstrates how to use a custom editor for a specific property 
' at design time. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'******************************************************************************************/

Imports System.ComponentModel
Imports System.Drawing.Design
Imports System.Windows.Forms.Design

Namespace CustomEditors
    Public Class UC_CustomEditor
        Private _cls As SubClass = New SubClass

        <Editor(GetType(MyEditor), GetType(UITypeEditor)), TypeConverter(GetType(ExpandableObjectConverter))> _
        Public Property Cls() As SubClass
            Get
                Return Me._cls
            End Get
            Set(ByVal value As SubClass)
                Me._cls = value
            End Set
        End Property

    End Class

    Public Class SubClass
        Private _date As DateTime = DateTime.Now
        Private _name As String

        Public Property MyDate() As DateTime
            Get
                Return Me._date
            End Get
            Set(ByVal value As DateTime)
                Me._date = value
            End Set
        End Property

        Public Property Name() As String
            Get
                Return Me._name
            End Get
            Set(ByVal value As String)
                Me._name = value
            End Set
        End Property
    End Class


    Public Class MyEditor
        Inherits UITypeEditor

        Private editorService As IWindowsFormsEditorService = Nothing

        Public Overrides Function EditValue(ByVal context As ITypeDescriptorContext, ByVal provider As IServiceProvider, ByVal value As Object) As Object
            If (Not provider Is Nothing) Then
                Me.editorService = TryCast(provider.GetService(GetType(IWindowsFormsEditorService)), IWindowsFormsEditorService)
            End If
            If (Not Me.editorService Is Nothing) Then
                Dim editorForm As EditorForm = New EditorForm
                editorForm.SubCls = DirectCast(value, SubClass)

                If (Me.editorService.ShowDialog(editorForm) = DialogResult.OK) Then
                    value = editorForm.SubCls
                End If
            End If
            Return value
        End Function

        Public Overrides Function GetEditStyle(ByVal context As ITypeDescriptorContext) As UITypeEditorEditStyle
            Return UITypeEditorEditStyle.Modal
        End Function

        Public Overrides Function GetPaintValueSupported(ByVal context As ITypeDescriptorContext) As Boolean
            Return False
        End Function

        Public Overrides Sub PaintValue(ByVal e As PaintValueEventArgs)
        End Sub

    End Class
End Namespace

