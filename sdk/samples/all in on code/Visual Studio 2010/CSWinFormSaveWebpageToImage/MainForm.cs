/****************************** Module Header ******************************\
* Module Name:  MainForm.cs
* Project:      CSWinFormSaveWebpageToImage
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to save the webpage as an image.
* 
* The code sample creates a WebBrowser to retrieve the target webpage's
* html code and uses WebBrowser.DrawToBitmap method convert the html 
* code to .bmp image. In this code-sample, users can set the image's 
* width, height and browser's width, height, generate an appropriate image.
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
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Reflection;

namespace CSWinFormSaveWebpageToImage
{
    public partial class MainForm : Form
    {
        public string rootDirectotyStr;

        public MainForm()
        {
            InitializeComponent();

            // Load web page of the application.
            this.rootDirectotyStr = Application.StartupPath;
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
                // Thumbnail image size
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
                    MessageBox.Show("Width or height are too small or too large. " +
                        "Please change the size.", "Application Warning",
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

        private void btnOpen_Click(object sender, EventArgs e)
        {
            try
            {
                webBrowserTargetPage.Navigate(new Uri(tbUrl.Text));
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
    }
}
