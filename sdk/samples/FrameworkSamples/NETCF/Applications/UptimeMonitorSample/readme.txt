SUMMARY

This is a messaging-level WCF client/server sample.  The client is a WinForms app that
runs on NetCF 3.5.  The server is a console app that runs on .NET 3.0 on the desktop.  
The server opens a WCF reply channel and responds to requests regarding how long it 
has been running.  The client opens a request channel to ask what the server's uptime is.

BUILD REQUIREMENTS

Visual Studio 2008, including the Smart Devices development components.

HOW TO BUILD

Either build each project within Visual Studio 2008 or using msbuild.exe <project>.csproj
at the command line.  
Note that in current builds of Visual Studio 2008, building the solution as a whole
is buggy and will not build both projects.