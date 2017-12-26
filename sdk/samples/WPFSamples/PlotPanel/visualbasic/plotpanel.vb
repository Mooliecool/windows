Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Media
Imports System.Windows.Shapes
Imports System.Windows.Navigation
Imports System.Threading

Namespace SDKSample
    Public Class PlotPanelSample
        Inherits Page
        Public Sub New()
            WindowTitle = "PlotPanel Sample"
            Dim plot1 As New PlotPanel
            plot1.Background = Brushes.White
            Dim rect1 As New Rectangle
            rect1.Fill = Brushes.CornflowerBlue
            rect1.Width = 200
            rect1.Height = 200
            plot1.Children.Add(rect1)
            Me.Content = plot1
        End Sub
    End Class
    Public Class PlotPanel
        Inherits Panel
        'Override the default Measure method of Panel.

        Protected Overrides Function MeasureOverride(ByVal availableSize As System.Windows.Size) As System.Windows.Size
            Dim childSize As Size = CType(availableSize, Size)
            For Each child As UIElement In InternalChildren
                child.Measure(childSize)
            Next
            Return MyBase.MeasureOverride(availableSize)
        End Function
        Protected Overrides Function ArrangeOverride(ByVal finalSize As System.Windows.Size) As System.Windows.Size
            For Each child As UIElement In InternalChildren
                Dim x As Double = 50
                Dim y As Double = 50
                child.Arrange(New Rect(New System.Windows.Point(x, y), child.DesiredSize))
            Next
            Return MyBase.ArrangeOverride(finalSize)
        End Function
    End Class

    'Displays the Sample.
    Public Class app
        Inherits Application

        Protected Overrides Sub OnStartup(ByVal e As StartupEventArgs)
            MyBase.OnStartup(e)
            CreateAndShowMainWindow()
        End Sub

        Private Sub CreateAndShowMainWindow()
            ' Create the application's main window.
            Dim myWindow As New NavigationWindow()
            ' Display the sample.
            Dim myContent As New PlotPanelSample()
            myWindow.Navigate(myContent)
            MainWindow = myWindow
            myWindow.Show()
        End Sub
    End Class
    ' Starts the application.
    Public NotInheritable Class EntryClass
        Public Shared Sub Main()
            Dim app As New app()
            app.Run()
        End Sub
    End Class
End Namespace

