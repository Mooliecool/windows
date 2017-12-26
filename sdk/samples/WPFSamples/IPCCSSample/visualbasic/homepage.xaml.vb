Imports System
Imports System.ComponentModel
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Navigation

Public Class HomePage
    Inherits Page

    Implements IProvideCustomContentState

    ' Methods
    Public Sub New()

        Me.InitializeComponent()

        ' Display initial read count
        Me.readCountLabel.Content = 1

    End Sub

    Function GetContentState() As CustomContentState Implements IProvideCustomContentState.GetContentState

        ' Store vertical position of scroll viewer with navigation history entry
        ' before being navigated to next page.
        Return New CustomPageContentState(Me.documentScrollViewer.VerticalOffset, Integer.Parse(Me.readCountLabel.Content.ToString()))

    End Function

End Class