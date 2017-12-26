'*********************************** Module Header **************************************\
' Module Name:	MainForm.vb
' Project:		VBWinFormBindToNestedProperties
' Copyright (c) Microsoft Corporation.
' 
' The sample demonstrates how to bind a DataGridView column to a nested property
' in the data source.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*****************************************************************************************/

Imports System.ComponentModel

Public Class MainForm
    Inherits Form

    Private mylist As New BindingList(Of Person)

    Public Sub New()
        MyBase.New()
        InitializeComponent()
    End Sub

    Private Sub Form1_Load(ByVal sender As Object, ByVal e As EventArgs) Handles MyBase.Load
        ' bind the DataGridView to the list
        Me.DataGridView1.AutoGenerateColumns = False
        Me.DataGridView1.DataSource = mylist

        Me.DataGridView1.Columns.Add("ID", "ID")
        Me.DataGridView1.Columns.Add("Name", "Name")
        Me.DataGridView1.Columns.Add("CityName", "City Name")
        Me.DataGridView1.Columns.Add("PostCode", "Post Code")

        CType(Me.DataGridView1.Columns("ID"), DataGridViewTextBoxColumn).DataPropertyName = "ID"
        CType(Me.DataGridView1.Columns("Name"), DataGridViewTextBoxColumn).DataPropertyName = "Name"
        CType(Me.DataGridView1.Columns("CityName"), DataGridViewTextBoxColumn).DataPropertyName = "HomeAddr_CityName"
        CType(Me.DataGridView1.Columns("PostCode"), DataGridViewTextBoxColumn).DataPropertyName = "HomeAddr_PostCode"
    End Sub

    Private Sub button1_Click(ByVal sender As Object, ByVal e As EventArgs) Handles Button1.Click
        ' add objects of type Person to a list            
        Dim p As New Person
        p.ID = "1"
        p.Name = "aa"

        Dim addr As New Address
        addr.Cityname = "city  name1"
        addr.Postcode = "post code1"
        p.HomeAddr = addr

        mylist.Add(p)

        p = New Person
        p.ID = "2"
        p.Name = "bb"

        addr = New Address
        addr.Cityname = "city name2"
        addr.Postcode = "post code2"
        p.HomeAddr = addr

        mylist.Add(p)
    End Sub
End Class


