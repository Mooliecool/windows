================================================================================
       WINDOWS FORMS APPLICATION : CSWinFormDesigner Project Overview
       
                   CustomPropertyTab Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CustomPropertyTab sample demonstrates how to add custom PropertyTab on to the 
Properties Windows 
 

/////////////////////////////////////////////////////////////////////////////
Creation:

1. Create a UserControl;

2. Add a reference to the System.Designer.dll;

3. Create an attribute derive from CustomTabDisplayAttribute class with a 
   property of bool type, name it with anything you like, for example:
   "CustomTabDisplayAttribute";

4. Create a custom property tab class derive from PropertiesTab class;

5. Override the PropertiesTab.CanExtend property to make it only accepts our 
   user control type "UC_CumstomPropertyTab";
   
6. Override the PropertiesTab.GetProperties method to return the properties 
   that are marked as Browserable(false) and CustomTabDisplay(true);

7. Override the PropertiesTab.TabName property to give the custom properties 
   tab a name;
   
8. Create some properties in the UserControl, mark the properties you want to
   display on the custom properties tab with Browserable(false) and 
   CustomTabDisplay(true), e.g.
   
        // display on the custom tab
        [Browsable(false)]
        [CustomTabDisplayAttribute(true)]
        public string TestProp
        {
            get { return this.testProp; }
            set { this.testProp = value; }
        }
   
9. Mark the UserControl with Designer attribute to make it use the custom 
   control designer service created by above steps:
   
    [PropertyTab(typeof(CustomTab), PropertyTabScope.Component)]
    public partial class UC_CumstomPropertyTab : UserControl
    {
     ...
    }
    
10. Create a bitmap file, name it with the same name as the type of custom 
    properties tab, in this sample, the name of the bitmap file should be
    "CustomTab";

11. Draw something on the bitmap file;

12. Select the bitmap file in the solution explorer, and switch to the Properties
    Windows, choose "Embedded Resource" from the "Build Action" list;
   
13. Build the project.

14. Now the UserControl is available on the toolbox for use.



/////////////////////////////////////////////////////////////////////////////
References:

1. How do I add a custom tab to the property grid when it is displaying my object's properties?
http://windowsclient.net/blogs/faqs/archive/2006/05/26/how-do-i-add-a-custom-tab-to-the-property-grid-when-it-is-displaying-my-object-s-properties.aspx

2. Windows Forms FAQs
http://windowsclient.net/blogs/faqs/archive/tags/Custom+Designers/default.aspx


/////////////////////////////////////////////////////////////////////////////



