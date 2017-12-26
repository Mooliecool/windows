'***************************** Module Header ******************************\
'* Module Name:	TableStoragePagingUtility.vb
'* Project:		AzureTableStoragePaging
'* Copyright (c) Microsoft Corporation.
'* 
'* This class can be reused for other applications. If you want 
'* to reuse the code, what you need to do is to implement custom ICachedDataProvider<T> 
'* class to store data required by TableStoragePagingUtility<T>.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************
Imports Microsoft.WindowsAzure
Imports Microsoft.WindowsAzure.StorageClient

Namespace Utilities
    Public Class TableStoragePagingUtility(Of T)
        Private _cloudStorageAccount As CloudStorageAccount
        Private _provider As ICachedDataProvider(Of T)
        Private _tableServiceContext As TableServiceContext
        Private Property RC() As ResultContinuation
            Get
                Return _provider.GetResultContinuation()
            End Get
            Set(ByVal value As ResultContinuation)
                _provider.SetResultContinuation(value)
            End Set
        End Property
        Private _entitySetName As String
        Private _pageSize As Integer
        Public Property CurrentPageIndex() As Integer
            Get
                Return _provider.GetCurrentIndex()
            End Get
            Private Set(ByVal value As Integer)
                _provider.SetCurrentIndex(value)
            End Set
        End Property
        Public Property PageSize() As Integer
            Get
                Return _pageSize
            End Get
            Private Set(ByVal value As Integer)
                _pageSize = value
            End Set
        End Property

        Public Sub New(ByVal provider As ICachedDataProvider(Of T), ByVal cloudStorageAccount As CloudStorageAccount, ByVal tableServiceContext As TableServiceContext, ByVal pageSize As Integer, ByVal entitySetName As String)
            Me._provider = Provider
            Me._cloudStorageAccount = CloudStorageAccount
            Me._entitySetName = entitySetName
            Me._tableServiceContext = TableServiceContext
            If PageSize <= 0 Then
                Throw New IndexOutOfRangeException("pageSize out of range")
            End If
            Me.PageSize = PageSize
        End Sub

        ''' <summary>
        ''' Get the next page
        ''' </summary>
        ''' <returns>The next page. If current page is the last page it returns the last page.</returns>
        Public Function GetNextPage() As IEnumerable(Of T)
            ' Get cached data
            Dim cachedData = Me._provider.GetCachedData()
            Dim pageCount As Integer = 0
            If Not cachedData Is Nothing Then
                pageCount = Convert.ToInt32(Math.Ceiling(CDbl(cachedData.Count()) / CDbl(Me.PageSize)))
            End If
            ' If there still has entities in table storage to read and the current page is the last page,
            ' request table storage to get new data.
            If (Not Me._provider.HasReachedEnd) AndAlso CurrentPageIndex = pageCount - 1 Then
                Dim q = Me._tableServiceContext.CreateQuery(Of T)(Me._entitySetName).Take(PageSize).AsTableServiceQuery()
                Dim r As IAsyncResult
                r = q.BeginExecuteSegmented(RC, Function(ar) 1 = 1, q)
                r.AsyncWaitHandle.WaitOne()
                Dim result As ResultSegment(Of T) = q.EndExecuteSegmented(r)
                Dim results = result.Results
                Me._provider.AddDataToCache(results)
                ' If there's any entity returns we need to increase pageCount
                If results.Count() > 0 Then
                    pageCount += 1
                End If
                RC = result.ContinuationToken
                ' If the returned token is null it means there's no more entities in table
                If result.ContinuationToken Is Nothing Then
                    Me._provider.HasReachedEnd = True
                End If
            End If
            If CurrentPageIndex + 1 < pageCount Then
                CurrentPageIndex = CurrentPageIndex + 1
            Else
                CurrentPageIndex = pageCount - 1
            End If
            If cachedData Is Nothing Then
                cachedData = Me._provider.GetCachedData()
            End If
            Return cachedData.Skip((Me.CurrentPageIndex) * Me.PageSize).Take(Me.PageSize)
        End Function

        ''' <summary>
        ''' Get the previous page
        ''' </summary>
        ''' <returns>The previous page. If current page is the first page it returns the first page. If there's no data in cache,
        ''' returns an empty collection/></returns>
        Public Function GetPreviousPage() As IEnumerable(Of T)

            Dim cachedData = Me._provider.GetCachedData()
            If Not cachedData Is Nothing AndAlso cachedData.Count() > 0 Then
                If CurrentPageIndex - 1 < 0 Then
                    CurrentPageIndex = 0
                Else
                    CurrentPageIndex = CurrentPageIndex - 1
                End If
                Return cachedData.Skip(Me.CurrentPageIndex * Me.PageSize).Take(Me.PageSize)
            Else
                Return New List(Of T)()
            End If
        End Function

        ''' <summary>
        ''' If there're entities cached, return the current page. Else retrieve table storage and
        ''' return the first page.
        ''' </summary>
        ''' <returns>The current page if there're entities cached. If there's no data cached first
        ''' page will be retrieved from table storage and returned.</returns>
        Public Function GetCurrentOrFirstPage() As IEnumerable(Of T)
            Dim cachedData = Me._provider.GetCachedData()
            If Not cachedData Is Nothing AndAlso cachedData.Count() > 0 Then
                Return cachedData.Skip(Me.CurrentPageIndex * Me.PageSize).Take(Me.PageSize)
            Else
                Return GetNextPage()
            End If
        End Function
    End Class

    ''' <summary>
    ''' The class implements this interface must take the responsibility of cache storage, including
    ''' data, ResultContinuation and HasReachedEnd flag.
    ''' </summary>
    ''' <typeparam name="T"></typeparam>
    Public Interface ICachedDataProvider(Of T)
        ''' <summary>
        ''' Return all cached data
        ''' </summary>
        ''' <returns></returns>
        Function GetCachedData() As IEnumerable(Of T)
        ''' <summary>
        ''' Save data to cache
        ''' </summary>
        ''' <param name="data">Data that user of this provider wants to add to cache</param>
        Sub AddDataToCache(ByVal data As IEnumerable(Of T))
        ''' <summary>
        ''' Save Current index
        ''' </summary>
        ''' <param name="index">Current page index sent from user of this provider</param>
        Sub SetCurrentIndex(ByVal index As Integer)
        ''' <summary>
        ''' Get Current index
        ''' </summary>
        ''' <returns>Current page index preserved in cache</returns>
        Function GetCurrentIndex() As Integer
        ''' <summary>
        ''' Set continuation token
        ''' </summary>
        ''' <param name="rc">ResultContinuation sent from user of this provider</param>
        Sub SetResultContinuation(ByVal rc As ResultContinuation)
        ''' <summary>
        ''' Get continuation token
        ''' </summary>
        ''' <returns>ResultContinuation preserved in cache</returns>
        Function GetResultContinuation() As ResultContinuation
        ''' <summary>
        ''' A flag tells the user of this provider whether he can fully rely on cache without
        ''' the need to fetch new data from table storage.
        ''' </summary>
        Property HasReachedEnd() As Boolean
    End Interface

    ''' <summary>
    ''' A sample implementation of ICachedDataProvider<T> that caches data in session for MVC
    ''' applications. Because the implementation of it uses Session of MVC the user of this class
    ''' need to be educated not to use reserved keywords of this class in their other session variables,
    ''' (such as one starts with "currentindex"). If they have to use it they can specify a special id
    ''' to distinguish them.
    ''' </summary>
    ''' <typeparam name="T"></typeparam>
    Public Class MVCSessionCachedDataProvider(Of T)
        Implements ICachedDataProvider(Of T)
        Private _session As HttpSessionStateBase
        Private _id As String
        ''' <summary>
        ''' Constructor
        ''' </summary>
        ''' <param name="c">Generally specify this for this parameter</param>
        ''' <param name="id">The id of the cache provider. You need to use the same id to access
        ''' the same cache store</param>
        Public Sub New(ByVal c As Controller, ByVal id As String)
            _session = c.Session
            _id = id
            ' Initialize currentindex
            If _session("currentindex" & Me._id) Is Nothing Then
                _session("currentindex" & Me._id) = -1
            End If
            ' Initialize hasreachedend flag to indicate whether there's no need to retrieve new data
            If _session("hasreachedend" & Me._id) Is Nothing Then
                _session("hasreachedend" & Me._id) = False
            End If
        End Sub
        Public Function GetCachedData() As IEnumerable(Of T) Implements ICachedDataProvider(Of T).GetCachedData
            Return TryCast(_session("inmemorycacheddata" & Me._id), List(Of T))
        End Function
        Public Sub AddDataToCache(ByVal data As IEnumerable(Of T)) Implements ICachedDataProvider(Of T).AddDataToCache
            Dim inmemorycacheddata = TryCast(_session("inmemorycacheddata" & Me._id), List(Of T))
            If inmemorycacheddata Is Nothing Then
                inmemorycacheddata = New List(Of T)()
            End If
            inmemorycacheddata.AddRange(data)
            _session("inmemorycacheddata" & Me._id) = inmemorycacheddata
        End Sub
        Public Sub SetCurrentIndex(ByVal index As Integer) Implements ICachedDataProvider(Of T).SetCurrentIndex
            _session("currentindex" & Me._id) = index
        End Sub
        Public Function GetCurrentIndex() As Integer Implements ICachedDataProvider(Of T).GetCurrentIndex
            Return Convert.ToInt32(_session("currentindex" & Me._id))
        End Function
        Public Function GetResultContinuation() As ResultContinuation Implements ICachedDataProvider(Of T).GetResultContinuation
            Return TryCast(_session("resultcontinuation" & Me._id), ResultContinuation)
        End Function
        Public Sub SetResultContinuation(ByVal rc As ResultContinuation) Implements ICachedDataProvider(Of T).SetResultContinuation
            _session("resultcontinuation" & Me._id) = rc
        End Sub

        Public Property HasReachedEnd() As Boolean Implements ICachedDataProvider(Of T).HasReachedEnd
            Get
                Return Convert.ToBoolean(_session("hasreachedend" & Me._id))
            End Get
            Set(ByVal value As Boolean)
                _session("hasreachedend" & Me._id) = value
            End Set
        End Property


    End Class
End Namespace

