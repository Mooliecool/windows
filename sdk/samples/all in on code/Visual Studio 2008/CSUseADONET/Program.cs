/****************************** Module Header ******************************\
* Module Name:	Program.cs
* Project:		CSUseADONET
* Copyright (c) Microsoft Corporation.
* 
* The CSUseADONET example demonstrates the Microsoft ADO.NET technology to 
* access databases using Visual C#. It shows the basic structure of 
* connecting to a data source, issuing SQL commands, using Untyped DataSet,
* using Strong Typed DataSet, updating related data tables and performing the 
* cleanup. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 2/25/2009 09:00 PM Lingzhi Sun Created
* * 3/27/2009 09:00 AM Lingzhi Sun Edited
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;
using System.Configuration;
using System.IO;
using System.Xml;
using CSUseADONET;
using CSUseADONET.SQLServer2005DBDataSetTableAdapters;
using System.Windows.Forms;
#endregion


class Program
{
    static void Main(string[] args)
    {
        // Create a SqlConnection object
        SqlConnection conn = null;

        try
        {
            /////////////////////////////////////////////////////////////////
            // Connect to the data source.
            // 

            Console.WriteLine("Connecting to the database ...");

            // Get the connection string from App.config. (The data source is
            // created in the sample SQLServer2005DB)
            string connStr = ConfigurationManager.ConnectionStrings[
                "SQLServer2005DB"].ConnectionString;

            // Open the connection
            conn = new SqlConnection(connStr);
            conn.Open();


            /////////////////////////////////////////////////////////////////
            // Build and Execute an ADO.NET Command.
            //
            // It can be a SQL statement (SELECT/UPDATE/INSERT/DELETE), or a  
            // stored procedure call. Here is the sample of an INSERT command.
            // 

            InsertUsingSqlCommand(conn);


            /////////////////////////////////////////////////////////////////
            // Use the Untyped DataSet Object.
            //
            // The DataSet, which is an in-memory cache of data retrieved 
            // from a data source, is a major component of the ADO.NET 
            // architecture. The DataSet consists of a collection of 
            // DataTable objects that you can relate to each other with 
            // DataRelation objects.
            // 

            SelectUsingUntypedDataSet(conn);


            /////////////////////////////////////////////////////////////////
            // Use the Strong Typed DataSet Object.
            //
            // A typed DataSet is a class that derives from a DataSet. As 
            // such, it inherits all the methods, events, and properties of a 
            // DataSet. Additionally, a typed DataSet provides strongly typed 
            // methods, events, and properties. This means you can access 
            // tables and columns by name, instead of using collection-based 
            // methods. Aside from the improved readability of the code, a 
            // typed DataSet also allows the Visual Studio .NET code editor 
            // to automatically complete lines as you type.
            //  

            InsertSelectUsingStrongTypedDataSet();


            /////////////////////////////////////////////////////////////////
            // Update Two Related Data Tables by TableAdapterManager and
            // writting codes manually.
            //
            // When you need to save data from a dataset that contains two or 
            // more related data tables, you must send the changes to the 
            // database in a specific order so that constraints are not 
            // violated. When you update modified data in related tables, you 
            // can provide the programmatic logic to extract the specific 
            // subsets of data from each data table and send the updates to 
            // the database in the correct order, or you can use the 
            // TableAdapterManager component introduced in Visual Studio 2008. 

            Application.Run(new RelatedTableUpdateForm());

        }
        catch (Exception ex)
        {
            Console.WriteLine(
                "The application throws the error: {0}", ex.Message);
        }
        finally
        {
            /////////////////////////////////////////////////////////////////
            // Clean up objects before exit.
            // 

            Console.WriteLine("Closing the connections ...");

            // Close the connection to the database if it is open
            if (conn != null && conn.State == ConnectionState.Open)
                conn.Close();
        }
    }


    /// <summary>
    /// Insert data into the Person table using SqlCommand
    /// </summary>
    /// <param name="conn">
    /// The corresponding SqlConnection
    /// </param>
    private static void InsertUsingSqlCommand(SqlConnection conn)
    {
        Console.WriteLine("Inserting a record to the Person table");

        // 1. Create a command object
        SqlCommand cmd = new SqlCommand();

        // 2. Assign the connection to the command
        cmd.Connection = conn;

        // 3. Set the command text
        // SQL statement or the name of the stored procedure 
        cmd.CommandText = "INSERT INTO Person(LastName, FirstName, " +
            "HireDate, EnrollmentDate, Picture) VALUES (@LastName, " +
            "@FirstName, @HireDate, @EnrollmentDate, @Picture)";

        // 4. Set the command type
        // CommandType.Text for ordinary SQL statements; 
        // CommandType.StoredProcedure for stored procedures.
        cmd.CommandType = CommandType.Text;

        // 5. Append the parameters
        // We don't set the PersonCategory value so the database will 
        // set its value to the default value 1.  
        cmd.Parameters.Add("@LastName", SqlDbType.NVarChar, 50).Value = 
            "Sun";
        cmd.Parameters.Add("@FirstName", SqlDbType.NVarChar, 50).Value = 
            "Lingzhi";
        cmd.Parameters.Add("@HireDate", SqlDbType.DateTime).Value = 
            DBNull.Value;
        cmd.Parameters.Add("@EnrollmentDate", SqlDbType.DateTime).Value = 
            DateTime.Now;

        // Read the image file into an arrary of bytes
        byte[] bImage = ReadImage(@"MSDN.jpg");

        // When sending a null value as a Parameter value in a 
        // command to the database, you cannot use null. Instead 
        // you need to use DBNull.Value
        cmd.Parameters.Add("@Picture", SqlDbType.Image).Value =
            bImage == null ? DBNull.Value : (object)bImage;

        // 6. Execute the command
        cmd.ExecuteNonQuery();
    }


    /// <summary>
    /// Select data from the Person table using Untyped DataSet
    /// </summary>
    /// <param name="conn">
    /// The corresponding SqlConnection
    /// </param>
    private static void SelectUsingUntypedDataSet(SqlConnection conn)
    {
        Console.WriteLine("\r\nSelecting data from the Person "
          + "table using Untyped DataSet...");

        // 1. Create a DataSet object
        DataSet ds = new DataSet();

        // 2. Create a SELECT SQL command
        string strSelectCmd = "SELECT * FROM Person";

        // 3. Create a SqlDataAdapter object
        // SqlDataAdapter represents a set of data commands and a 
        // database connection that are used to fill the DataSet and 
        // update a SQL Server database. 
        SqlDataAdapter da = new SqlDataAdapter(strSelectCmd, conn);

        // 4. Fill the DataSet object
        // Fill the DataTable named "Person" in DataSet with the rows
        // selected by the SQL statement.
        da.Fill(ds, "Person");

        // 5. Display each row of data in the "Person" data table 
        Console.WriteLine("Display certain columns of the Person table...");
        foreach (DataRow row in ds.Tables["Person"].Rows)
        {
            // When dumping SQL-Nullable field in the DataTable, test it
            // for System.DBNull type.
            Console.WriteLine("{0}\t{1} {2}", row["PersonID"],
                row["FirstName"] == DBNull.Value ? "(DBNull)" : 
                row["FirstName"], row["LastName"] == DBNull.Value ? 
                "(DBNull)" : row["LastName"]);
        }
    }


    /// <summary>
    /// Insert and Select data using Strong Typed DataSet
    /// </summary>
    private static void InsertSelectUsingStrongTypedDataSet()
    {
        Console.WriteLine("\r\nInsert and Select data using Strong Typed "
            + "DataSet...");

        // 1. Create a Strong Typed DataSet object and fill its corresponding
        //    data tables
        // Create a Strong Typed DataSet object
        SQLServer2005DBDataSet dsSQLServer = new SQLServer2005DBDataSet();

        // Use the PersonTableAdapter to fill the Person table
        PersonTableAdapter taPerson = new PersonTableAdapter();
        SQLServer2005DBDataSet.PersonDataTable tblPerson = dsSQLServer.
            Person;
        taPerson.Fill(tblPerson);

        // Use the CourseTableAdapter to fill the Course table
        CourseTableAdapter taCourse = new CourseTableAdapter();
        SQLServer2005DBDataSet.CourseDataTable tblCourse = dsSQLServer.
            Course;
        taCourse.Fill(tblCourse);

        // Use the DepartmentTableAdapter to fill the Department table
        DepartmentTableAdapter taDepartment = new DepartmentTableAdapter();
        SQLServer2005DBDataSet.DepartmentDataTable tblDepartment = 
            dsSQLServer.Department;
        taDepartment.Fill(tblDepartment);


        // 2. Insert a record into the Person table
        //--- (Strong Typed DataSet)
        // We don't set the PersonCategory value because we have set the 
        // default value property of the DataSet PersonCategory column.
        SQLServer2005DBDataSet.PersonRow addRowPerson = tblPerson.
            NewPersonRow();
        addRowPerson.LastName = "Ge";
        addRowPerson.FirstName = "Jialiang";
        addRowPerson.SetHireDateNull();
        addRowPerson.EnrollmentDate = DateTime.Now;
        addRowPerson.Picture = ReadImage(@"MSDN.jpg");
        tblPerson.AddPersonRow(addRowPerson);

        // ---Insert a record into the Person table
        // ---(Untyped DataSet)
        //DataRow addRowPerson = tblPerson.NewRow();
        //addRowPerson["LastName"] = "Ge";
        //addRowPerson["FirstName"] = "Jialiang";
        //addRowPerson["HireDate"] = DBNull.Value;
        //addRowPerson["EnrollmentDate"] = DateTime.Now;
        //addRowPerson["Picture"] = ReadImage(@"MSDN.jpg");
        //tblPerson.Rows.Add(addRowPerson);

        // Update the Person table
        taPerson.Update(tblPerson);

        // 3. Insert a record into the Person table using PersionTableAdapter
        taPerson.Insert("Sun", "Hongye", DateTime.Now, null,
            ReadImage(@"MSDN.jpg"), 2);

        // 4. Find a certain record in the Person table by primary key
        // ---(Strong Typed DataSet)
        SQLServer2005DBDataSet.PersonRow findRowPerson = tblPerson.
            FindByPersonID(1);

        // ---Find a certain record in the Person table by primary key
        // ---(Untyped DataSet)
        //DataRow findRowPerson = tblPerson.Rows.Find(1);

        // Display the result record
        if (findRowPerson != null)
        {
            // Use IsColumnNameNull method to check the DBNull value
            Console.WriteLine("{0}\t{1} {2}\t{3}", findRowPerson.PersonID,
                findRowPerson.FirstName, findRowPerson.LastName,
                findRowPerson.IsEnrollmentDateNull() ? "(DBNull)" :
                findRowPerson.EnrollmentDate.ToShortDateString());
        }

        // 5. Display data in two related data tables
        foreach (SQLServer2005DBDataSet.DepartmentRow rowDepartment in 
            dsSQLServer.Department)
        {
            Console.WriteLine("\r\nCourses for Department({0})", 
                rowDepartment.DepartmentID);
            foreach (SQLServer2005DBDataSet.CourseRow rowCourse
                in rowDepartment.GetCourseRows())
            {
                Console.WriteLine("{0} - {1}", rowCourse.CourseID,
                    rowCourse.Title);
            }
        }
    }


    /// <summary>
    /// Read an image file to an array of bytes.
    /// </summary>
    /// <param name="path">
    /// The path of the image file.
    /// </param>
    /// <returns>
    /// The output of the array.
    /// </returns>
    private static byte[] ReadImage(string path)
    {
        try
        {
            // Open the image file
            using (FileStream fs = new FileStream(path, FileMode.Open,
                FileAccess.Read))
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
            Console.WriteLine(
                "The application throws the error: {0}", ex.Message);
            return null;
        }
    }
}
