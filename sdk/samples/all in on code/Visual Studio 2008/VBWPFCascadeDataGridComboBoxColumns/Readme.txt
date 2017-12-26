================================================================================
       WPF Cascade DataGridcomboBoxColumn Project Overview                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The sample demonstrates how to show cascade the loop up list in two DataGrid 
combobox columns.
   

/////////////////////////////////////////////////////////////////////////////

Firstly, bind the first column to the parent data source and bind the second 
column to the whole child data source. 

In order to show cascade look up list in the second column, handle its 
PreparingCellEditing event to get the hosted editing element, in this case a 
ComboBox. 

Then rebind the ComboBox so as to show the corresponding child data in the 
ComboBox's drop down list.


/////////////////////////////////////////////////////////////////////////////
References:

   

/////////////////////////////////////////////////////////////////////////////
Demo

Step1. Build the sample project in Visual Studio 2008.

Step2. Select "China" in the first column, and click the cell under the second
column in the same row. You'll see two items "Beijing" and "Shanghai" in the
drop down list of the ComboBox.

Step3. Select "UnitedStates" in the first column, and click the cell under the second
column in the same row. You'll see two items "NewYork" and "Washington" in the
drop down list of the ComboBox.


/////////////////////////////////////////////////////////////////////////////