'***************************** Module Header ******************************\ 
' Module Name:    MainForm.vb 
' Project:        VBTiffImageConverter
' Copyright (c) Microsoft Corporation. 
'
' This sample demonstrates how to convert JPEG images into TIFF images and vice 
' versa. This sample also allows to create single multi-page TIFF images from 
' selected JPEG images.
'
' TIFF (originally standing for Tagged Image File Format) is a flexible, 
' adaptable file format for handling images and data within a single file, 
' by including the header tags (size, definition, image-data arrangement, 
' applied image compression) defining the image's geometry. For example, a 
' TIFF file can be a container holding compressed (lossy) JPEG and (lossless) 
' PackBits compressed images. A TIFF file also can include a vector-based 
' clipping path (outlines, cropping, image frames). 
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE. 
'**************************************************************************/

Imports System
Imports System.Windows.Forms


Public Class MainForm

    Private Sub btnConvertToTiff_Click(sender As System.Object, e As System.EventArgs) Handles btnConvertToTiff.Click


        dlgOpenFileDialog.Multiselect = True
        dlgOpenFileDialog.Filter = "Image files (.jpg, .jpeg)|*.jpg;*.jpeg"

        If dlgOpenFileDialog.ShowDialog() = DialogResult.OK Then
            Try
                TiffImageConverter.ConvertJpegToTiff(dlgOpenFileDialog.FileNames, chkIsMultipage.Checked)
                MessageBox.Show("Image conversion completed.")
            Catch ex As Exception
                MessageBox.Show("An error occurred: " + ex.Message, "Error")
            End Try
        End If

    End Sub

    Private Sub btnConvertToJpeg_Click(sender As System.Object, e As System.EventArgs) Handles btnConvertToJpeg.Click

        dlgOpenFileDialog.Multiselect = False
        dlgOpenFileDialog.Filter = "Image files (.tif, .tiff)|*.tif;*.tiff"

        If dlgOpenFileDialog.ShowDialog() = DialogResult.OK Then
            Try
                TiffImageConverter.ConvertTiffToJpeg(dlgOpenFileDialog.FileName)
                MessageBox.Show("Image conversion completed.")
            Catch ex As Exception
                MessageBox.Show("An error occurred: " + ex.Message, "Error")
            End Try
        End If
    End Sub

End Class