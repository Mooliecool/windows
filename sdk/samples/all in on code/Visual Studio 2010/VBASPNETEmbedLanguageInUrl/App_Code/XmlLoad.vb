'****************************** Module Header ******************************\
' Module Name: BasePage.vb
' Project:     VBASPNETEmbedLanguageInUrl
' Copyright (c) Microsoft Corporation
'
' The class used to load xml data of Language.xml
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*****************************************************************************/



Imports System.Xml

Public Class XmlLoad
    Public Sub XmlLoadMethod(ByVal language As String, ByRef strTitle As String, ByRef strText As String, ByRef strElement As String, ByRef flag As Boolean)
        Try
            flag = False
            Dim xmlDocument As New XmlDocument()
            xmlDocument.Load(AppDomain.CurrentDomain.BaseDirectory + "XmlFolder/Language.xml")
            Dim cnTitleList As XmlNodeList = xmlDocument.SelectSingleNode("root/title/element[@language='" & language & "']").ChildNodes
            If cnTitleList Is Nothing Then
                flag = True
                language = "en-us"
            End If
            cnTitleList = xmlDocument.SelectSingleNode("root/title/element[@language='" & language & "']").ChildNodes
            strTitle = cnTitleList(0).InnerText
            Dim cnTextList As XmlNodeList = xmlDocument.SelectSingleNode("root/text/element[@language='" & language & "']").ChildNodes
            strText = cnTextList(0).InnerText
            Dim cnElementList As XmlNodeList = xmlDocument.SelectSingleNode("root/comment/element[@language='" & language & "']").ChildNodes
            strElement = cnElementList(0).InnerText
        Catch generatedExceptionName As Exception
            strText = String.Empty
            strElement = String.Empty
            strTitle = String.Empty
            flag = True
        End Try
    End Sub
End Class
