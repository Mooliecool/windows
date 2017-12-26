================================================================================
WPF APPLICATION : VBWPFAnimationWhenDataChanged Project Overview
       
                       VBWPFAnimationWhenDataChanged Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The sample demonstrates how to trigger animation when  cell value in the datagrid
changed.
   

/////////////////////////////////////////////////////////////////////////////
Code Logic:

   1. create a style for DataGrid cell.
   2. In the cell style, we add Binding.TargetUpdated event triggers. 
   3. When the trigger condition is fulfilled, start the animation. 
   4. In the AutoGeneratingColumn event handler, we set NotifyOnTargetUpdated
      to true in order to trigger TargetUpdated event.
   5. In a button click event handler, apply the cell style to each column.
   
 
 
   
/////////////////////////////////////////////////////////////////////////////
References:


/////////////////////////////////////////////////////////////////////////////
Demo

Step1. Build the sample project in Visual Studio 2008.

Step2. Start Without Debugging, and the mian window of the project will 
show.

Step3. Select one item of the data grid, and the data will show in the two 
textboxes below.

Step4. Change the value in the textboxes, and the value in the data grid 
item will change correspond with the textboxes.

Step5. Click the button in the bottom of the window, and then repeat Step4, 
and it will show an animation when the data changes.
/////////////////////////////////////////////////////////////////////////////