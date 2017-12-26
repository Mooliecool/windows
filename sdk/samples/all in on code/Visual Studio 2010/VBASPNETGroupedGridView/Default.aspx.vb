'****************************** Module Header ******************************\
' Module Name:  Default.aspx.vb
' Project:      VBASPNETGroupedGridView
' Copyright (c) Microsoft Corporation
'
' The code sample shows how to group cells in GridView with the same value.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Public Class _Default
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        If Not IsPostBack Then
            BindSortedTestData(generalGridView)
            BindSortedTestData(groupedGridView)
        End If
    End Sub


    ''' <summary>
    ''' Bind sorted test data to the given GridView control.
    ''' </summary>
    ''' <param name="gridView">the GridView control</param>
    ''' <remarks></remarks>
    Private Sub BindSortedTestData(ByVal gridView As GridView)
        Const TestDataViewStateId As String = "TestData"

        Dim dt As DataTable = TryCast(ViewState(TestDataViewStateId), DataTable)
        If dt Is Nothing Then
            dt = New DataTable()
            dt.Columns.Add("Product Name", GetType(String))
            dt.Columns.Add("Category", GetType(Integer))
            dt.Columns.Add("Weight", GetType(Double))
            Dim r As New Random(DateTime.Now.Millisecond)

            For i As Integer = 1 To 50
                ' Adding ProductId, Category, and random price.
                dt.Rows.Add( _
                    "Product" & r.Next(1, 5), _
                    r.Next(1, 5), _
                    Math.Round(r.NextDouble() * 100 + 50, 2))
            Next

            ViewState(TestDataViewStateId) = dt
        End If

        ' Sort by Product Name and Category
        dt.DefaultView.Sort = "Product Name,Category"

        gridView.DataSource = dt
        gridView.DataBind()
    End Sub


    Protected Sub generalGridView_PageIndexChanging(ByVal sender As Object, ByVal e As GridViewPageEventArgs) Handles generalGridView.PageIndexChanging
        generalGridView.PageIndex = e.NewPageIndex
    End Sub

    Protected Sub generalGridView_PageIndexChanged(ByVal sender As Object, ByVal e As EventArgs) Handles generalGridView.PageIndexChanged
        BindSortedTestData(generalGridView)
    End Sub

    Protected Sub groupedGridView_PageIndexChanging(ByVal sender As Object, ByVal e As GridViewPageEventArgs) Handles groupedGridView.PageIndexChanging
        groupedGridView.PageIndex = e.NewPageIndex
    End Sub

    Protected Sub groupedGridView_PageIndexChanged(ByVal sender As Object, ByVal e As EventArgs) Handles groupedGridView.PageIndexChanged
        BindSortedTestData(groupedGridView)
    End Sub

End Class