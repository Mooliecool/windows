========================================================================
       ASP.NET MVC APPLICATION : CSASPNETMVCCustomActionFilter Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

	The CSASPNETMVCCustomActionFilter sample demonstrates how to use C# codes 
	to create custom ActionFilters for ASP.NET MVC web application. In this 
	sample,	there are two custom ActionFilters, one is used for customizing 
	ViewData before page view result get executed and rendered; another is 
	used for perform logging within the various events during the processing 
	of custom ActionFilters.


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

Visual Studio 2008 SP1 with ASP.NET MVC 1.0 extension installed. 

*ASP.NET MVC 1.0 RTM download:
http://www.microsoft.com/downloads/details.aspx?FamilyID=53289097-73ce-43bf-b6a6-35e00103cb4b&displaylang=en


/////////////////////////////////////////////////////////////////////////////
How to Run:
  
*open the project

*select  default.aspx page and view it in browser

*in the main page UI, the message data(modified by ActionFilter) is displayed

*click the "About" tab, the About page will be displayed, this will trigger 
the Logging ActionFilter which will log events into the specified log file.


/////////////////////////////////////////////////////////////////////////////
Key components:

*web.config file: contains all the necessary configuration information 
	of this web application

*global.asax: contains all the URL routing rules

*HomeController class: contains the main application 
	navigation logic(such as default page and about page)

*Home Views: the page UI elements for HomeController

*shared Views & Site.Master: those UI elements shared by all page UI

*MessageModifierActionFilter: this is one of the custom ActionFilters which 
is used for intercepting the ActionResult execution and modify the ViewData


*TextLogActionFilter: this is another ActionFilter which is used to log some
info during the various events of ActionResult execution.


/////////////////////////////////////////////////////////////////////////////
References:

#ASP.NET MVC Tutorials
http://www.asp.net/Learn/mvc/


/////////////////////////////////////////////////////////////////////////////