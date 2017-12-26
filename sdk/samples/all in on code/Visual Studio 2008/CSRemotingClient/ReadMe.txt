=============================================================================
         CONSOLE APPLICATION : CSRemotingClient Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

.NET remoting provides an abstract approach to interprocess communication 
that separates the remotable object from a specific client or server 
application domain and from a specific mechanism of communication. 

.NET remoting allows an application to make a remotable object available 
across remoting boundaries, which includes different appdomains, processes or 
even different computers connected by a network. .NET Remoting makes a 
reference of a remotable object available to a client application, which then 
instantiates and uses a remotable object as if it were a local object. 
However, the actual code execution happens at the server-side. Any requests 
to the remotable object are proxied by the .NET Remoting runtime over Channel 
objects, that encapsulate the actual transport mode, including TCP streams, 
HTTP streams and named pipes. As a result, by instantiating proper Channel 
objects, a .NET Remoting application can be made to support different 
communication protocols without recompiling the application. The runtime 
itself manages the act of serialization and marshalling of objects across the 
client and server appdomains.

CSRemotingClient is a .NET Remoting client project. It accesses the remote 
objects (SingleCall objects or Singleton objects or client-activated objects) 
exposed by the .NET Remoting server project, CSRemotingServer.

There are generally two ways to create the .NET Remoting client: using a 
configuration file or writing codes. The AccessRemotingServerByConfig method 
demonstrates the former and the AccessRemotingServerByCode method illustrates 
the latter method.


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the .NET remoting sample.

Step1. After you successfully build the CSRemotingClient and CSRemotingServer 
sample projects in Visual Studio 2008, you will get the console applications: 
CSRemotingClient.exe and CSRemotingServer.exe. 

Step2. Run CSRemotingServer.exe in a command prompt to start up the server 
project. The command 'CSRemotingServer.exe -configfile' creates and 
configures the .NET Remoting server using a configuration file. The command 
'CSRemotingServer.exe -code' uses code to create the .NET Remoting server 
with the same configuration. You can choose either one.

Because CSRemotingServer uses a TCP channel, you may be prompted that Windows 
Firewall has blocked some features of the application. You can safely allow 
the access when you see the Windows Firewall dialog. 

Step3. Run CSRemotingClient.exe in another command prompt to start up the 
client project. Similiarly, the command 'CSRemotingClient.exe -configfile' 
reads a configuration file and configure the remoting infrastructure for the 
client project to connect to the .NET Remoting server (CSRemotingServer). The 
command 'CSRemotingClient.exe -code' uses code to connect to the server with 
the same configuration. You can choose either one.

By default, CSRemotingClient creates a SingleCall server-activated object, 
and invokes its methods and sets properties.

    A SingleCall server-activated object is created
    Call GetRemoteObjectType => SingleCallObject
    The client process and thread: 6852, 4732
    Call GetProcessThreadID => 8092 8080
    Set FloatProperty += 1.2
    Get FloatProperty = 0

SingleCallObject is a server-activated object (SAO) with the "SingleCall" 
instancing mode. Such objects are created on each method call and objects are 
not shared among clients. State is not maintained in such objects because 
they are destroyed after each method call. Therefore, in the above output, 
the FloatProperty of the SingleCall object was set to 1.2 
(Set FloatProperty += 1.2), but the next retrieval of the float property 
still returns 0.

You can try out other types of remoting objects (e.g. Singleton object, 
client-activate object) by uncommenting the corresponding code lines and 
rebuilding the client project.

    // Create a SingleCall server-activated object.
    SingleCallObject remoteObj = new SingleCallObject();
    Console.WriteLine("A SingleCall server-activated object is created");

    // [-or-] Create a Singleton server-activated object.
    //SingletonObject remoteObj = new SingletonObject();
    //Console.WriteLine("A Singleton server-activated object is created");

    // [-or-] Create a client-activated object.
    //ClientActivatedObject remoteObj = new ClientActivatedObject();
    //Console.WriteLine("A client-activated object is created");

Step4. Exit the remoting server by pressing ENTER in the server application. 


/////////////////////////////////////////////////////////////////////////////
Sample Relation:
(The relationship between the current sample and the rest samples in 
Microsoft All-In-One Code Framework http://1code.codeplex.com)

CSRemotingClient -> CSRemotingServer
CSRemotingClient is the client project of the CSRemotingServer server project.

CSRemotingClient -> CSRemotingSharedLibrary
CSRemotingClient references a shared library for the client-activated 
remoting types.


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Adding remotable types on the client project 

------
For client-activated types, they must be defined in an assembly shared by 
both client and server projects, because client-activated types require not 
only the same namespace/class name on both sides, but also the same assembly.

Step1. Add the reference to the .NET class library shared by the .NET 
Remoting server. The class library exposes the client-activated type 
(ClientActivatedObject) that inherits MarshalByRefObject.

------
For server-activated types, they can be either defined in a shared assembly, 
or defined in the server project and have en empty proxy of the type in the 
client projects. Please make sure that the server type and the proxy type have 
the same namespace/class name though it is not necessary to place them in the 
same assembly.

Step1. Add the proxy of the server-activated types (SingleCallObject, 
SingletonObject) that inherits MarshalByRefObject to the client project. 
There is no need to implement the body of the types.

B. Accessing .NET Remoting server using configuration file

Step1. Add an application configuration file to the project.

Step2. Define the channel to transport message.

<system.runtime.remoting>
	<application>
		<channels>
			<channel ref="tcp">
				<clientProviders>
					<formatter ref="binary" />
				</clientProviders>
				<serverProviders>
					<formatter ref="binary" typeFilterLevel="Full" />
				</serverProviders>
			</channel>
		</channels>
	</application>
</system.runtime.remoting>

The above configuration registers a TCP channel whose formatter is binary on 
both server and client sides.

Step3. Register the remotable types.

------
For client-activated types

<system.runtime.remoting>
	<application>
		<client url="tcp://localhost:6100/RemotingService">
			<activated type="RemotingShared.ClientActivatedObject, CSRemotingSharedLibrary">
			</activated>
		</client>
	</application>
</system.runtime.remoting>

The url attribute in <client> is only required for client-activated types. It 
specifies the URL that will be used to locate the activator for the attribute.
"tcp://localhost:6100/RemotingService" corresponds to the application name of 
the server project (<application name="RemotingService">).

------
For server-activated types

<system.runtime.remoting>
	<application>
		<client>
			<wellknown type="RemotingShared.SingleCallObject, CSRemotingClient"
                   url="tcp://localhost:6100/SingleCallService">
			</wellknown>
			<wellknown type="RemotingShared.SingletonObject, CSRemotingClient"
					   url="tcp://localhost:6100/SingletonService">
			</wellknown>
		</client>
	</application>
</system.runtime.remoting>

Step4. Read the configuration file and configure the remoting infrastructure 
for the client project. (RemotingConfiguration.Configure)

Step5. Create the remotable object.

	// Create a SingleCall server-activated object
	SingleCallObject remoteObj = new SingleCallObject();

	// [-or-] a Singleton server-activated object
	SingletonObject remoteObj = new SingletonObject();
	        
	// [-or-] a client-activated object
	ClientActivatedObject remoteObj = new ClientActivatedObject();

Step6. Use the remotable object as if it were a local object. For example,

	Console.WriteLine(remoteObj.GetRemoteObjectType());

C. Accessing .NET Remoting server using code

Step1. Specify the formatter of the messages for delivery. 
(BinaryClientFormatterSinkProvider, BinaryServerFormatterSinkProvider)
Once message has been formatted, it is transported to other application 
domains through the appropriate channel. .NET comes with the SOAP formatter 
(System.Runtime.Serialization.Formatters.Soap) and Binary formatter 
(System.Runtime.Serialization.Formatters.Binary).

Step2. Create and register the channel to transport message according to the 
channel on the server project. (TcpChannel/HttpChannel/IpcChannel, 
ChannelServices.RegisterChannel)

Step3. Create the remotable object.

------
For client-activated types

	RemotingConfiguration.RegisterActivatedClientType(
		typeof(ClientActivatedObject),
		"tcp://localhost:6100/RemotingService");
	ClientActivatedObject remoteObj = new ClientActivatedObject();

------
For server-activated types

	SingletonObject remoteObj = (SingletonObject)Activator.GetObject(
		typeof(SingletonObject),
		"tcp://localhost:6100/SingletonService");

Step4. Use the remotable object as if it were a local object. For example,

	Console.WriteLine(remoteObj.GetRemoteObjectType());


/////////////////////////////////////////////////////////////////////////////
References:

.NET Framework Remoting Architecture  
http://msdn.microsoft.com/en-us/library/2e7z38xb(VS.85).aspx

.NET Framework Remoting Overview  
http://msdn.microsoft.com/en-us/library/kwdt6w2k(VS.85).aspx


/////////////////////////////////////////////////////////////////////////////