========================================================================
    EXCEL WORKBOOK : VBVstoExcelWorkbook Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The VBVstoExcelWorkbook provides the examples on how to customize Excel 
Workbooks by using the ListObject and the document Actions Pane. 


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

The code sample requires Microsoft Excel 2007.


/////////////////////////////////////////////////////////////////////////////
Creation:

Actions Pane

1. Create a User Control named CourseQueryPane.
2. In ThisWorkbook class, add the User Control to the Actions Pane using the
   following code:
   
   this.ActionsPane.Controls.Add(new CourseQueryPane());
   
List Object

1. Drag & drop a ListObject from the Toolbox onto Sheet1.
2. Set the DataSource property of the ListObject.
3. At runtime, update related tables and the ListObject will reflect the data.

/////////////////////////////////////////////////////////////////////////////
References:

ListObject Class:
http://msdn.microsoft.com/en-us/library/microsoft.office.tools.excel.listobject.aspx

ActionsPane Class:
http://msdn.microsoft.com/en-us/library/microsoft.office.tools.actionspane.aspx

/////////////////////////////////////////////////////////////////////////////