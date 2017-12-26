=============================================================================
     CONSOLE APPLICATION : VBEFComplexType Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The VBEFComplexType example illustrates how to work with the Complex Type
which is new in Entity Framework 4.0.  It shows how to add Complex Type 
properties to entities, how to map Complex Type properties to table columns,
and how to map a Function Import to a Complex Type.


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

1. Please attach the database file EFDemoDB.mdf under the folder 
_External_Dependencies to your SQL Server 2008 database instance.

2. Please modify the connection string in the App.config according to your
database instance name.


/////////////////////////////////////////////////////////////////////////////
Creation:

1. Create an ADO.NET Entity Data Model 
   1) Name it ComplexType.edmx.
   2) Set the connection string information of the EFDemoDB database.
   3) Select the data tables Person and PersonAddress, and select the 
      GetPersonNameByPersonID Stored Procedure.

2. Delete the entity PersonAddress.
   Note: When the dialog "Delete Unmapped Tables and Views" shows, select 
   "No" to keep the PersonAddress table in the SSDL. 

3. Add Complex Type PersonDate.
   1) Right click the EDM designer -> Add -> Complex Type to add a new Complex
      Type and set its name to PersonDate. 
   2) Right click the Complex Type PersonDate -> Add -> Scalar Property -> 
      DateTime to add three DateTime properties to the PersonDate type.
      (HireDate, EnrollmentDate and AdminDate)  
   3) Set all the three properties's Nullable value to True.

4. Add Complex Type PersonAddress.
   1) Right click the EDM designer -> Add -> Complex Type to add a new Complex
      Type and set its name to PersonAddress. 
   2) Right click the Complex Type PersonDate -> Add -> Scalar Property -> 
      String to add two String properties to the PersonDate type.
      (Address and Postcode)  
   3) Set both the two properties's Nullable value to True.

5. Add PersonDate Complex Type property into Person entity.
   1) Delete the HireDate, EnrollmentDate, and AdminDate in the Person entity.
   2) Right click the Person entity -> Add -> Complex Property to add a new
      Complex property named PersonDate in type of PersonDate.

6. Similar with step 7 to add a new Complex Property named PersonAddress in
   type of PersonAddress into the Person entity.

7. Map the Complex Properties to table columns.
   1) Right click the Person entity -> Table Mapping to open the Mapping 
      Details window.
   2) Map the HireDate, EnrollmentDate, and AdminDate columns to 
      PersonDate.HireDate, PersonDate.EnrollmentDate, and PersonDate.AdminDate.
   3) Add a Table to PersonAddress table.
   4) Map the PersonID column to PersonID property and Address, Postcode 
      columns to Complex Property PersonAddress.Address and 
	  PersonAddress.Postcode.

8. Add a Function Import of the stored procedure GetPersonNameByPersonID.
   1) Right click the EDM designer -> Add -> Function Import... to open the 
      "Add Function Import" dialog.
   2) Select Stored Procedure Name "GetPersonNameByPersonID" and make it as 
      the name of the Function Import.
   3) Click the "Get Column Information" button to get the stored procedure
      return value's column information. 
   4) Click the "Create New Complex Type" button to create a new Complex Type
      based on the stored procedure return value.
   5) Change the newly created Complex Type name to PersonName and click "OK".

9. Build the project.


/////////////////////////////////////////////////////////////////////////////
References:

Complex Type Objects (Entity Framework)
http://msdn.microsoft.com/en-us/library/bb738472(VS.100).aspx

How to: Add a Complex Type Property to an Entity (Entity Data Model Tools)
http://msdn.microsoft.com/en-us/library/dd456823(VS.100).aspx

How to: Create and Modify Complex Types (Entity Data Model Tools)
http://msdn.microsoft.com/en-us/library/dd456820(VS.100).aspx

How to: Map Complex Type Properties to Table Columns (Entity Data Model Tools)
http://msdn.microsoft.com/en-us/library/dd456822(VS.100).aspx

How to: Map a Function Import to a Complex Type (Entity Data Model Tools)
http://msdn.microsoft.com/en-us/library/dd456824(VS.100).aspx


/////////////////////////////////////////////////////////////////////////////
