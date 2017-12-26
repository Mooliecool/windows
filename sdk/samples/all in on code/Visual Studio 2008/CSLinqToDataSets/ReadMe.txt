========================================================================
    CONSOLE APPLICATION : CSLinqToDataSets Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CSLinqToDataSets sample demonstrates the Microsoft Language-Integrated 
Query (LINQ) technology to access untyped DataSet and strong typed DataSet 
using Visual C#. 


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CSLinqToDataSets -> SQLServer2005DB
CSLinqToDataSets accesses the database created by SQLServer2005DB.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

Query the untyped DataSet:

1. Fill the untyped DataSet and insert data into database.
   (System.Data.SqlClient.SqlDataAdapter.Fill)
   (System.Data.SqlClient.SqlDataAdapter.Update)
   
2. Perform the query operation in one DataTable and display the query results.

3. Perform the query operation across multiple DataTables and display the 
   query results.
   
Query the strong typed DataSet:

1. Create the strong typed DataSet.

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
   
2. Fill the strong typed DataSet and insert data into database.
   
3. Perform the query operation in one DataTable and display the query results.

4. Perform the query operation across multiple DataTables and display the 
   query results.
   
5. Perform the query operation across multiple related DataTables and display
   the query results.
   

/////////////////////////////////////////////////////////////////////////////
References:

LINQ to DataSet
http://msdn.microsoft.com/en-us/library/bb386977.aspx

LINQ to DataSet Samples
http://msdn.microsoft.com/en-us/vbasic/bb688086.aspx

101 LINQ Samples
http://msdn.microsoft.com/en-us/vcsharp/aa336746.aspx

Usage of LINQ and DataSets in Compact Framework 3.5
http://www.codeproject.com/KB/mobile/DatasetsAndLINQ.aspx

Querying DataSets - Introduction to LINQ to DataSet
http://blogs.msdn.com/adonet/archive/2007/01/26/querying-datasets-introduction-to-linq-to-dataset.aspx


///////////////////////////////////////////////////////////////////////////////////