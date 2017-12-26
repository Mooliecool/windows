'*************************** Module Header ******************************'
' Module Name:  WorkItemLinkQuery.vb
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
'*************************************************************************'

Imports System.Net
Imports Microsoft.TeamFoundation.Client
Imports Microsoft.TeamFoundation.WorkItemTracking.Client

Public Class WorkItemLinkQuery
    Implements IDisposable

    ' The query format.
    Private Const _queryFormat As String =
        "select * from WorkItemLinks where [Source].[System.ID] = {0}"

    Private _disposed As Boolean = False

    Private _linkTypes As Dictionary(Of Integer, WorkItemLinkType)

    ' The dictionary to store the ID and WorkItemLinkType KeyValuePair.
    Public ReadOnly Property LinkTypes() As Dictionary(Of Integer, WorkItemLinkType)
        Get
            ' Get all WorkItemLinkType from WorkItemStore.
            If _linkTypes Is Nothing Then
                _linkTypes = New Dictionary(Of Integer, WorkItemLinkType)()
                For Each type In Me.WorkItemStore.WorkItemLinkTypes
                    _linkTypes.Add(type.ForwardEnd.Id, type)
                Next type
            End If
            Return _linkTypes
        End Get
    End Property

    ''' <summary>
    ''' TFS Team Project Collection.
    ''' </summary>
    Private _projectCollection As TfsTeamProjectCollection
    Public Property ProjectCollection() As TfsTeamProjectCollection
        Get
            Return _projectCollection
        End Get
        Private Set(ByVal value As TfsTeamProjectCollection)
            _projectCollection = value
        End Set
    End Property

    ''' <summary>
    ''' WorkItemStore of the Team Project Collection.
    ''' </summary>
    Private _workItemStore As WorkItemStore
    Public Property WorkItemStore() As WorkItemStore
        Get
            Return _workItemStore
        End Get
        Private Set(ByVal value As WorkItemStore)
            _workItemStore = value
        End Set
    End Property

    ''' <summary>
    ''' Use the default credentials to initialize this instance.
    ''' </summary>
    Public Sub New(ByVal collectionUri As Uri)
        Me.New(collectionUri, CredentialCache.DefaultCredentials)
    End Sub

    ''' <summary>
    ''' Initialize this instance.
    ''' </summary>
    Public Sub New(ByVal collectionUri As Uri, ByVal credential As ICredentials)
        If collectionUri Is Nothing Then
            Throw New ArgumentNullException("collectionUrl")
        End If

        ' If the credential failed, an UICredentialsProvider instance will be launched.
        Me.ProjectCollection = New TfsTeamProjectCollection(
            collectionUri, credential, New UICredentialsProvider())
        Me.ProjectCollection.EnsureAuthenticated()

        ' Get the WorkItemStore service.
        Me.WorkItemStore = Me.ProjectCollection.GetService(Of WorkItemStore)()
    End Sub

    ''' <summary>
    ''' Get the WorkItemLinkInfoDetails of a work item.
    ''' </summary>
    Public Function GetWorkItemLinkInfos(ByVal workitemID As Integer) _
        As IEnumerable(Of WorkItemLinkInfoDetails)

        ' Construct the WIQL.
        Dim queryStr As String = String.Format(_queryFormat, workitemID)

        Dim linkQuery As New Query(Me.WorkItemStore, queryStr)

        ' Get all WorkItemLinkInfo objects.
        Dim linkinfos() As WorkItemLinkInfo = linkQuery.RunLinkQuery()

        ' Get WorkItemLinkInfoDetails from the  WorkItemLinkInfo objects.
        Dim detailsList As New List(Of WorkItemLinkInfoDetails)()
        For Each linkinfo In linkinfos
            If linkinfo.LinkTypeId <> 0 Then
                Dim details As WorkItemLinkInfoDetails =
                    GetDetailsFromWorkItemLinkInfo(linkinfo)
                Console.WriteLine(details.ToString())
            End If
        Next linkinfo
        Return detailsList
    End Function

    ''' <summary>
    ''' Get WorkItemLinkInfoDetails from the  WorkItemLinkInfo object.
    ''' </summary>
    Public Function GetDetailsFromWorkItemLinkInfo(ByVal linkInfo As WorkItemLinkInfo) _
        As WorkItemLinkInfoDetails

        If Me.LinkTypes.ContainsKey(linkInfo.LinkTypeId) Then
            Dim details As New WorkItemLinkInfoDetails(
                linkInfo,
                Me.WorkItemStore.GetWorkItem(linkInfo.SourceId),
                Me.WorkItemStore.GetWorkItem(linkInfo.TargetId),
                Me.LinkTypes(linkInfo.LinkTypeId))
            Return details
        Else
            Throw New ApplicationException("Cannot find WorkItemLinkType!")
        End If
    End Function

    Public Sub Dispose() Implements IDisposable.Dispose
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub

    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        ' Protect from being called multiple times.
        If _disposed Then
            Return
        End If

        If disposing Then
            If Me.ProjectCollection IsNot Nothing Then
                Me.ProjectCollection.Dispose()
            End If
            _disposed = True
        End If
    End Sub

End Class
