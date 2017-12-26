/****************************** Module Header ******************************\
* Module Name:  MainForm.cs
* Project:      CSWin7TaskbarThumbnail
* Copyright (c) Microsoft Corporation.
* 
* Thumbnail toolbars are another productivity feature that gives us the 
* ability to do more without actually switching to another application's 
* window.  A thumbnail toolbar is essentially a mini remote-control that is 
* displayed when you hover over the application's taskbar button, right 
* beneath the application’s thumbnail. 
* 
* CSWin7TaskbarThumbnail example demostrates how to set Taskbar Thumbnail
* previews, change Thumbnail previews order, set Thumbnail toolbars using 
* Taskbar related APIs in Windows API Code Pack.   
* 
* This MainForm can creates two Thumbnail previews based on previewPictureBox1
* and previewPicitureBox2, change the preivews order.  It also creates four
* Thumbnail toolbar buttons, First, Previous, Next, and Last to move the 
* selected image in the imageListView. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directive
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Microsoft.WindowsAPICodePack.Taskbar;
#endregion

namespace CSWin7TaskbarThumbnail
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        #region Thumbnail Preview members
        // The max count of the Thumbnail preview of this application
        const int MAX_PREVIEW_COUNT = 2;

        // Index of the TabControl
        const int THUMBNAIL_PREVIEW = 0;
        const int THUMBNAIL_TOOLBAR = 1;

        // The list holding the Thumbnail preview images
        private List<TabbedThumbnail> tabbedThumbnailList = new 
            List<TabbedThumbnail>(MAX_PREVIEW_COUNT);

        // Check the Windows version, then update the UI, otherwise
        // exit the current process
        private void MainForm_Load(object sender, EventArgs e)
        {
            // Check whether the current system is Windows 7 or 
            // Windows Server 2008 R2
            if (TaskbarManager.IsPlatformSupported)
            {
                // Update the buttons' enable status
                removeThumbnailButton.Enabled = false;
                removeThumbnailButton2.Enabled = false;
            }
            else
            {
                MessageBox.Show("Taskbar Application ID is not supported in"
                    + " your operation system!" + Environment.NewLine +
                    "Please launch the application in Windows 7 or " +
                    "Windows Server 2008 R2 systems.");

                // Exit the current process
                Application.Exit();
            }
        }


        // Check whether the Thumbnail preivew has created
        private static bool HasThumbnailPreview(Control control)
        {
            return TaskbarManager.Instance.TabbedThumbnail.
                GetThumbnailPreview(control) != null;
        }


        // Make the image on preivewPictureBox1 as a Thumbnail preview
        private void addThumbnailButton_Click(object sender, EventArgs e)
        {
            // Check whether the Thumbnail preview has been created
            if (!HasThumbnailPreview(previewPictureBox1))
            {
                // Add the Thumbnail preview
                this.AddThumbnailPreview(previewPictureBox1);

                // Update the buttons' enable status
                addThumbnailButton.Enabled = false;
                removeThumbnailButton.Enabled = true;
            }
        }


        // Remove the Thumbnail preview which is the image on 
        // preivewPictureBox1
        private void removeThumbnailButton_Click(object sender, EventArgs e)
        {
            // Try to retrieve the Thumbnail preview
            TabbedThumbnail preview = TaskbarManager.Instance.
                TabbedThumbnail.GetThumbnailPreview(previewPictureBox1);

            if (preview != null)
            {
                // Remove the Thumbnail preview
                this.RemoveThumbnailPreview(preview);

                // Update the buttons' enable status
                removeThumbnailButton.Enabled = false;
                addThumbnailButton.Enabled = true;
            }
        }


        // Make the image on preivewPictureBox2 as a Thumbnail preview
        private void addThumbnailButton2_Click(object sender, EventArgs e)
        {
            // Check whether the Thumbnail preview has been created
            if (!HasThumbnailPreview(previewPictureBox2))
            {
                // Add the Thumbnail preview
                this.AddThumbnailPreview(previewPictureBox2);

                // Update the buttons' enable status
                addThumbnailButton2.Enabled = false;
                removeThumbnailButton2.Enabled = true;
            }
        }


        // Remove the Thumbnail preview which is the image on 
        // preivewPictureBox1
        private void removeThumbnailButton2_Click(object sender, EventArgs e)
        {
            // Try to retrieve the Thumbnail preview
            TabbedThumbnail preview = TaskbarManager.Instance.
                TabbedThumbnail.GetThumbnailPreview(previewPictureBox2);

            if (preview != null)
            {
                // Remove the Thumbnail preview
                this.RemoveThumbnailPreview(preview);

                // Update the buttons' enable status
                removeThumbnailButton2.Enabled = false;
                addThumbnailButton2.Enabled = true;
            }
        }


        // Change the Thumbnail effect between Thumbnail preview and 
        // Thumbnail toolbar based on TabControl's selected index.
        private void thumbnailTabControl_SelectedIndexChanged(object sender,
            EventArgs e)
        {
            // Check if it is Thumbnail toolbar index
            if (thumbnailTabControl.SelectedIndex == THUMBNAIL_TOOLBAR)
            {
                // Clear all the Thumbnail previews first

                // Try to retrieve the Thumbnail preview of previewPictureBox1
                TabbedThumbnail preview = TaskbarManager.Instance.
                    TabbedThumbnail.GetThumbnailPreview(previewPictureBox1);

                if (preview != null)
                {
                    // Remove the Thumbnail preview
                    this.RemoveThumbnailPreview(preview);
                }

                // Try to retrieve the Thumbnail preview of previewPictureBox2
                preview = TaskbarManager.Instance.TabbedThumbnail.
                    GetThumbnailPreview(previewPictureBox2);

                if (preview != null)
                {
                    // Remove the Thumbnail preview
                    this.RemoveThumbnailPreview(preview);
                }


                // Then update the Thumbnail toolbar page effect
 
                // Select and focus the first image in the image list
                imageListView.Items[0].Selected = true;
                imageListView.Focus();

                // Check if the Thumbnail buttons have been created
                if (!toolBarButtonCreated)
                {
                    // Create the Thumbnail toolbar buttons
                    CreateToolbarButtons();

                    // Set the flag
                    toolBarButtonCreated = true;
                }
                else
                {
                    // Make all the Thumbnail toolbar buttons visible
                    ChangeVisibility(buttonPrevious, true);
                    ChangeVisibility(buttonFirst, true);
                    ChangeVisibility(buttonLast, true);
                    ChangeVisibility(buttonNext, true);
                }
            }
            // Check if it is Thumbnail preview index
            else if (thumbnailTabControl.SelectedIndex == THUMBNAIL_PREVIEW)
            {
                // Make all the Thumbnail toolbar buttons invisible
                ChangeVisibility(buttonPrevious, false);
                ChangeVisibility(buttonFirst, false);
                ChangeVisibility(buttonLast, false);
                ChangeVisibility(buttonNext, false);

                // Update the buttons' enable status
                addThumbnailButton.Enabled = true;
                removeThumbnailButton.Enabled = false;
                addThumbnailButton2.Enabled = true;
                removeThumbnailButton2.Enabled = false;
            }
        }


        // Change the Thumbnail previews order
        private void changePreviewOrderButton_Click(object sender, 
            EventArgs e)
        {
            // Check if the Thumbnail preview 
            if (tabbedThumbnailList.Count == MAX_PREVIEW_COUNT)
            {
                // Change the Thumbnail preview order
                TaskbarManager.Instance.TabbedThumbnail.SetTabOrder(
                    tabbedThumbnailList[1], tabbedThumbnailList[0]);

                // Reverse the Thumbnail preview image list
                tabbedThumbnailList.Reverse();
            }
        }


        // Make the given Control as the Thumbnail preview
        private void AddThumbnailPreview(Control control)
        {
            // Make a new Thumbnail preview object
            TabbedThumbnail preview = new TabbedThumbnail(this.Handle, 
                control);

            // Make the given Control as the Thumbnail preview 
            TaskbarManager.Instance.TabbedThumbnail.AddThumbnailPreview(
                preview);

            // Set the newly created Thumbnail preview as active
            TaskbarManager.Instance.TabbedThumbnail.SetActiveTab(preview);

            // Add the Thumbnail preview into the list
            tabbedThumbnailList.Add(preview);
        }


        // Remove the passed-in TabbedThumbnail preview object
        private void RemoveThumbnailPreview(TabbedThumbnail preview)
        {
            // Remove the Thumbnail preview 
            TaskbarManager.Instance.TabbedThumbnail.
                RemoveThumbnailPreview(preview);

            // Remove the Thumbnail preivew from the list
            tabbedThumbnailList.Remove(preview);
        }
        #endregion


        #region Thumbnail Toolbar Button members
        // Thumbnail toolbar button enum
        public enum ButtonTask
        {
            First,
            Last,
            Next,
            Previous,
            All
        }

        // The flag to record whether the Thumbnail toolbar buttons are 
        // created
        private bool toolBarButtonCreated = false;

        // The Thumbnail toolbar buttons instances
        private ThumbnailToolbarButton buttonPrevious;
        private ThumbnailToolbarButton buttonNext;
        private ThumbnailToolbarButton buttonFirst;
        private ThumbnailToolbarButton buttonLast;


        // Create the Thumbnail toolbar buttons instance
        private void CreateToolbarButtons()
        {
            // Initialize the First Thumbnail toolbar button
            buttonFirst = new ThumbnailToolbarButton(Properties.Resources.
                first, "First Image");

            // Update the button's enabled status and register Click event 
            // handler
            buttonFirst.Enabled = false;
            buttonFirst.Click += buttonFirst_Click;

            // Initialize the Previous Thumbnail toolbar button
            buttonPrevious = new ThumbnailToolbarButton(Properties.
                Resources.prevArrow, "Previous Image");

            // Update the button's enabled status and register Click event
            // handler
            buttonPrevious.Enabled = false;
            buttonPrevious.Click += buttonPrevious_Click;

            // Initialize the Next Thumbnail toolbar button
            buttonNext = new ThumbnailToolbarButton(Properties.Resources.
                nextArrow, "Next Image");

            // Register the Click event handler
            buttonNext.Click += buttonNext_Click;

            // Initialize the Last Thumbnail toolbar button
            buttonLast = new ThumbnailToolbarButton(Properties.Resources.
                last, "Last Image");

            // Register the Click event handler
            buttonLast.Click += buttonLast_Click;

            // Creates the four Thumbnail toolbar buttons
            TaskbarManager.Instance.ThumbnailToolbars.AddButtons(
                this.Handle,buttonFirst, buttonPrevious, buttonNext, 
                buttonLast);
        }


        // Update the visible and enable status of the Thumbnail buttons
        private void UpdateButtons()
        {
            ChangeVisibility(buttonPrevious, true, ButtonTask.Previous);
            ChangeVisibility(buttonFirst, true, ButtonTask.First);
            ChangeVisibility(buttonLast, true, ButtonTask.Last);
            ChangeVisibility(buttonNext, true, ButtonTask.Next);
        }


        // Change the certain Thumbnail button's visible and enable status
        // based on the current selected index in the imageListView
        private void ChangeVisibility(ThumbnailToolbarButton btn, 
            bool show, ButtonTask task)
        {
            if (btn != null)
            {
                // Update the button's UI status
                btn.Visible = show;
                btn.IsInteractive = show;

                // Change the certain button enable status based on the 
                // selected index of the imageListView
                switch (task)
                {
                    case ButtonTask.First:
                    case ButtonTask.Previous:
                        // The First and Previous Thumbnail toolbar button 
                        // is enabled if the first image in the 
                        // imageListView is not selected
                        btn.Enabled = !imageListView.Items[0].Selected;
                        break;
                    case ButtonTask.Last:
                    case ButtonTask.Next:
                        // The Last and Next Thumbnail toolbar button 
                        // is enabled if the last image in the 
                        // imageListView is not selected
                        btn.Enabled = !imageListView.Items[imageListView.
                            Items.Count - 1].Selected;
                        break;
                }
            }
        }


        // Only update the Thumbnal toolbar button visible status
        private void ChangeVisibility(ThumbnailToolbarButton btn, 
            bool show)
        {
            ChangeVisibility(btn, show, ButtonTask.All);
        }


        // Update the selected index when the certain Thumbnail toolbar 
        // button is clicked
        private void ShowList(int newIndex)
        {
            // Check whether the selected index is valid
            if (newIndex < 0 || newIndex > imageListView.Items.Count - 1)
                return;

            // Update the selected index and focus the imageListView
            imageListView.Items[newIndex].Selected = true;
            imageListView.Focus();
        }


        // Select the previous item in the imageListView
        private void buttonPrevious_Click(object sender, EventArgs e)
        {
            ShowList(imageListView.SelectedIndices[0] - 1);
        }


        // Select the next item in the imageListView
        private void buttonNext_Click(object sender, EventArgs e)
        {
            ShowList(imageListView.SelectedIndices[0] + 1);
        }


        // Select the first item in the imageListView
        private void buttonFirst_Click(object sender, EventArgs e)
        {
            ShowList(0);
        }


        // Select the last item in the imageListView
        private void buttonLast_Click(object sender, EventArgs e)
        {
            ShowList(imageListView.Items.Count - 1);
        }


        // Show different image in the imagePictureBox based on the 
        // selected index of the imageListView
        private void imageListView_SelectedIndexChanged(object sender, 
            EventArgs e)
        {
            if (imageListView.SelectedItems.Count > 0)
            {
                Image image;
                // Retrieve the image from the Resource according to the 
                // selected index of the imageListView
                switch (imageListView.SelectedItems[0].ImageIndex)
                {
                    case 0:
                        image = Properties.Resources.C;
                        break;
                    case 1:
                        image = Properties.Resources.O;
                        break;
                    case 2:
                        image = Properties.Resources.D;
                        break;
                    default:
                        image = Properties.Resources.E;
                        break;
                }

                // Update the image in the imagePictureBox
                imagePictureBox.Image = image;

                // Update the visible and enable status of the Thumbnail
                // toolbar buttons
                UpdateButtons();
            }
        }
        #endregion
    }
}
