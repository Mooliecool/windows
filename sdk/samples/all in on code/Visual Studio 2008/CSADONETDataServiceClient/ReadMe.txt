========================================================================
    CONSOLE APPLICATION : CSADONETDataServiceClient Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

CSADONETDataServiceClient example demonstrates an ADO.NET Data Service client
application. It calls certain ADO.NET Data Services for ADO.NET Entity Data 
Model, Linq To SQL Data Classes, and non-relational in-memory data, and it 
demonstrates these ways (LINQ, ADO.NET query options, custom service 
operations) to update and query the data source. 

Please note: The ADO.NET Data Services URLs need to be modified based on your
system's IIS setting.  


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CSADONETDataServiceClient -> CSADONETDataService
CSADONETDataServiceClient accesses the ADO.NET Data Services created by 
CSADONETDataService.


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the ADO.NET Data Services
client sample that calls certain ADO.NET Data Services for ADO.NET Entity 
Data Model, Linq To SQL Data Classes, and non-relational in-memory data.  

Step 1: Open the DB project SQLServer2005DB, right click the project file and
select Deploy to create the SQLServer2005DB database in your database 
instance.

Step 2: Modify the connection string information to consistent with your 
database instance and account.  

  <add name="SQLServer2005DBConnectionString" connectionString=...
  <add name="SQLServer2005DBEntities" connectionString=...

Step 3: Build the project CSADONETDataService and CSADONETDataServiceClient.

Step 4: Run the output executable file of project CSADONETDataServiceClient, 
CSADONETDataServiceClient.exe.


/////////////////////////////////////////////////////////////////////////////
Creation Steps:

A. Creating an ordinary Visual C# Console Application and adding the 
   ADO.NET Data Services references.

   1. In the same solution of the ADO.NET Data Services project, add a new 
      Visual C# / Windows / Console Application project named 
      CSADONETDataServiceClient.

   2. Right click the References in the Solution Explorer, select 
      Add Service References….

   3. In the Add Service Reference dialog, click Discover, add the three 
      ADO.NET Data Services Samples.svc, SchoolLinqToEntities.svc, and 
      SchoolLinqToSQL.svc, rename the Namespace of the services to 
      SamplesService, SchoolLinqToEntities, and SchoolLinqToSQLService 
      respectively.

B. Creating static ADO.NET Data Services URL fields, local entity classes, 
   and helper methods.

   1. Define static fields for the ADO.NET Data Services URL.  You need to 
      modify the URL to specify the port settings of the localhost.  

   2. Create a local entity class named TempCourse which only have CourseID 
      and Title properties and misses other properties than the Course entity 
      class in the ADO.NET Data Service references.

   3. Create a local entity class named InnerDataServiceException to hold 
      the inner exception information of the DataServiceException.

   4. Create a helper method ParseDataServiceClientException to parse the 
      DataServiceClientException via LINQ to XML methods, and return a local 
      entity class object InnerDataServiceException holding the inner 
      exception information.
                
   5. Create a helper method UpdateData to save the pending modifications of 
      the DataServiceContext.  If all the DataServiceResponse.StatusCode 
      start with 2, since the status code 2** means success.   For detail, 
      please see List of HTTP status codes.  If there are any exception occurs 
      and the InnerException is in type of DataServiceClientException, use 
      the helper method ParseDataServiceClientException to parse the 
      exception information.

C. Updating and querying database via calling ADO.NET Data Service for the 
   ADO.NET Entity Data Model.

   1. Initialize the DataService object for the ADO.NET Entity Data Model.

   2. Create an invalid entity class, add it into the DataService context, 
      and then try to save the pending modification.   Through the helper 
      methods UpdateData and ParseDataServiceClientException, we will see 
      the exception information from the server side: The valid value of 
      PersonCategory is 1 (for students) or 2 (for instructors).
   
   3. Create some valid entity classes and some relationships among them, 
      then save the pending modification.  Call AddTo** to add entity 
      class and call AddLink to add the relationships.  
     
   4. Perform a LINQ query which contains where, orderby, Skip, and Take 
      operators or methods.  These four LINQ operators or methods have 
      their corresponding supported ADO.NET Data Service query options 
      (filter, orderby, skip, top).   How to use the query options will 
      be introduced in the following steps.  
     
   5. Call the service operation CourseByPerson to get the certain person’s 
      courses based on the person table primary key PersonID.  The URL for 
      the service operation CourseByPersonID can be: 

D. Updating and querying database via ADO.NET Data Service for LINQ to SQL 
   Classes.

   1. Initialize the DataService object for the LINQ to SQL Classes.
     
   2. Create some valid entity classes and some relationship entity classes, 
      then save the pending modification.  Since LINQ to SQL does not 
      implement the many-to-many relationships, we need to add the 
      relationship entity as well instead of calling AddLink to create the 
      relationships.  First we add the entity classes, after saving the 
      entity classes, then we add the relationship entity class and save the 
      data.
      
   3. Query the many-to-many relationships through the relationship entity.  
      First we get all CourseGrade entities of one Person based on the 
      PersonID.  The URL can be:
      http://localhost/SchoolLinqToSQL.svc/Persons(PersonID)/CourseGrades.
      By CourseGrade entity, we can get the Course object via it CourseID.  
      The URL can be:
      http://localhost/SchoolLinqToSQL.svc/Courses(CourseID).
      
E. Querying the database via ADO.NET query options and custom service 
   operations.

   1. Initialize the DataService object.  Here we use the ADO.NET Data 
      Service for ADO.NET Entity Data Model for testing.
      
   2. Perform the data source using query options (filter, orderby, skip, 
      top) directly.  The query URL can be:
      http://localhost/SchoolLinqToEntities.svc/Person?$
      filter=PersonCategory eq 1&$orderby=EnrollmentDate desc&$skip=2&$top=2.
              
   3. Initialize another DataService object and set the 
      IgnoreMissingProperties property as true, so that the returned type 
      should be mapped to the client side type.  Here we use the ADO.NET 
      Data Service for LINQ to SQL Classes for testing.
      
   4. Call the service operation SearchCourses and use local entity class 
      LocalCourse to retrieve the returned course information.  The query 
      URL can be:
      http://localhost/SchoolLinqToSQL.svc/SearchCourses?searchText=
      ’SELECT * FROM [Course] AS [c] WHERE [c].[Credits] = 4’.
             
F. Updating and querying non-relational data via ADO.NET Data Service for 
   non-relational data.

   1. Initialize the DataService object for non-relational data.
      
   2. Insert a new entity object into the non-relational data collection.
      
   3. Query the non-relational data based on DataServiceKey value and 
      object links and group the data.
      

/////////////////////////////////////////////////////////////////////////////
References:

ADO.NET Data Services
http://msdn.microsoft.com/en-us/data/bb931106.aspx

Overview: ADO.NET Data Services
http://msdn.microsoft.com/en-us/library/cc956153.aspx

How Do I: Consume an ADO.NET Data Service in a .NET Application
http://msdn.microsoft.com/en-us/data/cc974504.aspx

Using Microsoft ADO.NET Data Services
http://msdn.microsoft.com/en-us/library/cc907912.aspx

ADO.NET Data Services Team Blog
http://blogs.msdn.com/astoriateam/

MSDN API Reference Documentation – Client
http://msdn.microsoft.com/en-us/library/system.data.services.client.aspx

Query Expressions (ADO.NET Data Services)
http://msdn.microsoft.com/en-us/library/cc668784.aspx

Query Functions (ADO.NET Data Services)
http://msdn.microsoft.com/en-us/library/cc668793.aspx

Filter Query Option: $filter (ADO.NET Data Services Framework)
http://msdn.microsoft.com/en-us/library/cc668778.aspx

Order By Query Option: $orderby (ADO.NET Data Services Framework)
http://msdn.microsoft.com/en-us/library/cc668776.aspx

Top Query Option: $top (ADO.NET Data Services Framework)
http://msdn.microsoft.com/en-us/library/cc716654.aspx

Skip Query Option: $skip (ADO.NET Data Services Framework)
http://msdn.microsoft.com/en-us/library/cc668773.aspx

Addressing Resources (ADO.NET Data Services)
http://msdn.microsoft.com/en-us/library/cc668766.aspx

Service Operations (ADO.NET Data Services)
http://msdn.microsoft.com/en-us/library/cc668788.aspx

Interceptors (ADO.NET Data Services)
http://msdn.microsoft.com/en-us/library/dd744842.aspx

Injecting Custom Logic in ADO.NET Data Services 
http://weblogs.asp.net/cibrax/archive/2009/06/08/injecting-custom-logic-in-ado-net-data-services.aspx

List of HTTP status codes
http://en.wikipedia.org/wiki/List_of_HTTP_status_codes


/////////////////////////////////////////////////////////////////////////////