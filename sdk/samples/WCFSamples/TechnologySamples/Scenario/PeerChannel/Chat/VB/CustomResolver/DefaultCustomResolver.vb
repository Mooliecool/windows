' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.ServiceModel
Imports System.ServiceModel.Channels
Imports System.ServiceModel.PeerResolvers
Imports System.Configuration

Namespace Microsoft.ServiceModel.Samples

    Class DefaultCustomResolver

        Public Shared Sub Main()

            ' Create a new resolver service
            Dim crs As New CustomPeerResolverService()
            crs.ControlShape = False

            ' Create a new service host
            Dim customResolver As New ServiceHost(crs)

            ' Open the custom resolver service and wait for user to press enter
            crs.Open()
            customResolver.Open()
            Console.WriteLine("Custom resolver service is started")
            Console.WriteLine("Press <ENTER> to terminate service")
            Console.ReadLine()

        End Sub

    End Class

End Namespace
