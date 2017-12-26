' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel.Description
Imports System.ServiceModel.Dispatcher
Imports System.Collections.Generic
Imports System.Text
Imports System.ServiceModel
Imports Microsoft.VisualBasic

Namespace Microsoft.ServiceModel.Samples

    Class Program

        Public Shared Sub Main(ByVal args As String())

            Dim host As New ServiceHost(GetType(CustomerService))
            host.Open()
            Console.WriteLine("Ready and waiting at:")

            For Each endpoint As ServiceEndpoint In host.Description.Endpoints

                Console.WriteLine(vbTab & endpoint.Address.Uri.ToString())

            Next
            Console.ReadLine()

            host.Close()

        End Sub

    End Class

End Namespace
