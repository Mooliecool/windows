================================================================================
       Windows APPLICATION: CSWebBrowserSuppressError Overview                        
===============================================================================
/////////////////////////////////////////////////////////////////////////////
Summary:
The sample demonstrates how to make WebBrowser suppress errors. The errors include

1. Calling Script Just In Time Debugger.

   function CallDebugger() {
       debugger;
   }

   This could be disabled by the value of DisableJITDebugger in the key 
   HKCU\Software\Microsoft\Internet Explorer\Main.

   Notice that to disable JIT debugger, the application has to be restarted. 

2. Html element errors.

   function CreateScriptError() {
       throw ("Here is an error! ");
   }

   You can register the Document.Window.Error event and handle it.
   
   Notice that Document.Window.Error event will only take effect when JITDebugger
   is disabled.

3. Navigation error. Like the page does not exist(Http 404 error).

   DWebBrowserEvents2 Interface designates an event sink interface that an application
   must implement to receive event notifications from the underlying ActiveX control,  
   and there is a NavigateError Event in this interface. See
   http://msdn.microsoft.com/en-us/library/aa768283(VS.85).aspx
    
4. Other errors, like permission is needed when clipboard is used in Javascript. 

   If you want to suppress all errors, you can set ScriptErrorsSuppressed to true. When 
   ScriptErrorsSuppressed is set to true, the WebBrowser control hides all its dialog
   boxes that originate from the underlying ActiveX control, not just script errors. 
   Occasionally you might need to suppress script errors while displaying dialog boxes 
   such as those used for browser security settings and user login. In this case, set 
   ScriptErrorsSuppressed to false and suppress script errors in a handler for the 
   HtmlWindow.Error event. 

////////////////////////////////////////////////////////////////////////////////
Demo:

Disable JIT Debugger

Step1. Run CSWebBrowserSuppressError.exe.

Step2. Uncheck "Suppress JIT Debugger", and restart this application. 
       You can skip this step if the checkbox is already unchecked.

Step3. Make the top textbox empty, and click the button "Go". This operation will let
       WebBrowser navigate to a build-in html with errors.

Step4. Click "Launch Debugger" in the page, you will see a dialog to launch JIT debugger 
       if VS is installed.

Step5. Check "Suppress JIT Debugger", and restart this application.

Step6. Make the top textbox empty, and click the button "Go".

Step7. Click "Launch Debugger" in the page, you will not see the  dialog to launch JIT 
       debugger.


Suppress html element errors

Step1. Run CSWebBrowserSuppressError.exe.

Step2. Check "Suppress JIT Debugger", and restart this application.
       You can skip this step if the checkbox is already checked.

Step3. Make the top textbox empty, and click the button "Go". This operation will let
       WebBrowser navigate to a build-in html with errors.

Step4. Uncheck "Suppress Html Element Errors". 

Step5. Click "Script Error" in the page, you will see a WebPage error dialog.

Step6. Check "Suppress Html Element Errors". 

Step7. Click "Script Error" in the page, you will not see the WebPage error dialog.
	 
	 

Handle navigation error

Step1. Run CSWebBrowserSuppressError.exe.

Step2. Uncheck "Suppress Navigation Error". 

Step3. Type http://www.microsoft.com/NotExist.html the top textbox, and click the button "Go".
       Microsoft.com will tell you that the page is not found.

Step4. Check "Suppress Navigation Error". 

Step5. Type http://www.microsoft.com/NotExist.html the top textbox, and click the button "Go".
       You will see the build-in HTTP404 html.



Suppress all dialogs	   	    

Step1. Run CSWebBrowserSuppressError.exe.

Step2. Make the top textbox empty, and click the button "Go". This operation will let
       WebBrowser navigate to a build-in html with errors.

Step3. Uncheck "Suppress all dialog". 

Step4. Click "Security Dialog" in the page, you will see a "Windows Security Warning" dialog.

Step5. Make the top textbox empty, and click the button "Go" again. Or refresh this page in the 
       Context menu.

Step6. Check "Suppress all dialog". 

Step7. Click "Security Dialog" in the page, you will not see the "Windows Security Warning"
       dialog.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Create html file Error.htm. This web page can create script error, security warning and
launch JIT debugger.

2. Design a class WebBrowserEx that inherits class System.Windows.Forms.WebBrowser. This 
   class supplies following features.
   
   2.1. Disable JIT Debugger.
        The static property  DisableJITDebugger can get or set the value of "Disable Script
	    Debugger" in the key HKCU\Software\Microsoft\Internet Explorer\Main.

   2.2. Suppress html element errors of document loaded in this browser.
        Handle the window error event of document loaded in this browser.

   2.3. Handle navigation error.
        The interface DWebBrowserEvents2 designates an event sink interface that an application
		must implement to receive event notifications from a WebBrowser control or from the 
		Windows Internet Explorer application. The event notifications include NavigateError 
		event that will be used in this application.

   2.4 The class WebBrowser itself also has a Property ScriptErrorsSuppressed to hides all its 
       dialog boxes that originate from the underlying ActiveX control, not just script errors.
	   
3. In the MainForm, handle the checkboxes CheckedChanged event to disable JIT debugger, suppress html 
   element errors and suppress all dialog. 
   
   It also registers the NavigateError event of WebBrowserEx. If "Suppress Navigation Error" is
   checked and the http status code is 404, then navigate to the build-in 404.htm.


/////////////////////////////////////////////////////////////////////////////
References:
http://msdn.microsoft.com/en-us/library/aa768283(VS.85).aspx
http://msdn.microsoft.com/en-us/library/system.windows.forms.webbrowser.scripterrorssuppressed.aspx
http://msdn.microsoft.com/en-us/library/system.windows.forms.webbrowser.createsink.aspx
/////////////////////////////////////////////////////////////////////////////
