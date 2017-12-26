/******************************** Module Header ********************************\
Module Name:  MainForm.cs
Project:      CSSetPowerAvailabilityRequest
Copyright (c) Microsoft Corporation.



This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*******************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
#endregion


namespace CSSetPowerAvailabilityRequest
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }


        #region Power Request - Display Required

        private PowerRequest displayPowerRequest;

        private void btnSetDisplayRequired_Click(object sender, EventArgs e)
        {
            if (this.btnSetDisplayRequired.Text == "Set")
            {
                // Create a display availablity request to keep the display 
                // from automatically dimming or turning off.
                displayPowerRequest = new PowerRequest(tbReason.Text);

                displayPowerRequest.DisplayRequired = true;

                // Done.
                this.btnSetDisplayRequired.Text = "Clear";
            }
            else
            {
                // Clear the display power request.
                displayPowerRequest.DisplayRequired = false;

                // Finished using the power request object.
                displayPowerRequest.Dispose();

                // Done.
                this.btnSetDisplayRequired.Text = "Set";
            }
        }

        #endregion


        #region Power Request - System Required

        private PowerRequest systemPowerRequest;

        private void btnSetSystemRequired_Click(object sender, EventArgs e)
        {
            if (this.btnSetSystemRequired.Text == "Set")
            {
                // Create a system availablity request to keep the system 
                // from automatically sleeping.
                systemPowerRequest = new PowerRequest(tbReason.Text);

                systemPowerRequest.SystemRequired = true;

                // Done.
                this.btnSetSystemRequired.Text = "Clear";
            }
            else
            {
                // Clear the system power request.
                systemPowerRequest.SystemRequired = false;

                // Finished using the system request object.
                systemPowerRequest.Dispose();

                // Done.
                this.btnSetSystemRequired.Text = "Set";
            }
        }

        #endregion


        #region Power Request - Away Mode Required

        private PowerRequest awayModePowerRequest;

        private void btnSetAwayModeRequired_Click(object sender, EventArgs e)
        {
            if (this.btnSetAwayModeRequired.Text == "Set")
            {
                // Enable Away Mode.
                awayModePowerRequest = new PowerRequest(tbReason.Text);

                awayModePowerRequest.AwayModeRequired = true;

                // Done.
                this.btnSetAwayModeRequired.Text = "Clear";
            }
            else
            {
                // Disable Away Mode.
                awayModePowerRequest.AwayModeRequired = false;

                // Finished using the away mode request object.
                awayModePowerRequest.Dispose();

                // Done.
                this.btnSetAwayModeRequired.Text = "Set";
            }
        }

        #endregion
    }
}