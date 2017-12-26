'****************************** Module Header ******************************\
' Module Name:  GlobalResources.aspx.vb
' Project:      VBASPNETLocalization
' Copyright (c) Microsoft Corporation
'
' The project illustrates how to build a multi-lingual website with ASP.NET
' Localization. ASP.NET enables pages obtain content and other data based
' on the preferred language setting of the browser or based on the user's 
' explicit choice of language. If controls are configured to get property
' values from resources, at run time, the resource expressions are replaced
' by resources from the appropriate resource file. 
'
' This page describes how to use Global Resources files to make controls in 
' different pages share consistent resource data. When we reach Page2.aspx 
' from Default.aspx, we can find only [lblGlobal] is displayed in correct 
' text and color while [lblLocal] with the totally same HTML code as it 
' is in LocalResources.aspx doesn't. It is because [lblLocal] is localized 
' by Local Resources while [lblGlobal] is based on data in Global Resources
' files.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' History:
' * 7/27/2009 7:20 PM Bravo Yang Created
'***************************************************************************/

#Region "Imports Directory"
Imports System.Threading
Imports System.Globalization
#End Region


Partial Public Class GlobalResources
    Inherits System.Web.UI.Page

    Protected Overrides Sub InitializeCulture()

        ' Get the user's selection from DropDownList by Request.Form().
        ' This value is passed from Defualt.aspx by form's action attribute
        ' because we use PostBackUrl property of the Button to do the
        Dim strLanguageInfo As String = Request.Form("ddlLanguage")

        If Not strLanguageInfo Is Nothing Then
            ' Set the CurrentUICulture and CurrentCulture to strLanguageInfo.
            Thread.CurrentThread.CurrentUICulture = New CultureInfo(strLanguageInfo)
            Thread.CurrentThread.CurrentCulture = CultureInfo.CreateSpecificCulture(strLanguageInfo)
        End If

    End Sub

End Class