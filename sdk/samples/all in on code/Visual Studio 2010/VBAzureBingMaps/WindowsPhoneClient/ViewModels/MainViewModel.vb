Imports System.ComponentModel
Imports System.Collections.Generic
Imports System.Diagnostics
Imports System.Text
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Data
Imports System.Windows.Documents
Imports System.Windows.Input
Imports System.Windows.Media
Imports System.Windows.Media.Imaging
Imports System.Windows.Shapes
Imports System.Collections.ObjectModel


Public Class MainViewModel
    Implements INotifyPropertyChanged
    Public Sub New()
        Me.Items = New ObservableCollection(Of ItemViewModel)()
    End Sub

    ''' <summary>
    ''' A collection for ItemViewModel objects.
    ''' </summary>
    Public Property Items() As ObservableCollection(Of ItemViewModel)
        Get
            Return m_Items
        End Get
        Private Set(ByVal value As ObservableCollection(Of ItemViewModel))
            m_Items = value
        End Set
    End Property
    Private m_Items As ObservableCollection(Of ItemViewModel)

    Private _sampleProperty As String = "Sample Runtime Property Value"
    ''' <summary>
    ''' Sample ViewModel property; this property is used in the view to display its value using a Binding
    ''' </summary>
    ''' <returns></returns>
    Public Property SampleProperty() As String
        Get
            Return _sampleProperty
        End Get
        Set(ByVal value As String)
            If value <> _sampleProperty Then
                _sampleProperty = value
                NotifyPropertyChanged("SampleProperty")
            End If
        End Set
    End Property

    Public Property IsDataLoaded() As Boolean
        Get
            Return m_IsDataLoaded
        End Get
        Private Set(ByVal value As Boolean)
            m_IsDataLoaded = value
        End Set
    End Property
    Private m_IsDataLoaded As Boolean

    ''' <summary>
    ''' Creates and adds a few ItemViewModel objects into the Items collection.
    ''' </summary>
    Public Sub LoadData()
        ' Sample data; replace with real data
        Me.Items.Add(New ItemViewModel() With { _
         .LineOne = "runtime one", _
         .LineTwo = "Maecenas praesent accumsan bibendum", _
          .LineThree = "Facilisi faucibus habitant inceptos interdum lobortis nascetur pharetra placerat pulvinar sagittis senectus sociosqu" _
        })
        Me.Items.Add(New ItemViewModel() With { _
          .LineOne = "runtime two", _
         .LineTwo = "Dictumst eleifend facilisi faucibus", _
          .LineThree = "Suscipit torquent ultrices vehicula volutpat maecenas praesent accumsan bibendum dictumst eleifend facilisi faucibus" _
        })
        Me.Items.Add(New ItemViewModel() With { _
         .LineOne = "runtime three", _
         .LineTwo = "Habitant inceptos interdum lobortis", _
         .LineThree = "Habitant inceptos interdum lobortis nascetur pharetra placerat pulvinar sagittis senectus sociosqu suscipit torquent" _
        })
        Me.Items.Add(New ItemViewModel() With { _
         .LineOne = "runtime four", _
         .LineTwo = "Nascetur pharetra placerat pulvinar", _
         .LineThree = "Ultrices vehicula volutpat maecenas praesent accumsan bibendum dictumst eleifend facilisi faucibus habitant inceptos" _
        })
        Me.Items.Add(New ItemViewModel() With { _
         .LineOne = "runtime five", _
         .LineTwo = "Maecenas praesent accumsan bibendum", _
         .LineThree = "Maecenas praesent accumsan bibendum dictumst eleifend facilisi faucibus habitant inceptos interdum lobortis nascetur" _
        })
        Me.Items.Add(New ItemViewModel() With { _
         .LineOne = "runtime six", _
         .LineTwo = "Dictumst eleifend facilisi faucibus", _
         .LineThree = "Pharetra placerat pulvinar sagittis senectus sociosqu suscipit torquent ultrices vehicula volutpat maecenas praesent" _
        })
        Me.Items.Add(New ItemViewModel() With { _
         .LineOne = "runtime seven", _
         .LineTwo = "Habitant inceptos interdum lobortis", _
         .LineThree = "Accumsan bibendum dictumst eleifend facilisi faucibus habitant inceptos interdum lobortis nascetur pharetra placerat" _
        })
        Me.Items.Add(New ItemViewModel() With { _
         .LineOne = "runtime eight", _
         .LineTwo = "Nascetur pharetra placerat pulvinar", _
         .LineThree = "Pulvinar sagittis senectus sociosqu suscipit torquent ultrices vehicula volutpat maecenas praesent accumsan bibendum" _
        })
        Me.Items.Add(New ItemViewModel() With { _
         .LineOne = "runtime nine", _
         .LineTwo = "Maecenas praesent accumsan bibendum", _
         .LineThree = "Facilisi faucibus habitant inceptos interdum lobortis nascetur pharetra placerat pulvinar sagittis senectus sociosqu" _
        })
        Me.Items.Add(New ItemViewModel() With { _
         .LineOne = "runtime ten", _
         .LineTwo = "Dictumst eleifend facilisi faucibus", _
         .LineThree = "Suscipit torquent ultrices vehicula volutpat maecenas praesent accumsan bibendum dictumst eleifend facilisi faucibus" _
        })
        Me.Items.Add(New ItemViewModel() With { _
         .LineOne = "runtime eleven", _
         .LineTwo = "Habitant inceptos interdum lobortis", _
         .LineThree = "Habitant inceptos interdum lobortis nascetur pharetra placerat pulvinar sagittis senectus sociosqu suscipit torquent" _
        })
        Me.Items.Add(New ItemViewModel() With { _
         .LineOne = "runtime twelve", _
         .LineTwo = "Nascetur pharetra placerat pulvinar", _
         .LineThree = "Ultrices vehicula volutpat maecenas praesent accumsan bibendum dictumst eleifend facilisi faucibus habitant inceptos" _
        })
        Me.Items.Add(New ItemViewModel() With { _
         .LineOne = "runtime thirteen", _
         .LineTwo = "Maecenas praesent accumsan bibendum", _
         .LineThree = "Maecenas praesent accumsan bibendum dictumst eleifend facilisi faucibus habitant inceptos interdum lobortis nascetur" _
        })
        Me.Items.Add(New ItemViewModel() With { _
         .LineOne = "runtime fourteen", _
         .LineTwo = "Dictumst eleifend facilisi faucibus", _
         .LineThree = "Pharetra placerat pulvinar sagittis senectus sociosqu suscipit torquent ultrices vehicula volutpat maecenas praesent" _
        })
        Me.Items.Add(New ItemViewModel() With { _
         .LineOne = "runtime fifteen", _
         .LineTwo = "Habitant inceptos interdum lobortis", _
         .LineThree = "Accumsan bibendum dictumst eleifend facilisi faucibus habitant inceptos interdum lobortis nascetur pharetra placerat" _
        })
        Me.Items.Add(New ItemViewModel() With { _
         .LineOne = "runtime sixteen", _
         .LineTwo = "Nascetur pharetra placerat pulvinar", _
         .LineThree = "Pulvinar sagittis senectus sociosqu suscipit torquent ultrices vehicula volutpat maecenas praesent accumsan bibendum" _
        })

        Me.IsDataLoaded = True
    End Sub

    Public Event PropertyChanged As PropertyChangedEventHandler Implements INotifyPropertyChanged.PropertyChanged
    Private Sub NotifyPropertyChanged(ByVal propertyName As [String])
        RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(propertyName))
    End Sub
End Class