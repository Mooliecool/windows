=============================================================================
     CONSOLE APPLICATION : VBEFCodeOnly Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The VBEFCodeOnly example illustrates how to use the new feature Code Only
in the Entity Framework 4.0 to use create the Entity Data Model metadata
and the corresponding .edmx file with POCO entity classes and ObjectContext
class at runtime.  It also demostrates some insert and query operations to 
test the created the Entity Data Model metadata.


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

1. Please install ADO.NET Entity Framework 4.0 CTP3. 
   (Please see the reference section)


/////////////////////////////////////////////////////////////////////////////
Demo:

Please directly run the executable file built from this project.  It will 
create the Entity Data Model metadata during the runtime, create the 
corresponding .edmx file, generate the database "CodeOnlyDB" on your 
SQL Server Express instance, and then insert and query some relational data.


/////////////////////////////////////////////////////////////////////////////
Creation:

1. Add Reference "Microsoft.Data.Entity.CTP" which is installed by ADO.NET
   Entity Framework 4.0 CTP2.
2. Create POCO entities and the corresponding custom ObjectContext class.
   1) Create a file named "CodeOnlyContainer.vb" to hold the entity classes.
   2) Add POCO entity classes: 
      Type-per-Table series: PersonTPT, AdminTPT, InstructorTPT, StudentTPT 
      and BusinessStudentTPT. 
	  Type-per-Hierarchy series: PersonTPH, AdminTPH, InstructorTPH and
	  StudentTPH.
	  Other relational entities: Department, Course, CourseStudent.
	  Complex Type classes: Name and Address.
   3) Create a class named CodeOnlyContainer which inherits ObjectContext 
      and add some IbjectSet<> collection according to the POCO entities.
	  (ICollection<>, IObjectSet<>)

3. Create EntityConfiguration and ComplexTypeConfiguration classes to map the
   POCO entities to the database tables. 
   1) Create a file named "EntityConfiguration.vb" to hold the 
      EntityConfiguration and ComplexTypeConfiguration classes. 
   2) Add EntityConfiguration and ComplexTypeConfiguration classes to map
      the POCO entities to the database tables.
	  (EntityConfiguration.HasKey, EntityConfiguration.Property,
       EntityConfiguration.MapHierarchy, EntityConfiguration.Relationship,
	   EntityConfiguration.MapHierarchy.Case, StringPropertyConfiguration.IsMax,
       StringPropertyConfiguration.IsRequired, DecimalConfiguration.Precision,
       StringPropertyConfiguration.HasMaxLength, DecimalConfiguration.Scale,
       NavigationPropertyConfiguration.FromProperty, EntityMap.ToTable, 
       NavigationPropertyConfiguration.HasConstraint, 
       NavigationProeprtyConfiguration.IsRequired)
   3) Create testing methods to create the Entity Data Model metadata during
      runtime, generate .edmx file, create the database and inser/query some
	  relational data.


/////////////////////////////////////////////////////////////////////////////
References:

Code Only (Entity Framework Design blog)
http://blogs.msdn.com/efdesign/archive/2009/06/10/code-only.aspx

Code Only Enhancements (Entity Framework Design blog)
http://blogs.msdn.com/efdesign/archive/2009/08/03/code-only-enhancements.aspx

Code Only – Further Enhancements (Entity Framework Design blog)
http://blogs.msdn.com/efdesign/archive/2009/10/12/code-only-further-enhancements.aspx

Feature CTP Walkthrough: Code Only for the Entity Framework (Updated)
http://blogs.msdn.com/adonet/pages/feature-ctp-walkthrough-code-only-for-the-entity-framework.aspx

ADO.NET Entity Framework 4.0 CTP3
http://www.microsoft.com/downloads/details.aspx?FamilyID=af18e652-9ea7-478b-8b41-8424b94e3f58&displayLang=en


/////////////////////////////////////////////////////////////////////////////
