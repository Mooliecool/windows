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
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Workflow.Runtime.Tracking;
using System.Data.Common;
using System.Data.SqlClient;

namespace Microsoft.Samples.Workflow.TrackingProfileDesigner.TrackingStore
{
    public partial class UpdateProfileVersion : Form
    {
        private Type workflowTypeValue;
        private TrackingProfile currentTrackingProfileValue;
        private TrackingProfile newTrackingProfileValue;

        public UpdateProfileVersion()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Gets or sets the workflow type to be saved.
        /// </summary>
        public Type WorkflowType
        {
            get { return workflowTypeValue; }
            set { workflowTypeValue = value; }
        }
        
        /// <summary>
        /// Gets or sets the existing tracking profile
        /// </summary>
        public TrackingProfile CurrentTrackingProfile
        {
            get { return currentTrackingProfileValue; }
            set { currentTrackingProfileValue = value; }
        }

        /// <summary>
        /// Gets or sets the new tracking profile
        /// </summary>
        public TrackingProfile NewTrackingProfile
        {
            get { return newTrackingProfileValue; }
            set { newTrackingProfileValue = value; }
        }

        private void SaveTrackingProfile_Load(object sender, EventArgs e)
        {
            this.existingProfileLabel.Text += currentTrackingProfileValue.Version.ToString();
        }

        private void save_Click(object sender, EventArgs e)
        {
            try
            {                
                this.NewTrackingProfile.Version = new Version(this.profileVersion.Text);
                if (this.NewTrackingProfile.Version <= this.CurrentTrackingProfile.Version)
                {
                    MessageBox.Show(string.Format("Please enter a version greater than the current version ({0})", this.CurrentTrackingProfile.Version.ToString()), "Incorrect Version");
                }
                this.DialogResult = DialogResult.OK;
                this.Close();
            }
            catch
            {
                MessageBox.Show("Please enter a valid version string (i.e. '1.0.0')","Incorrect Version");
            }
        }

        private void cancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }
    

    }
}