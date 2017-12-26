'****************************** Module Header ******************************\
' Module Name:  MainForm.vb
' Project:      VBWinFormSaveWebpageToImage
' Copyright (c) Microsoft Corporation
'
' The project illustrates how to save the webpage as an image.
' 
' The code sample creates a WebBrowser to retrieve the target webpage's
' html code and uses WebBrowser.DrawToBitmap method convert the html 
' code to .bmp image. In this code-sample, users can set the image's 
' width, height and browser's width, height, generate an appropriate image.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*****************************************************************************/

Public Class MainForm

    Public rootDirectotyStr As String

    Private Sub MainForm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) _
        Handles MyBase.Load

        ' Load web page of the application.
        rootDirectotyStr = Application.StartupPath
        Dim webPageUrl As String = rootDirectotyStr & "\Default.htm"
        webBrowserTargetPage.Url = New Uri(webPageUrl, UriKind.RelativeOrAbsolute)
        Me.pctPreview.SizeMode = PictureBoxSizeMode.Zoom
    End Sub

    ''' <summary>
    ''' Convert WebBrowser's web page as an image and rendering in this page.
    ''' You can also find an image in sample project root directory.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub btnSavePage_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSavePage.Click
        Try
            ' Thumbnail image size
            Dim width As Integer
            Dim height As Integer
            If Not Integer.TryParse(tbWidth.Text.Trim(), width) OrElse _
                Not Integer.TryParse(tbHeight.Text.Trim(), height) Then
                MessageBox.Show("Width or height must be integer number.")
                Return
            End If
            If width <= 0 OrElse width > 2000 OrElse height <= 0 OrElse height > 6000 Then
                MessageBox.Show("Width(1-2000) or height(1-6000) are too small or too large. " & _
                                "Please change the size.", "Application Warning", _
                                MessageBoxButtons.OK, MessageBoxIcon.Warning)
                Return
            End If

            ' Save web page as an image in root diectory, add an image in page.
            Dim htmlCode As String = webBrowserTargetPage.DocumentText
            Dim thumb As New WebPageThumbnail(htmlCode, width, height, width, height, _
                                              WebPageThumbnail.ThumbnailMethod.Html)
            Dim imageWebpage As Bitmap = thumb.GenerateThumbnail()
            imageWebpage.Save(rootDirectotyStr & "/image.bmp")
            pctPreview.Load(rootDirectotyStr & "/image.bmp")
        Catch ex As Exception
            MessageBox.Show(ex.Message + " Please try again")
        End Try

    End Sub

    Private Sub btnOpen_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnOpen.Click
        Try
            webBrowserTargetPage.Navigate(New Uri(tbUrl.Text))
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try
    End Sub
End Class
