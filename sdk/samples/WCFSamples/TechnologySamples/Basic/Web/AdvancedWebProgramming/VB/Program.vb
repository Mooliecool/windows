' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.Collections
Imports System.Collections.Generic
Imports System.Runtime.Serialization
Imports System.ServiceModel
Imports System.ServiceModel.Description
Imports System.ServiceModel.Web
Imports System.Text
Namespace Microsoft.ServiceModel.Samples.AdvancedWebProgramming

	Class Program
        Shared Sub Main(ByVal args As String())
            Using host As New WebServiceHost(GetType(Service), New Uri("http://localhost:8000/"))
                host.AddServiceEndpoint(GetType(ICustomerCollection), New WebHttpBinding(), "Customers")
                host.Open()

                Dim baseAddress As New Uri("http://localhost:8000/Customers")

                Using cf As New ChannelFactory(Of ICustomerCollection)(New WebHttpBinding(), baseAddress.ToString())
                    cf.Endpoint.Behaviors.Add(New WebHttpBehavior())
                    Dim channel As ICustomerCollection = cf.CreateChannel()

                    Dim template As New UriTemplate("{id}")

                    Console.WriteLine("Adding some customers with POST:")

                    Dim alice As New Customer("Alice", "123 Pike Place", Nothing)
                    alice = channel.AddCustomer(alice)
                    Console.WriteLine(alice.ToString())

                    Dim bob As New Customer("Bob", "2323 Lake Shore Drive", Nothing)
                    bob = channel.AddCustomer(bob)
                    Console.WriteLine(bob.ToString())

                    Console.WriteLine("")
                    Console.WriteLine("Using PUT to update a customer")

                    alice.Name = "Charlie"

                    Dim match As UriTemplateMatch = template.Match(baseAddress, alice.Uri)
                    alice = channel.UpdateCustomer(match.BoundVariables("id"), alice)
                    Console.WriteLine(alice.ToString())

                    Console.WriteLine("")
                    Console.WriteLine("Using GET to retrieve the list of customers")

                    Dim customers As List(Of Customer) = channel.GetCustomers()

                    For Each c As Customer In customers
                        Console.WriteLine(c.ToString())
                    Next

                    Console.WriteLine("")
                    Console.WriteLine("Using DELETE to delete a customer")

                    match = template.Match(baseAddress, bob.Uri)
                    channel.DeleteCustomer(match.BoundVariables("id"))

                    Console.WriteLine("")
                    Console.WriteLine("Final list of customers: ")

                    customers = channel.GetCustomers()

                    For Each c As Customer In customers
                        Console.WriteLine(c.ToString())
                    Next


                    Console.WriteLine("")
                End Using


                Console.WriteLine("Press any key to terminate")
                Console.ReadLine()
            End Using
        End Sub
	End Class
End Namespace