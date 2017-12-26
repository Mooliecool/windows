'****************************** Module Header ******************************\
' Module Name:	FKAssociationClass.vb
' Project:		VBEFForeignKeyAssociation
' Copyright (c) Microsoft Corporation.
' 
' This file demostrates how to insert new related entities, insert by 
' existing entities and update existing entities with the Foreign Key
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

Namespace VBEFForeignKeyAssociation.FKAssociation
    Friend Class FKAssociationClass
        ''' <summary>
        ''' Test method to run the insert and update methods with the 
        ''' Foreign Key Association
        ''' </summary>
        Public Shared Sub Test()
            Console.WriteLine("Inserting new related Course and Department" +
                    " entities by Foreign Key Association...")

            ' Insert new related entities by Foreign Key Association.
            InsertNewRelatedEntities()

            ' Query the database.
            Query()

            Console.WriteLine("Inserting a new Course related to an existing" +
                    " Department entity by Foreign Key Association...")

            ' Insert a new entity related to an existing entity by
            ' Foreign Key Association.
            InsertByExistingEntities()

            ' Query the database.
            Query()

            Console.WriteLine("Updating an existing Course entity along with" +
                    " its related Department entity...")

            ' Insert a new entity related to an existing entity by
            ' Foreign Key Association.
            UpdateExistingEntities(New Course With
                                   {
                                       .CourseID = 5002,
                                       .Title = "Basic Data Structure",
                                       .Credits = 4,
                                       .StatusID = True,
                                       .DepartmentID = 7
                                    })

            ' Query the database.
            Query()

        End Sub


        ''' <summary>
        ''' Insert a new Course and its Department entity by Foreign Key 
        ''' Association
        ''' </summary>
        Private Shared Sub InsertNewRelatedEntities()
            Using context As New FKAssociationEntities()
                ' Create a new Department entity.
                Dim department As New Department With
                    {
                        .DepartmentID = 5,
                        .Name = "Computer Science",
                        .Budget = 400000,
                        .StartDate = DateTime.Now
                    }

                ' Create a new Course entity
                Dim course As New Course With
                    {
                        .CourseID = 5001,
                        .Title = "Operation System",
                        .Credits = 4,
                        .StatusID = True,
                        .DepartmentID = department.DepartmentID
                    }

                Try
                    ' Add the Department and Course entities into the 
                    ' ObjectContext.
                    context.AddToDepartments(department)
                    context.AddToCourses(course)

                    ' Note: The navigation properties between the 
                    ' Department and Course entities won't map to each 
                    ' other until after SaveChanges is called.
                    context.SaveChanges()

                Catch ex As Exception
                    Console.WriteLine(ex.Message)
                End Try
            End Using
        End Sub


        ''' <summary>
        ''' Insert a new Course and set it belong to an existing Department 
        ''' by Foreign Key Association
        ''' </summary>
        Private Shared Sub InsertByExistingEntities()
            Using context As New FKAssociationEntities()
                ' Create a new Course entity.
                ' Set the foreign key property to an existing 
                ' Department's DepartmentID.
                Dim course As New Course With
                    {
                        .CourseID = 5002,
                        .Title = "Data Structure",
                        .Credits = 3,
                        .StatusID = True,
                        .DepartmentID = 5
                    }

                Try
                    ' Add the Course entity into the ObjectContext.
                    context.AddToCourses(course)

                    ' Note: The navigation property of the Course
                    ' won't map to the Department entity until 
                    ' after SaveChanges is called.
                    context.SaveChanges()

                Catch ex As Exception
                    Console.WriteLine(ex.Message)
                End Try
            End Using
        End Sub


        ''' <summary>
        ''' Update an existing Course entity as well as its relationship
        ''' </summary>
        ''' <param name="updatedCourse">An existing Course entity with
        ''' updated data</param>
        Private Shared Sub UpdateExistingEntities(ByVal updatedCourse As Course)
            Using context As New FKAssociationEntities()
                Try
                    ' Attach a new Course entity by the primary key of the
                    ' updated Course entity.
                    context.Courses.Attach(New Course With
                                           {.CourseID = updatedCourse.CourseID})

                    ' Apply the updated values to the attached Course entity
                    ' including the foreign key property.
                    ' We don't need to provide the existing Department entity
                    ' here.
                    context.Courses.ApplyCurrentValues(updatedCourse)

                    ' Save the modifications.
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
            Using context As New FKAssociationEntities()
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