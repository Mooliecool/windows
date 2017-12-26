using System;
using System.Windows;
using System.Data;
using System.Xml;
using System.Configuration;

namespace Microsoft.Samples.Animation
{
    /// <summary>
    /// Interaction logic for app.xaml
    /// </summary>

    public partial class app : Application
    {
    
        public app()
        {
        
        }
    
        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);
            CustomAnimationExample mainWindow = new CustomAnimationExample();
            mainWindow.Show();
            
        }    

    }
}