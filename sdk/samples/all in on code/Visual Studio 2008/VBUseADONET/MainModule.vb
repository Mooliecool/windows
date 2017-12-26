'****************************** Module Header ******************************\
' Module Name:  MainModule.vb
' Project:      VBUseADONET
' Copyright (c) Microsoft Corporation.
' 
' The VBUseADONET example demonstrates the Microsoft ADO.NET technology to 
' access databases using Visual Basic. It shows the basic structure of 
' connecting to a data source, issuing SQL commands, using Untyped DataSet,
' using Strong Typed DataSet, updating related data tables and performing the 
' cleanup. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

#Region "Imports directives"
Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Data
Imports System.Data.SqlClient
Imports System.Configuration
Imports System.IO
Imports System.Xml
Imports VBUseADONET
Imports VBUseADONET.SQLServer2005DBDataSetTableAdapters
Imports System.Windows.Forms
#End Region


Module MainModule

    Sub Main()

        'Create a SqlConnection object
        Dim conn As SqlConnection = Nothing

        Try
            ' ///////////////////////////////////////////////////////////////
            ' Connect to the data source.
            ' 

            Console.WriteLine("Connecting to the database ...")

            ' Get the connection string from App.config. (The data source is
            ' created in the sample SQLServer2005DB)
            Dim connStr As String = ConfigurationManager. _
                ConnectionStrings("SQLServer2005DB").ConnectionString

            ' Open the connection
            conn = New SqlConnection(connStr)
            conn.Open()

            ' ///////////////////////////////////////////////////////////////
            ' Build and Execute an ADO.NET Command.
            '
            ' It can be a SQL statement (SELECT/UPDATE/INSERT/DELETE), or a  
            ' stored procedure call. Here is the sample of an INSERT command.
            ' 

            InsertUsingSqlCommand(conn)

            ' ///////////////////////////////////////////////////////////////
            ' Use the Untyped DataSet Object.
            '
            ' The DataSet, which is an in-memory cache of data retrieved 
            ' from a data source, is a major component of the ADO.NET 
            ' architecture. The DataSet consists of a collection of 
            ' DataTable objects that you can relate to each other with 
            ' DataRelation objects.
            ' 

            SelectUsingUntypedDataSet(conn)

            ' ///////////////////////////////////////////////////////////////
            ' Use the Strong Typed DataSet Object.
            '
            ' A typed DataSet is a class that derives from a DataSet. As 
            ' such, it inherits all the methods, events, and properties of a 
            ' DataSet. Additionally, a typed DataSet provides strongly typed 
            ' methods, events, and properties. This means you can access 
            ' tables and columns by name, instead of using collection-based 
            ' methods. Aside from the improved readability of the code, a 
            ' typed DataSet also allows the Visual Studio .NET code editor 
            ' to automatically complete lines as you type.
            '  

            InsertSelectUsingStrongTypedDataSet()

            ' ///////////////////////////////////////////////////////////////
            ' Update Two Related Data Tables by TableAdapterManager and
            ' writting codes manually.
            '
            ' When you need to save data from a dataset that contains two or 
            ' more related data tables, you must send the changes to the 
            ' database in a specific order so that constraints are not 
            ' violated. When you update modified data in related tables, you 
            ' can provide the programmatic logic to extract the specific 
            ' subsets of data from each data table and send the updates to 
            ' the database in the correct order, or you can use the 
            ' TableAdapterManager component introduced in Visual Studio 2008. 
            '

            Dim RelatedTableUpdateForm1 As New RelatedTableUpdateForm

            RelatedTableUpdateForm1.ShowDialog()

        Catch ex As Exception

            Console.WriteLine( _
                "The application throws the error: " & ex.Message)

        Finally
            ' ///////////////////////////////////////////////////////////////
            ' Clean up objects before exit.
            ' 

            Console.WriteLine("Closing the connections ...")

            ' Close the connection to the database if it is open
            If (conn IsNot Nothing And conn.State = ConnectionState.Open) Then
                conn.Close()
            End If

        End Try

    End Sub

    ''' <summary>
    ''' Insert data into the Person table using SqlCommand
    ''' </summary>
    ''' <param name="conn">
    ''' The corresponding SqlConnection
    ''' </param>
    ''' 

    Private Sub InsertUsingSqlCommand(ByVal conn As SqlConnection)

        Console.WriteLine("Inserting a record to the Person table")

        ' 1. Create a command object
        Dim cmd As SqlCommand = New SqlCommand()

        ' 2. Assign the connection to the command
        cmd.Connection = conn

        ' 3. Set the command text
        ' SQL statement or the name of the stored procedure 
        cmd.CommandText = "INSERT INTO Person(LastName, FirstName, " & _
            "HireDate, EnrollmentDate, Picture) VALUES (@LastName, " & _
            "@FirstName, @HireDate, @EnrollmentDate, @Picture)"

        ' 4. Set the command type
        ' CommandType.Text for ordinary SQL statements; 
        ' CommandType.StoredProcedure for stored procedures.
        cmd.CommandType = CommandType.Text

        ' 5. Append the parameters
        ' We don't set the PersonCategory value so the database will 
        ' set its value to the default value 1.  
        cmd.Parameters.Add("@LastName", SqlDbType.NVarChar, 50).Value = _
            "Chen"
        cmd.Parameters.Add("@FirstName", SqlDbType.NVarChar, 50).Value = _
            "Yichun"
        cmd.Parameters.Add("@HireDate", SqlDbType.DateTime).Value = _
            DBNull.Value
        cmd.Parameters.Add("@EnrollmentDate", SqlDbType.DateTime).Value = _
            DateTime.Now


        ' Read the image file into an arrary of bytes
        Dim bImage As Byte() = ReadImage("MSDN.jpg")

        ' When sending a null value as a Parameter value in a 
        ' command to the database, you cannot use null. Instead 
        ' you need to use DBNull.Value
        cmd.Parameters.Add("@Picture", SqlDbType.Image).Value = _
            If(bImage Is Nothing, DBNull.Value, DirectCast(bImage, Object))

        ' 6. Execute the command
        cmd.ExecuteNonQuery()
    End Sub


    ''' <summary>
    ''' Select data from the Person table using Untyped DataSet
    ''' </summary>
    ''' <param name="conn">
    ''' The corresponding SqlConnection
    ''' </param>
    ''' 

    Private Sub SelectUsingUntypedDataSet(ByVal conn As SqlConnection)
        Console.WriteLine(vbCr & vbLf & "Selecting data from the Person " _
                          & "table using Untyped DataSet...")

        ' 1. Create a DataSet object
        Dim ds As New DataSet()

        ' 2. Create a SELECT SQL command
        Dim strSelectCmd As String = "SELECT * FROM Person"

        ' 3. Create a SqlDataAdapter object
        ' SqlDataAdapter represents a set of data commands and a 
        ' database connection that are used to fill the DataSet and 
        ' update a SQL Server database. 
        Dim da As New SqlDataAdapter(strSelectCmd, conn)

        ' 4. Fill the DataSet object
        ' Fill the DataTable named "Person" in DataSet with the rows
        ' selected by the SQL statement.
        da.Fill(ds, "Person")

        ' 5. Display each row of data in the "Person" data table 
        Console.WriteLine("Display certain columns of the Person table...")
        For Each row As DataRow In ds.Tables("Person").Rows
            ' When dumping SQL-Nullable field in the DataTable, test it
            ' for System.DBNull type.
            Console.WriteLine(row("PersonID") & vbTab & _
                              IIf(row("FirstName") Is DBNull.Value, "(DBNull)", row("FirstName")) & _
                              " " & _
                              IIf(row("LastName") Is DBNull.Value, "(DBNull)", row("LastName")))

        Next
    End Sub

    ''' <summary>
    ''' Insert and Select data using Strong Typed DataSet
    ''' </summary>
    Private Sub InsertSelectUsingStrongTypedDataSet()
        Console.WriteLine(vbCr & vbLf & "Insert and Select data using Strong Typed " & "DataSet...")

        ' 1. Create a Strong Typed DataSet object and fill its corresponding
        ' data tables
        ' Create a Strong Typed DataSet object
        Dim dsSQLServer As New SQLServer2005DBDataSet()

        ' Use the PersonTableAdapter to fill the Person table
        Dim taPerson As New PersonTableAdapter()
        Dim tblPerson As SQLServer2005DBDataSet.PersonDataTable = dsSQLServer.Person
        taPerson.Fill(tblPerson)

        ' Use the CourseTableAdapter to fill the Course table
        Dim taCourse As New CourseTableAdapter()
        Dim tblCourse As SQLServer2005DBDataSet.CourseDataTable = dsSQLServer.Course
        taCourse.Fill(tblCourse)

        ' Use the DepartmentTableAdapter to fill the Department table
        Dim taDepartment As New DepartmentTableAdapter()
        Dim tblDepartment As SQLServer2005DBDataSet.DepartmentDataTable = dsSQLServer.Department
        taDepartment.Fill(tblDepartment)


        ' 2. Insert a record into the Person table
        '--- (Strong Typed DataSet)
        ' We don't set the PersonCategory value because we have set the 
        ' default value property of the DataSet PersonCategory column.
        Dim addRowPerson As SQLServer2005DBDataSet.PersonRow = tblPerson.NewPersonRow()
        addRowPerson.LastName = "Ge"
        addRowPerson.FirstName = "Jialiang"
        addRowPerson.SetHireDateNull()
        addRowPerson.EnrollmentDate = DateTime.Now
        addRowPerson.Picture = ReadImage("MSDN.jpg")
        tblPerson.AddPersonRow(addRowPerson)

        ' ---Insert a record into the Person table
        ' ---(Untyped DataSet)
        'DataRow addRowPerson = tblPerson.NewRow();
        'addRowPerson["LastName"] = "Ge";
        'addRowPerson["FirstName"] = "Jialiang";
        'addRowPerson["HireDate"] = DBNull.Value;
        'addRowPerson["EnrollmentDate"] = DateTime.Now;
        'addRowPerson["Picture"] = ReadImage(@"MSDN.jpg");
        'tblPerson.Rows.Add(addRowPerson);

        ' Update the Person table
        taPerson.Update(tblPerson)

        ' 3. Insert a record into the Person table using PersionTableAdapter
        taPerson.Insert("Sun", "Hongye", DateTime.Now, Nothing, ReadImage("MSDN.jpg"), 2)

        ' 4. Find a certain record in the Person table by primary key
        ' ---(Strong Typed DataSet)
        Dim findRowPerson As SQLServer2005DBDataSet.PersonRow = tblPerson.FindByPersonID(1)

        ' ---Find a certain record in the Person table by primary key
        ' ---(Untyped DataSet)
        'DataRow findRowPerson = tblPerson.Rows.Find(1);

        ' Display the result record
        If findRowPerson IsNot Nothing Then
            ' Use IsColumnNameNull method to check the DBNull value
            Console.WriteLine(findRowPerson.PersonID & vbTab & findRowPerson.FirstName _
                              & " " & findRowPerson.LastName & vbTab & _
                              IIf(findRowPerson.IsEnrollmentDateNull(), "(DBNull)", _
                                  findRowPerson.EnrollmentDate.ToShortDateString()))
        End If

        ' 5. Display data in two related data tables
        For Each rowDepartment As SQLServer2005DBDataSet.DepartmentRow In dsSQLServer.Department
            Console.WriteLine(vbCr & vbLf & "Courses for Department( " _
                              & rowDepartment.DepartmentID & ")")
            For Each rowCourse As SQLServer2005DBDataSet.CourseRow In rowDepartment.GetCourseRows()
                Console.WriteLine(rowCourse.CourseID & " - " & rowCourse.Title)
            Next
        Next
    End Sub

    ''' <summary>
    ''' Read an image file to an array of bytes.
    ''' </summary>
    ''' <param name="path">
    ''' The path of the image file.
    ''' </param>
    ''' <returns>
    ''' The output of the array.
    ''' </returns>
    ''' 

    Private Function ReadImage(ByVal path As String) As Byte()
        Try
            ' Open the image file
            Using fs As New FileStream(path, FileMode.Open, FileAccess.Read)
                ' Create an array of bytes
                Dim bPicture As Byte() = New Byte(fs.Length - 1) {}

                ' Read the image file 
                fs.Read(bPicture, 0, Convert.ToInt32(fs.Length))

                Return (bPicture)
            End Using
        Catch ex As Exception
            Console.WriteLine("The application throws the error: " & ex.Message)
            Return (Nothing)
        End Try
    End Function

End Module
