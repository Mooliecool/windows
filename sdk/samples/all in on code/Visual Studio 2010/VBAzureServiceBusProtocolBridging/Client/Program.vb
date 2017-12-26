'****************************** Module Header ******************************\
' Project Name:   CSAzureServiceBusProtocolBridging
' Module Name:    Client
' File Name:      Program.vb
' Copyright (c) Microsoft Corporation
'
' This console application consumes the WCF service through Service Bus
' https://{namespace}.servicebus.windows.net/MyService.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Imports Common

Module Program

    Sub Main()
        Dim random As New Random()
        Dim number1 As Integer = random.[Next](100)
        Dim number2 As Integer = random.[Next](100)

        Try
            Console.WriteLine("Initializing proxy.")
            Using channelFactory As New MyChannelFactory("ServiceBusRouterService", Settings.ServiceEndpoint)
                Console.WriteLine("Calling Add({0}, {1}) via {2}", number1, number2, channelFactory.EndpointAddress)
                Dim result As Integer = channelFactory.MyService.Add(number1, number2)

                Console.WriteLine("Result: {0}", result)
            End Using
        Catch ex As Exception
            Console.WriteLine(ex.Message)
        End Try

        Console.WriteLine("Please press [Enter] to exit.")
        Console.ReadLine()
    End Sub

End Module
