/****************************** Module Header ******************************\
* Module Name:  Horizontal Partitioning.aspx.cs
* Project: CSSqlAzurePartitioning
* Copyright (c) Microsoft Corporation.
* 
* We are using a hash base partitioning schema in this example – hashing on the 
* primary key of the row.
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
using SQLAzure;
using System.Data.SqlClient;

namespace WebRole1
{
    public partial class Horizontal_Partitioning : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {

        }

        protected void btnAdd_Click(object sender, EventArgs e)
        {
            if (!string.IsNullOrEmpty(tbAccountName.Text))
            {
                InsertAccount(tbAccountName.Text);

                // Refresh Gridviews to update the account info.
                this.GridView1.DataBind();
                this.GridView2.DataBind();
            }
        }

        /// <summary>
        /// We use the primary key to calculate the connection string and the as well 
        /// as a parameter to the SqlCommand.
        /// </summary>
        /// <param name="id"></param>
        /// <returns></returns>
        static String AccountName(Guid id)
        {
            var accountDataReader = SQLAzureHelper.ExecuteReader(
                SQLAzureHelper.ConnectionString(id),
                sqlConnection =>
                {
                    String sql = @"SELECT [Name] FROM [Accounts] WHERE Id = @Id";
                    SqlCommand sqlCommand = new SqlCommand(sql, sqlConnection);
                    sqlCommand.Parameters.AddWithValue("@Id", id);
                    return (sqlCommand.ExecuteReader());
                });

            return ((from row in accountDataReader select (string)row["Name"]).
                FirstOrDefault());
        }

        /// <summary>
        /// When inserting a single row, we need to know the primary key before connecting to the database.
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        static Guid InsertAccount(String name)
        {
            Guid id = Guid.NewGuid();

            SQLAzureHelper.ExecuteNonQuery(
                 SQLAzureHelper.ConnectionString(id),
                 sqlConnection =>
                 {
                     String sql = @"INSERT INTO [Accounts] ([Id], [Name]) VALUES (@Id, @Name)";
                     SqlCommand sqlCommand = new SqlCommand(sql, sqlConnection);
                     sqlCommand.Parameters.AddWithValue("@Name", name);
                     sqlCommand.Parameters.AddWithValue("@Id", id);
                     sqlCommand.ExecuteNonQuery();
                 });

            return (id);
        }
    }
}