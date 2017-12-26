'****************************** Module Header ******************************'
' Module Name:  Customer.vb
' Project:      VBWPFMasterDetailBinding
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates how to do master/detail data binding in WPF.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.Collections.ObjectModel
Imports System.ComponentModel

Namespace Data
    Class Customer
        Implements INotifyPropertyChanged
        Private _id As Integer
        Private _name As String
        Private _orders As New ObservableCollection(Of Order)()

        Public Property ID() As Integer
            Get
                Return _id
            End Get
            Set(ByVal value As Integer)
                _id = value
                OnPropertyChanged("ID")
            End Set
        End Property

        Public Property Name() As String
            Get
                Return _name
            End Get
            Set(ByVal value As String)
                _name = value
                OnPropertyChanged("Name")
            End Set
        End Property

        Public ReadOnly Property Orders() As ObservableCollection(Of Order)
            Get
                Return _orders
            End Get
        End Property

#Region "INotifyPropertyChanged Members"

        Public Event PropertyChanged As PropertyChangedEventHandler Implements INotifyPropertyChanged.PropertyChanged

        Public Sub OnPropertyChanged(ByVal name As String)
            RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(name))
        End Sub

#End Region
    End Class
End Namespace