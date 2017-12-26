/****************************** Module Header ******************************\
* Module Name: Thumbnail.cs
* Project:     CSWinFormSaveWebpageToImage
* Copyright (c) Microsoft Corporation
*
* The Thumbnail class receives the image's information from "FrmSaveWebPage.cs"
* form, generates a .bmp image from html code.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*****************************************************************************/

using System;
using System.Collections.Generic;
using System.Web;
using System.Windows.Forms;
using System.Threading;
using System.Drawing;


namespace CSWinFormSaveWebpageToImage
{
    public class WebpageThumbnail
    {
        public string Url
        {
            get;
            set;
        }

        public Bitmap ThumbnailImage
        {
            get;
            set;
        }
        public int Width
        {
            get;
            set;
        }

        public int Height
        {
            get;
            set;
        }

        public int BrowserWidth
        {
            get;
            set;
        }

        public int BrowserHeight
        {
            get;
            set;
        }

        public string Html
        {
            get;
            set;
        }

        public ThumbnailMethod Method
        {
            get;
            set;
        }

        public enum ThumbnailMethod
        {
            Url,
            Html
        };

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
                webBrowser.ScriptErrorsSuppressed = true;

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
    }
}