=============================================================================
         CONSOLE APPLICATION : VBEFPOCOChangeTracking Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The VBEFPOCOChangeTracking example illustrates how to update POCO entity
properties and relationships with change tracking proxy and without change
tracking proxy.


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the sample.

Step1. After you successfully build the VBEFPOCOChangeTracking sample project 
in Visual Studio 2008, you will get the applications: 
VBEFPOCOChangeTracking.exe.

Step2. Attach the database file EFDemoDB.mdf under the folder 
_External_Dependencies to your SQL Server 2008 database instance.

Step3. Modify the connection string in the App.config according to your 
SQL Server 2008 database instance name.

Step4. Run the VBEFPOCOChangeTracking.exe, you will see the following outputs:

  Update properties with change tracking proxy...
  Entity State before modification: Unchanged
  Entity State after modification: Modified

  Update properties without change tracking proxy...
  Entity State before modification: Unchanged
  Entity State after modification: Unchanged
  Entity State after DetectChanges is called: Modified

  Update relationships with change tracking proxy...
  Is the newly created object a proxy? True
  1 object(s) added

  Update relationships without change tracking proxy...
  Is the newly created object a proxy? False
  Before DetectChanges is called, 0 object(s) added
  After DetectChanges is called, 1 object(s) added

  Press [Enter] to exit...


/////////////////////////////////////////////////////////////////////////////
Implementation:

1. Download ADO.NET C# POCO Entity Generator (see References) and install it 
   in Visual Studio 2010.  

2. Create a new C# console project named VBEFPOCOChangeTraching.

3. Add a new ADO.NET Entity Data Model item named POCOChangeTracking.edmx,
   Entity Container name is POCOChangeTrackingEntities and Namespace is
   POCOChangeTrackingModel.  Select the database EFDemoDB, tables 
   Department and Course.

4. After the Entity Data Model is created, right click the designer and 
   choose "Add Code Generation Item...", then select "ADO.NET POCO Entity
   Generator".

5. Write sample codes to update POCO entity properties and relationships 
   with change tracking proxy and without change tracking proxy.
   
   1) By default, EF creates proxies for POCO entities, so we can directly
      update the entity properties and EF will know the modification via
	  POCO change tracking proxy.

   2) If we do not use change tracking proxies, we need to turn off the 
      proxy creation and call DetectChanges after all the modification has
	  been performed.

   3) If the proxy creation is enabled, we need to use 
      ObjectSet.CreateObject to manually create a proxy.

   4) Turn off the proxy creation.  We need to call DetectChanges after the
      relationships have been updated. 


/////////////////////////////////////////////////////////////////////////////
References:
ADO.NET C# POCO Entity Generator
http://visualstudiogallery.msdn.microsoft.com/en-us/23df0450-5677-4926-96cc-173d02752313

POCO Proxies Part1 (ADO.NET team blog)
http://blogs.msdn.com/b/adonet/archive/2009/12/22/poco-proxies-part-1.aspx

POCO Proxies Part 2: Serializing POCO Proxies (ADO.NET team blog)
http://blogs.msdn.com/b/adonet/archive/2010/01/05/poco-proxies-part-2-serializing-poco-proxies.aspx

POCO in the Entity Framework: Part 3 – Change Tracking with POCO (ADO.NET team blog)
http://blogs.msdn.com/b/adonet/archive/2009/06/10/poco-in-the-entity-framework-part-3-change-tracking-with-poco.aspx


/////////////////////////////////////////////////////////////////////////////