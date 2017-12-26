'****************************** Module Header ******************************\
' Module Name: BasePage.vb
' Project:     VBASPNETEmbedLanguageInUrl
' Copyright (c) Microsoft Corporation
'
' The UrlRoutingHandlers will check the request url. This interface
' break the url string, check the file name and throw them to 
' the InvalidPage.aspx page if it not existence.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*****************************************************************************/


Imports System.Web.Compilation
Imports System.Web.Routing

Public Class UrlRoutingHandlers
    Implements IRouteHandler
    ''' <summary>
    ''' Create this RoutingHandler to check the HttpRequest and
    ''' return correct url path.
    ''' </summary>
    ''' <param name="context"></param>
    ''' <returns></returns>
    Public Function GetHttpHandler1(ByVal context As System.Web.Routing.RequestContext) As System.Web.IHttpHandler Implements System.Web.Routing.IRouteHandler.GetHttpHandler
        Dim language As String = context.RouteData.Values("language").ToString().ToLower()
        Dim pageName As String = context.RouteData.Values("pageName").ToString()
        If pageName = "ShowMe.aspx" Then
            Return TryCast(BuildManager.CreateInstanceFromVirtualPath("~/ShowMe.aspx", GetType(Page)), Page)
        Else
            Return BuildManager.CreateInstanceFromVirtualPath("~/InvalidPage.aspx", GetType(Page))
        End If
    End Function
End Class
