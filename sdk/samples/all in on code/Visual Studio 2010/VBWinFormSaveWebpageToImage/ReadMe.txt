=============================================================================
                VBWinFormSaveWebpageToImage Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The project illustrates how to save the webpage as an image.

The code sample creates a WebBrowser to retrieve the target webpage's
html code and uses be WebBrowser.DrawToBitmap method convert the html 
code to .bmp image. In this code-sample, users can set the image's width,
height and browser's width, height, generate an appropriate image.


/////////////////////////////////////////////////////////////////////////////
Demo the Sample. 

Please follow these demonstration steps below.

Step 1: Open the VBWinFormSaveWebpageToImage.sln.

Step 2: Expand the VBWinFormSaveWebpageToImage web application and press 
        Ctrl + F5 to show the MainForm.vb form.

Step 3: We will see a WebBrowser control, two TextBox controls, a Button,
        and a PictureBox control on the form, you can find the WebBrowser
		show a web page, the Save Page button will save the current web 
		page of WebBrowser control.

Step 4: Click the button to save an image of web application, you can also
        preview it on the PictureBox control. 

Step 5: If you update the image's size with TextBox controls text, you can
        retrieve the image in the root directory of web application.

Step 6: You can even click the link of the Default.htm page to redirect the 
        www.msdn.com, and click Save Page button to save the online 
		website's page as an image, if you find the image can not contain 
		whole content of pages, please adjust width or height TextBox of 
		MainForm.vb form.

Step 7: Validation finished.


/////////////////////////////////////////////////////////////////////////////
Implementation:

Step 1. Create a VB "Windows Forms Application" in Visual Studio 2010 or
        Visual Web Developer 2010. Name it as "VBWinFormSaveWebpageToImage".

Step 2. Add one windows form and one class file and named them as 
        "MainForm.vb", "WebPageThumbnail.vb".

Step 3. The WebPageThumbnail class used to receive image's information and 
        generate an appropriate image with html code.
		[code]
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
        [/code]

Step 4. In MainForm.vb form, we load the Default.htm page and use
        a button named "btnSavePage" to create image.
        [code]
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
		[/code]


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: BitMap Class
http://msdn.microsoft.com/en-us/library/system.drawing.bitmap.aspx

MSDN: WebBrowser Class
http://msdn.microsoft.com/en-us/library/system.windows.forms.webbrowser.aspx


/////////////////////////////////////////////////////////////////////////////