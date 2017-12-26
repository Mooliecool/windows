'****************************** Module Header ******************************\
' Module Name:    MainModule.vb
' Project:        VBLinqToSQL
' Copyright (c) Microsoft Corporation.
'
' The VBLinqToSQL sample demonstrates the Microsoft Language-Integrated Query 
' (LINQ) technology to access databases using VB.NET. It shows the basic 
' structure of connecting and querying the data source and inserting data  
' into the database with LINQ.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' History:
' * 5/10/2009 10:00 PM Lingzhi Sun Created
'***************************************************************************/

#Region "Imports directory"
Imports System.IO
Imports System.Configuration
#End Region


Module MainModule

    Sub Main()

        ' Query the database using manually-created data classes
        QueryDBByManuallyCreatedClasses()

        Console.WriteLine()

        ' Query the database using designer-generated data classes
        QueryDBByDesignerGeneratedClasses()

    End Sub


    ''' <summary>
    ''' The following method works with the manually-created classes
    ''' </summary>
    Sub QueryDBByManuallyCreatedClasses()

        Console.WriteLine("Query using the manually-created classes.")

        Try

            '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
            ' Construct the data context.
            ' 

            ' Get the connection string from App.config. (The data source is
            ' created in the example SQLServer2005DB)
            Dim connStr As String = ConfigurationManager.ConnectionStrings _
                ("SQLServer2005DB").ConnectionString

            ' DataContext takes a connection string
            Dim db As VBLinqToSQL.Manual.SchoolDataContext = New  _
                VBLinqToSQL.Manual.SchoolDataContext(connStr)


            '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
            ' Perform the Insert operation.
            ' 

            Console.WriteLine(vbNewLine & "Add a new student to the Person" _
                              & " table")

            ' Create a new Student.
            ' We don't need to set the PersonCategory value here if we want
            ' the default value to be set to this column in the database.
            Dim student As VBLinqToSQL.Manual.Student = New  _
                VBLinqToSQL.Manual.Student() With _
                { _
                    .FirstName = "Lingzhi", _
                    .LastName = "Sun", _
                    .EnrollmentDate = DateTime.Now, _
                    .Picture = ReadImage("MSDN.jpg") _
                }

            ' Add the new object to Students.
            db.Students.InsertOnSubmit(student)
            ' Submit the change to the database.
            db.SubmitChanges()


            '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
            ' Perform the Query operation.
            ' 

            ' Query in one table.

            Console.WriteLine(vbNewLine & "Query students whose first name" _
                              & " is Roger:")

            Dim students = From p In db.Students _
                           Where p.FirstName = "Roger" _
                           Select p

            For Each p In students
                Console.WriteLine("ID = {0}, Name = {1} {2}", p.PersonID, _
                    p.FirstName, p.LastName)
            Next

            ' Query across multiple tales.

            Console.WriteLine(vbNewLine & "Query max. grade of each course:")

            Dim courses = From grade In db.CourseGrades _
                          Group grade By Key = grade.CourseID Into Group _
                          Join cur In db.Courses _
                          On Key Equals cur.CourseID _
                          Select New With _
                          { _
                            .CourseID = Key, _
                            .Title = cur.Title, _
                            .TopGrade = Group.Max( _
                                Function(g As VBLinqToSQL.Manual.CourseGrade) _
                                    If(g.Grade = Nothing, Decimal.Zero, _
                                       g.Grade)) _
                          }

            For Each c In courses
                Console.WriteLine("Course = {0}, TopGrade = {1}", _
                    c.Title, c.TopGrade)
            Next


            Console.WriteLine(vbNewLine & "Query all grades that Nino got")

            Dim grades = From stu In db.Students _
                         From gra In stu.CourseGrades _
                         Join cur In db.Courses _
                         On gra.CourseID Equals cur.CourseID _
                         Where stu.FirstName = "Nino" _
                         Select New With _
                         { _
                            .Student = stu, _
                            .Course = cur, _
                            .CourseGrade = gra _
                         }

            For Each grade In grades
                Console.WriteLine("FirstName = {0}, Course= {1}, Grade= {2}", _
                    grade.Student.FirstName, _
                    grade.Course.Title, _
                    grade.CourseGrade.Grade)
            Next

        Catch ex As Exception
            Console.WriteLine("The application throws the error: {0}", _
                              ex.Message)
        End Try
    End Sub


    ''' <summary>
    ''' The following method works with the designer-generated classes
    ''' </summary>
    Sub QueryDBByDesignerGeneratedClasses()

        Console.WriteLine(vbNewLine & "Add a new student to the Person " & _
                          "table")

        Try

            '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
            ' Construct the data context.
            '

            Dim db As VBLinqToSQL.Designer.SchoolDataContext = New  _
                VBLinqToSQL.Designer.SchoolDataContext()


            '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
            ' Perform the Insert operation.
            '

            Dim person As VBLinqToSQL.Designer.Person = New  _
                VBLinqToSQL.Designer.Person() With _
                { _
                    .FirstName = "Lingzhi", _
                    .LastName = "Sun", _
                    .PersonCategory = 1, _
                    .EnrollmentDate = DateTime.Now _
                }
            Dim bImage As Byte() = ReadImage("MSDN.jpg")
            If Not bImage Is Nothing Then
                person.Picture = bImage
            End If

            ' Add the new object to the Students.
            db.Persons.InsertOnSubmit(person)

            ' Submit the change to the database.
            db.SubmitChanges()


            '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
            ' Perform the Query operation.
            '

            ' Query in one table.

            Console.WriteLine(vbNewLine & "Query students whose first name" _
                              & " is Roger:")

            Dim persons = From p In db.Persons _
                          Where p.FirstName = "Roger" _
                          Select p

            For Each p In persons
                Console.WriteLine("ID = {0}, Name = {1} {2}", p.PersonID, _
                    p.FirstName, p.LastName)
            Next

            ' Query across multiple tales.

            Console.WriteLine(vbNewLine & "Query max. grade of each course:")

            Dim courses = From grade In db.CourseGrades _
                          Group grade By Key = grade.CourseID Into Group _
                          Join cur In db.Courses _
                          On Key Equals cur.CourseID _
                          Select New With _
                          { _
                            .CourseID = Key, _
                            .Title = cur.Title, _
                            .TopGrade = Group.Max( _
                                Function(g As VBLinqToSQL.Designer.CourseGrade) _
                                    If(g.Grade = Nothing, Decimal.Zero, _
                                       g.Grade)) _
                          }

            For Each c In courses
                Console.WriteLine("Course = {0}, TopGrade = {1}", _
                    c.Title, c.TopGrade)
            Next


            Console.WriteLine(vbNewLine & "Query all grades that Nino got")

            Dim grades = From per In db.Persons _
                         From gra In per.CourseGrades _
                         Join cur In db.Courses _
                         On gra.CourseID Equals cur.CourseID _
                         Where per.FirstName = "Nino" _
                         Select New With _
                         { _
                            .Student = per, _
                            .Course = cur, _
                            .CourseGrade = gra _
                         }

            For Each grade In grades
                Console.WriteLine("FirstName = {0}, Course= {1}, Grade= {2}", _
                    grade.Student.FirstName, _
                    grade.Course.Title, _
                    grade.CourseGrade.Grade)
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
