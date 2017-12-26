'**************************** Module Header ******************************\
' Module Name: Default.aspx.vb
' Project:     VBASPNETDragItemInListView
' Copyright (c) Microsoft Corporation
'
' The project illustrates how to drag and drop items in ListView using JQuery.
' In this page, bind two xml data files to ListView and use ItemTemplate to display
' them, cite JQuery javascript library to implements these functions in 
' Default.aspx page.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'**************************************************************************/


Imports System.Xml

Public Class _Default
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        ' Bind two xml data file to ListView control, actually you can change the "open" property to "0",
        ' In that way, it will not display in ListView control.
        Dim xmlDocument As New XmlDocument()
        Using tabListView1 As New DataTable()
            tabListView1.Columns.Add("value", Type.[GetType]("System.String"))
            xmlDocument.Load(AppDomain.CurrentDomain.BaseDirectory + "/XmlFile/ListView1.xml")
            Dim xmlNodeList As XmlNodeList = xmlDocument.SelectNodes("root/data[@open='1']")
            For Each xmlNode As XmlNode In xmlNodeList
                Dim dr As DataRow = tabListView1.NewRow()
                dr("value") = xmlNode.InnerText
                tabListView1.Rows.Add(dr)
            Next
            ListView1.DataSource = tabListView1
            ListView1.DataBind()
        End Using

        Dim xmlDocument2 As New XmlDocument()
        Using tabListView2 As New DataTable()
            tabListView2.Columns.Add("value2", Type.[GetType]("System.String"))
            xmlDocument2.Load(AppDomain.CurrentDomain.BaseDirectory + "/XmlFile/ListView2.xml")
            Dim xmlNodeList2 As XmlNodeList = xmlDocument2.SelectNodes("root/data[@open='1']")
            For Each xmlNode As XmlNode In xmlNodeList2
                Dim dr As DataRow = tabListView2.NewRow()
                dr("value2") = xmlNode.InnerText
                tabListView2.Rows.Add(dr)
            Next
            ListView2.DataSource = tabListView2
            ListView2.DataBind()
        End Using

    End Sub

End Class