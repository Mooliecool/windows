'*************************** Module Header ********************************\
'* Module Name:    DataTableForCurrentOnlineUser.vb
'* Project:        VBASPNETCurrentOnlineUserList
'* Copyright (c) Microsoft Corporation
'*
'* The Membership.GetNumberOfUsersOnline Method can get the number of online
'* users,however many asp.net projects are not using membership.This project
'* shows how to display a list of current online users' information without 
'* using membership provider.
'
'* This class is used to initialize the datatable which store the information
'* of current online users.
'
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\**************************************************************************



Public Class DataTableForCurrentOnlineUser
    Private Shared _activeusers As DataTable
    Private _activeTimeout As Integer
    Private _refreshTimeout As Integer
    ''' <summary>
    ''' Initialization of UserOnlineTable.
    ''' </summary> 
    Private Sub UsersTableFormat()
        If _activeusers Is Nothing Then
            _activeusers = New DataTable("ActiveUsers")
            Dim myDataColumn As DataColumn
            Dim mystringtype As System.Type
            mystringtype = System.Type.[GetType]("System.String")
            Dim mytimetype As System.Type
            mytimetype = System.Type.[GetType]("System.DateTime")
            myDataColumn = New DataColumn("Ticket", mystringtype)
            _activeusers.Columns.Add(myDataColumn)
            myDataColumn = New DataColumn("UserName", mystringtype)
            _activeusers.Columns.Add(myDataColumn)
            myDataColumn = New DataColumn("TrueName", mystringtype)
            _activeusers.Columns.Add(myDataColumn)
            myDataColumn = New DataColumn("RoleID", mystringtype)
            _activeusers.Columns.Add(myDataColumn)
            myDataColumn = New DataColumn("RefreshTime", mytimetype)
            _activeusers.Columns.Add(myDataColumn)
            myDataColumn = New DataColumn("ActiveTime", mytimetype)
            _activeusers.Columns.Add(myDataColumn)
            myDataColumn = New DataColumn("ClientIP", mystringtype)
            _activeusers.Columns.Add(myDataColumn)
        End If
    End Sub

    Public Sub New()
        ' Initialize the datatable which used to store the information
        ' of current online user.
        UsersTableFormat()

        ' Initialization of User's active time(minute).
        Try
            _activeTimeout = Integer.Parse(ConfigurationManager.AppSettings("ActiveTimeout"))
        Catch
            _activeTimeout = 60
        End Try

        ' Initialization of refresh time(minute).
        Try
            _refreshTimeout = Integer.Parse(ConfigurationManager.AppSettings("RefreshTimeout"))
        Catch
            _refreshTimeout = 1
        End Try
    End Sub
    Public ReadOnly Property ActiveUsers() As DataTable
        Get
            Return _activeusers.Copy()
        End Get
    End Property

    ''' <summary>
    ''' Sign in method.
    ''' </summary>
    Public Sub Login(ByVal user As UserEntity, ByVal singleLogin As Boolean)
        ' Clear the record of user who is off line.
        DelTimeOut()
        If singleLogin Then
            ' Let the user who is already login sign out.
            Me.Logout(user.UserName, False)
        End If
        Dim _myrow As DataRow
        Try
            _myrow = _activeusers.NewRow()
            _myrow("Ticket") = user.Ticket.Trim()
            _myrow("UserName") = user.UserName.Trim()
            _myrow("TrueName") = "" & user.TrueName.Trim()
            _myrow("RoleID") = "" & user.RoleID.Trim()
            _myrow("ActiveTime") = DateTime.Now
            _myrow("RefreshTime") = DateTime.Now
            _myrow("ClientIP") = user.ClientIP.Trim()
            _activeusers.Rows.Add(_myrow)
        Catch
            Throw
        End Try
        _activeusers.AcceptChanges()

    End Sub

    ''' <summary>
    ''' Sign out the user，depend on ticket or username.
    ''' </summary> 
    Private Sub Logout(ByVal strUserKey As String, ByVal byTicket As Boolean)
        ' Clear the record of user who is off line.
        DelTimeOut()
        strUserKey = strUserKey.Trim()
        Dim _strExpr As String
        _strExpr = If(byTicket, "Ticket='" & strUserKey & "'", "UserName='" & strUserKey & "'")
        Dim _curuser As DataRow()
        _curuser = _activeusers.[Select](_strExpr)
        If _curuser.Length > 0 Then
            For i As Integer = 0 To _curuser.Length - 1
                _curuser(i).Delete()
            Next
        End If
        _activeusers.AcceptChanges()
    End Sub

    ''' <summary>
    ''' Sign out the user depend on ticket.
    ''' </summary>
    ''' <param name="strTicket">the ticket of user</param>
    Public Sub Logout(ByVal strTicket As String)
        Me.Logout(strTicket, True)
    End Sub

    ''' <summary>
    ''' Clear the record of user who is off line.
    ''' </summary>
    Private Function DelTimeOut() As Boolean
        Dim _strExpr As String
        _strExpr = "ActiveTime < '" & DateTime.Now.AddMinutes(0 - _activeTimeout) & "'or RefreshTime < '" & DateTime.Now.AddMinutes(0 - _refreshTimeout) & "'"
        Dim _curuser As DataRow()
        _curuser = _activeusers.[Select](_strExpr)
        If _curuser.Length > 0 Then
            For i As Integer = 0 To _curuser.Length - 1
                _curuser(i).Delete()
            Next
        End If
        _activeusers.AcceptChanges()
        Return True
    End Function

    ''' <summary>
    ''' Update the last active time of user.
    ''' </summary>
    Public Sub ActiveTime(ByVal strTicket As String)
        DelTimeOut()
        Dim _strExpr As String
        _strExpr = "Ticket='" & strTicket & "'"
        Dim _curuser As DataRow()
        _curuser = _activeusers.[Select](_strExpr)
        If _curuser.Length > 0 Then
            For i As Integer = 0 To _curuser.Length - 1
                _curuser(i)("ActiveTime") = DateTime.Now
                _curuser(i)("RefreshTime") = DateTime.Now
            Next
        End If
        _activeusers.AcceptChanges()
    End Sub

    ''' <summary>
    ''' Update the time when the page refresh or the page get a request.
    ''' </summary>
    Public Sub RefreshTime(ByVal strTicket As String)
        DelTimeOut()
        Dim _strExpr As String
        _strExpr = "Ticket='" & strTicket & "'"
        Dim _curuser As DataRow()
        _curuser = _activeusers.[Select](_strExpr)
        If _curuser.Length > 0 Then
            For i As Integer = 0 To _curuser.Length - 1
                _curuser(i)("RefreshTime") = DateTime.Now
            Next
        End If
        _activeusers.AcceptChanges()
    End Sub

    Private Function SingleUser(ByVal strUserKey As String, ByVal byTicket As Boolean) As UserEntity
        strUserKey = strUserKey.Trim()
        Dim _strExpr As String
        Dim user As New UserEntity()
        _strExpr = If(byTicket, "Ticket='" & strUserKey & "'", "UserName='" & strUserKey & "'")
        Dim _curuser As DataRow()
        _curuser = _activeusers.[Select](_strExpr)
        If _curuser.Length > 0 Then
            user.Ticket = DirectCast(_curuser(0)("Ticket"), String)
            user.UserName = DirectCast(_curuser(0)("UserName"), String)
            user.TrueName = DirectCast(_curuser(0)("TrueName"), String)
            user.RoleID = DirectCast(_curuser(0)("RoleID"), String)
            user.ActiveTime = CType(_curuser(0)("ActiveTime"), DateTime)
            user.RefreshTime = CType(_curuser(0)("RefreshTime"), DateTime)
            user.ClientIP = DirectCast(_curuser(0)("ClientIP"), String)
        Else
            user.UserName = ""
        End If
        Return user
    End Function

    ''' <summary>
    ''' Search the user by ticket.
    ''' </summary>
    Public Function SingleUser_byTicket(ByVal strTicket As String) As UserEntity
        Return Me.SingleUser(strTicket, True)
    End Function

    ''' <summary>
    ''' Search the user by username.
    ''' </summary>
    Public Function SingleUser_byUserName(ByVal strUserName As String) As UserEntity
        Return Me.SingleUser(strUserName, False)
    End Function

    ''' <summary>
    ''' Check whether the user is online by using ticket.
    ''' </summary>
    Public Function IsOnline_byTicket(ByVal strTicket As String) As Boolean
        Return CBool(Me.SingleUser(strTicket, True).UserName <> "")
    End Function

    ''' <summary>
    ''' Check whether the user is online by using username.
    ''' </summary>
    Public Function IsOnline_byUserName(ByVal strUserName As String) As Boolean
        Return CBool(Me.SingleUser(strUserName, False).UserName <> "")
    End Function
End Class

