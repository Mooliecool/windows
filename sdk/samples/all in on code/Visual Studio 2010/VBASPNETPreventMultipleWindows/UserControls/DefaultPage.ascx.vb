'****************************** Module Header ******************************\
' Module Name: DefaultPage.ascx.vb
' Project:     VBASPNETPreventMultipleWindows
' Copyright (c) Microsoft Corporation
'
' This is a user-control for start page.It will get a random  
' string and assign it to window.name.At last,jump to 
' Main.aspx page.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'*****************************************************************************/


Public Class DefaultPage
    Inherits System.Web.UI.UserControl

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

    End Sub


    '/ <summary>
    '/ This method will get a random string when it been invoked,
    '/ and stored it in session
    '/ </summary>
    '/ <returns>return this random string</returns>
    Public Function GetWindowName() As String
        Dim WindowName As String = Guid.NewGuid().ToString().Replace("-", "")
        Session("WindowName") = WindowName
        Return WindowName
    End Function

End Class