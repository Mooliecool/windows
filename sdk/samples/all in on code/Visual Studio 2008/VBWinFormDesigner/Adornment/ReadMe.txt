================================================================================
       WINDOWS FORMS APPLICATION : VBWinFormDesigner Project Overview
       
                   AdornmentPainting Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The AdornmentPainting sample demonstrates how to paint additional adornments 
on top of the control.  


/////////////////////////////////////////////////////////////////////////////
Creation:

1. Create a UserControl;

2. Add a reference to the System.Designer.dll;

3. Create a class derive from the ControlDesigner class;

4. Override the ControlDesigner.OnPaintAdornments method to do the custom 
   painting you need;
   
5. Mark the UserControl with Designer attribute to make it use the custom 
   control designer service created by above steps:
   
   <Designer(GetType(UC_AdornmentPaintingDesigner))> _
	Public Class UC_AdornmentPainting
		...
	End Class
    
6. Build the project.

7. Now the UserControl is available on the toolbox for use.



/////////////////////////////////////////////////////////////////////////////
References:

1. ControlDesigner.OnPaintAdornments Method 
http://msdn.microsoft.com/en-us/library/system.windows.forms.design.controldesigner.onpaintadornments.aspx

2. Windows Forms FAQs
http://windowsclient.net/blogs/faqs/archive/tags/Custom+Designers/default.aspx


/////////////////////////////////////////////////////////////////////////////