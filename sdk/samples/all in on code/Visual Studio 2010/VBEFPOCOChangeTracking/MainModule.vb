'******************************* Module Header *********************************'
' Module Name:  MainForm.vb
' Project:      VBEFPOCOChangeTracking
' Copyright (c) Microsoft Corporation.
'
' The VBEFPOCOChangeTracking example illustrates how to update POCO entity
' properties and relationships with change tracking proxy and without change
' tracking proxy.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*******************************************************************************'
Imports System.Data.Objects

Module MainModule

    Sub Main()
        ' Update POCO entity properties with chang tracking proxy.
        UpdatePropertyWithChangeTrackingProxy()
        Console.WriteLine()

        ' Update POCO entity properties without chang tracking proxy.
        UpdatePropertyWithoutChangeTrackingProxy()
        Console.WriteLine()

        ' Update POCO entity relationship with chang tracking proxy.
        UpdateRelationshipWithChangeTrackingProxy()
        Console.WriteLine()

        ' Update POCO entity relationship without chang tracking proxy.
        UpdateRelationshipWithoutChangeTrackingProxy()
        Console.WriteLine()

        Console.Write("Press [Enter] to exit...")
        Console.Read()
    End Sub



    ''' <summary>
    ''' Demostrate how to update POCO entity properties with change
    ''' tracking proxy.
    ''' </summary>
    Sub UpdatePropertyWithChangeTrackingProxy()
        Console.WriteLine("Update properties with change tracking proxy...")
        Try
            Using context As New POCOChangeTrackingEntities
                Dim de As Department = context.Departments.
                    Single(Function(d) d.Name = "Chinese")

                ' Retrieve the entity state entry.
                Dim entry As ObjectStateEntry = context.ObjectStateManager.
                    GetObjectStateEntry(de)

                ' It should be "Unchanged" now.
                Console.WriteLine("Entity State before modification: {0}",
                                  entry.State)

                de.Name = "New Department Name"

                ' It should be "Modified" now.
                Console.WriteLine("Entity State after modification: {0}",
                                  entry.State)
            End Using
        Catch ex As Exception
            Console.WriteLine(ex.ToString)
        End Try
    End Sub


    ''' <summary>
    ''' Demostrate how to update POCO entity properties without change
    ''' tracking proxy.
    ''' </summary>
    Sub UpdatePropertyWithoutChangeTrackingProxy()
        Console.WriteLine("Update properties without change tracking proxy...")
        Try
            Using context As New POCOChangeTrackingEntities
                ' Turn off proxy creation.
                context.ContextOptions.ProxyCreationEnabled = False

                Dim de As Department = context.Departments.
                    Single(Function(d) d.Name = "Chinese")

                ' Retrieve the entity state entry.
                Dim entry As ObjectStateEntry = context.ObjectStateManager.
                    GetObjectStateEntry(de)

                ' It should be "Unchanged" now.
                Console.WriteLine("Entity State before modification: {0}",
                                  entry.State)

                de.Name = "New Department Name"

                ' It should be "Unchanged" now.
                Console.WriteLine("Entity State after modification: {0}",
                                  entry.State)

                ' Detect the entity properties changes.
                context.DetectChanges()

                ' It should be "Modified" now.
                Console.WriteLine("Entity State after DetectChanges is called: {0}",
                                  entry.State)
            End Using
        Catch ex As Exception
            Console.WriteLine(ex.ToString)
        End Try
    End Sub


    ''' <summary>
    ''' Demostrate how to update POCO entity relationships with change
    ''' tracking proxy.
    ''' </summary>
    Sub UpdateRelationshipWithChangeTrackingProxy()
        Console.WriteLine("Update relationships with change tracking proxy...")
        Try
            Using context As New POCOChangeTrackingEntities
                Dim department As Department = context.Departments.
                    Single(Function(d) d.Name = "Chinese")

                ' Create a proxy object.
                Dim newCourse As Course = context.Courses.CreateObject
                newCourse.CourseID = 1234
                newCourse.Title = "Classical Chinese Literature"
                newCourse.Credits = 4
                newCourse.StatusID = True

                ' Set the relationship.
                department.Courses.Add(newCourse)

                Console.WriteLine("Is the newly created object a proxy? {0}",
                                  IsProxy(newCourse))

                ' Retrieve all the newly added entities' object state entry.
                Dim added = context.ObjectStateManager.GetObjectStateEntries(
                    EntityState.Added)

                ' It should be 1 entity added.
                Console.WriteLine("{0} object(s) added", added.Count())
            End Using
        Catch ex As Exception
            Console.WriteLine(ex.ToString)
        End Try
    End Sub


    ''' <summary>
    ''' Demostrate how to update POCO entity relationships without change
    ''' tracking proxy.
    ''' </summary>
    Sub UpdateRelationshipWithoutChangeTrackingProxy()
        Console.WriteLine("Update relationships without change tracking proxy...")
        Try
            Using context As New POCOChangeTrackingEntities
                ' Turn off proxy creation.
                context.ContextOptions.ProxyCreationEnabled = False

                Dim de As Department = context.Departments.
                    Single(Function(d) d.Name = "Chinese")

                ' Create a regular POCO entity.
                Dim newCourse As New Course With
                    {
                        .CourseID = 1234,
                        .Title = "Classical Chinese Literature",
                        .Credits = 4,
                        .StatusID = True
                    }

                ' Set the relationship.
                de.Courses.Add(newCourse)

                Console.WriteLine("Is the newly created object a proxy? {0}",
                                  IsProxy(newCourse))

                ' Retrieve all the newly added entities' object state entry before 
                ' DetectChanges is called.
                Dim addedBeforeDetectChanges = context.ObjectStateManager.
                    GetObjectStateEntries(EntityState.Added)

                ' It should be no entity added.
                Console.WriteLine("Before DetectChanges is called, {0} object(s) added",
                                  addedBeforeDetectChanges.Count())

                ' Detect the entity relationships modification.
                context.DetectChanges()

                ' Retrieve all the newly added entities' object state entry after 
                ' DetectChanges is called.
                Dim addedAftereDetectChanges = context.ObjectStateManager.
                    GetObjectStateEntries(EntityState.Added)

                ' It should be 1 entity added.
                Console.WriteLine("After DetectChanges is called, {0} object(s) added",
                                  addedAftereDetectChanges.Count())
            End Using
        Catch ex As Exception
            Console.WriteLine(ex.ToString)
        End Try
    End Sub


    ''' <summary>
    ''' Check whether the certain object is a proxy entity object.
    ''' <param name="type">The entity object</param>
    ''' <returns>Return true if the certain object is a proxy entity object,
    ''' otherwise return false</returns>
    ''' </summary>
    Function IsProxy(ByVal type As Object) As Boolean
        ' ObjectContext.GetObjectType returns the entity type of the POCO entity
        ' associated with a proxy object of a specified type.  If it does not 
        ' equal the entity type, it should be a proxy entity object.   
        Return ((Not type Is Nothing) AndAlso
                (Not ObjectContext.GetObjectType(type.GetType) Is type.GetType))
    End Function

End Module
