=============================================================================
                CSWinFormSaveWebpageToImage Overview
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

Step 1: Open the CSWinFormSaveWebpageToImage.sln.

Step 2: Expand the CSWinFormSaveWebpageToImage web application and press 
        Ctrl + F5 to show the MainForm.cs form.

Step 3: We will see a WebBrowser control, two TextBox controls, a Button,
        and a PictureBox control on the form, you can find the WebBrowser
		show a web page, the Save Page button will save the current web 
		page of WebBrowser control.

Step 4: Click the button to save an image in web application, you can also
        preview it on the PictureBox control. 

Step 5: If you update the image's size with TextBox controls text, you can
        retrieve the image in the root directory of web application.

Step 6: You can even click the link of the Default.htm page to redirect the 
        www.msdn.com, and click Save Page button to save the online 
		website's page as an image, if you find the image can not contain 
		whole content of pages, please adjust width or height TextBox of 
		MainForm.cs form.

Step 7: Validation finished.

/////////////////////////////////////////////////////////////////////////////
Implementation:

Step 1. Create a C# "Windows Forms Application" in Visual Studio 2010 or
        Visual Web Developer 2010. Name it as "CSWinFormSaveWebpageToImage".

Step 2. Add one windows form and one class file and named them as 
        "MainForm.cs", "WebPageThumbnail.cs".

Step 3. The WebPageThumbnail class used to receive image's information and 
        generate an appropriate image with html code.
		[code]
        // Constructor method
        public WebpageThumbnail(string data, int browserWidth, int browserHeight, int thumbnailWidth, int thumbnailHeight, ThumbnailMethod method)
        {
            this.Method = method;
            if (method == ThumbnailMethod.Url)
            {
                this.Url = data;
            }
            else if (method == ThumbnailMethod.Html)
            {
                this.Html = data;
            }
            this.BrowserWidth = browserWidth;
            this.BrowserHeight = browserHeight;
            this.Height = thumbnailHeight;
            this.Width = thumbnailWidth;
        }

        /// <summary>
        /// Create a thread to execute GenerateThumbnailInteral method.
        /// Because the System.Windows.Forms.WebBrowser control has to 
        /// run on a STA thread while the current thread is MTA.
        /// </summary>
        /// <returns></returns>
        public Bitmap GenerateThumbnail()
        {
            Thread thread = new Thread(new ThreadStart(GenerateThumbnailInteral));
            thread.SetApartmentState(ApartmentState.STA);
            thread.Start();
            thread.Join();
            return ThumbnailImage;
        }

        /// <summary>
        /// This method creates WebBrowser instance retrieve the html code. Invoke WebBrowser_DocumentCompleted 
        /// method and convert html code to a bmp image.
        /// </summary>
        private void GenerateThumbnailInteral()
        {
            WebBrowser webBrowser = new WebBrowser();
            try
            {             
                webBrowser.ScrollBarsEnabled = false;
                if (this.Method == ThumbnailMethod.Url)
                {
                    webBrowser.Navigate(this.Url);
                }
                else
                {
                    webBrowser.DocumentText = this.Html;
                }
                webBrowser.DocumentCompleted += new WebBrowserDocumentCompletedEventHandler(WebBrowser_DocumentCompleted);
                while (webBrowser.ReadyState != WebBrowserReadyState.Complete)
                {
                    Application.DoEvents();
                }
            }
            finally
            {
                webBrowser.Dispose();
            }
        }

        private void WebBrowser_DocumentCompleted(object sender, WebBrowserDocumentCompletedEventArgs e)
        {
            WebBrowser webBrowser = (WebBrowser)sender;
            webBrowser.ClientSize = new Size(this.BrowserWidth, this.BrowserHeight);
            webBrowser.ScrollBarsEnabled = false;
            this.ThumbnailImage = new Bitmap(webBrowser.Bounds.Width, webBrowser.Bounds.Height);
            webBrowser.BringToFront();
            webBrowser.DrawToBitmap(ThumbnailImage, webBrowser.Bounds);
            this.ThumbnailImage = (Bitmap)ThumbnailImage.GetThumbnailImage(Width, Height, null, IntPtr.Zero);
        }
		[/code]

Step 4. In MainForm.cs form, we load the Default.htm page and use
        a button named "btnSavePage" to create image.
        [code]
		public string rootDirectotyStr = string.Empty;
        public MainForm()
        {
            InitializeComponent();

            // Load web page of the application.
            rootDirectotyStr = Application.StartupPath;
            string webPageUrl = rootDirectotyStr + "\\Default.htm";
            webBrowserTargetPage.Url = new Uri(webPageUrl, UriKind.RelativeOrAbsolute);
            this.pctPreview.SizeMode = PictureBoxSizeMode.Zoom;
        }

        /// <summary>
        /// Convert WebBrowser's web page as an image and rendering in this page.
        /// You can also find an image in sample project root directory.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnSavePage_Click(object sender, EventArgs e)
        {
            try
            {             
                // Image's size
                int width;
                int height;
                if (!int.TryParse(tbWidth.Text.Trim(), out width) ||
                    !int.TryParse(tbHeight.Text.Trim(), out height))
                {
                    MessageBox.Show("Width or height must be integer number.");
                    return;
                }
                if (width <= 0 || width > 2000 || height <= 0 || height > 6000)
                {
                    MessageBox.Show("Width or height are too small or too large. Please change another one.", "Application Warning",
                                     MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return;
                }

                // Save web page as an image in root directory, add an image in page.
                string htmlCode = webBrowserTargetPage.DocumentText;
                WebpageThumbnail thumb = new WebpageThumbnail(htmlCode, width, height, width, height, WebpageThumbnail.ThumbnailMethod.Html);
                Bitmap imageWebpage = thumb.GenerateThumbnail();
                imageWebpage.Save(rootDirectotyStr + "/image.bmp");
                pctPreview.Load(rootDirectotyStr + "/image.bmp");
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message + " Please try again");
            }
        }
		[/code]


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: BitMap Class
http://msdn.microsoft.com/en-us/library/system.drawing.bitmap.aspx

MSDN: WebBrowser Class
http://msdn.microsoft.com/en-us/library/system.windows.forms.webbrowser.aspx


/////////////////////////////////////////////////////////////////////////////