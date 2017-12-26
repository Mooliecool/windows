========================================================================
    CONSOLE APPLICATION : CppUseADONET Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CppUseADONET example demonstrates the Microsoft ADO.NET technology to 
access databases using Visual C++ in both managed code and unmanaged code. 
It shows the basic structure of connecting to a data source, issuing SQL 
commands, using DataSet object and performing the cleanup. 


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CppUseADONET -> SQLServer2005DB
CppUseADONET accesses the database created by SQLServer2005DB.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Enable the support of CLR in Project Properties / Configuration 
   Properties / General / Set Common Language Runtime Support as "Common 
   Language Runtime Support (/clr)".

2. Connect to data source.  (System::Data::SqlClient::SqlConnection->Open)

3. Build and Execute an ADO.NET Command.
   (System::Data::SqlCommand->ExecuteNonQuery)
   It can be a SQL statement (SELECT/UPDATE/INSERT/DELETE), or a stored 
   procedure call.
   
4. Use the DataSet Object.  (System::Data::SqlClient::SqlDataAdapter->Fill)
   The DataSet, which is an in-memory cache of data retrieved from a data 
   source, is a major component of the ADO.NET architecture.  The DataSet 
   consists of a collection of DataTable objects that you can relate to each 
   other with DataRelation objects.

5. Clean up objects before exit.  
   (System::Data::SqlClient::SqlConnection->Close)


/////////////////////////////////////////////////////////////////////////////
References:

ADO.NET Introduction
http://msdn.microsoft.com/en-us/library/e80y5yhx.aspx

Data Access Using ADO.NET in C++
http://msdn.microsoft.com/en-us/library/9ctka9db.aspx

How to: Marshal a VARIANT for ADO.NET
http://msdn.microsoft.com/en-us/library/ms235266.aspx

How to: Marshal Unicode Strings for ADO.NET
http://msdn.microsoft.com/en-us/library/ms235208.aspx

C/C++ Preprocessor Reference (managed, unmanaged)
http://msdn.microsoft.com/en-us/library/0adb9zxe.aspx

/clr (Comman Language Runtime Compilation)
http://msdn.microsoft.com/en-us/library/k8d11d4s.aspx

MSDN: DataSet
http://msdn.microsoft.com/en-us/library/system.data.dataset.aspx

MSDN: SqlDataAdapter
http://msdn.microsoft.com/en-us/library/system.data.sqlclient.sqldataadapter.aspx


/////////////////////////////////////////////////////////////////////////////