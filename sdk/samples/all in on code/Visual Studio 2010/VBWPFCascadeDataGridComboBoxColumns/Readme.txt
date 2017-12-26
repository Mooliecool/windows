================================================================================
       WPF APPLICATION: VBWPFCascadeDataGridComboBoxColumns Overview                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The sample demonstrates how to show cascade data in the look up list in two DataGrid 
combobox columns.
   
////////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Build the sample project in Visual Studio 2010.

Step2. Select "China" in the first column, and click the cell under the second
column in the same row. You'll see two items "Beijing" and "Shanghai" in the
drop down list of the ComboBox.

Step3. Select "UnitedStates" in the first column, and click the cell under the second
column in the same row. You'll see two items "NewYork" and "Washington" in the
drop down list of the ComboBox.

/////////////////////////////////////////////////////////////////////////////
Code Logic:

Firstly, bind the first column to the parent data source and bind the second 
column to the whole child data source. 

In order to show cascade data in the look up list in the second column, handle its 
PreparingCellEditing event to get the hosted editing element, in this case a 
ComboBox. 

Then rebind the ComboBox so as to show the corresponding child data in the 
ComboBox's drop down list.


/////////////////////////////////////////////////////////////////////////////
References:

http://windowsclient.net/wpf/wpf35/wpf-dg-preview-ctrl-investments.aspx
http://windowsclient.net/wpf/wpf35/wpf-video-datagrid-ctp-preview.aspx 
http://windowsclient.net/wpf/wpf35/wpf-35sp1-toolkit-datagrid-feature-walkthrough.aspx  

/////////////////////////////////////////////////////////////////////////////


