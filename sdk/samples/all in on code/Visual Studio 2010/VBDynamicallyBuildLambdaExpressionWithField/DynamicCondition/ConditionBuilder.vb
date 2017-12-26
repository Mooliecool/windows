'******************************** Module Header ***********************************'
' Module Name:  ConditionBuilder.vb
' Project:      VBDynamicallyBuildLambdaExpressionWithField
' Copyright (c) Microsoft Corporation.
'
' The ConditionBuilder.vb file defines a UserControl for first condition
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

''' <summary>
''' Designer metadata
''' </summary>
<Designer(GetType(ConditionBuilderDesigner))>
Partial Public Class ConditionBuilder

    Public Sub New()
        InitializeComponent()
    End Sub


#Region "Properties"
    Private Const cName As String = "ConditionLine"

    ''' <summary>
    ''' enum for definition of condition type
    ''' </summary>
    Public Enum Compare As Integer
        [And] = DynamicCondition.DynamicQuery.Condition.Compare.And
        [Or] = DynamicCondition.DynamicQuery.Condition.Compare.Or
    End Enum

    Private _lines As Integer = 1
    Private _type As Type
    Private _operatorType As Compare = Compare.And

    ''' <summary>
    ''' The number of ConditionLine controls to display
    ''' </summary>
    Public Property Lines() As Integer
        Get
            Return _lines
        End Get
        Set(ByVal value As Integer)
            If value < 1 Then
                Throw New ArgumentException("Lines cannot be less than 1")
            End If

            If value > _lines Then
                ' Create the new ConditionLines
                For i = _lines To value - 1
                    Dim cLine As ConditionLine = New ConditionLine With {.Name = cName & (i + 1), .Left = ConditionLine1.Left, .Width = ConditionLine1.Width, .Top = ConditionLine1.Top + i * (ConditionLine1.Height + 1), .OperatorType = CType(_operatorType, DynamicQuery.Condition.Compare), .Anchor = AnchorStyles.Left Or AnchorStyles.Top Or AnchorStyles.Right}

                    Me.Controls.Add(cLine)
                Next i

            ElseIf value < _lines Then

                ' Remove the extra ConditionLines
                For i = value To _lines
                    Me.Controls.RemoveByKey(cName & (i + 1))
                Next i

            End If
            _lines = value
        End Set
    End Property

    ''' <summary>
    ''' Default operator (And/Or) to be used for each ConditionLine
    ''' </summary>
    Public Property OperatorType() As Compare
        Get
            Return _operatorType
        End Get
        Set(ByVal value As Compare)
            _operatorType = value
            For i = 1 To _lines
                GetConditionLine(cName & i).OperatorType = CType(value, DynamicQuery.Condition.Compare)
            Next i
        End Set
    End Property
#End Region

#Region "Public Methods"
    ''' <summary>
    ''' Populates the dropdowns with the columns contained in dataSource.  dataSource can
    ''' be either an IEnumerable(Of T) (for local), or an IQueryable(Of T) (for remote)
    ''' </summary>
    Public Sub SetDataSource(Of T)(ByVal dataSource As IEnumerable(Of T))
        _type = GetType(T)

        ' Filter out all properties that are not intrinsic types
        ' For example, a Customers object may have an Orders property of type 
        ' EntityRef(Of Order), but it's not meaningful to display this property in the list.
        ' Note that the underlying Condition API does support nested property access, but
        ' it's just the ConditionBuilder control doesn't give the user a mechanism to
        ' specify it.
        Dim props = From p In _type.GetProperties()
                    Where DynamicCondition.ConditionLine.GetSupportedTypes().Contains(p.PropertyType)
                    Select p

        ' Load the columns into each ConditionLine
        For i = 1 To _lines
            GetConditionLine(cName & i).DataSource = CType(props.ToArray().Clone(), System.Reflection.PropertyInfo())
        Next i
    End Sub



    ''' <summary>
    ''' Use this method to get a condition object which represents all the data the user
    ''' has entered into the ConditionBuilder.
    ''' </summary>
    Public Function GetCondition(Of T)() As DynamicQuery.Condition(Of T)

        ' This is just used to infer the type, so there's no need to instantiate it
        Dim dataSrc As T = Nothing
        Dim finalCond = GetConditionLine(cName & "1").GetCondition(Of T)(dataSrc)

        ' Extract the condition from each ConditionLine and then combine it with finalCond
        For i = 2 To _lines
            Dim cLine = GetConditionLine(cName & i)
            finalCond = DynamicCondition.DynamicQuery.Condition.Combine(Of T)(finalCond, cLine.OperatorType, cLine.GetCondition(Of T)(dataSrc))
        Next i

        Return finalCond
    End Function

#End Region

#Region "Private Methods"
    ''' <summary>
    ''' Takes in "ConditionLine2" and returns the actual instance of the control
    ''' </summary>
    Private Function GetConditionLine(ByVal name As String) As ConditionLine
        Return CType(Me.Controls(name), ConditionLine)
    End Function

    ''' <summary>
    ''' Execution when Loading ConditionBuilder 
    ''' </summary>
    Private Sub ConditionBuilder_Load(ByVal sender As Object, ByVal e As EventArgs) Handles MyBase.Load
        Me.ConditionLine1.lb.Visible = False
    End Sub
#End Region

End Class
