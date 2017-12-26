'***************************** Module Header ******************************\
'* Module Name:	MainPage.xaml.vb
'* Project:		AzureBingMaps
'* Copyright (c) Microsoft Corporation.
'* 
'* Code behind for MainPage.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************


Imports System.Windows
Imports System.Windows.Input
Imports WindowsPhoneClient.AzureBingMaps.DAL
Imports Microsoft.Phone.Controls
Imports Microsoft.Phone.Controls.Maps
Imports WindowsPhoneClient.GeocodeServiceReference

Partial Public Class MainPage
    Inherits PhoneApplicationPage
    ' Your Bing Maps credential.
    Private _mapCredential As String = "[your credential]"
    Private _geocodeClient As New GeocodeServiceClient()
    Private clickedPoint As Point

    ' Constructor
    Public Sub New()
        InitializeComponent()

        ' Set the data context of the listbox control to the sample data
        DataContext = App.ViewModel
        AddHandler Me.Loaded, New RoutedEventHandler(AddressOf MainPage_Loaded)
        Me.map.CredentialsProvider = New ApplicationIdCredentialsProvider(Me._mapCredential)
        AddHandler Me._geocodeClient.ReverseGeocodeCompleted, _
            New EventHandler(Of ReverseGeocodeCompletedEventArgs)(AddressOf GeocodeClient_ReverseGeocodeCompleted)
    End Sub

    ' Load data for the ViewModel Items
    Private Sub MainPage_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
        If Not App.ViewModel.IsDataLoaded Then
            App.ViewModel.LoadData()
        End If
        Me.mapItems.ItemsSource = App.DataSource.TravelItems
    End Sub

    Private Sub Map_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
        AddHandler App.DataSource.DataLoaded, New EventHandler(AddressOf DataSource_DataLoaded)
    End Sub

    Private Sub DataSource_DataLoaded(ByVal sender As Object, ByVal e As EventArgs)
        Me.Dispatcher.BeginInvoke(New Action(Function()
                                                 Me.mapItems.ItemsSource = App.DataSource.TravelItems
                                                 Return Nothing
                                             End Function))
    End Sub

    ''' <summary>
    ''' Callback method for the Bing Maps Geocode service.
    ''' </summary>
    Private Sub GeocodeClient_ReverseGeocodeCompleted(ByVal sender As Object, ByVal e As ReverseGeocodeCompletedEventArgs)
        If e.[Error] IsNot Nothing Then
            MessageBox.Show(e.[Error].Message)
        ElseIf e.Result.Results.Count > 0 Then
            ' We only care about the first result.
            Dim result = e.Result.Results(0)

            ' The PartitionKey represents the user.
            ' However, the client can send a fake identity as demonstrated below.
            ' To prevent clients faking the identity,
            ' our service always queries for the real identity on the service side.
            ' Latitude/Longitude is obtained from the service,
            ' so it may not be the exact clicked position.
            Dim travel As New Travel() With { _
              .PartitionKey = "fake@live.com", _
              .RowKey = Guid.NewGuid(), _
              .Place = result.DisplayName, _
              .Time = DateTime.Now, _
              .Latitude = result.Locations(0).Latitude, _
              .Longitude = result.Locations(0).Longitude _
            }
            ' Add to the ObservableCollection.
            App.DataSource.AddToTravel(travel)
        End If
    End Sub

    ''' <summary>
    ''' Windows Phone map control doesn't support Click,
    ''' so we have to handle MouseLeftButtonDown/Up.
    ''' These events will fire when the user touches the screen.
    ''' </summary>
    Private Sub map_MouseLeftButtonDown(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
        Me.clickedPoint = e.GetPosition(Me.map)
    End Sub

    Private Sub map_MouseLeftButtonUp(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
        ' Check to see if the finger has moved.
        Dim clickedPoint As Point = e.GetPosition(Me.map)
        If Math.Abs(clickedPoint.X - Me.clickedPoint.X) < 5 _
            AndAlso Math.Abs(clickedPoint.Y - Me.clickedPoint.Y) < 5 Then
            ' Invoke Bing Maps Geocode service to obtain the nearest location.
            Dim request As New ReverseGeocodeRequest() With { _
              .Location = map.ViewportPointToLocation(e.GetPosition(Me.map)) _
            }
            request.Credentials = New Microsoft.Phone.Controls.Maps.Credentials() With { _
              .ApplicationId = Me._mapCredential _
            }
            _geocodeClient.ReverseGeocodeAsync(request)
        End If
    End Sub

    ''' <summary>
    ''' Application bar event handler: Navigate to ListPage.
    ''' </summary>
    Private Sub ApplicationBarIconButton_Click(ByVal sender As Object, ByVal e As EventArgs)
        Me.NavigationService.Navigate(New Uri("/ListPage.xaml", UriKind.Relative))
    End Sub
End Class