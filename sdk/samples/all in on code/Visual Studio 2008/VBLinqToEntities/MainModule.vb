'****************************** Module Header ******************************\
' Module Name:    MainModule.vb
' Project:        VBLinqToEntities
' Copyright (c) Microsoft Corporation.
'
' The VBLinqToEntities example demonstrates the Microsoft Language-Integrated 
' Query (LINQ) technology to access ADO.NET Entity Data Model using VB.NET. 
' It shows the basic structure of connecting and querying the data source and
' inserting data into the database with LINQ.  It especially demonstrates the
' new features of LINQ to Entities comparing with LINQ to SQL, Inheritance
' and Many-to-Many Relationship (without payload).
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' History:
' * 5/11/2009 01:00 AM Lingzhi Sun Created
'***************************************************************************/

#Region "Improts directories"
Imports System.IO
#End Region


Module MainModule

    Sub Main()

        Try

            '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
            ' Construct the object context
            ' 

            ' Create Entity ObectContext for School database
            Dim entities As SchoolEntities = New SchoolEntities()

            '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
            ' Perform the Insert operation
            ' 

            Console.Write("Insert data into database...")

            ' Create a Student object which inherits the Person object
            Dim newStudent As Student = New Student() With _
            { _
                .FirstName = "Lingzhi", _
                .LastName = "Sun", _
                .EnrollmentDate = DateTime.Now, _
                .Picture = ReadImage("MSDN.jpg") _
            }

            ' Add the Student object into the Person list
            entities.AddToPerson(newStudent)

            ' Create a Instructor object which inherits the Person object
            Dim newInstructor As Instructor = New Instructor() With _
            { _
                .FirstName = "Jialiang", _
                .LastName = "Ge", _
                .HireDate = DateTime.Now, _
                .Picture = ReadImage("MSDN.jpg") _
            }

            ' Create a Course object
            Dim newCourse1 As Course = New Course() With _
            { _
                .CourseID = 5011, _
                .Title = "Computer", _
                .Credits = 4, _
                .DepartmentID = 1 _
            }

            ' Create a Course object
            Dim newCourse2 As Course = New Course() With _
            { _
                .CourseID = 5023, _
                .Title = "Database", _
                .Credits = 3, _
                .DepartmentID = 1 _
            }

            ' Add the new Course objects into the Instructor object's
            ' Course list (Many-to-Many Relationship)
            newInstructor.Course.Add(newCourse1)
            newInstructor.Course.Add(newCourse2)

            ' Add the Instructor object into the Person list
            ' This command will also add the corresponding related Course
            ' objects into the Course list
            ' We don't need to call entities.AddToCourse method here
            entities.AddToPerson(newInstructor)

            ' Update the changes to the database
            Dim result As Integer = entities.SaveChanges()

            If result > 0 Then
                Console.WriteLine("SUCCESS" & vbNewLine)
            End If


            '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
            ' Perform the query operation in one data table
            ' 

            Console.WriteLine(vbNewLine & "Query students whose first name" & _
                              " is Roger:" & vbNewLine)

            ' Perform the query
            Dim query = From p In entities.Person _
                        Where p.FirstName = "Roger" _
                        Select p

            For Each p In query
                Console.WriteLine("ID = {0}, Name = {1} {2}", p.PersonID, _
                    p.FirstName, p.LastName)
            Next

            Console.WriteLine()

            '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
            ' Perform the query operation across multiple data tables
            ' 

            Console.WriteLine(vbNewLine & "Query the max grade of each " & _
                              "course:" & vbNewLine)

            ' Perform the query and get an anonymous type collection
            Dim courses = From grade In entities.CourseGrade _
                          Group grade By Key = grade.Course.CourseID _
                          Into Group _
                          Join cur In entities.Course _
                          On Key Equals cur.CourseID _
                          Select New With _
                          { _
                            .CourseID = Key, _
                            .Title = cur.Title, _
                            .TopGrade = Group.Max( _
                                Function(g As CourseGrade) _
                                    If(g.Grade = Nothing, Decimal.Zero, _
                                       g.Grade)) _
                          }
            ' Display the query results
            For Each c In courses
                Console.WriteLine("Course = {0}, TopGrade = {1}", _
                    c.Title, c.TopGrade)
            Next

            Console.WriteLine()


            '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
            ' Perform the query operation across multiple related data 
            ' tables
            ' 

            Console.WriteLine(vbNewLine & "Query all the grades that Nino" & _
                              "got:" & vbNewLine)

            ' Perform the query between related data tables and get an 
            ' anonymous type collection
            Dim grades = From per In entities.Person.OfType(Of Student)() _
                         From gra In per.CourseGrade _
                         Join cur In entities.Course _
                         On gra.Course Equals cur _
                         Where per.FirstName = "Nino" _
                         Select New With _
                         { _
                            .Student = per, _
                            .Course = cur, _
                            .CourseGrade = gra _
                         }

            ' Display the query results
            For Each grade In grades
                Console.WriteLine("FirstName = {0}, Course= {1}, Grade= {2}", _
                    grade.Student.FirstName, _
                    grade.Course.Title, _
                    grade.CourseGrade.Grade)
            Next

            Console.WriteLine()


            '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
            ' Perform the query operation accross Many-to-Many related 
            ' data tables
            ' 

            Console.WriteLine(vbNewLine & "Query all the courses that " & _
                              "Jialiang Ge owns:" & vbNewLine)

            ' Perform the query across Many-to-Many related data tables and 
            ' get an anonymous type collection
            Dim instructors = From i In entities.Person.OfType _
                              (Of Instructor)() _
                              Where i.LastName = "Ge" And _
                              i.FirstName = "Jialiang" _
                              Select New With _
                              { _
                                .Name = i.FirstName & " " & i.LastName, _
                                .Courses = i.Course _
                              }

            ' Display the query results
            For Each i In instructors
                Console.WriteLine("The instructors {0}'s couses:", i.Name)

                For Each c In i.Courses
                    Console.WriteLine("Course Title: {0}, Credits: {1}", _
                       c.Title, c.Credits)
                Next
            Next

        Catch ex As Exception

            Console.WriteLine("The application throws the error: {0}", _
                ex.Message)

        End Try
    End Sub


    ''' <summary>
    ''' Read an image file to an array of bytes.
    ''' </summary>
    ''' <param name="path">The path of the image file.</param>
    ''' <returns>The output of the array.</returns>
    Function ReadImage(ByVal path As String) As Byte()
        Try
            ' Open the image file
            Using fs As FileStream = New FileStream(path, FileMode.Open, _
                                                    FileAccess.Read)

                ' Create an array of bytes
                Dim bPicture As Byte() = New Byte(fs.Length - 1) {}

                ' Read the image file
                fs.Read(bPicture, 0, Convert.ToInt32(fs.Length))

                Return bPicture
            End Using
        Catch ex As Exception

            Console.WriteLine( _
                "The application throws the error: {0}", ex.Message)
            Return Nothing

        End Try
    End Function

End Module
