'************************************* Module Header **************************************\
' Module Name:	UC_CumstomPropertyTab.vb
' Project:		VBWinFormDesigner
' Copyright (c) Microsoft Corporation.
' 
' 
' The CustomPropertyTab sample demonstrates how to add custom PropertyTab on to the 
' Properties Windows 
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.

' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.ComponentModel
Imports System.Windows.Forms.PropertyGridInternal

Namespace CustomPropertyTab

    <PropertyTab(GetType(CustomTab), PropertyTabScope.Component)> _
    Partial Public Class UC_CustomPropertyTab
        Inherits UserControl

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        <CustomTabDisplayAttribute(True), Browsable(False)> _
        Public Property TestProp() As String
            Get
                Return Me._testProp
            End Get
            Set(ByVal value As String)
                Me._testProp = value
            End Set
        End Property

        'Private components As IContainer = Nothing
        Private _testProp As String
    End Class


    Public Class CustomTab
        Inherits PropertiesTab

        Public Overrides Function CanExtend(ByVal extendee As Object) As Boolean
            Dim test As Boolean = TypeOf extendee Is UC_CustomPropertyTab
            MessageBox.Show(test.ToString())
            Return TypeOf extendee Is UC_CustomPropertyTab
        End Function

        Public Overrides Function GetProperties(ByVal context As ITypeDescriptorContext, ByVal component As Object, ByVal attrs As Attribute()) As PropertyDescriptorCollection
            Return TypeDescriptor.GetProperties(component, New Attribute() {New BrowsableAttribute(False), New CustomTabDisplayAttribute(True)})
        End Function

        Public Overrides ReadOnly Property TabName() As String
            Get
                Return "Custom Tab"
            End Get
        End Property
    End Class


    <AttributeUsage(AttributeTargets.Property)> _
    Public Class CustomTabDisplayAttribute
        Inherits Attribute

        Public Sub New(ByVal display As Boolean)
            Me.Display = display
        End Sub

        Public Property Display() As Boolean
            Get
                Return Me._display
            End Get
            Set(ByVal value As Boolean)
                Me._display = value
            End Set
        End Property

        Private _display As Boolean
    End Class
End Namespace
