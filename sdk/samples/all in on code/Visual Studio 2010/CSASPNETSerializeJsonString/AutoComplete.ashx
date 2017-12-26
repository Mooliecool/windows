/****************************** Module Header ******************************\
* Module Name:    AutoComplete.ashx
* Project:        CSASPNETSerializeJsonString
* Copyright (c) Microsoft Corporation
*
* This project illustrates how to serialize Json string. we use jQuery at client 
* side and manipulate XML data at server side.
* It demonstrates how to use the serializable json data through an autocomplete 
* example.  
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

<%@ WebHandler Language="C#" Class="AutoComplete" %> 
using System;
using System.Collections.ObjectModel;
using System.Data;
using System.Web;
using System.Web.Script.Serialization;

public class AutoComplete : IHttpHandler
{

    public void ProcessRequest(HttpContext context)
    {
        //  Query string 'term' is for autocomplete. By default, it sends the variable 
        //  "term" with the search word to the backend page.
        string searchText = context.Request.QueryString["term"];

        Collection<Book> books = new Collection<Book>();

        DataSet ds = new DataSet();
        ds.ReadXml(HttpContext.Current.Server.MapPath("App_Data/books.xml"));
        DataView dv = ds.Tables[0].DefaultView;
        dv.RowFilter = String.Format("title like '{0}*'", searchText.Replace("'", "''"));

        Book book;
        foreach (DataRowView myDataRow in dv)
        {
            book = new Book();
            book.id = myDataRow["id"].ToString();
            book.value = book.label = myDataRow["title"].ToString();
            book.Author = myDataRow["author"].ToString();
            book.Genre = myDataRow["genre"].ToString();
            book.Price = myDataRow["price"].ToString();
            book.Publish_date = myDataRow["publish_date"].ToString();
            book.Description = myDataRow["description"].ToString();
            books.Add(book);
        }

        JavaScriptSerializer serializer = new JavaScriptSerializer();

        string jsonString = serializer.Serialize(books);

        context.Response.Write(jsonString);
    }

    public bool IsReusable
    {
        get
        {
            return false;
        }
    }
}

