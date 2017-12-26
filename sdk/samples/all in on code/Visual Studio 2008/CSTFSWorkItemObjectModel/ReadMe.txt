=================================================================================
					C# TFS: CSTFSWorkItemObjectModel Project Overview
=================================================================================

/////////////////////////////////////////////////////////////////////////////////
Use: 

This C# sample works in machines where Team Explorer 2008 is installed. This 
sample uses object model from Team Explorer 2008 to access TFS. It will 
demostrate how to programatically manage work items. You may want to select a
team project for experimental purpose. Set the team project name and TFS URL in 
the App.config. 


/////////////////////////////////////////////////////////////////////////////////
Prerequisites:

Team Explorer 2008 must be installed on the machine. You can download it from:
https://www.microsoft.com/downloads/details.aspx?FamilyID=0ed12659-3d41-4420-bbb0-a46e51bfca86&displaylang=en

Otherwise the project may not be able to reference to work item object model.


/////////////////////////////////////////////////////////////////////////////////
Creation:

1. Create a new Windows Console application named "CSTFSWorkItemObjectModel".

2. Add reference to Microsoft.TeamFoundation.Client.dll and
   Microsoft.TeamFoundation.WorkItemTracking.Client.dll. They are located in
   %ProgramFiles%\Microsoft Visual Studio 9.0\Common7\IDE\PrivateAssemblies.
   
3. Add an App.config to the solution with the following content. Please replace
   TfsUrl value to your TFS server and TeamProject value to an existing team 
   project for experiment. 
   
	<?xml version="1.0" encoding="utf-8" ?>
	<configuration>
	  <appSettings>
		<add key="TfsUrl" value="http://170112m3:8080"/>
		<add key="TeamProject" value="WIT Test"/>
	  </appSettings>
	</configuration>

4. Copy the content in the Programs.cs in the sample to your project. 

7. To build and debug this sample you can press F5 from your keyboard, or click 
   Debug menu and choose "Start Debugging". 


/////////////////////////////////////////////////////////////////////////////////
Changes to your machine: 

1. Import a work item type "My WIT" to the specified team project.

2. Create work items of type "My WIT" in the specified team project.

3. Create a work item query "My WITs" in the specified team project.
   team project and create work items.


/////////////////////////////////////////////////////////////////////////////////
How do I rollback changes:

1. To destroy a work item, you can use "tfpt destroywi" from TFS power tool. 

2. To destroy a work item type, you can use "tfpt destroywitd" from TFS power tool.

3. To delete a work item query, you can right click the query in Team Explorer and
   select "Delete".


/////////////////////////////////////////////////////////////////////////////////
References:

1. Work Item Tracking Extensibility
   http://msdn.microsoft.com/en-us/library/bb130347.aspx


/////////////////////////////////////////////////////////////////////////////////
   
