/************************************* Module Header **************************************\
* Module Name:    Default.aspx.cs
* Project:        CSASPNETRssFeeds
* Copyright (c) Microsoft Corporation
*
* This is the main page of this sample that illustrate how to build a rss
* feed via ASP.NET.
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* History:
* 01/19/2010 5:30 PM Bravo Yang Created
\******************************************************************************************/

#region using namespaces
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Data;
using System.Data.SqlClient;
using System.Configuration;
using System.Xml;
using System.Text;
#endregion

namespace CSASPNETRssFeeds
{
    public partial class Rss : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            Response.ContentType = "application/rss+xml";
            Response.ContentEncoding = Encoding.UTF8;

            XmlTextWriter rsswriter = new XmlTextWriter(Response.OutputStream, Encoding.UTF8);

            WriteRssOpening(rsswriter);

            DataTable ArticlesRssTable = GetDateSet();
            WriteRssBody(rsswriter, ArticlesRssTable);

            WriteRssEnding(rsswriter);

            rsswriter.Flush();
            Response.End();
        }

        private void WriteRssOpening(XmlTextWriter rsswriter)
        {
            rsswriter.WriteStartElement("rss");
            rsswriter.WriteAttributeString("version", "2.0");
            rsswriter.WriteStartElement("channel");
            rsswriter.WriteElementString("title", "CSASPNETRssFeeds");
            rsswriter.WriteElementString("link", Request.Url.Host);
            rsswriter.WriteElementString("description", "This is a sample telling how to create rss feeds for a website.");
        }

        private void WriteRssBody(XmlTextWriter rsswriter, DataTable data)
        {
            foreach (DataRow rssitem in data.Rows)
            {
                rsswriter.WriteStartElement("item");
                rsswriter.WriteElementString("title", rssitem[1].ToString());
                rsswriter.WriteElementString("author", rssitem[2].ToString());
                rsswriter.WriteElementString("link", rssitem[3].ToString());
                rsswriter.WriteElementString("description", rssitem[4].ToString());
                rsswriter.WriteElementString("pubDate", rssitem[5].ToString());
                rsswriter.WriteEndElement();
            }
        }

        private void WriteRssEnding(XmlTextWriter rsswriter)
        {
            rsswriter.WriteEndElement();
            rsswriter.WriteEndElement();
        }

        private DataTable GetDateSet()
        {
            DataTable ArticlesRssTable = new DataTable();
            
            string strconn = ConfigurationManager.ConnectionStrings["ConnStr4Articles"].ConnectionString;
            SqlConnection conn = new SqlConnection(strconn);
            string strsqlquery = "SELECT * FROM [Articles]";

            SqlDataAdapter da = new SqlDataAdapter(strsqlquery, conn);
            da.Fill(ArticlesRssTable);

            return ArticlesRssTable;
        }
    }
}
