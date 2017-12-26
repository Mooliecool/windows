'*************************** Module Header ******************************'
' Module Name:  MainForm.vb
' Project:      VBManipulateImagesInWordDocument
' Copyright (c) Microsoft Corporation.
' 
' The Main UI of the application. 
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.Drawing.Imaging
Imports System.IO
Imports DocumentFormat.OpenXml.Drawing

Partial Public Class MainForm
    Inherits Form
    Private documentManipulator As WordDocumentImageManipulator

    Public Sub New()
        InitializeComponent()
    End Sub

    ''' <summary>
    ''' Handle the btnOpenFile click event.
    ''' </summary>
    Private Sub btnOpenFile_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnOpenFile.Click

        ' Open an OpenFileDialog instance.
        Using dialog As New OpenFileDialog()
            dialog.Filter = "Word document (*.docx)|*.docx"

            Dim result = dialog.ShowDialog()
            If result = DialogResult.OK Then
                Try
                    lstImage.Items.Clear()
                    If picView.Image IsNot Nothing Then
                        picView.Image.Dispose()
                    End If
                    picView.Image = Nothing
                    lbFileName.Text = String.Empty


                    ' Initialize a WordDocumentImageManipulator instance.
                    OpenWordDocument(dialog.FileName)

                    ' Update the lstImage listbox.
                    UpdateImageList()

                    lbFileName.Text = dialog.FileName
                Catch ex As Exception
                    MessageBox.Show(ex.Message)
                End Try
            End If
        End Using
    End Sub

    ''' <summary>
    ''' Initialize a WordDocumentImageManipulator instance.
    ''' </summary>
    Private Sub OpenWordDocument(ByVal filepath As String)
        If String.IsNullOrEmpty(filepath) OrElse (Not System.IO.File.Exists(filepath)) Then
            Throw New ArgumentException("filepath")
        End If

        Dim file As New FileInfo(filepath)

        ' Dispose the previous instance.
        If documentManipulator IsNot Nothing Then
            documentManipulator.Dispose()
        End If

        documentManipulator = New WordDocumentImageManipulator(file)

        ' Register the ImagesChanged event.
        AddHandler documentManipulator.ImagesChanged, AddressOf documentManipulator_ImagesChanged

    End Sub

    ''' <summary>
    ''' Update the lstImage listbox.
    ''' </summary>
    Private Sub UpdateImageList()
        If picView.Image IsNot Nothing Then
            picView.Image.Dispose()
        End If
        picView.Image = Nothing

        lstImage.Items.Clear()

        ' Display the "Embed" property of the Blip element. This property is the 
        ' reference ID of the ImagePart.
        lstImage.DisplayMember = "Embed"
        For Each blip In documentManipulator.GetAllImages()
            lstImage.Items.Add(blip)
        Next blip
    End Sub

    ''' <summary>
    ''' Handle the ImagesChanged event.
    ''' </summary>
    Private Sub documentManipulator_ImagesChanged(ByVal sender As Object, ByVal e As EventArgs)
        UpdateImageList()
    End Sub

    ''' <summary>
    ''' Handle the lstImage SelectedIndexChanged event to display the image in 
    ''' picView.
    ''' </summary>
    Private Sub lstImage_SelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs) _
        Handles lstImage.SelectedIndexChanged

        Dim imgBlip = TryCast(lstImage.SelectedItem, Blip)
        If imgBlip Is Nothing Then
            Return
        End If

        ' Dispose the previous image in the picView.
        If picView.Image IsNot Nothing Then
            picView.Image.Dispose()
            picView.Image = Nothing
        End If

        Try
            Dim newImg = documentManipulator.GetImageInBlip(imgBlip)
            picView.Image = New Bitmap(newImg)
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try

    End Sub

    ''' <summary>
    ''' Handle the btnDelete Click SelectedIndexChanged event.
    ''' </summary>
    Private Sub btnDelete_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnDelete.Click
        If lstImage.SelectedItem IsNot Nothing Then
            Dim result = MessageBox.Show("Do you want to delete this image?",
                                         "Delete Image",
                                         MessageBoxButtons.YesNo,
                                         MessageBoxIcon.Question)

            If result = DialogResult.Yes Then
                Try
                    documentManipulator.DeleteImage(TryCast(lstImage.SelectedItem, Blip))
                Catch ex As Exception
                    MessageBox.Show(ex.Message)
                End Try
            End If
        End If
    End Sub

    ''' <summary>
    ''' Handle the btnReplace Click SelectedIndexChanged event.
    ''' </summary>
    Private Sub btnReplace_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnReplace.Click
        If lstImage.SelectedItem IsNot Nothing Then
            Using dialog As New OpenFileDialog()
                dialog.Filter = "Image File (*.jpeg;*.jpg;*.png)|*.jpeg;*.jpg;*.png"

                Dim result = dialog.ShowDialog()
                If result = DialogResult.OK Then
                    Dim confirm = MessageBox.Show("Do you want to replace this image?",
                                                  "Replace Image",
                                                  MessageBoxButtons.YesNo,
                                                  MessageBoxIcon.Question)
                    If confirm = DialogResult.Yes Then
                        Try
                            documentManipulator.ReplaceImage(
                                TryCast(lstImage.SelectedItem, Blip),
                                New FileInfo(dialog.FileName))
                        Catch ex As Exception
                            MessageBox.Show(ex.Message)
                        End Try
                    End If

                End If
            End Using
        End If
    End Sub

    ''' <summary>
    ''' Handle the btnExport Click SelectedIndexChanged event.
    ''' </summary>
    Private Sub btnExport_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnExport.Click
        If lstImage.SelectedItem IsNot Nothing _
            AndAlso picView.Image IsNot Nothing Then
            Using dialog As New SaveFileDialog()
                dialog.Filter = "Image File (*.jpeg)|*.jpeg"
                Dim result = dialog.ShowDialog()
                If result = DialogResult.OK Then
                    picView.Image.Save(dialog.FileName, ImageFormat.Jpeg)
                End If
            End Using
        End If
    End Sub
End Class