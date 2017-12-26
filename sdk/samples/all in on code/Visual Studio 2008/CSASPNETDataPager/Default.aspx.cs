/****************************** Module Header ******************************\
* Module Name:  Default.aspx.cs
* Project:      CSASPNETDataPager
* Copyright (c) Microsoft Corporation.
* 
* The CSASPNETDataPager sample describes how to use ASP.NET DataPager to 
* render a paging interface and communicate to the corresponding data-bound 
* control, which is a ListView control in this sample.
*
* The ListView control is populated with data from SQL Server database in 
* ADO.NET way. The sample uses the SQLServer2005DB sample database.  
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* 9/2/2009 2:10 AM Jian Kang Created
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Data;
using System.Data.SqlClient;
using System.Configuration;
#endregion Using directives

namespace CSASPNETDataPager
{
    public partial class _Default : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
 
        }

        private void BindListView()
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


                // Bind the GridView control.
                lvPerson.DataSource = dsPerson;
                lvPerson.DataBind();
            }
        }

        protected void dpPerson_PreRender(object sender, EventArgs e)
        {
            // Rebind the ListView control to 
            // show data in the new page before the page is rendered.
            BindListView();
        }

        protected void TemplateNextPrevious_OnPagerCommand(object sender, DataPagerCommandEventArgs e)
        {
            // Check which LinkButton control raised the event.

            // MaximumRows: The maximum number of records that are displayed 
            // for each page of data.

            // StartRowIndex: The index of the first record that is displayed 
            // on a page of data.

            // TotalRowCount: The total number of records that are available 
            // in the underlying data source.
            
            switch (e.CommandName)
            {
                // "First" is clicked.
                case "First":
                    // In this sample, the maximum number of records on 
                    // each page is 10, which is the default page size. 

                    // The first page shows the first 10 rows of the data 
                    // source, whose row index start from 0.
                    e.NewStartRowIndex = 0;
                    e.NewMaximumRows = e.Item.Pager.MaximumRows;

                    break;

                // "Previous" is clicked.
                case "Previous":
                    // The previous page shows the following 10 rows of 
                    // the current page. 
                    e.NewStartRowIndex = e.Item.Pager.StartRowIndex - e.Item.Pager.PageSize;
                    e.NewMaximumRows = e.Item.Pager.MaximumRows;

                    break;

                // "Next" is clicked.
                case "Next":
                    // The next page shows 10 rows which start from the last 
                    // row in current page.
                    int iNewIndex = e.Item.Pager.StartRowIndex + e.Item.Pager.PageSize;
                    // The new index of the first row to display cannot
                    // be bigger than total row count
                    if (iNewIndex <= e.TotalRowCount)
                    {
                        e.NewStartRowIndex = iNewIndex;
                        e.NewMaximumRows = e.Item.Pager.MaximumRows;
                    }

                    break;   
           
                // "Last" is clicked.
                case "Last":
                    // The last page shows the last rows of the data source, 
                    // which are the remainders after total number is 
                    // divided by page size. 
                    e.NewStartRowIndex = e.TotalRowCount - e.TotalRowCount % e.Item.Pager.PageSize;
                    e.NewMaximumRows = e.Item.Pager.MaximumRows;

                    break;
            }
        }

        protected void TemplateGoTo_OnPagerCommand(object sender, DataPagerCommandEventArgs e)
        {
            // Since there is only one Button control, we don't need 
            // to use "switch" or "if".
            
            // Fetch the new pager number from the TextBox tbPagerNumber.
            int iNewPage = Convert.ToInt32(((TextBox)e.Item.FindControl("tbPageNumber")).Text);

            // Calculate the new index of the first row to display.
            int iNewIndex = e.Item.Pager.PageSize * (iNewPage-1);

            // The new row index cannot be bigger than total row count
            if (iNewIndex <= e.TotalRowCount)
            {
                e.NewStartRowIndex = iNewIndex;
                e.NewMaximumRows = e.Item.Pager.MaximumRows;
            }          
        }
    }
}
