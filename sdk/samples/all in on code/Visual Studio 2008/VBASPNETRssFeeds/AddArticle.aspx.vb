'****************************** Module Header ******************************\
' Module Name:  AddArticle.aspx.vb
' Project:      VBASPNETRssFeeds
' Copyright (c) Microsoft Corporation
'
' This page supports a feature to update the database to test whether
' the rss page works well.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' History:
' 01/19/2010 5:30 PM Bravo Yang Created
'***************************************************************************/


Partial Public Class AddArticle
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

    End Sub

    Private Sub ArticleFormView_PreRender(ByVal sender As Object, ByVal e As System.EventArgs) Handles ArticleFormView.PreRender
        Dim PubDateTextBox As TextBox
        PubDateTextBox = CType(ArticleFormView.FindControl("PubDateTextBox"), TextBox)
        If PubDateTextBox IsNot Nothing Then
            PubDateTextBox.Text = DateTime.Now.ToShortDateString()
        End If

    End Sub
End Class