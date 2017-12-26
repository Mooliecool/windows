'****************************** Module Header ******************************\
' Module Name:  Rss.aspx.vb
' Project:      VBASPNETRssFeeds
' Copyright (c) Microsoft Corporation
'
' This is the main page of this sample that illustrate how to build a rss
' feed via ASP.NET.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' History:
' 01/19/2010 5:30 PM Bravo Yang Created
'***************************************************************************/

Imports System.Xml
Imports System.Data.SqlClient

Partial Public Class Rss
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        Response.ContentType = "application/rss+xml"
        Response.ContentEncoding = Encoding.UTF8

        Dim rsswriter As New XmlTextWriter(Response.OutputStream, Encoding.UTF8)

        'opening
        WriteRssOpening(rsswriter)

        'body
        Dim ArticlesRssTable As DataTable = GetDateSet()
        WriteRssBody(rsswriter, ArticlesRssTable)

        'ending
        WriteRssEnding(rsswriter)

        rsswriter.Flush()
        Response.End()
    End Sub

    Private Sub WriteRssOpening(ByVal rsswriter As XmlTextWriter)
        rsswriter.WriteStartElement("rss")
        rsswriter.WriteAttributeString("version", "2.0")
        rsswriter.WriteStartElement("channel")
        rsswriter.WriteElementString("title", "VBASPNETRssFeeds")
        rsswriter.WriteElementString("link", Request.Url.Host)
        rsswriter.WriteElementString("description", "This is a sample telling how to create rss feeds for a website.")
    End Sub

    Private Sub WriteRssBody(ByVal rsswriter As XmlTextWriter, ByVal data As DataTable)
        For Each rssitem As DataRow In data.Rows
            rsswriter.WriteStartElement("item")
            rsswriter.WriteElementString("title", rssitem(1).ToString())
            rsswriter.WriteElementString("author", rssitem(2).ToString())
            rsswriter.WriteElementString("link", rssitem(3).ToString())
            rsswriter.WriteElementString("description", rssitem(4).ToString())
            rsswriter.WriteElementString("pubDate", rssitem(5).ToString())
            rsswriter.WriteEndElement()
        Next
    End Sub

    Private Sub WriteRssEnding(ByVal rsswriter As XmlTextWriter)
        rsswriter.WriteEndElement()
        rsswriter.WriteEndElement()
    End Sub

    Private Function GetDateSet() As DataTable
        Dim ArticlesRssTable As New DataTable()

        Dim strconn As String = ConfigurationManager.ConnectionStrings("ConnStr4Articles").ConnectionString
        Dim conn As New SqlConnection(strconn)
        Dim strsqlquery As String = "SELECT * FROM [Articles]"

        Dim da As New SqlDataAdapter(strsqlquery, conn)
        da.Fill(ArticlesRssTable)

        Return ArticlesRssTable
    End Function

End Class