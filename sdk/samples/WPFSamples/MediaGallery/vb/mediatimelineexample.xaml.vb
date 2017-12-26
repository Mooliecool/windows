
Imports System
Imports System.Media
Imports System.Globalization
Imports System.Windows
Imports System.ComponentModel
Imports System.Windows.Controls
Imports System.Windows.Data
Imports System.Windows.Documents
Imports System.Windows.Media
Imports System.Windows.Shapes
Imports System.Windows.Threading
Imports System.Windows.Media.Animation
Imports System.Windows.Media.Effects
Imports System.Windows.Media.Imaging

Namespace SDKSample

    Partial Class MediaTimelineExample
        Implements INotifyPropertyChanged

        Public Sub New()
            Me.InitializeComponent()

        End Sub 'New

        Private timer As DispatcherTimer


        Public Sub OnWindowLoaded(ByVal sender As Object, ByVal e As RoutedEventArgs)

            Dim b2 As New Binding()
            b2.Source = Me
            b2.Path = New PropertyPath("MyProp")

            ' Bind to the slider and the textbox
            BindingOperations.SetBinding(ClockSlider, Slider.ValueProperty, b2)
            BindingOperations.SetBinding(PositionTextBox, TextBox.TextProperty, b2)

            timer = New DispatcherTimer()
            timer.Interval = New TimeSpan(0, 0, 0, 0, 100)

            ' Every tick, the timer_Tick event handler is fired.
            AddHandler timer.Tick, AddressOf timer_Tick

        End Sub 'OnWindowLoaded


        Sub timer_Tick(ByVal sender As Object, ByVal e As EventArgs)
            OnPropertyChanged("MyProp")

        End Sub 'timer_Tick



        Public Property MyProp() As Double
            Get
                Return ClickedBSB.Storyboard.GetCurrentTime(DocumentRoot).Value.TotalSeconds
            End Get

            Set(ByVal value As Double)
                ClickedBSB.Storyboard.SeekAlignedToLastTick(DocumentRoot, New TimeSpan(CType(Math.Floor(value * TimeSpan.TicksPerSecond), Long)), TimeSeekOrigin.BeginTime)
                OnPropertyChanged("MyProp")
            End Set
        End Property

        Public Event PropertyChanged As PropertyChangedEventHandler _
            Implements INotifyPropertyChanged.PropertyChanged

        Private Sub OnPropertyChanged(ByVal name As String)
            'If Not (PropertyChanged Is Nothing) Then
            RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(name))
            'End If

        End Sub 'OnPropertyChanged


        Public Sub OnMediaOpened(ByVal sender As Object, ByVal e As RoutedEventArgs)
            If Not (MediaElement1.Clock Is Nothing) Then
                StatusBar.Text = MediaElement1.Clock.NaturalDuration.ToString()
                ClockSlider.Maximum = MediaElement1.Clock.NaturalDuration.TimeSpan.TotalSeconds + 10
            End If
            timer.Start()

        End Sub 'OnMediaOpened


        Sub mc_CurrentTimeInvalidated(ByVal sender As Object, ByVal e As EventArgs)
            StatusBar.Text = "CurrentStateInvalidated"

        End Sub 'mc_CurrentTimeInvalidated



        Public Function Convert(ByVal o As Object, ByVal type As Type, ByVal param As Object, ByVal cul As Globalization.CultureInfo) As Object
            Dim currPosition As TimeSpan = CType(o, TimeSpan)
            Dim NumMSecs As Double = currPosition.Milliseconds

            Dim NewValue As Double = NumMSecs / MediaElement1.Clock.NaturalDuration.TimeSpan.TotalMilliseconds * ClockSlider.Maximum
            Return NewValue

        End Function 'Convert


        Public Function ConvertBack(ByVal o As Object, ByVal type As Type, ByVal param As Object, ByVal cul As Globalization.CultureInfo) As Object
            Return Nothing

        End Function 'ConvertBack
    End Class 'MediaTimelineExample

    Public Class PositionConverter
        Implements IValueConverter

        Public Function Convert(ByVal o As Object, ByVal type As Type, ByVal param As Object, ByVal cul As Globalization.CultureInfo) As Object _
        Implements IValueConverter.Convert
            Dim currPosition As TimeSpan = CType(o, TimeSpan)
            Return currPosition.Seconds

        End Function 'Convert


        Public Function ConvertBack(ByVal o As Object, ByVal type As Type, ByVal param As Object, ByVal cul As Globalization.CultureInfo) As Object _
        Implements IValueConverter.ConvertBack
            Return Nothing

        End Function 'ConvertBack
    End Class 'PositionConverter
End Namespace 'SDKSample
