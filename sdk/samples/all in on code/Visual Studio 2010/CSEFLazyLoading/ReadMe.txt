=============================================================================
     CONSOLE APPLICATION : CSEFLazyLoading Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CSEFLazyLoading example illustrates how to work with the Lazy Loading
which is new in Entity Framework 4.0.  It also shows how to use the eager
loading and explicit loading which is already implemented in the first
version of Entity Framework.   


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

1. Please attach the database file EFDemoDB.mdf under the folder 
_External_Dependencies to your SQL Server 2008 database instance.

2. Please modify the connection string in the App.config according to your
database instance name.


/////////////////////////////////////////////////////////////////////////////
Creation:

1. Create an ADO.NET Entity Data Model 
   1) Name it LazyLoading.edmx.
   2) Set the connection string information of the EFDemoDB database.
   3) Select the data tables Department and Course.

2. Create static method LazyLoadingTest() to demostrate how to use 
   lazy loading for related entities.

3. Create static method EagerLoadingTest() to demostrate how to use 
   eager loading for related entities.
   ObjectContext.ContextOptions.LazyLoadingEnabled = false to turn off
   lazy loading in EF4.
   ObjectQuery<T>.Include() to specify the related objects to include in the 
   query results.

4. Create static method ExplicitLoadingTest() to demostrate how to use 
   explicit loading for related entities.
   ObjectContext.ContextOptions.LazyLoadingEnabled = false to turn off
   lazy loading in EF4.
   EntityCollection<TEntity>.Load() to load related objects into the 
   collection.


/////////////////////////////////////////////////////////////////////////////
References:

Loading Related Objects (Entity Framework)
http://msdn.microsoft.com/en-us/library/bb896272.aspx

How to: Use Lazy Loading to Load Related Objects (Entity Framework)
http://msdn.microsoft.com/en-us/library/dd456846.aspx

How to: Use Query Paths to Shape Results (Entity Framework)
http://msdn.microsoft.com/en-us/library/bb738449.aspx

How to: Explicitly Load Related Objects (Entity Framework)
http://msdn.microsoft.com/en-us/library/bb896249.aspx

Is Lazy Loading in EF 4 Evil or the Second Coming?
http://www.robbagby.com/entity-framework/is-lazy-loading-in-ef-4-evil-or-the-second-coming/

Getting Started with Entity Framework 4 – Lazy Loading
http://geekswithblogs.net/iupdateable/archive/2009/11/26/getting-started-with-entity-framework-4-ndash-lazy-loading.aspx


/////////////////////////////////////////////////////////////////////////////
