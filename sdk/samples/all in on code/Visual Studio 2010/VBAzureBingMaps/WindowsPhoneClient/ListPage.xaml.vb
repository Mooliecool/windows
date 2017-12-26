'***************************** Module Header ******************************\
'* Module Name:	ListPage.xaml.vb
'* Project:		AzureBingMaps
'* Copyright (c) Microsoft Corporation.
'* 
'* Code behind for ListPage.
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
Imports System.Windows.Controls
Imports WindowsPhoneClient.AzureBingMaps.DAL
Imports Microsoft.Phone.Controls

Partial Public Class ListPage
    Inherits PhoneApplicationPage
    Public Sub New()
        InitializeComponent()
    End Sub

    Private Sub PhoneApplicationPage_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.placeList.ItemsSource = App.DataSource.TravelItems
    End Sub

    ''' <summary>
    ''' When the DatePicker's value is changed, update the data source.
    ''' </summary>
    Private Sub DatePicker_ValueChanged(ByVal sender As Object, ByVal e As DateTimeValueChangedEventArgs)
        Dim datePicker As DatePicker = DirectCast(sender, DatePicker)
        Dim travel As Travel = TryCast(datePicker.DataContext, Travel)
        If travel IsNot Nothing AndAlso travel.Time <> datePicker.Value Then
            travel.Time = datePicker.Value.Value
            App.DataSource.UpdateTravel(travel)
        End If
    End Sub

    ''' <summary>
    ''' Delete the item from data source.
    ''' </summary>
    Private Sub DeleteButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim button As HyperlinkButton = DirectCast(sender, HyperlinkButton)
        Dim travel As Travel = TryCast(button.DataContext, Travel)
        If travel IsNot Nothing Then
            App.DataSource.RemoveFromTravel(travel)
        End If
    End Sub

    ''' <summary>
    ''' Save changes.
    ''' </summary>
    Private Sub SaveButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        App.DataSource.SaveChanges()
    End Sub

    ''' <summary>
    ''' Application bar event handler: Navigate to MainPage.
    ''' </summary>
    Private Sub ApplicationBarIconButton_Click(ByVal sender As Object, ByVal e As EventArgs)
        Me.NavigationService.Navigate(New Uri("/MainPage.xaml", UriKind.Relative))
    End Sub
End Class