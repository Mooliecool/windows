'**************************** Module Header ******************************\
' Module Name: Default.aspx.vb
' Project:     VBASPNETControlPermissionForFiles
' Copyright (c) Microsoft Corporation
'
' The project illustrates how to control the permission for protect files and 
' folders on server from being download. Here we give a solution that when the
' web application receive a URL request, we will make a judgment that if the 
' request file's extension name is not .jpg file then redirect to 
' NoPermissionPage page. Also, user can not access the image file via copy URL. 
' 
' This page is used to bind ListView control with XML file, users can visit image 
' file by click links.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'****************************************************************************




Imports System.Xml

Public Class _Default
    Inherits System.Web.UI.Page
    ''' <summary>
    ''' Binding ListView control with XML files.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        Dim xmlDocument As New XmlDocument()
        xmlDocument.Load(AppDomain.CurrentDomain.BaseDirectory + "XmlFile/PermissionFilesXml.xml")
        Dim nodeList As XmlNodeList = xmlDocument.SelectNodes("Root/File")
        Dim tabNodes As New DataTable()
        tabNodes.Columns.Add("ID", Type.[GetType]("System.Int32"))
        tabNodes.Columns.Add("Name", Type.[GetType]("System.String"))
        tabNodes.Columns.Add("Path", Type.[GetType]("System.String"))
        For Each node As XmlNode In nodeList
            Dim drTab As DataRow = tabNodes.NewRow()
            drTab("ID") = node("ID").InnerText
            drTab("Name") = node("Name").InnerText
            drTab("Path") = node("FilePath").InnerText
            tabNodes.Rows.Add(drTab)
        Next
        ListView1.DataSource = tabNodes
        ListView1.DataBind()
    End Sub
End Class