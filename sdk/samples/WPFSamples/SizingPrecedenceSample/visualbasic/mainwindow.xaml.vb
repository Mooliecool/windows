Imports System

' Interaction logic for MainWindow.xaml
Partial Public Class MainWindow
    Inherits System.Windows.Window

    Public Sub New()
        InitializeComponent()
    End Sub

    Private Sub showWindowButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        Dim sw As New SizingWindow

        Dim isChecked As Nullable(Of Boolean) = Me.setWSCB.IsChecked
        If (isChecked.GetValueOrDefault AndAlso isChecked.HasValue) Then
            sw.WindowState = DirectCast(System.Enum.Parse(GetType(WindowState), Me.wsLB.Text), WindowState)
        End If
        isChecked = Me.setMinWidthCB.IsChecked
        If (isChecked.GetValueOrDefault AndAlso isChecked.HasValue) Then
            sw.MinWidth = Double.Parse(Me.minWidthTB.Text)
        End If
        isChecked = Me.setMinHeightCB.IsChecked
        If (isChecked.GetValueOrDefault AndAlso isChecked.HasValue) Then
            sw.MinHeight = Double.Parse(Me.minHeightTB.Text)
        End If
        isChecked = Me.setMaxWidthCB.IsChecked
        If (isChecked.GetValueOrDefault AndAlso isChecked.HasValue) Then
            sw.MaxWidth = Double.Parse(Me.maxWidthTB.Text)
        End If
        isChecked = Me.setMaxHeightCB.IsChecked
        If (isChecked.GetValueOrDefault AndAlso isChecked.HasValue) Then
            sw.MaxHeight = Double.Parse(Me.maxHeightTB.Text)
        End If
        isChecked = Me.setWidthCB.IsChecked
        If (isChecked.GetValueOrDefault AndAlso isChecked.HasValue) Then
            sw.Width = Double.Parse(Me.widthTB.Text)
        End If
        isChecked = Me.setHeightCB.IsChecked
        If (isChecked.GetValueOrDefault AndAlso isChecked.HasValue) Then
            sw.Height = Double.Parse(Me.heightTB.Text)
        End If
        isChecked = Me.setSTCCB.IsChecked
        If (isChecked.GetValueOrDefault AndAlso isChecked.HasValue) Then
            sw.SizeToContent = DirectCast(System.Enum.Parse(GetType(SizeToContent), Me.stcLB.Text), SizeToContent)
        End If
        sw.Show()
    End Sub

End Class
