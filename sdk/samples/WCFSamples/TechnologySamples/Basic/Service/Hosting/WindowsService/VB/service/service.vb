' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ComponentModel
Imports System.Configuration
Imports System.Configuration.Install
Imports System.ServiceModel
Imports System.ServiceProcess

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface ICalculator

        <OperationContract()> _
        Function Add(ByVal n1 As Double, ByVal n2 As Double) As Double
        <OperationContract()> _
        Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As Double
        <OperationContract()> _
        Function Multiply(ByVal n1 As Double, ByVal n2 As Double) As Double
        <OperationContract()> _
        Function Divide(ByVal n1 As Double, ByVal n2 As Double) As Double

    End Interface

    <RunInstaller(True)> _
    Public Class ProjectInstaller
        Inherits Installer

        Private process As ServiceProcessInstaller
        Private service As ServiceInstaller

        Public Sub New()

            process = New ServiceProcessInstaller()
            process.Account = ServiceAccount.LocalSystem
            service = New ServiceInstaller()
            service.ServiceName = "WCFWindowsServiceSample"
            Installers.Add(process)
            Installers.Add(service)

        End Sub

    End Class

    ' Service class which implements the service contract.

    Public Class WindowsCalculatorService
        Inherits ServiceBase

        Public serviceHost As ServiceHost = Nothing

        Public Shared Sub Main()

            ServiceBase.Run(New WindowsCalculatorService())

        End Sub

        Public Sub New()

            ServiceName = "WCFWindowsServiceSample"

        End Sub

        'Start the Windows service.
        Protected Overloads Overrides Sub OnStart(ByVal args As String())

            If serviceHost IsNot Nothing Then

                serviceHost.Close()

            End If

            ' Create a ServiceHost for the CalculatorService type and provide the base address.
            serviceHost = New ServiceHost(GetType(WcfCalculatorService))

            ' Open the ServiceHostBase to create listeners and start listening for messages.
            serviceHost.Open()

        End Sub

        ' Stop the Windows service.
        Protected Overloads Overrides Sub OnStop()

            If serviceHost IsNot Nothing Then

                serviceHost.Close()
                serviceHost = Nothing

            End If

        End Sub
    End Class

    Public Class WcfCalculatorService
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
    End Class

End Namespace
