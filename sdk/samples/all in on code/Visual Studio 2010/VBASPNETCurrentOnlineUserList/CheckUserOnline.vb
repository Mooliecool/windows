'*************************** Module Header ********************************\
'* Module Name:    CheckUserOnline.vb
'* Project:        VBASPNETCurrentOnlineUserList
'* Copyright (c) Microsoft Corporation
'*
'* The Membership.GetNumberOfUsersOnline Method can get the number of online
'* users,however many asp.net projects are not using membership.This project
'* shows how to display a list of current online users' information without 
'* using membership provider.
'
'* This class is used to add JavaScript code to the page.The JavaScript function
'* can check the user's active time and post a request to the CheckUserOnlinePage.aspx 
'* page.The project will auto delete the off line users'record from user's table by 
'* checking the last active time.
'
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\**************************************************************************

Imports System.ComponentModel

<Description("CheckUserOnline"), DefaultProperty(""), ToolboxData("<{0}:CheckUserOnline runat=server />")> _
Public Class CheckUserOnline
    Inherits System.Web.UI.WebControls.PlaceHolder
    ''' <summary>
    ''' Interval of refresh time，the default is 25.
    ''' </summary>
    Public Overridable Property RefreshTime() As Integer
        Get
            Dim _obj1 As Object = Me.ViewState("RefreshTime")
            If _obj1 IsNot Nothing Then
                Return Integer.Parse(DirectCast(_obj1, String).Trim())
            End If
            Return 25
        End Get
        Set(ByVal value As Integer)
            Me.ViewState("RefreshTime") = value
        End Set
    End Property
    Protected Overrides Sub Render(ByVal writer As HtmlTextWriter)
        ' Get the visiting address for xmlhttp form web.config.
        Dim _refreshUrl As String = DirectCast(ConfigurationManager.AppSettings("refreshUrl"), String)
        Dim _scriptString As String = " <script language=""JavaScript"">"
        _scriptString += writer.NewLine
        _scriptString += "   window.attachEvent(""onload"", " & Me.ClientID
        _scriptString += "_postRefresh);" & writer.NewLine
        _scriptString += "   var " & Me.ClientID & "_xmlhttp=null;"
        _scriptString += writer.NewLine
        _scriptString += "   function " & Me.ClientID & "_postRefresh(){"
        _scriptString += writer.NewLine
        _scriptString += "    var " & Me.ClientID
        _scriptString += "_xmlhttp = new ActiveXObject(""Msxml2.XMLHTTP"");"
        _scriptString += writer.NewLine
        _scriptString += "    " & Me.ClientID
        _scriptString += "_xmlhttp.Open(""POST"", """ & _refreshUrl & """, false);"
        _scriptString += writer.NewLine
        _scriptString += "    " & Me.ClientID & "_xmlhttp.Send();"
        _scriptString += writer.NewLine
        _scriptString += "    var refreshStr= " & Me.ClientID
        _scriptString += "_xmlhttp.responseText;"
        _scriptString += writer.NewLine

        _scriptString += "    try {"
        _scriptString += writer.NewLine
        _scriptString += "     var refreshStr2=refreshStr;"
        _scriptString += writer.NewLine
        _scriptString += "    }"
        _scriptString += writer.NewLine
        _scriptString += "    catch(e) {}"
        _scriptString += writer.NewLine
        _scriptString += "    setTimeout("""
        _scriptString += Me.ClientID & "_postRefresh()"","
        _scriptString += Me.RefreshTime.ToString() & "000);"
        _scriptString += writer.NewLine
        _scriptString += "   }" & writer.NewLine
        _scriptString += "<"
        _scriptString += "/"
        _scriptString += "script>" & writer.NewLine
        writer.Write(writer.NewLine)
        writer.Write(_scriptString)
        writer.Write(writer.NewLine)
        MyBase.Render(writer)
    End Sub
End Class

