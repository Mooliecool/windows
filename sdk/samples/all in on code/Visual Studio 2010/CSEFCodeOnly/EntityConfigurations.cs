/****************************** Module Header ******************************\
* Module Name:    EntityConfigurations.cs
* Project:        CSEFCodeOnly
* Copyright (c) Microsoft Corporation.
*
* This code file contains the EntityConfiguration for the POCO entities to 
* create the Entity Data Model metadata.  
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directive
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Data.Objects;
#endregion

namespace CSEFCodeOnly
{
    #region EntityConfiguration for TPT inheritance POCO entities
    public class PersonTPTConfiguration : EntityConfiguration<PersonTPT>
    {
        public PersonTPTConfiguration()
        {
            // Set the entity key
            HasKey(p => p.PersonID);

            // Set the table mapping of the TPT inheritance entities
            // The table name here is "PeopleTPT"
            // Anonymous type properties are mapped to the table column names
            MapHierarchy(
                p => new
                {
                    p.PersonID,
                    Name_FirstName = p.Name.FirstName,
                    Name_LastName = p.Name.LastName,
                    Address_City = p.Address.City,
                    Address_Country = p.Address.Country,
                    Address_Zipcode = p.Address.Zipcode
                }).ToTable("PeopleTPT");
        }
    }

    public class InstructorTPTConfiguration : 
        EntityConfiguration<InstructorTPT>
    {
        public InstructorTPTConfiguration()
        {
            // Set the regular property column mapping
            Property(i => i.HireDate);

            // Set the many-to-many relationship between the Course and 
            // InstructorTPT entities 
            Relationship(i => i.Courses).FromProperty(c => c.Instructors);

            // Set the table mapping of the TPT inheritance entities
            // The table name here is "InstructorsTPT"
            // Anonymous type properties are mapped to the table column names
            MapHierarchy(
                i => new
                {
                    i.PersonID,
                    i.HireDate
                }).ToTable("InstructorsTPT");
        }
    }

    public class StudentTPTConfiguration : EntityConfiguration<StudentTPT>
    {
        public StudentTPTConfiguration()
        {
            // Set the regular property column mappings
            Property(s => s.EnrollmentDate);
            Property(s => s.Degree);
            Property(s => s.Credits);

            // Set the table mapping of the TPT inheritance entities
            // The table name here is "StudentsTPT"
            // Anonymous type properties are mapped to the table column names
            MapHierarchy(
                s => new
                {
                    s.PersonID,
                    s.EnrollmentDate,
                    s.Degree,
                    s.Credits
                }).ToTable("StudentsTPT");

            // Set the one-to-many relationship between the StudentTPT and 
            // CourseStudent entities
            Relationship(s => s.CourseStudents)
                .FromProperty(cs => cs.Student);
        }
    }

    public class AdminTPTConfiguration : EntityConfiguration<AdminTPT>
    {
        public AdminTPTConfiguration()
        {
            // Set the regular property column mapping
            Property(a => a.AdminDate);

            // Set the table mapping of the TPT inheritance entities
            // The table name here is "AdminsTPT"
            // Use the EntityMap.Row and EntityMap.Column to map the 
            // properties to the table columns
            MapHierarchy(
                a => EntityMap.Row(
                    EntityMap.Column(a.PersonID, "PersonID"),
                    EntityMap.Column(a.AdminDate, "AdminDate")
                )).ToTable("AdminsTPT");
        }
    }

    public class BusinessStudentTPTConfiguration : 
        EntityConfiguration<BusinessStudentTPT>
    {
        public BusinessStudentTPTConfiguration()
        {
            // Set the regular property column mapping
            Property(bs => bs.BusinessCredits);

            // Set the table mapping of the TPT inheritance entities
            // The table name here is "BusinessStudentsTPT"
            // Use the EntityMap.Row and EntityMap.Column to map the 
            // properties to the table columns
            MapHierarchy(
                bs => EntityMap.Row(
                    EntityMap.Column(bs.PersonID, "PersonID"),
                    EntityMap.Column(bs.BusinessCredits, "BusinessCredits")
                )).ToTable("BusinessStudentsTPT");
        }
    }
    #endregion

    #region EntityConfiguration for other relational POCO entities
    public class DepartmentConfiguration : EntityConfiguration<Department>
    {
        public DepartmentConfiguration()
        {
            // Set the entity key
            HasKey(d => d.DepartmentID);

            // Set the varchar typed column, max length: 100, non-nullable
            Property(d => d.Name).HasMaxLength(100).IsRequired();

            // Set the decimal typed column, precision: 18, scale: 0,
            // nullable
            Property(d => d.Budget).Precision = 18;
            Property(d => d.Budget).Scale = 0;

            // Set the regular property column mapping
            Property(d => d.StartDate);

            // Set the one-to-many relationship between the Department and 
            // Course entities
            Relationship(d => d.Courses).FromProperty(c => c.Department);
        }
    }

    public class CourseConfiguration : EntityConfiguration<Course>
    {
        public CourseConfiguration()
        {
            // Set the entity key
            HasKey(c => c.CourseID);

            // Set the varchar typed column, max length: 100, non-nullable
            Property(c => c.Title).HasMaxLength(100).IsRequired();

            // Set the regular property column mapping
            Property(c => c.Credits);

            // Set the FK association between the Department and Course 
            // entities, and set the FK association property as
            // Course.DepartmentID
            // IsRequired() indicates it is 1:* relationship instead of
            // 0..1:* relationship
            Relationship(c => c.Department).IsRequired()
                .FromProperty(d => d.Courses).HasConstraint(
                (c, d) => c.DepartmentID == d.DepartmentID);

            // Set the one-to-many relationship between the Course and 
            // CourseStudent entities
            Relationship(c => c.CourseStudents)
                .FromProperty(cs => cs.Course);

            // Set the many-to-many relationship between the Course and 
            // InstructorTPT entities
            Relationship(c => c.Instructors)
                .FromProperty(i => i.Courses);
        }
    }

    public class CourseStudentConfiguration :
        EntityConfiguration<CourseStudent>
    {
        public CourseStudentConfiguration()
        {
            // Set the composite entity key
            HasKey(cs => new { cs.PersonID, cs.CourseID });

            // Set the regular property column mappings
            Property(cs => cs.PersonID);
            Property(cs => cs.CourseID);
            Property(cs => cs.Score);

            // Set the FK association between the Course and CourseStudent 
            // entities, and set the FK association property as
            // CourseStudent.CourseID
            Relationship(cs => cs.Course).IsRequired()
                .FromProperty(c => c.CourseStudents)
                .HasConstraint((cs, c) => cs.CourseID == c.CourseID);

            // Set the FK association between the StudentTPT and 
            // CourseStudent entities, and set the FK association property 
            // as CourseStudent.PersonID
            Relationship(cs => cs.Student).IsRequired()
                .FromProperty(s => s.CourseStudents)
                .HasConstraint((cs, s) => cs.PersonID == s.PersonID);
        }
    }
    #endregion

    #region EntityConfiguration for TPH inheritance POCO entities
    public class PersonTPHConfiguration : EntityConfiguration<PersonTPH>
    {
        public PersonTPHConfiguration()
        {
            // Set the entity key
            HasKey(p => p.PersonID);

            // Set the table mapping of the TPT inheritance entities
            // The table name here is "PeopleTPH"
            // Each type has its own property and column mappings
            // The base and each derived type have the discriminitor column
            // "PersonCategory"
            MapHierarchy()
                .Case<PersonTPH>(p => new
                    {
                        p.PersonID,
                        p.Name.FirstName,
                        p.Name.LastName,
                        p.Address.City,
                        p.Address.Country,
                        p.Address.Zipcode,
                        PersonCategory = 0
                    })
                .Case<InstructorTPH>(i => new
                    {
                        i.PersonID,
                        i.Name.FirstName,
                        i.Name.LastName,
                        i.Address.City,
                        i.Address.Country,
                        i.Address.Zipcode,
                        i.HireDate,
                        PersonCategory = 1
                    })
                .Case<StudentTPH>(s => new
                    {
                        s.PersonID,
                        s.Name.FirstName,
                        s.Name.LastName,
                        s.Address.City,
                        s.Address.Country,
                        s.Address.Zipcode,
                        s.EnrollmentDate,
                        PersonCategory = 2
                    })
                .Case<AdminTPH>(a => new
                    {
                        a.PersonID,
                        a.Name.FirstName,
                        a.Name.LastName,
                        a.Address.City,
                        a.Address.Country,
                        a.Address.Zipcode,
                        a.AdminDate,
                        PersonCategory = 3
                    }).ToTable("PeopleTPH");
        }
    }
    #endregion

    #region ComplexTypeCinfiguration for Complex Type entities
    public class ComplexTypeNameConfiguration : 
        ComplexTypeConfiguration<Name>
    {
        public ComplexTypeNameConfiguration()
        {
            // Set the two varchar typed column the max length and 
            // non-nullable
            Property(n => n.FirstName).IsMax().IsRequired();
            Property(n => n.LastName).IsMax().IsRequired();
        }
    }

    public class ComplexTypeAddressConfiguration : 
        ComplexTypeConfiguration<Address>
    {
        public ComplexTypeAddressConfiguration()
        {
            // Set the three varchar typed column the max length
            Property(a => a.City).IsMax();
            Property(a => a.Country).IsMax();
            Property(a => a.Zipcode).IsMax();
        }
    }
    #endregion
}