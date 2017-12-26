'****************************** Module Header ******************************\
' Module Name:    MainModule.vb
' Project:        VBEFModelFirst
' Copyright (c) Microsoft Corporation.
'
' The VBEFModelFirst example illustrates how to use the new feature Model
' First in the Entity Framework 4.0 to generate DDL that will create a 
' database according to the Entity Data Model.  It also demostrates some 
' insert and query operations to test the generated database. 
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Module MainModule

    Sub Main()
        InsertEntities()
        QueryEntities()
    End Sub


    ''' <summary>
    ''' Insert new related entities into the database that is created by
    ''' the DDL
    ''' </summary>
    Private Sub InsertEntities()
        Using context As New ModelFirstContainer
            ' Create a new Department entity.
            Dim department As New Department With
                {
                    .DepartmentID = 1,
                    .Name = "Computer Science",
                    .Budget = 400000,
                    .StartDate = DateTime.Now
                }

            ' Create a new Course entity.
            ' Note: Here we only use the entity reference or entity set
            ' to set the relationship.  In EF 4.0, we can also use the 
            ' Foreign Key properties. For detail, please see examples 
            ' CSEFForeignKeyAssociation.
            Dim course1 As New Course With
                {
                    .CourseID = 1001,
                    .Title = "Operation System",
                    .Credits = 4,
                    .Department = department
                }

            ' Create another new Course entity.
            Dim course2 As New Course With
                {
                    .CourseID = 1002,
                    .Title = ".NET Framework",
                    .Credits = 3,
                    .Department = department
                }

            ' Create a new Instructor.
            Dim instructor1 As New Instructor With
                {
                    .PersonID = 1,
                    .Name = New Name With
                        {
                            .FirstName = "Jialiang",
                            .LastName = "Ge"
                        },
                    .Address = New Address With
                        {
                            .Country = "China",
                            .City = "Shanghai",
                            .Zipcode = "200000"
                        },
                    .HireDate = DateTime.Now
                }

            ' Add the relationship.
            instructor1.Courses.Add(course1)

            ' Create another new Instructor.
            Dim instructor2 As New Instructor With
                {
                    .PersonID = 2,
                    .Name = New Name With
                        {
                            .FirstName = "Colbert",
                            .LastName = "Zhou"
                        },
                    .Address = New Address With
                        {
                            .Country = "China",
                            .City = "Shanghai",
                            .Zipcode = "200000"
                        },
                    .HireDate = DateTime.Now
                }

            ' Add the relationship.
            instructor2.Courses.Add(course2)

            ' Create a new Student entity.
            Dim student As New Student With
                {
                    .PersonID = 3,
                    .Name = New Name With
                        {
                            .FirstName = "Lingzhi",
                            .LastName = "Sun"
                        },
                    .Address = New Address With
                        {
                            .Country = "China",
                            .City = "Shanghai",
                            .Zipcode = "200000"
                        },
                    .EnrollmentDate = DateTime.Now,
                    .Degreee = 1
                }

            ' Add the relationships.
            student.CourseStudents.Add(New CourseStudent With
                                       {
                                           .Course = course1,
                                           .Score = 90
                                    })

            student.CourseStudents.Add(New CourseStudent With
                                       {
                                           .Course = course2,
                                           .Score = 95
                                    })

            Try
                ' Note: We can only add the Department entity because
                ' all the other related entities will be added as well.
                context.AddToDepartments(department)
                context.SaveChanges()
            Catch ex As Exception
                Console.WriteLine(ex.Message)
            End Try
        End Using
    End Sub


    ''' <summary>
    ''' Query the related entities from the database that is created by
    ''' the DDL
    ''' </summary>
    Private Sub QueryEntities()
        Using context As New ModelFirstContainer
            ' Display all the Instructor and Student typed People 
            ' entities.
            For Each p In context.People
                ' Check if it is an Instructor entity.
                If TypeOf p Is Instructor Then
                    ' Display the Instructor Name and Course information.
                    ' Note: The ToStrin() methods of the Complex Types
                    ' Name and Address have been overriden. 
                    Console.WriteLine("Instructor {0}'s courses:", p.Name)
                    For Each c In DirectCast(p, Instructor).Courses
                        Console.WriteLine(c.Title)
                    Next
                End If

                ' Check if it is a Student entity.
                If TypeOf p Is Student Then
                    ' Display the Student Name and Course information.
                    Console.WriteLine("Student {0}'s courses and scores:",
                            p.Name)
                    For Each cs In DirectCast(p, Student).CourseStudents
                        Console.WriteLine("{0} (Score:{1})",
                                cs.Course.Title, cs.Score)
                    Next
                End If

                ' Display each Person's Address information.
                Console.WriteLine("Address:{0}", p.Address)
                Console.WriteLine()
            Next
        End Using
    End Sub
End Module


' Complex Type Name partial class. 
Partial Public Class Name
    ' Override the ToString() method.
    Public Overrides Function ToString() As String
        Return Me.FirstName + " " + Me.LastName
    End Function
End Class


' Complex Type Address partial class. 
Partial Public Class Address
    ' Override the ToString() method.
    Public Overrides Function ToString() As String
        Return String.Format("{0}, {1} (Zipcode:{2})", Me.City,
                Me.Country, Me.Zipcode)
    End Function
End Class
