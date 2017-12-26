'****************************** Module Header ******************************'
' Module Name:  MainModule.vb
' Project:      VBRemotingClient
' Copyright (c) Microsoft Corporation.
' 
' .NET remoting provides an abstract approach to interprocess communication 
' that separates the remotable object from a specific client or server 
' application domain and from a specific mechanism of communication. 
' 
' .NET Remoting allows an application to make a remotable object available 
' across remoting boundaries, which includes different appdomains, processes 
' or even different computers connected by a network. .NET Remoting makes a 
' reference of a remotable object available to a client application, which  
' then instantiates and uses a remotable object as if it were a local object. 
' However, the actual code execution happens at the server-side. Any requests 
' to the remotable object are proxied by the .NET Remoting runtime over  
' Channel objects, that encapsulate the actual transport mode, including TCP 
' streams, HTTP streams and named pipes. As a result, by instantiating proper 
' Channel objects, a .NET Remoting application can be made to support 
' different communication protocols without recompiling the application. The  
' runtime itself manages the act of serialization and marshalling of objects 
' across the client and server appdomains.
' 
' VBRemotingClient is a .NET Remoting client project. It accesses the remote 
' objects exposed by the .NET Remoting server project VBRemotingServer.
' 
' There are generally two ways to create the .NET Remoting client: using a 
' configuration file or writing codes. The AccessRemotingServerByConfig 
' method demonstrates the former, and the AccessRemotingServerByCode method 
' illustrates the latter method.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Imports directives"

Imports System.Runtime.Remoting
Imports System.Runtime.InteropServices
Imports System.Runtime.Serialization.Formatters
Imports System.Runtime.Remoting.Channels
Imports System.Runtime.Remoting.Channels.Tcp
Imports System.Net.Sockets

Imports RemotingShared

#End Region


Module MainModule

    Sub Main(ByVal args As String())
        If ((args.Length > 0) AndAlso (args(0).StartsWith("-") OrElse args(0).StartsWith("/"))) Then
            Dim cmd As String = args(0).Substring(1)

            If (cmd.Equals("configfile", StringComparison.OrdinalIgnoreCase)) Then
                ' Access the .NET Remoting server using a configuration file.
                AccessRemotingServerByConfig()
            ElseIf (cmd.Equals("code", StringComparison.OrdinalIgnoreCase)) Then
                ' Access the .NET Remoting server using code.
                AccessRemotingServerByCode()
            Else
                PrintInstructions()
            End If
        Else
            ' By default, access the .NET Remoting server using a config file.
            AccessRemotingServerByConfig()
        End If
    End Sub


    Sub PrintInstructions()
        Console.WriteLine("VBRemotingClient Instructions:")
        Console.WriteLine("Access the .NET Remoting server")
        Console.WriteLine(" -configfile  using a config file")
        Console.WriteLine(" -code        using code")
    End Sub


#Region "Access the .NET Remoting server using a configuration file."

    ''' <summary>
    ''' Access the .NET Remoting server using a configuration file.
    ''' </summary>
    Sub AccessRemotingServerByConfig()

        ' Reads the configuration file and configures the remoting 
        ' infrastructure for the client project.

        ' The format for .NET Remoting configuration file:
        ' http://msdn.microsoft.com/en-us/library/ms973907.aspx
        RemotingConfiguration.Configure("VBRemotingClient.exe.config", True)

        Try
            '
            ' Create a remotable object.
            '

            ' Create a SingleCall server-activated object
            Dim remoteObj As New SingleCallObject()
            Console.WriteLine("A SingleCall server-activated proxy is created")

            ' [-or-] Create a Singleton server-activated object
            'Dim remoteObj As New SingletonObject()
            'Console.WriteLine("A Singleton server-activated proxy is created")

            ' [-or-] Create a client-activated object
            'Dim remoteObj As New ClientActivatedObject()
            'Console.WriteLine("A client-activated object is created")

            '
            ' Use the remotable object as if it were a local object.
            '

            Dim remoteType As String = remoteObj.GetRemoteObjectType()
            Console.WriteLine("Call GetRemoteObjectType => {0}", remoteType)

            Console.WriteLine("The client process and thread: {0}, {1}", _
                Process.GetCurrentProcess.Id, GetCurrentThreadId())

            Dim processId, threadId As UInteger
            remoteObj.GetProcessThreadID(processId, threadId)
            Console.WriteLine("Call GetProcessThreadID => {0} {1}", processId, threadId)

            Console.WriteLine("Set FloatProperty += {0}", 1.2F)
            remoteObj.FloatProperty += 1.2F

            Console.WriteLine("Get FloatProperty = {0}", remoteObj.FloatProperty)

        Catch ex As SocketException
            Console.WriteLine(ex.Message)
        End Try
        
    End Sub

#End Region


#Region "Access the .NET Remoting server using code."

    ''' <summary>
    ''' Access the .NET Remoting server using code.
    ''' </summary>
    Sub AccessRemotingServerByCode()

        ' Create and register a channel (TCP channel in this example) that 
        ' is used to transport messages across the remoting boundary.

        ' Set the properties of the channel.
        Dim props As New Hashtable()
        props("typeFilterLevel") = TypeFilterLevel.Full

        ' Set the formatters of the messages for delivery.
        Dim clientProvider As New BinaryClientFormatterSinkProvider()
        Dim serverProvider As New BinaryServerFormatterSinkProvider()
        serverProvider.TypeFilterLevel = TypeFilterLevel.Full

        ' Create a TCP channel.
        Dim tcpChannel As New TcpChannel(props, clientProvider, serverProvider)

        ' Register the TCP channel.
        ChannelServices.RegisterChannel(tcpChannel, True)

        Try
            '
            ' Create a remotable object.
            '

            ' Create a SingleCall server-activated proxy.
            Dim remoteObj As SingleCallObject = Activator.GetObject( _
                GetType(SingleCallObject), _
                "tcp://localhost:6100/SingleCallService")
            Console.WriteLine("A SingleCall server-activated proxy is created")

            ' [-or-] a Singleton server-activated object
            'Dim remoteObj As SingletonObject = Activator.GetObject( _
            '    GetType(SingletonObject), _
            '    "tcp://localhost:6100/SingletonService")
            'Console.WriteLine("A Singleton server-activated proxy is created")

            ' [-or-] a client-activated object
            'RemotingConfiguration.RegisterActivatedClientType( _
            '    GetType(ClientActivatedObject), _
            '    "tcp://localhost:6100/RemotingService")
            'Dim remoteObj As New ClientActivatedObject()
            'Console.WriteLine("A client-activated object is created")

            '
            ' Use the remotable object as if it were a local object.
            '

            Dim remoteType As String = remoteObj.GetRemoteObjectType()
            Console.WriteLine("Call GetRemoteObjectType => {0}", remoteType)

            Console.WriteLine("The client process and thread: {0}, {1}", _
                Process.GetCurrentProcess.Id, GetCurrentThreadId())

            Dim processId, threadId As UInteger
            remoteObj.GetProcessThreadID(processId, threadId)
            Console.WriteLine("Call GetProcessThreadID => {0} {1}", processId, threadId)

            Console.WriteLine("Set FloatProperty += {0}", 1.2F)
            remoteObj.FloatProperty += 1.2F

            Console.WriteLine("Get FloatProperty = {0}", remoteObj.FloatProperty)

        Catch ex As SocketException
            Console.WriteLine(ex.Message)
        End Try
        
    End Sub

#End Region


    ''' <summary>
    ''' Get current thread ID.
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <DllImport("kernel32.dll")> _
    Friend Function GetCurrentThreadId() As UInteger
    End Function

End Module
