/****************************** Module Header ******************************\
* Module Name:  MainForm.cs
* Project:      CSShellCommonFileDialog
* Copyright (c) Microsoft Corporation.
* 
* The code sample demos the use of shell common file dialogs.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
#endregion


namespace CSShellCommonFileDialog
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void btnCFDInBCL_Click(object sender, EventArgs e)
        {
            // Demo Common File Dialogs through .NET BCL
            CFDInBCLForm bclForm = new CFDInBCLForm();
            bclForm.ShowDialog();
        }

        private void btnCFDInCodePack_Click(object sender, EventArgs e)
        {
            // Demo Common File Dialogs through Windows API Code Pack
            CFDInCodePackForm codePackForm = new CFDInCodePackForm();
            codePackForm.ShowDialog();
        }
    }
}
