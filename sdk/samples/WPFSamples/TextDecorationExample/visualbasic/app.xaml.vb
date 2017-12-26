Imports System 
Imports System.Windows 

namespace TextDecorationExample

    '@ <summary>
    '@ Interaction logic for Application.xaml
    '@ </summary>

    partial class app inherits  Application

        void AppStartingUp(ByVal sender As Object, ByVal args As StartupEventArgs )

             Window1 mainWindow = new Window1() 
             mainWindow.InitializeComponent() 
             mainWindow.Show() 
          End

      End
End Namespace
