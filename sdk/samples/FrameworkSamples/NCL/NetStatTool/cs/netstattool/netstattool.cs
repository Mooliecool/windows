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
using System.Net;
using System.Net.Sockets;
using System.Net.NetworkInformation;
using System.Globalization;

namespace Microsoft.Samples.NetStatTool
{
    [Flags]
    public enum ProtocolTypes
    {
		None = 0x0000,
        Tcp = 0x0001,
        Udp = 0x0002,
        TcpV6 = 0x0004,
        UdpV6 = 0x0008,
        IP = 0x0010,
        IPv6 = 0x0020,
        Icmp = 0x0040,
        IcmpV6 = 0x0080
    }

    static class NetStatTool
    {
        private static bool showNumeric;
        private static bool showListening;
        private static bool showProtocolStatistics;
        private static bool showInterfaceStatistics;
        private static int interval = -1;
        private static ProtocolTypes selectedProtocols = ProtocolTypes.None;
        private const string connectionFormatString =
            "\t{0,-12}{1, -16}{2, -28}{3}";
        private const string equalsFormatString =
            "\t{0, -32} = {1}";
        private const string sendReceiveFormatString =
            "\t{0, -32}{1, -16}{2}";
        private const string errorFormatString =
            "\tStatistics not available for {0}: {1}";

        static void Main(string[] args)
        {
            if (ParseArguments(args))
            {
                while (true)
                {
                    if (showInterfaceStatistics)
                        DisplayInterfaceStatistics();
                    if (showProtocolStatistics)
                        DisplayProtocolStatistics();
                    if (!showInterfaceStatistics && !showProtocolStatistics)
                        DisplayConnections();
                    if (interval <= 0)
                        break;
                    else
                        System.Threading.Thread.Sleep(interval * 1000);
                }
            }
            else
            {
                Usage();
                System.Environment.Exit(0);
            }
        }

        // Helper function display the all possible command line options and 
        // their purpose for this tool.
        private static void Usage()
        {
            Console.WriteLine();
            Console.WriteLine("NETSTATTOOL [-?] [-a] [-e] [-n] [-s] [-p protocol] [interval]");
            Console.WriteLine();
            Console.WriteLine("(default)     Displays open connection information.");
            Console.WriteLine("-?            Displays this help message.");
            Console.WriteLine("-e            Displays NIC statistics. May be combined with -s.");
            Console.WriteLine("-s            Displays per-protocol statistics.  By default, statistics are");
            Console.WriteLine("                  shown for TCP.  The -p and -a options may be used to specify");
            Console.WriteLine("                  other protocols to display.");
            Console.WriteLine("-p protocol   Display information for the protocol specified.");
            Console.WriteLine("                  May be any of: IP, IPv6, ICMP, ICMPv6, TCP, TCPv6, UDP,");
            Console.WriteLine("                  or UDPv6TCP.  Connections can only be shown for TCP,");
            Console.WriteLine("                  UDP, TCPv6, or UDPv6.");
            Console.WriteLine("-a            Display information for all protocals.");
            Console.WriteLine("-n            Displays addresses and port numbers in numerical form.");
            Console.WriteLine("interval      Redisplays selected statistics, pausing interval seconds");
            Console.WriteLine("                  between each display.  Press CTRL+C to stop.");
        }

        // Parse the command line arguments and set appropriate properties as 
        // per requested user options.
        private static bool ParseArguments(string[] args)
        {
            for (int i = 0; i < args.Length; i++)
            {
                string arg = args[i].ToLower(CultureInfo.CurrentCulture);
                if (arg.Length < 2 || (arg[0] != '/' && arg[0] != '-'))
                {
                    try
                    {
                        interval =
                            Convert.ToInt32(arg, NumberFormatInfo.CurrentInfo);
                    }
                    catch (FormatException)
                    {
                        return false;
                    }
                    catch (OverflowException)
                    {
                        return false;
                    }
                }
                else
                {
                    switch (arg[1])
                    {
                        // Just display the help message
                        case '?': 
                            return false;
                        case 'n':
                            showNumeric = true;
                            break;
                        case 'e':
                            showInterfaceStatistics = true;
                            break;
                        case 'a':
                            selectedProtocols =
                                ProtocolTypes.Tcp | ProtocolTypes.Udp |
                                ProtocolTypes.IP | ProtocolTypes.Icmp |
                                ProtocolTypes.TcpV6 | ProtocolTypes.UdpV6 |
                                ProtocolTypes.IPv6 | ProtocolTypes.IcmpV6;
                            showListening = true;
                            break;
                        case 's':
                            showProtocolStatistics = true;
                            break;
                        case 'p':
                            if (args.Length < i + 1)
                            {
                                return false; // invalid parameters
                            }
                            else
                            {
                                i++;
                                AddSelectedProtocol(
                                    args[i].ToLower(
                                    CultureInfo.CurrentCulture));
                            }
                            break;
                        default:
                            return false;
                    }
                }
            }
            if (selectedProtocols == ProtocolTypes.None)
            {
                // User wants Tcp as default.
                selectedProtocols = selectedProtocols | ProtocolTypes.Tcp;
            }
            return true;
        }

        // Get the display string for localIPEndPoint, either numric or full
        // name format.
        private static String GetLocalAddressAsString(IPEndPoint ipe)
        {
            string displayString = ipe.ToString();
            if (!showNumeric)
            {
                displayString =
                    Dns.GetHostName() + ":" + GetPortNameMapping(ipe.Port);
            }
            return displayString;
        }

        // Get the display string for Remote IPEndPoint, either numric or full 
        // name format.
        private static String GetRemoteAddressAsString(IPEndPoint ipe)
        {
            string displayString = ipe.ToString();
            if (!showNumeric)
            {
                try
                {
                    displayString =
                        Dns.GetHostEntry(ipe.Address).HostName + ":" +
                        GetPortNameMapping(ipe.Port);
                }
                catch (SocketException)
                {
                }
            }
            return displayString;
        }

        private static void AddSelectedProtocol(string protocol)
        {
            switch (protocol)
            {
                case "tcp":
                    selectedProtocols = selectedProtocols | ProtocolTypes.Tcp;
                    break;
                case "udp":
                    selectedProtocols = selectedProtocols | ProtocolTypes.Udp;
                    break;
                case "ip":
                    selectedProtocols = selectedProtocols | ProtocolTypes.IP;
                    break;
                case "icmp":
                    selectedProtocols = selectedProtocols | ProtocolTypes.Icmp;
                    break;
                case "tcpv6":
                    selectedProtocols =
                        selectedProtocols | ProtocolTypes.TcpV6;
                    break;
                case "udpv6":
                    selectedProtocols =
                        selectedProtocols | ProtocolTypes.UdpV6;
                    break;
                case "ipv6":
                    selectedProtocols = selectedProtocols | ProtocolTypes.IPv6;
                    break;
                case "icmpv6":
                    selectedProtocols =
                        selectedProtocols | ProtocolTypes.IcmpV6;
                    break;
            }
        }

        // Return the displaystring for known protocol ports
        private static string GetPortNameMapping(int Port)
        {
            string Portname = Port.ToString(NumberFormatInfo.CurrentInfo);
            switch (Port)
            {
                case 21:
                    Portname = "ftp";
                    break;
                case 25:
                    Portname = "smtp";
                    break;
                case 80:
                    Portname = "http";
                    break;
                case 135:
                    Portname = "epmap";
                    break;
                case 137:
                    Portname = "netbios-ns";
                    break;
                case 138:
                    Portname = "netbios-dgm";
                    break;
                case 139:
                    Portname = "netbios-ssn";
                    break;
                case 443:
                    Portname = "ssl";
                    break;
                case 445:
                    Portname = "microsoft-ds";
                    break;
                case 500:
                    Portname = "isakmp";
                    break;

            }
            return Portname;
        }

        private static string GetProtocolString(ProtocolTypes type)
        {
            switch (type)
            {
                case ProtocolTypes.Icmp: return "ICMP";
                case ProtocolTypes.IcmpV6: return "ICMP V6";
                case ProtocolTypes.IP: return "IP";
                case ProtocolTypes.IPv6: return "IP V6";
                case ProtocolTypes.Tcp: return "TCP";
                case ProtocolTypes.TcpV6: return "TCP V6";
                case ProtocolTypes.Udp: return "UDP";
                case ProtocolTypes.UdpV6: return "UDP V6";
                default: return "Unknown";
            }
        }

        // Display the list of connections on console.
        private static void DisplayConnections()
        {
            IPGlobalProperties ipGlobal =
                IPGlobalProperties.GetIPGlobalProperties();
            Console.WriteLine();
            Console.WriteLine("Connections");
            Console.WriteLine(
                connectionFormatString, "Protocol", "State", "Local", "Remote");
            if ((selectedProtocols & ProtocolTypes.Tcp) == ProtocolTypes.Tcp)
            {
                TcpConnectionInformation[] tcpInfoList =
                    ipGlobal.GetActiveTcpConnections();
                foreach (TcpConnectionInformation tcpInfo in tcpInfoList)
                {
                    if (tcpInfo.State == TcpState.Listen && !showListening)
                    {
                        // Skip listening port unless user requested all.
                        continue;
                    }
                    else
                    {
                        Console.WriteLine(
                            connectionFormatString,
                            "TCP",
                            tcpInfo.State,
                            GetLocalAddressAsString(tcpInfo.LocalEndPoint),
                            GetRemoteAddressAsString(tcpInfo.RemoteEndPoint));
                    }
                }
            }
            if ((selectedProtocols & ProtocolTypes.Udp) == ProtocolTypes.Udp)
            {
                IPEndPoint[] udpInfoList = ipGlobal.GetActiveUdpListeners();
                foreach (IPEndPoint udpInfo in udpInfoList)
                {
                    Console.WriteLine(
                        connectionFormatString,
                        "UDP",
                        "(None)",
                        GetLocalAddressAsString(udpInfo),
                        "*:*");
                }
            }
        }

        // Display the interface statisitics of all interfaces on console.
        private static void DisplayInterfaceStatistics()
        {
            NetworkInterface[] networkInterfaceList =
                NetworkInterface.GetAllNetworkInterfaces();
            Console.WriteLine();
            Console.WriteLine("Interface Statistics");

            foreach (NetworkInterface nic in networkInterfaceList)
            {
                IPv4InterfaceStatistics interfaceStatistics =
                    nic.GetIPv4Statistics();
                Console.WriteLine(
                    sendReceiveFormatString, nic.Name, "Received", "Sent");
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Bytes",
                    interfaceStatistics.BytesReceived,
                    interfaceStatistics.BytesSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Unicast packets",
                    interfaceStatistics.UnicastPacketsReceived,
                    interfaceStatistics.UnicastPacketsSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Non-unicast packets",
                    interfaceStatistics.NonUnicastPacketsReceived,
                    interfaceStatistics.NonUnicastPacketsSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Discards",
                    interfaceStatistics.IncomingPacketsDiscarded,
                    interfaceStatistics.OutgoingPacketsDiscarded);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Errors",
                    interfaceStatistics.IncomingPacketsWithErrors,
                    interfaceStatistics.OutgoingPacketsWithErrors);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Unknown protocols",
                    interfaceStatistics.IncomingUnknownProtocolPackets,
                    "");
                Console.WriteLine();
            }
        }

        // Display the protocol statisitics of requested protocols.
        private static void DisplayProtocolStatistics()
        {
            IPGlobalProperties ipGlobal =
                IPGlobalProperties.GetIPGlobalProperties();

            if ((selectedProtocols & ProtocolTypes.IP) ==
                ProtocolTypes.IP)
                DisplayIPv4Statistics(ipGlobal);
            if ((selectedProtocols & ProtocolTypes.Tcp) ==
                ProtocolTypes.Tcp)
                DisplayTcpV4Statistics(ipGlobal);
            if ((selectedProtocols & ProtocolTypes.Udp) ==
                ProtocolTypes.Udp)
                DisplayUdpV4Statistics(ipGlobal);
            if ((selectedProtocols & ProtocolTypes.Icmp) ==
                ProtocolTypes.Icmp)
                DisplayIcmpV4Statistics(ipGlobal);
            if ((selectedProtocols & ProtocolTypes.UdpV6) ==
                ProtocolTypes.UdpV6)
                DisplayUdpV6Statistics(ipGlobal);
            if ((selectedProtocols & ProtocolTypes.TcpV6) ==
                ProtocolTypes.TcpV6)
                DisplayTcpV6Statistics(ipGlobal);
            if ((selectedProtocols & ProtocolTypes.IPv6) ==
                ProtocolTypes.IPv6)
                DisplayIPv6Statistics(ipGlobal);
            if ((selectedProtocols & ProtocolTypes.IcmpV6) ==
                ProtocolTypes.IcmpV6)
                DisplayIcmpV6Statistics(ipGlobal);
        }


        // Display the data for given IcmpV4Statistics object.
        private static void DisplayIcmpV4Statistics(
            IPGlobalProperties ipGlobal)
        {
            string protocol = GetProtocolString(ProtocolTypes.Icmp);
            Console.WriteLine();
            Console.WriteLine("{0} Statistics", protocol);

            try
            {
                IcmpV4Statistics icmpStat = ipGlobal.GetIcmpV4Statistics();
                Console.WriteLine(sendReceiveFormatString, "", "Received", "Sent");
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Messages",
                    icmpStat.MessagesReceived,
                    icmpStat.MessagesSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Errors",
                    icmpStat.ErrorsReceived,
                    icmpStat.ErrorsSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Destination Unreachable",
                    icmpStat.DestinationUnreachableMessagesReceived,
                    icmpStat.DestinationUnreachableMessagesSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Time Exceeded",
                    icmpStat.TimeExceededMessagesReceived,
                    icmpStat.TimeExceededMessagesSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Parameter Problems",
                    icmpStat.ParameterProblemsReceived,
                    icmpStat.ParameterProblemsSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Source Quenches",
                    icmpStat.SourceQuenchesReceived,
                    icmpStat.SourceQuenchesSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Redirects",
                    icmpStat.RedirectsReceived,
                    icmpStat.RedirectsSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Echos",
                    icmpStat.EchoRequestsReceived,
                    icmpStat.EchoRequestsSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Echo Replies",
                    icmpStat.EchoRepliesReceived,
                    icmpStat.EchoRepliesSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Timestamps",
                    icmpStat.TimestampRequestsReceived,
                    icmpStat.TimestampRequestsSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Timestamp Replies",
                    icmpStat.TimestampRepliesReceived,
                    icmpStat.TimestampRepliesSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Address Masks",
                    icmpStat.AddressMaskRequestsReceived,
                    icmpStat.AddressMaskRequestsSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Address Mask Replies",
                    icmpStat.AddressMaskRepliesReceived,
                    icmpStat.AddressMaskRepliesSent);
            }
            catch (NetworkInformationException ex)
            {
                Console.WriteLine(errorFormatString, protocol, ex.Message);
            }
        }

        // Display the data for given IcmpV6Statistics object.
        private static void DisplayIcmpV6Statistics(
            IPGlobalProperties ipGlobal)
        {
            string protocol = GetProtocolString(ProtocolTypes.IcmpV6);
            Console.WriteLine();
            Console.WriteLine("{0} Statistics", protocol);
            try
            {
                IcmpV6Statistics icmpStat = ipGlobal.GetIcmpV6Statistics();
                Console.WriteLine(sendReceiveFormatString, "", "Received", "Sent");
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Messages",
                    icmpStat.MessagesReceived,
                    icmpStat.MessagesSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Errors",
                    icmpStat.ErrorsReceived,
                    icmpStat.ErrorsSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Destination Unreachable",
                    icmpStat.DestinationUnreachableMessagesReceived,
                    icmpStat.DestinationUnreachableMessagesSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Time Exceeded",
                    icmpStat.TimeExceededMessagesReceived,
                    icmpStat.TimeExceededMessagesSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Parameter Problems",
                    icmpStat.ParameterProblemsReceived,
                    icmpStat.ParameterProblemsSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Redirects",
                    icmpStat.RedirectsReceived,
                    icmpStat.RedirectsSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Echos",
                    icmpStat.EchoRepliesReceived,
                    icmpStat.EchoRepliesSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Echo Replies",
                    icmpStat.EchoRepliesReceived,
                    icmpStat.EchoRepliesSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Membership Queries",
                    icmpStat.MembershipQueriesReceived,
                    icmpStat.MembershipQueriesSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Membership Reductios",
                    icmpStat.MembershipReductionsReceived,
                    icmpStat.MembershipReductionsSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Membership Reports",
                    icmpStat.MembershipReportsReceived,
                    icmpStat.MembershipReportsSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Neighbour Advertisements",
                    icmpStat.NeighborAdvertisementsReceived,
                    icmpStat.NeighborAdvertisementsSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Neighbour Solicit",
                    icmpStat.NeighborSolicitsReceived,
                    icmpStat.NeighborSolicitsSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Packets Too Large",
                    icmpStat.PacketTooBigMessagesReceived,
                    icmpStat.PacketTooBigMessagesSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Redirects",
                    icmpStat.RedirectsReceived,
                    icmpStat.RedirectsSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Router Advertisements",
                    icmpStat.RouterAdvertisementsReceived,
                    icmpStat.RouterAdvertisementsSent);
                Console.WriteLine(
                    sendReceiveFormatString,
                    "Router Solicits",
                    icmpStat.RouterSolicitsReceived,
                    icmpStat.RouterSolicitsSent);
            }
            catch (NetworkInformationException ex)
            {
                Console.WriteLine(errorFormatString, protocol, ex.Message);
            }
        }

        private static void DisplayUdpV4Statistics(IPGlobalProperties ipGlobal)
        {
            string protocol = GetProtocolString(ProtocolTypes.Udp);
            Console.WriteLine();
            Console.WriteLine("{0} Statistics", protocol);
            try
            {
                DisplayUdpStatistics(ipGlobal.GetUdpIPv4Statistics());
            }
            catch (NetworkInformationException ex)
            {
                Console.WriteLine(errorFormatString, protocol, ex.Message);
            }
        }

        private static void DisplayUdpV6Statistics(IPGlobalProperties ipGlobal)
        {
            string protocol = GetProtocolString(ProtocolTypes.UdpV6);
            Console.WriteLine();
            Console.WriteLine("{0} Statistics", protocol);
            try
            {
                DisplayUdpStatistics(ipGlobal.GetUdpIPv6Statistics());
            }
            catch (NetworkInformationException ex)
            {
                Console.WriteLine(errorFormatString, protocol, ex.Message);
            }
        }

        // Display the data for given UdpStatistics object.
        private static void DisplayUdpStatistics(UdpStatistics udpStat)
        {
            Console.WriteLine(
                equalsFormatString,
                "Datagrams Received",
                udpStat.DatagramsReceived);
            Console.WriteLine(
                equalsFormatString,
                "No Ports",
                udpStat.IncomingDatagramsDiscarded);
            Console.WriteLine(
                equalsFormatString,
                "Receive Errors",
                udpStat.IncomingDatagramsWithErrors);
            Console.WriteLine(
                equalsFormatString, "Datagrams Sent", udpStat.DatagramsSent);
        }

        private static void DisplayTcpV4Statistics(IPGlobalProperties ipGlobal)
        {
            string protocol = GetProtocolString(ProtocolTypes.Tcp);
            Console.WriteLine();
            Console.WriteLine("{0} Statistics", protocol);
            try
            {
                DisplayTcpStatistics(ipGlobal.GetTcpIPv4Statistics());
            }
            catch (NetworkInformationException ex)
            {
                Console.WriteLine(errorFormatString, protocol, ex.Message);
            }
        }

        private static void DisplayTcpV6Statistics(IPGlobalProperties ipGlobal)
        {
            string protocol = GetProtocolString(ProtocolTypes.TcpV6);
            Console.WriteLine();
            Console.WriteLine("{0} Statistics", protocol);
            try
            {
                DisplayTcpStatistics(ipGlobal.GetTcpIPv6Statistics());
            }
            catch (NetworkInformationException ex)
            {
                Console.WriteLine(errorFormatString, protocol, ex.Message);
            }
        }

        // Display the data for given TcpStatistics object.
        private static void DisplayTcpStatistics(TcpStatistics tcpStat)
        {
            Console.WriteLine(
                equalsFormatString,
                "Active Opens",
                tcpStat.ConnectionsInitiated);
            Console.WriteLine(
                equalsFormatString,
                "Passive Opens",
                tcpStat.ConnectionsAccepted);
            Console.WriteLine(
                equalsFormatString,
                "Failed Connection Attempts",
                tcpStat.FailedConnectionAttempts);
            Console.WriteLine(
                equalsFormatString,
                "Reset Connections",
                tcpStat.ResetConnections);
            Console.WriteLine(
                equalsFormatString,
                "Current Connections",
                tcpStat.CurrentConnections);
            Console.WriteLine(
                equalsFormatString,
                "Segments Received",
                tcpStat.SegmentsReceived);
            Console.WriteLine(
                equalsFormatString,
                "Segments Sent",
                tcpStat.SegmentsSent);
            Console.WriteLine(
                equalsFormatString,
                "Segments Retransmitted",
                tcpStat.SegmentsResent);
        }

        private static void DisplayIPv4Statistics(IPGlobalProperties ipGlobal)
        {
            string protocol = GetProtocolString(ProtocolTypes.IP);
            Console.WriteLine();
            Console.WriteLine("{0} Statistics", protocol);
            try
            {
                DisplayIPStatistics(ipGlobal.GetIPv4GlobalStatistics());
            }
            catch (NetworkInformationException ex)
            {
                Console.WriteLine(errorFormatString, protocol, ex.Message);
            }
        }

        private static void DisplayIPv6Statistics(IPGlobalProperties ipGlobal)
        {
            string protocol = GetProtocolString(ProtocolTypes.IPv6);
            Console.WriteLine();
            Console.WriteLine("{0} Statistics", protocol);
            try
            {
                DisplayIPStatistics(ipGlobal.GetIPv6GlobalStatistics());
            }
            catch (NetworkInformationException ex)
            {
                Console.WriteLine(errorFormatString, protocol, ex.Message);
            }
        }

        // Display the global IP statisitcs for given IPGlobalStatistics 
        // object.
        private static void DisplayIPStatistics(IPGlobalStatistics ipStat)
        {
            Console.WriteLine(
                equalsFormatString,
                "Packets Received",
                ipStat.ReceivedPackets);
            Console.WriteLine(
                equalsFormatString,
                "Received Header Errors",
                ipStat.ReceivedPacketsWithHeadersErrors);
            Console.WriteLine(
                equalsFormatString,
                "Received Address Errors",
                ipStat.ReceivedPacketsWithAddressErrors);
            Console.WriteLine(
                equalsFormatString,
                "Datagrams Forwarded",
                ipStat.ReceivedPacketsForwarded);
            Console.WriteLine(
                equalsFormatString,
                "Unknown Protocols Received",
                ipStat.ReceivedPacketsWithUnknownProtocol);
            Console.WriteLine(
                equalsFormatString,
                "Received Packets Discarded",
                ipStat.ReceivedPacketsDiscarded);
            Console.WriteLine(
                equalsFormatString,
                "Received Packets Delivered",
                ipStat.ReceivedPacketsDelivered);
            Console.WriteLine(
                equalsFormatString,
                "Output Requests",
                ipStat.OutputPacketRequests);
            Console.WriteLine(
                equalsFormatString,
                "Routing Discards",
                ipStat.OutputPacketRoutingDiscards);
            Console.WriteLine(
                equalsFormatString,
                "Discarded Output Packets",
                ipStat.OutputPacketsDiscarded);
            Console.WriteLine(
                equalsFormatString,
                "Output Packet No Route",
                ipStat.OutputPacketsWithNoRoute);
            Console.WriteLine(
                equalsFormatString,
                "Reassembly Required",
                ipStat.PacketReassembliesRequired);
            Console.WriteLine(
                equalsFormatString,
                "Reassembly Successful",
                ipStat.PacketsReassembled);
            Console.WriteLine(
                equalsFormatString,
                "Reassembly Failures",
                ipStat.PacketReassemblyFailures);
            Console.WriteLine(
                equalsFormatString,
                "Datagrams Successfuly Fragmented",
                ipStat.PacketsFragmented);
            Console.WriteLine(
                equalsFormatString,
                "Datagrams Failing Fragmentation",
                ipStat.PacketFragmentFailures);
            Console.WriteLine(
                equalsFormatString,
                "Fragments Created",
                ipStat.PacketsFragmented);
        }
    }
}
