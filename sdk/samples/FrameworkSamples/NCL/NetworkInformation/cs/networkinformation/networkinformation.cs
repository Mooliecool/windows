//---------------------------------------------------------------------
//	This file is part of the Microsoft .NET	Framework SDK Code Samples.
// 
//	Copyright (C) Microsoft	Corporation.  All rights reserved.
// 
//This source code is intended only	as a supplement	to Microsoft
//Development Tools	and/or on-line documentation.  See these other
//materials	for	detailed information regarding Microsoft code samples.
// 
//THIS CODE	AND	INFORMATION	ARE	PROVIDED AS	IS WITHOUT WARRANTY	OF ANY
//KIND,	EITHER EXPRESSED OR	IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF	MERCHANTABILITY	AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.Net;
using System.Drawing;
using System.Collections;
using System.Globalization;
using System.Windows.Forms;
using System.ComponentModel;
using System.Net.NetworkInformation;

namespace Microsoft.Samples.NetworkInformation
{
    ///	<summary>
    ///	This form provides information pertaining to the network elements
    ///	on the current machine such	as network interfaces, IP addresses,
    ///	and	transmission statistics
    ///	</summary>
    public partial class NetworkInformation : System.Windows.Forms.Form
    {
        ///	<summary>
        ///	Required designer variable.
        ///	</summary>
        private System.ComponentModel.IContainer components = null;
        private NetworkInterface[] networkInterfaces = null;
        private NetworkInterface currentInterface = null;

        public NetworkInformation()
        {
            InitializeComponent();
        }

        private void NetworkInformation_Load(object sender, System.EventArgs e)
        {
            // Wire	up the NetworkAddressChanged events so we can get notified
            // when	an address change occurs on any of the network interfaces.
            // These changes occur when the interface changes operational
            // status (up/down) or a new interface is added. 
            NetworkChange.NetworkAddressChanged +=
                new NetworkAddressChangedEventHandler(
                NetworkChange_NetworkAddressChanged);
            NetworkChange.NetworkAvailabilityChanged +=
                new NetworkAvailabilityChangedEventHandler(
                NetworkChange_NetworkAvailabilityChanged);

            // Populate	the	global interfaces container	with the list of all
            // network interfaces.
            networkInterfaces = NetworkInterface.GetAllNetworkInterfaces();

            // Determine if	the	network	is available at	startup.
            UpdateNetworkAvailability(NetworkInterface.GetIsNetworkAvailable());

            // Update the information for the network interfaces.
            UpdateNetworkInformation();
        }

        // This	timer call updates information associated with the currently
        // selected	interface that is often	changing such as operational 
        // status, transfer	statistics,	and	speed.
        private void tmrUpdateInfo_Tick(object sender, System.EventArgs e)
        {
            UpdateNicStats();
        }

        private void UpdateNicStats()
        {
            // Get the IPv4	statistics for the currently selected interface.
            IPv4InterfaceStatistics ipStats =
                currentInterface.GetIPv4Statistics();

            NumberFormatInfo numberFormat = NumberFormatInfo.CurrentInfo;

            long bytesReceivedInKB = ipStats.BytesReceived / 1024;
            long bytesSentInKB = ipStats.BytesSent / 1024;

            this.speedTextLabel.Text = GetSpeedString(currentInterface.Speed);
            this.bytesReceivedTextLabel.Text = 
                bytesReceivedInKB.ToString("N0", numberFormat) + " KB";
            this.bytesSentTextLabel.Text =
                bytesSentInKB.ToString("N0", numberFormat) + " KB";

            this.operationalStatusTextLabel.Text =
                currentInterface.OperationalStatus.ToString();
            this.supportsMulticastTextLabel.Text =
                currentInterface.SupportsMulticast.ToString();
        }

        // Provide better formatting for some common speeds.
        static private string GetSpeedString(long speed)
        {
            switch (speed)
            {
                case 10000000:
                    return "10 MB";
                case 11000000:
                    return "11 MB";
                case 54000000:
                    return "54 MB";
                case 100000000:
                    return "100 MB";
                case 1000000000:
                    return "1 GB";
                default:
                    return speed.ToString(NumberFormatInfo.CurrentInfo);
            }
        }

        // Display the interface information that doesn't update so	often that
        // it should be	on the timer that fires	every 100 ms.  
        private void UpdateCurrentNicInformation()
        {
            // Set the DNS suffix if any exists
            IPInterfaceProperties ipProperties =
                currentInterface.GetIPProperties();
            this.dnsSuffixTextLabel.Text =
                ipProperties.DnsSuffix.ToString();

            // Display the IP address information associated with this
            // interface including anycast,	unicast, multicast,	DNS	servers,
            // WINS	servers, DHCP servers, and the gateway.
            this.addressListView.Items.Clear();
            IPAddressInformationCollection anycastInfo =
                ipProperties.AnycastAddresses;
            foreach (IPAddressInformation info in anycastInfo)
                InsertAddress(info.Address, "Anycast");
            UnicastIPAddressInformationCollection unicastInfo =
                ipProperties.UnicastAddresses;
            foreach (UnicastIPAddressInformation info in unicastInfo)
                InsertAddress(info.Address, "Unicast");
            MulticastIPAddressInformationCollection multicastInfo =
                ipProperties.MulticastAddresses;
            foreach (MulticastIPAddressInformation info in multicastInfo)
                InsertAddress(info.Address, "Multicast");
            GatewayIPAddressInformationCollection gatewayInfo =
                ipProperties.GatewayAddresses;
            foreach (GatewayIPAddressInformation info in gatewayInfo)
                InsertAddress(info.Address, "Gateway");

            IPAddressCollection ipAddresses = 
                ipProperties.WinsServersAddresses;
            InsertAddresses(ipAddresses, "WINS Server");
            ipAddresses = ipProperties.DhcpServerAddresses;
            InsertAddresses(ipAddresses, "DHCP Server");
            ipAddresses = ipProperties.DnsAddresses;
            InsertAddresses(ipAddresses, "DNS Server");
        }

        // Used	for	inserting the IPAddress	information	into the listbox.
        private void InsertAddresses(
            IPAddressCollection ipAddresses, string addressType)
        {
            foreach (IPAddress ipAddress in ipAddresses)
                InsertAddress(ipAddress, addressType);
        }

        private void InsertAddress(IPAddress ipAddress, string addressType)
        {
            string[] listViewInformation = new string[2];
            listViewInformation[0] = ipAddress.ToString();
            listViewInformation[1] = addressType;

            ListViewItem item = new ListViewItem(listViewInformation);
            addressListView.Items.Add(item);
        }

        // Called on startup and whenever there	is a network change	event, this
        // method reloads the list of interfaces and sets the selection	to the
        // first one in	the	list.  The list	is reloaded	because	this event 
        // could have been fired as	a result of	an interface being removed from
        // the machine.
        private void UpdateNetworkInformation()
        {
            networkInterfaces = NetworkInterface.GetAllNetworkInterfaces();
            this.networkInterfacesComboBox.Items.Clear();
            foreach (NetworkInterface networkInterface in networkInterfaces)
            {
                networkInterfacesComboBox.Items.Add(
                    networkInterface.Description);
            }

            if (networkInterfaces.Length == 0)
            {
                this.networkInterfacesComboBox.Items.Add(
                    "No	NICs found on the machine");
            }
            else
            {
                currentInterface = networkInterfaces[0];
                UpdateCurrentNicInformation();
            }

            this.networkInterfacesComboBox.SelectedIndex = 0;

        }

        private void UpdateNetworkAvailability(bool isNetworkAvailable)
        {
            if (isNetworkAvailable)
            {
                this.networkAvailabilityTextLabel.Text =
                    "At least one network interface is up.";
            }
            else
            {
                this.networkAvailabilityTextLabel.Text =
                    "The network is not currently available.";
            }
        }

        private delegate void NetworkAddressChangedCallback();
        private delegate void NetworkAvailabilityCallback(
            bool isNetworkAvailable);

        // Update network interface	information	whenever an	address	changes.
        private void NetworkChange_NetworkAddressChanged(
            object sender, EventArgs e)
        {
            this.Invoke(
                new NetworkAddressChangedCallback(UpdateNetworkInformation));
        }

        // Update network availability.
        private void NetworkChange_NetworkAvailabilityChanged(
            object sender, NetworkAvailabilityEventArgs e)
        {
            this.Invoke(
                new NetworkAvailabilityCallback(UpdateNetworkAvailability),
                new object[1] { e.IsAvailable });
        }

        // Update current interface	information	whenever selection changes.
        private void OnSelectionChanged(object sender, System.EventArgs e)
        {
            currentInterface =
                networkInterfaces[networkInterfacesComboBox.SelectedIndex];
            UpdateCurrentNicInformation();
        }
    }
}

