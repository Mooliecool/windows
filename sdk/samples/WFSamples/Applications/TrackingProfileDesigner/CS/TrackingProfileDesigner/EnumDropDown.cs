//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
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
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Workflow.ComponentModel;
using Microsoft.Samples.Workflow.TrackingProfileDesigner.Properties;
using System.Drawing;

namespace Microsoft.Samples.Workflow.TrackingProfileDesigner
{
    /// <summary>
    /// Create a common drop down class that presents an enumeration
    /// </summary>
    public class EnumDropDown : ToolStripDropDown
    {
        ToolStripMenuItem toggleAll;        

        public EnumDropDown(Type enumType)
        {            
            toggleAll = new ToolStripMenuItem();
            toggleAll.Text = "Select All";
            toggleAll.Tag = true;
            toggleAll.Click += new EventHandler(toggleAll_Click);

            this.Items.Add(toggleAll);
            
            foreach (object value in Enum.GetValues(enumType))
            {
                ToolStripButton button = new ToolStripButton();                
                button.Name = button.Text = value.ToString();
                button.CheckOnClick = true;                                
                button.Image = Resources.eventIcon;

                this.Items.Add(button);
            }
        }

        /// <summary>
        /// Raised when 'Select All' or 'Select None' is displayed.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void toggleAll_Click(object sender, EventArgs e)
        {
            foreach (ToolStripItem item in this.Items)
            {
                ToolStripButton button = item as ToolStripButton;
                if (button!=null)
                {
                    if ((bool)toggleAll.Tag)
                    {
                        if (!button.Checked) { button.PerformClick(); }
                    }
                    else
                    {
                        if (button.Checked) { button.PerformClick(); }
                    }
                }
            }
            UpdateToggleAll();
        }

        /// <summary>
        /// Update the 'toggle all' button to be either 'select all' or 'select none'
        /// </summary>
        public void UpdateToggleAll()
        {
            bool allChecked = true;
            foreach (ToolStripItem item in this.Items)
            {
                if (item is ToolStripButton)
                {
                    if (!((ToolStripButton)item).Checked)
                    {
                        allChecked = false;
                    }
                }
            }
            if (allChecked)
            {
                toggleAll.Text = "Select None";
                toggleAll.Tag = false;
            }
            else
            {
                toggleAll.Text = "Select All";
                toggleAll.Tag = true;
            }
        }

        /// <summary>
        /// Clear all the checked items
        /// </summary>
        public void Reset()
        {
            UpdateToggleAll();
            foreach (ToolStripItem button in this.Items)
            {
                if (button is ToolStripButton)
                {
                    ((ToolStripButton)button).Checked = false;
                }
            }
        }
    }
}
