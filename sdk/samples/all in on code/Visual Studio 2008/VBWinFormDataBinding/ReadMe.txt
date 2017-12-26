================================================================================
       WINDOWS FORMS APPLICATION : VBWinFormDtaBinding Project Overview
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The VBWinFormDataBinding sample demonstrates the Windows Forms Data Binding 
technology. It shows how to perform simple binding and complex binding in a 
Windows Forms application. It also shows how to navigate through items in a 
data source. 


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Simple binding example 1.

   This example binds TextBox.Text property to CheckBox.Checked property using 
   simple binding, so that when the CheckBox is checked, the TextBox shows
   "true", otherwise shows "false".

2. Simple binding example 2.

   This example bind TextBox.Text property to Form.Size property using simple 
   binding with update mode set to DataSourceUpdateMode.Never, so that the data
   source won't update unless we explicitly call the Binding.WriteValue() method.
   
3. Simple binding example 3.

   This example bind TextBox.Text property to one of the fields of a DataTable.

   1). Create a DataTable with two columns and several rows of data.
   
   2). Bind TextBox.Text property to one of the fields of the DataTable through a 
   BindingSource object.
   
   3). Call BindingSource.MovePrevious, BindingSource.MoveNext methods to navigate 
   through the records.
   
4. Complex binding example 1.

   This example demonstrates how to display data from database in a DataGridView 
   by Visual Studio designer.
            
   Steps:
	 
   1). Click the smart tag glyph (Smart Tag Glyph) on the upper-right corner of 
	   the DataGridView control.
   2). Click the drop-down arrow for the Choose Data Source option.
   3). If your project does not already have a data source, click 
	   "Add Project Data Source.." and follow the steps indicated by the wizard. 
   4). Expand the Other Data Sources and Project Data Sources nodes if they are 
	   not already expanded, and then select the data source to bind the control to. 
   5). If your data source contains more than one member, such as if you have 
	   created a DataSet that contains multiple tables, expand the data source, 
	   and then select the specific member to bind to. 

5. Complex binding example 2.
   
   This example use business objects as data source for data binding.
   
6. Complex binding example 3.

   This example demonstrates how to perform Master/Detail binding in Windows Forms.
   
   1). Create a master DataTable and a detail DataTable.
   2). Add some records to both tables.
   3). Create a DataSet object to hold both table.
   4). Add a relationship to he DataSet.
   5). Create two BindingSource objects, one for the master table, the other for the 
       corresponding detail records.
   6). Use the BindingSource objects to bind the DataGridView controls.
   

/////////////////////////////////////////////////////////////////////////////
References:

1. Roadmap for Windows Forms data binding
   http://support.microsoft.com/kb/313482

2. Windows Forms Data Binding
   http://msdn.microsoft.com/en-us/library/ef2xyb33.aspx

3. Windows Forms Data Controls and Databinding FAQ
   http://social.msdn.microsoft.com/Forums/en-US/winformsdatacontrols/thread/a44622c0-74e1-463b-97b9-27b87513747e


/////////////////////////////////////////////////////////////////////////////