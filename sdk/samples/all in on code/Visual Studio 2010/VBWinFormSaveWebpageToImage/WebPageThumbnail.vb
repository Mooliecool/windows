'****************************** Module Header ******************************\
' Module Name: Thumbnail.vb
' Project:     VBWinFormSaveWebpageToImage
' Copyright (c) Microsoft Corporation
'
' The Thumbnail class receives the image's information from "MainForm.vb"
' form, generates a .bmp image from html code.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*****************************************************************************/

Imports System.Drawing
Imports System.Windows.Forms
Imports System.Threading


Public Class WebPageThumbnail

    ' Image's properties
    Public Property Url() As String
    Public Property ThumbnailImage() As Bitmap
    Public Property Width() As Integer
    Public Property Height() As Integer
    Public Property BrowserWidth() As Integer
    Public Property BrowserHeight() As Integer
    Public Property Html() As String
    Public Property Method() As ThumbnailMethod

    Public Enum ThumbnailMethod
        Url
        Html
    End Enum

    ' Constructor method
    Public Sub New(ByVal data As String, ByVal browserWidth As Integer, ByVal browserHeight As Integer, ByVal thumbnailWidth As Integer, ByVal thumbnailHeight As Integer, ByVal method As ThumbnailMethod)
        Me.Method = method
        If method = ThumbnailMethod.Url Then
            Me.Url = data
        ElseIf method = ThumbnailMethod.Html Then
            Me.Html = data
        End If
        Me.BrowserWidth = browserWidth
        Me.BrowserHeight = browserHeight
        Me.Height = thumbnailHeight
        Me.Width = thumbnailWidth
    End Sub

    ''' <summary>
    ''' Create a thread to execute GenerateThumbnailInteral method.
    ''' Because the System.Windows.Forms.WebBrowser control has to 
    ''' run on a STA thread while the current thread is MTA.
    ''' </summary>
    ''' <returns></returns>
    Public Function GenerateThumbnail() As Bitmap
        Dim thread As New Thread(New ThreadStart(AddressOf GenerateThumbnailInteral))
        thread.SetApartmentState(ApartmentState.STA)
        thread.Start()
        thread.Join()
        Return ThumbnailImage
    End Function

    ''' <summary>
    ''' This method creates WebBrowser instance retrieve the html code. Invoke WebBrowser_DocumentCompleted 
    ''' method and convert html code to a bmp image.
    ''' </summary>
    Private Sub GenerateThumbnailInteral()
        Dim webBrowser As New WebBrowser()
        Try
            webBrowser.ScrollBarsEnabled = False
            webBrowser.ScriptErrorsSuppressed = True

            If Me.Method = ThumbnailMethod.Url Then
                webBrowser.Navigate(Me.Url)
            Else
                webBrowser.DocumentText = Me.Html
            End If
            AddHandler webBrowser.DocumentCompleted, AddressOf WebBrowser_DocumentCompleted
            While webBrowser.ReadyState <> WebBrowserReadyState.Complete
                Application.DoEvents()
            End While
        Catch e As Exception
            ' Record the exception...
            Throw e
        Finally
            webBrowser.Dispose()
        End Try
    End Sub

    Private Sub WebBrowser_DocumentCompleted(ByVal sender As Object, ByVal e As WebBrowserDocumentCompletedEventArgs)
        Dim webBrowser As WebBrowser = DirectCast(sender, WebBrowser)
        WebBrowser.ClientSize = New Size(Me.BrowserWidth, Me.BrowserHeight)
        WebBrowser.ScrollBarsEnabled = False
        Me.ThumbnailImage = New Bitmap(WebBrowser.Bounds.Width, WebBrowser.Bounds.Height)
        WebBrowser.BringToFront()
        WebBrowser.DrawToBitmap(ThumbnailImage, WebBrowser.Bounds)
        Me.ThumbnailImage = DirectCast(ThumbnailImage.GetThumbnailImage(Width, Height, Nothing, IntPtr.Zero), Bitmap)
    End Sub

End Class
