========================================================================
    CONSOLE APPLICATION : CSEFForeignKeyAssociation Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

CSEFForeignKeyAssociation example demonstrates one of the new features of
Entity Framework(EF) 4.0, Foreign Key Association.  This example compares 
the new Foreign Key Association and the Independent Association and shows 
how to insert new related entities, insert by existing entities and update
existing entities with the two associations.


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

1. Please attach the database file EFDemoDB.mdf under the folder 
_External_Dependencies to your SQL Server 2008 database instance.

2. Please modify the connection string in the App.config according to your
database instance name.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

Foreign Key Association:

1. Create an Entity Data Model with Foreign Key Association.  
   1) Create folder FKAssociation.
   2) Add an ADO.NET Entity Data Model named FKAssociation.edmx into the 
      folder FKAssociation.  
   3) Set the connection string information of the EFDemoDB database.
      Note: Please see the "Prerequisite" section first!
   4) Select the data tables Course and Department.
      Note: Remember to select the check box "Include the foreign key columns 
      "in the model" to allow the Foreign Key Association!

2. Create a static class named FKAssociationClass to test the insert and 
   update methods with the Foreign Key Association.
   1) Create method InsertNewRelatedEntities to insert a new Course and its 
      Department entity by Foreign Key Association.
   2) Create method InsertByExistingEntities to insert a new Course and set 
      it belong to an existing Department by Foreign Key Association.
   3) Create method UpdateExistingEntities to update an existing Course 
      entity as well as its relationship.
   4) Create method Query to query the Course entities and the corresponding 
      Department entities.
   5) Create method Test to run the insert and update methods with the 
      Foreign Key Association.

Independent Association:

1. Create an Entity Data Model with Independent Association.
   1) Create a folder IndependentAssociation.
   2) Add an ADO.NET Entity Data Model named IndependentAssociation.edmx into
      the folder IndependentAssociation.
   3) Set the connection string information of the EFDemoDB database.
      Note: Please see the "Prerequisite" section first!
   4) Select the data tables Course and Department.
      Note: Remember to uncheck the check box "Include the foreign key columns 
      "in the model" to allow the Independent Association!

2. Create a static class named IndependentAssociationClass to test the insert
   and update methods with the IndependentAssociation.
   1) Create method InsertNewRelatedEntities to insert a new Course and its 
      Department entity by Independent Association.
   2) Create method InsertByExistingEntities to insert a new Course and set 
      it belong to an existing Department by Independent Association.
   3) Create method UpdateExistingEntities to update an existing Course 
      entity (only regular properties).
   4) Create method Query to query the Course entities and the corresponding 
      Department entities.
   5) Create method Test to run the insert and update methods with the 
      Independent Association.


/////////////////////////////////////////////////////////////////////////////
References:

Defining and Managing Relationships (Entity Framework)
http://msdn.microsoft.com/en-us/library/ee373856%28VS.100%29.aspx

How to: Use EntityReference Object to Change Relationships Between Objects(Entity Framework)
http://msdn.microsoft.com/en-us/library/cc716754(VS.100).aspx

How to: Use the Foreign Key Property to Change Relationships Between Objects
http://msdn.microsoft.com/en-us/library/ee473440(VS.100).aspx

Foreign Keys in the Entity Framework
http://blogs.msdn.com/efdesign/archive/2009/03/16/foreign-keys-in-the-entity-framework.aspx

Foreign Keys in the Conceptual and Object Models
http://blogs.msdn.com/efdesign/archive/2008/10/27/foreign-keys-in-the-conceptual-and-object-models.aspx

Foreign Key Relationships in the Entity Framework
http://blogs.msdn.com/adonet/archive/2009/11/06/foreign-key-relationships-in-the-entity-framework.aspx


/////////////////////////////////////////////////////////////////////////////