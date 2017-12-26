'****************************** Module Header ******************************\
' Module Name:    MainModule.vb
' Project:        VBEFCodeOnly
' Copyright (c) Microsoft Corporation.
'
' The VBEFCodeOnly example illustrates how to use the new feature Code Only
' in the Entity Framework 4.0 to use create the Entity Data Model metadata
' and the corresponding .edmx file with POCO entity classes and ObjectContext
' class at runtime.  It also demostrates some insert and query operations to 
' test the created the Entity Data Model metadata.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

#Region "Imports Directive"
Imports Microsoft.Data.Objects
Imports System.Data.SqlClient
Imports System.Data.Objects
Imports System.Xml
Imports System.Text
#End Region

Module MainModule
    ' Const connection string
    Private Const DB_CONN As String = "Data Source=.\SQLEXPRESS;" &
            "Initial Catalog=CodeOnlyDB;Integrated Security=SSPI;"

    Sub Main()
        ' Create a ContextBuilder to create the Entity Data Model 
        ' metadata.
        Dim builder As New ContextBuilder(Of CodeOnlyContainer)

        ' Create a SqlConnection to use the const connection string to 
        ' connect the SQL Server Express
        Dim conn As New SqlConnection(DB_CONN)

        ' Register the entity configurations to create the Entity Data
        ' Model metadata
        RegisterConfigurations(builder)

        ' Create the .edmx file based on the EDM metadata
        CreateEDMX(builder, conn)
        Console.WriteLine()

        ' Create the database based on EDM metadata, then insert and 
        ' query relational entities
        CreateDatabaseAndInsertQueryEntities(builder, conn)
        Console.WriteLine()

        ' Query the Type-per-Hierarchy inheriance entities
        QueryTPHData(builder, conn)
        Console.WriteLine()
    End Sub


    ''' <summary>
    ''' Create the database based on EDM metadata, then insert and query
    ''' relational entities
    ''' </summary>
    ''' <param name="builder">The certain ContextBuilder to create certain
    ''' ObjectContext</param>
    ''' <param name="conn">The certain SqlConnection to connect the 
    ''' SQL Server Express</param>
    Sub CreateDatabaseAndInsertQueryEntities(
                    ByVal builder As ContextBuilder(Of CodeOnlyContainer),
                    ByVal conn As SqlConnection)
        ' Create the ObjectContext object by the ContextBuilder and 
        ' SqlConnection
        Using context As CodeOnlyContainer = builder.Create(conn)
            ' Check if the database exists
            If context.DatabaseExists() Then
                ' Delete it if it exists
                context.DeleteDatabase()
            End If
            ' Create the database
            context.CreateDatabase()

            ' Create a new Type-per-Table(TPT) inheritance Admin entity
            Dim adim As New AdminTPT() With
                {
                    .PersonID = 1,
                    .Name = New Name() With
                        {
                            .FirstName = "Jialiang",
                            .LastName = "Ge"
                        },
                    .Address = New Address() With
                        {
                            .City = "Shanghai",
                            .Country = "China",
                            .Zipcode = "200030"
                        },
                    .AdminDate = DateTime.Now
                }

            ' Add the newly created Admin entity into the ObjectContext
            context.PeopleTPT.AddObject(adim)

            ' Create a new Deparment entity
            Dim department As New Department() With
                {
                    .DepartmentID = 1,
                    .Name = "Computer Science",
                    .Budget = 400000,
                    .StartDate = DateTime.Now
                }

            ' Create a new Course entity
            Dim course As New Course() With
                {
                    .CourseID = 1001,
                    .Title = ".NET Framework",
                    .Credits = 3,
                    .Department = department,
                    .Instructors = New List(Of InstructorTPT)()
                }

            ' Create a new TPT inheritance Instructor entity
            Dim instructor As New InstructorTPT() With
                {
                    .PersonID = 2,
                    .Name = New Name() With
                        {
                            .FirstName = "Colbert",
                            .LastName = "Zhou"
                        },
                    .Address = New Address() With
                        {
                            .City = "Shanghai",
                            .Country = "China",
                            .Zipcode = "200030"
                        },
                    .HireDate = DateTime.Now
                }

            ' Add the newly created Instructor entity into the 
            ' Course's Instructor collection
            course.Instructors.Add(instructor)

            ' Create a new TPT inheritance Student entity
            Dim student As New StudentTPT() With
                {
                    .PersonID = 3,
                    .Name = New Name() With
                        {
                            .FirstName = "Lingzhi",
                            .LastName = "Sun"
                        },
                    .Address = New Address() With
                        {
                            .City = "Shanghai",
                            .Country = "China",
                            .Zipcode = "200032"
                        },
                    .EnrollmentDate = DateTime.Now
                }

            ' Create a new CourseStudent relationship entity
            Dim courseStudent As New CourseStudent() With
                {
                    .Student = student,
                    .Course = course,
                    .Score = 90
                }

            ' Add the CourseStudent relationship entity into the 
            ' ObjectContext
            context.CourseStudents.AddObject(courseStudent)


            ' Create a new Type-per-Hierarchy(TPH) inheritance Admin 
            ' entity
            Dim adminTPH As New AdminTPH() With
                {
                    .PersonID = 1,
                    .Name = New Name() With
                        {
                            .FirstName = "Jialiang",
                            .LastName = "Sun"
                        },
                    .Address = New Address() With
                        {
                            .City = "Shanghai",
                            .Country = "China",
                            .Zipcode = "200032"
                        },
                    .AdminDate = DateTime.Now
                }

            ' Create a new TPH inheritance Instructor entity
            Dim instructorTPH As New InstructorTPH() With
                {
                    .PersonID = 2,
                    .Name = New Name() With
                        {
                            .FirstName = "Colbert",
                            .LastName = "Zhou"
                        },
                    .Address = New Address() With
                        {
                            .City = "Shanghai",
                            .Country = "China",
                            .Zipcode = "200030"
                        },
                    .HireDate = DateTime.Now
                }

            ' Create a new TPH inheritance Student entity
            Dim studentTPH As New StudentTPH() With
                {
                    .PersonID = 3,
                    .Name = New Name() With
                        {
                            .FirstName = "Lingzhi",
                            .LastName = "Sun"
                        },
                    .Address = New Address() With
                        {
                            .City = "Shanghai",
                            .Country = "China",
                            .Zipcode = "200032"
                        },
                    .EnrollmentDate = DateTime.Now
                }

            ' Add the TPH inheritance entities into the ObjectContext
            context.PeopleTPH.AddObject(adminTPH)
            context.PeopleTPH.AddObject(instructorTPH)
            context.PeopleTPH.AddObject(studentTPH)

            Try
                ' Saving the relational entities
                Console.Write("Saving the relational entities...")
                context.SaveChanges()
                Console.WriteLine("Successfully!")
            Catch ex As Exception
                Console.WriteLine("Failed!")
                Console.WriteLine(ex.ToString())
            End Try


            ' Query the single Course entitiy from the ObjectContext
            Dim queryCourse = context.Courses.Single()

            ' Display the Course informaion
            Console.WriteLine("Course: {0} under Department: {1}",
                    queryCourse.Title, queryCourse.Department.Name)
            Console.WriteLine()

            ' Query and display the Course's Instructors information
            Console.WriteLine("Course Instructors:")
            For Each i In queryCourse.Instructors
                Console.WriteLine(i.Name)
            Next
            Console.WriteLine()

            ' Query and display the Course's Students information
            Console.WriteLine("Course Students:")
            For Each cs In queryCourse.CourseStudents
                Console.WriteLine(cs.Student.Name)
            Next
        End Using
    End Sub


    ''' <summary>
    ''' Create the .edmx file based on the EDM metadata
    ''' </summary>
    ''' <param name="builder">The certain ContextBuilder to create the 
    ''' .edmx file</param>
    ''' <param name="conn">The certain SqlConnection to connect the 
    ''' SQL Server Express</param>
    Sub CreateEDMX(ByVal builder As ContextBuilder(Of CodeOnlyContainer),
                   ByVal conn As SqlConnection)
        ' Create a XmlWriter object
        Dim writer As New XmlTextWriter("CodeOnly.edmx", Encoding.UTF8)
        Try
            ' Write the .edmx file by the ContextBuilder
            Console.Write("Creating the .edmx file...")
            builder.WriteEdmx(conn, writer)
            Console.WriteLine("Successfully!")
        Catch ex As Exception
            Console.WriteLine("Failed!")
            Console.WriteLine(ex.ToString())
        End Try
    End Sub


    ''' <summary>
    ''' Query the TPH inheritance entities
    ''' </summary>
    ''' <param name="builder">The certain ContextBuilder to create the 
    ''' ObjectContext</param>
    ''' <param name="conn">The certain SqlConnection to connect the 
    ''' SQL Server Express</param>
    Sub QueryTPHData(ByVal builder As ContextBuilder(Of CodeOnlyContainer),
                ByVal conn As SqlConnection)
        ' Create the ObjectContext object by the ContextBuilder and 
        ' SqlConnection
        Using context As CodeOnlyContainer = builder.Create(conn)
            ' Query and display the TPH inheritance entities
            Console.WriteLine("Display People (TPH):")
            For Each peopleTPH In context.PeopleTPH
                ' Check if it is the Admin type
                If TypeOf peopleTPH Is AdminTPH Then
                    Dim admin = DirectCast(peopleTPH, AdminTPH)
                    Console.WriteLine("Admin:")
                    Console.WriteLine(
                        "ID: {0}, Name: {1}, AdminDate: {2}",
                        admin.PersonID, admin.Name, admin.AdminDate)
                    ' Check if it is the Student type
                ElseIf TypeOf peopleTPH Is StudentTPH Then
                    Dim student = DirectCast(peopleTPH, StudentTPH)
                    Console.WriteLine("Student:")
                    Console.WriteLine(
                        "ID: {0}, Name: {1}, EnrollmentDate: {2}",
                        student.PersonID, student.Name,
                        student.EnrollmentDate)
                    ' Check if it is the Instructor type
                ElseIf TypeOf peopleTPH Is InstructorTPH Then
                    Dim instructor = DirectCast(peopleTPH, InstructorTPH)
                    Console.WriteLine("Instructor:")
                    Console.WriteLine(
                        "ID: {0}, Name: {1}, HireDate: {2}",
                        instructor.PersonID, instructor.Name,
                        instructor.HireDate)
                End If
            Next
        End Using
    End Sub


    ''' <summary>
    ''' Use the ContextBuilder to create the EDM metadata
    ''' </summary>
    ''' <param name="builder">The certain ContextBuilder to create the 
    ''' EDM metadata</param>
    Sub RegisterConfigurations(
            ByVal builder As ContextBuilder(Of CodeOnlyContainer))
        ' Config the Complex Types, Name and Address
        builder.Configurations.Add(New ComplexTypeNameConfiguration())
        builder.Configurations.Add(New ComplexTypeAddressConfiguration())

        ' Config the TPT inheritance entities
        builder.Configurations.Add(New PersonTPTConfiguration())
        builder.Configurations.Add(New StudentTPTConfiguration())
        builder.Configurations.Add(New InstructorTPTConfiguration())
        builder.Configurations.Add(New AdminTPTConfiguration())
        builder.Configurations.Add(New BusinessStudentTPTConfiguration())

        ' Config the TPH inheritance entities
        builder.Configurations.Add(New PersonTPHConfiguration())

        ' Config other relational entities
        builder.Configurations.Add(New CourseStudentConfiguration())
        builder.Configurations.Add(New CourseConfiguration())
        builder.Configurations.Add(New DepartmentConfiguration())
    End Sub

End Module
