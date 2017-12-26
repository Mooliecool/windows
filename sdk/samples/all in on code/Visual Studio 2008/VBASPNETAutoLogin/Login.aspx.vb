'\**************************** Module Header ********************************\
' Module Name:  Login.aspx.vb
' Project:      VBASPNETAutoLogin
' Copyright (c) Microsoft Corporation.
' 
' This page is used to display the user's login information.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\***************************************************************************/

Partial Public Class Login
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

    End Sub


    Protected Sub LoginButton_Click(ByVal sender As Object, ByVal e As EventArgs) Handles LoginButton.Click
        Response.Clear()
        Response.Write("You have login the website." + "<br>")
        Response.Write("Your userName：" + Request.Form("UserName").ToString() + "<br>")
        Response.Write("Your passWord：" + Request.Form("Password").ToString() + "<br>")
        Response.End()
    End Sub
End Class