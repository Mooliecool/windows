/****************************** Module Header ******************************\
Module Name:  Default.aspx
Project:      CSASPNETDataListImageGallery
Copyright (c) Microsoft Corporation.

This module is used to show how to do a simple image gallery with a thumbnail 
navigation bar using DataList. User can select a thumbnail image from DataList 
to view the bigger one in main image. This can be implemented in some personal 
space site, online shopping site and etc.


This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Web.UI.WebControls;
using System.IO;
using System.Data;

namespace CSASPNETDataListImageGallery
{
    public partial class _Default : System.Web.UI.Page
    {
        // Set page size.
        int _pageSize = 5;

        // Property for current page index.
        public int Page_Index
        {
            get { return (int)ViewState["_Page_Index"]; }
            set { ViewState["_Page_Index"] = value; }
        }

        // Property for total page count.
        public int Page_Count
        {
            get { return (int)ViewState["_Page_Count"]; }
            set { ViewState["_Page_Count"] = value; }
        }
        
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!Page.IsPostBack)
            {
                // Initial page index.
                Page_Index = 0;

                // Set page count.
                if ((ImageCount() % _pageSize) == 0)
                {
                    Page_Count = ImageCount() / _pageSize;
                    if (Page_Count == 0)
                    {
                        lbnFirstPage.Enabled = false;
                        lbnPrevPage.Enabled = false;
                        lbnNextPage.Enabled = false;
                        lbnLastPage.Enabled = false;
                    }
                }
                else
                {
                    Page_Count = (ImageCount() / _pageSize + 1);
                }

                // Bind DataList.
                DataList1.DataSource = BindGrid();
                DataList1.DataBind();

                // Disable two buttons for the initial page.
                lbnFirstPage.Enabled = false;
                lbnPrevPage.Enabled = false;
            }
        }

        // Return the data source for DataList.
        protected DataTable BindGrid()
        {
            // Get all image paths.            
            DirectoryInfo di = new DirectoryInfo(Server.MapPath("/Image/"));
            FileInfo[] fi = di.GetFiles();

            // Save all paths to the DataTable as the data source.
            DataTable dt = new DataTable();
            DataColumn dc = new DataColumn("Url", typeof(System.String));
            dt.Columns.Add(dc);
            int lastindex = 0;
            if (Page_Count == 0 || Page_Index == Page_Count - 1)
            {
                lastindex = ImageCount();
            }
            else
            {
                lastindex = Page_Index * _pageSize + 5;
            }
            for (int i = Page_Index * _pageSize; i < lastindex; i++)
            {
                DataRow dro = dt.NewRow();
                dro[0] = fi[i].Name;
                dt.Rows.Add(dro);
            }
            return dt;
        }

        // Return total number of images.
        protected int ImageCount()
        {
            DirectoryInfo di = new DirectoryInfo(Server.MapPath("/Image/"));
            FileInfo[] fi = di.GetFiles();
            return fi.GetLength(0);
        }

        // Handle the navigation button event.
        public void Page_OnClick(Object sender, CommandEventArgs e)
        {
            if (e.CommandName.Equals("first"))
            {
                Page_Index = 0;
                lbnFirstPage.Enabled = false;
                lbnPrevPage.Enabled = false;
                lbnNextPage.Enabled = true;
                lbnLastPage.Enabled = true;
            }
            else if (e.CommandName.Equals("prev"))
            {
                Page_Index -= 1;
                if (Page_Index == 0)
                {
                    lbnFirstPage.Enabled = false;
                    lbnPrevPage.Enabled = false;
                    lbnNextPage.Enabled = true;
                    lbnLastPage.Enabled = true;
                }
                else
                {
                    lbnFirstPage.Enabled = true;
                    lbnPrevPage.Enabled = true;
                    lbnNextPage.Enabled = true;
                    lbnLastPage.Enabled = true;
                }
            }
            else if (e.CommandName.Equals("next"))
            {
                Page_Index += 1;
                if (Page_Index == Page_Count - 1)
                {
                    lbnFirstPage.Enabled = true;
                    lbnPrevPage.Enabled = true;
                    lbnNextPage.Enabled = false;
                    lbnLastPage.Enabled = false;
                }
                else
                {
                    lbnFirstPage.Enabled = true;
                    lbnPrevPage.Enabled = true;
                    lbnNextPage.Enabled = true;
                    lbnLastPage.Enabled = true;
                }
            }
            else if (e.CommandName.Equals("last"))
            {
                Page_Index = Page_Count - 1;
                lbnFirstPage.Enabled = true;
                lbnPrevPage.Enabled = true;
                lbnNextPage.Enabled = false;
                lbnLastPage.Enabled = false;
            }

            DataList1.SelectedIndex = 0;
            DataList1.DataSource = BindGrid();
            DataList1.DataBind();
            Image1.ImageUrl 
                = ((Image)DataList1.Items[0].FindControl("imgBtn")).ImageUrl;
        }

        // Handle the thumbnail image selecting event.
        protected void imgBtn_Click(object sender, EventArgs e)
        {
            ImageButton ib = (ImageButton)sender;
            Image1.ImageUrl = ib.ImageUrl;
            DataList1.SelectedIndex = Convert.ToInt32(ib.CommandArgument);
        }
    }
}
