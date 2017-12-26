/****************************** Module Header ******************************\
* Module Name:  MainPage.cs
* Project:      CSSL3OOB
* Copyright (c) Microsoft Corporation.
* 
* This example demonstrates how to work with OOB using C#.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 10/15/2009 16:21 Yilun Luo Created
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Net.NetworkInformation;

namespace CSSL3OOB
{
	public partial class MainPage : UserControl
	{
		public MainPage()
		{
			InitializeComponent();
			Application.Current.CheckAndDownloadUpdateCompleted += new CheckAndDownloadUpdateCompletedEventHandler(Current_CheckAndDownloadUpdateCompleted);
			NetworkChange.NetworkAddressChanged += new NetworkAddressChangedEventHandler(NetworkChange_NetworkAddressChanged);
		}

		/// <summary>
		/// Detect the network connection changing.
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		void NetworkChange_NetworkAddressChanged(object sender, EventArgs e)
		{
			if (!NetworkInterface.GetIsNetworkAvailable())
			{
				this.informationTextBlock.Text = "Your network connection has been lost!";
			}
			else
			{
				this.informationTextBlock.Text = "Your network connection has been restored!";
			}
		}

		/// <summary>
		/// Install OOB with code.
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void InstallButton_Click(object sender, RoutedEventArgs e)
		{
			if (Application.Current.InstallState == InstallState.NotInstalled)
			{
				Application.Current.Install();
			}
			else if (Application.Current.InstallState == InstallState.Installed)
			{
				MessageBox.Show("Application already installed. You cannot remove the OOB via code!");
			}
		}

		/// <summary>
		/// Check if an update is available.
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void CheckUpdateButton_Click(object sender, System.Windows.RoutedEventArgs e)
		{
			Application.Current.CheckAndDownloadUpdateAsync();
		}

		void Current_CheckAndDownloadUpdateCompleted(object sender, CheckAndDownloadUpdateCompletedEventArgs e)
		{
			if (e.UpdateAvailable)
			{
				MessageBox.Show("You have to upgrade this application to the latest version in order to use it. Restart this application to upgrade automatically!");
			}
		}
	}
}
