'****************************** Module Header ******************************\
' Module Name: NextPage.ascx.vb
' Project:     VBASPNETPreventMultipleWindows
' Copyright (c) Microsoft Corporation
'
' This is a user-control for other page.It will get window name 
' and check whether allow this jump request
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'*****************************************************************************/

Public Class NextPage
    Inherits System.Web.UI.UserControl
    Dim InvalidPage As String = "InvalidPage"

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

    End Sub

    '/ <summary>
    '/  This method can get window name from Default.aspx
    '/ </summary>
    '/ <returns></returns>
    Public Function GetWindowName() As String
        If Not Session("WindowName") Is Nothing Then
            Dim WindowName As String = Session("WindowName").ToString()
            Return WindowName
        Else
            Return InvalidPage
        End If
    End Function

End Class