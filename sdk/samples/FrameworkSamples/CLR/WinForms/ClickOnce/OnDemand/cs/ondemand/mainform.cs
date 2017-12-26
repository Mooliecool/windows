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
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Diagnostics;
using System.Deployment.Application;

namespace Microsoft.Samples.Windows.Forms.ClickOnce.OnDemand
{

	/// <summary>
	/// Summary description for form.
	/// </summary>
	public partial class MainForm : System.Windows.Forms.Form
	{
		private ApplicationDeployment _appDeployment;
		private bool _appDeployed;
		private const string _notDeployedErrorText = "The deployment APIs can not be used if the app is not currently deployed via ClickOnce.";
        private DownloadProgressForm _progressForm;

        /// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		public MainForm()
		{
			InitializeComponent();
        }

        //****************************************************************
        // INITIALIZATION SECTION
        //****************************************************************
        #region Inialization
        private void MainForm_Load(object sender, EventArgs e)
        {
            _appDeployed = ApplicationDeployment.IsNetworkDeployed;
            isDeployedBox.Text = _appDeployed.ToString();
            customerLocalBox.Text = IsLocal("customer").ToString();
            orderLocalBox.Text = IsLocal("order").ToString();

            if (_appDeployed)
            {
                _appDeployment = ApplicationDeployment.CurrentDeployment;

                //Register events for async work
                _appDeployment.DownloadFileGroupProgressChanged += new DeploymentProgressChangedEventHandler(OnDownloadFileGroupProgressChanged);
                _appDeployment.DownloadFileGroupCompleted += new DownloadFileGroupCompletedEventHandler(OnDownloadFileGroupCompleted);
            }
        }
        
        #endregion
        
        //****************************************************************
        // LOAD CUSTOMERS SECTION
        //****************************************************************
        #region LoadCustomers
        private void customerButton_Click(object sender, EventArgs e)
		{
            //If app is not deployed via clickonce assume all files are local... debug case
            if (!_appDeployed)
            {
                ShowCustomers();
                return;
            }

            // Check if files are already local
            if (IsLocal("customer"))
            {
                ShowCustomers();
            }
            else
            {
                if (downloadAsyncBox.Checked)
                    LoadGroupAsync("customer");
                else
                    LoadGroupSync("customer");    
            }
        }

        // Loads the customer control & displays it. 
        private void ShowCustomers()
        {
            CustomerControl cust = new CustomerControl();
			MainPanel.Controls.Clear();
			cust.Dock = DockStyle.Fill;
            MainPanel.Controls.Add(cust);
        }

        #endregion LoadCustomers

		//****************************************************************
		// LOAD ORDERS SECTION
		//****************************************************************
		#region LoadOrders
		private void orderButton_Click(object sender, EventArgs e)
		{
			//If app is not deployed via clickonce assume all files are local... debug case
			if (!_appDeployed)
			{
				ShowOrders();
				return;
			}

			// Check if files are already local
			if (IsLocal("order"))
			{
				ShowOrders();
				return;
			}

			if (downloadAsyncBox.Checked)
				LoadGroupAsync("order");
			else
				LoadGroupSync("order");
        }

        // Loads the order control & displays it.  
        private void ShowOrders()
		{
			OrderControl order = new OrderControl();
			order.Dock = DockStyle.Fill;
			MainPanel.Controls.Clear();
			MainPanel.Controls.Add(order);
        }

        #endregion

        //****************************************************************
        // GENERAL HELPER METHOD SECTION
        //****************************************************************
        #region GeneralHelper

        // Downloads the customer files syncronously & then displays it.
        private void LoadGroupSync(string groupName)
        {
			try
			{
				_appDeployment.DownloadFileGroup(groupName);

				switch (groupName)
				{
					case "customer":
						customerLocalBox.Text = "True";
						ShowCustomers();
						break;
					case "order":
						orderLocalBox.Text = "True";
						ShowOrders();
						break;
				}

			}
			catch (Exception e)
			{
				//Exception here is most likely due to networking problems
				MessageBox.Show("Failed to download application components:  " + e.Message);
			}
        }

        private void LoadGroupAsync(string groupName)
        {
            //Start download
            _appDeployment.DownloadFileGroupAsync(groupName, groupName);

            //Show Prgress Form
            _progressForm = new DownloadProgressForm();
            
            _progressForm.ShowDialog();
        }

        // Checks if group is already local.
        private bool IsLocal(string groupName)
        {
            try
            {
                if (_appDeployment.IsFileGroupDownloaded(groupName))
                    return true;
                else
                    return false;
            }
            catch (Exception)
            {
                return false;
            }
        }
        #endregion

        //****************************************************************
        // ASYNC DOWNLOAD EVENT HANDLERS
        //****************************************************************
        #region AsyncEventHandlers

        // DownloadFileGroupProgressChanged, so update the progress dialog
        private void OnDownloadFileGroupProgressChanged(object sender, DeploymentProgressChangedEventArgs e)
        {
            _progressForm.SetProgress(e.ProgressPercentage, e.BytesCompleted, e.BytesTotal, e.State.ToString());
        }

        // DownloadFileGroupCompleted, so update the progress dialog and the main form
        private void OnDownloadFileGroupCompleted(object sender, DownloadFileGroupCompletedEventArgs e)
        {
            if (e.Error == null)
            {
                _progressForm.SetProgressCompleted(true);
                
                switch ((string)e.UserState)
                {
                    case "customer":
                        customerLocalBox.Text = "True";
                        ShowCustomers();
                        break;
					case "order":
						orderLocalBox.Text = "True";
						ShowOrders();
						break;
				}
            }
            else
            {
                _progressForm.SetProgressCompleted(false);
            }
        }
        #endregion
    }
}
