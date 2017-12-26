/****************************** Module Header ******************************\
Module Name:  MainForm.cs
Project:      CSApplicationHang
Copyright (c) Microsoft Corporation.



This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;


namespace CSApplicationHang
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void btnSpinning_Click(object sender, EventArgs e)
        {
            Spinning.Trigger();
        }

        private void btnDeadlock_Click(object sender, EventArgs e)
        {
            Deadlock.Trigger();
        }

        private void btnContention_Click(object sender, EventArgs e)
        {
            Contention.Trigger();
        }
    }
}