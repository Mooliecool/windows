' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.ServiceModel
Imports System.Configuration

Namespace Microsoft.ServiceModel.Samples

    Class Host

        ' Host the service within this EXE console application.
        Public Shared Sub Main()

            ' Create a ServiceHost for the StreamingService type
            Using serviceHost As New ServiceHost(GetType(StreamingService))

                ' Open the ServiceHostBase to create listeners and start listening for messages.
                serviceHost.Open()

                ' The service can now be accessed.
                Console.WriteLine("The streaming service is ready.")
                Console.WriteLine("Press <ENTER> to terminate service.")
                Console.WriteLine()
                Console.ReadLine()

            End Using

        End Sub

    End Class

End Namespace