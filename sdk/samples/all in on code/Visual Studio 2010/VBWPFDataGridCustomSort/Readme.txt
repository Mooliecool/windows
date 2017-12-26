================================================================================
          WPF APPLICATION: VBWPFDataGridcustomSort Overview
================================================================================

////////////////////////////////////////////////////////////////////////////////
Summary:

The sample demonstrates how to implement a custom sort for
one or several columns in the WPF DataGrid control.

////////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Build the sample project in Visual Studio 2010.

Step2. Select "Job" column, and click this column header. You'll see the datagrid sorts custom 
data type by "Job" kind, custom defines sort by "CTO" > "Manager" > "Leader" > "Member".

Step3. Click on "Job" column header once again. You'll see the datagrid sorts the custom data type
by the reverse order as before.


////////////////////////////////////////////////////////////////////////////////
Code Logic:

Firstly, define a class to implement IComparer interface. IComparer.Compare method
can compare two objects while the column is sorting.


Then specify the CustomSort property of the bound view to sort the custom column
in the sorting event handler.

////////////////////////////////////////////////////////////////////////////////
References:

http://social.msdn.microsoft.com/Forums/en/wpf/thread/a60d179d-082d-44c5-b3a1-340d51d22164
http://www.codeproject.com/KB/WPF/WPFListViewSorter.aspx


////////////////////////////////////////////////////////////////////////////////