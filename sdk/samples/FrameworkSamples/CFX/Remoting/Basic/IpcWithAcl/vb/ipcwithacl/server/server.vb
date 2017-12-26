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
Imports System.Collections
Imports System.Security
Imports System.Security.AccessControl
Imports System.Runtime.Remoting
Imports System.Runtime.Remoting.Channels
Imports System.Runtime.Remoting.Channels.Ipc
Imports System.Security.Principal
Imports Microsoft.Samples.Server


'/ <summary>
'/ Summary description for Class1.
'/ </summary>

Public Class Class1

    '/ <summary>
    '/ The main entry point for the application.
    '/ </summary>
    Shared Sub Main()
        RemotingConfiguration.Configure("Server.exe.config", false)

        Dim props As Hashtable = New Hashtable()
        props("portName") = "test"

        ' This is the wellknown sid for network sid
        Dim networkSidSddlForm As String = "S-1-5-2"
        ' Local administrators sid
        Dim localAdminSid As New SecurityIdentifier(WellKnownSidType.BuiltinAdministratorsSid, Nothing)
        ' Local Power users sid
        Dim powerUsersSid As New SecurityIdentifier(WellKnownSidType.BuiltinPowerUsersSid, Nothing)
        ' Network sid
        Dim networkSid As New SecurityIdentifier(networkSidSddlForm)

        Dim dacl As New DiscretionaryAcl(False, False, 1)

        ' Disallow access from off machine
        dacl.AddAccess(AccessControlType.Deny, networkSid, -1, InheritanceFlags.None, PropagationFlags.None)

        ' Allow acces only from local administrators and power users
        dacl.AddAccess(AccessControlType.Allow, localAdminSid, -1, InheritanceFlags.None, PropagationFlags.None)
        dacl.AddAccess(AccessControlType.Allow, powerUsersSid, -1, InheritanceFlags.None, PropagationFlags.None)

        Dim securityDescriptor As New CommonSecurityDescriptor(False, False, ControlFlags.GroupDefaulted Or ControlFlags.OwnerDefaulted Or ControlFlags.DiscretionaryAclPresent, Nothing, Nothing, Nothing, dacl)

        Dim channel As New IpcServerChannel(props, Nothing, securityDescriptor)

        ChannelServices.RegisterChannel(channel, false)

        Dim chan As IChannel
        For Each chan In ChannelServices.RegisteredChannels
            Console.WriteLine(chan.ChannelName)
        Next chan

        Console.WriteLine("Waiting for connections...")
        Console.WriteLine("Press enter to exit.")
        Console.ReadLine()

    End Sub 'Main
End Class 'Class1