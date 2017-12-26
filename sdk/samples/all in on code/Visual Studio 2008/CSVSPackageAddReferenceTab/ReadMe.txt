================================================================================
       VSX application : CSVSPackageAddReferenceTab Project Overview                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Visual Studio supports to extend the Add Reference dialog and add custom
tab page into the dialog.

This sample demostrate you how to add a custom .NET user control as a tab 
page into the add reference dialog, and how to enable select button and
handle item selection events.

All the sample code is based on MPF.

The sample is initiated by the thread on the forum:
http://social.msdn.microsoft.com/Forums/en-US/vsx/thread/ddb0f935-b8ac-400d-9e3d-64d74be85031


//////////////////////////////////////////////////////////////////////////////
Prerequisites:

VS 2008 SDK must be installed on the machine. You can download it from:
http://www.microsoft.com/downloads/details.aspx?FamilyID=30402623-93ca-479a-867c-04dc45164f5b&displaylang=en

Otherwise the project may not be opened by Visual Studio.

If you run this project on a x64 OS, please also config the Debug tab of the project
Setting. Set the "Start external program" to 
C:\Program Files(x86)\Microsoft Visual Studio 9.0\Common7\IDE\devenv.exe

NOTE: The Package Load Failure Dialog occurs because there is no PLK(Package Load Key)
      Specified in this package. To obtain a PLK, please to go to WebSite:
      http://msdn.microsoft.com/en-us/vsx/cc655795.aspx
      More info
      http://msdn.microsoft.com/en-us/library/bb165395.aspx

/////////////////////////////////////////////////////////////////////////////

Steps:
In order to implement this sample, following are the essential steps:
(For detailed informaiton, please view sample code)

1. Create ProvideReferencePageAttribute
This attribute is derived from RegistrationAttribute, so it main purpose 
is to write information into registry.
VSRoot\ComponentPickerPages\<Tab Name>
#AddToMru = 1
#ComponentType = .NET Assembly
#Package = <package guid>
#Page = <.Net user control guid>
#Sort = 0x35

2. Apply the ProvideReferencePageAttribute to the package

3. The package implements IVsComponentSelectorProvider interface
This interface has method GetComponentSelectorPage, which will be called 
when Visual Studio first time load add reference tabs.

So implement the GetComponentSelectorPage method and VSPROPSHEETPAGE 
structure to invoker.The most important thing is its hwndDlg, which should
be the handle of the tab page control.

4. Implement ReferencePageDialog by inheriting from UserControl
Add a ListView control into the user control

5. Handle ListView's OnSelectionChange and OnDoubleClick events
Send CPDN_SELCHANGED and CPDN_SELDBLCLICK messages to its great grandparent
respectively to notify for those two events.

6. Override the WinProc method and handle the following messages:
CPPM_INITIALIZELIST: Initialize the list
CPPM_QUERYCANSELECT: Check if select button is enabled
CPPM_SETMULTISELECT: Set if multiple select is supported
CPPM_CLEARSELECTION: Clear the selection
CPPM_GETSELECTION: Get selected items

7. Initialize the list
Add demo items into ListView control

8. Get selected items
See the HandleGetSelectionMessage method.
m.WParam = the count of the items
m.LParam = the pointer of VSCOMPONENTSELECTORDATA data array


/////////////////////////////////////////////////////////////////////////////
References:

Adding a tab to the Add Reference dialog
http://social.msdn.microsoft.com/Forums/en-US/vsx/thread/ddb0f935-b8ac-400d-9e3d-64d74be85031

IDE Constants
http://msdn.microsoft.com/en-us/library/bb165453.aspx



