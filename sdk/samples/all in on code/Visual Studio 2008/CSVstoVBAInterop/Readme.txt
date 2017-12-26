========================================================================
    VSTO : CSVstoVBAInterop Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CSVstoVBAInterop project demonstrates how to interop with VBA project
object model in VSTO projects. Including how to programmatically add Macros
(or VBA UDF in Excel) into an Office document; how to call Macros / VBA UDFs 
from VSTO code; and how to call VSTO code from VBA code.

On Excel Ribbon -> VSTO Samples, click Interop Form (C#) button.


/////////////////////////////////////////////////////////////////////////////

To access the VBA object model from VSTO code, you must reference the
following assembly:

Microsoft.Vbe.Interop

Starting from Office 2003, programmatically access VBA project object model
is disabled by default. You must enable this option either manually in Office
application settings or programmatically modify the registry
(see VBEHelper.cs).

To access the VBA project OM, use Application.VBE property.
To get the VBA project associated to a specific Workbook, use the
Workbook.VBProject property.

In order to expose your .NET objects to VBA code in Office, you must define
the class with ComVisible attribute and the class must expose the IDispatch
interface (see VstoClass.cs).

Then you can override the ThisAddIn.RequestComAddInAutomationService method 
and return an instance of the class (in this sample VstoClass).

To call VBA code within VSTO add-in, use the Application.Run method (see
InteropForm.cs).


/////////////////////////////////////////////////////////////////////////////
References:

Calling Code in Application-Level Add-ins from Other Solutions
http://msdn.microsoft.com/en-us/library/bb608621.aspx

Walkthrough: Calling Code in an Application-Level Add-in from VBA
http://msdn.microsoft.com/en-us/library/bb608614.aspx


/////////////////////////////////////////////////////////////////////////////