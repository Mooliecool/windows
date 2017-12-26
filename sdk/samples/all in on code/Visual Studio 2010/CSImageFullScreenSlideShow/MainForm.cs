/********************************** Module Header **********************************\
* Module Name:  MainForm.cs
* Project:      CSImageFullScreenSlideShow
* Copyright (c) Microsoft Corporation.
*
* The sample demonstrates how to Show slides full screen and how to modify the internal of
 * these image slides playing.
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***********************************************************************************/

using System;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;


namespace CSImageFullScreenSlideShow
{
    public partial class MainForm : Form
    {
        private string[] imageFiles = null;

        // Image index
        private int selected = 0;
        private int begin = 0;
        private int end = 0;

        private FullScreen fullScreen = new FullScreen();

        public MainForm()
        {
            InitializeComponent();

            this.btnPrevious.Enabled = false;
            this.btnNext.Enabled = false;
            this.btnImageSlideShow.Enabled = false;
        }

        /// <summary>
        /// Select the image folder.
        /// </summary>
        private void btnOpenFolder_Click(object sender, EventArgs e)
        {
            if (this.imageFolderBrowserDlg.ShowDialog() == DialogResult.OK)
            {
                this.imageFiles = GetFiles(this.imageFolderBrowserDlg.SelectedPath,
                    "*.jpg;*.jpeg;*.png;*.bmp;*.tif;*.tiff;*.gif");

                this.selected = 0;
                this.begin = 0;
                this.end = imageFiles.Length;

                if (this.imageFiles.Length == 0)
                {
                    MessageBox.Show("No image can be found");

                    this.btnPrevious.Enabled = false;
                    this.btnNext.Enabled = false;
                    this.btnImageSlideShow.Enabled = false;
                }
                else
                {
                    ShowImage(imageFiles[selected], pictureBox);

                    this.btnPrevious.Enabled = true;
                    this.btnNext.Enabled = true;
                    this.btnImageSlideShow.Enabled = true;
                }
            }
        }

        public static string[] GetFiles(string path, string searchPattern)
        {
            string[] patterns = searchPattern.Split(';');
            List<string> files = new List<string>();
            foreach (string filter in patterns)
            {
                // Iterate through the directory tree and ignore the 
                // DirectoryNotFoundException or UnauthorizedAccessException 
                // exceptions. 
                // http://msdn.microsoft.com/en-us/library/bb513869.aspx

                // Data structure to hold names of subfolders to be
                // examined for files.
                Stack<string> dirs = new Stack<string>(20);

                if (!Directory.Exists(path))
                {
                    throw new ArgumentException();
                }
                dirs.Push(path);

                while (dirs.Count > 0)
                {
                    string currentDir = dirs.Pop();
                    string[] subDirs;
                    try
                    {
                        subDirs = Directory.GetDirectories(currentDir);
                    }
                    // An UnauthorizedAccessException exception will be thrown 
                    // if we do not have discovery permission on a folder or 
                    // file. It may or may not be acceptable to ignore the 
                    // exception and continue enumerating the remaining files 
                    // and folders. It is also possible (but unlikely) that a 
                    // DirectoryNotFound exception will be raised. This will 
                    // happen if currentDir has been deleted by another 
                    // application or thread after our call to Directory.Exists. 
                    // The choice of which exceptions to catch depends entirely 
                    // on the specific task you are intending to perform and 
                    // also on how much you know with certainty about the 
                    // systems on which this code will run.
                    catch (UnauthorizedAccessException)
                    {
                        continue;
                    }
                    catch (DirectoryNotFoundException)
                    {
                        continue;
                    }

                    try
                    {
                        files.AddRange(Directory.GetFiles(currentDir, filter));
                    }
                    catch (UnauthorizedAccessException)
                    {
                        continue;
                    }
                    catch (DirectoryNotFoundException)
                    {
                        continue;
                    }

                    // Push the subdirectories onto the stack for traversal.
                    // This could also be done before handing the files.
                    foreach (string str in subDirs)
                    {
                        dirs.Push(str);
                    }
                }
            }

            return files.ToArray();
        }

        /// <summary>
        /// Click the "Previous" button to navigate to the previous image.
        /// </summary>
        private void btnPrevious_Click(object sender, EventArgs e)
        {
            if (this.imageFiles == null || this.imageFiles.Length == 0)
            {
                MessageBox.Show("Please select the images to slideshow!");
                return;
            }
            ShowPrevImage();
        }

        /// <summary>
        /// Click the "Next" button to navigate to the next image.
        /// </summary>
        private void btnNext_Click(object sender, EventArgs e)
        {
            if (this.imageFiles == null || this.imageFiles.Length == 0)
            {
                MessageBox.Show("Please select the images to slideshow!");
                return;
            }
            ShowNextImage();
        }

        /// <summary>
        /// Image slideshow.
        /// </summary>
        private void btnImageSlideShow_Click(object sender, EventArgs e)
        {
            if (this.imageFiles == null || this.imageFiles.Length == 0)
            {
                MessageBox.Show("Please select the images to slideshow!");
                return;
            }

            if (timer.Enabled == true)
            {
                this.timer.Enabled = false;
                this.btnOpenFolder.Enabled = true;
                this.btnImageSlideShow.Text = "Start Slideshow";
            }
            else
            {
                this.timer.Enabled = true;
                this.btnOpenFolder.Enabled = false;
                this.btnImageSlideShow.Text = "Stop Slideshow";
            }
        }

        /// <summary>
        /// Show the next image at every regular intervals.
        /// </summary>
        private void timer_Tick(object sender, EventArgs e)
        {
            ShowNextImage();
        }

        /// <summary>
        /// Show child windows to alternate the settings about Timer control.
        /// </summary>
        private void btnSetting_Click(object sender, EventArgs e)
        {
            Settings frmSettings = new Settings(ref timer);
            frmSettings.ShowDialog();
        }

        /// <summary>
        /// Enter or leave the full screen mode.
        /// </summary>
        private void btnFullScreen_Click(object sender, EventArgs e)
        {
            if (!this.fullScreen.IsFullScreen)
            {
                // Hide the buttons and max the slideshow panel.
                this.gbButtons.Visible = false;
                this.pnlSlideShow.Dock = DockStyle.Fill;

                fullScreen.EnterFullScreen(this);
            }
        }

        /// <summary>
        /// Respond to the keystroke "ESC".
        /// </summary>
        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            if (keyData == Keys.Escape)
            {
                if (this.fullScreen.IsFullScreen)
                {
                    // Unhide the buttons and restore the slideshow panel.
                    this.gbButtons.Visible = true;
                    this.pnlSlideShow.Dock = DockStyle.Top;

                    fullScreen.LeaveFullScreen(this);
                }
                return true;
            }
            else
                return base.ProcessCmdKey(ref msg, keyData);
        }

        /// <summary>
        /// Show the image in the PictureBox.
        /// </summary>
        public static void ShowImage(string path, PictureBox pct)
        {
            pct.ImageLocation = path;
        }

        /// <summary>
        /// Show the previous image.
        /// </summary>
        private void ShowPrevImage()
        {
            ShowImage(this.imageFiles[(--this.selected) % this.imageFiles.Length], this.pictureBox);
        }

        /// <summary>
        /// Show the next image.
        /// </summary>
        private void ShowNextImage()
        {
            ShowImage(this.imageFiles[(++this.selected) % this.imageFiles.Length], this.pictureBox);
        }
    }
}
