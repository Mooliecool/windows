'******************************** Module Header *************************************'
' Module Name:  MainForm.vb
' Project:      VBDynamicallyBuildLambdaExpressionWithField
' Copyright (c) Microsoft Corporation.
'
' This sample demonstrates how to dynamically build lambda expression and show data 
' into DataGridView Control.
' 
' This sample shows up multiple conditions jointing together and dynamically 
' generate  LINQ TO SQL. LINQ is a great way to declaratively filter and query data 
' in a Type_Safe,Intuitive,and very expressive way.this sample achieve it. For example,
' the search feature in this application allow the customer to find all records that 
' meet criteria defined on multiple columns.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************************'

Imports DynamicCondition


Partial Public Class MainForm
    Private db As New NorthwindDataContext()

    ''' <summary>
    ''' Load the list of fields into the control when Winform loads.
    ''' </summary>
    Private Sub MainForm_Load(ByVal sender As Object, ByVal e As EventArgs) Handles MyBase.Load
        ' Load the list of fields into the control
        ConditionBuilder1.SetDataSource(db.Orders)
    End Sub

    ''' <summary>
    ''' Dynamically generate LINQ query and put it into DataGridView Control 
    ''' </summary>
    Private Sub btnSearch_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnSearch.Click
        ' Get the Condition out of the control
        Dim c = ConditionBuilder1.GetCondition(Of Order)()

        ' Filter out all Orders that don't match the Condition
        ' Note that the query does not actually get executed yet to due to deferred execution
        Dim filteredQuery = db.Orders.Where(c)

        ' We can now perform any other operations (such as Order By or Select) on filteredQuery
        Dim query = From row In filteredQuery
                    Order By row.OrderDate, row.OrderID
                    Select row

        ' Executes the query and displays the results in DataGridView1
        dgResult.DataSource = query
    End Sub

    ''' <summary>
    ''' Property DefaultInstance
    ''' </summary>
    Private Shared _defaultInstance As MainForm
    Public Shared ReadOnly Property DefaultInstance() As MainForm
        Get
            If _defaultInstance Is Nothing Then
                _defaultInstance = New MainForm()
            End If
            Return _defaultInstance
        End Get
    End Property
End Class

