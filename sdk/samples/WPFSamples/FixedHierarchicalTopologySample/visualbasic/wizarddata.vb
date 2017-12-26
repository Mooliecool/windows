Imports System

Public Class WizardData

    Public Property DataItem1() As String
        Get
            Return Me._dataItem1
        End Get
        Set(ByVal value As String)
            Me._dataItem1 = value
        End Set
    End Property

    Public Property DataItem2() As String
        Get
            Return Me._dataItem2
        End Get
        Set(ByVal value As String)
            Me._dataItem2 = value
        End Set
    End Property

    Public Property DataItem3() As String
        Get
            Return Me._dataItem3
        End Get
        Set(ByVal value As String)
            Me._dataItem3 = value
        End Set
    End Property

    Public Property DataItem4() As String
        Get
            Return Me._dataItem4
        End Get
        Set(ByVal value As String)
            Me._dataItem4 = value
        End Set
    End Property

    ' Fields
    Private _dataItem1 As String
    Private _dataItem2 As String
    Private _dataItem3 As String
    Private _dataItem4 As String

End Class



