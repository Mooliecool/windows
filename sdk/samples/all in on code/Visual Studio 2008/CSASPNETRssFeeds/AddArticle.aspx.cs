/************************************* Module Header **************************************\
* Module Name:    AddArticle.aspx.cs
* Project:        CSASPNETRssFeeds
* Copyright (c) Microsoft Corporation
*
* This page supports a feature to update the database to test whether
* the rss page works well.
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* History:
* 01/19/2010 5:30 PM Bravo Yang Created
\******************************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace CSASPNETRssFeeds
{
    public partial class AddArticle : System.Web.UI.Page
    {
        protected void ArticleFormView_PreRender(object sender, EventArgs e)
        {
            TextBox PubDateTextBox = (TextBox)ArticleFormView.FindControl("PubDateTextBox");
            if (PubDateTextBox != null)
            {
                PubDateTextBox.Text = DateTime.Now.ToShortDateString();
            }
        }
    }
}
