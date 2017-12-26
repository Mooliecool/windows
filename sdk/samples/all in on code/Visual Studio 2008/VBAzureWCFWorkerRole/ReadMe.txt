========================================================================
    CLOUD SERVICE : VBAzureWCFWorkerRole Solution Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

In some scenarios we need self-hosting WCF services.If we want to create an HTTP based
service, we should use web role. For a TCP based WCF service however, worker role is a
better choice. Due to the existance of load balancer we need to take care of the logic
and physical listening address. The purpose of this sample is to provide a handy working project
that hosts WCF in a Worker Role.

This solution contains three projects:

1. Client project. It's the client application that consumes WCF services.
2. CloudService project. It's a common Cloud Service that has one Worker Role.
3. VBWorkerRoleHostingWCF project. It's the key project in the solution, which demonstrates
how to host WCF in a Worker Role.

Two endpoints are exposed from the WCF service in VBWorkerRoleHostingWCF project:
1. A metadata endpoint
2. A service endpoint for MyServiceMetaDataEndpoint service contract

Both endpoints uses TCP bindings.


/////////////////////////////////////////////////////////////////////////////
Pre-requests:
Windows Azure Tools for Microsoft Visual Studio
http://www.microsoft.com/downloads/en/details.aspx?FamilyID=7a1089b6-4050-4307-86c4-9dadaa5ed018


/////////////////////////////////////////////////////////////////////////////
Code Logic of VBWorkerRoleHostingWCF project:

1. Get local ip address and local listening port of Virtual Machine:

Dim ip As String = RoleEnvironment.CurrentRoleInstance.InstanceEndpoints("tcpinput").IPEndpoint.Address.ToString()
Dim tcpport As Integer = RoleEnvironment.CurrentRoleInstance.InstanceEndpoints("tcpinput").IPEndpoint.Port
Dim mexport As Integer = RoleEnvironment.CurrentRoleInstance.InstanceEndpoints("mexinput").IPEndpoint.Port

2. Add a metadata TCP endpoint. The logical listening port is 8001. Client should use this port to request metadata.
The physical port is the mexport we got in step 1.

Dim metadatabehavior As ServiceMetadataBehavior = New ServiceMetadataBehavior()
host.Description.Behaviors.Add(metadatabehavior)
Dim mexBinding As Binding = MetadataExchangeBindings.CreateMexTcpBinding()
Dim mexlistenurl As String = String.Format("net.tcp://{0}:{1}/MyServiceMetaDataEndpoint", ip, mexport)
Dim mexendpointurl As String = String.Format("net.tcp://{0}:{1}/MyServiceMetaDataEndpoint", RoleEnvironment.GetConfigurationSettingValue("Domain"), 8001)
host.AddServiceEndpoint(GetType(IMetadataExchange), mexBinding, mexendpointurl, New Uri(mexlistenurl))

3. Add a TCP endpoint for MyServiceMetaDataEndpoint.The logical listening port is 9001. Client should use this port to send request.
The physical port is the tcpport we got in step 1.

Dim listenurl As String = String.Format("net.tcp://{0}:{1}/MyServiceEndpoint ", ip, tcpport)
Dim endpointurl As String = String.Format("net.tcp://{0}:{1}/MyServiceEndpoint ", RoleEnvironment.GetConfigurationSettingValue ("Domain"), 9001)
host.AddServiceEndpoint(GetType(IMyServiceMetaDataEndpoint), New NetTcpBinding(SecurityMode.None), endpointurl, New Uri(listenurl))


/////////////////////////////////////////////////////////////////////////////
Demo:

A. In Compute Emulator:

1. Set CloudService project as Startup project.
2. Press F5 to start debugging.
3. Run Client.exe in Client project or debug Client project.

Note if you want to create your own proxy class, When you add service reference in your client project, 
the metadata endpoint you input should be net.tcp://localhost:8001/MyServiceMetaDataEndpoint.

B. After deployment to cloud:

1.Please change the setting in ServiceConfiguration.cscfg of CloudService project to:

    <Setting name="Domain" value="[yourdomain.cloudapp.net]" />

2.Please change the setting in app.config of Client project to:

<client>
            <endpoint address="net.tcp://[yourdomain.cloudapp.net]:9001/MyServiceEndpoint " binding="netTcpBinding"
                bindingConfiguration="NetTcpBinding_IMyServiceMetaDataEndpoint" contract="ServiceReference1.IMyServiceMetaDataEndpoint"
                name="NetTcpBinding_IMyServiceMetaDataEndpoint" />
</client>

Note the metadata endpoint should be net.tcp://[yourdomain.cloudapp.net]:8001/MyServiceMetaDataEndpoint.
    
/////////////////////////////////////////////////////////////////////////////
References:

Service Definition Schema
http://msdn.microsoft.com/en-us/library/ee758711.aspx


/////////////////////////////////////////////////////////////////////////////