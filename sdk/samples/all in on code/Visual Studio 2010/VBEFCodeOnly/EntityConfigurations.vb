'****************************** Module Header ******************************\
' Module Name:    EntityConfigurations.vb
' Project:        VBEFCodeOnly
' Copyright (c) Microsoft Corporation.
'
' This code file contains the EntityConfiguration for the POCO entities to 
' create the Entity Data Model metadata. 
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/
Imports Microsoft.Data.Objects

#Region "EntityConfiguration for TPT inheritance POCO entities"
Public Class PersonTPTConfiguration
    Inherits EntityConfiguration(Of PersonTPT)

    Sub New()
        ' Set the entity key
        HasKey(Function(p) p.PersonID)

        ' Set the table mapping of the TPT inheritance entities
        ' The table name here is "PeopleTPT"
        ' Anonymous type properties are mapped to the table column names
        MapHierarchy(Function(p) New With
                            {
                                p.PersonID,
                                .Name_FirstName = p.Name.FirstName,
                                .Name_LastName = p.Name.LastName,
                                .Address_City = p.Address.City,
                                .Address_Country = p.Address.Country,
                                .Address_Zipcode = p.Address.Zipcode
                             }).ToTable("PeopleTPT")
    End Sub
End Class

Public Class InstructorTPTConfiguration
    Inherits EntityConfiguration(Of InstructorTPT)

    Sub New()
        ' Set the regular property column mapping
        [Property](Function(i) i.HireDate)

        ' Set the many-to-many relationship between the Course and 
        ' InstructorTPT entities 
        Relationship(Of Course)(Function(i) i.Courses).FromProperty(Function(c) c.Instructors)

        ' Set the table mapping of the TPT inheritance entities
        ' The table name here is "InstructorsTPT"
        ' Anonymous type properties are mapped to the table column names
        MapHierarchy(Function(i) New With
                                {
                                    i.PersonID,
                                    i.HireDate
                                }).ToTable("InstructorTPT")
    End Sub
End Class

Public Class StudentTPTConfiguration
    Inherits EntityConfiguration(Of StudentTPT)

    Sub New()
        ' Set the regular property column mappings
        [Property](Function(s) s.EnrollmentDate)
        [Property](Function(s) s.Degree)
        [Property](Function(s) s.Credits)

        ' Set the table mapping of the TPT inheritance entities
        ' The table name here is "StudentsTPT"
        ' Anonymous type properties are mapped to the table column names
        MapHierarchy(Function(s) New With
                                {
                                    s.PersonID,
                                    s.EnrollmentDate,
                                    s.Degree,
                                    s.Credits
                                }).ToTable("StudentsTPT")

        ' Set the one-to-many relationship between the StudentTPT and 
        ' CourseStudent entities
        Relationship(Of CourseStudent)(Function(s) s.CourseStudents).
            FromProperty(Function(cs) cs.Student)
    End Sub
End Class

Public Class AdminTPTConfiguration
    Inherits EntityConfiguration(Of AdminTPT)

    Sub New()
        ' Set the regular property column mapping
        [Property](Function(a) a.AdminDate)

        ' Set the table mapping of the TPT inheritance entities
        ' The table name here is "AdminsTPT"
        ' Use the EntityMap.Row and EntityMap.Column to map the 
        ' properties to the table columns
        MapHierarchy(Function(a) EntityMap.Row(
                         EntityMap.Column(a.PersonID, "PersonID"),
                         EntityMap.Column(a.AdminDate, "AdminDate"))).
                 ToTable("AdminsTPT")
    End Sub
End Class

Public Class BusinessStudentTPTConfiguration
    Inherits EntityConfiguration(Of BusinessStudentTPT)

    Sub New()
        ' Set the regular property column mapping
        [Property](Function(bs) bs.BusinessCredits)

        ' Set the table mapping of the TPT inheritance entities
        ' The table name here is "BusinessStudentsTPT"
        ' Use the EntityMap.Row and EntityMap.Column to map the 
        ' properties to the table columns
        MapHierarchy(Function(bs) EntityMap.Row(
                            EntityMap.Column(bs.PersonID, "PersonID"),
                            EntityMap.Column(bs.BusinessCredits,
                                             "BusinessCredits")
                            )).ToTable("BusinessStudentsTPT")
    End Sub
End Class
#End Region

#Region "EntityConfiguration for other relational POCO entities"
Public Class DepartmentConfiguration
    Inherits EntityConfiguration(Of Department)

    Sub New()
        ' Set the entity key
        HasKey(Function(d) d.DepartmentID)

        ' Set the varchar typed column, max length: 100, non-nullable
        [Property](Function(d) d.Name).HasMaxLength(100).IsRequired()

        ' Set the decimal typed column, precision: 18, scale: 0,
        ' nullable
        [Property](Function(d) d.Budget).Precision = 18
        [Property](Function(d) d.Budget).Scale = 0

        ' Set the regular property column mapping
        [Property](Function(d) d.StartDate)

        ' Set the one-to-many relationship between the Department and 
        ' Course entities
        Relationship(Of Course)(Function(d) d.Courses).FromProperty(Function(c) c.Department)
    End Sub
End Class

Public Class CourseConfiguration
    Inherits EntityConfiguration(Of Course)
    Sub New()
        ' Set the entity key
        HasKey(Function(c) c.CourseID)

        ' Set the varchar typed column, max length: 100, non-nullable
        [Property](Function(c) c.Title).HasMaxLength(100).IsRequired()

        ' Set the regular property column mapping
        [Property](Function(c) c.Credits)

        ' Set the FK association between the Department and Course 
        ' entities, and set the FK association property as
        ' Course.DepartmentID
        ' IsRequired() indicates it is 1:* relationship instead of
        ' 0..1:* relationship
        Relationship(Of Department)(Function(c) c.Department).IsRequired().
            FromProperty(Function(d) d.Courses).
            HasConstraint(Function(c, d) c.DepartmentID = d.DepartmentID)

        '  Set the one-to-many relationship between the Course and 
        ' CourseStudent entities
        Relationship(Of CourseStudent)(Function(c) c.CourseStudents).
            FromProperty(Function(cs) cs.Course)

        ' Set the many-to-many relationship between the Course and 
        ' InstructorTPT entities
        Relationship(Of InstructorTPT)(Function(c) c.Instructors).
            FromProperty(Function(i) i.Courses)
    End Sub
End Class

Public Class CourseStudentConfiguration
    Inherits EntityConfiguration(Of CourseStudent)
    Sub New()
        ' Set the composite entity key
        HasKey(Function(cs) New With {cs.PersonID, cs.CourseID})

        ' Set the regular property column mappings
        [Property](Function(cs) cs.PersonID)
        [Property](Function(cs) cs.CourseID)
        [Property](Function(cs) cs.Score)

        ' Set the FK association between the Course and CourseStudent 
        ' entities, and set the FK association property as
        ' CourseStudent.CourseID
        Relationship(Of Course)(Function(cs) cs.Course).IsRequired().
            FromProperty(Function(c) c.CourseStudents).
            HasConstraint(Function(cs, c) cs.CourseID = c.CourseID)

        ' Set the FK association between the StudentTPT and 
        ' CourseStudent entities, and set the FK association property 
        ' as CourseStudent.PersonID
        Relationship(Of StudentTPT)(Function(cs) cs.Student).IsRequired().
            FromProperty(Function(s) s.CourseStudents).
            HasConstraint(Function(cs, s) cs.PersonID = s.PersonID)
    End Sub
End Class
#End Region

#Region "EntityConfiguration for TPH inheritance POCO entities"
Public Class PersonTPHConfiguration
    Inherits EntityConfiguration(Of PersonTPH)

    Sub New()
        ' Set the entity key
        HasKey(Function(p) p.PersonID)

        ' Set the table mapping of the TPT inheritance entities
        ' The table name here is "PeopleTPH"
        ' Each type has its own property and column mappings
        ' The base and each derived type have the discriminitor column
        ' "PersonCategory"
        MapHierarchy().Case(Of PersonTPH)(Function(p) New With
                                                    {
                                                        p.PersonID,
                                                        p.Name.FirstName,
                                                        p.Name.LastName,
                                                        p.Address.City,
                                                        p.Address.Country,
                                                        p.Address.Zipcode,
                                                        .PersonCategory = 0
                                                    }) _
                      .Case(Of InstructorTPH)(Function(i) New With
                                                    {
                                                        i.PersonID,
                                                        i.Name.FirstName,
                                                        i.Name.LastName,
                                                        i.Address.City,
                                                        i.Address.Country,
                                                        i.Address.Zipcode,
                                                        i.HireDate,
                                                        .PersonCategory = 1
                                                    }) _
                       .Case(Of StudentTPH)(Function(s) New With
                                                    {
                                                        s.PersonID,
                                                        s.Name.FirstName,
                                                        s.Name.LastName,
                                                        s.Address.City,
                                                        s.Address.Country,
                                                        s.Address.Zipcode,
                                                        s.EnrollmentDate,
                                                        .PersonCategory = 2
                                                    }) _
                        .Case(Of AdminTPH)(Function(a) New With
                                                    {
                                                        a.PersonID,
                                                        a.Name.FirstName,
                                                        a.Name.LastName,
                                                        a.Address.City,
                                                        a.Address.Country,
                                                        a.Address.Zipcode,
                                                        a.AdminDate,
                                                        .PersonCategory = 3
                                                    }).ToTable("PeopleTPH")
    End Sub
End Class
#End Region

#Region "ComplexTypeCinfiguration for Complex Type entities"
Public Class ComplexTypeNameConfiguration
    Inherits ComplexTypeConfiguration(Of Name)
    Sub New()
        ' Set the two varchar typed column the max length and 
        ' non-nullable
        [Property](Function(n) n.FirstName).IsMax()
        [Property](Function(n) n.LastName).IsMax()
    End Sub
End Class

Public Class ComplexTypeAddressConfiguration
    Inherits ComplexTypeConfiguration(Of Address)
    Sub New()
        ' Set the three varchar typed column the max length
        [Property](Function(a) a.City).IsMax()
        [Property](Function(a) a.Country).IsMax()
        [Property](Function(a) a.Zipcode).IsMax()
    End Sub
End Class
#End Region

