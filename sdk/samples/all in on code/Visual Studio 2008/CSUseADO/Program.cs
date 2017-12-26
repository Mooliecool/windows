/************************************* Module Header **************************************\
* Module Name:  Program.cs
* Project:      CSUseADO
* Copyright (c) Microsoft Corporation.
* 
* The CSUseADO sample demonstrates the Microsoft ActiveX Data Objects(ADO) technology to 
* access databases using Visual C#. It shows the basic structure of connecting to a data 
* source, issuing SQL commands, using the Recordset object and performing the cleanup.  
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 2/26/2009 9:28 PM Rongchun Zhang Created
* * 3/2/2009 3:24 PM Jialiang Ge Reviewed
\******************************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Configuration;
#endregion


class Program
{
    static void Main(string[] args)
    {
        ADODB.Connection conn = null;
        ADODB.Recordset rs = null;

        try
        {
            ////////////////////////////////////////////////////////////////////////////////
            // Connect to the data source.
            // 

            Console.WriteLine("Connecting to the database ...");

            // Get the connection string from App.config. (The data source is created in the
            // sample SQLServer2005DB)
            string connStr = ConfigurationManager.ConnectionStrings["SQLServer2005DB"].
                ConnectionString;

            // Open the connection
            conn = new ADODB.Connection();
            conn.Open(connStr, null, null, 0);


            ////////////////////////////////////////////////////////////////////////////////
            // Build and Execute an ADO Command.
            // It can be a SQL statement (SELECT/UPDATE/INSERT/DELETE), or a stored 
            // procedure call. Here is the sample of an INSERT command.
            // 

            Console.WriteLine("Inserting a record to the Person table...");

            // 1. Create a command object
            ADODB.Command cmdInsert = new ADODB.Command();

            // 2. Assign the connection to the command
            cmdInsert.ActiveConnection = conn;

            // 3. Set the command text
            // SQL statement or the name of the stored procedure 
            cmdInsert.CommandText = "INSERT INTO Person(LastName, FirstName, EnrollmentDate, Picture)"
                + " VALUES (?, ?, ?, ?)";

            // 4. Set the command type
            // ADODB.CommandTypeEnum.adCmdText for oridinary SQL statements; 
            // ADODB.CommandTypeEnum.adCmdStoredProc for stored procedures.
            cmdInsert.CommandType = ADODB.CommandTypeEnum.adCmdText;

            // 5. Append the parameters

            // Append the parameter for LastName (nvarchar(50)
            ADODB.Parameter paramLN = cmdInsert.CreateParameter(
                "LastName",                                 // Parameter name
                ADODB.DataTypeEnum.adVarChar,               // Parameter type (nvarchar(50))
                ADODB.ParameterDirectionEnum.adParamInput,  // Parameter direction
                50,                                         // Max size of value in bytes
                "Zhang");                                   // Parameter value
            cmdInsert.Parameters.Append(paramLN);

            // Append the parameter for FirstName (nvarchar(50))
            ADODB.Parameter paramFN = cmdInsert.CreateParameter(
                "FirstName",                                // Parameter name
                ADODB.DataTypeEnum.adVarChar,               // Parameter type (nvarchar(50))
                ADODB.ParameterDirectionEnum.adParamInput,  // Parameter direction
                50,                                         // Max size of value in bytes
                "Rongchun");                                // Parameter value
            cmdInsert.Parameters.Append(paramFN);

            // Append the parameter for EnrollmentDate (datetime)
            ADODB.Parameter paramED = cmdInsert.CreateParameter(
                "EnrollmentDate",                           // Parameter name
                ADODB.DataTypeEnum.adDate,                  // Parameter type (datetime)
                ADODB.ParameterDirectionEnum.adParamInput,  // Parameter direction
                -1,                                         // Max size (ignored for datetime)
                DateTime.Now);                              // Parameter value
            cmdInsert.Parameters.Append(paramED);

            // Append the parameter for Picture (image)  

            // Read the image file into a safe array of bytes
            Byte[] bImage = ReadImage(@"MSDN.jpg");
            ADODB.Parameter paramImage = cmdInsert.CreateParameter(
                "Picture",                                  // Parameter name
                ADODB.DataTypeEnum.adLongVarBinary,         // Parameter type (Image)
                ADODB.ParameterDirectionEnum.adParamInput,  // Parameter direction
                bImage != null ? bImage.Length : 1,         // Max size of value in bytes
                bImage);                                    // Parameter value
            cmdInsert.Parameters.Append(paramImage);

            // 6. Execute the command
            object nRecordsAffected = Type.Missing;
            object oParams = Type.Missing;
            cmdInsert.Execute(out nRecordsAffected, ref oParams,
                (int)ADODB.ExecuteOptionEnum.adExecuteNoRecords);


            ////////////////////////////////////////////////////////////////////////////////
            // Use the Recordset Object.
            // http://msdn.microsoft.com/en-us/library/ms681510.aspx
            // Recordset represents the entire set of records from a base table or the  
            // results of an executed command. At any time, the Recordset object refers to  
            // only a single record within the set as the current record.
            // 

            Console.WriteLine("Enumerating the records in the Person table");

            // 1. Create a Recordset object
            rs = new ADODB.Recordset();

            // 2. Open the Recordset object
            string strSelectCmd = "SELECT * FROM Person"; // WHERE ...
            rs.Open(strSelectCmd,                       // SQL statement / table,view name / 
                                                        // stored procedure call / file name
                conn,                                   // Connection / connection string
                ADODB.CursorTypeEnum.adOpenForwardOnly, // Cursor type. (forward-only cursor)
                ADODB.LockTypeEnum.adLockOptimistic,	// Lock type. (locking records only 
                                                        // when you call the Update method.
                (int)ADODB.CommandTypeEnum.adCmdText);	// Evaluate the first parameter as
                                                        // a SQL command or stored procedure.

            // 3. Enumerate the records by moving the cursor forward
            rs.MoveFirst();  // Move to the first record in the Recordset
            while (!rs.EOF)
            {
                int nPersonId = (int)rs.Fields["PersonID"].Value;

                // When dumping a SQL-Nullable field in the table, need to test it for 
                // DBNull.Value.
                string strFirstName = (rs.Fields["FirstName"].Value == DBNull.Value) ?
                    "(DBNull)" : rs.Fields["FirstName"].Value.ToString();

                string strLastName = (rs.Fields["LastName"].Value == DBNull.Value) ? 
                    "(DBNull)" : rs.Fields["LastName"].Value.ToString();

                Console.WriteLine("{0}\t{1} {2}", nPersonId, strFirstName, strLastName);

                // Update the current record while enumerating the Recordset.
                //rs.Fields["XXXX"].Value = XXXX
                //rs.Update(); [-or-] rs.UpdateBatch(); outside the loop.

                rs.MoveNext();   // Move to the next record
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine("The application throws the error: {0}", ex.Message);
            if (ex.InnerException != null)
                Console.WriteLine("Description: {0}", ex.InnerException.Message);
        }
        finally
        {
            ////////////////////////////////////////////////////////////////////////////////
            // Clean up objects before exit.
            // 

            Console.WriteLine("Closing the connections ...");

            // Close the record set if it is open
            if (rs != null && rs.State == (int)ADODB.ObjectStateEnum.adStateOpen)
                rs.Close();

            // Close the connection to the database if it is open
            if (conn != null && conn.State == (int)ADODB.ObjectStateEnum.adStateOpen)
                conn.Close();
        }
    }

    /// <summary>
    /// Read an image file to an array of bytes.
    /// </summary>
    /// <param name="path">The path of the image file.</param>
    /// <returns>The output of the array.</returns>
    private static byte[] ReadImage(string path)
    {
        try
        {
            // Open the image file
            using (FileStream fs = new FileStream(path, FileMode.Open, FileAccess.Read))
            {
                // Create an array of bytes
                byte[] bPicture = new byte[fs.Length];

                // Read the image file 
                fs.Read(bPicture, 0, Convert.ToInt32(fs.Length));

                return bPicture;
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine("The application throws the error: {0}", ex.Message);
            return null;
        }
    }

}

