using System;
using System.Windows;
using System.Windows.Navigation;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Interop;


namespace Microsoft.Samples.Animation.AnimationGallery
{



    public partial class app : Application
    {
    
        public app()
        {
        
        }
    
        protected override void OnStartup(StartupEventArgs e)
        {
      
            Window myWindow = new Window();
            myWindow.Content = new GridSampleViewer();
            myWindow.Show();
            base.OnStartup(e);
        }
          
    
    }


}
