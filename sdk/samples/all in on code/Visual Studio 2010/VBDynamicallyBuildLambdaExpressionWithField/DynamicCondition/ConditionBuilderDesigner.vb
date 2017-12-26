'******************************** Module Header ************************************'
' Module Name:  ConditionBuilderDesigner.vb
' Project:      VBDynamicallyBuildLambdaExpressionWithField
' Copyright (c) Microsoft Corporation.
' 
' The ConditionBuilderDesigner.vb file defines some collections in order to 
' demonstrate how to integrating with other control
'
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'**********************************************************************************'

Imports System.ComponentModel
Imports System.ComponentModel.Design
Imports System.Windows.Forms.Design

' Designer used to display Smart Tags for ConditionBuilder
Friend Class ConditionBuilderDesigner
    Inherits ControlDesigner
    Private actions As New DesignerActionListCollection()

    ''' <summary>
    ''' override property for integrating PropertyGrid control
    ''' </summary>
    Public Overrides ReadOnly Property ActionLists() As DesignerActionListCollection
        Get
            If actions.Count = 0 Then
                actions.Add(New ConditionBuilderActionList(Me.Component))
            End If
            Return actions
        End Get
    End Property

    ''' <summary>
    ''' Provides the base class for types that define a list of items used to create 
    ''' a smart tag panel.
    ''' </summary>
    Public Class ConditionBuilderActionList
        Inherits DesignerActionList
        Private cBuilder As ConditionBuilder
        Public Sub New(ByVal component As IComponent)
            MyBase.New(component)
            cBuilder = CType(component, ConditionBuilder)
        End Sub

        ''' <summary>
        ''' Property Lines
        ''' </summary>
        Public Property Lines() As Integer
            Get
                Return cBuilder.Lines
            End Get
            Set(ByVal value As Integer)
                GetPropertyByName("Lines").SetValue(cBuilder, value)
            End Set
        End Property

        ''' <summary>
        ''' Property OperatorType
        ''' </summary>
        Public Property OperatorType() As ConditionBuilder.Compare
            Get
                Return cBuilder.OperatorType
            End Get
            Set(ByVal value As ConditionBuilder.Compare)
                GetPropertyByName("OperatorType").SetValue(cBuilder, value)
            End Set
        End Property

        ''' <summary>
        ''' Property Box
        ''' </summary>
        Private Function GetPropertyByName(ByVal propName As String) As PropertyDescriptor
            Dim prop = TypeDescriptor.GetProperties(cBuilder)(propName)
            If prop Is Nothing Then
                Throw New ArgumentException("Invalid Property.", propName)
            End If
            Return prop
        End Function

        ''' <summary>
        ''' Create the elements which will appear in the Smart Tag
        ''' </summary>
        Public Overrides Function GetSortedActionItems() As System.ComponentModel.Design.DesignerActionItemCollection
            Dim items As New DesignerActionItemCollection()
            items.Add(New DesignerActionHeaderItem("Appearance"))
            items.Add(New DesignerActionPropertyItem("Lines", "Number of Lines:", "Appearance", "Sets the number of lines in the ConditionBuilder"))
            items.Add(New DesignerActionPropertyItem("OperatorType", "Default Operator:", "Appearance", "Default operator to use"))
            Return items
        End Function
    End Class
End Class
