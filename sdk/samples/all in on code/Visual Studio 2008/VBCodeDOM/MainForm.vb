'/************************************* Module Header **************************************\
'* Module Name:	MainForm.vb
'* Project:		VBCodeDOM
'* Copyright (c) Microsoft Corporation.
'* 
'* The VBCodeDOM project demonstrates how to use the .NET CodeDOM mechanism to enable
'* dynamic souce code generation and compilation at runtime.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 9/12/2009 1:00 PM Jie Wang Created
'\******************************************************************************************/

#Region "Using directives"
Imports System
Imports System.Text
Imports System.CodeDom
Imports System.CodeDom.Compiler
Imports System.Reflection
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Data
Imports System.Drawing
Imports System.Linq
Imports System.Windows.Forms
#End Region


Public Class MainForm

    Private Sub MainForm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        cboLang.SelectedIndex = 1  ' Set default language to VB.NET
    End Sub

    Private Sub txtSource_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtSource.TextChanged
        btnRun.Enabled = txtSource.Text.Trim().Length > 0  ' Enable Run button if there is something in the TextBox.
    End Sub

    Private Sub txtNamespace_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtNamespace.TextChanged
        btnAddNamespace.Enabled = txtNamespace.Text.Trim().Length > 0  ' Enable/disable the add namespace button
    End Sub

    Private Sub txtAssemblyRef_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtAssemblyRef.TextChanged
        btnAddAsmRef.Enabled = txtAssemblyRef.Text.Trim().Length > 0  ' Enable/disable the add reference button
    End Sub

    Private Sub lstNamespaces_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles lstNamespaces.SelectedIndexChanged
        btnRemoveNamespace.Enabled = lstNamespaces.SelectedItem IsNot Nothing  ' Enable/disable the remove namespace button
    End Sub

    Private Sub lstAssemblyRef_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles lstAssemblyRef.SelectedIndexChanged
        btnRemoveAsmRef.Enabled = lstAssemblyRef.SelectedItem IsNot Nothing  ' Enable/disable the remove reference button
    End Sub

    Private Sub btnAddNamespace_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAddNamespace.Click
        ' Add namespace import
        Dim ns As String = txtNamespace.Text.Trim()
        If lstNamespaces.Items.IndexOf(ns) < 0 Then
            lstNamespaces.Items.Add(ns)
        End If
        txtNamespace.Text = String.Empty
    End Sub

    Private Sub btnRemoveNamespace_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRemoveNamespace.Click
        lstNamespaces.Items.Remove(lstNamespaces.SelectedItem) ' Remove namespace import
    End Sub

    Private Sub btnAddAsmRef_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAddAsmRef.Click
        ' Add reference
        Dim asm As String = txtAssemblyRef.Text.Trim()
        If lstAssemblyRef.Items.IndexOf(asm) < 0 Then
            lstAssemblyRef.Items.Add(asm)
        End If
        txtAssemblyRef.Text = String.Empty
    End Sub

    Private Sub btnRemoveAsmRef_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRemoveAsmRef.Click
        lstAssemblyRef.Items.Remove(lstAssemblyRef.SelectedItem) ' Remove reference
    End Sub

    Private Sub btnRun_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRun.Click
        Dim lang As ScriptControl.Language

        ' Set the language enum according to the user selection.
        Select Case cboLang.SelectedItem.ToString().ToLowerInvariant
            Case "visual basic"
                lang = ScriptControl.Language.VisualBasic
                Exit Select
            Case "jscript"
                lang = ScriptControl.Language.JScript
                Exit Select
            Case Else
                lang = ScriptControl.Language.CSharp
                Exit Select
        End Select

        Try
            ' Create new instance of ScriptControl, passing in script and language.
            Dim sc As New ScriptControl(txtSource.Text, lang)
            sc.RunInSeparateDomain = chkSpDomain.Checked

            ' Set namespace imports
            For i As Integer = 0 To lstNamespaces.Items.Count - 1
                sc.CodeNamespaceImports.Add(CStr(lstNamespaces.Items(i)))
            Next

            ' Set assembly references
            For i As Integer = 0 To lstAssemblyRef.Items.Count - 1
                sc.AssemblyReferences.Add(CStr(lstAssemblyRef.Items(i)))
            Next

            Dim result As Object = sc.Run()     ' Run the script and get the return value

            MsgBox(String.Format("Result from the script {0}", result), _
                    MsgBoxStyle.Information Or MsgBoxStyle.OkOnly, Me.Text)

        Catch ex As Exception
            If TypeOf ex Is TargetInvocationException Then
                ex = ex.InnerException
            End If

            MsgBox(String.Format("Error in script: {0}{1}", vbCrLf & vbCrLf, ex.Message), _
                    MsgBoxStyle.Critical Or MsgBoxStyle.OkOnly, Me.Text)
        End Try
    End Sub
End Class
