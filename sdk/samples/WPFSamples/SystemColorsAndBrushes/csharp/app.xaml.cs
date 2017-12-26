#define DEBUG  //Workaround - This allows Debug.Write to work.

//This is a list of commonly used namespaces for an application class.
using System;
using System.Windows;
using System.Data;
using System.Xml;
using System.Configuration;

namespace SystemColorsAndBrushes_csharp
{
    /// <summary>
    /// Interaction logic for Application.xaml
    /// </summary>

    public partial class app : Application
    {
		void AppStartingUp(object sender, StartupEventArgs e)
		{
			Window1 mainWindow = new Window1();
			mainWindow.Show();
		}

    }
}