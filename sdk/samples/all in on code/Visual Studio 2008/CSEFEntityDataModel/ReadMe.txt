=============================================================================
     CONSOLE APPLICATION : CSEFEntityDataaModel Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This example illustrates how to work with EDM in various ways. It includes 
many to many association, one to many association, one to one association, 
table merging, table splitting, table per hierarchy inheritance, and table 
per type inheritance. In the example, you will see the  insert, update and 
query operations to entities.


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

1, Please attach database file EFDemoDB.mdf under the foler 
_External_Dependencies to your SQL Server 2005 Express or SQL Server 2005 
database instance.  

2. Please modify the connection string in the App.config file according to 
your database instance name. 


/////////////////////////////////////////////////////////////////////////////
Creation:

/////////////////////////////////////////////////////////////////////////////
Many To Many Association
/////////////////////////////////////////////////////////////////////////////

Tables:(main related columns)
[Course]
 CourseID [PK]
 Title
 
[CourseInstructor]
 CourseID [PK] [FK]
 PersonID [PK] [FK]
 
[Person]
 PersonID [PK]
 LastName
 FirstName
 
Create EDM Steps:
1) Add -> New Item -> ADO.NET Entity Data Model.
2) Choose EFDemoDB -> Select the three tables above.
3) Get the model with many to many association.
4) Build the solution.


/////////////////////////////////////////////////////////////////////////////
One To Many Association
/////////////////////////////////////////////////////////////////////////////

Tables:(main related columns)
[Department]
DepartmentID [PK]
Name

[Course]
 CourseID [PK]
 Title
 DepartmentID [FK]
 
Create EDM Steps:
1) Add -> New Item -> ADO.NET Entity Data Model.
2) Choose EFDemoDB -> Select the two tables above.
3) Get the model with one to many association.
4) Build the solution.


/////////////////////////////////////////////////////////////////////////////
One To One Association
/////////////////////////////////////////////////////////////////////////////

Tables:(main related columns)
[Person]
 PersonID [PK]
 LastName
 FirstName

[PersonAddress]
 PersonID [PK] [FK]
 Address
 Postcode
 
Create EDM Steps:
1) Add -> New Item -> ADO.NET Entity Data Model.
2) Choose EFDemoDB -> Select the two tables above.
3) Get the model with one to one association.
4) Build the solution.


/////////////////////////////////////////////////////////////////////////////
Table Merging
/////////////////////////////////////////////////////////////////////////////

Tables:(main related columns)
[Person]
 PersonID [PK]
 LastName
 FirstName

[PersonAddress]
 PersonID [PK] [FK]
 Address
 Postcode
 
Create EDM Steps:
1) Add -> New Item -> ADO.NET Entity Data Model.
2) Choose EFDemoDB -> Select the two tables.
3) Cut(Ctrl+X) the Address and Postcode properties in PersonAddress entity 
   and paste(Ctrl+V) it to Person entity.
4) Delete the PersonAddress entity.
5) Right click Person entity -> Table Mapping -> Add a Table or View -> 
   Choose PersonAddress table in the dropdown list.
6) Build the solution.


/////////////////////////////////////////////////////////////////////////////
Table Per Hierarchy Inheritance
/////////////////////////////////////////////////////////////////////////////

Tables:(main related columns)
[Person]
 PersonID [PK]
 LastName
 FirstName
 PersonCategory
 HireDate
 EnrollmentDate
 Picture
 BusinessCredits
 AdminDate
 
Create EDM Steps:
1) Add -> New Item -> ADO.NET Entity Data Model.
2) Choose EFDemoDB -> Select the Person table.
3) Right click the canvas -> Add -> Entity.
4) Entity name: Student Base type: Person.
5) Repeat the third step and add entities as follows,
   Entity name: Instructor      Base type: Person
   Entity name: Admin           Base type: Person
   Entity name: BusinessStudent Base type: Student
6) Simply single-click on the property in the Person entity and press ctrl+x.  
   Then single-click on the Scalar Properties section of the appropriate 
   entity and press 'ctrl + v'. 
   The properties to cut and paste are as follows:

   EnrollmentDate –> Student 
   AdminDate      –> Admin 
   HireDate       –> Instructor 
   BusinessCredits–> BusinessStudent

7) Ritht click Student entity -> Table Mapping -> Map the entity to Person 
   table.Do the same to Admin,Instructor and  BusinessStudent.  
8) Right click Person entity -> Table Mapping -> Click Add a Condition -> 
   Choose PersonCategory -> Value = 0.
   Do the same to other entities and set as follows,
   Student         1
   Instrutor       2
   Admin           3
   BusinessStudent 4
   
9) Delete PersonCategory in the Person entity. 
   It must be done after the step7.
10)Build the solution.   


/////////////////////////////////////////////////////////////////////////////
Table Per Type Inheritance
/////////////////////////////////////////////////////////////////////////////

Tables:(main related columns)
[PersonTPT]
 PersonID [PK]
 LastName
 FirstName

[StudentTPT]
 PersonID [PK] [FKtoPersonTPT]
 Degree
 EnrollmentDate

[InstructorTPT]
 PersonID [PK] [FKtoPersonTPT]
 HireDate

[AdminTPT]
 PersonID [PK] [FKtoPersonTPT]
 AdminDate
 
[BusinessStudentTPT]
 PersonID [PK] [FKtoStudentTPT]
 BusinessCredits
 
Create EDM Steps:
1) Add -> New Item -> ADO.NET Entity Data Model
2) Choose EFDemoDB -> Select the five tables
3) Rename the PersonTPT to Person and do the same to other entities.
4) Renmae the Person entity's Entity Set Name as People.
5) Delete all the one to many associations generated.
6) Create inheritance to Person (base type) and Student. Do the same to 
   Instructor and Admin entities.
7) Create inheritance to Student (base type) and BusinessStudent.
8) Keep then PersonID property in Person entity and delete the PersonID 
   propery in all derived entities.
9) Right click the Student entity -> Table Mapping -> Map the PersonID 
   propery to PersonID in the dropdown list.Do the same to Instructor, 
   Admin and BusinessStudent.
10)Build the solution


/////////////////////////////////////////////////////////////////////////////
Table Splitting
/////////////////////////////////////////////////////////////////////////////

Tables:(main related columns)
[Person]
 PersonID [PK]
 LastName
 FirstName
 PersonCategory
 Picture
 
Create EDM Steps:
1) Add -> New Item -> ADO.NET Entity Data Model.
2) Choose EFDemoDB -> Select the Person table.
3) Click the Person table -> Copy the Person entity(Ctrl+ C).
4) Click the canvas -> Paste the Person entity(Ctrl + V) -> 
   Rename it to "PersonDetail".
5) Delete all the properties in Person entity except PersonID, LastName and
   FirstName.
6) Delete LastName and FirstName properties in PersonDetail entity.
7) Add a 1:1 association between "Person" and "PersonDetail" named 
   PersonPersonDetail.
8) Right click the PersonDetail -> Table Mapping -> Map the entity
   to Person table.
9) Right click the TblSplitEntitie.edmx -> Open With -> XML Editor.
10)Add ReferentialConstraint element to PersonPersonDetail in CSDL as follows

	<Association Name="PersonPersonDetail">
	  <End Type="EFTblSplitModel.Person" Role="Person" Multiplicity="1" />
	  <End Type="EFTblSplitModel.PersonDetail" Role="PersonDetail" 
	  Multiplicity="1" />
	  <ReferentialConstraint>
		<Principal Role="Person">
		  <PropertyRef Name="PersonID"/>
		</Principal>
		<Dependent Role="PersonDetail">
		  <PropertyRef Name="PersonID"/>
		</Dependent>
	  </ReferentialConstraint>
	</Association>
	
11)Build solution.


/////////////////////////////////////////////////////////////////////////////
References:

ADO.NET Entity Framework
http://msdn.microsoft.com/en-us/library/bb399572.aspx

Entity Data Model
http://msdn.microsoft.com/en-us/library/bb387122.aspx

Table Merging
http://blogs.msdn.com/simonince/archive/2009/03/23/mapping-two-tables-to-one-entity-in-the-entity-framework.aspx

Table Per Hierarchy Inheritance
http://www.robbagby.com/entity-framework/entity-framework-modeling-table-per-Hierarchy-inheritance/

Table Per Type Inheritance
http://www.robbagby.com/entity-framework/entity-framework-modeling-table-per-type-inheritance/

Table Splitting
http://blogs.msdn.com/adonet/archive/2008/12/05/table-splitting-mapping-multiple-entity-types-to-the-same-table.aspx


/////////////////////////////////////////////////////////////////////////////
