========================================================================
    OFFICE ADD-IN : VBOutlookUIDesigner Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The VBOutlookUIDesigner provides samples on how to customize Office UI using
the VSTO Designers. It will demonstrate all Ribbon Controls available in the
Ribbon Designer. It will also demonstrate how to add FormRegions and 
TaskPanes to the Outlook UI.


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Ribbon

In order to make the ribbon show corresponding controls on different types
of Inspectors, we need to:
1. Group the controls on the designer. One group per type of Inspector.
2. In the Ribbon's Load event, get a reference to current Inspector using
   the Context property.
3. Determine the type of the Inspector by checking the CurrentItem property.
   Sample: If TypeOf inspector.CurrentItem Is MailItem Then  ... End If
4. Set the Visible property of corresponding Group on Ribbon to show / hide
   the controls.
   
MyRibbon uses both custom PNG files stored in project resource file and
Office built in images as control images. For those controls using Office
built in images, please note their OfficeImageId property.

B. Custom Task Pane (CTP)

In order to associate a CTP with specific types of Inspectors, we need to:
1. Handle the Application.Inspectors.NewInspector event.
2. In the event handler, get the item type of the Inspector and decide
   whether to associate a CTP with the Inspector.
3. If the item type is right, we can use ThisAddIn's CustomTaskPanes.Add
   method to have the job done.

In order to access the parent CTP / Window object within our UserControl
code, we need to:
1. Add a property of type CustomTaskPane to the UserControl.
2. After creating the CTP, pass the CTP reference to the property created
   in step 1.
3. Within the UserControl code, access the property to get the CTP reference.
   
In MyRibbon class, we have a button once clicked, will show the CTP associated
with the current Inspector. In order to achieve this, we need to:
1. Go through the Globals.ThisAddIn.CustomTaskPanes, compare each CTP's
   Window property with current Inspector.
2. If Window = Current Inspector, that CTP is the one associated with the
   Inspector.
3. Do whatever necessary with the CTP.

C. FormRegion
To get the current Outlook item within the FormRegion code, use:
Me.OutlookItem

To get the current Outlook Inspector within the FormRegion code, use:
Me.OutlookFormRegion.Inspector


/////////////////////////////////////////////////////////////////////////////
References:

Ribbon Designer
http://msdn.microsoft.com/en-us/library/bb386089.aspx

Office Ribbon Class
http://msdn.microsoft.com/en-us/library/microsoft.office.tools.ribbon.officeribbon.aspx

CustomTaskPane Class
http://msdn.microsoft.com/en-us/library/microsoft.office.tools.customtaskpane.aspx


/////////////////////////////////////////////////////////////////////////////