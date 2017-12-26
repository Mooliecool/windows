========================================================================
    ASP.NET APPLICATION: CSASPNETCustomHttpHandlerandModule Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This sample introduces how to write custom Http Handler and Http Module and
 use them in ASP.NET web application on IIS.

It includes two projects: CSASPNETCustomHttpHandlerandModule and 
CustomHandlerandModuleProject.

CSASPNETCustomHttpHandlerandModule is ASP.NET web application to use custom
Http Handler and Module.
 
CustomHandlerandModuleProject is Class Library project to implement custom
Http Handler and Module.

The sample can be running on IIS 7.0 directly. To use it on IIS 6 or earlier 
version, we need to register the extension .demo in aspnet_isapi.dll.


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Create Class Library project named "CustomHandlerandModuleProject".

Step2. Add two classes to inherit IHttpHandler and IHttpModule to implement
its methods. 

   note: Need to add reference to System.Web assembly.

Step3. Create ASP.NET web application named "CSASPNETCustomHttpHandlerandModule".  

Step4. Add reference to project "CustomHandlerandModuleProject" from 
CSASPNETCustomHttpHandlerandModule and registercustom Http Handler and Module 
in IIS:
 
<handlers>
	<add name="CustomHandlerFor.demo" verb="*" path="*.demo" type="CustomHandlerandModuleProject.CustomHttpHandler"/>
</handlers>

<modules>
	<add name="CustomModule" type="CustomHandlerandModuleProject.CustomHttpModule"/>
</modules>

Step5. Publish ASP.NET web application and deploy it on IIS 7 and request 
default.htm page to view sample.


/////////////////////////////////////////////////////////////////////////////
References:

HTTP Handlers and HTTP Modules Overview
http://msdn.microsoft.com/en-us/library/bb398986.aspx

How to: Configure an HTTP Handler Extension in IIS
http://msdn.microsoft.com/en-us/library/bb515343.aspx

Walkthrough: Creating and Registering a Custom HTTP Module
http://msdn.microsoft.com/en-us/library/ms227673.aspx

/////////////////////////////////////////////////////////////////////////////
