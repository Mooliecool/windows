'***************************** Module Header ******************************\
'* Module Name:	DataSource.vb
'* Project:		AzureBingMaps
'* Copyright (c) Microsoft Corporation.
'* 
'* The data source used by both MainPage and ListPage.
'* Wraps calls to WCF Data Services.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************


Imports System.Collections.ObjectModel
Imports System.Data.Services.Client
Imports System.Linq
Imports WindowsPhoneClient.AzureBingMaps.DAL

''' <summary>
''' The data source used by both MainPage and ListPage.
''' Wraps calls to WCF Data Services.
''' </summary>
Public Class DataSource
    ' Windows Phone applications cannot be hosted in Windows Azure.
    ' So we have to use absolute address.
    ' Use http://127.0.0.1:81/DataService/TravelDataService.svc/ if you're using Compute Emulator. Otherwise, use your own Windows Azure service address.
    Private _dataServiceContext As New TravelDataServiceContext(New Uri("http://127.0.0.1:81/DataService/TravelDataService.svc/"))
    Private _travelItems As New ObservableCollection(Of Travel)()
    Public Event DataLoaded As EventHandler

    Public ReadOnly Property TravelItems() As ObservableCollection(Of Travel)
        Get
            Return Me._travelItems
        End Get
    End Property

    ''' <summary>
    ''' Query the data.
    ''' </summary>
    Public Sub LoadDataAsync()
        Me._dataServiceContext.BeginExecute(Of Travel)( _
            New Uri(Me._dataServiceContext.BaseUri, "Travels"), _
            Function(result)
                Dim results = Me._dataServiceContext.EndExecute(Of Travel)(result).ToList().OrderBy(Function(t) t.Time)
                Me._travelItems = New ObservableCollection(Of Travel)()
                For Each item In results
                    Me._travelItems.Add(item)
                Next
                RaiseEvent DataLoaded(Me, EventArgs.Empty)
                Return Nothing
            End Function, Nothing)
    End Sub

    Public Sub AddToTravel(ByVal travel As Travel)
        Me._travelItems.Add(travel)
        Me._dataServiceContext.AddObject("Travels", travel)
    End Sub

    Public Sub UpdateTravel(ByVal travel As Travel)
        Me._dataServiceContext.UpdateObject(travel)
    End Sub

    Public Sub RemoveFromTravel(ByVal travel As Travel)
        Me._travelItems.Remove(travel)
        Me._dataServiceContext.DeleteObject(travel)
    End Sub

    Public Sub SaveChanges()
        ' Our data service provider implementation doesn't support MERGE, so let's do a full update (PUT).
        Me._dataServiceContext.BeginSaveChanges(SaveChangesOptions.ReplaceOnUpdate, _
                                                New AsyncCallback( _
                                                    Function(result)
                                                        Dim response = Me._dataServiceContext.EndSaveChanges(result)
                                                        Return Nothing
                                                    End Function), Nothing)
    End Sub
End Class