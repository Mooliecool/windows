/****************************** Module Header ******************************\
* Module Name:  SubForm.cs
* Project:      CSWin7TaskbarAppID
* Copyright (c) Microsoft Corporation.
* 
* This SubForm remove its form object reference from the MainForm's list when
* this SubForm is closing. 
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

namespace CSWin7TaskbarAppID
{
    public partial class SubForm : Form
    {
        public SubForm()
        {
            InitializeComponent();
        }

        // Remove the current SubForm reference from the MainForm's list
        private void SubForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            // Find the MainForm reference
            MainForm mainForm = Application.OpenForms["MainForm"] as MainForm;
            if (mainForm != null)
            {
                // Remove the current SubForm reference from the list
                mainForm.SubFormList.Remove(this);
            }
        }
    }
}
