/****************************** Module Header ******************************\
* Module Name:    Default.aspx.cs
* Project:        CSASPNETBreadcrumbWithQueryString
* Copyright (c) Microsoft Corporation
*
* This is the root page that shows a category list.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;

namespace CSASPNETBreadcrumbWithQueryString
{
    public partial class Default : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!IsPostBack)
            {
                // Show a category list.
                gvCategories.DataSource = Database.Categories;
                gvCategories.DataBind();
            }
        }
    }
}