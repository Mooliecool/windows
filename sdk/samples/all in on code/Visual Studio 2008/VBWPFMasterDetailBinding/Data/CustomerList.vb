'****************************** Module Header ******************************'
' Module Name:  CustomerList.vb
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

Namespace Data
    Class CustomerList
        Private _customers As ObservableCollection(Of Customer)

        Public Sub New()
            _customers = New ObservableCollection(Of Customer)()

            ' Insert customer and corresponding order information into 
            Dim c As New Customer() With {.ID = 1, .Name = "Customer1"}
            c.Orders.Add(New Order() With {.ID = 1, .Date = New DateTime(2009, 1, 1), .ShipCity = "Shanghai"})
            c.Orders.Add(New Order() With {.ID = 1, .Date = New DateTime(2009, 2, 1), .ShipCity = "Beijing"})
            c.Orders.Add(New Order() With {.ID = 1, .Date = New DateTime(2009, 11, 10), .ShipCity = "Guangzhou"})
            _customers.Add(c)

            c = New Customer() With {.ID = 2, .Name = "Customer2"}
            c.Orders.Add(New Order() With {.ID = 1, .Date = New DateTime(2009, 1, 1), .ShipCity = "New York"})
            c.Orders.Add(New Order() With {.ID = 1, .Date = New DateTime(2009, 2, 1), .ShipCity = "Seattle"})
            _customers.Add(c)

            c = New Customer() With {.ID = 3, .Name = "Customer3"}
            c.Orders.Add(New Order() With {.ID = 1, .Date = New DateTime(2009, 1, 1), .ShipCity = "Xiamen"})
            c.Orders.Add(New Order() With {.ID = 1, .Date = New DateTime(2009, 2, 1), .ShipCity = "Shenzhen"})
            c.Orders.Add(New Order() With {.ID = 1, .Date = New DateTime(2009, 11, 10), .ShipCity = "Tianjin"})
            c.Orders.Add(New Order() With {.ID = 1, .Date = New DateTime(2009, 11, 10), .ShipCity = "Wuhan"})
            c.Orders.Add(New Order() With {.ID = 1, .Date = New DateTime(2009, 11, 10), .ShipCity = "Jinan"})
            _customers.Add(c)

            c = New Customer() With {.ID = 4, .Name = "Customer4"}
            c.Orders.Add(New Order() With {.ID = 1, .Date = New DateTime(2009, 1, 1), .ShipCity = "Lanzhou"})
            _customers.Add(c)
        End Sub

        Public ReadOnly Property Customers() As ObservableCollection(Of Customer)
            Get
                Return _customers
            End Get
        End Property
    End Class
End Namespace