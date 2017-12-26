'****************************** Module Header ******************************'
' Module Name:  MainModule.vb
' Project:      VBADONETDataServiceClient
' Copyright (c) Microsoft Corporation.
' 
' VBADONETDataServiceClient example demonstrates an ADO.NET Data Service
' client application. It calls certain ADO.NET Data Services for ADO.NET
' Entity Data Model, Linq To SQL Data Classes, and non-relational in-memory
' data, and it demonstrates these ways (LINQ, ADO.NET query options, custom
' service operations) to update and query the data source. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Imports directive and XML namespace"
Imports System.Data.Services.Client
Imports <xmlns="http://schemas.microsoft.com/ado/2007/08/dataservices/metadata">
#End Region


Module MainModule

#Region "ADO.NE Data Services URLs"
    Private schoolLinqToEntitiesUri As String = _
    "http://localhost:8889/SchoolLinqToEntities.svc"
    Private schoolLinqToSQLUri As String = _
    "http://localhost:8889/SchoolLinqToSQL.svc"
    Private samplesUri As String = _
    "http://localhost:8889/Samples.svc"
#End Region

    Sub Main()
        Try
            ' Update and query relational database via LINQ to Entities
            UpdateQueryRelationalDataByLinqToEntities()

            ' Update and query relational database via LINQ to SQL
            UpdateRelationalDataByLinqToSQL()

            ' Query relational database via ADO.NET query options and
            ' custom service operations
            QueryRelationalDataByQueryOptionAndServiceOperations()

            ' Insert and query non-relational data
            UpdateQueryNonRelationalData()

        Catch ex As Exception

            ' Retrieve the exception information if there is some
            ' DataServiceException is thrown at the server side
            If TypeOf ex.InnerException Is DataServiceClientException Then

                ' Parse the DataServieClientException
                Dim innerException = ParseDataServiceClientException( _
                ex.InnerException.Message)

                ' Display the DataServiceClientException message
                If innerException IsNot Nothing Then
                    Console.WriteLine("DataServiceException Message: " & _
                            innerException.Message)

                End If
            Else
                Console.WriteLine("The application throws the error: " & _
                        ex.Message)
            End If
        Finally
            Console.ReadLine()
        End Try

    End Sub


#Region "Update and query methods"
    ''' <summary>
    ''' Update and query relational database via LINQ to Entities
    ''' </summary>
    Private Sub UpdateQueryRelationalDataByLinqToEntities()

        Console.WriteLine("======================================")
        Console.WriteLine("Update and query data via LINQ to Entities:")
        Console.WriteLine("======================================")

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Initialize the DataService object for ADO.NET Entity Data Model
        '

        Dim svc As SchoolLinqToEntitiesService.SQLServer2005DBEntities = _
        New SchoolLinqToEntitiesService.SQLServer2005DBEntities( _
        New Uri(schoolLinqToEntitiesUri))


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Insert wrong data which the server side handles
        '

        ' Create a wrong instructor
        Dim wrongInstructor As New SchoolLinqToEntitiesService.Person() With _
        { _
            .FirstName = "Riquel", _
            .LastName = "Dong", _
            .HireDate = DateTime.Now, _
            .PersonCategory = 3 _
        }

        Console.Write("Insert wrong Person information into database...")

        ' Update the changes to the database
        If (UpdateData(svc)) Then
            Console.WriteLine("Successfully!")
        Else
            Console.WriteLine("Failed!")

            Try

                ' Get the person update exception
                svc.Execute(Of SchoolLinqToEntitiesService.Person)( _
                New Uri("/GetPersonUpdateException", UriKind.Relative))

            Catch ex As Exception

                ' Retrieve the exception information if there is some
                ' DataServiceException is thrown at the server side
                If TypeOf ex.InnerException Is DataServiceClientException Then

                    ' Parse the DataServieClientException
                    Dim innerException = ParseDataServiceClientException( _
                    ex.InnerException.Message)

                    ' Display the DataServiceClientException message
                    If innerException IsNot Nothing Then
                        Console.WriteLine("DataServiceException Message: " & _
                                innerException.Message)
                    End If
                End If
            End Try

            ' Detach the incorrect Person object
            svc.Detach(wrongInstructor)
        End If


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Insert relational data into database 
        ' 

        ' Create a new instructor
        Dim newInstructor As New SchoolLinqToEntitiesService.Person() With _
        { _
            .FirstName = "Riquel", _
            .LastName = "Dong", _
            .HireDate = DateTime.Now, _
            .PersonCategory = 2 _
        }

        ' Create a new course
        Dim newCourse1 As New SchoolLinqToEntitiesService.Course() With _
        { _
            .CourseID = 5010, _
            .Title = "Network", _
            .Credits = 4, _
            .DepartmentID = 1 _
        }

        ' Create a new course
        Dim newCourse2 As New SchoolLinqToEntitiesService.Course() With _
        { _
            .CourseID = 5020, _
            .Title = "Database", _
            .Credits = 3, _
            .DepartmentID = 1 _
        }

        ' Add the newly-created instructor into context
        svc.AddToPerson(newInstructor)

        ' Add the newly-created courses into context
        svc.AddToCourse(newCourse1)
        svc.AddToCourse(newCourse2)

        ' Add relationships to the newly-created instructor and courses
        svc.AddLink(newCourse1, "Person", newInstructor)
        svc.AddLink(newCourse2, "Person", newInstructor)

        Console.Write("Insert related data into database...")

        ' Update the changes to the database
        If UpdateData(svc) Then
            Console.WriteLine("Successfully!")
        Else
            Console.WriteLine("Faild!")
        End If

        Console.WriteLine()


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Query single data table via LINQ
        ' 

        ' Get all the teachers whose first name is 'Roger'
        ' LINQ operator and ADO.NET Data Service query option comparison
        ' 'where'(LINQ) <==> 'filter'(Query Option)
        Dim teachers = From p In svc.Person _
                       Where p.PersonCategory = 2 And p.FirstName = "Roger" _
                       Select p

        Console.WriteLine("All the teachers whose first name is 'Roger':")

        ' Display the query results
        For Each t In teachers
            Console.WriteLine("{0} {1}", t.FirstName, t.LastName)
        Next

        Console.WriteLine()


        ' Get the third and fourth newly-enrolled students
        ' LINQ operator and ADO.NET Data Service query option comparison
        ' 'where'(LINQ) <==> 'filter'(Query Option)
        ' 'orderby' (LINQ) <==> 'orderby' (Query Option)
        ' 'Skip' (LINQ) <=> 'skip' (Query Option)
        ' 'Take' (LINQ) <==> 'top' (Query Option)
        Dim students = (From p In svc.Person _
                        Where p.PersonCategory = 1 _
                        Order By p.EnrollmentDate Descending _
                        Select p).Skip(2).Take(2)

        Console.WriteLine("The third and fourth newly-enrolled students:")

        ' Display the query results
        For Each s In students
            Console.WriteLine("{0} {1}", s.FirstName, s.LastName)
        Next

        Console.WriteLine()


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Query relational data tables via LINQ and custom service operation
        ' 

        ' Get the instructors whose name is 'Riquel Dong'
        Dim instructors = From p In svc.Person _
                          Where p.LastName = "Dong" And p.FirstName = "Riquel" _
                          Select p

        ' Get all the courses that 'Riquel Dong' owns
        For Each i In instructors

            ' Call the service operation CoursesByPersonID to get the 
            ' certain person's courses based on primary key PersonID
            Dim uri As Uri = New Uri(String.Format("/CoursesByPersonID?ID={0}", _
                                                   i.PersonID), UriKind.Relative)

            ' Exceute the URL to the retrieve the course list
            Dim courses = svc.Execute(Of SchoolLinqToEntitiesService.Course) _
            (uri)

            Console.WriteLine("The instructor {0}'s couses:", _
                    i.FirstName & " " & i.LastName)

            ' Display the query results
            For Each c In courses
                Console.WriteLine("Course Title: {0}, Credits: {1}", _
                        c.Title, c.Credits)
            Next
        Next

        Console.WriteLine()

    End Sub


    ''' <summary>
    ''' Update and query relational database via LINQ to SQL
    ''' </summary>
    Private Sub UpdateRelationalDataByLinqToSQL()

        Console.WriteLine("======================================")
        Console.WriteLine("Update and query data via LINQ to SQL:")
        Console.WriteLine("======================================")

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Initialize the DataService object for LINQ to SQL Data Class
        '

        Dim svc As New SchoolLinqToSQLService.SchoolLinqToSQLDataContext( _
        New Uri(schoolLinqToSQLUri))


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Insert relational data into database 
        '

        ' Create a new student
        Dim newStudent As New SchoolLinqToSQLService.Person() With _
        { _
            .FirstName = "Lingzhi", _
            .LastName = "Sun", _
            .HireDate = DateTime.Now, _
            .PersonCategory = 1 _
        }

        ' Create a new course
        Dim newCourse1 As New SchoolLinqToSQLService.Course() With _
        { _
            .CourseID = 5030, _
            .Title = "Operation System", _
            .Credits = 4, _
            .DepartmentID = 1 _
        }

        ' Create a new course
        Dim newCourse2 As New SchoolLinqToSQLService.Course() With _
        { _
            .CourseID = 5040, _
            .Title = ".NET Framework", _
            .Credits = 3, _
            .DepartmentID = 1 _
        }

        ' Add the newly-created instructor into context
        svc.AddToPersons(newStudent)

        ' Add the newly-created courses into context
        svc.AddToCourses(newCourse1)
        svc.AddToCourses(newCourse2)

        Console.Write("Insert data into database...")

        ' Update the changes to the database
        If UpdateData(svc) Then
            Console.WriteLine("Successfully!")
        Else
            Console.WriteLine("Failed!")
        End If

        ' Create a CourseGrade object
        Dim newCourseGrade1 As New SchoolLinqToSQLService.CourseGrade() With _
        { _
            .StudentID = newStudent.PersonID, _
            .CourseID = newCourse1.CourseID, _
            .Grade = New Decimal(3.0) _
        }

        ' Create a CourseGrade object
        Dim newCourseGrade2 As New SchoolLinqToSQLService.CourseGrade() With _
        { _
            .StudentID = newStudent.PersonID, _
            .CourseID = newCourse2.CourseID, _
            .Grade = New Decimal(3.0) _
        }

        ' Add the newly-created CourseGrade objects into context 
        svc.AddToCourseGrades(newCourseGrade1)
        svc.AddToCourseGrades(newCourseGrade2)

        Console.Write("Insert data relationships into database...")

        ' Update the changes to the database
        If UpdateData(svc) Then
            Console.WriteLine("Successfully!")
        Else
            Console.WriteLine("Failed!")
        End If

        Console.WriteLine()


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Query single data table via LINQ
        '

        ' Get all the teachers whose first name is 'Roger'
        ' LINQ operator and ADO.NET Data Service query option comparison
        ' 'where'(LINQ) <==> 'filter'(Query Option)
        Dim teachers = From p In svc.Persons _
                       Where p.PersonCategory = 2 And p.FirstName = "Roger" _
                       Select p

        Console.WriteLine("All the teachers whose first name is 'Roger':")

        ' Display the query results
        For Each t In teachers
            Console.WriteLine("{0} {1}", t.FirstName, t.LastName)
        Next

        Console.WriteLine()


        ' Get the third and fourth newly-enrolled students
        ' LINQ operator and ADO.NET Data Service query option comparison
        ' 'where'(LINQ) <==> 'filter'(Query Option)
        ' 'orderby' (LINQ) <==> 'orderby' (Query Option)
        ' 'Skip' (LINQ) <=> 'skip' (Query Option)
        ' 'Take' (LINQ) <==> 'top' (Query Option)
        Dim students = (From p In svc.Persons _
                        Where p.PersonCategory = 1 _
                        Order By p.EnrollmentDate Descending _
                        Select p).Skip(2).Take(2)

        Console.WriteLine("The third and fourth newly-enrolled students:")

        ' Display the query results
        For Each s In students
            Console.WriteLine("{0} {1}", s.FirstName, s.LastName)
        Next

        Console.WriteLine()


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Query relational data tables via LINQ
        '

        ' Get the students whose name is 'Lingzhi Sun'
        Dim studentsWithCourses = From p In svc.Persons _
                                  Where p.LastName = "Sun" And p.FirstName = _
                                  "Lingzhi" _
                                  Select p

        ' Get all the courses that' Lingzhi Sun' learns
        For Each s In studentsWithCourses

            ' Create URL to get the certain person's CourseGrade list 
            ' based on primary key PersonID
            Dim uri As New Uri(String.Format("/Persons({0})/CourseGrades", _
                                             s.PersonID), UriKind.Relative)

            ' Exceute the URL to the retrieve the CourseGarde list 
            Dim courseGrades = svc.Execute(Of SchoolLinqToSQLService. _
            CourseGrade)(uri)

            Console.WriteLine("The student {0}'s courses:", s.FirstName _
                    & " " & s.LastName)

            For Each cg In courseGrades

                ' Create URL to get the certain course list based on 
                ' primary key CourseID
                uri = New Uri(String.Format("/Courses({0})", cg.CourseID), _
                              UriKind.Relative)

                ' Exceute the URL to the retrieve the course list 
                Dim course = svc.Execute(Of SchoolLinqToSQLService.Course) _
                (uri).First()

                ' Display the query results
                Console.WriteLine("Course Title: {0}, Credits: {1}", _
                       course.Title, course.Credits)
            Next
        Next

        Console.WriteLine()
    End Sub


    ''' <summary>
    ''' Query relational database via ADO.NET query options and custom 
    ''' service operations
    ''' </summary>
    Private Sub QueryRelationalDataByQueryOptionAndServiceOperations()

        Console.WriteLine("======================================")
        Console.WriteLine("Query data via ADO.NET query options" & _
                          vbNewLine & _
                          "and custom service operations:")
        Console.WriteLine("======================================")

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Initialize the DataService object
        '

        Dim context As New DataServiceContext(New Uri( _
                                              schoolLinqToEntitiesUri))


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Query single data table via ADO.NET query options
        '

        ' Get all the teachers whose first name is 'Roger'
        ' LINQ operator and ADO.NET Data Service query option comparison
        ' 'where'(LINQ) <==> 'filter'(Query Option)
        Dim teachers = context.Execute(Of SchoolLinqToSQLService.Person)( _
            New Uri("/Person?$filter=PersonCategory eq 2 and " & _
                    "FirstName eq 'Roger'", UriKind.Relative))

        Console.WriteLine("All the teachers whose first name is 'Roger':")

        ' Display the query results
        For Each t In teachers
            Console.WriteLine("{0} {1}", t.FirstName, t.LastName)
        Next

        Console.WriteLine()


        ' Get the third and fourth newly-enrolled students
        ' LINQ operator and ADO.NET Data Service query option comparison
        ' 'where'(LINQ) <==> 'filter'(Query Option)
        ' 'orderby' (LINQ) <==> 'orderby' (Query Option)
        ' 'Skip' (LINQ) <=> 'skip' (Query Option)
        ' 'Take' (LINQ) <==> 'top' (Query Option)
        Dim students = context.Execute(Of SchoolLinqToSQLService.Person)( _
            New Uri("/Person?$filter=PersonCategory eq 1&$" & _
                    "orderby=EnrollmentDate desc&$skip=2&$top=2", _
                    UriKind.Relative))

        Console.WriteLine("The third and fourth newly-enrolled students:")

        ' Display the query results
        For Each s In students
            Console.WriteLine("{0} {1}", s.FirstName, s.LastName)
        Next

        Console.WriteLine()


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Query single data table via service operations and the result
        ' entity class misses some properties
        '

        ' Initialize a new DataServiceContext
        Dim contextSQL As New DataServiceContext(New Uri(schoolLinqToSQLUri))

        ' Set tje IgnoreMissingProperties to ture to retrieve entities
        ' that miss properties
        contextSQL.IgnoreMissingProperties = True

        ' SQL search command
        Dim searchText As String = _
            "SELECT * FROM [Course] AS [c] WHERE [c].[Credits] = 4"

        ' Query the SQL commands at the server side
        Dim courses = contextSQL.Execute(Of TempCourse)(New Uri( _
                String.Format("/SearchCourses?searchText='{0}'", searchText), _
                UriKind.Relative))

        Console.WriteLine("All The Courses which credits is 4:")
        Console.WriteLine("Course ID        Title")

        ' Display the query results
        For Each c In courses
            Console.WriteLine("  {0}          {1}", c.CourseID, _
                    c.Title)
        Next

        Console.WriteLine()

    End Sub


    ''' <summary>
    ''' Insert and query non-relational data
    ''' </summary>
    Private Sub UpdateQueryNonRelationalData()

        Console.WriteLine("======================================")
        Console.WriteLine("Insert and query non-relational data:")
        Console.WriteLine("======================================")

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Initialize the DataService object for non-relational data
        '

        Dim svc As New DataServiceContext(New Uri(samplesUri))


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Insert non-relational data
        '

        Console.Write("Insert non-relational data...")

        Dim newCategory As New SamplesService.Category() With _
        {.CategoryName = "Silverlight"}

        svc.AddObject("Categories", newCategory)

        ' Update the changes to the non-relational data
        If UpdateData(svc) Then
            Console.WriteLine("Successfully!")
        Else
            Console.WriteLine("Failed!")
        End If

        Console.WriteLine()

        ' Get all the category objects
        Dim categories = svc.Execute(Of SamplesService.Category)(New Uri( _
                            "/Categories", UriKind.Relative))

        Console.WriteLine("Categories after insert operation:")

        ' Display the query results
        For Each c In categories
            Console.WriteLine(c.CategoryName)
        Next

        Console.WriteLine()


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Query non-relational data
        '

        ' Get all the project objects to the local memory
        Dim projects = svc.Execute(Of SamplesService.Project)(New Uri( _
                            "/Projects", UriKind.Relative)).ToList()

        For Each p In projects

            ' Create URL to get the project category information based
            ' on primary key ProjectName 
            Dim uri As New Uri(String.Format("/Projects('{0}')/" & _
                    "ProjectCategory", p.ProjectName), UriKind.Relative)

            ' Set the ProjectCategory property
            Dim category As SamplesService.Category = svc.Execute(Of  _
                SamplesService.Category)(uri).First()

            p.ProjectCategory = category
        Next

        ' Group the projects by project category
        Dim projectsGroups = From p In projects _
                             Group p By p.ProjectCategory Into Group

        Console.WriteLine("Group the sample projects by category name:")

        ' Display the query results
        For Each g In projectsGroups
            Console.WriteLine("Projects in Category: {0}", _
                    g.ProjectCategory.CategoryName)

            For Each p In g.Group
                Console.WriteLine("{0} owned by {1}", p.ProjectName, _
                        p.Owner)
            Next

            Console.WriteLine()
        Next
    End Sub


    ''' <summary>
    ''' Update the changes to the data at the server side
    ''' </summary>
    ''' <param name="context">The DataServiceContext to be updated.
    ''' </param>
    ''' <returns>Whether the update is successful.</returns>
    Private Function UpdateData(ByVal context As DataServiceContext) _
        As Boolean

        Try
            ' Get the response from the DataServiceContext update operation
            Dim response As DataServiceResponse = context.SaveChanges()

            Dim isSuccess As Boolean = False

            For Each r In response

                ' If response status code shows the update fails
                ' return false
                If Not r.StatusCode.ToString().StartsWith("2") Then
                    Return False
                Else
                    isSuccess = True
                End If
            Next

            ' The update is successful
            Return isSuccess
        Catch ex As Exception

            ' Retrieve the exception information if there is some
            ' DataServiceException is thrown at the server side
            If TypeOf ex.InnerException Is DataServiceClientException Then

                ' Parse the DataServieClientException
                Dim innerException = ParseDataServiceClientException( _
                    ex.InnerException.Message)

                ' Display the DataServiceClientException message
                If innerException IsNot Nothing Then
                    Console.WriteLine("DataServiceException Message: " & _
                            innerException.Message)
                End If
            Else
                Console.WriteLine("The update operation throws the error: " & _
                   "{0}", ex.Message)
            End If

            Return False
        End Try
    End Function


    ''' <summary>
    ''' Parse the DataServiceClientException to get the error code 
    ''' and message
    ''' </summary>
    ''' <param name="exception">The DataServiceClientException message
    ''' </param>
    ''' <returns>The local InnerDataServiceException class</returns>
    Private Function ParseDataServiceClientException(ByVal exception As String) _
        As InnerDataServiceException

        Try
            ' Load the DataServiceClientException by XDocument
            Dim doc As XDocument = XDocument.Parse(exception)

            ' Return the error code and message
            Return New InnerDataServiceException With _
            { _
                .Code = If(String.IsNullOrEmpty(doc...<code>.Value), 400, _
                           Integer.Parse(doc...<code>.Value)), _
                .Message = doc...<message>.Value _
            }
        Catch ex As Exception
            Console.WriteLine( _
                "Exceptions when parsing the DataServiceClientException: " _
                & ex.Message)
            Return Nothing
        End Try
    End Function
#End Region

#Region "Local entity classes"
    ' The local course entity class that misses some properties
    ' It only contains CourseID, Title, and Credits properties
    Public Class TempCourse

        Private _courseID As Integer
        Private _title As String

        Public Property CourseID() As Integer
            Get
                Return _courseID
            End Get
            Set(ByVal value As Integer)
                _courseID = value
            End Set
        End Property

        Public Property Title() As String
            Get
                Return _title
            End Get
            Set(ByVal value As String)
                _title = value
            End Set
        End Property

    End Class

    ' The local InnerDataServiceException to hold the DataServiceException
    ' data from the server side
    Public Class InnerDataServiceException

        Private _code As Integer
        Private _message As String

        Public Property Code() As String
            Get
                Return _code
            End Get
            Set(ByVal value As String)
                _code = value
            End Set
        End Property

        Public Property Message() As String
            Get
                Return _message
            End Get
            Set(ByVal value As String)
                _message = value
            End Set
        End Property

    End Class
#End Region

End Module
