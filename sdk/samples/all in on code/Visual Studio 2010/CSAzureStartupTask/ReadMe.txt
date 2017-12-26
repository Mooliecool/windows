=============================================================================
          APPLICATION : CSAzureStartupTask Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary: 

You can use Startup element to specify tasks to configure your role environment. 
Applications that are deployed on Windows Azure usually have a set of prerequisites 
that must be installed on the host computer. You can use the start-up tasks to 
install the prerequisites or to modify configuration settings for your environment. 
Web and worker roles can be configured in this manner.

/////////////////////////////////////////////////////////////////////////////
Prerequisite:

，	IIS 7 (with ASP.NET, WCF HTTP Activation)
，	Microsoft .NET Framework 4.0
，	Microsoft Visual Studio 2010
，	Windows Azure Tools for Microsoft Visual Studio 1.4


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the CSAzureStartup sample.

The worker role hosts a WCF service, which provides reverse string service via 
endpoint http://+:81/reversestring . 
When you deploy this service to Azure without specifying the startup task in the 
sample, you will find the role instances keep busy and the WCF service doesn't start.
The following exception occurs when worker role calls ServiceHost.Open()

Exception object: 00000000064312f8
Exception type:   System.ServiceModel.AddressAccessDeniedException
Message:          HTTP could not register URL http://+:81/Service/. 
				  Your process does not have access rights to this namespace
				  (see http://go.microsoft.com/fwlink/?LinkId=70353 for details).
InnerException:   System.Net.HttpListenerException, Use !PrintException 0000000006417cd8 to see more.
StackTrace (generated):
    SP               IP               Function
    0000000024767DC0 000007FF0078A44C system_servicemodel!System.ServiceModel.Channels.SharedHttpTransportManager.OnOpen()+0x82c
    000000002476E110 000007FF007896A6 system_servicemodel!System.ServiceModel.Channels.TransportManager.Open(System.ServiceModel.Channels.TransportChannelListener)+0x336
    000000002476E1C0 000007FF00786FE9 system_servicemodel!System.ServiceModel.Channels.TransportManagerContainer.Open(System.ServiceModel.Channels.SelectTransportManagersCallback)+0x79
    000000002476E230 000007FF00786F1F system_servicemodel!System.ServiceModel.Channels.HttpChannelListener.OnOpen(System.TimeSpan)+0x14f
    000000002476E2B0 000007FF0052C134 system_servicemodel!System.ServiceModel.Channels.CommunicationObject.Open(System.TimeSpan)+0x2f4
    000000002476E3D0 000007FF007863F7 system_servicemodel!System.ServiceModel.Dispatcher.ChannelDispatcher.OnOpen(System.TimeSpan)+0xc7
    000000002476E420 000007FF0052C134 system_servicemodel!System.ServiceModel.Channels.CommunicationObject.Open(System.TimeSpan)+0x2f4
    000000002476E540 000007FF0052F389 system_servicemodel!System.ServiceModel.ServiceHostBase.OnOpen(System.TimeSpan)+0xb9
    000000002476E5A0 000007FF0052C134 system_servicemodel!System.ServiceModel.Channels.CommunicationObject.Open(System.TimeSpan)+0x2f4
    000000002476E6C0 000007FF005088E9 workerrole1!WorkerRole1.WorkerRole.StartService(Int32)+0x4b9
In Windows Azure, every HTTP path is reserved for use by the system administrator by default. 
The WCF services will fail to start with an AddressAccessDeniedException if it isn't 
running from an elevated account. Windows 2003 includes a tool called httpcfg.exe that 
lets the owner of an HTTP namespace delegate that ownership to another user. In Windows 
Azure, httpcfg.exe is no longer included and instead there's a new command set available 
through netsh.exe.

The sample has a startup task running elevated 
    <Startup>
      <Task commandLine="Startup\HttpUrl.cmd" executionContext="elevated" taskType="simple" />
    </Startup>
and call the following on command to open the access to port 81:
netsh http add urlacl url=http://+:81/ReverseString user=everyone listen=yes delegate=yes

There is another option to this problem. You can change the binding property HostNameComparisonMode 
as Exact. You can do it either in code or in .config file like below:
new BasicHttpBinding
{ 
  HostNameComparisonMode = HostNameComparisonMode.Exact 
};

Or in config:
<bindings>
        <basicHttpBinding>
          <binding name="basicHttp"
                   hostNameComparisonMode="Exact" />
        </basicHttpBinding>
      </bindings>

After re-deploy the project, create a client application, adding the service reference to reverse 
string service. 
You can add code in the client as below to consume the WCF service
	TestWCFServiceClient proxy = new TestWCFServiceClient();		
	string s = proxy.ReverseString("Hello World");
	Console.WriteLine(s);

/////////////////////////////////////////////////////////////////////////////
Implementation

Step1. Create a WCF reverse string service.
	[ServiceContract]
	public interface ITestWCFService
	{
		[OperationContract]
		string ReverseString(string s);
	}
	public class TestWCFService : ITestWCFService
	{
		#region ITest Members

		public string ReverseString(string s)
		{
			char[] arr = s.ToCharArray();
			Array.Reverse(arr);
			return new string(arr);
		}

		#endregion
	}


Step2. Create a Windows Azure Worker Role project and add an Endpoint to open port 81 for HTTP protocol.
<Endpoints>
      <InputEndpoint name="HttpIn" protocol="http" port="81" />
</Endpoints>

Step3. Create the following 2 methods to start and stop service
private void StartService(Int32 retries)
		{
			if (retries == 0)
			{
				RoleEnvironment.RequestRecycle();
				return;
			}

			Uri httpUri = new Uri(String.Format("http://{0}/{1}",
									RoleEnvironment.CurrentRoleInstance.InstanceEndpoints["HttpIn"].IPEndpoint.ToString(),
									"ReverseString"));


			serviceHost = new ServiceHost(typeof(TestWCFService), httpUri);
			
			serviceHost.Faulted += (sender, e) =>
				{
					Trace.TraceError("Host fault occured. Aborting and restarting the host. Retry count: {0}", retries);

					serviceHost.Abort();
					StartService(--retries);
				};

			try
			{
				Trace.TraceInformation("Trying to open service host");
				serviceHost.Open();
				Trace.TraceInformation("Service host started successfully.");
				
			}
			catch (TimeoutException timeoutException)
			{
				Trace.TraceError("The service operation time out. {0}",
							timeoutException.Message);

			}
			catch (CommunicationException communicationException)
			{
				Trace.TraceError("Could not start service host. {0}",
					 communicationException.Message);
			}
		}

		private void StopService()
		{
			if (serviceHost != null)
			{
				try
				{
					serviceHost.Close();
				}
				catch (TimeoutException timeoutException)
				{
					Trace.TraceError("The service close time out. {0}",
								timeoutException.Message);

					serviceHost.Abort();
				}
				catch (CommunicationException communicationException)
				{
					Trace.TraceError("Could not close service host. {0}",
						 communicationException.Message);

					serviceHost.Abort();
				}
			}
		}
Step4. Call StartService() in Worker Role's OnStart() method like below
		public override bool OnStart()
		{
			// Set the maximum number of concurrent connections 
			ServicePointManager.DefaultConnectionLimit = 12;

			StartService(3);

			// For information on handling configuration changes
			// see the MSDN topic at http://go.microsoft.com/fwlink/?LinkId=166357.
			RoleEnvironment.Changing += RoleEnvironmentChanging;

			return base.OnStart();
		}
Step5. Call StopService() in Worker Role's OnStop() method like below
		public override void OnStop()
		{
			StopService();
			base.OnStop();
		}
Step6. Add a startup task
<Startup>
      <Task commandLine="Startup\HttpUrl.cmd" executionContext="elevated" taskType="simple" />
</Startup>
In HttpUrl.cmd reserve port 81 for every one.
netsh http add urlacl url=http://+:81/ReverseString user=everyone listen=yes delegate=yes


Step7. Build and deploy the project to Windows Azure.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: How to Define Startup Tasks for a Role
http://msdn.microsoft.com/en-us/library/gg456327.aspx

/////////////////////////////////////////////////////////////////////////////