'***************************** Module Header ******************************\
'* Module Name:	GeneralScenarios.xaml.vb
'* Project:		VBSL3DataBinding
'* Copyright (c) Microsoft Corporation.
'* 
'* This module demonstrates general scenarios of Data Binding in SL3
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 11/26/2009 06:00 PM Allen Chen Created
'\**************************************************************************

Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Net
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Documents
Imports System.Windows.Input
Imports System.Windows.Media
Imports System.Windows.Media.Animation
Imports System.Windows.Shapes
Imports System.Windows.Data
Imports System.Collections
Imports System.Text
Imports System.Collections.ObjectModel
Imports System.ComponentModel.DataAnnotations
Imports VBSL3DataBinding.DAL

Partial Public Class GeneralScenarios
    Inherits UserControl
    Dim _customer As Object = CustomerGenerator.GetSingleCustomer()
    ' _newcustomer caches the customer pending to insert
    Dim _newcustomer As Object
    Dim customercollection As IEnumerable = CustomerGenerator.GetCustomersList()


    Public Sub New()

        _newcustomer = New Customer()
        _newcustomer.ID = 4
        _newcustomer.Name = "Vince Xu"

        InitializeComponent()

        ' Init DataContext and ItemsSource
        Me.BindingModeStackPanel.DataContext = _customer
        Me.ConverterScenarioDataGrid.ItemsSource = customercollection
        Me.InsertStackPanel.DataContext = _newcustomer
    End Sub

    Private Sub Button_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' In TwoWay bindings, changes to the target automatically update the source, except when binding to the Text property of a TextBox. 
        ' In this case, the update occurs when the TextBox loses focus. 
        ' You can refer to http://forums.silverlight.net/forums/t/11547.aspx
        ' for workarounds.
    End Sub

    ''' <summary>
    ''' This is an event handler we can hook for notification of validation errors.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub BindingModeStackPanel_BindingValidationError(ByVal sender As Object, ByVal e As ValidationErrorEventArgs)
        If e.Action = ValidationErrorEventAction.Added Then
            MessageBox.Show(e.Error.ErrorContent.ToString())
        Else
            ' Error has been fixed. You can do additional works here.
        End If
    End Sub

    ''' <summary>
    ''' Add a new Customer object to customercollection
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub Button_Click_1(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Try
            Dim id As Integer = Int32.Parse(Me.InsertIDTextBox.Text)
            Dim name As String = Me.InsertNameTextBox.Text
            ' To notify collection elements' change, collection class should implement INotifyCollectionChanged.
            ' It's recommended to use ObservableCollection rather than implementing INotifyCollectionChanged on your own.
            Dim collection As ObservableCollection(Of Customer) = TryCast(Me.customercollection, ObservableCollection(Of Customer))
            If Not collection Is Nothing Then
                Dim newcustomer As Customer = New Customer()
                newcustomer.ID = id
                newcustomer.Name = name
                collection.Add(newcustomer)
            End If
        Catch ex As Exception
        End Try
    End Sub
End Class

''' <summary>
''' This is a custom ValueConverter class. It converts int to Brush. 
''' If ID is larger than 1 a redbrush will be returned.
''' </summary>
Public Class MyConverter
    Implements IValueConverter
    Private redbrush As Brush = New SolidColorBrush(Color.FromArgb(255, 255, 0, 0))
    Public Function Convert(ByVal value As Object, ByVal targetType As Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements IValueConverter.Convert

        If TypeOf value Is Integer Then

            If CInt(Fix(value)) > 1 Then
                Return redbrush
            Else
                Return Nothing
            End If
        Else
            Return Nothing
        End If
    End Function

    Public Function ConvertBack(ByVal value As Object, ByVal targetType As Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements IValueConverter.ConvertBack
        Throw New NotImplementedException()
    End Function
End Class