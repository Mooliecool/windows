'****************************** Module Header ******************************\
' Module Name:    Default.aspx.cs
' Project:        VBASPNETShareSessionBetweenSubDomainsSite1
' Copyright (c) Microsoft Corporation
'
' This project demonstrates how to configure a SQL Server as SessionState and 
' make a module to share Session between two Web Sites with the same root domain.
' 
' This page is used to set value to Session and read value from Session to test 
' if Session value has been change by Web Site 2 or not.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Public Class _Default
    Inherits System.Web.UI.Page

    Protected Sub Page_PreRender(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.PreRender
        ' Display Session in Web Site 1.
        lbMsg.Text = DirectCast(Session("MySession"), String)
    End Sub

    Protected Sub btnSetSession_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnSetSession.Click
        ' Set Session in Web Site 1.
        Session("MySession") = "The Session content from Site1."
    End Sub
End Class