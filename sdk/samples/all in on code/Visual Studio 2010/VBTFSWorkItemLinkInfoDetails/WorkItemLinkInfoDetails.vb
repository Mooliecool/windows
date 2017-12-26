'*************************** Module Header ******************************\
' Module Name:  WorkItemLinkInfoDetails.vb
' Project:      VBTFSWorkItemLinkInfoDetails
' Copyright (c) Microsoft Corporation.
' 
' The details of the WorkItemLinkInfoentry class. 
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'**************************************************************************

Imports System.Linq
Imports System.Text
Imports Microsoft.TeamFoundation.WorkItemTracking.Client

Public Class WorkItemLinkInfoDetails

    Private privateLinkInfo As WorkItemLinkInfo
    Public Property LinkInfo() As WorkItemLinkInfo
        Get
            Return privateLinkInfo
        End Get
        Private Set(ByVal value As WorkItemLinkInfo)
            privateLinkInfo = value
        End Set
    End Property

    Private privateSourceWorkItem As WorkItem
    Public Property SourceWorkItem() As WorkItem
        Get
            Return privateSourceWorkItem
        End Get
        Private Set(ByVal value As WorkItem)
            privateSourceWorkItem = value
        End Set
    End Property

    Private privateTargetWorkItem As WorkItem
    Public Property TargetWorkItem() As WorkItem
        Get
            Return privateTargetWorkItem
        End Get
        Private Set(ByVal value As WorkItem)
            privateTargetWorkItem = value
        End Set
    End Property

    Private privateLinkType As WorkItemLinkType
    Public Property LinkType() As WorkItemLinkType
        Get
            Return privateLinkType
        End Get
        Private Set(ByVal value As WorkItemLinkType)
            privateLinkType = value
        End Set
    End Property

    Public Sub New(ByVal linkInfo As WorkItemLinkInfo, ByVal sourceWorkItem As WorkItem, ByVal targetWorkItem As WorkItem, ByVal linkType As WorkItemLinkType)
        Me.LinkInfo = linkInfo
        Me.SourceWorkItem = sourceWorkItem
        Me.TargetWorkItem = targetWorkItem
        Me.LinkType = linkType
    End Sub

    ''' <summary>
    ''' Display the link as 
    ''' Source:[Source title] ==> LinkType:[Link Type] ==> Target:[Target title]
    ''' </summary>
    ''' <returns></returns>
    Public Overrides Function ToString() As String
        Return String.Format("Source:{0} ==> LinkType:{1} ==> Target:{2}", SourceWorkItem.Title, LinkType.ForwardEnd.Name, TargetWorkItem.Title)
    End Function
End Class

