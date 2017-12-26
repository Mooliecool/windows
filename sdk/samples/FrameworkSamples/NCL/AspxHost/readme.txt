ASPX Hosting sample
=======================
This sample demonstrates how to combine the features of HttpListener to create an Http server that routes calls to the hosted Aspx application. Version 2.0 of the .NET Framework introduces the HttpListener class built on top of Http.Sys, which enables users to create a standalone Http server.

This sample uses the following features of HttpListener:
1. Authentication
2. Enabling SSL
3. Reading Client Certificates on secure connections


Sample Language Implementations
===============================
     This sample is available in the following language implementations:
     C#


To build the sample using the command prompt:
=============================================

     1. Open the SDK Command Prompt window and navigate to the CS subdirectory under the AspxHost directory.

     2. Type msbuild AspxHostCS.sln.


To build the sample using Visual Studio:
=======================================

     1. Open Windows Explorer and navigate to the CS subdirectory under the AspxHost directory.

     2. Double-click the icon for the .sln (solution) file to open the file in Visual Studio.

     3. In the Build menu, select Build Solution.
     The application will be built in the default \bin or \bin\Debug directory.


To run the sample:
=================
     1. Navigate to the directory that contains the new executable, using the command prompt or Windows Explorer.
     2. Type AspxHostCS.exe at the command line, or double-click the icon for AspxHostingCS.exe to launch it from Windows Explorer. 


Remarks
======================
1. Class Information

The AspxHostCS.cs file contains the main class that creates and configures a listener and an Aspx application.

The AspxVirtualRoot.cs file contains the class that configures an HttpListener to listen on prefixes and supported authentication schemes.

The AspxNetEngine.cs file contains the class that configures an Aspx application by assigning a virtual alias that maps to a physical directory.

The AspxPage.cs file contains the class that implements SimpleWorkerRequest class and represents a page requested by the client.

The AspxRequestInfo.cs file contains the data holder class used to pass relevant data from HttpListenerContext to the hosted application.

The AspxException.cs file contains the custom exception class.

The Demopages directory contains sample Aspx pages.


2. Sample Usage
 
The AspxHostCS.cs file is the class that contains the main method that will launch an HttpListener and configure a physical directory as a hosted ASPX application. By default, the class tries to configure the DemoPages directory (which is found in the same samples directory) as a hosted application under virtual alias /.  Since the HttpListener in this sample listens on port 80 you may need to stop IIS to run this sample.

 
Change the code for individual use: 

                //Create a AspxVirtualRoot object with a http port and https port if required
                
                AspxVirtualRoot virtualRoot = new AspxVirtualRoot(80);


                //Configure a Physical directory as a virtual alias.

                //TODO: Replace the physical directory with the directory to be configured.

		virtualRoot.Configure("/", Path.GetFullPath(@"..\..\DemoPages"));


                //TODO: If Authentication is to be added, add it here

                //virtualRoot.AuthenticationSchemes = AuthenticationSchemes.Basic;


3. Setting Authentication Scheme
 
After configuring an AspxVirtualRoot object, set the required authentication scheme by setting the AuthenticationScheme field on the AspxVirtualRoot object.

 
4. Enabling Ssl
 

To enable SSL, a Server certificate installed on the machine store must be configured on the port where SSL is required. For more information about how to configure a server certificate on a port using Httpcfg.exe util, refer Httpcfg link.

 
Note: Winhttpcertcfg can also be used for configuring Server cert on a port.


Known Issue
====================== 

Issue:
When I start the application, the following error message appears:

"System.IO.FileNotFoundException: Could not load file or assembly 'AspxHostCS, Version=1.0.1809.19805, Culture=neutral, PublicKeyToken=null' or one of its dependencies. The system cannot find the file specified.File name: 'AspxHostCS, Version=1.0.1809.19805, Culture=neutral, PublicKeyToken=null'”

Solution:
The AspxHostCs.exe file is not present in the bin directory of the physical directory being configured. Copy the AspxHostcs.exe file to the bin directory.


See Also
============
See HttpListener and Aspx Hosting API documentation in the .NET Framework SDK documentation and on MSDN.