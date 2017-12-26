'*************************** Module Header ********************************\
'* Module Name:    CurrentOnlineUserList.aspx.vb
'* Project:        VBASPNETCurrentOnlineUserList
'* Copyright (c) Microsoft Corporation
'*
'* The Membership.GetNumberOfUsersOnline Method can get the number of online
'* users,however many asp.net projects are not using membership.This project
'* shows how to display a list of current online users' information without 
'* using membership provider.
'
'* This page is used to display the current online user's information. 
'
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\**************************************************************************

Public Class CurrentOnlineUserList
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        ' Check whether the user is login.
        CheckLogin()
    End Sub
    Public Sub CheckLogin()
        Dim _userticket As String = ""
        If Session("Ticket") IsNot Nothing Then
            _userticket = Session("Ticket").ToString()
        End If
        If _userticket <> "" Then
            ' Initialize the datatable which used to store the information
            ' of current online user.
            Dim _onlinetable As New DataTableForCurrentOnlineUser()

            ' Check whether the user is online by using ticket.
            If _onlinetable.IsOnline_byTicket(Me.Session("Ticket").ToString()) Then
                ' Update the last active time.
                _onlinetable.ActiveTime(Session("Ticket").ToString())

                ' Bind the datatable which used to store the information of 
                ' current online user to gridview control.
                gvUserList.DataSource = _onlinetable.ActiveUsers
                gvUserList.DataBind()
            Else
                ' If the current User is not exist in the table,then redirect
                ' the page to LogoOut.
                Response.Redirect("LogOut.aspx")
            End If
        Else
            Response.Redirect("Login.aspx")
        End If
    End Sub
End Class


