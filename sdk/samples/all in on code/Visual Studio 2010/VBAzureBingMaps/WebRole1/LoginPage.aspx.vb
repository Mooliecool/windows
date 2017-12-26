'***************************** Module Header ******************************\
'* Module Name:	LoginPage.aspx.vb
'* Project:		AzureBingMaps
'* Copyright (c) Microsoft Corporation.
'* 
'* The code behind for LoginPage.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************


Imports System.Web
Imports Microsoft.Live

Partial Public Class LoginPage
    Inherits System.Web.UI.Page
    Protected Sub Page_Load(ByVal sender As Object, ByVal e As EventArgs)
        ' Store the return page in session.
        If Request.QueryString("returnpage") IsNot Nothing Then
            Session("ReturnPage") = Request.QueryString("returnpage")
        End If
    End Sub

    ''' <summary>
    ''' Windows Live Messenger Connect session ID.
    ''' </summary>
    Public ReadOnly Property SessionId() As String
        Get
            Dim oauth As New SessionIdProvider()
            Return "wl_session_id=" & oauth.GetSessionId(HttpContext.Current)
        End Get
    End Property
End Class