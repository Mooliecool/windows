/****************************** Module Header ******************************\
* Module Name:  Default.aspx.cs
* Project:      CSASPNETFormViewUpload
* Copyright (c) Microsoft Corporation.
* 
* This page populates a FromView control with data from a SQL Server 
* database and provides UI for data manipulation.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Data.SqlClient;
using System.Configuration;
using System.Data;
#endregion Using directives

namespace CSASPNETFormViewUpload
{
    public partial class Default : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            // The Page is accessed for the first time.
            if (!IsPostBack)
            {
                // Enable the FormView paging option and 
                // specify the PageButton count.
                fvPerson.AllowPaging = true;
                fvPerson.PagerSettings.PageButtonCount = 15;

                // Populate the FormView control.
                BindFormView();
            }
        }

        private void BindFormView()
        {
            // Get the connection string from Web.config. 
            // When we use Using statement, 
            // we don't need to explicitly dispose the object in the code, 
            // the using statement takes care of it.
            using (SqlConnection conn = new SqlConnection(ConfigurationManager.ConnectionStrings["SQLServer2005DBConnectionString"].ToString()))
            {
                // Create a DataSet object.
                DataSet dsPerson = new DataSet();

                // Create a SELECT query.
                string strSelectCmd = "SELECT PersonID,LastName,FirstName FROM Person";

                // Create a SqlDataAdapter object
                // SqlDataAdapter represents a set of data commands and a 
                // database connection that are used to fill the DataSet and 
                // update a SQL Server database. 
                SqlDataAdapter da = new SqlDataAdapter(strSelectCmd, conn);

                // Open the connection
                conn.Open();

                // Fill the DataTable named "Person" in DataSet with the rows
                // returned by the query.
                da.Fill(dsPerson, "Person");


                // Bind the FormView control.
                fvPerson.DataSource = dsPerson;
                fvPerson.DataBind();
            }
        }

        // FormView.PageIndexChanging Event
        protected void fvPerson_PageIndexChanging(object sender, FormViewPageEventArgs e)
        {
            // Set the index of the new display page. 
            fvPerson.PageIndex = e.NewPageIndex;

            // Rebind the FormView control to show data in the new page.
            BindFormView();
        }

        // FormView.ItemInserting Event
        protected void fvPerson_ItemInserting(object sender, FormViewInsertEventArgs e)
        {
            // Get the connection string from Web.config. 
            // When we use Using statement, 
            // we don't need to explicitly dispose the object in the code, 
            // the using statement takes care of it.
            using (SqlConnection conn = new SqlConnection(ConfigurationManager.ConnectionStrings["SQLServer2005DBConnectionString"].ToString()))
            {
                // Create a command object.
                SqlCommand cmd = new SqlCommand();

                // Assign the connection to the command.
                cmd.Connection = conn;

                // Set the command text
                // SQL statement or the name of the stored procedure. 
                cmd.CommandText = "INSERT INTO Person ( LastName, FirstName, Picture ) VALUES ( @LastName, @FirstName, @Picture )";

                // Set the command type
                // CommandType.Text for ordinary SQL statements; 
                // CommandType.StoredProcedure for stored procedures.
                cmd.CommandType = CommandType.Text;

                // Get the first name and last name from the 
                // InsertItemTemplate of the FormView control.
                string strLastName = ((TextBox)fvPerson.Row.FindControl("tbLastName")).Text;
                string strFirstName = ((TextBox)fvPerson.Row.FindControl("tbFirstName")).Text;

                // Append the parameters to the SqlCommand and set values.
                cmd.Parameters.Add("@LastName", SqlDbType.NVarChar, 50).Value = strLastName;
                cmd.Parameters.Add("@FirstName", SqlDbType.NVarChar, 50).Value = strFirstName;

                FileUpload uploadPicture = (FileUpload)fvPerson.FindControl("uploadPicture");

                if (uploadPicture.HasFile)
                {
                    // Append the Picture parameter to the SqlCommand.
                    // If a picture is specified, set the parameter with 
                    // the value of bytes in the specified picture file. 
                    cmd.Parameters.Add("@Picture", SqlDbType.VarBinary).Value = uploadPicture.FileBytes;
                }
                else
                {
                    // Append the Picture parameter to the SqlCommand.
                    // If no picture is specified, set the parameter's 
                    // value to NULL.
                    cmd.Parameters.Add("@Picture", SqlDbType.VarBinary).Value = DBNull.Value;
                }

                // Open the connection.
                conn.Open();

                // Execute the command.
                cmd.ExecuteNonQuery();
            }

            // Switch FormView control to the ReadOnly display mode. 
            fvPerson.ChangeMode(FormViewMode.ReadOnly);

            // Rebind the FormView control to show data after inserting.
            BindFormView();
        }

        // FormView.ItemUpdating Event
        protected void fvPerson_ItemUpdating(object sender, FormViewUpdateEventArgs e)
        {
            // Get the connection string from Web.config. 
            // When we use Using statement, 
            // we don't need to explicitly dispose the object in the code, 
            // the using statement takes care of it.
            using (SqlConnection conn = new SqlConnection(ConfigurationManager.ConnectionStrings["SQLServer2005DBConnectionString"].ToString()))
            {
                // Create a command object.
                SqlCommand cmd = new SqlCommand();

                // Assign the connection to the command.
                cmd.Connection = conn;

                // Set the command text
                // SQL statement or the name of the stored procedure. 
                cmd.CommandText = "UPDATE Person SET LastName = @LastName, FirstName = @FirstName, Picture = ISNULL(@Picture,Picture) WHERE PersonID = @PersonID";

                // Set the command type
                // CommandType.Text for ordinary SQL statements; 
                // CommandType.StoredProcedure for stored procedures.
                cmd.CommandType = CommandType.Text;

                // Get the person ID, first name and last name from the 
                // EditItemTemplate of the FormView control.
                string strPersonID = ((Label)fvPerson.Row.FindControl("lblPersonID")).Text;
                string strLastName = ((TextBox)fvPerson.Row.FindControl("tbLastName")).Text;
                string strFirstName = ((TextBox)fvPerson.Row.FindControl("tbFirstName")).Text;

                // Append the parameters to the SqlCommand and set values.
                cmd.Parameters.Add("@PersonID", SqlDbType.Int).Value = strPersonID;
                cmd.Parameters.Add("@LastName", SqlDbType.NVarChar, 50).Value = strLastName;
                cmd.Parameters.Add("@FirstName", SqlDbType.NVarChar, 50).Value = strFirstName;

                // Find the FileUpload control in the EditItemTemplate of 
                // the FormView control.
                FileUpload uploadPicture = (FileUpload)fvPerson.FindControl("uploadPicture");

                if (uploadPicture.HasFile)
                {
                    // Append the Picture parameter to the SqlCommand.
                    // If a picture is specified, set the parameter with 
                    // the value of bytes in the specified picture file. 
                    cmd.Parameters.Add("@Picture", SqlDbType.VarBinary).Value = uploadPicture.FileBytes;
                }
                else
                {
                    // Append the Picture parameter to the SqlCommand.
                    // If no picture is specified, set the parameter's 
                    // value to NULL.
                    cmd.Parameters.Add("@Picture", SqlDbType.VarBinary).Value = DBNull.Value;
                }

                // Open the connection.
                conn.Open();

                // Execute the command.
                cmd.ExecuteNonQuery();
            }

            // Switch FormView control to the ReadOnly display mode. 
            fvPerson.ChangeMode(FormViewMode.ReadOnly);

            // Rebind the FormView control to show data after updating.
            BindFormView();
        }

        // FormView.ItemDeleting Event
        protected void fvPerson_ItemDeleting(object sender, FormViewDeleteEventArgs e)
        {
            // Get the connection string from Web.config. 
            // When we use Using statement, 
            // we don't need to explicitly dispose the object in the code, 
            // the using statement takes care of it.
            using (SqlConnection conn = new SqlConnection(ConfigurationManager.ConnectionStrings["SQLServer2005DBConnectionString"].ToString()))
            {
                // Create a command object.
                SqlCommand cmd = new SqlCommand();

                // Assign the connection to the command.
                cmd.Connection = conn;

                // Set the command text
                // SQL statement or the name of the stored procedure. 
                cmd.CommandText = "DELETE FROM Person WHERE PersonID = @PersonID";

                // Set the command type
                // CommandType.Text for ordinary SQL statements; 
                // CommandType.StoredProcedure for stored procedures.
                cmd.CommandType = CommandType.Text;

                // Get the PersonID from the ItemTemplate of the FormView 
                // control.
                string strPersonID = ((Label)fvPerson.Row.FindControl("lblPersonID")).Text;

                // Append the parameter to the SqlCommand and set value.
                cmd.Parameters.Add("@PersonID", SqlDbType.Int).Value = strPersonID;

                // Open the connection.
                conn.Open();

                // Execute the command.
                cmd.ExecuteNonQuery();
            }

            // Rebind the FormView control to show data after deleting.
            BindFormView();
        }

        // FormView.ModeChanging Event
        protected void fvPerson_ModeChanging(object sender, FormViewModeEventArgs e)
        {
            // Switch FormView control to the new mode
            fvPerson.ChangeMode(e.NewMode);

            // Rebind the FormView control to show data in new mode.
            BindFormView();
        }   
    }
}
