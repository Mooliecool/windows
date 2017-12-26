'****************************** Module Header ******************************\
' Module Name:  LocalResources.aspx.vb
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
' This page tells how to use Local Resources files to make controls simply
' localized into other languages following the selection from DropDownList.
' According to the choice, the language of the controls in the page can be
' changed. Besides the text's language, we can also change other properties, 
' like the color of the Label in the demo. However, all these changes are
' based on the Local Resources file whose name is as the same as the page's 
' That means all these changes only works in this single page and when we 
' redirect to another page, they will be lost. To handle this issue, please 
' refer to GlobalResources.aspx's description.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' History:
' * 7/27/2009 6:00 PM Bravo Yang Created
'***************************************************************************/

#Region "Imports Directory"
Imports System.Threading
Imports System.Globalization
#End Region


Partial Public Class LocalResources
    Inherits System.Web.UI.Page

    Protected Overrides Sub InitializeCulture()

        ' Get the user's selection from DropDownList by Request.Form().
        ' The reason why here we don't use ddlLanguage.SelectedValue is that
        ' all the page controls, including ddlLanguage is unavailable now.
        Dim strLanguageInfo As String = Request.Form("ddlLanguage")

        If Not strLanguageInfo Is Nothing Then
            ' Set the CurrentUICulture and CurrentCulture to strLanguageInfo.
            Thread.CurrentThread.CurrentUICulture = New CultureInfo(strLanguageInfo)
            Thread.CurrentThread.CurrentCulture = CultureInfo.CreateSpecificCulture(strLanguageInfo)
        End If

    End Sub

End Class