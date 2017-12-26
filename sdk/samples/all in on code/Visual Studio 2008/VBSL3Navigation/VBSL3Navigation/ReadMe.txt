========================================================================
    SILVERLIGHT APPLICATION : VBSL3Navigation Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This project created a sample browser. By use of this browser, user could view
the profiles of former All-In-One Silverlight Samples.
Silverlight Application Navigation is a newly added feature in Silverlight 3, it's 
ideal for creating multipage application. This project demonstrates how to use
navigation.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 3 Tools for Visual Studio 2008 SP1
http://www.microsoft.com/downloads/details.aspx?familyid=9442b0f2-7465-417a-88f3-5e7b5409e9dd&displaylang=en

Silverilght 3 runtime:
http://silverlight.net/getstarted/


/////////////////////////////////////////////////////////////////////////////
Demo:

To run the navigation sample, please follow the steps below:
1. Compile the entrie solution.
2. Right click VBSL3NavigationTestPage.aspx, select "View in Browser".
3. When Silverlight application started up, click "Sample List" and other
hyperlink button to test the navigation function. 


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. How to navigate internal page in Silverlight?
	1. In main page, create and layout a Frame as the display window for Silverlight pages.
	   Frame control can be found at System.Windows.Controls.Navigation.dll, the frame must 
	   be assigned a name.
	2. Create Silverlight pages. Pages can be created from VS installed template "Silverlight
	   Page".
	3. There are certain ways to navigate the internal pages:
		* Create a HyperLinkButton, set relative page uri to NavigationUri, and set
		  TargetName to Frame's name. When user click the HyperLinkButton, the Frame
		  will change inner page to the linked page.
		* In main page, we can navigate pages by manipulating Frame control directly.
			this.myFrame.Navigate( myuri )
		* In pages(derived from System.Windows.Controls.Page), we can use NavigationService
		  to control the page navigation.
			this.NavigationService.Navigate( myuri )
			
2. How to use User-Friendly URIs for internal navigation?
	We could map the page uri to more friendly uri by manipulating Frame.UriMapper property.
	For details, please refer to http://msdn.microsoft.com/en-us/library/cc838245(VS.95).aspx
			
3. How to navigate to external web pages?
	    * Create HyperLinkButton, set web page url to NavigationUri, and set TargetName
	      to "_blank" if we need open new window, or set to "_self" if we want to open at
	      current window.
	    * Use Window.Navigate method
	       System.Windows.Browser.HtmlPage.Window.Navigate(myuri, targetwindow)
		* Use Popup method
		   System.Windows.Browser.HtmlPage.PopupWindow(myuri, targetwindow, options)

   
/////////////////////////////////////////////////////////////////////////////
References:

Navigation Overview
http://msdn.microsoft.com/en-us/library/cc838245(VS.95).aspx


/////////////////////////////////////////////////////////////////////////////
