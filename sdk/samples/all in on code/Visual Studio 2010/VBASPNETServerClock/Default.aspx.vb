'*************************** Module Header ********************************\
' Module Name:    Default.aspx.vb
' Project:        VBASPNETServerClock
' Copyright (c) Microsoft Corporation
'
' This project illustrates how to get the time of the server side and show 
' it to the client page. Sometimes a website need to show an unified clock 
' on pages to all the visitors. However, if we use JavaScript to handle this
' target, the time will be different from each client. So we need the server
' to return the server time and refresh the clock per second via AJAX. 
'
' This page is used to get and show the time. Please refer to the Clock.aspx
' page to view the code which responses the time.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'**************************************************************************\


Public Class _Default
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

    End Sub
End Class