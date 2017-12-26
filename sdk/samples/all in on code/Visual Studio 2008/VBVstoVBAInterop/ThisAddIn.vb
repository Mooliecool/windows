'/************************************* Module Header **************************************\
'* Module Name:  ThisAddIn.vb
'* Project:      VBVstoVBAInterop
'* Copyright (c) Microsoft Corporation.
'* 
'* The VBVstoVBAInterop project demonstrates how to interop with VBA project object model in 
'* VSTO projects. Including how to programmatically add Macros (or VBA UDF in Excel) into an
'* Office document; how to call Macros / VBA UDFs from VSTO code; and how to call VSTO code
'* from VBA code. 
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 10/30/2009 3:50 PM Jie Wang Created
'\******************************************************************************************/

Public Class ThisAddIn

    ''' <summary>
    ''' The COM Add-in automation service object.
    ''' </summary>
    Private vstoClass As VstoClass = Nothing

    Private Sub ThisAddIn_Startup(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Startup

    End Sub

    Private Sub ThisAddIn_Shutdown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shutdown

    End Sub

    ''' <summary>
    ''' Returns an instance of <see cref="VstoClass"/> that can be used by the sample
    ''' Excel VBA macro included in this project.
    ''' </summary>
    Protected Overrides Function RequestComAddInAutomationService() As Object
        If vstoClass Is Nothing Then
            vstoClass = New VstoClass()
        End If

        Return vstoClass
    End Function
End Class
