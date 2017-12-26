=============================================================================
       Windows APPLICATION: VBTabbedWebBrowser Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The sample demonstrates how to create a tabbed WebBrowser.

The "Open in new Tab" context command is disabled in WebBorwser by default, 
you can add a value *.exe=1 (* means the process name)to the key 
HKCU\Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_TABBED_BROWSING.
This menu will only take effect after the application is restarted.
See http://msdn.microsoft.com/en-us/library/ms537636(VS.85).aspx

DWebBrowserEvents2 Interface designates an event sink interface that an 
application must implement to receive event notifications from the underlying 
ActiveX control, and there is a NewWindow3 Event in this interface. See
http://msdn.microsoft.com/en-us/library/aa768283(VS.85).aspx


/////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Build this project in VS2010. 

Step2. Run VBTabbedWebBrowser.exe.

Step3. Type http://1code.codeplex.com/ in the Url, and press Enter.

Step4. Right click the "Downloads" in the header of the page, and then click 
       "Open in new tab". This application will open the link in a new tab.

	   If the "Open in new tab" is disabled, check "Enable Tab" and restart 
	   the application.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Design a class WebBrowserEx that inherits class 
   System.Windows.Forms.WebBrowser. This class can handle NewWindow3 event.

   The interface DWebBrowserEvents2 designates an event sink interface that 
   an application must implement to receive event notifications from a 
   WebBrowser control or from the Windows Internet Explorer application. The 
   event notifications include NewWindow3 event that will be used in this 
   application.

2. Design a class WebBrowserTabPage that inherits the the 
   System.Windows.Forms.TabPage class and contains a WebBrowserEx property. 
   An instance of this class could be add to a tab control directly.
       
3. Create a UserControl that contains a System.Windows.Forms.TabControl 
   instance. This UserControl supplies the method to create/close the 
   WebBrowserTabPage in the TabControl. It also supplies a Property 
   IsTabEnabled to get or set whether the "Open in new Tab" context menu in 
   WebBrowser is enabled.

4. In the MainForm, it supplies controls to enable/disable tab, create/close 
   the tab, and make the WebBrowser GoBack, GoForward or Refresh.


/////////////////////////////////////////////////////////////////////////////
References:

http://msdn.microsoft.com/en-us/library/aa768283(VS.85).aspx
http://msdn.microsoft.com/en-us/library/ms537636(VS.85).aspx
http://msdn.microsoft.com/en-us/library/system.windows.forms.webbrowser.createsink.aspx


/////////////////////////////////////////////////////////////////////////////
