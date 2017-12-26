========================================================================
     WEB APPLICATION : VBADONETDataService Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The VBADONETDataService example demonstrates the ADO.NET Data Services for 
three different data sources, ADO.NET Entity Data Model, Linq to SQL Data
Classes, and non-relational in-memory data. 

Please note: The connection strings of in Web.config file need to be modified
according to the SQL Server database instancea nd accout information in your 
system.


/////////////////////////////////////////////////////////////////////////////
Project Relation:

VBADONETDataService -> SQLServer2005DB
VBADONETDataService accesses the database created by SQLServer2005DB.


/////////////////////////////////////////////////////////////////////////////
Creation Steps:

A. Creating an ordinary ASP.NET Web Application

   1. In Visual Studio 2008, add a new Visual Basic.NET/ Web / ASP.NET Web 
      Application project named VBADONETDataService.

   2. Delete the web page Default.aspx and the App_Data folder.  

   3. Add two new folders named LinqToEntities and LinqToSQL respectively.

B. Creating an ADO.NET Data Service for ADO.NET Entity Data Model.

   1. Creating an ADO.NET Entity Data Model as the data source in 
      Visual Studio 2008.

      1) In the folder LinqToEntities, add a new Entity Data Model named 
         SchoolLinqToEntities.edmx.

      2) In the Entity Data Model Wizard, configure the connection string 
         and database information, set the entity name as 
         SQLServer2005DBEntities, and Data Model name as SQLServer2005DBModel.
         (Connect to the SQLServer2005DB database deployed by the 
          SQLServer2005DB database project in All-In-One Code Framework)

      3) In Tables tab, select tables Course, CourseGrade, CourseInstructor, 
         and Person.
         (For detail, please see How to: Create a New Entity Data Model.)

    2. Creating and configuring an ADO.NET Data Service for the 
       ADO.NET Entity Data Model.

      1) Add a new ADO.NET Data Service named SchoolLinqToEntities.svc.

      2) Set the SchoolLinqToEntities Data Service class targeting to the 
         SQLServer2005DBEntities.

      3) Create a service operation CoursesByPersonID to retrieve the 
         instructor’s courses list by the primary key PersonID.

      4) Create a service operation GetPersonUpdateException to retrieve the 
         information of the Person table update exception.

      5) Create a service query interceptor to filter the Course collection 
         to only return the Course objects whose CourseID is larger than 4000.

      6) Create a service change interceptor to check the PersonCategory value 
         of the newly added or modified Person objects.

      7) Override the HandleException method to throw 400 Bad Request 
         exception to the client side.

      8) Set rules to indicate which entity sets and service operations are 
         visible, updatable, etc.

C. Creating an ADO.NET Data Service for LINQ to SQL Classes.

   1. Creating a LINQ to SQL Class as the data source in Visual Studio 2008.

      1) In LinqToSQL folder, add a LINQ to SQL Class named 
         SchoolLinqToSQL.dbml.

      2) Drag the tables Course, CourseGrade, CourseInstructor, and Person 
         from the Server Explorer into the O/R Designer.  For detail, please 
         see Walkthrough: Creating LINQ to SQL Classes (O/R Designer).
         (The connection in Server Explorer is connecting to the 
         SQLServer2005DB database deployed by the SQLServer2005DB database 
         project in All-In-One Code Framework)

   2. Implementing the IUpdatable interface to enable the insert, update, 
      and delete functionalities of the LINQ to SQL DataContext class.

      1) Download the ADO.NET Data Services IUpdatable implementation for 
         LINQ to SQL (It has both Visual C# and VB.NET versions).   
         This implementation realizes most of IUpdatable methods targeting 
         LINQ to SQL via .NET Reflection and LINQ to SQL related APIs, and 
         also leaves some methods for the customization.   For detailed 
         information, please see IUpdateable for Linq To Sql and 
         Updated Linq to Sql IUpdatable implementation 

      2) Add the downloaded Visual C# version IUpdatableLinqToSQL.cs to 
         LinqToSQL folder, and modify the DataContext class name to 
         SchoolLinqToSQLDataContext.

    3. Set the data service keys for the LINQ to SQL data classes using 
       DataServiceKey attribute.

    4. Creating and configuring an ADO.NET Data Service for the LINQ to SQL 
       Classes.

       1) Add a new ADO.NET Data Service named SchoolLinqToSQL.svc.

       2) Set the SchoolLinqToSQL Data Service class targeting to the 
          SchoolLinqToSQL.

       3) Create a service operation SearchCourses to search the Course 
          objects via T-SQL commands and retrieve an IQueryable collection 
          of certain Course objects.

       4) Set rules to indicate which entity sets and service operations are 
          visible, updatable, etc.

D. Creating an ADO.NET Data Service for non-relation CLR objects.  

   1. Add a new ADO.NET Data Service named Samples.svc.

   2. Creating the in-memory data classes.

      1) Create class Category with property CategoryName to represent the 
         sample project category, and set the data service key value as 
         CategoryName.

      2) Create class Project with properties ProjectName, Owner, and 
         ProjectCategory to represent the sample project, and set the data 
         service key value as ProjectName.

   3. Creating the ADO.NET Data Service entity class for non-relational 
      CLR objects.

      1) Create a class named SampleProjects and set the Samples.svc Data 
         Service targeting to the SampleProjects class.
 
      2) Declare two static members for SampleProjects class, categories 
         in type of List<Category> and projects in type of List<Project>, 
         and a static constructor for SampleProjects class to initialize 
         the two static members.

      3) Declare two public readonly properties named Categories and 
         Projects to return IQueryable representation of the two static 
         members categories and projects, so that the ADO.NET Data Service 
         client side can retrieve the non-relation data via these two 
         IQuerable properties.

    4. Implementing the IUpdatable methods to enable the insert 
       functionality of the ADO.NET Data Service entity class SampleProjects.

       1) Declare a temp object to hold the temporarily added objects 
          (Category or Project).

       2) Realize the method GreateResource to create the resource of the 
          given type and belonging to the given container via 
          Activator.CreateInstance method.

       3) Realize the SetValue method to set the value of the given property 
          on the target object via .NET Reflection.

       4) Realize the ResolveResource method to return the actual instance of 
          the resource represented by the given resource object.

       5) Realize the SaveChanges method to save all the pending changes 
          made till now.   This method first checks the temporarily added 
          object’s type (Category or Project) and then adds it into the 
          certain collection (categories or projects).

       6) Other IUpdatable methods can throw the NotImplementedException if 
          we only want the insert functionality currently.  These methods 
          include AddReferenceToCollection, ClearChanges, DeleteResouce, 
          GetResource, GetValue, RemoveReferenceFromCollection, ResetResource, 
          and SetReference.  I have implemented some of them for your 
          references in the source code of VBADONETDataService and 
          VBADONETDataService samples.

       7) Set rules to indicate which entity sets and service operations are 
          visible, updatable, etc.


/////////////////////////////////////////////////////////////////////////////
References:

ADO.NET Data Services
http://msdn.microsoft.com/en-us/data/bb931106.aspx

Overview: ADO.NET Data Services
http://msdn.microsoft.com/en-us/library/cc956153.aspx

How Do I: Getting Started with ADO.NET Data Services over a Relational 
Database
http://msdn.microsoft.com/en-us/data/cc745957.aspx

How Do I: Getting Started with ADO.NET Data Services over a Non-Relational 
Data Source
http://msdn.microsoft.com/en-us/data/cc745968.aspx

How to: Create a New Entity Data Model
http://msdn.microsoft.com/en-us/library/cc716703.aspx

Walkthrough: Creating LINQ to SQL Classes (O/R Designer)
http://msdn.microsoft.com/en-us/library/bb384428.aspx

MSDN API Reference Documentation – Server
http://msdn.microsoft.com/en-us/library/system.data.services.aspx

Using Microsoft ADO.NET Data Services
http://msdn.microsoft.com/en-us/library/cc907912.aspx

ADO.NET Data Services Team Blog
http://blogs.msdn.com/astoriateam/

ADO.NET Data Services IUpdateable implementation for Linq to Sql
http://code.msdn.microsoft.com/IUpdateableLinqToSql

IUpdateable for Linq To Sql 
http://blogs.msdn.com/aconrad/archive/2008/11/05/iupdateable-for-linq-to-sql.aspx

Updated Linq to Sql IUpdatable implementation 
http://blogs.msdn.com/aconrad/archive/2009/03/17/updated-linq-to-sql-iupdatable-implementation.aspx

Service Operations (ADO.NET Data Services)
http://msdn.microsoft.com/en-us/library/cc668788.aspx

Interceptors (ADO.NET Data Services)
http://msdn.microsoft.com/en-us/library/dd744842.aspx

Injecting Custom Logic in ADO.NET Data Services
http://weblogs.asp.net/cibrax/archive/2009/06/08/injecting-custom-logic-in-ado-net-data-services.aspx

IUpdatable &amp; ADO.NET Data Services Framework
http://blogs.msdn.com/astoriateam/archive/2008/04/10/iupdatable-ado-net-data-services-framework.aspx


/////////////////////////////////////////////////////////////////////////////