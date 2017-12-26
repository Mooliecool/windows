========================================================================
    LIBRARY APPLICATION : CSRegFreeCOMClient Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

Registration-free COM is a mechanism available on the Microsoft Windows XP 
(SP2 for .NET Framework-based components), Microsoft Windows Server 2003 and 
newer platforms. As the name suggests, the mechanism enables easy (e.g. 
XCOPY) deployment of COM components to a machine without the need to register 
them.

The CSRegFreeCOMClient sample demonstrates how to create a registration-free 
COM from the aspect of .NET Framework based client, so that the client can 
consume existing COM server as if the COM server is Registration-free.


/////////////////////////////////////////////////////////////////////////////
Sample Relation:
(Relation of the current sample and other samples in 
Microsoft All-In-One Code Framework http://cfx.codeplex.com)

	CSRegFreeCOMClient -> ATLDllCOMServer


/////////////////////////////////////////////////////////////////////////////
Creation:

A. The client automatically searches manifest file of its dependency, 
and use the manifest file to Activate the activation context.

	Step1. 
	Add a reference to the ATLDllCOMServer COM component by right-clicking 
	the project, selecting "Add Reference...", turning to the COM tab, and 
	adding ATLDllCOMServer Type Library. Visual Studio will automatically 
	generate an interop assembly "Interop.ATLDllCOMServerLib.dll". If the
	ATLDllCOMServer Type Library is not in the list, please build and setup 
	the ATLDllCOMServer sample first.

	Step2. 
	Create a thread whose thread apartement is set to STA, because we are 
	going to demonstrate the instantiation and use of a STA COM object: 
	ATLDllCOMServer.SimpleObject.

	Step3. 
	Create method ConsumeCOMComponent which contains logic of consuming COM 
	server.

	Step4. 
	Add app.manifest file by right-clicking the project, selecting "Add" 
	--> "New Item" --> "General" --> "Application Manifest File" and click 
	Add button. 

	Step5. 
	Open the app.manifest file, find assemblyIdentity element, modify its 
	Name property to CSRegFreeCOMClient:
	
		<assemblyIdentity version="1.0.0.0" name="CSRegFreeCOMClient"/>
    
	Step6. 
	Add dependency element immediate after assemblyIdentity node:
	
		<dependency>
		<dependentAssembly>
		  <assemblyIdentity
					  type="win32"
					  name="ATLDllCOMServer.X"
					  version="1.0.0.0" />
		</dependentAssembly>
		</dependency>
	
	Step7. 
	Open property page of the app.manifest file by right-clicking the file 
	and selecting Property, change the value of Build Action dropdown to 
	Embedded Resource, and rebuild the application.

	Step8. 
	Open the application's output folder (Debug or Release folder), put a 
	copy of ATLDllCOMServer.dll to this folder,	create a txt file and rename 
	it (including file extention) to ATLDllCOMServer.X.manifest, input the 
	following content:

		<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
		<assembly xmlns="urn:schemas-microsoft-com:asm.v1"  manifestVersion="1.0">

			<assemblyIdentity
			   type="win32"
			   name="ATLDllCOMServer.X"
			   version="1.0.0.0" />
			   
			<file name = "ATLDllCOMServer.dll">
				<comClass clsid="{92FCF37F-F6C7-4F8A-AA09-1A14BA118084}"
					threadingModel = "Apartment" />
				    
				<typelib tlbid="{9B23EFED-A0C1-46B6-A903-218206447F3E}"
					   version="1.0" helpdir=""/>
			</file>

			<comInterfaceExternalProxyStub
				name="ISimpleObject"
				iid="{830F85D0-91B9-406D-A273-BC33133DD44B}"
				proxyStubClsid32="{00020424-0000-0000-C000-000000000046}"
				baseInterface="{00000000-0000-0000-C000-000000000046}"
				tlbid = "{9B23EFED-A0C1-46B6-A903-218206447F3E}" />
			    
		</assembly>
		
	Step9. 
	Unregistry the ATLDllCOMServer.dll, then run CSRegFreeCOMClient.exe.
	the output is:
	
		------Activate activation context automatically------
		Call HelloWorld => HelloWorld
		Press any key to continue...
		
B. The client activates activation context by specifying manifest file 
manually.

	Step1. 
	Add a reference to the ATLDllCOMServer COM component by right-clicking 
	the project, selecting "Add Reference...", turning to the COM tab, and 
	adding ATLDllCOMServer Type Library. Visual Studio will automatically 
	generate an interop assembly "Interop.ATLDllCOMServerLib.dll". If the 
	ATLDllCOMServer Type Library is not in the list, please build and setup 
	the ATLDllCOMServer sample first.

	Step2. 
	Create a thread whose thread apartement is set to STA, because we are 
	going to demonstrate the instantiation and use of a STA COM object: 
	ATLDllCOMServer.SimpleObject.

	Step3. 
	Create method ConsumeCOMComponent which contains logic of consuming COM 
	server.

	Step4. 
	Create the ActivateActivationContext method which is used to activate 
	the activation context according to given manifest file.

	Step5. 
	Add app.manifest file by right-clicking the project, selecting "Add" 
	--> "New Item" --> "General" --> "Application Manifest File" and click 
	"Add" button. 

	Step6. 
	Open the app.manifest file, find assemblyIdentity element, modify its 
	Name property to CSRegFreeCOMClient:
	
		<assemblyIdentity version="1.0.0.0" name="CSRegFreeCOMClient"/>
    
	Step7. 
	Add dependency element immediate after assemblyIdentity node:
		
		<dependency>
		<dependentAssembly>
		  <assemblyIdentity
					  type="win32"
					  name="ATLDllCOMServer.X"
					  version="1.0.0.0" />
		</dependentAssembly>
		</dependency>
		
	Step8. 
	Open property page of the app.manifest file by right-clicking the file 
	and select Property, change the value of Build Action dropdown to 
	Embedded Resource, and rebuild the application.

	Step9. 
	Create a new folder, for example, "D:\regfreecom", put a copy of 
	ATLDllCOMServer.dll to this folder, create a txt file and rename it 
	(including file extention) to ATLDllCOMServer.X.manifest, input following 
	content:

		<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
		<assembly xmlns="urn:schemas-microsoft-com:asm.v1"  manifestVersion="1.0">

			<assemblyIdentity
			   type="win32"
			   name="ATLDllCOMServer.X"
			   version="1.0.0.0" />
			   
			<file name = "ATLDllCOMServer.dll">
				<comClass clsid="{92FCF37F-F6C7-4F8A-AA09-1A14BA118084}"
					threadingModel = "Apartment" />
				    
				<typelib tlbid="{9B23EFED-A0C1-46B6-A903-218206447F3E}"
					   version="1.0" helpdir=""/>
			</file>

			<comInterfaceExternalProxyStub
				name="ISimpleObject"
				iid="{830F85D0-91B9-406D-A273-BC33133DD44B}"
				proxyStubClsid32="{00020424-0000-0000-C000-000000000046}"
				baseInterface="{00000000-0000-0000-C000-000000000046}"
				tlbid = "{9B23EFED-A0C1-46B6-A903-218206447F3E}" />
			    
		</assembly>
		
	Step10. 
	Unregistry the ATLDllCOMServer.dll, launch cmd.exe and navigate to the 
	folder where CSRegFreeCOMClient.exe exists, start CSRegFreeCOMClient.exe 
	with command:

		CSRegFreeCOMClient.exe someargs
		
	then, you will be required to input manifest file path:

		------ Activate activation context manually ------
		Please input the full path of manifest file:
		
	in this case, since we put the manifest file at D:\regfreecom, so we 
	input:

		D:\regfreecom\ATLDllCOMServer.X.manifest

	the full output is:

		------ Activate activation context manually ------
		Please input the full path of manifest file:
		D:\regfreecom\ATLDllCOMServer.X.manifest
		Call HelloWorld => HelloWorld
		Press any key to continue...


/////////////////////////////////////////////////////////////////////////////
FAQ:

1. How can I get the CLSID, tlbID, iid etc if it is a third-party dll?
To build a reg-free version for a given com dll, the key point it to 
construct a correct manifest file for the com dll, here is some tip for you:

   1. Make sure that the dll has no embedded manifest, if there is, a 
      separate manifest file will no take effect, manifest view can help to 
      verify whether a dll has embedded manifest.( but you can modify the 
      embedded manifest using some tools, anyway, it is not suggested).
      
   2. Create a manifest file for the dll, the file usually looks like:
   
		<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
		<assembly xmlns="urn:schemas-microsoft-com:asm.v1"
		  manifestVersion="1.0">

			<assemblyIdentity
			   type="win32"
			   name=" mycomdll.x"
			   version="1.0.0.0" />

			<file name = "mycomdll.dll">
			
				<comClass
					clsid="{[CLSID_ MyComClass]}"
					threadingModel = "Apartment" />

				<typelib tlbid="{[LIBID_ MyComClass]}"
					   version="1.0" helpdir=""/>
					   
			</file>

			<comInterfaceExternalProxyStub
				name=" ISomeInterface "
				iid="{[IID_I SomeInterface]}"
				proxyStubClsid32="{00020424-0000-0000-C000-000000000046}"
				baseInterface="{00000000-0000-0000-C000-000000000046}"
				tlbid = "{[LIBID_ MyComClass ]}" />
		    
		</assembly>

   3. To get values for placeholders in the manifest, we can use OLE/COM 
   ObjectViewer (Oleview.exe) tool. 
   
   Open Oleview.exe ( on Vista or Win7, you may run it as administrator),
   click File -> View TypeLib… --> select target com dll and click OK, 
   let's open ATLDllCOMServer.dll, and you will see:

		// Generated .IDL file (by the OLE/COM Object Viewer)
		// 
		// typelib filename: ATLDllCOMServer.dll

		[
		  uuid(9B23EFED-A0C1-46B6-A903-218206447F3E),
		  version(1.0),
		  helpstring("ATLDllCOMServer 1.0 Type Library"),
		  custom(DE77BA64-517C-11D1-A2DA-0000F8773CE9, 117441012),
		  custom(DE77BA63-517C-11D1-A2DA-0000F8773CE9, 1269332262),
		  custom(DE77BA65-517C-11D1-A2DA-0000F8773CE9, "Created by MIDL version 7.00.0500 at Tue Mar 23 16:17:41 2010
		")

		]
		library ATLDllCOMServerLib
		{
			// TLib :     // TLib : OLE Automation : {00020430-0000-0000-C000-000000000046}
			importlib("stdole2.tlb");

			// Forward declare all types defined in this typelib
			dispinterface _ISimpleObjectEvents;
			interface ISimpleObject;

			[
			  uuid(87AD6FBC-8735-407C-9758-C80B48C78E7C),
			  helpstring("_ISimpleObjectEvents Interface")
			]
			dispinterface _ISimpleObjectEvents {
				properties:
				methods:
					[id(0x00000001), helpstring("method FloatPropertyChanging")]
					void FloatPropertyChanging(
									[in] single NewValue, 
									[in, out] VARIANT_BOOL* Cancel);
			};

			[
			  uuid(92FCF37F-F6C7-4F8A-AA09-1A14BA118084),
			  helpstring("SimpleObject Class")
			]
			coclass SimpleObject {
				[default] interface ISimpleObject;
				[default, source] dispinterface _ISimpleObjectEvents;
			};

			[
			  odl,
			  uuid(830F85D0-91B9-406D-A273-BC33133DD44B),
			  helpstring("ISimpleObject Interface"),
			  dual,
			  nonextensible,
			  oleautomation
			]
			interface ISimpleObject : IDispatch {
				[id(0x00000001), propget, helpstring("property FloatProperty")]
				HRESULT FloatProperty([out, retval] single* pVal);
				[id(0x00000001), propput, helpstring("property FloatProperty")]
				HRESULT FloatProperty([in] single pVal);
				[id(0x00000002), helpstring("method HelloWorld")]
				HRESULT HelloWorld([out, retval] BSTR* pRet);
				[id(0x00000003), helpstring("method GetProcessThreadID")]
				HRESULT GetProcessThreadID(
								[out] long* pdwProcessId, 
								[out] long* pdwThreadId);
			};
		};

   6. With above information, we can fill in the manifest as following:

			<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
			<assembly xmlns="urn:schemas-microsoft-com:asm.v1"  manifestVersion="1.0">

				<assemblyIdentity
				   type="win32"
				   name="ATLDllCOMServer.X"
				   version="1.0.0.0" />
				   
				<file name = "ATLDllCOMServer.dll">
					<comClass clsid="{92FCF37F-F6C7-4F8A-AA09-1A14BA118084}"
						threadingModel = "Apartment" />
					    
					<typelib tlbid="{9B23EFED-A0C1-46B6-A903-218206447F3E}"
						   version="1.0" helpdir=""/>
				</file>

				<comInterfaceExternalProxyStub
					name="ISimpleObject"
					iid="{830F85D0-91B9-406D-A273-BC33133DD44B}"
					proxyStubClsid32="{00020424-0000-0000-C000-000000000046}"
					baseInterface="{00000000-0000-0000-C000-000000000046}"
					tlbid = "{9B23EFED-A0C1-46B6-A903-218206447F3E}" />
				    
			</assembly>


/////////////////////////////////////////////////////////////////////////////
References:

Registration-Free Activation of COM Components: A Walkthrough
http://msdn.microsoft.com/en-us/library/ms973913.aspx

Registration-Free Activation of .NET-Based Components: A Walkthrough
http://msdn.microsoft.com/en-us/library/ms973915.aspx


/////////////////////////////////////////////////////////////////////////////