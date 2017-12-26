========================================================================
    CONSOLE APPLICATION : CSASPNETGridView Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

This CSASPNETGridView project describes how to populate ASP.NET GridView 
control and how to implement Insert, Edit, Update, Delete, Paging 
and Sorting functions in ASP.NET GridView control.

This project includes two samples: DataInMemory and DataFromDatabase.

DataInMemory populates GridView with a simple DataTable. The DataTable 
is stored in ViewState to persist data across postbacks. 

DataFromDatabase populates GridView with data from SQL Server database in 
ADO.NET way. The sample uses the GriView database.  

/////////////////////////////////////////////////////////////////////////////
Implementation:

Step1. Create a C# ASP.NET Web Application named CSASPNETGridView in 
Visual Studio 2010 / Visual Web Developer.

Step2. Drag a GridView control, a LinkButton control and a Panel control 
into an ASP.NET Web Form page.

	(1) Rename the controls as follows:

    GridView1    -> gvPerson
    LinkButton1  -> lbtnAdd
    Panel1		 -> pnlAdd
    
    (2) Change the Text property of lbtnAdd to AddNew.
    
	(3) Right-click on gvPerson, select Show Smart Tag -> Auto Format, 
	choose style Oceanica and press OK to save.
	
	(4) On Show Smart Tag, select Add New Columns, choose CommandField, 
	check Delete, Edit/Update and Show cancel button then press OK.
	
	(5) On Show Smart Tag, select Add New Columns, choose BoundField, 
	and add the following three columns:
	
	Header text		Data field		Read only
	-----------------------------------------
	PersonID		PersonID		Y
	LastName		LastName		N
	FirstName		FirstName		N
	
		
	(6) On Show Smart Tag, select Edit Columns, 
	un-check Auto-generate fields, select LastName field, 
	and click Convert this field into a TemplateField. 
	Do the same operation to FirstName field. 
	
	Related references:
	
	ASP.NET: Using TemplateFields in the GridView Control	
	MSDN: TemplateField Class
	
	
Step3. Copy the following methods from the sample, 
and paste them in code-behind:

	DataInMemory.aspx.cs
	
		Page_Load Method:
		Initialize underlying objects, when the Page is accessed 
		for the first time.

		InitializeDataSource Method:
		Initialize the DataTable and store it in ViewState.

		BindGridView Method:
		Set the sort column and sort order and bind the GridView 
		control with a DataTable in ViewState.

		
	DataFromDatabase.aspx.cs
	
		Page_Load Method:
		Initialize underlying objects, when the Page is accessed 
		for the first time.
		
		BindGridView	Method:
		Set the sort column and sort order and bind the GridView 
		control with a SQL Server table.
		
	Related references:
	
	MSDN: using Statement (C# Reference)
	MSDN: Understanding ASP.NET View State
	
	
Step4. Drag two TextBox controls and two LinkButton controls into 
pnlAdd.

	(1) Rename the controls as follows:

    TextBox1     -> tbLastName
    TextBox2	 -> tbFirstName
    LinkButton1  ->	lbtnSubmit
    LinkButton2  ->	lbtnCancel
    
    (2) Change the Text properties of lbtnSubmit and to Submit 
    and Cancel.    
	
Step5. Navigate to the Property panel of gvPerson and then switch to Event. 
Double-click on the following events to generate the Event handlers. 
After that, fill the generated methods with the sample code.


	(1)	RowDataBound Event: Occurs when a data row is bound to 
	data in a GridView control.

	In this event, we add a client-side confirmation dialog box that 
	appears when the Delete button is clicked. It will prevent deleting a 
	row accidentally.
	
	Related references:
	
	MSDN: GridView.RowDataBound Event
	ASP.NET: Editing, Inserting, and Deleting Data
	ASP.NET: Adding Client-Side Confirmation When Deleting
	MSDN: WebControl.Attributes Property
	MSDN: DataControlRowType Enumeration
	MSDN: GridViewRow.RowState Property 
	

	(2)	PageIndexChanging Event: Occurs when one of the pager buttons is 
	clicked, but before the GridView control handles the paging operation.

	In other to show data in the new page, we need to set the index of new 
	page and then rebind the GridView control to show data in view mode. 	
   
	When clicking the Edit button to edit a particular row, the GridVew 
	control will enter the edit mode and show Update and Cancel buttons.
	
	Related reference:
	
	MSDN: GridView.PageIndexChanging Event


	(3)	RowEditing Event: Occurs when a row's Edit button is clicked, 
	but before the GridView control enters edit mode.
	
	To make the GridView control into edit mode for the select row, 
	we need to set the index of the row to edit and then rebind the 
	GridView control to render data in edit mode.  	
	
	Related references:
	
	MSDN: GridView.RowEditing Event
	MSDN: GridView.EditIndex Property 
	

	(4)	RowCancelingEdit Event: Occurs when the Cancel button of a row 
	in edit mode is clicked, but before the row exits edit mode.

	We can click the Cancel button to cancel the edit mode and show data 
	in normal view mode.

	In this Event, we need to set the zero-based GridView.EditIndex 
	property to -1, which means no row is being edited, and then rebind 
	the GridView to show data in view mode.
	
	////////////////////////////////////////////////////////////////
	gvPerson.EditIndex = -1;
	BindGridView();
	////////////////////////////////////////////////////////////////
	
	Related reference:
	
	MSDN: GridView.RowCancelingEdit Event


	(5)	RowUpdating Event: Occurs when a row's Update button is clicked, 
	but before the GridView control updates the row.

	After modifying values in the selected row, we click the Update button 
	to save changes back to the data source. 

	To identify the person for editing, the PersonID value is required, 
	which is read-only and cannot be modified.
	
	////////////////////////////////////////////////////////////////
	string strPersonID = gvPerson.Rows[e.RowIndex].Cells[2].Text;
	////////////////////////////////////////////////////////////////

	e.RowIndex is the index of current row.

	In Step2 we converted LastName and FirstName to TemplateFields, so we 
	cannot get the edit values directly. Since LastName and FirstName are 
	both string values, Label controls are generated in ItemTemplate for 
	displaying values and TextBox controls are generated in EditItemTemplate 
	for editing values.

	We can access the cells and fetch the values in the following way:
	
	////////////////////////////////////////////////////////////////
	string strLastName = 
	((TextBox)gvPerson.Rows[e.RowIndex].FindControl("TextBox1")).Text;
	
	string strFirstName = 
	((TextBox)gvPerson.Rows[e.RowIndex].FindControl("TextBox2")).Text;
	////////////////////////////////////////////////////////////////

	After fetch these values, we can save them back to the DataTable in 
	ViewState or the table in SQL Server.
	
	Related references:	
	
	MSDN: GridView.RowUpdating Event
	ASP.NET: Editing, Inserting, and Deleting Data


	(6)	RowDeleting Event: Occurs when a row's Delete button is clicked, 
	but before the GridView control deletes the row.

	To identify the person for deleting, the PersonID value is required, 
	which is read-only and cannot be modified. 
	
	////////////////////////////////////////////////////////////////
	string strPersonID = gvPerson.Rows[e.RowIndex].Cells[2].Text;
	////////////////////////////////////////////////////////////////

	After fetching the PersonID, we can delete the person from the DataTable 
	in ViewState or the table in SQL Server.
	
	Related references:	
	
	MSDN: GridView.RowDeleting Event
	ASP.NET: Editing, Inserting, and Deleting Data

	
	(7)	Sorting Event: Occurs when the hyperlink to sort a column is 
	clicked, but before the GridView control handles the sort operation.

	The SortDirection property on the GridView is changed only when the 
	GridView is bound to a DataSource control using the DataSourceID 
	property. Otherwise, sort direction always return "Ascending" and 
	needs to be manipulated manually.

	In the Page_Load Event, we store a default sorting expression 
	in ViewState.
	
	////////////////////////////////////////////////////////////////
	ViewState["SortExpression"] = "PersonID ASC";
	////////////////////////////////////////////////////////////////

	And set the sort column and sort order based on it in BindGridView method.
	
	////////////////////////////////////////////////////////////////
	dvPerson.Sort = ViewState["SortExpression"].ToString();
	////////////////////////////////////////////////////////////////

	So when first visiting the page, all Person record will be shown in 
	ascending order of PersonID. 
	
	When clicking a column’s header to sort 
	this column, we need to get previous sort column and sort order and  
	compare the sort column with the current column.

	If they are same, we just change the sort order to show data in the 
	other order, e.g. ascending to descending or descending to ascending.

	If not, we specify the current column as the sort column and set sort 
	order to ASC. The sort expression is stored into ViewState to persist 
	data across postbacks.
	
	Related references:	
	
	MSDN: DataView.Sort Property 
	MSDN: GridView.Sorting Event
	

Step6. Double-click on the Click event of lbtnAdd to generate the Event 
handlder and fill the generated methods with the sample code. Do the same 
operations to lbtnSubmit and lbtnCancel. 

	lbtnAdd.Click Event:	
	Hide the Add button and showing Add panel.	
	
	lbtnSubmit.Click Event:
	Fetch the values of the TextBox controls and add new row to the 
	DataTable in ViewState or the table in SQL Server.

	lbtnCancel.Click Event:
	Show the Add button and hiding the Add panel. 



/////////////////////////////////////////////////////////////////////////////
References:

MSDN: using Statement (C# Reference)
http://msdn.microsoft.com/en-us/library/yh598w02.aspx

MSDN: Understanding ASP.NET View State
http://msdn.microsoft.com/en-us/library/ms972976.aspx

ASP.NET: Using TemplateFields in the GridView Control
http://www.asp.net/learn/data-access/tutorial-12-cs.aspx

MSDN: TemplateField Class
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.templatefield.aspx

MSDN: GridView.RowDataBound Event
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.gridview.rowdatabound.aspx

ASP.NET: Editing, Inserting, and Deleting Data
http://www.asp.net/learn/data-access/#editinsertdelete

ASP.NET: Adding Client-Side Confirmation When Deleting
http://www.asp.net/learn/data-access/tutorial-22-cs.aspx

MSDN: WebControl.Attributes Property
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.webcontrol.attributes.aspx

MSDN: DataControlRowType Enumeration
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.datacontrolrowtype.aspx

MSDN: GridViewRow.RowState Property 
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.gridviewrow.rowstate.aspx

MSDN: GridView.PageIndexChanging Event
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.gridview.pageindexchanging.aspx

MSDN: GridView.RowEditing Event
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.gridview.rowediting.aspx

MSDN: GridView.EditIndex Property 
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.gridview.editindex.aspx

MSDN: GridView.RowCancelingEdit Event
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.gridview.rowcancelingedit.aspx

MSDN: GridView.RowUpdating Event
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.gridview.rowupdating.aspx

MSDN: GridView.RowDeleting Event
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.gridview.rowdeleting.aspx

MSDN: DataView.Sort Property 
http://msdn.microsoft.com/en-us/library/system.data.dataview.sort.aspx

MSDN: GridView.Sorting Event
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.gridview.sorting.aspx

/////////////////////////////////////////////////////////////////////////////
