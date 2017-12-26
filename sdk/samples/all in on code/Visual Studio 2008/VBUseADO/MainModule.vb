'************************************ Module Header **************************************\
' Module Name:  Program.cs
' Project:      CSUseADO
' Copyright (c) Microsoft Corporation.
' 
' The CSUseADO sample demonstrates the Microsoft ActiveX Data Objects(ADO) technology to 
' access databases using Visual C#. It shows the basic structure of connecting to a data 
' source, issuing SQL commands, using the Recordset object and performing the cleanup.  
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 2/26/2010 7:57 PM Yichun Chen Created
'******************************************************************************************/

#Region "Imports directives"
Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.IO
Imports System.Configuration
#End Region


Module MainModule

    Sub Main()

        Dim conn As ADODB.Connection = Nothing
        Dim rs As ADODB.Recordset = Nothing

        Try
            '///////////////////////////////////////////////////////////////////////////////
            ' Connect to the data source.
            ' 

            Console.WriteLine("Connecting to the database ...")

            ' Get the connection string from App.config. (The data source is created in the
            ' sample SQLServer2005DB)
            Dim connStr As String = ConfigurationManager.ConnectionStrings("SQLServer2005DB").ConnectionString

            ' Open the connection
            conn = New ADODB.Connection()
            conn.Open(connStr, Nothing, Nothing, 0)

            '///////////////////////////////////////////////////////////////////////////////
            ' Build and Execute an ADO Command.
            ' It can be a SQL statement (SELECT/UPDATE/INSERT/DELETE), or a stored 
            ' procedure call. Here is the sample of an INSERT command.
            ' 

            Console.WriteLine("Inserting a record to the Person table...")

            ' 1. Create a command object
            Dim cmdInsert As New ADODB.Command()

            ' 2. Assign the connection to the command
            cmdInsert.ActiveConnection = conn

            ' 3. Set the command text
            ' SQL statement or the name of the stored procedure 
            cmdInsert.CommandText = "INSERT INTO Person(LastName, FirstName, EnrollmentDate, Picture)" & " VALUES (?, ?, ?, ?)"

            ' 4. Set the command type
            ' ADODB.CommandTypeEnum.adCmdText for oridinary SQL statements; 
            ' ADODB.CommandTypeEnum.adCmdStoredProc for stored procedures.
            cmdInsert.CommandType = ADODB.CommandTypeEnum.adCmdText

            ' 5. Append the parameters

            ' Append the parameter for LastName (nvarchar(50)
            ' Parameter name
            ' Parameter type (nvarchar(50))
            ' Parameter direction
            ' Max size of value in bytes
            Dim paramLN As ADODB.Parameter = cmdInsert.CreateParameter("LastName", ADODB.DataTypeEnum.adVarChar, ADODB.ParameterDirectionEnum.adParamInput, 50, "Zhang")
            ' Parameter value
            cmdInsert.Parameters.Append(paramLN)

            ' Append the parameter for FirstName (nvarchar(50))
            ' Parameter name
            ' Parameter type (nvarchar(50))
            ' Parameter direction
            ' Max size of value in bytes
            Dim paramFN As ADODB.Parameter = cmdInsert.CreateParameter("FirstName", ADODB.DataTypeEnum.adVarChar, ADODB.ParameterDirectionEnum.adParamInput, 50, "Rongchun")
            ' Parameter value
            cmdInsert.Parameters.Append(paramFN)

            ' Append the parameter for EnrollmentDate (datetime)
            ' Parameter name
            ' Parameter type (datetime)
            ' Parameter direction
            ' Max size (ignored for datetime)
            Dim paramED As ADODB.Parameter = cmdInsert.CreateParameter("EnrollmentDate", ADODB.DataTypeEnum.adDate, ADODB.ParameterDirectionEnum.adParamInput, -1, DateTime.Now)
            ' Parameter value
            cmdInsert.Parameters.Append(paramED)

            ' Append the parameter for Picture (image) 

            ' Read the image file into a safe array of bytes
            Dim bImage As [Byte]() = ReadImage("MSDN.jpg")
            ' Parameter name
            ' Parameter type (Image)
            ' Parameter direction
            ' Max size of value in bytes
            Dim paramImage As ADODB.Parameter = cmdInsert.CreateParameter("Picture", ADODB.DataTypeEnum.adLongVarBinary, ADODB.ParameterDirectionEnum.adParamInput, If(bImage IsNot Nothing, bImage.Length, 1), bImage)
            ' Parameter value
            cmdInsert.Parameters.Append(paramImage)

            ' 6. Execute the command
            Dim nRecordsAffected As Object = Type.Missing
            Dim oParams As Object = Type.Missing
            cmdInsert.Execute(nRecordsAffected, oParams, CInt(ADODB.ExecuteOptionEnum.adExecuteNoRecords))


            '///////////////////////////////////////////////////////////////////////////////
            ' Use the Recordset Object.
            ' http://msdn.microsoft.com/en-us/library/ms681510.aspx
            ' Recordset represents the entire set of records from a base table or the 
            ' results of an executed command. At any time, the Recordset object refers to 
            ' only a single record within the set as the current record.
            ' 

            Console.WriteLine("Enumerating the records in the Person table")

            ' 1. Create a Recordset object
            rs = New ADODB.Recordset()

            ' 2. Open the Recordset object
            Dim strSelectCmd As String = "SELECT * FROM Person" ' WHERE ...

            ' SQL statement / table,view name /
            ' stored procedure call / file name
            ' Connection / connection string
            ' Cursor type. (forward-only cursor)
            ' Lock type. (locking records only 
            ' when you call the Update method.
            ' Evaluate the first parameter as
            ' a SQL command or stored procedure.

            rs.Open(strSelectCmd, _
                    conn, _
                    ADODB.CursorTypeEnum.adOpenForwardOnly, _
                    ADODB.LockTypeEnum.adLockOptimistic, _
                    CInt(ADODB.CommandTypeEnum.adCmdText))

            ' 3. Enumerate the records by moving the cursor forward
            rs.MoveFirst()
            ' Move to the first record in the Recordset
            While (Not rs.EOF)
                Dim nPersonId As Integer = CInt(rs.Fields("PersonID").Value)

                ' When dumping a SQL-Nullable field in the table, need to test it for 
                ' DBNull.Value.
                Dim strFirstName As String = If((rs.Fields("FirstName").Value Is DBNull.Value), "(DBNull)", rs.Fields("FirstName").Value.ToString())

                Dim strLastName As String = If((rs.Fields("LastName").Value Is DBNull.Value), "(DBNull)", rs.Fields("LastName").Value.ToString())

                Console.WriteLine(nPersonId & vbTab & strFirstName & " " & strLastName)

                ' Update the current record while enumerating the Recordset.
                'rs.Fields["XXXX"].Value = XXXX
                'rs.Update(); [-or-] rs.UpdateBatch(); outside the loop.

                ' Move to the next record
                rs.MoveNext()
            End While

        Catch ex As Exception

            Console.WriteLine("The application throws the error: " & ex.Message)
            If ex.InnerException IsNot Nothing Then
                Console.WriteLine("Description: " & ex.InnerException.Message)
            End If

        Finally

            '///////////////////////////////////////////////////////////////////////////////
            ' Clean up objects before exit.
            ' 

            Console.WriteLine("Closing the connections ...")

            ' Close the record set if it is open
            If rs IsNot Nothing AndAlso rs.State = CInt(ADODB.ObjectStateEnum.adStateOpen) Then
                rs.Close()
            End If

            ' Close the connection to the database if it is open
            If conn IsNot Nothing AndAlso conn.State = CInt(ADODB.ObjectStateEnum.adStateOpen) Then
                conn.Close()
            End If

        End Try

    End Sub

    ''' <summary>
    ''' Read an image file to an array of bytes.
    ''' </summary>
    ''' <param name="path">The path of the image file.</param>
    ''' <returns>The output of the array.</returns>
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
