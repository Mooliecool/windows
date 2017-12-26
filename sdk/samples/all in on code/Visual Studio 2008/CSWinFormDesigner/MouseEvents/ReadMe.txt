================================================================================
       WINDOWS FORMS APPLICATION : CSWinFormDesigner Project Overview
       
                   DetectMouseEvent Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The DetectMouseEvent sample demonstrates how to detect mouse events at design 
time.


/////////////////////////////////////////////////////////////////////////////
Creation:

1. Create a UserControl;

2. Add a reference to the System.Designer.dll;

3. Create a class derive from the ControlDesigner class;

4. Override the following methods as you need:
   
   ControlDesigner.GetHitTest
   ControlDesigner.OnMouseEnter
   ControlDesigner.OnMouseLeave
   
   
5. Mark the UserControl with Designer attribute to make it use the custom 
   control designer service created by above steps:
   
   [Designer(typeof(UC_DetectMouseEventDesigner))]
    public partial class UC_DetectMouseEvent : UserControl
    {
     ...
    }
    
6. Build the project.

7. Now the UserControl is available on the toolbox for use.



/////////////////////////////////////////////////////////////////////////////
References:

1. ControlDesigner.GetHitTest  Method 
http://msdn.microsoft.com/en-us/library/system.windows.forms.design.controldesigner.gethittest.aspx

2. Windows Forms FAQs
http://windowsclient.net/blogs/faqs/archive/tags/Custom+Designers/default.aspx


/////////////////////////////////////////////////////////////////////////////