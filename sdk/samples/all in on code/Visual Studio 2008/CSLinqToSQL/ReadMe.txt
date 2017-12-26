========================================================================
    CONSOLE APPLICATION : CSLinqToSQL Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CSLinqToSQL sample demonstrates the Microsoft Language-Integrated Query 
(LINQ) technology to access databases using Visual C#. It shows the basic 
structure of connecting to a data source with LINQ, and how to query and 
manipulate data in database with LINQ. This example contains manually-created 
entities and designer-generated entities.


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CSLinqToSQL -> SQLServer2005DB
CSLinqToSQL accesses the database created by SQLServer2005DB.


/////////////////////////////////////////////////////////////////////////////
Creation:

Please refer to the MSDN article 
http://msdn.microsoft.com/en-us/library/bb387007.aspx
for the typical steps of using LINQ to SQL.

A. Creating the Object Model (LINQ to SQL)

Step1. Select a tool to create the model.

	* The Object Relational Designer 
	
	1. Right-click the project in the Solution Explorer and select Add New 
	Item on the shortcut menu.
	
	2. Select the LINQ to SQL Classes template, and type School.dbml as the 
	name. Click Add.
	
	3. In the Server Explorer window, select the SQLServer2005DB database and
	toss the tables (Person, Course, CourseGrade) onto the O/R designer to
	generate the entities and the DataContext.

	* A code editor
	
	1. Add a class named SchoolDB into the project.
	
	2. In the class, define all the entities as well as their relationship.
	
	3. Define the DataContext class: SchoolDataContext.
	
Step2. Select the kind of code you want to generate.

Step3. Refine the code file to reflect the needs of your application.

B. Using the Object Model 

Step1. Perform the Insert operation

Step2. Perform the Query operations


/////////////////////////////////////////////////////////////////////////////
Usage of designer:

1. From within a C# application, on the Project menu, click Add New Item.

2. Select the LINQ to SQL Classes template.

3. Type Scholl.dbml as the name and click Add.

4. In the Server Explorer windows, select the School Database and drop the 
   tables(Person, Course, CourseGrade) onto the O/R designer to create data 
   classes and DataContext methods.
   
5. Rename the Context Namespace and Entity Namespace to CSLinqToSQL.Designer

6. Build and run the project.


/////////////////////////////////////////////////////////////////////////////
Handle the column that has a default value in the database:

"When you insert an entity using LINQ to SQL you first create an instance of 
that entity.  At this point the DataContext has no means by which to 
determine that you would like certain fields to be assigned their default on 
the server during SubmitChanges.  For example, you give a default value of 10 
for an integer column in the database. When you create the entity that field 
in the object is defaulted to zero. The DataContext cannot distinguish between 
a field with an assigned value of zero and one that is merely unassigned.  So, 
the DataContext must assume that the zero was assigned and send it as part of 
the generated insert statement."             

Possible workaround we use in this example is to set the property to the 
default value in the entity's constructor manually.  


/////////////////////////////////////////////////////////////////////////////
References:

Getting Started (LINQ to SQL)
http://msdn.microsoft.com/en-us/library/bb399398.aspx

Typical Steps for Using LINQ to SQL
http://msdn.microsoft.com/en-us/library/bb387007.aspx

How to: Insert Rows Into the Database (LINQ to SQL)
http://msdn.microsoft.com/en-us/library/bb386941.aspx

LINQ to SQL: .NET Language-Integrated Query for Relational Data
http://msdn.microsoft.com/en-us/library/bb425822.aspx

101 LINQ Samples
http://msdn.microsoft.com/en-us/vcsharp/aa336746.aspx

Linq to SQL: Columns with NOT NULL and DEFAULT
http://social.msdn.microsoft.com/forums/en-US/linqprojectgeneral/thread/f5753fc9-89bd-4f9a-94d9-be616a8d9a14/


///////////////////////////////////////////////////////////////////////////////////