'************************************* Module Header **************************************'
'* Module Name:	ThisAddIn.vb
'* Project:		VBOutlookRibbonXml
'* Copyright (c) Microsoft Corporation.
'* 
'* The VBOutlookRibbonXml example demonstrates how to use Ribbon XML to create customized 
'* Ribbon for Outlook 2007 inspectors.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 9/17/2009 18:00 PM Li ZhenHao Created
'******************************************************************************************


Public Class ThisAddIn

    Private Sub ThisAddIn_Startup(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Startup

    End Sub

    Protected Overrides Function CreateRibbonExtensibilityObject() As Microsoft.Office.Core.IRibbonExtensibility
        ' Outlook will call this method to get our IRibbonExtensibility
        ' implementation.
        Return New Ribbon()
    End Function

    Private Sub ThisAddIn_Shutdown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shutdown

    End Sub

End Class
