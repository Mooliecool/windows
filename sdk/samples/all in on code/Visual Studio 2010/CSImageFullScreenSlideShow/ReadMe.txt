=============================================================================
       Windows APPLICATION: CSImageFullScreenSlideShow Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The sample demonstrates how to display image slideshow in a Windows Forms 
application.  It also shows how to enter the full screen mode to slide-show 
images. 


/////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Build and run the sample project in Visual Studio 2010. 

Step2. Prepare some image files.  Click the "Open Folder..." button and 
       select the path which includes image files. 

Step3. Click "Previous" button and "Next" button to make image files 
       displayed in order.

Step4. Left-click the "Settings" button and select the internal between the 
       displayed image files for Timer control in order to display them 
       with a fixed interval time. Finally, left-click the "Start Slideshow"
	   button to make the image files displayed one by one.

Step5. Left-click the "Full Screen" button to display images in the full 
screen mode.  Press the "ESC" key to leave the full screen mode.


/////////////////////////////////////////////////////////////////////////////
Implementation:

1. When user selects the root folder of image files, the sample enumerates 
   the image files in the folder using the stack-based iteration method 
   demonstrated in this MSDN article: 
   http://msdn.microsoft.com/en-us/library/bb513869.aspx
   The sample does not use 
        Directory.GetFiles(path, "*.*", SearchOption.AllDirectories);
   to enumerate the files because it will abort when the user does not have 
   access permissions for certain directories or files in the root folder.

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

2. The sample displays the images in a PictureBox. 

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

   A timer is used to automatically slideshow the images.

        /// <summary>
        /// Show the next image at every regular intervals.
        /// </summary>
        private void timer_Tick(object sender, EventArgs e)
        {
            ShowNextImage();
        }

2. To slide-show images in the full-screen mode, the sample provides a helper 
   class 'FullScreen'.  FullScreen.cs contains two public methods: 
   
        EnterFullScreen - used to make a Windows Form display in the full screen.
        LeaveFullScreen - used to restore a Windows Form to its original state.

        /// <summary>
        /// Maximize the window to the full screen.
        /// </summary>
        public void EnterFullScreen(Form targetForm)
        {
            if (!IsFullScreen)
            {
                Save(targetForm);  // Save the original form state.

                targetForm.WindowState = FormWindowState.Maximized;
                targetForm.FormBorderStyle = FormBorderStyle.None;
                targetForm.TopMost = true;
                targetForm.Bounds = Screen.GetBounds(targetForm);

                IsFullScreen = true;
            }
        }

        /// <summary>
        /// Leave the full screen mode and restore the original window state.
        /// </summary>
        public void LeaveFullScreen(Form targetForm)
        {
            if (IsFullScreen)
            {
                // Restore the original Window state.
                targetForm.WindowState = winState;
                targetForm.FormBorderStyle = brdStyle;
                targetForm.TopMost = topMost;
                targetForm.Bounds = bounds;

                IsFullScreen = false;
            }
        }


/////////////////////////////////////////////////////////////////////////////
References:

How to: Iterate Through a Directory Tree (C# Programming Guide)
http://msdn.microsoft.com/en-us/library/bb513869.aspx

Screen.GetBounds Method 
http://msdn.microsoft.com/en-us/library/system.windows.forms.screen.getbounds.aspx


/////////////////////////////////////////////////////////////////////////////
