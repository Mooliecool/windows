'*************************** Module Header ********************************\
'* Module Name:    CheckUserOnlinePage.aspx.vb
'* Project:        VBASPNETCurrentOnlineUserList
'* Copyright (c) Microsoft Corporation
'*
'* The Membership.GetNumberOfUsersOnline Method can get the number of online
'* users,however many asp.net projects are not using membership.This project
'* shows how to display a list of current online users' information without 
'* using membership provider.
'
'* This page is used to get request from other pages which contains the 
'* CheckUserOnline custom control and delete the records of user who is off 
'* line from CurrentOnlineUser table.
'
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\**************************************************************************

Public Class CheckUserOnlinePage
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        Check()
    End Sub
    Public Overridable Property SessionName() As String
        Get
            Dim _obj1 As Object = Me.ViewState("SessionName")
            If _obj1 IsNot Nothing Then
                Return DirectCast(_obj1, String).Trim()
            End If
            Return "Ticket"
        End Get
        Set(ByVal value As String)
            Me.ViewState("SessionName") = value
        End Set
    End Property
    Protected Sub Check()
        Dim _myTicket As String = ""
        If System.Web.HttpContext.Current.Session(Me.SessionName) IsNot Nothing Then
            _myTicket = System.Web.HttpContext.Current.Session(Me.SessionName).ToString()
        End If
        If _myTicket <> "" Then
            ' Initialize the datatable which used to store the information of
            ' current online user.
            Dim _onlinetable As New DataTableForCurrentOnlineUser()

            ' Update the time when the page refresh or the page get a request.
            _onlinetable.RefreshTime(_myTicket)
            Response.Write("OK：" & DateTime.Now.ToString())
        Else
            Response.Write("Sorry：" & DateTime.Now.ToString())
        End If
    End Sub
End Class
