'**************************** Module Header ********************************\\
' Module Name:  MyUserControl.ascx.vb
' Project:      VBASPNETuserControlEventsExpose 
' Copyright (c) Microsoft Corporation.
' 
' This usercontrol declare the delagate and event as public.
' Thus the event will be subsribed by the webpage.
' When btnTest button is clicked, it will fire the event named 'MyEvent'.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\***************************************************************************/

Partial Public Class MyUserControl
    Inherits System.Web.UI.UserControl

    Public Delegate Sub MyEventHandler(ByVal sender As Object, ByVal e As EventArgs)
    Public Event MyEvent As MyEventHandler

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

    End Sub

    Protected Sub btnTest_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnTest.Click
        RaiseEvent MyEvent(sender, e)
        Response.Write("User Control’s Button Click <BR/>")
    End Sub
End Class