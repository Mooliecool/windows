/************************************* Module Header **************************************\
* Module Name:	MainForm.cs
* Project:		CSWinFormGroupRadioButtons
* Copyright (c) Microsoft Corporation.
* 
* This sample demonstrates how to group the RadioButtons in the different containers.
* 
* For more information about the RadioButton control, see:
* 
*  Windows Forms RadioButton control
*  http://msdn.microsoft.com/en-us/library/f5h102xz.aspx
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\******************************************************************************************/

using System;
using System.Windows.Forms;

namespace CSWinFormGroupRadioButtons
{
    public partial class MainForm : Form
    {
        // store the old RadioButton
        private RadioButton radTmp = null;

        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            // we checked the rad1 in the MainForm.Designer.cs file
            // then this RadioButton would be the old one
            radTmp = this.rad1;
        }

        // let the 4 Radiobuttons use this method to deal with their CheckedChanged event
        // in the MainForm.Designer.cs file
        private void radioButton_CheckedChanged(object sender, EventArgs e)
        {
            // uncheck the old one
            radTmp.Checked = false;
            radTmp = (RadioButton)sender;
            
            // find out the checked one
            if (radTmp.Checked)
            {
                this.lb.Text = radTmp.Name + " has been selected";
            }
        }
    }
}
