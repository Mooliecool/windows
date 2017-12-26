/****************************** Module Header ******************************\
* Module Name:	MainPage.xaml.cs
* Project:		CSAzureServiceBusSLRest
* Copyright (c) Microsoft Corporation.
* 
* This is the Silverlight client.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.IO;
using System.Net;
using System.Net.Browser;
using System.Windows;
using System.Windows.Controls;

namespace Client
{
	public partial class MainPage : UserControl
	{
		// Change to your namespace.
		private const string ServiceNamespace = "[namespace]";

		public MainPage()
		{
			InitializeComponent();
		}

		private void UserControl_Loaded(object sender, RoutedEventArgs e)
		{
			// Switch to client HTTP stack to take advantage of features such as status code.
			HttpWebRequest.RegisterPrefix("http://", WebRequestCreator.ClientHttp);
			HttpWebRequest.RegisterPrefix("https://", WebRequestCreator.ClientHttp);
		}

		private void UploadButton_Click(object sender, RoutedEventArgs e)
		{
			OpenFileDialog ofd = new OpenFileDialog();
			if (ofd.ShowDialog().Value)
			{
				this.FileNameTextBox.Text = ofd.File.Name;
				FileStream fs = ofd.File.OpenRead();

				// To make the sample work for large files, please configure the webHttpRelayBinding's max***
				// properties. To make the sample code easier to understand, we use the default settings.
				// WCF configurations will be demonstrated in a separate WCF sample.
				if (fs.Length >= 65535)
				{
					MessageBox.Show("The file is too large to be sent using the default WCF settings.");
					return;
				}
				this.infoTextBlock.Text = "Uploading the file. Please wait...";
				this.infoTextBlock.Visibility = Visibility.Visible;

				// Invoke the WCF REST Service with HTTP POST.
				HttpWebRequest request = (HttpWebRequest)HttpWebRequest.Create("https://" + ServiceNamespace + ".servicebus.windows.net/file/" + ofd.File.Name);
				request.Method = "POST";
				request.ContentType = "application/octet-stream";
				request.BeginGetRequestStream(result1 =>
				{
					Stream requestStream = request.EndGetRequestStream(result1);
					byte[] buffer = new byte[fs.Length];
					fs.Read(buffer, 0, buffer.Length);
					requestStream.Write(buffer, 0, buffer.Length);
					requestStream.Close();
					request.BeginGetResponse(result2 =>
					{
						HttpWebResponse response = (HttpWebResponse)request.EndGetResponse(result2);
						using (Stream responseStream = response.GetResponseStream())
						{
							if (response.StatusCode == HttpStatusCode.Created)
							{
								this.Dispatcher.BeginInvoke(() =>
								{
									this.infoTextBlock.Visibility = Visibility.Collapsed;
									MessageBox.Show("Upload succeeded.");
								});
							}
						}
					}, null);
				}, null);
			}
		}

		private void DownloadButton_Click(object sender, RoutedEventArgs e)
		{
			if (string.IsNullOrEmpty(this.FileNameTextBox.Text))
			{
				MessageBox.Show("Please type a file name.");
				return;
			}
			SaveFileDialog sfd = new SaveFileDialog();
			if (sfd.ShowDialog().Value)
			{
				this.infoTextBlock.Text = "Downloading the file. Please wait...";
				this.infoTextBlock.Visibility = Visibility.Visible;
				Stream stream = sfd.OpenFile();
				// Invoke the WCF REST Service with HTTP GET.
				WebClient webClient = new WebClient();
				webClient.OpenReadCompleted += new OpenReadCompletedEventHandler(webClient_OpenReadCompleted);
				webClient.OpenReadAsync(new Uri("https://" + ServiceNamespace + ".servicebus.windows.net/file/" + this.FileNameTextBox.Text), stream);
			}
		}

		void webClient_OpenReadCompleted(object sender, OpenReadCompletedEventArgs e)
		{
			try
			{
				Stream fileStream = (Stream)e.UserState;
				byte[] buffer = new byte[e.Result.Length];
				e.Result.Read(buffer, 0, buffer.Length);
				fileStream.Write(buffer, 0, buffer.Length);
				fileStream.Close();
				this.infoTextBlock.Visibility = Visibility.Collapsed;
				MessageBox.Show("File downloaded. Please check your saved file.");
			}
			catch
			{
				this.infoTextBlock.Visibility = Visibility.Collapsed;
				MessageBox.Show("File download failed. Please check if the file exits on the server.");
			}
		}
	}
}
