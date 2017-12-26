========================================================================
    CONSOLE APPLICATION : VBLinqToEntities Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The VBLinqToEntities example demonstrates the Microsoft Language-Integrated 
Query (LINQ) technology to access ADO.NET Entity Data Model using VB.NET. 
It shows the basic structure of connecting and querying the data source and
inserting data into the database with LINQ.  It especially demonstrates the
new features of LINQ to Entities comparing with LINQ to SQL, Inheritance
and Many-to-Many Relationship (without payload).


/////////////////////////////////////////////////////////////////////////////
Project Relation:

VBLinqToEntities -> SQLServer2005DB
VBLinqToEntities accesses the database created by SQLServer2005DB.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Generating ADO.NET Entity Data Model in Visual Studio 2008.
   
   Steps:
   (1) Right-click on the project and add a new item. Add an 
       "ADO.NET Entity Data Model" and call it "School.edmx" to correspond to 
       your database.
   
   (2) The Entity Data Model Wizard shows up and you now can use this to query 
       your database and generate the model diagram, as long as you supply it 
       with the right credentials. In the Wizard, click "Generate from Database" 
       and click "Next".
      
   (3) Supply it with the right server name, authentication, credentials, and 
       the database name.
      
   (4) Modify "Save entity connections settings in App.Config as" to 
       "SchoolEntities" to correspond to your database.
      
   (5) In the next dialog box, choose all of the options—tables, views, and 
       stored procedures—and the model will be generated for you.  Here we 
       choose the table, Course, CourseGrade, CourseInstructor, Person.  
       
2. Create Table-per-Hierarchy Inheritance.  
   
   Steps:
   (1) Right Click on the model diagram. From the context menu select 
       "Add" -> "Entity".
   (2) On Add Entity dialog window, set Entity name to "Student". Set Base 
       type to "Person", this should define the inheritance. Note that once 
       you define Base type, all other properties on the dialog will be 
       disabled. Click "Ok".
       
   (3) Delete "EnrollmentDate" property on "Person" Entity as it will be 
       specific "Student" Entity. Do the same for "HireDate" as it will be 
       specific for "Instructor" Entity.
       
   (4) Right-click on "Student" Entity. Select "Add" -> "Scalar Property". 
       Name the property "EnrollmentDate". Set its data type on the property 
       window to DateTime. 

3. Define data mappings:

   Steps:
   (1) Right-click on "Student" Entity and select "Table Mapping". This 
       should display Mapping Details Pan for "Student" Entity.
    
   (2) From Mapping Details pan under Tables click on <Add a Table or View> 
       and select "Person".
       
   (3) Click on <Add a Condition> and select "PersonCategory". Leave the 
       Operator as "=" and set the Value/Property to 1. 
       
   (4) Under Column Mappings make sure that "EnrollmentDate" column is mapped 
       to "EnrollmentDate" property. 

   Repeat the above steps for Instructor Entity and set the mapping condition 
   to its proper values. Delete the "PersonCategory" property from "Person" 
   Entity as it is not needed anymore.
   
4. Perform the Insert operation and display the query results.

5. Perform the query operation in one data table and display the query 
   results.

6. Perform the query operation across multiple data tables and display the 
   query results.

7. Perform the query operation across multiple related data tables and 
   display the query results.

8. Perform the query operation accross Many-to-Many related data tables and 
   display the query results.
   
   
/////////////////////////////////////////////////////////////////////////////
References:

LINQ to Entities
http://msdn.microsoft.com/en-us/library/bb386964.aspx

ADO.NET Entity Framework
http://msdn.microsoft.com/en-us/library/bb399572.aspx

Introducing LINQ to Relational Data
http://msdn.microsoft.com/en-us/library/cc161164.aspx

Defining Advanced Data Models
http://msdn.microsoft.com/en-us/library/bb738640.aspx

How to: Define a Model with Table-per-Type Inheritance (Entity Framework)
http://msdn.microsoft.com/en-us/library/bb738685.aspx

How to: Define a Model with Table-per-Hierarchy Inheritance (Entity Framework)
http://msdn.microsoft.com/en-us/library/bb738443.aspx

Entity Data Model 101: Part 1
http://blogs.msdn.com/adonet/archive/2007/01/30/entity-data-model-part-1.aspx

Entity Data Model 101: Part 2
http://blogs.msdn.com/adonet/archive/2007/02/12/entity-data-model-101-part-2.aspx

Inheritance in the Entity Framework
http://blogs.msdn.com/adonet/archive/2007/03/15/inheritance-in-the-entity-framework.aspx

ADO.NET Entity Framework Tutorial and Basics
http://www.codeguru.com/csharp/csharp/cs_linq/article.php/c15489/

Inheritance and Associations with Entity Framework Part 1
http://mosesofegypt.net/post/Inheritance-and-Associations-with-Entity-Framework-Part-1.aspx


/////////////////////////////////////////////////////////////////////////////