========================================================================
    CONSOLE APPLICATION : VBIISAdminWMI Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:
This sample demonstrates how to use Windows Management Instrumentation (WMI) 
to configure IIS by using .Net System.Management namespace to access IIS WMI 
Provider. The classes, methods, and properties of the IIS WMI provider can be
used to configure IIS from scripts or executables. 
 
The IIS WMI provider, like the IIS ADSI provider, provides a standard syntax 
for accessing IIS configuration data through the use of the IIS admin objects. 
Any script or code that attempts to manage IIS using Windows Management 
Instrumentation (WMI) needs to access the IIS WMI provider objects. For example,
if you want to change a Web site property in the metabase, you must instantiate
the class called IIsWebServerSetting which is a child of the CIM_Setting class.
The IIS WMI provider cannot be used without certain methods of the Windows WMI 
classes. 


To run this example project. you need:
Step1. Install IIS 6.0, IIS 7.0 or IIS7.5.
Step2. You must log on as a system administrator to execute this sample.

Note:
1.Ensure there is no Web site called "IISWMIDemo" on your IIS server. 
2.Make sure TCP port 83 is available.

/////////////////////////////////////////////////////////////////////////////
Prerequisite

Install IIS 6.0, IIS 7.0 or IIS7.5

/////////////////////////////////////////////////////////////////////////////
Creation:

1. Connection to IIS WMI Provider by calling ManagementScope.Connect().
2. Create a 'ServerBinding' WMI class instance.
3. Retrive W3SVC instance through ManagementObject.
4. Invoke 'CreateNewSite' method to create the new web site.
5. Invoke 'Start' method of the new site instance to start it.
6. Create an 'IIsWebVirtualDirSetting' instance.
7. Create new virtual directory object by calling 
    ManagementClass.CreateInstance().
8. Invoke 'AppCreate2' method to create web application for the new 
    virtual directory in IIS.
9. Query and enumerate 'IISWebServerSetting' objects on the server to
    list all web sites.	
	
/////////////////////////////////////////////////////////////////////////////
References:

IIS WMI Provider
http://msdn.microsoft.com/en-us/library/ms525265.aspx


/////////////////////////////////////////////////////////////////////////////


