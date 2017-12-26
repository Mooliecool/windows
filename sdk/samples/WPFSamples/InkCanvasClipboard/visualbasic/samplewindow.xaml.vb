' Interaction logic for SampleWindow.xaml
Imports System.Collections

Partial Public Class SampleWindow
    Inherits System.Windows.Window

    Public Sub New()
        InitializeComponent()
        miInk.IsChecked = True
    End Sub

#Region "Action Menu Handlers"
    ''' <summary>
    ''' clears strokes and elements from the InkCanvas
    ''' </summary>
    Sub OnClear(ByVal sender As Object, ByVal e As RoutedEventArgs)
        inkCanvas.Strokes.Clear()
        inkCanvas.Children.Clear()
    End Sub

    ''' <summary>
    ''' closes the application
    ''' </summary>
    Sub OnExit(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.Close()
    End Sub
#End Region

#Region "Edit Menu Handlers"
    ''' <summary>
    ''' pastes clipboard content into the center
    ''' of the InkCanvas
    ''' </summary>
    Sub OnPasteInCenter(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim center As New Point(inkCanvas.ActualWidth / 2D, inkCanvas.ActualHeight / 2D)
        inkCanvas.Paste(center)
    End Sub

    ''' <summary>
    ''' switches InkCanvas to selection mode
    ''' </summary>
    Sub OnSelectChecked(ByVal sender As Object, ByVal e As RoutedEventArgs)
        miInk.IsChecked = False
        inkCanvas.EditingMode = InkCanvasEditingMode.Select
    End Sub

    ''' <summary>
    ''' switches InkCanvas to inking mode
    ''' </summary>
    Sub OnInkChecked(ByVal sender As Object, ByVal e As RoutedEventArgs)
        miSelect.IsChecked = False
        inkCanvas.EditingMode = InkCanvasEditingMode.Ink
    End Sub
#End Region

#Region "PasteFormat Menu Handler"
    ''' <summary>
    ''' updates InkCanvas preferred paste formats
    ''' </summary>
    Sub OnPasteFormatChanged(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim preferredPasteFormats As New List(Of InkCanvasClipboardFormat)()

        If (miXaml.IsChecked) Then
            preferredPasteFormats.Add(InkCanvasClipboardFormat.Xaml)
        End If

        If (miIsf.IsChecked) Then
            preferredPasteFormats.Add(InkCanvasClipboardFormat.InkSerializedFormat)
        End If

        If (miText.IsChecked) Then
            preferredPasteFormats.Add(InkCanvasClipboardFormat.Text)
        End If

        inkCanvas.PreferredPasteFormats = preferredPasteFormats
    End Sub
#End Region

#Region "Insert Menu Handlers"
    ''' <summary>
    ''' inserts a new TextBox into InkCanvas
    ''' </summary>
    Sub OnInsertTextBox(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim textBoxToInsert As New TextBox()
        textBoxToInsert.Text = "New TextBox"
        textBoxToInsert.AcceptsReturn = True
        textBoxToInsert.SetValue(inkCanvas.TopProperty, inkCanvas.ActualHeight / 2D)
        textBoxToInsert.SetValue(inkCanvas.LeftProperty, inkCanvas.ActualWidth / 2D)
        inkCanvas.Children.Add(textBoxToInsert)
    End Sub

    ''' <summary>
    ''' inserts a new Label into InkCanvas
    ''' </summary>
    Sub OnInsertLabel(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim labelToInsert As New Label()
        labelToInsert.Content = "New Label"
        labelToInsert.Background = Brushes.LightBlue
        labelToInsert.SetValue(inkCanvas.TopProperty, inkCanvas.ActualHeight / 2D)
        labelToInsert.SetValue(inkCanvas.LeftProperty, inkCanvas.ActualWidth / 2D)
        inkCanvas.Children.Add(labelToInsert)
    End Sub

    ''' <summary>
    ''' inserts a new Ellipse shape into InkCanvas
    ''' </summary>
    Sub OnInsertShape(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim ellipseToInsert As New Ellipse
        ellipseToInsert.Width = 150D
        ellipseToInsert.Height = 100D
        ellipseToInsert.Fill = New LinearGradientBrush(Colors.Goldenrod, Colors.HotPink, 20D)
        ellipseToInsert.SetValue(inkCanvas.TopProperty, inkCanvas.ActualHeight / 2D)
        ellipseToInsert.SetValue(inkCanvas.LeftProperty, inkCanvas.ActualWidth / 2D)
        inkCanvas.Children.Add(ellipseToInsert)
    End Sub
#End Region
End Class
