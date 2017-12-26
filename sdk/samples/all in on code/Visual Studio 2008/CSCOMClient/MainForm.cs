/****************************** Module Header ******************************\
* Module Name:  MainForm.cs
* Project:      CSCOMClient
* Copyright (c) Microsoft Corporation.
* 
* Windows Form hosts ActiveX control.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 2/28/2009 2:52 PM Jialiang Ge Created
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


namespace CSCOMClient
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void bnSetFloatProperty_Click(object sender, EventArgs e)
        {
            float fProp;

            // Verify the value in the FloatProperty TextBox control
            if (float.TryParse(this.tbFloatProperty.Text, out fProp))
            {
                // Set FloatProperty to the ActiveX control
                this.axMFCActiveX1.FloatProperty = fProp;
            }
            else
            {
                MessageBox.Show("Invalid FloatProperty value.");
            }
        }

        private void bnGetFloatProperty_Click(object sender, EventArgs e)
        {
            string message = String.Format("FloatProperty equals {0}",
                this.axMFCActiveX1.FloatProperty);
            MessageBox.Show(message, "MFCActiveX!FloatProperty");
        }

        private void axMFCActiveX1_FloatPropertyChanging(object sender,
            AxMFCActiveXLib._DMFCActiveXEvents_FloatPropertyChangingEvent e)
        {
            string message = String.Format(
                "FloatProperty is being changed to {0}", e.newValue);

            // OK or cancel the change of FloatProperty
            e.cancel = (DialogResult.Cancel ==
                MessageBox.Show(message, "MFCActiveX!FloatPropertyChanging",
                MessageBoxButtons.OKCancel));
        }
    }
}