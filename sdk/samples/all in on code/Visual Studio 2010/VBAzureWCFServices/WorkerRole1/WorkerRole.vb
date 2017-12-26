
Imports System
Imports System.Diagnostics
Imports System.Net
Imports System.Threading
Imports Microsoft.WindowsAzure
Imports Microsoft.WindowsAzure.Diagnostics
Imports Microsoft.WindowsAzure.ServiceRuntime
Imports Microsoft.WindowsAzure.StorageClient
Imports System.ServiceModel
Imports WCFContract

Public Class WorkerRole
    Inherits RoleEntryPoint
    Private serviceHost As ServiceHost
    Public Overrides Sub Run()

        StartWCFService()
        While True
            Thread.Sleep(10000)
            Trace.WriteLine("Working", "Information")
        End While

    End Sub

    Public Overrides Function OnStart() As Boolean
        CloudStorageAccount.SetConfigurationSettingPublisher(Sub(configName, configSetter) configSetter(RoleEnvironment.GetConfigurationSettingValue(configName)))


        ' Set the maximum number of concurrent connections 
        ServicePointManager.DefaultConnectionLimit = 12

        ' For information on handling configuration changes
        ' see the MSDN topic at http://go.microsoft.com/fwlink/?LinkId=166357.

        Return MyBase.OnStart()

    End Function
    Private Sub StartWCFService()

        Trace.TraceInformation("Starting WCF service host...")

        serviceHost = New ServiceHost(GetType(WCFService))

        ' Use NetTcpBinding with no security
        Dim binding As NetTcpBinding = New NetTcpBinding(SecurityMode.None)

        ' Define an external endpoint for client traffic
        Dim externalEndPoint As RoleInstanceEndpoint =
            RoleEnvironment.CurrentRoleInstance.InstanceEndpoints("External")
        serviceHost.AddServiceEndpoint(
           GetType(IContract),
           binding,
           String.Format("net.tcp://{0}/External", externalEndPoint.IPEndpoint))



        ' Define an internal endpoint for inter-role traffic

        Dim internalEndPoint As RoleInstanceEndpoint =
            RoleEnvironment.CurrentRoleInstance.InstanceEndpoints("Internal")
        serviceHost.AddServiceEndpoint(
           GetType(IContract),
           binding,
           String.Format("net.tcp://{0}/Internal", internalEndPoint.IPEndpoint))



        Try
            serviceHost.Open()
            Trace.TraceInformation("WCF service host started successfully.")

        Catch timeoutException As TimeoutException
            Trace.TraceError("The service operation timed out. {0}",
                                 timeoutException.ToString())
            
        Catch communicationException As CommunicationException
            Trace.TraceError("Could not start WCF service host. {0}",
                                 communicationException.ToString())
       
        End Try


    End Sub

End Class
