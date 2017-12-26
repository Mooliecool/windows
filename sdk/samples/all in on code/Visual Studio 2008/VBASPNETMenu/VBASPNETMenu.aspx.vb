'****************************** Module Header ******************************\
' Module Name:  VBASPNETMenu.aspx.vb
' Project:      VBASPNETMenu
' Copyright (c) Microsoft Corporation.
'
' The VBASPNETMenu sample demonstrates how to bind a ASP.NET Menu Control
' to Database.
' 
' All other rights reserved.
'
' History:
' * 1/25/2010 11:00 AM Zong-Qing Li Created
'***************************************************************************

Partial Public Class VBASPNETMenu
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        If Not IsPostBack Then
            GenerateMenuItem()
        End If
    End Sub
    Public Sub GenerateMenuItem()
        ' Get the data from database.
        Dim ds As DataSet = GetData()

        For Each mainRow As DataRow In ds.Tables(0).Rows
            ' Load the records from the main table to the menu control.
            Dim masterItem As New MenuItem(mainRow("mainName").ToString())
            masterItem.NavigateUrl = mainRow("mainUrl").ToString()
            Menu1.Items.Add(masterItem)

            For Each childRow As DataRow In mainRow.GetChildRows("Child")
                ' According to the relation of the main table and the child table, load the data from the child table.
                Dim childItem As New MenuItem(DirectCast(childRow("childName"), String))
                childItem.NavigateUrl = childRow("childUrl").ToString()
                masterItem.ChildItems.Add(childItem)
            Next
        Next
    End Sub

    Public Function GetData() As DataSet
        ' In order to test, we use the memory tables as the datasource.
        Dim mainTB As New DataTable()
        Dim mainIdCol As New DataColumn("mainId")
        Dim mainNameCol As New DataColumn("mainName")
        Dim mainUrlCol As New DataColumn("mainUrl")
        mainTB.Columns.Add(mainIdCol)
        mainTB.Columns.Add(mainNameCol)
        mainTB.Columns.Add(mainUrlCol)

        Dim childTB As New DataTable()
        Dim childIdCol As New DataColumn("childId")
        Dim childNameCol As New DataColumn("childName")

        ' The MainId column of the child table is the foreign key to the main table.
        Dim childMainIdCol As New DataColumn("MainId")
        Dim childUrlCol As New DataColumn("childUrl")

        childTB.Columns.Add(childIdCol)
        childTB.Columns.Add(childNameCol)
        childTB.Columns.Add(childMainIdCol)
        childTB.Columns.Add(childUrlCol)


        ' Insert some test records to the main table.
        Dim dr As DataRow = mainTB.NewRow()
        dr(0) = "1"
        dr(1) = "Home"
        dr(2) = "test.aspx"
        mainTB.Rows.Add(dr)
        Dim dr1 As DataRow = mainTB.NewRow()
        dr1(0) = "2"
        dr1(1) = "Articles"
        dr1(2) = "test.aspx"
        mainTB.Rows.Add(dr1)
        Dim dr2 As DataRow = mainTB.NewRow()
        dr2(0) = "3"
        dr2(1) = "Help"
        dr2(2) = "test.aspx"
        mainTB.Rows.Add(dr2)
        Dim dr3 As DataRow = mainTB.NewRow()
        dr3(0) = "4"
        dr3(1) = "DownLoad"
        dr3(2) = "test.aspx"
        mainTB.Rows.Add(dr3)


        ' Insert some test records to the child table
        Dim dr5 As DataRow = childTB.NewRow()
        dr5(0) = "1"
        dr5(1) = "ASP.NET"
        dr5(2) = "2"
        dr5(3) = "test.aspx"
        childTB.Rows.Add(dr5)
        Dim dr6 As DataRow = childTB.NewRow()
        dr6(0) = "2"
        dr6(1) = "SQL Server"
        dr6(2) = "2"
        dr6(3) = "test.aspx"
        childTB.Rows.Add(dr6)
        Dim dr7 As DataRow = childTB.NewRow()
        dr7(0) = "3"
        dr7(1) = "JavaScript"
        dr7(2) = "2"
        dr7(3) = "test.aspx"
        childTB.Rows.Add(dr7)

        ' Use the DataSet to contain that two tables.
        Dim ds As New DataSet()
        ds.Tables.Add(mainTB)
        ds.Tables.Add(childTB)

        ' Build the relation between the main table and the child table.
        ds.Relations.Add("Child", ds.Tables(0).Columns("mainId"), ds.Tables(1).Columns("MainId"))


        Return ds
    End Function
End Class