
Imports System
Imports System.Windows
Imports System.Xml
Imports System.Configuration

Namespace ImageView

    '/ <summary>
    '/ Interaction logic for app.xaml
    '/ </summary>

    Partial Class app
        Inherits Application
        '
        'ToDo: Error processing original source shown below
        '
        ' public partial class app : Application
        '---------^--- 'class', 'struct', 'interface' or 'delegate' expected
        '
        'ToDo: Error processing original source shown below
        '
        ' public partial class app : Application
        '-----------------^--- Syntax error: ';' expected
        Sub AppStartingUp(ByVal sender As Object, ByVal e As StartupEventArgs)
            Dim defaultWindow As New ImageViewExample()
            defaultWindow.InitializeComponent()
            defaultWindow.Show()

        End Sub 'AppStartingUp
    End Class 'app 
End Namespace 'ImageView
