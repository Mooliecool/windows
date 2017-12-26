/****************************** Module Header ******************************\
* Module Name:    DBProcess.cs
* Project:        CSExcelLikeGridView
* Copyright (c) Microsoft Corporation
*
* This is a UI module which lets which lets you do a batch inserting, updating
* as well as deleting.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
\*****************************************************************************/

using System;
using System.Web.UI.WebControls;
using System.Data;
using System.Text;

namespace CSExcelLikeGridView
{
    public partial class _Default : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!IsPostBack)
            {
                DBProcess p = new DBProcess();
                DataTable dt = p.GetDataTable();
                ViewState["dt"] = dt;
                GridView1.DataSource = dt;
                GridView1.DataBind();
            }
        }

        /// <summary>
        /// This function will confirm the last modification
        /// and do a batch save.
        /// </summary>
        protected void btnSaveAll_Click(object sender, EventArgs e)
        {
            //Default the value is false, meaning that the db isn't saved
            bool flag = false;

            DBProcess p = new DBProcess();
            DataTable dt = ViewState["dt"] as DataTable;

            // Change states and do a batch update
            for (int i = 0; i < GridView1.Rows.Count; i++)
            {
                if ((GridView1.Rows[i].FindControl("chkDelete") as CheckBox).Checked)
                {
                    dt.Rows[i].Delete();
                    flag = true;
                }
                else
                {
                    if (dt.Rows[i]["PersonName"].ToString()
                        != (GridView1.Rows[i].FindControl("tbName") as TextBox).Text)
                    {
                        if (dt.Rows[i].RowState == DataRowState.Unchanged)
                        {
                            dt.Rows[i].BeginEdit();
                        }
                        dt.Rows[i]["PersonName"] =
                            (GridView1.Rows[i].FindControl("tbName") as TextBox).Text;
                        if (dt.Rows[i].RowState == DataRowState.Unchanged)
                        {
                            dt.Rows[i].EndEdit();
                        }
                        flag = true;
                    }
                    if (dt.Rows[i]["PersonAddress"].ToString()
                        != (GridView1.Rows[i].FindControl("tbAddress") as TextBox).Text)
                    {
                        if (dt.Rows[i].RowState == DataRowState.Unchanged)
                        {
                            dt.Rows[i].BeginEdit();
                        }
                        dt.Rows[i]["PersonAddress"] =
                            (GridView1.Rows[i].FindControl("tbAddress") as TextBox).Text;
                        if (dt.Rows[i].RowState == DataRowState.Unchanged)
                        {
                            dt.Rows[i].EndEdit();
                        }
                        flag = true;
                    }
                }
            }

            p.BatchSave(dt);

            // Save data into db, so no need to maintain the state colors
            HidState.Value = "[]";

            dt = p.GetDataTable();
            GridView1.DataSource = dt;
            GridView1.DataBind();

            if (flag)
            {
                ClientScript.RegisterStartupScript
        (GetType(), "js", "alert('Save All Changes successfully!');", true);
            }

        }

        /// <summary>
        ///  This function processes with the following aspect:
        ///  Record every row and every cell's state into HidState.
        /// </summary>
        protected void btnAdd_Click(object sender, EventArgs e)
        {
            int i = 0;
            DBProcess p = new DBProcess();
            DataTable dt = ViewState["dt"] as DataTable;
            TextBox tb1 = null;
            TextBox tb2 = null;
            StringBuilder sbu = new StringBuilder();
            CheckBox chkbox = null;

            // To maintain the JSON state.
            sbu.Append("[");

            for (; i < dt.Rows.Count; ++i)
            {
                // Loop the cells
                for (int j = 2; j < GridView1.HeaderRow.Cells.Count; ++j)
                {
                    tb1 = GridView1.Rows[i].FindControl("tbName") as TextBox;
                    tb2 = GridView1.Rows[i].FindControl("tbAddress") as TextBox;

                    sbu.Append("{'Index':'" + (i * GridView1.HeaderRow.Cells.Count + j));

                    //Not added, meaning unchanged or added
                    if (dt.Rows[i].RowState !=DataRowState.Added)
                    {
                        // Decide whether the 1st cell in db changed or not
                        if (j == 2)
                        {
                            if (!tb1.Text.Equals(dt.Rows[i][j - 1, DataRowVersion.Original]))
                            {
                                dt.Rows[i].BeginEdit();
                                sbu.Append("','Color':'blue',");
                                dt.Rows[i][j - 1] = tb1.Text;
                            }
                            else
                            {
                                sbu.Append("','Color':'',");
                            }
                        }
                        else
                        {
                            // Decide whether the 2nd cell in db changed or not
                            if (!tb2.Text.Equals(dt.Rows[i][j - 1, DataRowVersion.Original]))
                            {
                                dt.Rows[i].BeginEdit();
                                sbu.Append("','Color':'blue',");
                                dt.Rows[i][j - 1] = tb2.Text;
                            }
                            else
                            {
                                sbu.Append("','Color':'',");
                            }
                        }
                        dt.Rows[i].EndEdit();
                    }

                    else
                    {
                        // Add row should be marked as green
                        if (dt.Rows[i].RowState == DataRowState.Added)
                        {
                            sbu.Append("','Color':'green',");
                        }
                        // Other rows should keep the original color
                        else
                        {
                            sbu.Append("','Color':'',");
                        }
                    }

                    // Keep the Delete Statement
                    chkbox = GridView1.Rows[i].FindControl("chkDelete") as CheckBox;
                    sbu.Append("'Deleted':'" + chkbox.Checked + "'},");
                }
            }

            DataRow r = dt.NewRow();
            r["PersonName"] = (GridView1.FooterRow.FindControl("tbNewName") as TextBox).Text;
            r["PersonAddress"] = (GridView1.FooterRow.FindControl("tbNewAddress") as TextBox).Text;
            dt.Rows.Add(r);
            sbu.Append("{'Index':'" + (i * GridView1.HeaderRow.Cells.Count
                            + 2) + "','Color':'green','Deleted':'false'},");
            sbu.Append("{'Index':'" + (i * GridView1.HeaderRow.Cells.Count
                            + 3) + "','Color':'green','Deleted':'false'}");
            sbu.Append("]");
            HidState.Value = sbu.ToString();
            GridView1.DataSource = dt;
            GridView1.DataBind();
        }
    }
}