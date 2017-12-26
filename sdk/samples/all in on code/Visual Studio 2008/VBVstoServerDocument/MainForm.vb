'/************************************* Module Header **************************************\
'* Module Name:  MainForm.vb
'* Project:      VBVstoServerDocument
'* Copyright (c) Microsoft Corporation.
'* 
'* The VBVstoServerDocument project demonstrates how to use the ServerDocument
'* class to extract information from a VSTO customized Word document or Excel
'* Workbook; and also how to programmatically add / remove VSTO customizations.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 10/16/2009 5:00 PM Tim Li Created
'\******************************************************************************************/


#Region "Imports directives"
Imports System
Imports System.IO
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Data
Imports System.Drawing
Imports System.Text
Imports System.Windows.Forms
Imports SERuntime = Microsoft.VisualStudio.Tools.Applications.Runtime
Imports V3Runtime = Microsoft.VisualStudio.Tools.Applications
#End Region

Public Class MainForm

#Region "Control event handlers"

    Private Sub btnSelectFile_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnSelectFile.Click
        ofd.FileName = String.Empty

        ' Select an Word document / Excel workbook.
        If ofd.ShowDialog() = DialogResult.OK Then
            txtFileName.Text = ofd.FileName
        End If
    End Sub

    Private Sub txtFileName_TextChanged(ByVal sender As Object, ByVal e As EventArgs) Handles txtFileName.TextChanged
        toolTip.SetToolTip(txtFileName, txtFileName.Text)
        ' New file selected, show doc info.
        ShowDocInfo(txtFileName.Text)
    End Sub

    Private Sub btnAdd_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnAdd.Click
        AddCustomization(txtFileName.Text)
        ShowDocInfo(txtFileName.Text)
    End Sub

    Private Sub btnRemove_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnRemove.Click
        RemoveCustomization(txtFileName.Text)
        ShowDocInfo(txtFileName.Text)
    End Sub

#End Region

#Region "Document Info"

    Private Sub ShowDocInfo(ByVal docPath As String)
        ' Get the version of the customization associated with the document.
        Dim rtVer As Integer = V3Runtime.ServerDocument.GetCustomizationVersion(docPath)

        ' Update UI elements
        lstDocInfo.Items.Clear()
        btnAdd.Enabled = rtVer = 0
        btnRemove.Enabled = Not btnAdd.Enabled
        txtAssembly.Text = String.Empty
        txtManifest.Text = String.Empty
        txtAssembly.Enabled = btnAdd.Enabled And (Not Is2007Doc(docPath))
        lblAssembly.Enabled = txtAssembly.Enabled
        txtManifest.Enabled = btnAdd.Enabled
        lblManifest.Enabled = txtManifest.Enabled

        Select Case (rtVer)

            Case 0
                lstDocInfo.Items.Add("No customization in this document")


            Case 1
                lstDocInfo.Items.Add("The document has a customization that was " & _
                        "created by using Visual Studio Tools for Office, Version 2003.")


            Case 2
                ShowDocInfoSE(docPath)


            Case 3
                ShowDocInfoV3(docPath)


            Case Else
                lstDocInfo.Items.Add(String.Format("Version {0} is not supported in this demo.", rtVer))

        End Select
    End Sub

    Private Sub ShowDocInfoSE(ByVal docPath As String)
        Dim cacheEnabled As Boolean = SERuntime.ServerDocument.IsCacheEnabled(docPath)

        Using doc As SERuntime.ServerDocument = New SERuntime.ServerDocument(docPath)

            lstDocInfo.Items.Add("The document has a customization that was created by " & _
                    "using the Visual Studio 2005 Tools for Office Second Edition runtime.")
            lstDocInfo.Items.Add("This is the version of the runtime that is used to create " & _
                "customizations for Microsoft Office 2003.")

            ' Deployment manifest info
            lstDocInfo.Items.Add(String.Empty)
            lstDocInfo.Items.Add("This URL of the deployment manifest is: " + doc.AppManifest.DeployManifestPath)
            lstDocInfo.Items.Add("AppManifest Idenetiy Name: " + doc.AppManifest.Identity.Name)
            lstDocInfo.Items.Add("AppManifest Idenetiy PublicKeyToken: " + doc.AppManifest.Identity.PublicKeyToken.ToString())
            lstDocInfo.Items.Add("AppManifest Idenetiy Version: " + doc.AppManifest.Identity.Version.ToString())

            ' Cached data
            lstDocInfo.Items.Add(String.Empty)
            If cacheEnabled Then
                lstDocInfo.Items.Add("Data cache enabled")

                For Each hostItem As SERuntime.CachedDataHostItem In doc.CachedData.HostItems
                    lstDocInfo.Items.Add("Namespace and class: " + hostItem.Id)

                    For Each dataItem As SERuntime.CachedDataItem In hostItem.CachedData
                        lstDocInfo.Items.Add("     Data item: " + dataItem.Id)
                    Next
                Next
            Else
                lstDocInfo.Items.Add("Data cache not enabled")
            End If
        End Using
    End Sub

    Private Sub ShowDocInfoV3(ByVal docPath As String)
        Dim cacheEnabled As Boolean = V3Runtime.ServerDocument.IsCacheEnabled(docPath)

        Using doc As V3Runtime.ServerDocument = New V3Runtime.ServerDocument(docPath)

            lstDocInfo.Items.Add("The document has a customization that was created by " & _
                "using the Microsoft Visual Studio Tools for the Microsoft Office system " & _
                "(version 3.0 Runtime).")
            lstDocInfo.Items.Add("This is the version of the runtime that is used to create " & _
                    "customizations for the 2007 Microsoft Office system.")

            ' Deployment manifest & solution ID
            lstDocInfo.Items.Add(String.Empty)
            lstDocInfo.Items.Add("The URL of the deployment manifest is: " + doc.DeploymentManifestUrl.ToString())
            lstDocInfo.Items.Add("The Solution ID is: " + doc.SolutionId.ToString())

            ' Cached data
            lstDocInfo.Items.Add(String.Empty)
            If cacheEnabled Then
                lstDocInfo.Items.Add("Data cache enabled")

                For Each hostItem As V3Runtime.CachedDataHostItem In doc.CachedData.HostItems
                    lstDocInfo.Items.Add("Namespace and class: " + hostItem.Id)

                    For Each dataItem As V3Runtime.CachedDataItem In hostItem.CachedData
                        lstDocInfo.Items.Add("     Data item: " + dataItem.Id)
                    Next
                Next
            Else
                lstDocInfo.Items.Add("Data cache not enabled")
            End If
        End Using
    End Sub

    Private Sub AddCustomization(ByVal docPath As String)
        Try
            ' Call the corresponding ServerDocument class to add the customization.
            If Is2007Doc(docPath) Then
                V3Runtime.ServerDocument.AddCustomization(docPath, New Uri(txtManifest.Text))
            Else
                SERuntime.ServerDocument.AddCustomization(docPath, txtAssembly.Text, txtManifest.Text, "1.0.0.0", True)
            End If

        Catch ex As Exception
            MessageBox.Show(ex.Message, _
                Me.Text, _
                MessageBoxButtons.OK, _
                MessageBoxIcon.Error)
        End Try
    End Sub

    Private Sub RemoveCustomization(ByVal docPath As String)
        ' Get the version of the customization associated with the document.
        Dim rtVer As Int32 = V3Runtime.ServerDocument.GetCustomizationVersion(docPath)

        ' Call the corresponding ServerDocument class to remove the customization.
        Select Case rtVer
            Case 2
                SERuntime.ServerDocument.RemoveCustomization(docPath)

            Case 3
                V3Runtime.ServerDocument.RemoveCustomization(docPath)

            Case Else
                MessageBox.Show(String.Format("Version {0} is not supported in this demo.", rtVer), _
                    Me.Text, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Select
    End Sub

    Private Function Is2007Doc(ByVal docPath As String) As Boolean
        ' How to get the document version correctly is not in the scope of this demo.
        ' Here we just use file extension to make a fast and easy implementation.
        Dim ext As String = Path.GetExtension(docPath).ToLowerInvariant()

        Return (ext = ".docx") Or (ext = ".dotx") Or (ext = ".docm") Or (ext = ".dotm") Or _
               (ext = ".xlsx") Or (ext = ".xltx") Or (ext = ".xlsm") Or (ext = ".xltm")
    End Function

#End Region

End Class
