'****************************** Module Header ******************************\
' Module Name:    Receiver.aspx.vb
' Project:        VBASPNETReverseAJAX
' Copyright (c) Microsoft Corporation
'
' The user will use this page to log in with a unique user name. Then when there is a 
' new message on the server, the server will immediately push the message to the client.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Partial Public Class Receiver
    Inherits System.Web.UI.Page
    Protected Sub btnLogin_Click(ByVal sender As Object, ByVal e As EventArgs)
        Dim userName As String = tbUserName.Text.Trim()

        ' Join into the recipient list.
        If Not String.IsNullOrEmpty(userName) Then
            ClientAdapter.Instance.Join(userName)

            Session("userName") = userName
        End If
    End Sub

    Protected Sub Page_PreRender(ByVal sender As Object, ByVal e As EventArgs)
        ' Activate the JavaScript waiting loop.
        If Session("userName") IsNot Nothing Then
            Dim userName As String = DirectCast(Session("userName"), String)

            ' Call JavaScript method waitEvent to start the wait loop.
            ClientScript.RegisterStartupScript(Me.[GetType](), "ActivateWaitingLoop", "waitEvent();", True)

            lbNotification.Text = String.Format("Your user name is <b>{0}</b>. It is waiting for new message now.", userName)

            ' Disable the login.
            tbUserName.Visible = False
            btnLogin.Visible = False
        End If
    End Sub
End Class