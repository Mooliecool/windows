'****************************** Module Header ******************************\
' Module Name:    Item.aspx.vb
' Project:        VBASPNETBreadcrumbWithQueryString
' Copyright (c) Microsoft Corporation
'
' This page shows a item name. At the same time, this page show the 
' category name and the item name in the breadcrumb.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Public Class Item
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As EventArgs)
        If Not IsPostBack AndAlso Not String.IsNullOrEmpty(Request.QueryString("name")) Then
            ' Show the item name.
            lbMsg.Text = Request.QueryString("name")

            ' Handle SiteMapResolve event to dynamically change current SiteMapNode.
            AddHandler SiteMap.SiteMapResolve, AddressOf SiteMap_SiteMapResolve
        End If
    End Sub

    ''' <summary>
    ''' Occurs when the CurrentNode property is accessed.
    ''' </summary>
    ''' <param name="sender">
    ''' The source of the event, an instance of the SiteMapProvider class.
    ''' </param>
    ''' <param name="e">
    ''' A SiteMapResolveEventArgs that contains the event data.
    ''' </param>
    ''' <returns>
    ''' The SiteMapNode that represents the result of the SiteMapResolveEventHandler operation.
    ''' </returns>
    Private Function SiteMap_SiteMapResolve(ByVal sender As Object,
                                            ByVal e As SiteMapResolveEventArgs) As SiteMapNode
        ' Only need one execution in one request.
        RemoveHandler SiteMap.SiteMapResolve, AddressOf SiteMap_SiteMapResolve

        If SiteMap.CurrentNode IsNot Nothing Then
            ' SiteMap.CurrentNode is readonly, so we need to clone one to operate.
            Dim currentNode As SiteMapNode = SiteMap.CurrentNode.Clone(True)

            currentNode.Title = Request.QueryString("name")
            currentNode.ParentNode.Title = Database.GetCategoryByItem(Request.QueryString("name"))
            currentNode.ParentNode.Url = "/Category.aspx?name=" & Database.GetCategoryByItem(Request.QueryString("name"))

            ' Use the changed one in the Breadcrumb.
            Return currentNode
        End If
        Return Nothing
    End Function

End Class