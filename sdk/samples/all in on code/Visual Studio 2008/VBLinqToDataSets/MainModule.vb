'****************************** Module Header ******************************\
' Module Name:    Program.cs
' Project:        VBLinqToDataSets
' Copyright (c) Microsoft Corporation.
'
' The VBLinqToDataSets sample demonstrates the Microsoft Language-Integrated 
' Query (LINQ) technology to access untyped DataSet and strong typed DataSet 
' using VB.NET. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' History:
' * 5/8/2009 05:00 PM Lingzhi Sun Created
'***************************************************************************/

#Region "Imports directory"
Imports System.IO
Imports System.Configuration
Imports System.Data.SqlClient
Imports VBLinqToDataSets.SQLServer2005DBDataSetTableAdapters
Imports VBLinqToDataSets.SQLServer2005DBDataSet
#End Region


Module MainModule

    Sub Main()
        Try
            ' Query the data in untyped DataSet
            QueryDBByUntypedDataSet()

            Console.WriteLine()
            Console.WriteLine("======================================")
            Console.WriteLine()

            ' Query the data in strong typed DataSet
            QueryDBByStrongTypedDataSet()

        Catch ex As Exception

            Console.WriteLine( _
                "The application throws the error: {0}", ex.Message)

        End Try
    End Sub


    ''' <summary>
    ''' This method works with the untyped DataSet
    ''' </summary>
    Private Sub QueryDBByUntypedDataSet()

        Console.WriteLine("Use LINQ to query untyped DataSet...")


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Fill the untyped DataSet && Insert data into database
        ' 

        Console.WriteLine()
        Console.WriteLine("Fill the DataTables and Insert a new record " _
            + "into the Person DataTable...")

        ' Get the connection string from App.config. (The data source is
        ' created in the example SQLServer2005DB)
        Dim connStr As String = ConfigurationManager.ConnectionStrings( _
            "SQLServer2005DB").ConnectionString

        ' Create SqlDataAdapter object and fill the Person DataTable
        Dim selectCmd As String = "SELECT * FROM Person"
        Dim da As New SqlDataAdapter(selectCmd, connStr)
        Dim ds As New DataSet()
        da.Fill(ds, "Person")

        ' Create a new row for the Person DataTable
        ' We don't set the PersonCategory column value so the
        ' database will set the default value to this column
        Dim rowPerson As DataRow = ds.Tables.Item("Person").NewRow
        rowPerson.Item("FirstName") = "Lingzhi"
        rowPerson.Item("LastName") = "Sun"
        rowPerson.Item("HireDate") = Convert.DBNull
        rowPerson.Item("EnrollmentDate") = DateTime.Now
        rowPerson.Item("Picture") = ReadImage("MSDN.jpg")
        ds.Tables.Item("Person").Rows.Add(rowPerson)

        ' Create corresponding update/insert/delete SQL command
        Dim cmdBuilder As New SqlCommandBuilder(da)

        ' Update the changes in the Person DataTable to the database
        da.Update(ds, "Person")

        ' Fill the Course DataTable
        da.SelectCommand.CommandText = "SELECT * FROM Course"
        da.Fill(ds, "Course")

        ' Fill the CourseGrade DataTable
        da.SelectCommand.CommandText = "SELECT * FROM CourseGrade"
        da.Fill(ds, "CourseGrade")


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Perform the query operation in one DataTable
        ' 

        Console.WriteLine()
        Console.WriteLine("Query people whose first name is Roger:")

        ' Perform the query
        Dim query = From p In ds.Tables("Person").AsEnumerable() _
                    Where p.Field(Of String)("FirstName") = "Roger" _
                    Select p

        ' Display the query results
        For Each p In query
            Console.WriteLine("ID = {0}, Name = {1} {2}", _
                              p.Field(Of Integer)("PersonID"), _
                              p.Field(Of String)("FirstName"), _
                              p.Field(Of String)("LastName"))
        Next


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Perform the query operation across multiple DataTables
        ' 

        Console.WriteLine()
        Console.WriteLine("Query the max grade of each course:")

        ' Perform the query and get a collection of hte anonymous type,
        ' New With { .CourseID As Interger, .Title As String, _
        '               .TopGrade As Decimal }
        Dim courses = From grade In ds.Tables("CourseGrade").AsEnumerable() _
                      Group grade By Key = grade.Field(Of Integer)("CourseID") Into Group _
                      Join cur In ds.Tables("Course").AsEnumerable() _
                      On Key Equals cur.Field(Of Integer)("CourseID") _
                      Select New With _
                      { _
                        .CourseID = Key, _
                        .Title = cur.Field(Of String)("Title"), _
                        .TopGrade = Group.Max( _
                            Function(row As DataRow) _
                                If(row.IsNull("Grade"), Decimal.Zero, _
                                   row.Field(Of Decimal)("Grade"))) _
                      }

        ' Display the query results
        For Each c In courses
            Console.WriteLine("Course = {0}, TopGrade = {1}", c.Title, _
                c.TopGrade)
        Next

    End Sub


    ''' <summary>
    ''' This method works with the strong typed DataSet
    ''' </summary>
    Private Sub QueryDBByStrongTypedDataSet()

        Console.WriteLine("Use LINQ to query strong typed DataSet...")


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Fill the strong typed DataSet && Insert data into database
        ' 

        ' Create a strong typed DataSet object
        Dim dsSQLServer As New SQLServer2005DBDataSet

        ' Create the PersonTableAdapter and fill the Person DataTable
        Dim taPerson As New PersonTableAdapter
        Dim tblPerson As PersonDataTable = dsSQLServer.Person
        taPerson.Fill(tblPerson)

        ' Create a new row into the Person DataTable
        ' We don't set the PersonCategory value because we have set the 
        ' default value property of the DataSet PersonCategory column.
        Dim rowPerson As PersonRow = tblPerson.NewPersonRow
        rowPerson.LastName = "Sun"
        rowPerson.FirstName = "Hongye"
        rowPerson.SetHireDateNull()
        rowPerson.EnrollmentDate = DateTime.Now
        rowPerson.Picture = ReadImage("MSDN.jpg")
        tblPerson.AddPersonRow(rowPerson)

        ' Update the database
        taPerson.Update(tblPerson)

        ' Use PersonTableAdapter to insert new record into the database 
        ' directly
        taPerson.Insert("Ge", "Jialiang", New DateTime?(DateTime.Now), Nothing, ReadImage("MSDN.jpg"), 2)

        ' Create the CourseTableAdapter and fill the Course DataTable
        Dim taCourse As New CourseTableAdapter
        Dim tblCourse As CourseDataTable = dsSQLServer.Course
        taCourse.Fill(tblCourse)

        ' Create the CourseGradeTableAdapter and fill the CourseGrade 
        ' DataTable
        Dim taCourseGrade As New CourseGradeTableAdapter
        Dim tblCourseGrade As CourseGradeDataTable = dsSQLServer.CourseGrade
        taCourseGrade.Fill(tblCourseGrade)


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Perform the query operation in one DataTable
        ' 

        Console.WriteLine()
        Console.WriteLine("Query people whose first name is Roger:")

        ' Perform the query
        Dim query = From p In tblPerson.AsEnumerable() _
                    Where p.FirstName = "Roger" _
                    Select p

        ' Display the query results
        For Each p In query
            Console.WriteLine("ID = {0}, Name = {1} {2}", p.PersonID, _
                p.FirstName, p.LastName)
        Next


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Perform the query operation across multiple DataTables
        ' 

        Console.WriteLine()
        Console.WriteLine("Query the max grade of each course:")

        ' Perform the query and get a collection of hte anonymous type,
        ' New With { .CourseID As Interger, .Title As String, _
        '               .TopGrade As Decimal }
        Dim courses = From grade In tblCourseGrade.AsEnumerable() _
                      Group grade By Key = grade.CourseID Into Group _
                      Join cur In tblCourse.AsEnumerable() _
                      On Key Equals cur.CourseID _
                      Select New With _
                      { _
                        .CourseID = Key, _
                        .Title = cur.Title, _
                        .TopGrade = Group.Max( _
                            Function(row As CourseGradeRow) _
                                If(row.IsGradeNull(), Decimal.Zero, _
                                   row.Grade)) _
                      }

        ' Display the query results
        For Each c In courses
            Console.WriteLine("Course = {0}, TopGrade = {1}", c.Title, _
                c.TopGrade)
        Next


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Perform the query operation across multiple DataTables
        ' 

        Console.WriteLine()
        Console.WriteLine("Query all grades that Nino got:")

        ' Perform the query betweem related DataTables and get the anonymous
        ' type, New With { .Person As PersonRow, .Course As CourseRow, _
        '                   .CourseGrade As CourseGradeRow }
        Dim grades = From per In tblPerson.AsEnumerable() _
                     From gra In per.GetCourseGradeRows() _
                     Join cur In tblCourse.AsEnumerable() _
                     On gra.CourseID Equals cur.CourseID _
                     Where per.FirstName = "Nino" _
                     Select New With _
                     { _
                        .Person = per, _
                        .Course = cur, _
                        .CourseGrade = gra _
                     }

        ' Display the query results
        For Each grade In grades
            Console.WriteLine("FirstName = {0}, Course= {1}, Grade= {2}", _
                grade.Person.FirstName, grade.Course.Title, _
                grade.CourseGrade.Grade)
        Next

    End Sub


    ''' <summary>
    ''' Read an image file to an array of bytes.
    ''' </summary>
    ''' <param name="path">The path of the image file.</param>
    ''' <returns>The output of the array.</returns>
    Private Function ReadImage(ByVal path As String) As Byte()
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
