#define DEBUG  //Workaround - This allows Debug.Write to work.


using System;
using System.Windows;
using System.Data;
using System.Xml;
using System.Configuration;
using System.Windows.Navigation;

namespace Microsoft.Samples.Graphics
{

    public partial class app : Application
    {

        public app()
        {
            
        
        }

        private void myAppStartingUp(object sender, StartupEventArgs args)
        {
            Window mySampleWindow = new SampleViewer();
            mySampleWindow.Show();

        }
          

    }
}