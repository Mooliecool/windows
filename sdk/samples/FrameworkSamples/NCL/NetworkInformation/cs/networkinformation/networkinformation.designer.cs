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

namespace Microsoft.Samples.NetworkInformation
{
	public partial class NetworkInformation : System.Windows.Forms.Form
	{
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this.networkInterfacesComboBox = new System.Windows.Forms.ComboBox();
			this.interfacesLabel = new System.Windows.Forms.Label();
			this.updateInfoTimer = new System.Windows.Forms.Timer(this.components);
			this.addressListView = new System.Windows.Forms.ListView();
			this.clmnAddress = new System.Windows.Forms.ColumnHeader("");
			this.clmnAddressType = new System.Windows.Forms.ColumnHeader("");
			this.networkAvailabilityLabel = new System.Windows.Forms.Label();
			this.networkAvailabilityTextLabel = new System.Windows.Forms.Label();
			this.operationalStatusLabel = new System.Windows.Forms.Label();
			this.operationalStatusTextLabel = new System.Windows.Forms.Label();
			this.supportsMulticastLabel = new System.Windows.Forms.Label();
			this.supportsMulticastTextLabel = new System.Windows.Forms.Label();
			this.bytesSentLabel = new System.Windows.Forms.Label();
			this.bytesSentTextLabel = new System.Windows.Forms.Label();
			this.bytesReceivedLabel = new System.Windows.Forms.Label();
			this.bytesReceivedTextLabel = new System.Windows.Forms.Label();
			this.speedLabel = new System.Windows.Forms.Label();
			this.speedTextLabel = new System.Windows.Forms.Label();
			this.dnsSuffixLabel = new System.Windows.Forms.Label();
			this.dnsSuffixTextLabel = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// networkInterfacesComboBox
			// 
			this.networkInterfacesComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.networkInterfacesComboBox.FormattingEnabled = true;
			this.networkInterfacesComboBox.Location = new System.Drawing.Point(85, 36);
			this.networkInterfacesComboBox.Margin = new System.Windows.Forms.Padding(1, 3, 3, 3);
			this.networkInterfacesComboBox.Name = "networkInterfacesComboBox";
			this.networkInterfacesComboBox.Size = new System.Drawing.Size(431, 21);
			this.networkInterfacesComboBox.TabIndex = 0;
			this.networkInterfacesComboBox.SelectedIndexChanged += new System.EventHandler(this.OnSelectionChanged);
			// 
			// interfacesLabel
			// 
			this.interfacesLabel.Location = new System.Drawing.Point(11, 37);
			this.interfacesLabel.Margin = new System.Windows.Forms.Padding(3, 3, 0, 3);
			this.interfacesLabel.Name = "interfacesLabel";
			this.interfacesLabel.Size = new System.Drawing.Size(72, 20);
			this.interfacesLabel.TabIndex = 1;
			this.interfacesLabel.Text = "Interfaces:";
			// 
			// updateInfoTimer
			// 
			this.updateInfoTimer.Enabled = true;
			this.updateInfoTimer.Tick += new System.EventHandler(this.tmrUpdateInfo_Tick);
			// 
			// addressListView
			// 
			this.addressListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.clmnAddress,
            this.clmnAddressType});
			this.addressListView.FullRowSelect = true;
			this.addressListView.GridLines = true;
			this.addressListView.BackgroundImageTiled = false;
			this.addressListView.Location = new System.Drawing.Point(24, 147);
			this.addressListView.Name = "addressListView";
			this.addressListView.Size = new System.Drawing.Size(492, 272);
			this.addressListView.TabIndex = 2;
			this.addressListView.View = System.Windows.Forms.View.Details;
			// 
			// clmnAddress
			// 
			this.clmnAddress.Text = "Address";
			this.clmnAddress.Width = 236;
			// 
			// clmnAddressType
			// 
			this.clmnAddressType.Text = "Type";
			this.clmnAddressType.Width = 248;
			// 
			// networkAvailabilityLabel
			// 
			this.networkAvailabilityLabel.Location = new System.Drawing.Point(11, 12);
			this.networkAvailabilityLabel.Margin = new System.Windows.Forms.Padding(3, 3, 3, 1);
			this.networkAvailabilityLabel.Name = "networkAvailabilityLabel";
			this.networkAvailabilityLabel.Size = new System.Drawing.Size(106, 19);
			this.networkAvailabilityLabel.TabIndex = 3;
			this.networkAvailabilityLabel.Text = "Network Availability:";
			// 
			// networkAvailabilityTextLabel
			// 
			this.networkAvailabilityTextLabel.Location = new System.Drawing.Point(122, 12);
			this.networkAvailabilityTextLabel.Name = "networkAvailabilityTextLabel";
			this.networkAvailabilityTextLabel.Size = new System.Drawing.Size(380, 20);
			this.networkAvailabilityTextLabel.TabIndex = 4;
			this.networkAvailabilityTextLabel.Text = "At least one network interface is up";
			// 
			// operationalStatusLabel
			// 
			this.operationalStatusLabel.Location = new System.Drawing.Point(286, 88);
			this.operationalStatusLabel.Margin = new System.Windows.Forms.Padding(3, 0, 3, 3);
			this.operationalStatusLabel.Name = "operationalStatusLabel";
			this.operationalStatusLabel.Size = new System.Drawing.Size(104, 15);
			this.operationalStatusLabel.TabIndex = 5;
			this.operationalStatusLabel.Text = "Operational Status:";
			// 
			// operationalStatusTextLabel
			// 
			this.operationalStatusTextLabel.Location = new System.Drawing.Point(418, 86);
			this.operationalStatusTextLabel.Margin = new System.Windows.Forms.Padding(3, 2, 3, 3);
			this.operationalStatusTextLabel.Name = "operationalStatusTextLabel";
			this.operationalStatusTextLabel.Size = new System.Drawing.Size(113, 16);
			this.operationalStatusTextLabel.TabIndex = 6;
			this.operationalStatusTextLabel.Text = "Up";
			// 
			// supportsMulticastLabel
			// 
			this.supportsMulticastLabel.Location = new System.Drawing.Point(286, 63);
			this.supportsMulticastLabel.Name = "supportsMulticastLabel";
			this.supportsMulticastLabel.Size = new System.Drawing.Size(104, 21);
			this.supportsMulticastLabel.TabIndex = 7;
			this.supportsMulticastLabel.Text = "Supports Multicast:";
			// 
			// supportsMulticastTextLabel
			// 
			this.supportsMulticastTextLabel.Location = new System.Drawing.Point(418, 63);
			this.supportsMulticastTextLabel.Margin = new System.Windows.Forms.Padding(3, 3, 3, 1);
			this.supportsMulticastTextLabel.Name = "supportsMulticastTextLabel";
			this.supportsMulticastTextLabel.Size = new System.Drawing.Size(73, 20);
			this.supportsMulticastTextLabel.TabIndex = 8;
			this.supportsMulticastTextLabel.Text = "Yes";
			// 
			// bytesSentLabel
			// 
			this.bytesSentLabel.Location = new System.Drawing.Point(11, 90);
			this.bytesSentLabel.Name = "bytesSentLabel";
			this.bytesSentLabel.Size = new System.Drawing.Size(89, 18);
			this.bytesSentLabel.TabIndex = 9;
			this.bytesSentLabel.Text = "Bytes Sent:";
			// 
			// bytesSentTextLabel
			// 
			this.bytesSentTextLabel.Location = new System.Drawing.Point(122, 91);
			this.bytesSentTextLabel.Name = "bytesSentTextLabel";
			this.bytesSentTextLabel.Size = new System.Drawing.Size(144, 17);
			this.bytesSentTextLabel.TabIndex = 10;
			this.bytesSentTextLabel.Text = "0";
			// 
			// bytesReceivedLabel
			// 
			this.bytesReceivedLabel.Location = new System.Drawing.Point(11, 115);
			this.bytesReceivedLabel.Name = "bytesReceivedLabel";
			this.bytesReceivedLabel.Size = new System.Drawing.Size(88, 17);
			this.bytesReceivedLabel.TabIndex = 11;
			this.bytesReceivedLabel.Text = "Bytes Received:";
			// 
			// bytesReceivedTextLabel
			// 
			this.bytesReceivedTextLabel.Location = new System.Drawing.Point(122, 116);
			this.bytesReceivedTextLabel.Name = "bytesReceivedTextLabel";
			this.bytesReceivedTextLabel.Size = new System.Drawing.Size(158, 16);
			this.bytesReceivedTextLabel.TabIndex = 12;
			this.bytesReceivedTextLabel.Text = "0";
			// 
			// speedLabel
			// 
			this.speedLabel.Location = new System.Drawing.Point(286, 113);
			this.speedLabel.Name = "speedLabel";
			this.speedLabel.Size = new System.Drawing.Size(88, 18);
			this.speedLabel.TabIndex = 13;
			this.speedLabel.Text = "Speed:";
			// 
			// speedTextLabel
			// 
			this.speedTextLabel.Location = new System.Drawing.Point(418, 114);
			this.speedTextLabel.Margin = new System.Windows.Forms.Padding(3, 1, 3, 3);
			this.speedTextLabel.Name = "speedTextLabel";
			this.speedTextLabel.Size = new System.Drawing.Size(119, 22);
			this.speedTextLabel.TabIndex = 14;
			this.speedTextLabel.Text = "100";
			// 
			// dnsSuffixLabel
			// 
			this.dnsSuffixLabel.Location = new System.Drawing.Point(11, 63);
			this.dnsSuffixLabel.Name = "dnsSuffixLabel";
			this.dnsSuffixLabel.Size = new System.Drawing.Size(68, 18);
			this.dnsSuffixLabel.TabIndex = 15;
			this.dnsSuffixLabel.Text = "DNS Suffix:";
			// 
			// dnsSuffixTextLabel
			// 
			this.dnsSuffixTextLabel.Location = new System.Drawing.Point(121, 63);
			this.dnsSuffixTextLabel.Margin = new System.Windows.Forms.Padding(3, 3, 3, 1);
			this.dnsSuffixTextLabel.Name = "dnsSuffixTextLabel";
			this.dnsSuffixTextLabel.Size = new System.Drawing.Size(159, 23);
			this.dnsSuffixTextLabel.TabIndex = 16;
			this.dnsSuffixTextLabel.Text = "n/a";
			// 
			// NetworkInformation
			// 
            this.AutoSize = true;
			this.ClientSize = new System.Drawing.Size(558, 440);
			this.Controls.Add(this.dnsSuffixTextLabel);
			this.Controls.Add(this.dnsSuffixLabel);
			this.Controls.Add(this.speedTextLabel);
			this.Controls.Add(this.speedLabel);
			this.Controls.Add(this.bytesReceivedTextLabel);
			this.Controls.Add(this.bytesReceivedLabel);
			this.Controls.Add(this.bytesSentTextLabel);
			this.Controls.Add(this.bytesSentLabel);
			this.Controls.Add(this.supportsMulticastTextLabel);
			this.Controls.Add(this.supportsMulticastLabel);
			this.Controls.Add(this.operationalStatusTextLabel);
			this.Controls.Add(this.operationalStatusLabel);
			this.Controls.Add(this.networkAvailabilityTextLabel);
			this.Controls.Add(this.networkAvailabilityLabel);
			this.Controls.Add(this.addressListView);
			this.Controls.Add(this.interfacesLabel);
			this.Controls.Add(this.networkInterfacesComboBox);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
			this.MaximizeBox = false;
			this.Name = "NetworkInformation";
			this.Padding = new System.Windows.Forms.Padding(9);
			this.Text = "Network Information";
			this.Load += new System.EventHandler(this.NetworkInformation_Load);
			this.ResumeLayout(false);

		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				if (components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose(disposing);
		}

		
		private System.Windows.Forms.ComboBox networkInterfacesComboBox;
		private System.Windows.Forms.Label interfacesLabel;
		private System.Windows.Forms.Timer updateInfoTimer;
		private System.Windows.Forms.ColumnHeader clmnAddress;
		private System.Windows.Forms.ColumnHeader clmnAddressType;
		private System.Windows.Forms.ListView addressListView;
		private System.Windows.Forms.Label networkAvailabilityLabel;
		private System.Windows.Forms.Label networkAvailabilityTextLabel;
		private System.Windows.Forms.Label operationalStatusLabel;
		private System.Windows.Forms.Label operationalStatusTextLabel;
		private System.Windows.Forms.Label supportsMulticastLabel;
		private System.Windows.Forms.Label supportsMulticastTextLabel;
		private System.Windows.Forms.Label bytesSentTextLabel;
		private System.Windows.Forms.Label bytesSentLabel;
		private System.Windows.Forms.Label bytesReceivedLabel;
		private System.Windows.Forms.Label bytesReceivedTextLabel;
		private System.Windows.Forms.Label speedTextLabel;
		private System.Windows.Forms.Label speedLabel;
		private System.Windows.Forms.Label dnsSuffixLabel;		
		private System.Windows.Forms.Label dnsSuffixTextLabel;

	}
}

