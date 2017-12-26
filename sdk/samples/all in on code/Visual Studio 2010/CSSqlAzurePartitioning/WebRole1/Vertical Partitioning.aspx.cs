/****************************** Module Header ******************************\
* Module Name:  Vertical Partitioning.aspx.cs
* Project: CSSqlAzurePartitioning
* Copyright (c) Microsoft Corporation.
* 
* If you want to store larger amounts of data in SQL Azure you can divide your tables 
* across multiple SQL Azure databases(Vertically Partitions your Data). 
*
* This sample shows how to join two tables on different SQL Azure databases using LINQ. 
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Configuration;
using SQLAzure;
using System.Data.SqlClient;

namespace WebRole1
{
    public partial class Vertical_Partitioning : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!IsPostBack)
            {
                // Load data.
                var studentDataReader = SQLAzureHelper.ExecuteReader(
                        ConfigurationManager.ConnectionStrings["StudentsConnectionString"].ConnectionString,
                    sqlConnection =>
                    {
                        SqlCommand sqlCommand =
                            new SqlCommand("SELECT StudentId, StudentName FROM Student",
                                sqlConnection);
                        return (sqlCommand.ExecuteReader());
                    });

                var courseDataReader = SQLAzureHelper.ExecuteReader(
                    ConfigurationManager.ConnectionStrings["CoursesConnectionString"].ConnectionString,
                    sqlConnection =>
                    {
                        SqlCommand sqlCommand =
                            new SqlCommand("SELECT CourseName, StudentId FROM Course",
                                sqlConnection);
                        return (sqlCommand.ExecuteReader());
                    });

                // Join two tables on different SQL Azure databases using LINQ. 
                var query =
                    from student in studentDataReader
                    join course in courseDataReader on
                        (Int32)student["StudentId"] equals (Int32)course["StudentId"]
                    select new
                    {
                        CourseName = (string)course["CourseName"],
                        StudentName = (string)student["StudentName"]
                    };

                this.GridView3.DataSource = query;
                this.GridView3.DataBind();
            }
        }
    }
}