========================================================================
    CONSOLE APPLICATION : CppUseADO Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CppUseADO sample demonstrates the Microsoft ActiveX Data Objects (ADO) 
technology to access databases using #import and Visual C++. It shows the 
basic structure of connecting to a data source, issuing SQL commands, using 
the Recordset object and performing the cleanup.


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CppUseADO -> SQLServer2005DB
CppUseADO accesses the database created by SQLServer2005DB.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Connect to the data source. (ADODB::Connection15::Open)

2. Build and execute an ADO command. (ADODB::Command15::Execute)
The command can be a SQL statement (SELECT/UPDATE/INSERT/DELETE), or a stored
procedure call. 

3. Use the Recordset object. (ADODB::Recordset15::Open / MoveFirst / MoveNext 
/ Fields / Update / UpdateBatch)
Recordset represents the entire set of records from a base table or the 
results of an executed command. It facilitates the enumeration, insertion, 
update, deletion of records. At any time, the Recordset object refers to 
only a single record within the set as the current record.

4. Clean up objects before exit. (ADODB::Recordset15::Close, 
ADODB::Connection15::Close)


/////////////////////////////////////////////////////////////////////////////
References:

ADO Code Examples in Visual C++
http://msdn.microsoft.com/en-us/library/ms677563.aspx

Microsoft ActiveX Data Objects (ADO)
http://msdn.microsoft.com/en-us/library/ms877664.aspx

SAMPLE: Vcspnp.exe Demonstrates Passing SQL NULL Parameters and Reading NULL 
Values with ADO
http://support.microsoft.com/kb/229088

ADO Data Types
(The table below shows the ADO Data Type mapping between Access, SQL Server)
http://www.w3schools.com/ADO/ado_datatypes.asp

Loading image files from a database, using ADO
http://www.codeproject.com/KB/database/ADOImageDB.aspx


/////////////////////////////////////////////////////////////////////////////
