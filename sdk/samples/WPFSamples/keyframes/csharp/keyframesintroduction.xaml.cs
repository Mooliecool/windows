using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace keyframes_markup
{


    public partial class KeyFramesIntroduction : Page
    {
    
        public KeyFramesIntroduction()
        {
            // InitializeComponent();
        }
        
        public void pageLoaded(object sender, RoutedEventArgs args)
        {
            myVisualBrush.Visual = myImage;
        
        }
        
        private void exampleCanvasLayoutUpdated(object sender, EventArgs args)
        {
        
            
            myVisualBrush.Viewbox = 
                            new Rect(
                                Canvas.GetLeft(myRectangle),
                                Canvas.GetTop(myRectangle),
                                myRectangle.ActualWidth,
                                myRectangle.ActualHeight        
                            );
            
        
            
        
        }
        

    }
}