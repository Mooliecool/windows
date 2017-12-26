===========================================================================
             CSASPNETRssFeeds Project Overview
===========================================================================

/////////////////////////////////////////////////////////////////////////////
Note Before Start:

  When you go on through this sample, we assume that you are familiar with 
  Rss, including its usage,  format, and so forth. If not, please refer to this link
  first. It tells what is rss, the format of a standard rss file.

  http://www.mnot.net/rss/tutorial/

/////////////////////////////////////////////////////////////////////////////
Use:

  The project shows how to create a rss feed using ASP.NET. The AddArticle
  page in the sample is used to update the database. We can add, edit, update
  and delete a record, an article it is in this sample, and then turn to Rss page 
  to find the change. Using classes within XML namespace, Rss page create a
  rss feed that can be subscribed by users so that users can be noticed as soon
  as there is any change happened in the website.  

/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step1: Create a C# ASP.NET Web Application in Visual Studio 2008 / Visual 
Web Developer and name it as CSASPNETRssFeeds.

Step2: Add a database file into the application folder App_Data. In this sample,
the database is the same name as the project name.

Step3: Create a table named Article in the database and add these columns into
the table: ArticleID, Title, Author, Link, Description, PubDate.

Step4: Insert a new record into this table as a test. We will use this test record 
later.

Step5: Add a new ASP.NET page called AddArticle.aspx to the project, which is
used to update the database table.

Step6: Add a DataSource and a FormView control into this page. In short, set 
the DataSource connect to the Article table and bind this DataSource to the 
FormView. Enable the paging of the FormView and run the page to find that the 
datatable is totally under the control, which means records can be inserted, 
edited, updated and deleted from the FormView.

Step7:  Add a new ASP.NET page called Rss.aspx to the project. This is the star
of this project.

Step8: Write a function to get the data from the Article table in the codebehind of 
the rss page. It could be look like this.

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

[NOTE] This function returns a DataTable containing the records selected from 
the Article table via the SQL query string above. As a test, it returns all records
from the table. However, usually we only selecte 20 records of the latest article
from the table so that the process will not be too long and the rss page will not
be too big. Anyway, it depends on the requirement of the rss feed.

Step9: Write code to create the rss xml file. As it is only the code work, please 
refer to ths Rss.aspx.vb file in this sample.

[NOTE] Because a rss feed is an XML format file instead of a normal web
page, we need to modify the Response.ContentType property. Also, unicode
might be contained in the rss feed, we need to change the ContentEncoding 
property of current response as well.

    Response.ContentType = "application/rss+xml";
    Response.ContentEncoding = Encoding.UTF8;

As the rss feed content contains three parts: the opening, the body and the 
ending. We separate them into three methods: WriteRssOpening, WriteRssBody
and WriteRssEnding. When writing the body, we use For Each to loop through
the records in the DataTable and write them in an item node and write the title,
author, link, description and pubDate fileds as the attributes under the item node.

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

Finally, when all work has been doen, we need to end the response. Otherwise,
some error will occur to destory all things we did before and only return an error 
message "Internet Explorer cannot display this feed" on the page. So, do NOT
forget to add the code at the end of Page_Load event handler.

    Response.End();

/////////////////////////////////////////////////////////////////////////////
References:

MSDN: XmlTextWriter Class
http://msdn.microsoft.com/en-us/library/system.xml.xmltextwriter.aspx

MSDN: RSS Tutorial
http://www.mnot.net/rss/tutorial/

/////////////////////////////////////////////////////////////////////////////