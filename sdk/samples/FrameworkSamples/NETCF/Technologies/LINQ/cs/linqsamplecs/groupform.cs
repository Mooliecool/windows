//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------
using System;
using System.Linq;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
namespace Microsoft.Samples.Linq
{
    /// <summary>
    /// Class for GroupForm
    /// </summary>
    public partial class GroupForm : Form
    {
        #region Constructor
        /// <summary>
        /// Constructor for GroupForm
        /// </summary>
        public GroupForm()
        {
            InitializeComponent();
        }

        #endregion

        #region HelperMethods
        /// <summary>
        /// Populates the List View with list of File System entries grouped by their extension
        /// </summary>
        private void PopulteListView()
        {
            //Clear the list view items
            this.lvGroup.Items.Clear();

            //LINQ query that lets you group the list of File System Entries by their extension
            //group has Key = Extension, Files = list of files of that Extension
            //this query creates new Anonymous Type with two members Extension and Files
            var query = from f in Directory.GetFileSystemEntries(this.txtDir.Text)
                    let fi = new FileInfo(f)
                    group f by fi.Extension into g
                    select new { Extension = g.Key, Files = g };


            //for every item in the query result
            foreach (var v in query)
            {
                string ext;

                //get the extension
                if(string.IsNullOrEmpty (v.Extension))
                    ext = "Folder";
                else
                    ext = v.Extension;

                //create new List View Item (Extension)
                ListViewItem lviExtension = new ListViewItem(ext);
                lviExtension.SubItems.Add("");
                lviExtension.SubItems.Add("");

                //add it to the ListView Items
                this.lvGroup.Items.Add(lviExtension);

                //for every file that belongs to the current Extension
                foreach (var file in v.Files)
                {
                    //Create new ListViewItem and add the file's Name and LastWriteTime
                    ListViewItem lviFile = new ListViewItem("");
                    FileInfo fi = new FileInfo(file);
                    lviFile.SubItems.Add(fi.Name);
                    lviFile.SubItems.Add(fi.LastWriteTime.ToString());

                    //add it to the ListView Items
                    this.lvGroup.Items.Add(lviFile);
                }
            }
            //make the List View visible
            this.lvGroup.Visible = true;
        }
        #endregion

        #region EventHandlers

        /// <summary>
        /// Click event handler for button Group
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnGroup_Click(object sender, EventArgs e)
        {
            //if text entered is empty, return
            if (string.IsNullOrEmpty(this.txtDir.Text))
                return;
            
            //if Directory does not exist, return
            if (!Directory.Exists(this.txtDir.Text))
                return;
            //set the Current Cursor to WaitCursor	
            Cursor.Current = Cursors.WaitCursor;

            //Group the directory by Extension
            PopulteListView();

            //set Cursor back to Default
            Cursor.Current = Cursors.Default ;
            
            
        }
        /// <summary>
        /// Click Event handler for menu item Exit
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void miExit_Click(object sender, EventArgs e)
        {
            //Close all instances of forms.
            Common.CloseAll();
            
            //exit the Application
            Application.Exit();
        }

        /// <summary>
        /// Click event handler for menu item Search View
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void miSearch_Click(object sender, EventArgs e)
        {

            //if SearchForm is null, create new SearchForm and Show it.
            if (Common.SearchForm == null)
            {
                Common.SearchForm = new SearchForm();
                Common.SearchForm.Show();
            }
            else //else show the already existing SearchForm
                Common.SearchForm.Show();
        }
        #endregion
    }

    
}