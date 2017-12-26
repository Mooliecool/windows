Imports System.ServiceModel

Module Program

    Sub Main()
        Console.WriteLine("Starting service.")
        Using host As New ServiceHost(New MyService())
            host.Open()

            Console.WriteLine("Service is ready at {0}", host.Description.Endpoints(0).Address.ToString())

            Console.WriteLine("Please press [Enter] to exit.")
            Console.ReadLine()
        End Using
    End Sub

End Module
