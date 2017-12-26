'****************************** Module Header ******************************\
' Module Name:    DynamicBreadcrumb.aspx.vb
' Project:        VBASPNETBreadcrumbWithQueryString
' Copyright (c) Microsoft Corporation
'
' This page shows that even a page is not in the site map, we still can 
' create the breadcrumb dynamically. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Public Class DynamicBreadcrumb
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As EventArgs)
        If Not IsPostBack Then
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

        ' We can dynamically create many SiteMapNodes here.
        Dim childNode As New SiteMapNode(SiteMap.Provider, "2")
        childNode.Url = "/child.aspx"
        childNode.Title = "child"

        childNode.ParentNode = New SiteMapNode(SiteMap.Provider, "1")
        childNode.ParentNode.Url = "/root.aspx"
        childNode.ParentNode.Title = "root"

        ' Also we can associate the dynamic nodes with the existent site map.
        Dim nodeFromSiteMap As SiteMapNode = GetSiteMapNode("item")
        If nodeFromSiteMap IsNot Nothing Then
            childNode.ParentNode.ParentNode = nodeFromSiteMap
        End If

        ' Use the new SiteMapNode in the breadcrumb.
        Return childNode
    End Function

    ''' <summary>
    ''' Get a siteMapNode from the site map.
    ''' </summary>
    ''' <param name="key">
    ''' The resourceKey of the siteMapNode.
    ''' </param>
    ''' <returns></returns>
    Private Function GetSiteMapNode(ByVal key As String) As SiteMapNode
        Return GetSiteMapNode(SiteMap.RootNode, key)
    End Function
    Private Function GetSiteMapNode(ByVal rootNode As SiteMapNode, ByVal key As String) As SiteMapNode
        If rootNode.ResourceKey = key Then
            Return rootNode
        ElseIf rootNode.HasChildNodes Then
            For Each childNode As SiteMapNode In rootNode.ChildNodes
                Dim resultNode As SiteMapNode = GetSiteMapNode(childNode, key)
                If resultNode IsNot Nothing Then
                    Return resultNode
                End If
            Next
        End If
        Return Nothing
    End Function

End Class