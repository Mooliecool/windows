'*************************** Module Header ********************************\
'* Module Name:    UserEntity.vb
'* Project:        VBASPNETCurrentOnlineUserList
'* Copyright (c) Microsoft Corporation
'*
'* The Membership.GetNumberOfUsersOnline Method can get the number of online
'* users,however many asp.net projects are not using membership.This project
'* shows how to display a list of current online users' information without 
'* using membership provider.
'
'* This class is used as user's entity. 
'
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\**************************************************************************


Public Class UserEntity
    Public Sub New()
    End Sub
    ' Ticket.
    Private _ticket As String

    ' UserName.
    Private _username As String

    ' TrueName.
    Private _truename As String

    ' Role.
    Private _roleid As String

    ' Last refresh time of page.
    Private _refreshtime As DateTime

    ' Last active time of user.
    Private _activetime As DateTime

    ' Ip address of user.
    Private _clientip As String

    Public Property Ticket() As String
        Get
            Return _ticket
        End Get
        Set(ByVal value As String)
            _ticket = value
        End Set
    End Property
    Public Property UserName() As String
        Get
            Return _username
        End Get
        Set(ByVal value As String)
            _username = value
        End Set
    End Property
    Public Property TrueName() As String
        Get
            Return _truename
        End Get
        Set(ByVal value As String)
            _truename = value
        End Set
    End Property
    Public Property RoleID() As String
        Get
            Return _roleid
        End Get
        Set(ByVal value As String)
            _roleid = value
        End Set
    End Property
    Public Property RefreshTime() As DateTime
        Get
            Return _refreshtime
        End Get
        Set(ByVal value As DateTime)
            _refreshtime = value
        End Set
    End Property
    Public Property ActiveTime() As DateTime
        Get
            Return _activetime
        End Get
        Set(ByVal value As DateTime)
            _activetime = value
        End Set
    End Property
    Public Property ClientIP() As String
        Get
            Return _clientip
        End Get
        Set(ByVal value As String)
            _clientip = value
        End Set
    End Property
End Class

