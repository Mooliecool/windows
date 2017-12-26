'************************************* Module Header **************************************'
' Module Name:  VBEHelper.vb
' Project:      VBVstoVBAInterop
' Copyright (c) Microsoft Corporation.
' 
' The VBVstoVBAInterop project demonstrates how to interop with VBA project object model in 
' VSTO projects. Including how to programmatically add Macros (or VBA UDF in Excel) into an
' Office document; how to call Macros / VBA UDFs from VSTO code; and how to call VSTO code
' from VBA code. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'******************************************************************************************'

#Region "Import directives"
Imports Microsoft.Win32
#End Region


''' <summary>
''' This static class provides helper methods of VBE related functions to the project.
''' </summary>
Friend Module VBEHelper

    ''' <summary>
    ''' Gets or sets whether access to Excel VBA project object model is allowed.
    ''' </summary>
    Friend Property AccessVBOM() As Boolean
        Get
            Dim key As RegistryKey = Nothing
            Dim val As Boolean = False

            Try
                key = Registry.CurrentUser.OpenSubKey("Software\Microsoft\Office\12.0\Excel\Security", False)
                val = CInt(key.GetValue("AccessVBOM", 0)) <> 0
            Finally
                If key IsNot Nothing Then
                    key.Close()
                End If
            End Try

            Return val
        End Get

        Set(ByVal value As Boolean)
            Dim key As RegistryKey = Nothing

            Try
                key = Registry.CurrentUser.OpenSubKey("Software\Microsoft\Office\12.0\Excel\Security", True)
                key.SetValue("AccessVBOM", IIf(value, 1, 0))
            Finally
                If key IsNot Nothing Then
                    key.Close()
                End If
            End Try
        End Set
    End Property

End Module
