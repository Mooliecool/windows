'****************************** Module Header ******************************'
' Module Name:  DataModel.vb
' Project:      VBWPFAnimationWhenDataChanged
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates how to trigger animation when the value of the 
' datagrid cell is changed.
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
Imports System.Collections.ObjectModel

'datamodel used by the application, fetched from msdn 
Public Class NameList
    Inherits ObservableCollection(Of PersonName)
    Public Sub New()
        MyBase.New()
        Add(New PersonName("Willa", "Cather"))
        Add(New PersonName("Isak", "Dinesen"))
        Add(New PersonName("Victor", "Hugo"))
        Add(New PersonName("Jules", "Verne"))
    End Sub
End Class

Public Class PersonName
    Implements INotifyPropertyChanged
    Private m_firstName As String
    Private m_lastName As String
    ''' <summary> 
    ''' Initializes a new instance of the <see cref="PersonName"/> class. 
    ''' </summary> 
    ''' <param name="first">firstName.</param> 
    ''' <param name="last">lastName.</param> 
    Public Sub New(ByVal first As String, ByVal last As String)
        Me.m_firstName = first
        Me.m_lastName = last
    End Sub
    ''' <summary> 
    ''' Gets or sets the firstName. 
    ''' </summary> 
    ''' <value>The first name.</value> 
    Public Property FirstName() As String
        Get
            Return m_firstName
        End Get
        Set(ByVal value As String)
            m_firstName = value
            OnPropertyChanged("FirstName")
        End Set
    End Property

    ''' <summary> 
    ''' Gets or sets the lastName. 
    ''' </summary> 
    ''' <value>The last name.</value> 
    Public Property LastName() As String
        Get
            Return m_lastName
        End Get
        Set(ByVal value As String)
            m_lastName = value
            OnPropertyChanged("LastName")
        End Set
    End Property

#Region "INotifyPropertyChanged Members"
    Public Event PropertyChanged As PropertyChangedEventHandler Implements INotifyPropertyChanged.PropertyChanged
#End Region
    ''' <summary> 
    ''' Called when [property changed]. 
    ''' </summary> 
    ''' <param name="propertyName">Name of the property.</param> 
    Private Sub OnPropertyChanged(ByVal propertyName As String)
        RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(propertyName))
    End Sub
End Class