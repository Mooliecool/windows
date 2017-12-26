'***************************** Module Header ******************************\
'* Module Name:	DAL.vb
'* Project:		VBSL3DataBinding
'* Copyright (c) Microsoft Corporation.
'* 
'* This module contains Data Access Layer code and custom validation class. 
'* For the simplicity, it gets in-momory data.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 11/27/2009 11:00 AM Allen Chen Created
'\**************************************************************************
Imports System
Imports System.Net
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Documents
Imports System.Windows.Ink
Imports System.Windows.Input
Imports System.Windows.Media
Imports System.Windows.Media.Animation
Imports System.Windows.Shapes
Imports System.ComponentModel
Imports System.Collections.ObjectModel
Imports System.Collections
Imports System.ComponentModel.DataAnnotations

Namespace DAL
    Public Class CustomerGenerator
        Shared Sub New()
            _customer.ID = 1
            _customer.Name = "Allen Chen"
            _customercollection = New ObservableCollection(Of Customer)()
            Dim c1 As New Customer
            c1.ID = 1
            c1.Name = "Allen Chen"
            Dim c2 As New Customer
            c2.ID = 2
            c2.Name = "Mike Sun"
            Dim c3 As New Customer
            c3.ID = 3
            c3.Name = "Jialiang Ge"
            _customercollection.Add(c1)
            _customercollection.Add(c2)
            _customercollection.Add(c3)

        End Sub

        Private Shared _customercollection As ObservableCollection(Of Customer)
        Private Shared _customer As Customer = New Customer()
        Public Shared Function GetSingleCustomer() As Object
            Return _customer
        End Function

        Public Shared Function GetCustomersList() As IEnumerable
            Return _customercollection
        End Function

    End Class

    Public Class Customer
        Implements INotifyPropertyChanged
        Public _id As Integer
        ' Use custom validation class for this property
        <CustomValidation(GetType(MyValidation), "Validate")> _
        Public Property ID() As Integer
            Get
                Return _id
            End Get
            Set(ByVal value As Integer)

                Dim vc As ValidationContext = New ValidationContext(Me, Nothing, Nothing)
                vc.MemberName = "ID"
                Validator.ValidateProperty(value, vc)
                _id = value
                If Not PropertyChangedEvent Is Nothing Then
                    RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs("ID"))
                End If
            End Set
        End Property
        Private _name As String
        ' Use DataAnnotation to restrict the length of Name property
        <StringLength(30, MinimumLength:=1, ErrorMessage:="Name must be between 1 and 30 characters long.")> _
        Public Property Name() As String
            Get
                Return _name
            End Get
            Set(ByVal value As String)

                Dim vc As ValidationContext = New ValidationContext(Me, Nothing, Nothing)
                vc.MemberName = "Name"
                Validator.ValidateProperty(value, vc)
                _name = value
                If Not PropertyChangedEvent Is Nothing Then
                    RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs("Name"))
                End If
            End Set
        End Property

#Region "INotifyPropertyChanged Members"

        Event PropertyChanged As PropertyChangedEventHandler Implements INotifyPropertyChanged.PropertyChanged

#End Region

    End Class

    Public Class MyValidation
        Private Sub New()
        End Sub
        Public Shared Function Validate(ByVal [property] As Object, ByVal context As ValidationContext) As ValidationResult
            ' You can get Property name from context.MemberName. It's not used here for simplicity.
            Dim customerid As Integer
            ' For the simplicity, the change is invalid if new ID equals 11.
            If Int32.TryParse([property].ToString(), customerid) AndAlso customerid = 11 Then
                Dim validationResult As ValidationResult = New ValidationResult("Custom Validation Failed. ID cannot be 11")
                Return validationResult
            Else
                Return ValidationResult.Success
            End If
        End Function
    End Class
End Namespace
