'****************************** Module Header ******************************\
' Module Name:	IndependentAssociationClass.vb
' Project:		VBEFForeignKeyAssociation
' Copyright (c) Microsoft Corporation.
' 
' This file demostrates how to insert new related entities, insert by 
' existing entities and update existing entities with the Independent 
' Association.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Namespace VBEFForeignKeyAssociation.IndependentAssociation
    Friend Class IndependentAssociationClass
        ''' <summary>
        ''' Test method to run the insert and update methods with the 
        ''' Independent Association
        ''' </summary>
        Public Shared Sub Test()
            Console.WriteLine("Inserting new related Course and Department" +
                " entities by Independent Association...")

            ' Insert new related entities by Independent Association.
            InsertNewRelatedEntities()

            ' Query the database.
            Query()

            Console.WriteLine("Inserting a new Course related to an existing" +
                " Department entity by Independent Association...")

            ' Insert a new entity related to an existing entity by
            ' Independent Association.
            InsertByExistingEntities()

            ' Query the database.
            Query()

            Console.WriteLine("Updating an existing Course entity " +
                "(only its regular properties)...")

            Dim course As Course = Nothing
            Using context As New IndependentAssociationEntities()
                ' Retrieve an existing Course entity.
                ' Note: The Single method is new in EF 4.0 
                course = context.Courses.Single(Function(c) c.CourseID = 5002)

                ' Modify the Title property of the Course entity
                course.Title = "Data Structures"

                ' Set the relationship by Independent Association.
                course.Department = context.Departments.Single(
                    Function(d) d.DepartmentID = 5)
            End Using

            ' Update an existing Course entity.
            UpdateExistingEntities(course)

            ' Query the database.
            Query()
        End Sub


        ''' <summary>
        ''' Insert a new Course and its Department entity by Independent 
        ''' Association
        ''' </summary>
        Private Shared Sub InsertNewRelatedEntities()
            Using context As New IndependentAssociationEntities()
                ' Create a new Department entity.
                Dim department As New Department With
                {
                    .DepartmentID = 6,
                    .Name = "Software Engineering",
                    .Budget = 300000,
                    .StartDate = DateTime.Now
                }

                ' Create a new Course entity.
                ' Set the navigation property.
                Dim course As New Course With
                    {
                        .CourseID = 6001,
                        .Title = "Object Oriented",
                        .Credits = 4,
                        .StatusID = True,
                        .Department = department
                    }

                Try
                    ' Note: Only need to add one entity because the 
                    ' relationship and related entity will be added
                    ' automatically.
                    context.AddToCourses(course)
                    context.SaveChanges()

                Catch ex As Exception
                    Console.WriteLine(ex.Message)
                End Try
            End Using
        End Sub


        ''' <summary>
        ''' Insert a new Course and set it belong to an existing Department 
        ''' by Independent Association
        ''' </summary>
        Private Shared Sub InsertByExistingEntities()
            Using context As New IndependentAssociationEntities()
                ' Create a new Course entity.
                ' Set the navigation property to an existing Department 
                ' entity.
                Dim course As New Course With
                    {
                        .CourseID = 6002,
                        .Title = ".NET Framework",
                        .Credits = 4,
                        .StatusID = True,
                        .Department = context.Departments.Single(
                            Function(d) d.DepartmentID = 6)
                    }

                Try
                    ' Note: No need to add the Course entity into the 
                    ' ObjectContext because it is automatically done by
                    ' relating to an existing Department entity.
                    context.SaveChanges()

                Catch ex As Exception
                    Console.WriteLine(ex.Message)
                End Try
            End Using
        End Sub


        ''' <summary>
        ''' Update an existing Course entity (only its regular properties, 
        ''' not for the relationship)
        ''' </summary>
        ''' <param name="updatedCourse">An existing Course entity with
        ''' updated data</param>
        Private Shared Sub UpdateExistingEntities(ByVal updatedCourse As Course)
            Using context As New IndependentAssociationEntities()
                Try
                    ' Attach a new Course entity by the primary key of the
                    ' updated Course entity.
                    context.Courses.Attach(New Course With
                                           {.CourseID = updatedCourse.CourseID})

                    ' Apply the updated regular properties to the attached 
                    ' Course entity.
                    ' Note: the navigation property is not updated here 
                    ' even if it is modified.
                    context.Courses.ApplyCurrentValues(updatedCourse)
                    context.SaveChanges()

                Catch ex As Exception
                    Console.WriteLine(ex.Message)
                End Try
            End Using
        End Sub


        ''' <summary>
        ''' Query the Course entities and the corresponding Department 
        ''' entities
        ''' </summary>
        Private Shared Sub Query()
            Using context As New IndependentAssociationEntities()
                For Each c In context.Courses
                    Console.WriteLine("Course ID:{0}" + vbNewLine +
                                      "Title:{1}" + vbNewLine +
                                      "Department:{2}", c.CourseID, c.Title,
                                      c.Department.Name)
                Next

                Console.WriteLine()
            End Using
        End Sub
    End Class
End Namespace
