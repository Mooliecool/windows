' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Security.Principal
Imports System.Security.Cryptography.X509Certificates
Imports System.Configuration
Imports System.Runtime.Serialization
Imports System.Xml.Serialization
Imports System.ServiceModel
Imports System.ServiceModel.Security
Imports System.ServiceModel.Dispatcher
Imports System.ServiceModel.Description
Imports System.ServiceModel.Channels

Namespace Microsoft.ServiceModel.Samples

    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Interface ICalculator

        <OperationContract()> _
        Function Add(ByVal n1 As Double, ByVal n2 As Double) As Double
        <OperationContract()> _
        Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As Double
        <OperationContract()> _
        Function Multiply(ByVal n1 As Double, ByVal n2 As Double) As Double
        <OperationContract()> _
        Function Divide(ByVal n1 As Double, ByVal n2 As Double) As Double

    End Interface

    ' Service class which implements the service contract.
    ' Added code to write output to the console window
    <ServiceBehavior(IncludeExceptionDetailInFaults:=True)> _
    Class CalculatorService
        Implements ICalculator

        Public Function Add(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculator.Add

            Dim result As Double = n1 + n2
            Return result

        End Function

        Public Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculator.Subtract

            Dim result As Double = n1 - n2
            Return result

        End Function

        Public Function Multiply(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculator.Multiply

            Dim result As Double = n1 * n2
            Return result

        End Function

        Public Function Divide(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculator.Divide

            Dim result As Double = n1 / n2
            Return result

        End Function

        Public Shared Sub Main(ByVal args As String())

            'Create a ServiceHost for the CalculatorService type
            Using serviceHost As New ServiceHost(GetType(CalculatorService))

                'Create and configure the WseHttpBinding
                Dim binding As New WseHttpBinding()
                binding.SecurityAssertion = WseSecurityAssertion.AnonymousForCertificate
                binding.EstablishSecurityContext = True
                binding.RequireDerivedKeys = True
                binding.MessageProtectionOrder = MessageProtectionOrder.SignBeforeEncrypt

                serviceHost.AddServiceEndpoint(GetType(ICalculator), binding, "")
                Dim creds As New ServiceCredentials()
                creds.ServiceCertificate.SetCertificate(StoreLocation.LocalMachine, StoreName.My, X509FindType.FindBySubjectName, "localhost")
                serviceHost.Description.Behaviors.Add(creds)
                serviceHost.Open()

                ' The service can now be accessed.
                Console.WriteLine("Press enter to terminate the service")

                For i As Integer = 0 To serviceHost.ChannelDispatchers.Count - 1

                    ' Uncomment the following to see the account the service is running under
                    'Console.WriteLine("The service is running in the following account: {0}", WindowsIdentity.GetCurrent().Name);
                    Dim channelDispatcher As ChannelDispatcher = TryCast(serviceHost.ChannelDispatchers(i), ChannelDispatcher)
                    If channelDispatcher IsNot Nothing Then

                        For j As Integer = 0 To channelDispatcher.Endpoints.Count - 1

                            Dim endpointDispatcher As EndpointDispatcher = channelDispatcher.Endpoints(j)
                            Console.WriteLine("The service is listening on {0}", endpointDispatcher.EndpointAddress.Uri)

                        Next

                    End If

                Next

                Console.WriteLine("The service is ready")
                Console.ReadLine()

            End Using

        End Sub

    End Class

End Namespace
