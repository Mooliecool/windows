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


Imports System.Collections.ObjectModel
Imports System.Data.Services.Client
Imports System.Linq
Imports System.Windows
Imports System.Windows.Controls
Imports Microsoft.Maps.MapControl
Imports SilverlightClient.GeocodeServiceReference
Imports SilverlightClient.TravelDataServiceReference

Partial Public Class MainPage
    Inherits UserControl
    ' Your Bing Maps credential.
    Private _mapCredential As String = "[your credential]"
    Private _geocodeClient As New GeocodeServiceClient("CustomBinding_IGeocodeService")
    ' Since the Silverlight client is hosted in the same Web Role as the service,
    ' we'll use relative address.
    Private _dataServiceContext As New TravelDataServiceContext( _
        New Uri("../DataService/TravelDataService.svc", UriKind.Relative))
    Private _travelItems As New ObservableCollection(Of Travel)()

    Public Sub New()
        InitializeComponent()
        ' Display sign in link or welcome message based on identity.
        If App.IsAuthenticated Then
            Me.LoginLink.Visibility = System.Windows.Visibility.Collapsed
            Me.WelcomeTextBlock.Visibility = System.Windows.Visibility.Visible
            Me.WelcomeTextBlock.Text = App.WelcomeMessage
        Else
            Me.LoginLink.Visibility = System.Windows.Visibility.Visible
            Me.WelcomeTextBlock.Visibility = System.Windows.Visibility.Collapsed
        End If
        Me.map.CredentialsProvider = New ApplicationIdCredentialsProvider(Me._mapCredential)
        AddHandler Me._geocodeClient.ReverseGeocodeCompleted, New EventHandler( _
            Of ReverseGeocodeCompletedEventArgs)(AddressOf GeocodeClient_ReverseGeocodeCompleted)
        Me.LoginLink.NavigateUri = New Uri(Application.Current.Host.Source, "../LoginPage.aspx?returnpage=SilverlightClient.aspx")
    End Sub

    Private Sub Map_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Query the data.
        Me._dataServiceContext. _
            Travels.BeginExecute(Function(result)
                                     Me._travelItems = New ObservableCollection(Of Travel) _
(Me._dataServiceContext.Travels.EndExecute(result).ToList().OrderBy(Function(t) t.Time))
                                     Me.Dispatcher.BeginInvoke(New Action(Function()
                                                                              Me.mapItems.ItemsSource = Me._travelItems
                                                                              Me.placeList.ItemsSource = Me._travelItems
                                                                              Return Nothing
                                                                          End Function))
                                     Return Nothing
                                 End Function, Nothing)
    End Sub

    Private Sub map_MouseClick(ByVal sender As Object, ByVal e As Microsoft.Maps.MapControl.MapMouseEventArgs)
        ' Invoke Bing Maps Geocode service to obtain the nearest location.
        Dim request As New ReverseGeocodeRequest() With { _
         .Location = map.ViewportPointToLocation(e.ViewportPoint) _
        }
        request.Credentials = New Credentials() With { _
         .Token = Me._mapCredential _
        }
        _geocodeClient.ReverseGeocodeAsync(request)
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
            Me._travelItems.Add(travel)
            Me._dataServiceContext.AddObject("Travels", travel)
        End If
    End Sub

    Private Sub DatePicker_SelectedDateChanged(ByVal sender As Object, ByVal e As SelectionChangedEventArgs)
        Dim datePicker As DatePicker = DirectCast(sender, DatePicker)
        Dim travel As Travel = TryCast(datePicker.DataContext, Travel)
        If travel IsNot Nothing AndAlso travel.Time <> datePicker.SelectedDate.Value Then
            travel.Time = datePicker.SelectedDate.Value
            Me._dataServiceContext.UpdateObject(travel)
        End If
    End Sub

    ''' <summary>
    ''' Save changes.
    ''' </summary>
    Private Sub SaveButton_Click(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
        ' Our data service provider implementation doesn't support MERGE, so let's do a full update (PUT).
        Me._dataServiceContext.BeginSaveChanges( _
            SaveChangesOptions.ReplaceOnUpdate, _
            New AsyncCallback(Function(result)
                                  Dim response = Me._dataServiceContext.EndSaveChanges(result)
                                  Return Nothing
                              End Function), Nothing)
    End Sub

    ''' <summary>
    ''' Delete the item from data source.
    ''' </summary>
    Private Sub DeleteButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim button As HyperlinkButton = DirectCast(sender, HyperlinkButton)
        Dim travel As Travel = TryCast(button.DataContext, Travel)
        If travel IsNot Nothing Then
            Me._travelItems.Remove(travel)
            Me._dataServiceContext.DeleteObject(travel)
        End If
    End Sub
End Class