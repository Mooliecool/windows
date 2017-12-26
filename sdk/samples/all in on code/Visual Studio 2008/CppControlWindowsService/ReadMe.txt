=============================================================================
      CONSOLE APPLICATION : CppControlWindowsService Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The code sample demonstrates how to install, uninstall, find, start, stop a 
Windows service and set the service DACL to grant start, stop, delete and 
read control access to all authenticated users programmatically.


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the service control 
sample.

Step1. After you successfully build the CppControlWindowsService and 
CppWindowsService sample projects in Visual Studio 2008, you will get the 
applications: CppControlWindowsService.exe and CppWindowsService.exe. 

Step2. In a command prompt running as administrator, navigate to the folder 
containing CppControlWindowsService.exe and CppWindowsService.exe. Run 
CppControlWindowsService.exe without any parameters. You will see the 
instructions of the sample application. 

    CppControlWindowsService Instructions:
    -install <ServiceName> <BinaryPath>  Install a service
    -uninstall <ServiceName>             Uninstall a service
    -find <ServiceName>                  Find a service
    -start <ServiceName>                 Start a service
    -stop <ServiceName>                  Stop a service
    -update dacl <ServiceName>           Update the DACL of a service

Step3. Enter the following command in the same command prompt. It will 
install CppWindowsService into the local Service Control Manager (SCM) as a 
Windows service. 

    CppControlWindowsService.exe -install CppWindowsService CppWindowsService.exe

You can verify that the service is installed successfully by opening 
services.msc, and checking if CppWindowsService is in the service list, or by 
entering the following command in the command prompt, and see if it prints s
"Service CppWindowsService was installed".

	CppControlWindowsService.exe -find CppWindowsService

Step4. Enter the following commands to start the service, and then stop the 
service.

    CppControlWindowsService.exe -start CppWindowsService
    CppControlWindowsService.exe -stop CppWindowsService

You can verify that the service was started and stopped successfully by 
checking the Application event log. You should be able to find the 
"CppWindowsService in OnStart" and "CppWindowsService in OnStop" records. 

Step5. Create a new *standard* user account in the system. Log on the user, 
and find the CppWindowsService service in services.msc. You will find that 
neither "Start" nor "Stop" is enabled in the context menu of the service. You 
are not able to start or stop the service with the standard user account 
because the start and stop control accesses of a Windows service are not 
granted to standard users by default. You have to modify the DACL of the 
service with an admin account. 

In a command prompt running as administrator, navigate to the folder 
containing CppControlWindowsService.exe and CppWindowsService.exe, and enter 
the command:

	CppControlWindowsService.exe -update dacl CppWindowsService

The command sets the service DACL to grant start, stop, delete and read 
control access to all authenticated users. The operation succeeds if you see 
"Service DACL updated successfully".

If you switch to the standard user account, you will find that you are able 
start or stop the CppWindowsService service now.

Step6. Enter the following commands in the command prompt running as 
administrator to stop and uninstall the service.

	CppControlWindowsService.exe -uninstall CppWindowsService


/////////////////////////////////////////////////////////////////////////////
Implementation:

A. Installing a Windows Service


-------------------------------------------------

B. Uninstalling a Windows Service


-------------------------------------------------

C. Finding a Windows Service


-------------------------------------------------

D. Starting a Windows Service


-------------------------------------------------

E. Updating the DACL of a Windows Service




/////////////////////////////////////////////////////////////////////////////
References:

MSDN: CreateService Function
http://msdn.microsoft.com/en-us/library/ms682450.aspx

MSDN: SvcControl.cpp
http://msdn.microsoft.com/en-us/library/bb540474.aspx

MSDN: Installing a Service
http://msdn.microsoft.com/en-us/library/ms683500.aspx


/////////////////////////////////////////////////////////////////////////////