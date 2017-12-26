========================================================================
    CONSOLE APPLICATION : CSIISAdminADSI Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:
This sample demonstrates how to use Active Directory Service Interfaces (ADSI) 
to configure IIS by using .Net System.DirectoryServices namespace to access IIS 
ADSI Provider. The classes, methods, and properties of the IIS ADSI provider can
be used to configure IIS from scripts or executables. 
 
Active Directory Service Interfaces (ADSI) is a directory service model and a 
set of Component Object Model (COM) interfaces. ADSI enables Windows applications
and Active Directory clients to access several network directory services. The
IIS ADSI provider, like the IIS WMI provider provides a standard syntax for 
accessing IIS configuration data.

To run this example project. you need:
Step1. Install IIS 6.0, IIS 7.0 or IIS7.5.
Step2. You must log on as a system administrator to execute this sample.

Note:
1.Ensure there is no Web site called "IISADSIDemo" on your IIS server. 
2.Make sure TCP port 83 is available.

/////////////////////////////////////////////////////////////////////////////
Prerequisite

Install IIS 6.0, IIS 7.0 or IIS7.5

/////////////////////////////////////////////////////////////////////////////
Creation:

1. Retrive W3SVC object with System.DirectoryServices.DirectoryEntry.
2. Create a 'siteProperty' object arrary contains new site's properties.
3. Invoke 'CreateNewSite' method to create the new web site.
4. Invoke 'Start' method of the new site instance to start it.
5. Retrive new site's instance with System.DirectoryServices.DirectoryEntry.
6. Create new virtual directory object by calling 
    DirectoryEntries.Add().
7. Set virtual directory properties.
8. List all web sites on the server by enumerating DirectoryEntry objects 
    in W3SVC instance's Children collection.
	
/////////////////////////////////////////////////////////////////////////////
References:

IIS ADSI Provider
http://msdn.microsoft.com/en-us/library/ms524997.aspx


/////////////////////////////////////////////////////////////////////////////


