========================================================================
    CONSOLE APPLICATION : CSIIS7AdminMWA Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:
The Microsoft.Web.Administration namespace contains classes that a developer 
can use to administer IIS Manager. With the classes in this namespace, an 
administrator can read and write configuration information to ApplicationHost.
config, Web.config, and Administration.config files.

The classes in the Microsoft.Web.Administration namespace contain a series of 
convenient top-level objects that allow the developer to perform administrative
tasks. The different logical objects available include sites, applications,
application pools, application domains, virtual directories, and worker 
processes. You can use the API to obtain and work with the configuration 
and state of these objects and to perform such actions as creating a site, 
starting or stopping a site, deleting an application pool, recycling an 
application pool, and even unloading application domains.

To run this example project. you need:
Step1. Install IIS7 or IIS7.5.
Step2. Add a refernce to Microsoft.Web.Administration.dll. You can find 
	   this dll at (%WinDir%\System32\InetSrv).
Step3. You must run as administrator to execute this sample.

Note:
1.Ensure there is no Web site called "MySite" on your IIS server. 
2.Make sure port 8080 is available.


/////////////////////////////////////////////////////////////////////////////
Prerequisite

Install IIS 7 or IIS 7.5


/////////////////////////////////////////////////////////////////////////////
Creation:

0. Add reference to Microsoft.Web.Administration.dll 
1. Create a IIS7 Site by calling ServerManager.Sites.Add
2. Create a IIS7 Application by calling 
	ServerManager.Sites[siteName].Applications.Add
3. Create a Virtual Directory by calling ServerManager.Sites[siteName].
	Applications[appName].VirtualDirectories.Add
4. Create a Application Pool by calling ServerManager.ApplicationPools.Add
5. Delete a IIS7 Site by calling ServerManager.Sites.Remove
6. Delete a IIS7 Application Pool by calling 
	ServerManager.ApplicationPools.Remove
	
	
/////////////////////////////////////////////////////////////////////////////
References:

Microsoft.Web.Administrator Namespaces
http://msdn.microsoft.com/en-us/library/microsoft.web.administration.aspx


/////////////////////////////////////////////////////////////////////////////


