Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Media
Imports System.Windows.Shapes
Imports System.Windows.Navigation
Imports System.Threading

Namespace SDKSample

    Public Class ScrollViewerScrollChangedSample
        Inherits Page

        Dim scrView1 As New ScrollViewer
        Dim txt1 As New TextBlock
        Dim txt2 As New TextBlock

        Public Sub sChanged(ByVal Sender As Object, ByVal e As ScrollChangedEventArgs)
            'Get value of computed scroll values and assign to a TextBlock.
            txt1.Text = "ScrollViewer.HorizontalScrollBarVisibility is set to: " + scrView1.ComputedHorizontalScrollBarVisibility.ToString()
            txt2.Text = "ScrollViewer.VerticalScrollBarVisibility is set to: " + scrView1.ComputedVerticalScrollBarVisibility.ToString()
        End Sub

        Public Sub New()

            WindowTitle = "ScrollViewer ScrollChanged Event Sample"
            WindowHeight = 400
            WindowWidth = 400


            scrView1.CanContentScroll = True
            scrView1.HorizontalScrollBarVisibility = ScrollBarVisibility.Auto
            scrView1.VerticalScrollBarVisibility = ScrollBarVisibility.Auto

            'Create a StackPanel.
            Dim sPanel As New StackPanel
            sPanel.Orientation = Orientation.Vertical
            sPanel.Children.Add(txt1)
            sPanel.Children.Add(txt2)

            'Add the first rectangle to the StackPanel
            Dim Rect1 As New Rectangle
            Rect1.Stroke = Brushes.Black
            Rect1.Fill = Brushes.SkyBlue
            Rect1.Height = 400
            Rect1.Width = 400
            sPanel.Children.Add(Rect1)

            'scrView1.ScrollChanged += sChanged()
            AddHandler scrView1.ScrollChanged, New ScrollChangedEventHandler(AddressOf sChanged)
            scrView1.Content = sPanel
            Me.Content = scrView1
        End Sub

    End Class
    'Displays the Sample
    Public Class app
        Inherits Application

        Protected Overrides Sub OnStartup(ByVal e As StartupEventArgs)
            MyBase.OnStartup(e)
            CreateAndShowMainWindow()
        End Sub

        Private Sub CreateAndShowMainWindow()
            ' Create the application's main window.
            Dim myWindow As New NavigationWindow()
            ' Display the sample
            Dim myContent As New ScrollViewerScrollChangedSample()
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

