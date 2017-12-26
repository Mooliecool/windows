#define Debug  //Workaround - This allows Debug.Write to work.

//This is a list of commonly used namespaces for an application class.
using System;
using System.Windows;
using System.Windows.Navigation;
using System.Data;
using System.Xml;
using System.Configuration;


namespace SelectionPatternClient
{
    public partial class app : Application
    {
      private void On_Startup(object sender, StartupEventArgs args)
      {
        Client selectionPatternClient = new Client();
        selectionPatternClient.Top = 50;
        selectionPatternClient.Left = 50;
        selectionPatternClient.InitializeComponent();
        selectionPatternClient.Show();
      }
    }
}