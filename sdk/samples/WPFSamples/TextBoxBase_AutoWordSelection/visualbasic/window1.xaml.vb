Imports System     
Imports System.Windows     
Imports System.Windows.Controls
Imports System.IO

Namespace SDKSample

    Partial Public Class Window1
        Inherits Window

        Public Sub windowLoaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
            InitializeComponent()
            myCheckBox.IsChecked = False
            myTextBox.AutoWordSelection = False
            LoadSampleContent()
        End Sub
        Public Sub AutoWordSelectionToggle(ByVal sender As Object, ByVal args As RoutedEventArgs)
            myTextBox.AutoWordSelection = myCheckBox.IsChecked.Value
        End Sub
        Public Sub LoadSampleContent()
            'Assumes sample content file is in the root project folder, and that
            'the project exectutable will be in bin\debug relative to the root
            'project folder.  Tweak as necessary.
            Dim relativePathAndFileName As String = "..\\..\\sample_content.txt"
            Dim fileToLoad As New StreamReader(relativePathAndFileName)
            myTextBox.Text = fileToLoad.ReadToEnd()
            fileToLoad.Close()
        End Sub
    End Class
End Namespace
