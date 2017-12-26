
Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Documents
Imports System.Windows.Navigation
Imports System.Windows.Shapes
Imports System.Windows.Data
Imports System.Windows.Media
Imports System.Windows.Input




Class MediaElementExample
    Inherits Page
     '
    'ToDo: Error processing original source shown below
    '
    ' public partial class MediaElementExample : Page
    '---------^--- 'class', 'struct', 'interface' or 'delegate' expected
    '
    'ToDo: Error processing original source shown below
    '
    ' public partial class MediaElementExample : Page
    '-----------------^--- Syntax error: ';' expected
    
    ' Play the media.
    Sub OnMouseDownPlayMedia(ByVal sender As Object, ByVal args As MouseButtonEventArgs) 
        
        ' The Play method will begin the media if it is not currently active or 
        ' resume media if it is paused. This has no effect if the media is
        ' already running.
        myMediaElement.Play()
        
        ' Initialize the MediaElement property values.
        InitializePropertyValues()
    
    End Sub 'OnMouseDownPlayMedia
     
    
    ' Pause the media.
    Sub OnMouseDownPauseMedia(ByVal sender As Object, ByVal args As MouseButtonEventArgs) 
        
        ' The Pause method pauses the media if it is currently running.
        ' The Play method can be used to resume.
        myMediaElement.Pause()
    
    End Sub 'OnMouseDownPauseMedia
     
    
    ' Stop the media.
    Sub OnMouseDownStopMedia(ByVal sender As Object, ByVal args As MouseButtonEventArgs) 
        
        ' The Stop method stops and resets the media to be played from
        ' the beginning.
        myMediaElement.Stop()
    
    End Sub 'OnMouseDownStopMedia
     
    
    ' Change the volume of the media.
    Public Sub ChangeMediaVolume(ByVal sender As Object, ByVal __unknown As RoutedPropertyChangedEventArgs) 
        __unknown < Double > args '
        'ToDo: Error processing original source shown below
        '        // Change the volume of the media.
        '        public void ChangeMediaVolume(object sender, RoutedPropertyChangedEventArgs<double> args)
        '------------------------------------------------------------------------------------^--- Syntax error: 'identifier' expected
        '
        'ToDo: Error processing original source shown below
        '        // Change the volume of the media.
        '        public void ChangeMediaVolume(object sender, RoutedPropertyChangedEventArgs<double> args)
        '-------------------------------------------------------------------------------------------^--- Syntax error: '.' expected
        '
        'ToDo: Error processing original source shown below
        '        // Change the volume of the media.
        '        public void ChangeMediaVolume(object sender, RoutedPropertyChangedEventArgs<double> args)
        '-------------------------------------------------------------------------------------------------^--- Syntax error: ';' expected
        If (True) Then
            myMediaElement.Volume = System.Convert.ToDouble(volumeSlider.Value)
        End If
        
        ' Change the speed of the media.
        Dim ChangeMediaSpeedRatio As
        '
        'ToDo: Error processing original source shown below
        '        // Change the speed of the media.
        '        public void ChangeMediaSpeedRatio(object sender, RoutedPropertyChangedEventArgs<double> args)
        '----------------^--- GenCode(token): unexpected token type
        RoutedPropertyChangedEventArgs < Double > args '
        'ToDo: Error processing original source shown below
        '        // Change the speed of the media.
        '        public void ChangeMediaSpeedRatio(object sender, RoutedPropertyChangedEventArgs<double> args)
        '---------^--- expression expected
        '
        'ToDo: Error processing original source shown below
        '        // Change the speed of the media.
        '        public void ChangeMediaSpeedRatio(object sender, RoutedPropertyChangedEventArgs<double> args)
        '------------------------------------------^--- Syntax error: ';' expected
        '
        'ToDo: Error processing original source shown below
        '        // Change the speed of the media.
        '        public void ChangeMediaSpeedRatio(object sender, RoutedPropertyChangedEventArgs<double> args)
        '--------------------------------------------------^--- Syntax error: '.' expected
        '
        'ToDo: Error processing original source shown below
        '        // Change the speed of the media.
        '        public void ChangeMediaSpeedRatio(object sender, RoutedPropertyChangedEventArgs<double> args)
        '--------------------------------------------------------^--- Syntax error: ';' expected
        '
        'ToDo: Error processing original source shown below
        '        // Change the speed of the media.
        '        public void ChangeMediaSpeedRatio(object sender, RoutedPropertyChangedEventArgs<double> args)
        '-----------------------------------------------------------------------------------------------^--- Syntax error: '.' expected
        '
        'ToDo: Error processing original source shown below
        '        // Change the speed of the media.
        '        public void ChangeMediaSpeedRatio(object sender, RoutedPropertyChangedEventArgs<double> args)
        '-----------------------------------------------------------------------------------------------------^--- Syntax error: ';' expected
        If (True) Then
            myMediaElement.SpeedRatio = System.Convert.ToDouble(speedRatioSlider.Value)
        End If
        
        Dim InitializePropertyValues As
        '
        'ToDo: Error processing original source shown below
        '
        '        void InitializePropertyValues()
        '---------^--- GenCode(token): unexpected token type '
        'ToDo: Error processing original source shown below
        '
        '        void InitializePropertyValues()
        '--------------------------------------^--- Syntax error: ';' expected
        '
        'ToDo: Error processing original source shown below
        '
        '        void InitializePropertyValues()
        '---------------------------------------^--- expression expected
        If (True) Then '
            'ToDo: Error processing original source shown below
            '        void InitializePropertyValues()
            '        {
            '---------^--- Syntax error: ';' expected
            ' Set the media's starting Volume and SpeedRatio to the current value of the
            ' their respective slider controls.
            myMediaElement.Volume = System.Convert.ToDouble(volumeSlider.Value)
            myMediaElement.SpeedRatio = System.Convert.ToDouble(speedRatioSlider.Value)
        End If
    
    End Sub 'ChangeMediaVolume 
End Class 'MediaElementExample