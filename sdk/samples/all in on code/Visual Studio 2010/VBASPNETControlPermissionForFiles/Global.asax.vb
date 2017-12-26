'****************************** Module Header ******************************\
' Module Name: Global.asax.vb
' Project:     VBASPNETControlPermissionForFiles
' Copyright (c) Microsoft Corporation
'
' The project illustrates how to control the permission for protect files and 
' folders on server from being download. Here we give a solution that when the
' web application receive a URL request, we will make a judgment that if the 
' request file's extension name is not .jpg file then redirect to 
' NoPermissionPage page. Also, user can not access the image file via copy URL. 
' 
' This Gload.aspx.vb file can check the user's http request is correct or not.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*****************************************************************************/



Imports System.Web.SessionState

Public Class Global_asax
    Inherits System.Web.HttpApplication

    Sub Application_Start(ByVal sender As Object, ByVal e As EventArgs)
        ' Fires when the application is started
    End Sub

    Sub Session_Start(ByVal sender As Object, ByVal e As EventArgs)
        ' Fires when the session is started
    End Sub
    ''' <summary>
    ''' The Application_BeginRequest method is use to make a judgment whether the request file 
    ''' is jpg file, and throw illegal request to NoPermissionPage.aspx page.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Sub Application_BeginRequest(ByVal sender As Object, ByVal e As EventArgs)
        ' Fires at the beginning of each request
        Dim path As String = HttpContext.Current.Request.Path
        Dim pathElements As String() = path.Split("."c)
        Dim extenseName As String = pathElements(pathElements.Length - 1)
        If Not extenseName.Equals("aspx", StringComparison.OrdinalIgnoreCase) Then
            If Not extenseName.Equals("jpg", StringComparison.OrdinalIgnoreCase) OrElse Not IsUrl() Then
                HttpContext.Current.Response.Redirect("~/NoPermissionPage.aspx")
            End If
        End If

    End Sub

    ''' <summary>
    ''' The method is used to check whether the page is opened by typing the URL in browser  
    ''' </summary>
    ''' <returns></returns>
    Protected Function IsUrl() As Boolean
        Dim httpReferer As String = System.Web.HttpContext.Current.Request.ServerVariables("HTTP_REFERER")
        Dim serverName As String = System.Web.HttpContext.Current.Request.ServerVariables("SERVER_NAME")
        If (httpReferer IsNot Nothing) AndAlso (httpReferer.IndexOf(serverName) = 7) Then
            Return True
        Else
            Return False
        End If
    End Function


    Sub Application_AuthenticateRequest(ByVal sender As Object, ByVal e As EventArgs)
        ' Fires upon attempting to authenticate the use
    End Sub

    Sub Application_Error(ByVal sender As Object, ByVal e As EventArgs)
        ' Fires when an error occurs
    End Sub

    Sub Session_End(ByVal sender As Object, ByVal e As EventArgs)
        ' Fires when the session ends
    End Sub

    Sub Application_End(ByVal sender As Object, ByVal e As EventArgs)
        ' Fires when the application ends
    End Sub

End Class