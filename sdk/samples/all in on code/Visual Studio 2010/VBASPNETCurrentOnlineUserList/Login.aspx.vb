'*************************** Module Header ********************************\
'* Module Name:    Login.aspx.vb
'* Project:        VBASPNETCurrentOnlineUserList
'* Copyright (c) Microsoft Corporation
'*
'* The Membership.GetNumberOfUsersOnline Method can get the number of online
'* users,however many asp.net projects are not using membership.This project
'* shows how to display a list of current online users' information without 
'* using membership provider.
'
'* This page is used to let user sign in. 
'
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\**************************************************************************


Public Class Login
    Inherits System.Web.UI.Page

    Protected Sub btnLogin_Click(ByVal sender As Object, ByVal e As EventArgs)
        Dim _error As String = ""

        ' Check the value of user's input data.
        If check_text(_error) Then
            ' Initialize the datatable which used to store the
            ' information of current online user.
            Dim _onLineTable As New DataTableForCurrentOnlineUser()

            ' An instance of user's entity.
            Dim _user As New UserEntity()
            _user.Ticket = DateTime.Now.ToString("yyyyMMddHHmmss")
            _user.UserName = tbUserName.Text.Trim()
            _user.TrueName = tbTrueName.Text.Trim()
            _user.ClientIP = Me.Request.UserHostAddress
            _user.RoleID = "MingXuGroup"

            ' Use session variable to store the ticket.
            Me.Session("Ticket") = _user.Ticket

            ' Log in.
            _onLineTable.Login(_user, True)
            Response.Redirect("CurrentOnlineUserList.aspx")
        Else
            Me.lbMessage.Visible = True
            Me.lbMessage.Text = _error
        End If
    End Sub
    Public Function check_text(ByRef errormessage As String) As Boolean
        errormessage = ""
        If Me.tbUserName.Text.Trim() = "" Then
            errormessage = "Please enter the username"
            Return False
        End If
        If Me.tbTrueName.Text.Trim() = "" Then
            errormessage = "Please enter the truename"
            Return False
        End If
        Return True
    End Function


End Class