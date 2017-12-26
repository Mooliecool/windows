================================================================================
WPF APPLICATION : CSWPFAnimationWhenDataChanged Project Overview
       
                       CSWPFAnimationWhenDataChanged Sample
                        
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