' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.Collections.Generic
Imports System.ServiceModel
Imports System.ServiceModel.Description
Imports System.ServiceModel.Web
Imports System.Text
Namespace Microsoft.ServiceModel.Samples.BasicWebProgramming

	Class Program
        Shared Sub Main(ByVal args As String())
            Using host As New WebServiceHost(GetType(Service), New Uri("http://localhost:8000/"))
                host.AddServiceEndpoint(GetType(IService), New WebHttpBinding(), "")
                host.Open()

                Using cf As New ChannelFactory(Of IService)(New WebHttpBinding(), "http://localhost:8000")
                    cf.Endpoint.Behaviors.Add(New WebHttpBehavior())
                    Dim channel As IService = cf.CreateChannel()

                    Dim s As String

                    Console.WriteLine("Calling EchoWithGet via HTTP GET: ")
                    s = channel.EchoWithGet("Hello, world")
                    Console.WriteLine("   Output: {0}", s)

                    Console.WriteLine("")
                    Console.WriteLine("This can also be accomplished by navigating to")
                    Console.WriteLine("http://localhost:8000/EchoWithGet?s=Hello, world!")
                    Console.WriteLine("in a web browser while this sample is running.")

                    Console.WriteLine("")

                    Console.WriteLine("Calling EchoWithPost via HTTP POST: ")
                    s = channel.EchoWithPost("Hello, world")
                    Console.WriteLine("   Output: {0}", s)

                    Console.WriteLine("")
                End Using


                Console.WriteLine("Press any key to terminate")
                Console.ReadLine()
            End Using
        End Sub
	End Class
End Namespace