'****************************** Module Header ******************************'
' Module Name:  MyFunctions.vb
' Project:      VBExcelAutomationAddIn
' Copyright (c) Microsoft Corporation.
' 
' The VBExcelAutomationAddIn project is a class library project written in VB.
' It illustrates how to write a managed COM component which can be used as an
' Automation AddIn in Excel. The Automation AddIn can provide user defined 
' functions for Excel.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Imports directives"

Imports System.Runtime.InteropServices
Imports System.Text
Imports Microsoft.Win32
Imports Excel = Microsoft.Office.Interop.Excel
Imports System.Reflection

#End Region


<ComVisible(True), _
ClassInterface(ClassInterfaceType.AutoDual), _
Guid("83111578-8F0D-4821-835A-714DD2AACE3B")> _
Public Class MyFunctions

#Region "User Defined Functions"

    Public Function MinusNumbers(ByVal num1 As Double, _
                               Optional ByVal num2 As Object = Nothing, _
                               Optional ByVal num3 As Object = Nothing) _
                               As Double

        Dim result As Double = num1
        If Not TypeOf num2 Is Missing And Not num2 Is Nothing Then
            Dim r2 As Excel.Range = TryCast(num2, Excel.Range)
            result = (result - Convert.ToDouble(r2.get_Value2))
        End If
        If Not TypeOf num3 Is Missing And Not num3 Is Nothing Then
            Dim r3 As Excel.Range = TryCast(num3, Excel.Range)
            result = (result - Convert.ToDouble(r3.get_Value2))
        End If
        Return result

    End Function

    Public Function NumberOfCells(ByVal range As Object) As Double
        Dim r As Excel.Range = TryCast(range, Excel.Range)
        Return CDbl(r.get_Cells.get_Count)
    End Function

#End Region


#Region "Registration of Automation Add-in"

    ''' <summary>
    ''' This is function which is called when we register the dll
    ''' </summary>
    ''' <param name="type"></param>
    ''' <remarks></remarks>
    <ComRegisterFunction()> _
    Public Shared Sub RegisterFunction(ByVal type As Type)

        ' Add the "Programmable" registry key under CLSID
        Registry.ClassesRoot.CreateSubKey(GetCLSIDSubKeyName( _
                                          type, "Programmable"))

        ' Register the full path to mscoree.dll which makes Excel happier.
        Dim key As RegistryKey = Registry.ClassesRoot.OpenSubKey( _
        GetCLSIDSubKeyName(type, "InprocServer32"), True)
        key.SetValue("", (Environment.SystemDirectory & "\mscoree.dll"), _
                     RegistryValueKind.String)

    End Sub

    ''' <summary>
    ''' This is function which is called when we unregister the dll
    ''' </summary>
    ''' <param name="type"></param>
    ''' <remarks></remarks>
    <ComUnregisterFunction()> _
    Public Shared Sub UnregisterFunction(ByVal type As Type)

        ' Remove the "Programmable" registry key under CLSID
        Registry.ClassesRoot.DeleteSubKey( _
        GetCLSIDSubKeyName(type, "Programmable"), False)

    End Sub

    ''' <summary>
    ''' Assistant function used by RegisterFunction/UnregisterFunction
    ''' </summary>
    ''' <param name="type"></param>
    ''' <param name="subKeyName"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Shared Function GetCLSIDSubKeyName( _
    ByVal type As Type, ByVal subKeyName As String) As String

        Dim s As New StringBuilder
        s.Append("CLSID\{")
        s.Append(type.GUID.ToString.ToUpper)
        s.Append("}\")
        s.Append(subKeyName)
        Return s.ToString

    End Function

#End Region

End Class