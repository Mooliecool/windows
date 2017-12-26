========================================================================
    CONSOLE APPLICATION : CSUseADONET Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CSUseADONET example demonstrates the Microsoft ADO.NET technology to 
access databases using Visual C#. It shows the basic structure of 
connecting to a data source, issuing SQL commands, using Untyped DataSet,
using Strong Typed DataSet, updating related data tables and performing the 
cleanup. 


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CSUseADONET -> SQLServer2005DB
CSUseADONET accesses the database created by SQLServer2005DB.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Connect to data source. (System.Data.SqlClient.SqlConnection.Open)

2. Build and Execute an ADO.NET Command. 
   (System.Data.SqlCommand.ExecuteNonQuery)
   It can be a SQL statement (SELECT/UPDATE/INSERT/DELETE), or a stored 
   procedure call.
   
3. Use the Untyped DataSet Object. 
   (System.Data.SqlClient.SqlDataAdapter.Fill)
   The DataSet, which is an in-memory cache of data retrieved from a data 
   source, is a major component of the ADO.NET architecture.  The DataSet 
   consists of a collection of DataTable objects that you can relate to each 
   other with DataRelation objects.
   
4. Use the Strong Typed DataSet Object.
   A typed DataSet is a class that derives from a DataSet. As such, it 
   inherits all the methods, events, and properties of a DataSet. 
   Additionally, a typed DataSet provides strongly typed methods, events, and
   properties. This means you can access tables and columns by name, instead 
   of using collection-based methods. Aside from the improved readability of 
   the code, a typed DataSet also allows the Visual Studio .NET code editor 
   to automatically complete lines as you type.
   
   Steps to create a strong typed DataSet in Visual Studio 2008:
   
   (1) On the "Data" menu, click Show Data Sources. 
   
   (2) In the "Data Sources" window, click "Add New Data Source" to start the 
       "Data Source Configuration Wizard". 
       
   (3) On the "Choose a Data Source Type" page, click "Database" and then 
       click "Next". 
       
   (4) On the "Choose Your Data Connection" page, perform one of the 
       following actions: 
			If a data connection to the Northwind sample database is available 
			in the drop-down list box, click it. 
			-or- 
			Click "New Connection" to open the "Add/Modify Connection" dialog 
			box. For more information, please see 
			http://msdn.microsoft.com/en-us/library/c3t1z354.aspx. 
			
   (5) If the database requires a password, select the option to include 
	   sensitive data, and then click "Next". 
	   
   (6) Click "Next" on the "Save the Connection String to the Application 
	   Configuration File" page. 

   (7) Expand the "Tables" node on the "Choose Your Database Objects" page. 

   (8) Click the check boxes for the tables you want in the DataSet, and then
       click "Finish". 
       
   (9) Click the "PersonCategory" column of the "Person" DataTable to open 
       the properties window.  Set the "DefaultValue" property to 1.
       
   To create a strong typed DataSet by XSD.exe, please see
   http://msdn.microsoft.com/en-us/library/wha85tzb.aspx
   
5. Update Two Related Data Tables by TableAdapterManager and writting codes 
   manually.   
   
   Steps when we use TableAdapterManager:
   (1) Create a Windows Form application.
       For detail, please see 
       http://msdn.microsoft.com/en-us/library/54xbah2z.aspx.
       
   (2) Create a strong typed DataSet. 
       For detail, please see Item 4.  
   
   (3) Change the default data-bound controls to be created.
       After populating the Data Sources window, you can choose the controls 
       to be created when you drag items to a Windows Form. For this 
       example, the data from the Department table will be displayed in 
       individual controls (Details). The data from the Course table will be 
       displayed in a DataGridView control (DataGridView). 

   (4) Create the data-bound Form.
       After choosing the controls in the Data Sources window, create the 
       data-bound controls by dragging items in the Data Sources window onto 
       the form.
       
   (5) Update the code to commit changes to the related before saving.
       Add a line of code to call the courseBindingSource.EndEdit method 
       after the line that calls the departmentBindingSource.EndEdit method
       in departmentBindingNavigatorSaveItem_Click event.
       
   (6) Add code to commit parent records in the DataSet before adding new
	   child records
	   Create an event handler for the courseBindingSource.AddingNew event. 
	   Add to the event handler a line of code that calls the 
	   DepartmentBindingSource.EndEdit method.

   Steps when we write code manually:
   (1) Create a Windows Form application.
       For detail, please see 
       http://msdn.microsoft.com/en-us/library/54xbah2z.aspx.
       
   (2) Create a strong typed DataSet. 
       For detail, please see Item 4.  
   
   (3) Change the default data-bound controls to be created.
       After populating the Data Sources window, you can choose the controls 
       to be created when you drag items to a Windows Form. For this 
       example, the data from the Department table will be displayed in 
       individual controls (Details). The data from the Course table will be 
       displayed in a DataGridView control (DataGridView). 

   (4) Create the data-bound Form.
       After choosing the controls in the Data Sources window, create the 
       data-bound controls by dragging items in the Data Sources window onto 
       the form.
       
   (5) Add code to update the database.
       Replace the code in the event handler to call the Update methods of 
       the related TableAdapterManager.
       
   Note: For clarity this example(Updating related data tables with manually
   written code) does not use a transaction, but if you are updating two or 
   more related tables, then you should include all the update logic within 
   a transaction. A transaction is a process that assures all related changes 
   to a database are successful before committing any changes. 
   For more information, please see 
   http://msdn.microsoft.com/en-us/library/777e5ebh.aspx.

6. Clean up objects before exit. (System.Data.SqlClient.SqlConnection.Close)


/////////////////////////////////////////////////////////////////////////////
References:

ADO.NET Introduction
http://msdn.microsoft.com/en-us/library/e80y5yhx.aspx

Using DataSets in ADO.NET
http://msdn.microsoft.com/en-us/library/ss7fbaez.aspx

Storing and Retrieving Images from SQL Server using Visual C# .NET
http://www.codeproject.com/KB/database/ImageSaveInDataBase.aspx

Handling Null Values (ADO.NET)
http://msdn.microsoft.com/en-us/library/ms172138.aspx

Best Practices for Using ADO.NET
http://msdn.microsoft.com/en-us/library/ms971481.aspx

How to Call a Parameterized Stored Procedure by Using ADO.NET and Visual 
C#.NET
http://support.microsoft.com/kb/310070/

Typed DataSets (ADO.NET)
http://msdn.microsoft.com/en-us/library/esbykkzb.aspx

Generating Strongly Typed DataSets (ADO.NET)
http://msdn.microsoft.com/en-us/library/wha85tzb.aspx

HOW TO: Create and Use a Typed DataSet by Using Visual C# .NET
http://support.microsoft.com/kb/320714/en-us

Saving Data from Related Data Tables (Hierarchical Update)
http://msdn.microsoft.com/en-us/library/bb384432.aspx

Saving Data to a Database (Multiple Tables)
http://msdn.microsoft.com/en-us/library/4esb49b4.aspx

MSDN: DataSet
http://msdn.microsoft.com/en-us/library/system.data.dataset.aspx

MSDN: SqlDataAdapter
http://msdn.microsoft.com/en-us/library/system.data.sqlclient.sqldataadapter.aspx


/////////////////////////////////////////////////////////////////////////////