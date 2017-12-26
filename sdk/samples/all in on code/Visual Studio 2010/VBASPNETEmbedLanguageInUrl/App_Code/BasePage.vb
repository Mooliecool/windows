'****************************** Module Header ******************************\
' Module Name: BasePage.vb
' Project:     VBASPNETEmbedLanguageInUrl
' Copyright (c) Microsoft Corporation
'
' The multiple language web pages are inheriting this class.
' The BasePage class will check the request url language part
' and name part, and set the Page's Culture and UICultrue
' properties.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*****************************************************************************/


Public Class BasePage
    Inherits Page
    ''' <summary>
    ''' The BasePage class used to set Page.Culture and Page.UICulture.
    ''' </summary>
    Protected Overrides Sub InitializeCulture()
        Try
            Dim language As String = RouteData.Values("language").ToString().ToLower()
            Dim pageName As String = RouteData.Values("pageName").ToString()
            Session("info") = language & "," & pageName
            Page.Culture = language
            Page.UICulture = language
        Catch generatedExceptionName As Exception
            Session("info") = "error,error"
        End Try

    End Sub


End Class
