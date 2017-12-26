Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Data

Class MyApp
    Inherits Application

    ' Methods
    Private Sub OnClick(ByVal obj As Object, ByVal args As RoutedEventArgs)
        Dim element1 As FrameworkElement = DirectCast(obj, FrameworkElement)
        Select Case element1.Name
            Case "Clear"
                BindingOperations.ClearBinding(Me.myText, TextBlock.TextProperty)
            Case "Refresh"
                BindingOperations.ClearBinding(Me.myText, TextBlock.TextProperty)
                Dim data1 As New MyData(DateTime.Now)
                Dim binding1 As New Binding("MyDataProperty")
                binding1.Source = data1
                Me.myText.SetBinding(TextBlock.TextProperty, binding1)
        End Select
    End Sub

    Protected Overrides Sub OnStartup(ByVal e As StartupEventArgs)
        Dim handler1 As RoutedEventHandler = New RoutedEventHandler(AddressOf Me.OnClick)
        Dim window1 As New Window
        window1.Width = 250
        window1.Height = 200
        Dim panel1 As New DockPanel
        window1.Content = panel1
        panel1.Width = 200
        panel1.Height = 150
        Me.dp = New DockPanel
        DockPanel.SetDock(Me.dp, Dock.Top)
        panel1.Children.Add(Me.dp)
        Me.button = New Button
        Me.button.Name = "Clear"
        Me.button.Content = "Clear Binding"
        Me.button.Width = 120
        Me.button.Height = 30
        AddHandler Me.button.Click, handler1
        DockPanel.SetDock(Me.button, Dock.Top)
        Me.dp.Children.Add(Me.button)
        Me.button2 = New Button
        Me.button2.Name = "Refresh"
        Me.button2.Content = "Refresh Binding"
        Me.button2.Width = 120
        Me.button2.Height = 30
        AddHandler Me.button2.Click, handler1
        DockPanel.SetDock(Me.button2, Dock.Top)
        Me.dp.Children.Add(Me.button2)
        Me.myText = New TextBlock
        Me.myText.Text = "no binding yet..."
        Me.myText.Height = 35
        Me.myText.HorizontalAlignment = HorizontalAlignment.Center
        DockPanel.SetDock(Me.myText, Dock.Top)
        Me.dp.Children.Add(Me.myText)
        window1.Show()
    End Sub


    ' Fields
    Public button As Button
    Public button2 As Button
    Public dp As DockPanel
    Public myBinding As Binding
    Public myDataObject As MyData
    Public myText As TextBlock
End Class


