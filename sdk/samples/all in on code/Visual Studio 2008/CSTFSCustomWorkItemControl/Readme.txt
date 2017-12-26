========================================================================
    C# TFS : CSTFSCustomWorkItemControl Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The logic of Work Item Type Definition can meet most our requirement, 
but Custom Wok Item Control can do more.
This C# sample works in machines where Team Explorer 2008 is installed. The 
OpenOtherWorkItemControl demonstrates how to create and deploy a Custom 
Workitem Control that can open another work item in Visual Studio 2008.

/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the deploy the control

Step1. After you successfully build the sample project in Visual Studio 2008, 
you will get  assemblies: CSTFSCustomWorkItemControl.dll and some reference 
assemblies

Step2. Copy the assemblies and OpenOtherWorkItemControl.wicc(under project folder) 
to¡± C:\Documents and Settings\All Users\Application Data\Microsoft\Team Foundation
\Work Item Tracking\Custom Controls\9.0¡± (Windows XP/2003)or  "C:\ProgramData\
Microsoft\Team Foundation\Work Item Tracking\Custom Controls\9.0"(Vista or later).
If this location does not exist, create it.

Step3. Import Task_OpenOtherWorkItem.xml to TFS using TF.exe or TFS Power Tools.

Step4. Refresh Work Items node in Team Explorer, add a new work item 
¡°Task_OpenOtherWorkItem¡±

Step5. Type a valid work item ID in WIID, then click GO. If there is no error,
VS will show a new work item UI.




/////////////////////////////////////////////////////////////////////////////
Build:

Set the platform target to ¡°X86¡±.

/////////////////////////////////////////////////////////////////////////////
Deployment:

Copy the assemblies (under bin folder) and OpenOtherWorkItemControl.wicc
(under project folder) to¡± C:\Documents and Settings\All Users\Application Data\
Microsoft\Team Foundation\Work Item Tracking\Custom Controls\9.0¡± (Windows XP/2003)
or "C:\ProgramData\Microsoft\Team Foundation\Work Item Tracking\Custom Controls\9.0"
(Vista or later). If this location does not exist, create it.


/////////////////////////////////////////////////////////////////////////////
 Code Logic:


A. Creating a Class Library Project in Visual Studio 2008

B. Adding following assemblies to Project Reference

EnvDTE
EnvDTE80
EnvDTE90
Extensibility
Microsoft.TeamFoundation
Microsoft.TeamFoundation.Client
Microsoft.TeamFoundation.WorkItemTracking.Client
Microsoft.TeamFoundation.WorkItemTracking.Controls
Microsoft.VisualStudio.TeamFoundation
Microsoft.VisualStudio.TeamFoundation.WorkItemTracking

C. Adding an User Control to project

D. Adding a textbox and a button to User Control

E. Adding fields and properties
        private EventHandlerList m_events;
        private static object EventBeforeUpdateDatasource = new object();
        private static object EventAfterUpdateDatasource = new object();

        private StringDictionary m_properties;
        private bool m_readOnly;
        private IServiceProvider m_serviceProvider;
        private WorkItem m_workItem;
        private string m_fieldName;

        EnvDTE80.DTE2 dte2;

F. Implementing IWorkItemControl Interface

G. Handling button click event.


/////////////////////////////////////////////////////////////////////////////
References:


Work Item Tracking Custom Controls
http://msdn.microsoft.com/en-us/library/bb286959.aspx


/////////////////////////////////////////////////////////////////////////////
