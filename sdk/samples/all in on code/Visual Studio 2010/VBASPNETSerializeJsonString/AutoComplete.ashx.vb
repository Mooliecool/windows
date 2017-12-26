'/****************************** Module Header ******************************\
'* Module Name:    AutoComplete.ashx.vb
'* Project:        VBASPNETSerializeJsonString
'* Copyright (c) Microsoft Corporation
'*
'* This project illustrates how to serialize Json string. we use jQuery at client 
'* side and manipulate XML data at server side.
'* It demonstrates how to use the serializable json data through an autocomplete 
'* example. 
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\*****************************************************************************/

Imports System.Web
Imports System.Web.Services
Imports System.Web.Script.Serialization
Imports System.Data


Public Class AutoComplete
    Implements System.Web.IHttpHandler

    Sub ProcessRequest(ByVal context As HttpContext) Implements IHttpHandler.ProcessRequest

        '  Query string 'term' is for autocomplete. By default, it sends the variable 
        '  "term" with the search word to the backend page.
        Dim searchText As String = context.Request.QueryString("term")
        Dim books As Collection = New Collection

        Dim ds As New DataSet()
        ds.ReadXml(HttpContext.Current.Server.MapPath("App_Data/books.xml"))
        Dim dv As DataView = ds.Tables(0).DefaultView
        dv.RowFilter = [String].Format("title like '{0}*'", searchText.Replace("'", "''"))

        Dim book As Book
        For Each myDataRow As DataRowView In dv
            book = New Book()
            book.id = myDataRow("id").ToString()
            book.value = myDataRow("title").ToString()
            book.label = myDataRow("title").ToString()
            book.Author = myDataRow("author").ToString()
            book.Genre = myDataRow("genre").ToString()
            book.Price = myDataRow("price").ToString()
            book.Publish_date = myDataRow("publish_date").ToString()
            book.Description = myDataRow("description").ToString()
            books.Add(book)
        Next

        Dim serializer As JavaScriptSerializer = New JavaScriptSerializer
        Dim jsonString As String = serializer.Serialize(books)
        context.Response.Write(jsonString)

    End Sub

    ReadOnly Property IsReusable() As Boolean Implements IHttpHandler.IsReusable
        Get
            Return False
        End Get
    End Property


End Class