OnDemand ClickOnce sample

This sample demonstrates how to use ClickOnce APIs to download application 
components on demand.

This application is contains the following components:
OnDemand.exe		(required)
CustomerLibrary.dll	(optional)
OrderLibrary.dll	(optional)	
OrderListControl.dll	(optional)

When the application is initially installed, only OnDemand.exe is downloaded.
Run the application and review the Deployment State on the UI. Running the 
app and clicking on the Customer Tracking and Order Tracking buttons will 
download these optional components from the server, install them and load 
them in the OnDemand.exe UI.

To See this work, do the following.
1. Publish the OnDemand application via the Visual Studio Publish tab.
     Note in the Deployment State for Customer Local and Order Local is false.
2. Click the Customer Tracking button to download, install and run the Customer
Tracking component. Note the Customer Local deployement status is now set to true.
3. Repeat Step 2 for the Order Tracking component.
4. Optionally Click the Download Async check box to perform an async download.

 