'---------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.Net
Imports System.Net.Sockets
Imports System.Net.NetworkInformation
Imports System.Globalization

Module NetStatTool

    <Flags()> _
    Enum ProtocolTypes
        None = &H0
        TCP = &H1
        UDP = &H2
        TCPV6 = &H4
        UDPV6 = &H8
        IP = &H10
        IPV6 = &H20
        ICMP = &H40
        ICMPV6 = &H80
    End Enum

    Dim showNumeric As Boolean
    Dim showListening As Boolean
    Dim showProtocolStatistics As Boolean
    Dim showInterfaceStatistics As Boolean
    Dim interval As Integer = -1
    Dim selectedProtocols As ProtocolTypes = ProtocolTypes.None
    Dim connectionFormatString As String = _
        vbTab & "{0,-12}{1, -16}{2, -28}{3}"
    Dim equalsFormatString As String = _
        vbTab & "{0, -32} = {1}"
    Dim sendReceiveFormatString As String = _
        vbTab & "{0, -32}{1, -16}{2}"
    Dim errorFormatString As String = _
        vbTab & "Statistics not available for {0}: {1}"

    Sub Main(ByVal args() As String)
        If ParseArguments(args) Then
            While True
                If showInterfaceStatistics Then
                    DisplayInterfaceStatistics()
                End If
                If showProtocolStatistics Then
                    DisplayProtocolStatistics()
                End If
                If Not showInterfaceStatistics AndAlso _
                    Not showProtocolStatistics Then
                    DisplayConnections()
                End If
                If interval <= 0 Then
                    Exit While
                Else
                    System.Threading.Thread.Sleep(interval * 1000)
                End If
            End While
        Else
            Usage()
            System.Environment.Exit(0)
        End If
    End Sub

    ' Helper function display the all possible command line options and their
    ' purpose for this tool.
    Private Sub Usage()
        Console.WriteLine()
        Console.WriteLine("NETSTATTOOL [-?] [-a] [-e] [-n] [-s] [-p protocol] [interval]")
        Console.WriteLine()
        Console.WriteLine("(default)     Displays open connection information.")
        Console.WriteLine("-?            Displays this help message.")
        Console.WriteLine("-e            Displays NIC statistics. May be combined with -s.")
        Console.WriteLine("-s            Displays per-protocol statistics.  By default, statistics are")
        Console.WriteLine("                  shown for TCP.  The -p and -a options may be used to specify")
        Console.WriteLine("                  other protocols to display.")
        Console.WriteLine("-p protocol   Display information for the protocol specified.")
        Console.WriteLine("                  May be any of: IP, IPv6, ICMP, ICMPv6, TCP, TCPv6, UDP,")
        Console.WriteLine("                  or UDPv6TCP.  Connections can only be shown for TCP,")
        Console.WriteLine("                  UDP, TCPv6, or UDPv6.")
        Console.WriteLine("-a            Display information for all protocals.")
        Console.WriteLine("-n            Displays addresses and port numbers in numerical form.")
        Console.WriteLine("interval      Redisplays selected statistics, pausing interval seconds")
        Console.WriteLine("                  between each display.  Press CTRL+C to stop.")
    End Sub

    ' Parse the command line arguments and set appropriate properties as per 
    ' requested user options
    Private Function ParseArguments(ByVal args() As String) As Boolean
        For i As Integer = 0 To args.Length - 1
            Dim arg As String = args(i).ToLower(CultureInfo.CurrentCulture)
            If arg.Length < 2 OrElse _
                (arg(0) <> "/"c AndAlso arg(0) <> "-"c) Then
                Try
                    interval = _
                        Convert.ToInt32(arg, NumberFormatInfo.CurrentInfo)
                Catch ex As FormatException
                    Return False
                Catch ex As OverflowException
                    Return False
                End Try
            Else
                Select Case arg(1)
                    ' Just display the help message
                    Case "?"
                        Return False
                    Case "n"
                        showNumeric = True
                    Case "e"
                        showInterfaceStatistics = True
                    Case "a"
                        selectedProtocols = _
                            ProtocolTypes.TCP Or ProtocolTypes.UDP Or _
                            ProtocolTypes.IP Or ProtocolTypes.ICMP Or _
                            ProtocolTypes.TCPV6 Or ProtocolTypes.UDPV6 Or _
                            ProtocolTypes.IPV6 Or ProtocolTypes.ICMPV6
                        showListening = True
                    Case "s"
                        showProtocolStatistics = True
                    Case "p"
                        If args.Length < i + 1 Then
                            ' Invalid parameters
                            Return False
                        Else
                            i += 1
                            AddSelectedProtocol( _
                                args(i).ToLower(CultureInfo.CurrentCulture))
                        End If
                    Case Else
                        Return False
                End Select
            End If
        Next i
        If selectedProtocols = ProtocolTypes.None Then
            ' User wants Tcp as default.
            selectedProtocols = selectedProtocols Or ProtocolTypes.TCP
        End If
        Return True
    End Function

    ' Get the display string for localIPEndPoint, either numric or full 
    ' name format.
    Private Function GetLocalAddressAsString( _
        ByVal ipe As IPEndPoint) As String
        Dim displayString As String = ipe.ToString()
        If Not showNumeric Then
            displayString = _
                    Dns.GetHostName() & ":" & GetPortNameMapping(ipe.Port)
        End If
        Return displayString
    End Function

    ' Get the display string for Remote IPEndPoint, either numric or full 
    ' name format
    Private Function GetRemoteAddressAsString( _
        ByVal ipe As IPEndPoint) As String
        Dim displayString As String = ipe.ToString()
        If Not showNumeric Then
            Try
                displayString = _
                    Dns.GetHostEntry(ipe.Address).HostName & ":" & _
                    GetPortNameMapping(ipe.Port)
            Catch ex As SocketException
            End Try
        End If
        Return displayString
    End Function

    Private Sub AddSelectedProtocol(ByVal protocol As String)
        Select Case protocol
            Case "tcp"
                selectedProtocols = _
                    selectedProtocols Or ProtocolTypes.TCP
            Case "udp"
                selectedProtocols = _
                    selectedProtocols Or ProtocolTypes.UDP
            Case "ip"
                selectedProtocols = _
                    selectedProtocols Or ProtocolTypes.IP
            Case "icmp"
                selectedProtocols = _
                    selectedProtocols Or ProtocolTypes.ICMP
            Case "tcpv6"
                selectedProtocols = _
                    selectedProtocols Or ProtocolTypes.TCPV6
            Case "udpv6"
                selectedProtocols = _
                    selectedProtocols Or ProtocolTypes.UDPV6
            Case "ipv6"
                selectedProtocols = _
                    selectedProtocols Or ProtocolTypes.IPV6
            Case "icmpv6"
                selectedProtocols = _
                selectedProtocols Or ProtocolTypes.ICMPV6
        End Select
    End Sub

    Private Function GetPortNameMapping(ByVal Port As Integer) As String
        Dim Portname As String = Port.ToString(NumberFormatInfo.CurrentInfo)
        Select Case Port
            Case 21
                Portname = "ftp"
            Case 25
                Portname = "smtp"
            Case 80
                Portname = "http"
            Case 135
                Portname = "epmap"
            Case 137
                Portname = "netbios-ns"
            Case 138
                Portname = "netbios-dgm"
            Case 139
                Portname = "netbios-ssn"
            Case 443
                Portname = "ssl"
            Case 445
                Portname = "microsoft-ds"
            Case 500
                Portname = "isakmp"
        End Select

        Return Portname
    End Function

    Private Function GetProtocolString(ByVal type As ProtocolTypes) As String
        Select Case type
            Case ProtocolTypes.ICMP
                Return "ICMP"
            Case ProtocolTypes.ICMPV6
                Return "ICMP V6"
            Case ProtocolTypes.IP
                Return "IP"
            Case ProtocolTypes.IPV6
                Return "IP V6"
            Case ProtocolTypes.TCP
                Return "TCP"
            Case ProtocolTypes.TCPV6
                Return "TCP V6"
            Case ProtocolTypes.UDP
                Return "UDP"
            Case ProtocolTypes.UDPV6
                Return "UDP V6"
            Case Else
                Return "Unknown"
        End Select
    End Function

    Private Sub DisplayConnections()
        Dim ipGlobal As IPGlobalProperties = _
            IPGlobalProperties.GetIPGlobalProperties()
        Console.WriteLine()
        Console.WriteLine("Connections")
        Console.WriteLine( _
            connectionFormatString, "Protocol", "State", "Local", "Remote")
        If (selectedProtocols And ProtocolTypes.TCP) = ProtocolTypes.TCP Then
            Dim tcpInfoList As TcpConnectionInformation() = _
                ipGlobal.GetActiveTcpConnections()
            Dim tcpInfo As TcpConnectionInformation
            For Each tcpInfo In tcpInfoList
                If tcpInfo.State = _
                    TcpState.Listen AndAlso Not showListening Then
                    ' Skip listening port unless user requested all.
                    Continue For
                Else
                    Console.WriteLine( _
                        connectionFormatString, _
                        "TCP", _
                        tcpInfo.State, _
                        GetLocalAddressAsString(tcpInfo.LocalEndPoint), _
                        GetRemoteAddressAsString(tcpInfo.RemoteEndPoint))
                End If
            Next tcpInfo
        End If
        If (selectedProtocols And ProtocolTypes.UDP) = ProtocolTypes.UDP Then
            Dim udpInfoList As IPEndPoint() = ipGlobal.GetActiveUdpListeners()
            Dim udpInfo As IPEndPoint
            For Each udpInfo In udpInfoList
                Console.WriteLine( _
                    connectionFormatString, _
                    "UDP", _
                    "(None)", _
                    GetLocalAddressAsString(udpInfo), _
                    "*:*")
            Next udpInfo
        End If
    End Sub

    Private Sub DisplayInterfaceStatistics()
        Dim networkInterfaceList() As NetworkInterface = _
            NetworkInterface.GetAllNetworkInterfaces()
        Console.WriteLine()
        Console.WriteLine("Interface Statistics")

        Dim nic As NetworkInterface
        For Each nic In networkInterfaceList
            Dim interfaceStatistics As IPv4InterfaceStatistics = _
                nic.GetIPv4Statistics()
            Console.WriteLine( _
                sendReceiveFormatString, nic.Name, "Received", "Sent")
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Bytes", _
                interfaceStatistics.BytesReceived, _
                interfaceStatistics.BytesSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Unicast packets", _
                interfaceStatistics.UnicastPacketsReceived, _
                interfaceStatistics.UnicastPacketsSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Non-unicast packets", _
                interfaceStatistics.NonUnicastPacketsReceived, _
                interfaceStatistics.NonUnicastPacketsSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Discards", _
                interfaceStatistics.IncomingPacketsDiscarded, _
                interfaceStatistics.OutgoingPacketsDiscarded)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Errors", _
                interfaceStatistics.IncomingPacketsWithErrors, _
                interfaceStatistics.OutgoingPacketsWithErrors)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Unknown protocols", _
                interfaceStatistics.IncomingUnknownProtocolPackets, _
                "")
            Console.WriteLine()
        Next nic
    End Sub

    Private Sub DisplayProtocolStatistics()
        Dim ipGlobal As IPGlobalProperties = _
            IPGlobalProperties.GetIPGlobalProperties()
        If (selectedProtocols And ProtocolTypes.IP) = _
            ProtocolTypes.IP Then
            DisplayIPv4Statistics(ipGlobal)
        End If
        If (selectedProtocols And ProtocolTypes.TCP) = _
            ProtocolTypes.TCP Then
            DisplayTcpV4Statistics(ipGlobal)
        End If
        If (selectedProtocols And ProtocolTypes.UDP) = _
            ProtocolTypes.UDP Then
            DisplayUdpV4Statistics(ipGlobal)
        End If
        If (selectedProtocols And ProtocolTypes.ICMP) = _
            ProtocolTypes.ICMP Then
            DisplayIcmpV4Statistics(ipGlobal)
        End If
        If (selectedProtocols And ProtocolTypes.UDPV6) = _
            ProtocolTypes.UDPV6 Then
            DisplayUdpV6Statistics(ipGlobal)
        End If
        If (selectedProtocols And ProtocolTypes.TCPV6) = _
            ProtocolTypes.TCPV6 Then
            DisplayTcpV6Statistics(ipGlobal)
        End If
        If (selectedProtocols And ProtocolTypes.IPV6) = _
            ProtocolTypes.IPV6 Then
            DisplayIPv6Statistics(ipGlobal)
        End If
        If (selectedProtocols And ProtocolTypes.ICMPV6) = _
            ProtocolTypes.ICMPV6 Then
            DisplayIcmpV6Statistics(ipGlobal)
        End If
    End Sub

    ' Display the data for given IcmpV4Statistics object.
    Private Sub DisplayIcmpV4Statistics(ByVal ipGlobal As IPGlobalProperties)
        Dim protocol As String = GetProtocolString(ProtocolTypes.ICMP)
        Console.WriteLine()
        Console.WriteLine("{0} Statistics", protocol)

        Try
            Dim icmpStat As IcmpV4Statistics = ipGlobal.GetIcmpV4Statistics()
            Console.WriteLine(sendReceiveFormatString, "", "Received", "Sent")
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Messages", _
                icmpStat.MessagesReceived, _
                icmpStat.MessagesSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Errors", _
                icmpStat.ErrorsReceived, _
                icmpStat.ErrorsSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Destination Unreachable", _
                icmpStat.DestinationUnreachableMessagesReceived, _
                icmpStat.DestinationUnreachableMessagesSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Time Exceeded", _
                icmpStat.TimeExceededMessagesReceived, _
                icmpStat.TimeExceededMessagesSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Parameter Problems", _
                icmpStat.ParameterProblemsReceived, _
                icmpStat.ParameterProblemsSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Source Quenches", _
                icmpStat.SourceQuenchesReceived, _
                icmpStat.SourceQuenchesSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Redirects", _
                icmpStat.RedirectsReceived, _
                icmpStat.RedirectsSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Echos", _
                icmpStat.EchoRequestsReceived, _
                icmpStat.EchoRequestsSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Echo Replies", _
                icmpStat.EchoRepliesReceived, _
                icmpStat.EchoRepliesSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Timestamps", _
                icmpStat.TimestampRequestsReceived, _
                icmpStat.TimestampRequestsSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Timestamp Replies", _
                icmpStat.TimestampRepliesReceived, _
                icmpStat.TimestampRepliesSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Address Masks", _
                icmpStat.AddressMaskRequestsReceived, _
                icmpStat.AddressMaskRequestsSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Address Mask Replies", _
                icmpStat.AddressMaskRepliesReceived, _
                icmpStat.AddressMaskRepliesSent)
        Catch ex As NetworkInformationException
            Console.WriteLine(errorFormatString, protocol, ex.Message)
        End Try

    End Sub 'DisplayIcmpV4Statistics

    ' Display the data for given IcmpV6Statistics object.
    Private Sub DisplayIcmpV6Statistics(ByVal ipGlobal As IPGlobalProperties)
        Dim protocol As String = GetProtocolString(ProtocolTypes.ICMPV6)
        Console.WriteLine()
        Console.WriteLine("{0} Statistics", protocol)
        Try
            Dim icmpStat As IcmpV6Statistics = ipGlobal.GetIcmpV6Statistics()
            Console.WriteLine(sendReceiveFormatString, "", "Received", "Sent")
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Messages", _
                icmpStat.MessagesReceived, _
                icmpStat.MessagesSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Errors", _
                icmpStat.ErrorsReceived, _
                icmpStat.ErrorsSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Destination Unreachable", _
                icmpStat.DestinationUnreachableMessagesReceived, _
                icmpStat.DestinationUnreachableMessagesSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Time Exceeded", _
                icmpStat.TimeExceededMessagesReceived, _
                icmpStat.TimeExceededMessagesSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Parameter Problems", _
                icmpStat.ParameterProblemsReceived, _
                icmpStat.ParameterProblemsSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Redirects", _
                icmpStat.RedirectsReceived, _
                icmpStat.RedirectsSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Echos", _
                icmpStat.EchoRepliesReceived, _
                icmpStat.EchoRepliesSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Echo Replies", _
                icmpStat.EchoRepliesReceived, _
                icmpStat.EchoRepliesSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Membership Queries", _
                icmpStat.MembershipQueriesReceived, _
                icmpStat.MembershipQueriesSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Membership Reductios", _
                icmpStat.MembershipReductionsReceived, _
                icmpStat.MembershipReductionsSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Membership Reports", _
                icmpStat.MembershipReportsReceived, _
                icmpStat.MembershipReportsSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Neighbour Advertisements", _
                icmpStat.NeighborAdvertisementsReceived, _
                icmpStat.NeighborAdvertisementsSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Neighbour Solicit", _
                icmpStat.NeighborSolicitsReceived, _
                icmpStat.NeighborSolicitsSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Packets Too Large", _
                icmpStat.PacketTooBigMessagesReceived, _
                icmpStat.PacketTooBigMessagesSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Redirects", _
                icmpStat.RedirectsReceived, _
                icmpStat.RedirectsSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Router Advertisements", _
                icmpStat.RouterAdvertisementsReceived, _
                icmpStat.RouterAdvertisementsSent)
            Console.WriteLine( _
                sendReceiveFormatString, _
                "Router Solicits", _
                icmpStat.RouterSolicitsReceived, _
                icmpStat.RouterSolicitsSent)
        Catch ex As NetworkInformationException
            Console.WriteLine(errorFormatString, protocol, ex.Message)
        End Try
    End Sub 'DisplayIcmpV6Statistics

    Private Sub DisplayUdpV4Statistics(ByVal ipGlobal As IPGlobalProperties)
        Dim protocol As String = GetProtocolString(ProtocolTypes.UDP)
        Console.WriteLine()
        Console.WriteLine("{0} Statistics", protocol)
        Try
            DisplayUdpStatistics(ipGlobal.GetUdpIPv4Statistics())
        Catch ex As NetworkInformationException
            Console.WriteLine(errorFormatString, protocol, ex.Message)
        End Try
    End Sub 'DisplayUdpV4Statistics

    Private Sub DisplayUdpV6Statistics(ByVal ipGlobal As IPGlobalProperties)
        Dim protocol As String = GetProtocolString(ProtocolTypes.UDPV6)
        Console.WriteLine()
        Console.WriteLine("{0} Statistics", protocol)
        Try
            DisplayUdpStatistics(ipGlobal.GetUdpIPv6Statistics())
        Catch ex As NetworkInformationException
            Console.WriteLine(errorFormatString, protocol, ex.Message)
        End Try
    End Sub 'DisplayUdpV6Statistics

    ' Display the data for given UdpStatistics object.
    Private Sub DisplayUdpStatistics(ByVal udpStat As UdpStatistics)
        Console.WriteLine( _
            equalsFormatString, _
            "Datagrams Received", _
            udpStat.DatagramsReceived)
        Console.WriteLine( _
            equalsFormatString, _
            "No Ports", _
            udpStat.IncomingDatagramsDiscarded)
        Console.WriteLine( _
            equalsFormatString, _
            "Receive Errors", _
            udpStat.IncomingDatagramsWithErrors)
        Console.WriteLine( _
            equalsFormatString, _
            "Datagrams Sent", _
            udpStat.DatagramsSent)
    End Sub 'DisplayUdpStatistics

    Private Sub DisplayTcpV4Statistics(ByVal ipGlobal As IPGlobalProperties)
        Dim protocol As String = GetProtocolString(ProtocolTypes.TCP)
        Console.WriteLine()
        Console.WriteLine("{0} Statistics", protocol)
        Try
            DisplayTcpStatistics(ipGlobal.GetTcpIPv4Statistics())
        Catch ex As NetworkInformationException
            Console.WriteLine(errorFormatString, protocol, ex.Message)
        End Try
    End Sub 'DisplayTcpV4Statistics

    Private Sub DisplayTcpV6Statistics(ByVal ipGlobal As IPGlobalProperties)
        Dim protocol As String = GetProtocolString(ProtocolTypes.TCPV6)
        Console.WriteLine()
        Console.WriteLine("{0} Statistics", protocol)
        Try
            DisplayTcpStatistics(ipGlobal.GetTcpIPv6Statistics())
        Catch ex As NetworkInformationException
            Console.WriteLine(errorFormatString, protocol, ex.Message)
        End Try
    End Sub 'DisplayTcpV6Statistics


    ' Display the data for given TcpStatistics object.
    Private Sub DisplayTcpStatistics(ByVal tcpStat As TcpStatistics)
        Console.WriteLine( _
            equalsFormatString, _
            "Active Opens", _
            tcpStat.ConnectionsInitiated)
        Console.WriteLine( _
            equalsFormatString, _
            "Passive Opens", _
            tcpStat.ConnectionsAccepted)
        Console.WriteLine( _
            equalsFormatString, _
            "Failed Connection Attempts", _
            tcpStat.FailedConnectionAttempts)
        Console.WriteLine( _
            equalsFormatString, _
            "Reset Connections", _
            tcpStat.ResetConnections)
        Console.WriteLine( _
            equalsFormatString, _
            "Current Connections", _
            tcpStat.CurrentConnections)
        Console.WriteLine( _
            equalsFormatString, _
            "Segments Received", _
            tcpStat.SegmentsReceived)
        Console.WriteLine( _
            equalsFormatString, _
            "Segments Sent", _
            tcpStat.SegmentsSent)
        Console.WriteLine( _
            equalsFormatString, _
            "Segments Retransmitted", _
            tcpStat.SegmentsResent)
    End Sub 'DisplayTcpStatistics


    Private Sub DisplayIPv4Statistics(ByVal ipGlobal As IPGlobalProperties)
        Dim protocol As String = GetProtocolString(ProtocolTypes.IP)
        Console.WriteLine()
        Console.WriteLine("{0} Statistics", protocol)
        Try
            DisplayIPStatistics(ipGlobal.GetIPv4GlobalStatistics())
        Catch ex As NetworkInformationException
            Console.WriteLine(errorFormatString, protocol, ex.Message)
        End Try
    End Sub 'DisplayIPv4Statistics


    Private Sub DisplayIPv6Statistics(ByVal ipGlobal As IPGlobalProperties)
        Dim protocol As String = GetProtocolString(ProtocolTypes.IPV6)
        Console.WriteLine()
        Console.WriteLine("{0} Statistics", protocol)
        Try
            DisplayIPStatistics(ipGlobal.GetIPv6GlobalStatistics())
        Catch ex As NetworkInformationException
            Console.WriteLine(errorFormatString, protocol, ex.Message)
        End Try
    End Sub 'DisplayIPv6Statistics


    ' Display the global IP statisitcs for given IPGlobalStatistics 
    ' object.
    Private Sub DisplayIPStatistics(ByVal ipStat As IPGlobalStatistics)
        Console.WriteLine( _
            equalsFormatString, _
            "Packets Received", _
            ipStat.ReceivedPackets)
        Console.WriteLine( _
            equalsFormatString, _
            "Received Header Errors", _
            ipStat.ReceivedPacketsWithHeadersErrors)
        Console.WriteLine( _
            equalsFormatString, _
            "Received Address Errors", _
            ipStat.ReceivedPacketsWithAddressErrors)
        Console.WriteLine( _
            equalsFormatString, _
            "Datagrams Forwarded", _
            ipStat.ReceivedPacketsForwarded)
        Console.WriteLine( _
            equalsFormatString, _
            "Unknown Protocols Received", _
            ipStat.ReceivedPacketsWithUnknownProtocol)
        Console.WriteLine( _
            equalsFormatString, _
            "Received Packets Discarded", _
            ipStat.ReceivedPacketsDiscarded)
        Console.WriteLine( _
            equalsFormatString, _
            "Received Packets Delivered", _
            ipStat.ReceivedPacketsDelivered)
        Console.WriteLine( _
            equalsFormatString, _
            "Output Requests", _
            ipStat.OutputPacketRequests)
        Console.WriteLine( _
            equalsFormatString, _
            "Routing Discards", _
            ipStat.OutputPacketRoutingDiscards)
        Console.WriteLine( _
            equalsFormatString, _
            "Discarded Output Packets", _
            ipStat.OutputPacketsDiscarded)
        Console.WriteLine( _
            equalsFormatString, _
            "Output Packet No Route", _
            ipStat.OutputPacketsWithNoRoute)
        Console.WriteLine( _
            equalsFormatString, _
            "Reassembly Required", _
            ipStat.PacketReassembliesRequired)
        Console.WriteLine( _
            equalsFormatString, _
            "Reassembly Successful", _
            ipStat.PacketsReassembled)
        Console.WriteLine( _
            equalsFormatString, _
            "Reassembly Failures", _
            ipStat.PacketReassemblyFailures)
        Console.WriteLine( _
            equalsFormatString, _
            "Datagrams Successfuly Fragmented", _
            ipStat.PacketsFragmented)
        Console.WriteLine( _
            equalsFormatString, _
            "Datagrams Failing Fragmentation", _
            ipStat.PacketFragmentFailures)
        Console.WriteLine( _
            equalsFormatString, _
            "Fragments Created", _
            ipStat.PacketsFragmented)
    End Sub 'DisplayIPStatistics
End Module
