/****************************** Module Header ******************************\
* Module Name:    Program.cs
* Project:        CSLinqToSQL
* Copyright (c) Microsoft Corporation.
*
* The CSLinqToSQL sample demonstrates the Microsoft Language-Integrated Query 
* (LINQ) technology to access databases using Visual C#. It shows the basic 
* structure of connecting and querying the data source and inserting data  
* into the database with LINQ.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* History:
* * 3/08/2009 9:28 PM Rongchun Zhang Created
* * 3/27/2009 5:40 PM Jialiang Ge Reviewed
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.Linq.Mapping;
using System.Data.Linq;
using System.Configuration;
using System.IO;
#endregion


class Program
{
    static void Main(string[] args)
    {
        // Query the database using manually-created data classes
        QueryDBByManuallyCreatedClasses();

        Console.WriteLine();

        // Query the database using designer-generated data classes
        QueryDBByDesignerGeneratedClasses();
    }

    /// <summary>
    /// The following method works with the manually-created classes
    /// </summary>
    private static void QueryDBByManuallyCreatedClasses()
    {
        Console.WriteLine("Query using the manually-created classes.");

        try
        {
            /////////////////////////////////////////////////////////////////
            // Construct the data context.
            // 

            // Get the connection string from App.config. (The data source is
            // created in the example SQLServer2005DB)
            string connStr = ConfigurationManager.ConnectionStrings[
                "SQLServer2005DB"].ConnectionString;

            // DataContext takes a connection string
            CSLinqToSQL.Manual.SchoolDataContext db = new 
                CSLinqToSQL.Manual.SchoolDataContext(connStr);


            /////////////////////////////////////////////////////////////////
            // Perform the Insert operation.
            // 

            Console.WriteLine("\nAdd a new student to the Person table");

            // Create a new Student.
            // We don't need to set the PersonCategory value here if we want
            // the default value to be set to this column in the database.
            CSLinqToSQL.Manual.Student student = new CSLinqToSQL.Manual.Student
            {
                FirstName = "Rongchun",
                LastName = "Zhang",
                EnrollmentDate = DateTime.Now,
                Picture = ReadImage(@"MSDN.jpg"),
            };

            // Add the new object to Students.
            db.Students.InsertOnSubmit(student);

            // Submit the change to the database.
            db.SubmitChanges();


            /////////////////////////////////////////////////////////////////
            // Perform the Query operation.
            // 

            // Query in one table.

            Console.WriteLine("\nQuery students whose first name is Roger:");

            var students = from p in db.Students
                           where p.FirstName == "Roger"
                           select p;

            foreach (var p in students)
                Console.WriteLine("ID = {0}, Name = {1} {2}", p.PersonID,
                    p.FirstName, p.LastName);

            // Query across multiple tales.

            Console.WriteLine("\nQuery max. grade of each course:");

            var courses =
                from grade in db.CourseGrades
                group grade by grade.CourseID into g
                join cur in db.Courses on g.Key equals cur.CourseID
                select new
                {
                    CourseID = g.Key,
                    Title = cur.Title,
                    TopGrade = g.Max(gra => gra.Grade)
                };

            foreach (var c in courses)
                Console.WriteLine("Course = {0}, TopGrade = {1}", 
                    c.Title, c.TopGrade);

            Console.WriteLine("\nQuery all grades that Nino got");

            var grades =
               from stu in db.Students
               from gra in stu.CourseGrades
               join cur in db.Courses on gra.CourseID equals cur.CourseID
               where stu.FirstName == "Nino"
               select new
               {
                   Stduent = stu,
                   Course = cur,
                   CourseGrade = gra
               };

            foreach (var grade in grades)
            {
                Console.WriteLine("FirstName = {0}, Course= {1}, Grade= {2}",
                    grade.Stduent.FirstName,
                    grade.Course.Title,
                    grade.CourseGrade.Grade);
            }
        }
        catch (Exception e)
        {
            Console.WriteLine("The application throws the error: {0}", e.Message);
        }
    }

    /// <summary>
    /// The following method works with the designer-generated classes
    /// </summary>
    private static void QueryDBByDesignerGeneratedClasses()
    {
        Console.WriteLine("Query with the designer-generated classes.");

        try
        {
            /////////////////////////////////////////////////////////////////
            // Construct the data context.
            // 

            // Create DataContext for School database
            CSLinqToSQL.Designer.SchoolDataContext db =
                new CSLinqToSQL.Designer.SchoolDataContext();


            /////////////////////////////////////////////////////////////////
            // Perform the Insert operation.
            // 

            Console.WriteLine("\nAdd a new student to the Person table");

            // Create a new Student.
            CSLinqToSQL.Designer.Student std = new CSLinqToSQL.Designer.Student
            {
                FirstName = "Rongchun",
                LastName = "Zhang",
                PersonCategory = 1,
                EnrollmentDate = DateTime.Now
            };
            byte[] bImage = ReadImage(@"MSDN.jpg");
            if (bImage != null)
                std.Picture = bImage;

            // Add the new object to the Students.
            db.Students.InsertOnSubmit(std);

            // Submit the change to the database.
            db.SubmitChanges();


            /////////////////////////////////////////////////////////////////
            // Perform the Query operation.
            // 

            // Query in one table.

            Console.WriteLine("\nQuery students whose first name is Roger:");

            var students = from p in db.Students
                           where p.FirstName == "Roger"
                           select p;

            foreach (var p in students)
                Console.WriteLine("ID = {0}, Name = {1} {2}", p.PersonID, 
                    p.FirstName, p.LastName);

            // Query across multiple tales.

            Console.WriteLine("\nQuery max. grade of each course:");

            var courses =
                from grade in db.CourseGrades
                group grade by grade.CourseID into g
                join cur in db.Courses on g.Key equals cur.CourseID
                select new
                {
                    CourseID = g.Key,
                    Title = cur.Title,
                    TopGrade = g.Max(gra => gra.Grade)
                };

            foreach (var c in courses)
                Console.WriteLine("Course = {0}, TopGrade = {1}",
                    c.Title, c.TopGrade);


            Console.WriteLine("\nQuery all grades that Nino got");

            var grades =
               from stu in db.Students
               from gra in stu.CourseGrades
               join cur in db.Courses on gra.CourseID equals cur.CourseID
               where stu.FirstName == "Nino"
               select new 
               { 
                   Stduent = stu, 
                   Course = cur,
                   CourseGrade = gra 
               };

            foreach (var grade in grades)
            {
                Console.WriteLine("FirstName = {0}, Course= {1}, Grade= {2}",
                    grade.Stduent.FirstName,
                    grade.Course.Title,
                    grade.CourseGrade.Grade);
            }

            Console.Read();
        }
        catch (Exception e)
        {
            Console.WriteLine("The application throws the error: {0}", e.Message);
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
