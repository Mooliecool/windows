'****************************** Module Header ******************************'
' Module Name:    One2ManyClass.vb
' Project:        VBEFEntityDataModel
' Copyright (c) Microsoft Corporation.
'
' This example illustrates how to dinsert, update and query the two entities 
' with one to many association.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Imports directives"
Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
#End Region


Namespace VBEFEntityDataModel.One2Many
    Friend Class One2ManyClass

        ' Test all the methods in One2ManyClass
        Public Shared Sub One2ManyTest()
            InsertDepartmentWithCourse()

            InsertCourse()

            UpdateCourse()

            UpdateDepartment()

        End Sub

        ' Insert new department with new course
        Public Shared Sub InsertDepartmentWithCourse()

            Using context As New EFO2MEntities()

                Dim department As New Department() With _
                { _
                    .DepartmentID = 4, _
                    .Name = "Software Engineering" _
                }

                Dim course As New Course() With _
                { _
                    .CourseID = 2202, _
                    .Title = "ADO.NET" _
                }

                department.Course.Add(course)

                context.AddToDepartment(department)

                Try

                    Console.WriteLine("Inserting department {0} with course " _
                        + "{1}", department.Name, course.Title)

                    context.SaveChanges()

                    Query()

                Catch ex As Exception
                    Console.WriteLine(ex.Message)
                End Try
            End Using
        End Sub


        ' Insert new course to existing department 
        Public Shared Sub InsertCourse()

            Using context As New EFO2MEntities()
                Dim course As New Course() With _
                { _
                    .CourseID = 2203, _
                    .Title = "Object Oriented Programming" _
                }

                course.Department = (From p In context.Department _
                    Where p.DepartmentID = 7 _
                    Select p).First()

                context.AddToCourse(course)

                Try

                    Console.WriteLine("Inserting course {0} to department " _
                        + "{1}", course.Title, course.Department.Name)

                    context.SaveChanges()

                    Query()

                Catch ex As Exception
                    Console.WriteLine(ex.Message)
                End Try
            End Using
        End Sub


        ' Get all the departments with courses 
        Public Shared Sub Query()

            Using context As New EFO2MEntities()

                Dim query = From p In context.Department.Include("Course") _
                            Select p

                Console.WriteLine("Deparments with their courses:")

                For Each d As Department In query

                    Console.WriteLine("{0} {1}", d.DepartmentID, d.Name)

                    For Each c As Course In d.Course

                        Console.WriteLine("  {0}", c.Title)

                    Next
                Next

                Console.WriteLine()

            End Using
        End Sub


        ' Update one existing course 
        Public Shared Sub UpdateCourse()

            Using context As New EFO2MEntities()

                Dim course As New Course()

                course.CourseID = 2203

                context.AttachTo("Course", course)

                course.Title = "OOP"

                Try

                    Console.WriteLine("Modifying Course 2203's Title to {0}", _
                        course.Title)

                    context.SaveChanges()

                    Query()

                Catch ex As Exception
                    Console.WriteLine(ex.Message)
                End Try
            End Using
        End Sub


        ' Update one existing department 
        Public Shared Sub UpdateDepartment()

            Using context As New EFO2MEntities()

                Dim department As New Department()

                department.DepartmentID = 1

                context.AttachTo("Department", department)

                department.Name = "Computer Engineering"

                department.Course.Add(New Course() With _
                { _
                    .CourseID = 2204, _
                    .Title = "Arithmetic" _
                })

                Try

                    Console.WriteLine("Modifying Department 1's Title to {0}" _
                       + ", and insert a new Course 2204 into the " + _
                       "Department 1", department.Name)

                    context.SaveChanges()

                    Query()

                Catch ex As Exception
                    Console.WriteLine(ex.Message)
                End Try
            End Using
        End Sub

    End Class

End Namespace