'****************************** Module Header ******************************
' Module Name:                MainPage.xaml.vb
' Project:                    VBSL4DataFormCancelButton
' Copyright (c) Microsoft Corporation.
' 
' MainPage's code hehind file.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************

Imports System.Collections.ObjectModel
Imports System.ComponentModel
Imports System.ComponentModel.DataAnnotations

Partial Public Class MainPage
    Inherits UserControl

    Public Sub New()
        InitializeComponent()
        AddHandler Loaded, AddressOf MainPage_Loaded

    End Sub

    Private Sub MainPage_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
        myDataForm.ItemsSource = New Employee()
    End Sub

End Class

' Your class inherit from IEditableObject to achieve the purpose
Public Class EmployeeItem
    Implements INotifyPropertyChanged
    Implements IEditableObject
    Private Structure ItemData
        Friend EmployeName As String
        Friend Gender As String
        Friend Age As Integer
        Friend OnboardDate As DateTime
    End Structure

    Private originalItem As ItemData
    Private currentItem As ItemData

    Public Sub New()
        Me.New("Employee Name", 25, "Male", DateTime.Now)
    End Sub

    Public Sub New(ByVal empName As String, ByVal empAge As Integer, ByVal empGender As String, ByVal empOnboardDate As DateTime)
        EmployeeName = empName
        Gender = empGender
        Age = empAge
        OnboardDate = empOnboardDate
    End Sub

    Public Property EmployeeName() As String
        Get
            Return currentItem.EmployeName
        End Get
        Set(ByVal value As String)
            If currentItem.EmployeName <> value Then
                currentItem.EmployeName = value
                NotifyPropertyChanged("EmployeName")
            End If
        End Set
    End Property

    Public Property Gender() As String
        Get
            Return currentItem.Gender
        End Get
        Set(ByVal value As String)
            If currentItem.Gender <> value Then
                currentItem.Gender = value
                NotifyPropertyChanged("Gender")
            End If
        End Set
    End Property


    <Range(10, 80, ErrorMessage:="Employee's Age range should within 10 to 80")> _
    <Display(Name:="Age", Description:="Employee's Age")> _
    Public Property Age() As Integer
        Get
            Return currentItem.Age
        End Get
        Set(ByVal value As Integer)
            If currentItem.Age <> value Then
                Validator.ValidateProperty(value, New ValidationContext(Me, Nothing, Nothing) With { _
                  .MemberName = "Age" _
                })
                currentItem.Age = value
                NotifyPropertyChanged("Age")
            End If
        End Set
    End Property

    Public Property OnboardDate() As DateTime
        Get
            Return currentItem.OnboardDate
        End Get
        Set(ByVal value As DateTime)
            If value <> currentItem.OnboardDate Then
                currentItem.OnboardDate = value
                NotifyPropertyChanged("OnboardDate")
            End If
        End Set
    End Property


#Region "INotifyPropertyChanged Members"

    Public Event PropertyChanged As PropertyChangedEventHandler _
        Implements INotifyPropertyChanged.PropertyChanged

    Private Sub NotifyPropertyChanged(ByVal info As String)
        RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(info))
    End Sub

#End Region

#Region "IEditableObject Members"

    ' Copy the original value before editing
    Public Sub BeginEdit() Implements IEditableObject.BeginEdit

        originalItem = currentItem
    End Sub

    ' Restore the original value if edit operation is cancelled.
    Public Sub CancelEdit() Implements IEditableObject.CancelEdit
        currentItem = originalItem
        NotifyPropertyChanged("")

    End Sub

    Public Sub EndEdit() Implements IEditableObject.EndEdit

    End Sub

#End Region

End Class

Public Class Employee
    Inherits ObservableCollection(Of EmployeeItem)
    Public Sub New()
        Add((New EmployeeItem("Steven", 20, "Male", New DateTime(2010, 9, 1))))
        Add((New EmployeeItem("Vivian", 26, "Female", New DateTime(2008, 5, 1))))
        Add((New EmployeeItem("Bill", 28, "Male", New DateTime(2006, 2, 2))))
        Add((New EmployeeItem("Janney", 30, "Female", New DateTime(2004, 3, 10))))
        Add((New EmployeeItem("Bob", 40, "Male", New DateTime(2009, 5, 27))))
        Add((New EmployeeItem("Jonathan", 27, "Male", New DateTime(2000, 10, 25))))
    End Sub

End Class
