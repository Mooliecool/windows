'****************************** Module Header ******************************\
' Module Name: ShowMe.aspx.vb
' Project:     VBASPNETEmbedLanguageInUrl
' Copyright (c) Microsoft Corporation
'
' The project illustrates how to embed the language code in URL such
' as http://domain/en-us/ShowMe.aspx. The page will display different
' content according to the language code, the sample use url-routing 
' and resource files to localize the content of web page.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*****************************************************************************/



Public Class _Default
    Inherits BasePage
    ''' <summary>
    ''' Load this page with a certain language.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        Dim elementArray As String() = Session("info").ToString().Split(","c)
        Dim language As String = elementArray(0)
        Dim pageName As String = elementArray(1)
        If language = "error" Then
            Response.Write("The url routing error: please restart the web application with Start.aspx page")
            Return
        End If
        Dim xmlPath As String = Server.MapPath("~/XmlFolder/Language.xml")
        Dim strTitle As String = String.Empty
        Dim strText As String = String.Empty
        Dim strElement As String = String.Empty
        Dim flag As Boolean = False

        ' Load xml data.
        Dim xmlLoad As New XmlLoad()
        xmlLoad.XmlLoadMethod(language, strTitle, strText, strElement, flag)

        ' If specific language is inexistence, return English version of this web page.
        If flag = True Then
            language = "en-us"
            Response.Write("no language, use English web page")
            xmlLoad.XmlLoadMethod(language, strTitle, strText, strElement, flag)
        End If
        lbTitleContent.Text = strTitle
        lbTextContent.Text = strText
        lbTimeContent.Text = DateTime.Now.ToLongDateString()
        lbCommentContent.Text = strElement
    End Sub

End Class