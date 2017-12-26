'****************************** Module Header ******************************'
' Module Name:  Person.vb
' Project:      VBWPFDataBinding
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates how to use DataBinding in WPF
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.ComponentModel

Public Class Person
    Implements INotifyPropertyChanged
    Private m_name As String

    ''' <summary> 
    ''' Gets or sets the name. 
    ''' </summary> 
    ''' <value>The name.</value> 
    Public Property Name() As String
        Get
            Return m_name
        End Get
        Set(ByVal value As String)
            If value <> m_name Then
                m_name = value
                OnPropertyChanged("Name")
            End If
        End Set
    End Property
    Private m_age As Integer

    ''' <summary> 
    ''' Gets or sets the age. 
    ''' </summary> 
    ''' <value>The age.</value> 
    Public Property Age() As Integer
        Get
            Return m_age
        End Get
        Set(ByVal value As Integer)
            If value <> m_age Then
                m_age = value
                OnPropertyChanged("Age")
            End If
        End Set
    End Property
    Private m_job As String

    ''' <summary> 
    ''' Gets or sets the job. 
    ''' </summary> 
    ''' <value>The job.</value> 
    Public Property Job() As String
        Get
            Return m_job
        End Get
        Set(ByVal value As String)
            If value <> m_job Then
                m_job = value
                OnPropertyChanged("Job")
            End If
        End Set
    End Property

    Private m_salary As Integer

    ''' <summary> 
    ''' Gets or sets the salary. 
    ''' </summary> 
    ''' <value>The salary.</value> 
    Public Property Salary() As Integer
        Get
            Return m_salary
        End Get
        Set(ByVal value As Integer)
            If value <> m_salary Then
                m_salary = value
                OnPropertyChanged("Salary")
            End If
        End Set
    End Property


    Private m_interest As String

    ''' <summary> 
    ''' Gets or sets the interest. 
    ''' </summary> 
    ''' <value>The interest.</value> 
    Public Property Interest() As String
        Get
            Return m_interest
        End Get
        Set(ByVal value As String)
            If value <> m_interest Then
                m_interest = value
                OnPropertyChanged("Interest")

            End If
        End Set
    End Property

#Region "INotifyPropertyChanged Members"


    Public Event PropertyChanged As PropertyChangedEventHandler Implements INotifyPropertyChanged.PropertyChanged

#End Region

    ''' <summary> 
    ''' Fire the property changed event 
    ''' </summary> 
    ''' <param name="propertyName">Name of the property.</param> 
    Private Sub OnPropertyChanged(ByVal propertyName As String)
        RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(propertyName))
    End Sub

#Region "Methods"

    ''' <summary> 
    ''' Initializes a new instance of the <see cref="Person"/> class. 
    ''' </summary> 
    Public Sub New()
        Name = "Default"
        Job = "None"
        Age = 20
        Interest = "None"
        Salary = 500
    End Sub

#End Region
End Class