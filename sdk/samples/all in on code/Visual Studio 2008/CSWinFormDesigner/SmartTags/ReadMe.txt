================================================================================
       WINDOWS FORMS APPLICATION : CSWinFormDesigner Project Overview
       
                   SmartTagSupport Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The SmartTagSupport sample demonstrates how to add smart tags for a control 
at design time.
 

/////////////////////////////////////////////////////////////////////////////
Creation:

1. Create a UserControl;

2. Add a reference to the System.Designer.dll;'

3. Create a class derive from the DesignerActionList class;

4. Override the DesignerActionList.GetSortedActionItems method to define 
   smart tag entries and resultant actions.

5. Create a class derive from the ControlDesigner class;

6. Override the ControlDesigner.ActionLists property to populate smart tag menu;
   
7. Mark the UserControl with Designer attribute to make it use the custom 
   control designer service created by above steps:
   
   [Designer(typeof(UC_SmartTagSupportDesigner))]
    public partial class UC_SmartTagSupport : UserControl
    {
     ...
    }
    
6. Build the project.

7. Now the UserControl is available on the toolbox for use.



/////////////////////////////////////////////////////////////////////////////
References:

1. Walkthrough: Adding Smart Tags to a Windows Forms Component
http://msdn.microsoft.com/en-us/library/ms171829.aspx

2. Windows Forms FAQs
http://windowsclient.net/blogs/faqs/archive/tags/Custom+Designers/default.aspx


/////////////////////////////////////////////////////////////////////////////



