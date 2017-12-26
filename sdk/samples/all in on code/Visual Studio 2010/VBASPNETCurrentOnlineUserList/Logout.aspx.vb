'*************************** Module Header ********************************\
'* Module Name:    Logout.aspx.vb
'* Project:        VBASPNETCurrentOnlineUserList
'* Copyright (c) Microsoft Corporation
'*
'* The Membership.GetNumberOfUsersOnline Method can get the number of online
'* users,however many asp.net projects are not using membership.This project
'* shows how to display a list of current online users' information without 
'* using membership provider.
'
'* This page is used to let user sign out.  
'
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\**************************************************************************


Public Class Logout
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        ' Initialize the datatable which used to store the information
        ' of current online user.
        Dim _onlinetable As New DataTableForCurrentOnlineUser()
        If Me.Session("Ticket") IsNot Nothing Then
            ' Log out.
            _onlinetable.Logout(Me.Session("Ticket").ToString())
            Me.Session.Clear()
        End If
    End Sub

End Class