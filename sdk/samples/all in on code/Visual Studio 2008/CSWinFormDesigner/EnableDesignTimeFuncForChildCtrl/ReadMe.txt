================================================================================
       WINDOWS FORMS APPLICATION : CSWinFormDesigner Project Overview
       
                   EnableDesignTimeFuncForChildCtrl Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The EnableDesignTimeFuncOnChildCtrl sample demonstrates how to enable design 
time functionality on for a child control.


/////////////////////////////////////////////////////////////////////////////
Creation


Main Steps:

1. Create a UserControl named "EnableDesignTimeFuncOnChildCtrl";

2. Drag a Panel control to the UserControl;

3. Create a public property named "Panel1" with only a getter; 

4. Add a reference to the System.Designer.dll;

5. Create a class named "UC_EnableDesignTimeFuncForChildCtrlDesigner" derive 
   from the ControlDesigner class;

6. Override the ControlDesigner.Initialize method and call EnableDesignMode 
   method to enable design time functionality for the panel;
   
7. Specify the Designer attribute for the UserControl as follows:

     [Designer(typeof(UC_EnableDesignTimeFuncForChildCtrlDesigner))]

8. Enable design time serialization on the panel by setting the 
   DesignerSerializationVisibility attribute as follows:
   
     [DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
   
9. Build the project, after the compiling succeed, a UserControl 
   "EnableDesignTimeFuncOnChildCtrl" should be appear on the toolbox; 
   
10. Drag the "EnableDesignTimeFuncOnChildCtrl" UserControl from the toolbox 
    to the form;
    
11. You'll find you're able to edit the panel, you  can also drag controls 
    onto the panel;
    
    
/////////////////////////////////////////////////////////////////////////////
References:

1. DesignerAttribute Class
   http://msdn.microsoft.com/en-us/library/system.componentmodel.designerattribute.aspx
   
2. ControlDesigner.EnableDesignMode Method 
   http://msdn.microsoft.com/en-us/library/system.windows.forms.design.controldesigner.enabledesignmode.aspx
   
3. DesignerSerializationVisibilityAttribute Class
   http://msdn.microsoft.com/en-us/library/system.componentmodel.designerserializationvisibilityattribute.aspx
   

/////////////////////////////////////////////////////////////////////////////

