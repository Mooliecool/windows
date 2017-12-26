'***************************** Module Header ******************************\ 
' Module Name:    TiffImageConverter.vb 
' Project:        VBTiffImageConverter
' Copyright (c) Microsoft Corporation. 
'
' The class defines the helper methods for converting TIFF from or to JPEG 
' file(s)
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE. 
'**************************************************************************/


Imports System.Drawing
Imports System.Drawing.Imaging
Imports System.IO


Public Class TiffImageConverter
    ''' <summary>
    ''' Converts tiff image(s) to jpeg image(s).
    ''' </summary>
    ''' <param name="fileName">Full name to tiff image.</param>
    ''' <returns>String array having full name to jpeg images.</returns>
    Public Shared Function ConvertTiffToJpeg(fileName As String) As String()
        Using imageFile As Image = Image.FromFile(fileName)
            Dim frameDimensions As New FrameDimension(imageFile.FrameDimensionsList(0))

            ' Gets the number of pages from the tiff image (if multi-page)
            Dim frameNum As Integer = imageFile.GetFrameCount(frameDimensions)
            Dim jpegPaths As String() = New String(frameNum) {}

            Dim frame As Integer = 0
            While frame < frameNum
                ' Selects one frame at a time and save as jpeg.
                imageFile.SelectActiveFrame(frameDimensions, frame)
                Using bmp As New Bitmap(imageFile)
                    jpegPaths(frame) = [String].Format("{0}\{1}{2}.jpeg",
                                       Path.GetDirectoryName(fileName),
                                       Path.GetFileNameWithoutExtension(fileName), frame)
                    bmp.Save(jpegPaths(frame), ImageFormat.Jpeg)
                End Using
                System.Math.Max(System.Threading.Interlocked.Increment(frame), frame - 1)
            End While

            Return jpegPaths
        End Using
    End Function

    ''' <summary>
    ''' Converts jpeg image(s) to tiff image(s).
    ''' </summary>
    ''' <param name="fileNames">
    ''' String array having full name to jpeg images.
    ''' </param>
    ''' <param name="isMultipage">
    ''' true to create single multi-page tiff file otherwise, false.
    ''' </param>
    ''' <returns>
    ''' String array having full name to tiff images.
    ''' </returns>
    Public Shared Function ConvertJpegToTiff(fileNames As String(),
                                             isMultipage As Boolean) As String()
        Using encoderParams As New EncoderParameters(1)
            Dim tiffCodecInfo As ImageCodecInfo = ImageCodecInfo.GetImageEncoders(). _
                First(Function(ie) ie.MimeType = "image/tiff")

            Dim tiffPaths As String() = Nothing
            If isMultipage Then
                tiffPaths = New String(1) {}
                Dim tiffImg As Image = Nothing
                Try
                    Dim i As Integer = 0
                    While i < fileNames.Length
                        If i = 0 Then
                            tiffPaths(i) = [String].Format("{0}\{1}.tiff",
                                        Path.GetDirectoryName(fileNames(i)),
                                        Path.GetFileNameWithoutExtension(fileNames(i)))

                            ' Initialize the first frame of multi-page tiff.
                            tiffImg = Image.FromFile(fileNames(i))
                            encoderParams.Param(0) = New EncoderParameter(
                                Encoder.SaveFlag, DirectCast(EncoderValue.MultiFrame, 
                                EncoderParameterValueType))
                            tiffImg.Save(tiffPaths(i), tiffCodecInfo, encoderParams)
                        Else
                            ' Add additional frames.
                            encoderParams.Param(0) = New EncoderParameter(
                                Encoder.SaveFlag,
                                DirectCast(EncoderValue.FrameDimensionPage, 
                                EncoderParameterValueType))
                            Using frame As Image = Image.FromFile(fileNames(i))
                                tiffImg.SaveAdd(frame, encoderParams)
                            End Using
                        End If

                        If i = fileNames.Length - 1 Then
                            ' When it is the last frame, flush the resources and closing.
                            encoderParams.Param(0) = New EncoderParameter(
                                Encoder.SaveFlag,
                                DirectCast(EncoderValue.Flush, EncoderParameterValueType))
                            tiffImg.SaveAdd(encoderParams)
                        End If
                        System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
                    End While
                Finally
                    If tiffImg IsNot Nothing Then
                        tiffImg.Dispose()
                        tiffImg = Nothing
                    End If
                End Try
            Else
                tiffPaths = New String(fileNames.Length) {}

                Dim i As Integer = 0
                While i < fileNames.Length
                    tiffPaths(i) = [String].Format("{0}\{1}.tiff",
                                       Path.GetDirectoryName(fileNames(i)),
                                       Path.GetFileNameWithoutExtension(fileNames(i)))

                    ' Save as individual tiff files.
                    Using tiffImg As Image = Image.FromFile(fileNames(i))
                        tiffImg.Save(tiffPaths(i), ImageFormat.Tiff)
                    End Using
                    System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
                End While
            End If

            Return tiffPaths
        End Using
    End Function
End Class