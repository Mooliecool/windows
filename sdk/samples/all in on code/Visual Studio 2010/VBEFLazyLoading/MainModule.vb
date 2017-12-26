'****************************** Module Header ******************************\
' Module Name:    MainModule.vb
' Project:        VBEFLazyLoading
' Copyright (c) Microsoft Corporation.
'
' The VBEFLazyLoading example illustrates how to work with the Lazy Loading
' which is new in Entity Framework 4.0.  It also shows how to use the eager
' loading and explicit loading which is already implemented in the first
' version of Entity Framework.   
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
        LazyLoadingTest()
        EagerLoadingTest()
        ExplicitLoadingTest()

        Console.WriteLine("Press [Enter] to exit...")
        Console.Read()
    End Sub


    ''' <summary>
    ''' Demostrates how to use lazy loading for related entities 
    ''' </summary>
    Sub LazyLoadingTest()
        Console.WriteLine("---Lazy Loading---")

        Using context As New LazyLoadingEntities
            ' Query the department entities which Budget is not NULL
            Dim departments = From d In context.Departments
                              Where d.Budget IsNot Nothing
                              Select d

            For Each department In departments
                Console.WriteLine("Department: {0}", department.Name)

                Console.WriteLine("Courses")

                ' With Lazy Loading enabled, directly access the 
                ' Courses property of the Department will load the
                ' related course entities automatically
                ' Note: here for each department, there will be a 
                ' seperate database call to load the course entities
                For Each course In department.Courses
                    Console.WriteLine(course.Title)
                Next

                Console.WriteLine()
            Next
        End Using
    End Sub


    ''' <summary>
    ''' Demostrates how to use eager loading for related entities 
    ''' </summary>
    Sub EagerLoadingTest()
        Console.WriteLine("---Eager Loading---")

        Using context As New LazyLoadingEntities
            ' Lazy loading is enabled dy default in EF4, so turn it off 
            ' to use eager loading later
            context.ContextOptions.LazyLoadingEnabled = False

            ' Query the department entities which Budget is not NULL
            ' Here we use the .Include() method to eager load the related
            ' course entities
            Dim departments = From d In context.Departments.Include("Courses")
                              Where d.Budget IsNot Nothing
                              Select d

            For Each department In departments
                Console.WriteLine("Department: {0}", department.Name)

                Console.WriteLine("Courses")

                ' The related course entities has been already loaded
                ' Note: for eager loading, there will be only one
                ' database call to load the department and corresponding
                ' course entities
                For Each course In department.Courses
                    Console.WriteLine(course.Title)
                Next

                Console.WriteLine()
            Next
        End Using
    End Sub


    ''' <summary>
    ''' Demostrates how to use explicit loading for related entities 
    ''' </summary>
    Sub ExplicitLoadingTest()
        Console.WriteLine("---Explicit Loading---")

        Using context As New LazyLoadingEntities
            ' Lazy loading is enabled dy default in EF4, so turn it off 
            ' to use eager loading later
            context.ContextOptions.LazyLoadingEnabled = False

            ' Query the department entities which Budget is not NULL
            Dim departments = From d In context.Departments.Include("Courses")
                              Where d.Budget IsNot Nothing
                              Select d

            For Each department In departments
                Console.WriteLine("Department: {0}", department.Name)

                Console.WriteLine("Courses")

                ' Explicit load the related courses entities if they
                ' are not loaded yet
                ' Note: here for each department, there will be a 
                ' seperate database call to load the course entities
                If Not department.Courses.IsLoaded Then
                    department.Courses.Load()
                End If

                For Each course In department.Courses
                    Console.WriteLine(course.Title)
                Next

                Console.WriteLine()
            Next
        End Using
    End Sub

End Module
