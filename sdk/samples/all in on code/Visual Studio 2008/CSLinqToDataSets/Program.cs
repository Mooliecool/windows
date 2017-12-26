/****************************** Module Header ******************************\
* Module Name:    Program.cs
* Project:        CSLinqToDataSets
* Copyright (c) Microsoft Corporation.
*
* The CSLinqToDataSets sample demonstrates the Microsoft Language-Integrated 
* Query (LINQ) technology to access untyped DataSet and strong typed DataSet 
* using Visual C#. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* History:
* * 4/01/2009 10:30 PM Lingzhi Sun Created
\***************************************************************************/

#region Using directives
using System;
using System.IO;
using System.Linq;
using System.Data;
using System.Data.SqlClient;
using CSLinqToDataSets;
using System.Configuration;
using CSLinqToDataSets.SQLServer2005DBDataSetTableAdapters;
#endregion


class Program
{
    static void Main(string[] args)
    {
        try
        {
            // Query the data in untyped DataSet
            QueryDBByUntypedDataSet();

            Console.WriteLine("\n======================================\n");

            // Query the data in strong typed DataSet
            QueryDBByStrongTypedDataSet();

            Console.Read();
        }
        catch (Exception e)
        {
            Console.WriteLine(
                "The application throws the error: {0}", e.Message);
            Console.Read();
        }
    }


    /// <summary>
    /// This method works with the untyped DataSet
    /// </summary>
    private static void QueryDBByUntypedDataSet()
    {
        Console.WriteLine("Use LINQ to query untyped DataSet...");

        /////////////////////////////////////////////////////////////////////
        // Fill the untyped DataSet && Insert data into database
        // 

        Console.WriteLine("\nFill the DataTables and Insert a new record "
            + "into the Person DataTable...");

        // Get the connection string from App.config. (The data source is
        // created in the example SQLServer2005DB)
        string connStr = ConfigurationManager.ConnectionStrings[
                "SQLServer2005DB"].ConnectionString;

        // Create SqlDataAdapter object and fill the Person DataTable
        string selectCmd = "SELECT * FROM Person";
        SqlDataAdapter da = new SqlDataAdapter(selectCmd, connStr);
        DataSet ds = new DataSet();
        da.Fill(ds, "Person");

        // Create a new row for the Person DataTable
        // We don't set the PersonCategory column value so the
        // database will set the default value to this column
        DataRow rowPerson = ds.Tables["Person"].NewRow();
        rowPerson["FirstName"] = "Lingzhi";
        rowPerson["LastName"] = "Sun";
        rowPerson["HireDate"] = Convert.DBNull;
        rowPerson["EnrollmentDate"] = DateTime.Now;
        rowPerson["Picture"] = ReadImage(@"MSDN.jpg");
        ds.Tables["Person"].Rows.Add(rowPerson);

        // Create corresponding update/insert/delete SQL command
        SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(da);

        // Update the changes in the Person DataTable to the database
        da.Update(ds, "Person");

        // Fill the Course DataTable
        da.SelectCommand.CommandText = "SELECT * FROM Course";
        da.Fill(ds, "Course");

        // Fill the CourseGrade DataTable
        da.SelectCommand.CommandText = "SELECT * FROM CourseGrade";
        da.Fill(ds, "CourseGrade");


        /////////////////////////////////////////////////////////////////////
        // Perform the query operation in one DataTable
        // 

        Console.WriteLine("\nQuery people whose first name is Roger:");

        // Perform the query
        var query = from p in ds.Tables["Person"].AsEnumerable()
                    where p.Field<string>("FirstName") == "Roger"
                    select p;

        // Display the query results
        foreach (var p in query)
        {
            Console.WriteLine("ID = {0}, Name = {1} {2}", 
                p.Field<int>("PersonID"), p.Field<string>("FirstName"), 
                p.Field<string>("LastName"));
        }


        /////////////////////////////////////////////////////////////////////
        // Perform the query operation across multiple DataTables
        // 

        Console.WriteLine("\nQuery the max grade of each course:");

        // Perform the query and get a collection of hte anonymous type,
        // new { int CourseID, string Title, decimal TopGrade }
        var courses = from grade in ds.Tables["CourseGrade"].AsEnumerable()
                      group grade by grade.Field<int>("CourseID") into g
                      join cur in ds.Tables["Course"].AsEnumerable()
                      on g.Key equals cur.Field<int>("CourseID")
                      select new
                      {
                          CourseID = g.Key,
                          Title = cur.Field<string>("Title"),
                          TopGrade = g.Max(gra => gra.IsNull("Grade") ? 
                              decimal.Zero : gra.Field<decimal>("Grade"))
                      };

        // Display the query results
        foreach (var c in courses)
        {
            Console.WriteLine("Course = {0}, TopGrade = {1}", c.Title, 
                c.TopGrade);
        }
    }


    /// <summary>
    /// This method works with the strong typed DataSet
    /// </summary>
    private static void QueryDBByStrongTypedDataSet()
    {
        Console.WriteLine("Use LINQ to query strong typed DataSet...");

        /////////////////////////////////////////////////////////////////////
        // Fill the strong typed DataSet && Insert data into database
        // 

        // Create a strong typed DataSet object
        SQLServer2005DBDataSet dsSQLServer = new SQLServer2005DBDataSet();

        // Create the PersonTableAdapter and fill the Person DataTable
        PersonTableAdapter taPerson = new PersonTableAdapter();
        SQLServer2005DBDataSet.PersonDataTable tblPerson = dsSQLServer.
            Person;
        taPerson.Fill(tblPerson);

        // Create a new row into the Person DataTable
        // We don't set the PersonCategory value because we have set the 
        // default value property of the DataSet PersonCategory column.
        SQLServer2005DBDataSet.PersonRow rowPerson = tblPerson.NewPersonRow();
        rowPerson.LastName = "Sun";
        rowPerson.FirstName = "Hongye";
        rowPerson.SetHireDateNull();
        rowPerson.EnrollmentDate = DateTime.Now;
        rowPerson.Picture = ReadImage(@"MSDN.jpg");
        tblPerson.AddPersonRow(rowPerson);

        // Update the database
        taPerson.Update(tblPerson);

        // Use PersonTableAdapter to insert new record into the database 
        // directly
        taPerson.Insert("Ge", "Jialiang", DateTime.Now, null,
            ReadImage(@"MSDN.jpg"), 2);

        // Create the CourseTableAdapter and fill the Course DataTable
        CourseTableAdapter taCourse = new CourseTableAdapter();
        SQLServer2005DBDataSet.CourseDataTable tblCourse = dsSQLServer.Course;
        taCourse.Fill(tblCourse);

        // Create the CourseGradeTableAdapter and fill the CourseGrade 
        // DataTable
        CourseGradeTableAdapter taCourseGrade = new CourseGradeTableAdapter();
        SQLServer2005DBDataSet.CourseGradeDataTable tblCourseGrade = 
            dsSQLServer.CourseGrade;
        taCourseGrade.Fill(tblCourseGrade);


        /////////////////////////////////////////////////////////////////////
        // Perform the query operation in one DataTable
        // 

        Console.WriteLine("\nQuery people whose first name is Roger:");

        // Person the query
        var query = from p in tblPerson.AsEnumerable()
                    where p.FirstName == "Roger"
                    select p;

        // Display the query results
        foreach (var p in query)
        {
            Console.WriteLine("ID = {0}, Name = {1} {2}", p.PersonID,
                p.FirstName, p.LastName);
        }


        /////////////////////////////////////////////////////////////////////
        // Perform the query operation across multiple DataTables
        // 

        Console.WriteLine("\nQuery the max grade of each course:");

        // Perform the query and get a collection of hte anonymous type,
        // new { int CourseID, string Title, decimal TopGrade }
        var courses = from grade in tblCourseGrade.AsEnumerable()
                      group grade by grade.CourseID into g
                      join cur in tblCourse.AsEnumerable()
                      on g.Key equals cur.CourseID
                      select new
                      {
                          CourseID = g.Key,
                          Title = cur.Title,
                          TopGrade = g.Max(gra => gra.IsGradeNull() ? 
                              decimal.Zero : gra.Grade)
                      };

        // Display the query results
        foreach (var c in courses)
        {
            Console.WriteLine("Course = {0}, TopGrade = {1}", c.Title, 
                c.TopGrade);
        }


        /////////////////////////////////////////////////////////////////////
        // Perform the query operation across multiple related DataTables
        // 

        Console.WriteLine("\nQuery all grades that Nino got:");

        // Perform the query betweem related DataTables and get the anonymous
        // type, new { PersonRow Person, CourseRow Course, CourseGradeRow
        // CourseGrade}
        var grades = from per in tblPerson.AsEnumerable()
                     from gra in per.GetCourseGradeRows()
                     join cur in tblCourse.AsEnumerable()
                     on gra.CourseID equals cur.CourseID
                     where per.FirstName == "Nino"
                     select new
                     {
                         Person = per,
                         Course = cur,
                         CourseGrade = gra
                     };

        // Display the query results
        foreach (var grade in grades)
        {
            Console.WriteLine("FirstName = {0}, Course= {1}, Grade= {2}",
                grade.Person.FirstName, grade.Course.Title,
                grade.CourseGrade.Grade);
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