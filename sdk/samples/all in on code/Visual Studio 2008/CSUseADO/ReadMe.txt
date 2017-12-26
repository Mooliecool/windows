========================================================================
    CONSOLE APPLICATION : CSUseADO Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CSUseADO sample demonstrates the Microsoft ADO technology to access 
databases using Visual C#. It shows the basic structure of connecting to a 
data source, issuing SQL commands, using Recordset object and performing the 
cleanup.


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CSUseADO -> SQLServer2005DB
CSUseADO accesses the database created by SQLServer2005DB.


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Add the reference to Microsoft ActiveX Data Objects 2.X Library in the 
COM tab of the Add Reference dialog of the project. Visual Studio generates 
the interop assembly of the ADO COM component so that the developers can 
early-bind to the ADO COM objects.

Step2. Connect to data source. (ADODB.Connection.Open)

Step3. Build and Execute an ADO Command.(ADODB.Command.Execute)
It can be a SQL statement (SELECT/UPDATE/INSERT/DELETE), or a stored 
procedure call.

Step4. Use the Recordset Object. (ADODB.Recordset.Open / MoveFirst / MoveNext 
/ Fields / Update / UpdateBatch)
Recordset, which is an in-memory cache of data retrieved from a data source, 
is a major component of the ADO architecture. The ADO Recordset object is 
used to hold a set of records from a database table. A Recordset object 
consists of records and columns (fields). When you first open a Recordset, 
the current record pointer will point to the first record and the BOF and EOF 
properties are False. If there are no records, the BOF and EOF property are 
True.

4. Clean up objects before exit. (ADODB.Connection.Close, 
ADODB.Recordset.Close)


/////////////////////////////////////////////////////////////////////////////
References:

HOW TO: Open ADO Connection and RecordSet Objects in Visual C# .NET
http://support.microsoft.com/kb/308611


/////////////////////////////////////////////////////////////////////////////