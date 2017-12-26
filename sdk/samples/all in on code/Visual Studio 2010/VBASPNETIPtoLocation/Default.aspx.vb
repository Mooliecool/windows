'*************************** Module Header ********************************\
' Module Name:    Default.aspx.vb
' Project:        VBASPNETIPtoLocation
' Copyright (c) Microsoft Corporation
'
' This project illustrates how to get the geographical location from a db file.
' You need install Sqlserver Express for run the web applicaiton. The code-sample
' only support Internet Protocol version 4.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*****************************************************************************/


Imports System.Net
Imports System.IO
Imports System.Data
Imports System.Data.SqlClient
Imports System.Configuration

Public Class _Default
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As EventArgs) Handles Me.Load
        Dim ipAddress As String

        ' Get the client's IP address.If you get the result of "::1", it's the IPv6 version
        ' of your IP address, you can disable IPv6 components if you want to get IPv4 version.
        ' And you need change the registry key when you disable it, check this link:
        ipAddress = Request.ServerVariables("HTTP_X_FORWARDED_FOR")
        If String.IsNullOrEmpty(ipAddress) Then
            ipAddress = Request.ServerVariables("REMOTE_ADDR")
        End If
        lbIPAddress.Text = "You IP Address is: [" & ipAddress & "]."
    End Sub

    Protected Sub btnSubmit_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnSubmit.Click
        Dim ipAddress As String = tbIPAddress.Text.Trim()
        Dim locationInfo As New Location()
        If String.IsNullOrEmpty(ipAddress) Then
            Response.Write("<strong>Please input an IP address</strong>")
            Return
        End If

        ' Get the IP address string and calculate IP number.
        Dim ipRange As String = IPConvert.ConvertToIPRange(ipAddress)
        Dim tabLocation As New DataTable()

        ' Create a connection to Sqlserver
        Using sqlConnection As New SqlConnection(ConfigurationManager.ConnectionStrings("ConectString").ToString())
            Dim selectCommand As String = "select * from IPtoLocation where CAST(" & ipRange & " as bigint) between BeginingIP and EndingIP"
            Dim sqlAdapter As New SqlDataAdapter(selectCommand, sqlConnection)
            sqlConnection.Open()
            sqlAdapter.Fill(tabLocation)
        End Using

        ' Store IP infomation into Location entity class
        If tabLocation.Rows.Count = 1 Then
            locationInfo.BeginIP = tabLocation.Rows(0)(0).ToString()
            locationInfo.EndIP = tabLocation.Rows(0)(1).ToString()
            locationInfo.CountryTwoCode = tabLocation.Rows(0)(2).ToString()
            locationInfo.CountryThreeCode = tabLocation.Rows(0)(3).ToString()
            locationInfo.CountryName = tabLocation.Rows(0)(4).ToString()
        Else
            Response.Write("<strong>Cannot find the location based on the IP address [" & ipAddress & "].</strong> ")
            Return
        End If

        ' Output.
        Response.Write("<strong>Country Code(Two):</strong> ")
        Response.Write(locationInfo.CountryTwoCode + "<br />")

        Response.Write("<strong>Country Code(Three):</strong> ")
        Response.Write(locationInfo.CountryThreeCode + "<br />")

        Response.Write("<strong>Country Name:</strong> ")
        Response.Write(locationInfo.CountryName + "<br />")

        lbIPAddress.Visible = False
    End Sub
End Class