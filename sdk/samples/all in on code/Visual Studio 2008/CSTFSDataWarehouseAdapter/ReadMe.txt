=================================================================================
					C# TFS: CSTFSDataWarehouseAdapter Project Overview
=================================================================================

/////////////////////////////////////////////////////////////////////////////////
Use: 

This C# sample works for Team Foundation Server 2008.  It demostrates how to 
create a TFS warehouse adapter and populates data into the custom fact. 

/////////////////////////////////////////////////////////////////////////////////
Creation:

1. Create a new Windows Console application named "CSTFSDataWarehouseAdapter".

2. Add reference to Microsoft.TeamFoundation.dll, 
   Microsoft.TeamFoundation.Client.dll and Microsoft.TeamFoundation.Common.dll.
   They are located in GAC at computers where Team Explorer is installed.   
   
3. Copy the content in the CreatedFactDemo.cs in the sample to your project.

/////////////////////////////////////////////////////////////////////////////////
Deployment:

1. Compile the project. 

2. Copy the genetated CSTFSDataWarehouseAdapter.dll into 
   "Web Services\Warehouse\bin\Plugins" in the installation folder of TFS on the 
   application tier. 

3. Restart IIS by running "iisreset" from command prompt. 

4. On the application tier, navigate to 
   http://localhost:8080/Warehouse/v1.0/warehousecontroller.asmx?op=Run and 
   click Invoke.


/////////////////////////////////////////////////////////////////////////////////
Debug: 

The adpater is loadded in worker process of TFS web site. To debug the adapter,
you need to attache to the worker process with Visual Studio.

Additionally, TFS writes error messages related to the warehouse adapters to 
Windows Event Log. You can check the logged errors there in case your adapter is
not executed during warehare synchronization. 

When Visual Studio is installed on the application tier, you can debug with the 
below steps:

1. Open the project with Visual Studio in the application tier and set break points. 

2. Navigate to http://localhost:8080/Warehouse/v1.0/warehousecontroller.asmx. This
   step ensure the work process of the TFS web site is tarted. 
   
3. In Visual Studio, click Tools->Attache to Process.

4. Check "Show processes from all sessions". 

5. Double click the w3wp.exe process which runs under the TFSSERVICE account.

6. Navigate to http://localhost:8080/Warehouse/v1.0/warehousecontroller.asmx?op=Run 
   and click Invoke.


If Visual Studio is not installed on the application tier, you can setup the remote
debugger on it, then debug the adapter from another machine. The remote debugger 
enables us to attach to remote process. Please follow the link in the references to
configure remote debugger.

/////////////////////////////////////////////////////////////////////////////////
References:

1. Remote Debugging Setup
   http://msdn.microsoft.com/en-us/library/y7f5zaaa.aspx

2. How to: Create an Adapter
   http://msdn.microsoft.com/en-us/library/bb286956.aspx


/////////////////////////////////////////////////////////////////////////////////
   
