'************************************* Module Header **************************************'
' Module Name:  InteropForm.vb
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
'\******************************************************************************************'

#Region "Import directives"
Imports Microsoft.Vbe.Interop
Imports System.Runtime.InteropServices

#End Region


''' <summary>
''' This form contains the UI for the demo.
''' </summary>
Public Class InteropForm

    Private Const ModuleName As String = "VBVstoVBAInterop"
    Private Const VbaFunctionName As String = "SayHello"

    Private Sub InteropForm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        ' Set control Enable status based on VBA project Object Model access option.
        Me.SetAccVbaOMStatus()

        If VBEHelper.AccessVBOM Then
            ' Get the VBA Project list
            Me.GetPrjList()
        End If
    End Sub

    Private Sub chkEnableVbaAccess_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkEnableVbaAccess.CheckedChanged
        If Not chkEnableVbaAccess.Checked Then
            If MsgBox("Disable this option will prevent access to VBA project OM.", _
                      MsgBoxStyle.OkCancel Or MsgBoxStyle.Question, _
                      Me.Text) = MsgBoxResult.Ok Then
                VBEHelper.AccessVBOM = False
            Else
                chkEnableVbaAccess.Checked = True
            End If
        Else
            VBEHelper.AccessVBOM = True
        End If

        SetAccVbaOMStatus()
    End Sub

    Private Sub btnRefresh_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRefresh.Click
        Me.GetPrjList()
    End Sub

    Private Sub btnShowVBE_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnShowVBE.Click
        ' Show the VBA IDE
        Globals.ThisAddIn.Application.VBE.MainWindow.Visible = True
    End Sub

    Private Sub btnInsertRun_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnInsertRun.Click
        Try
            Dim item As Object = cboProjects.SelectedItem

            If item IsNot Nothing Then
                MsgBox(String.Format("Return value: {0}", InsertAndRun(CType(item, VBProject))), _
                        MsgBoxStyle.Information Or MsgBoxStyle.OkOnly, _
                        Me.Text)

                MsgBox("Now you can run Macro ShowAsmInfo which calls VSTO code from Excel VBA.", _
                        MsgBoxStyle.Information Or MsgBoxStyle.OkOnly, _
                        Me.Text)
                btnShowVBE.PerformClick()
            Else
                MsgBox("Please select a VBA project.", MsgBoxStyle.Critical Or MsgBoxStyle.OkOnly, Me.Text)
            End If

        Catch comEx As COMException
            MsgBox(comEx.ToString, MsgBoxStyle.Critical Or MsgBoxStyle.OkOnly, Me.Text)

        Catch appEx As ApplicationException
            MsgBox(appEx.Message, MsgBoxStyle.Critical Or MsgBoxStyle.OkOnly, Me.Text)
        End Try
    End Sub

    ''' <summary>
    ''' Set controls' Enable value according to the VBA project OM access option.
    ''' </summary>
    Private Sub SetAccVbaOMStatus()
        Dim enabled As Boolean = VBEHelper.AccessVBOM
        chkEnableVbaAccess.Checked = enabled
        cboProjects.Enabled = enabled
        btnRefresh.Enabled = enabled
        btnInsertRun.Enabled = enabled
        btnShowVBE.Enabled = enabled
    End Sub

    ''' <summary>
    ''' Gets a list of available VBA projects and update the combobox.
    ''' </summary>
    Private Sub GetPrjList()
        cboProjects.Items.Clear()

        For Each prj As VBProject In Globals.ThisAddIn.Application.VBE.VBProjects
            cboProjects.Items.Add(prj)
        Next
    End Sub

    ''' <summary>
    ''' Insert the sample macro code into the specified VBProject and call the code from VSTO.
    ''' </summary>
    ''' <param name="prj">Target VBProject.</param>
    ''' <returns>The return value from the VBA function.</returns>
    Private Function InsertAndRun(ByVal prj As VBProject) As Object
        ' Check for existing module.
        For Each component As VBComponent In prj.VBComponents
            If component.Name = ModuleName Then
                Throw New ApplicationException("There is already a " + ModuleName + " in this VBA project.")
            End If
        Next

        ' Add a standard module
        Dim myMod As VBComponent = prj.VBComponents.Add(vbext_ComponentType.vbext_ct_StdModule)

        ' Name the module
        myMod.Name = ModuleName

        ' Add code into the module directly from a string.
        myMod.CodeModule.AddFromString(txtVbaSub.Text)

        ' Call the newly added VBA function.
        ' The first parameter to the function is a string "VSTO".
        Return Globals.ThisAddIn.Application.Run(VbaFunctionName, "VSTO")
    End Function
End Class