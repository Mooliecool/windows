'**************************** Module Header ******************************\
' Module Name: Default.aspx.vb
' Project:     VBASPNETFixedHeaderGridView
' Copyright (c) Microsoft Corporation
'
' As we know, GridView usually has many rows with a vertical scroll bar. 
' When users scroll using vertical bar, the header of the GridView will  
' move and disappear. This sample illustrates how to fix the header
' of GridView via JQuery, and this approach can also cross multiple browsers
' at client side.
' 
' This .vb file only use to bind a data table to the GridView. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'****************************************************************************




Public Class _Default
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        ' Define a data table as GridView's data source.
        Dim tab As New DataTable()
        tab.Columns.Add("a", Type.[GetType]("System.String"))
        tab.Columns.Add("b", Type.[GetType]("System.String"))
        tab.Columns.Add("c", Type.[GetType]("System.String"))
        tab.Columns.Add("d", Type.[GetType]("System.String"))
        tab.Columns.Add("e", Type.[GetType]("System.String"))
        tab.Columns.Add("f", Type.[GetType]("System.String"))
        tab.Columns.Add("g", Type.[GetType]("System.String"))
        tab.Columns.Add("h", Type.[GetType]("System.String"))
        tab.Columns.Add("i", Type.[GetType]("System.String"))
        tab.Columns.Add("j", Type.[GetType]("System.String"))
        For i As Integer = 0 To 34
            Dim dr As DataRow = tab.NewRow()
            dr("a") = String.Format("row:{0} columns:a", i.ToString().PadLeft(2, "0"c))
            dr("b") = String.Format("row:{0} columns:b", i.ToString().PadLeft(2, "0"c))
            dr("c") = String.Format("row:{0} columns:c", i.ToString().PadLeft(2, "0"c))
            dr("d") = String.Format("row:{0} columns:d", i.ToString().PadLeft(2, "0"c))
            dr("e") = String.Format("row:{0} columns:e", i.ToString().PadLeft(2, "0"c))
            dr("f") = String.Format("row:{0} columns:f", i.ToString().PadLeft(2, "0"c))
            dr("g") = String.Format("row:{0} columns:g", i.ToString().PadLeft(2, "0"c))
            dr("h") = String.Format("row:{0} columns:h", i.ToString().PadLeft(2, "0"c))
            dr("i") = String.Format("row:{0} columns:i", i.ToString().PadLeft(2, "0"c))
            dr("j") = String.Format("row:{0} columns:j", i.ToString().PadLeft(2, "0"c))
            tab.Rows.Add(dr)
        Next

        ' Data bind method.
        gvwList.DataSource = tab
        gvwList.DataBind()
    End Sub

End Class