========================================================================
            VBASPNETAJAXConsumeExternalWebService Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The project illustrates how to consume an external Web Service from a
different domain.

/////////////////////////////////////////////////////////////////////////////
Demo the Sample. 

Please follow this demonstration steps below.

Step 1: Open the VBASPNETAJAXConsumeExternalWebService.sln.

Step 2: Expand the ExternalWebSite and right-click the ExternalWebService.asmx 
        and click "View in Browser". This step is very important, 
        it impersonates to open an external web service.

Step 3: Expand the TestWebSite and right-click the default.aspx and click 
        "View in Browser".

Step 4: You will see a black panel and a Button. Click on the button. You will
        see "Please wait a moment..." in the Panel, after about one second or 
        less time, you will see the datetime returned from the server.

Step 5: That's all.

/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step 1.  Create an VB.NET "ASP.NET Empty Web Site" in Visual Studio 2010 or
         Visual Web Developer 2010. Change the last folder name to ExternalWebSite.

Step 2.  Add a new "Web Service" item. We call it ExternalWebService.asmx.

Step 3.  Open the ExternalWebService.asmx.vb in App_Code.

Step 4.  Un-comment this line above the class name.
         [CODE]
         <System.Web.Script.Services.ScriptService()>
         [/CODE]

Step 5.  Write a new web method like below and save the file.
         [CODE]
         <WebMethod()> _
         Public Function GetServerTime() As DateTime
              Return DateTime.Now
         End Function
         [/CODE]

Step 6.  Open the ExternalWebService.asmx to view the page in the browser.
         Copy the URL address in the navigation bar.
         
Step 7.  Add a new C# "ASP.NET Empty Web Site". Change the last folder name
         to TestWebSite.
         
Step 8.  Click on the new website in the Solution Explorer and look at the menu
         bar. Click the WebSite -> Add Web Reference...

Step 9.  Paste the URL address which we get in the step 6 into the "URL" textbox.
         Change the "Web Reference Name" to "ExternalWebService". Click  
         "Add Reference".

Step 10. Create a new "Web Service" item. Change the name to BridgeWebService.asmx

Step 11. Open the BridgeWebService.asmx.vb in the App_Code. Do the same thing in
         step 4.

Step 12. Write the code below to call the external web service and save the file.
         [CODE]
         <WebMethod()> _
         Public Function GetServerTime() As DateTime
             ' Get an instance of the external web service
             Dim ews As ExternalWebService.ExternalWebService = New ExternalWebService.ExternalWebService()
             ' Return the result from the web service method.
             Return ews.GetServerTime()
         End Function
         [/CODE]

Step 13. Create a new "Web Form" item. Chage the name to Default.aspx.

Step 14. Add a ScriptManager control into the page. And a service reference 
         which path is the local bridge web service: BridgeWebService.asmx.

Step 15. Create a DIV for showing the result and a Button to call the service.
         [CODE]
         <div id="Result" 
              style="width: 100%; height: 100px; background-color: Black; color: White">
         </div>
         <input type="button" 
                value="Get Server Time From External Web Service"
                onclick="GetServerDateTime()" />
         [/CODE]

Step 16. Create the Javascript functions to call the web service.
         [CODE]
         <script type="text/javascript">
         // This function is used to call the service by Ajax Extension.
         function GetServerDateTime() {
             $get("Result").innerHTML = "Please wait a moment...";
             BridgeWebService.GetServerTime(onSuccess, onFailed);
         }
         // This function will be executed when get a response 
         // from the service.
         function onSuccess(result) {
             $get("Result").innerHTML = "Server DateTime is : " + result.toLocaleString();
         }

         // This function will be executed when get an exception
         // from the service.
         function onFailed(args) {
             alert("Server Return Error:\n" +
             "Message:" + args.get_message() + "\n" +
             "Status Code:" + args.get_statusCode() + "\n" +
             "Exception Type:" + args.get_exceptionType());
         }
         </script>
         [/CODE]
         When we add the ServiceReference of the ScriptManager, we will see
         IntelliSense effect.

Step 17. Test the Default.aspx and we will get the datetime from the server.

PS
If we want to use an AJAX-enabled WCF service from a different domain with 
Service Reference of the ScriptManager, the steps are same as we talked above.
Just create a local AJAX-enabled WCF service to make a bridge to consume the
remote one.
/////////////////////////////////////////////////////////////////////////////
References:

MSDN: ServiceReference Class
http://msdn.microsoft.com/en-us/library/system.web.ui.servicereference.aspx

MSDN: ASP.NET Web Services
http://msdn.microsoft.com/en-us/library/t745kdsh.aspx

/////////////////////////////////////////////////////////////////////////////