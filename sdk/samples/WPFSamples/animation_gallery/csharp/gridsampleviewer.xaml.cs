using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Navigation;
using System.IO;
using System.Windows.Data;


namespace Microsoft.Samples.Animation.AnimationGallery
{


    public partial class GridSampleViewer : Page
    {
    
    
        
    
        private DoubleAnimation sampleGridOpacityAnimation;
        private DoubleAnimation sampleGridTranslateTransformAnimation;
        private DoubleAnimation borderTranslateDoubleAnimation;

        public GridSampleViewer()
        {
            InitializeComponent(); 
            
            
            Binding widthBinding = new Binding("ActualWidth");
            widthBinding.Source = this;
            
            sampleGridOpacityAnimation = new DoubleAnimation();
            sampleGridOpacityAnimation.To = 0; 
            sampleGridOpacityAnimation.Duration = new Duration(TimeSpan.FromSeconds(1));
            
            sampleGridTranslateTransformAnimation = new DoubleAnimation();
            sampleGridTranslateTransformAnimation.BeginTime = TimeSpan.FromSeconds(1);
            
            BindingOperations.SetBinding(sampleGridTranslateTransformAnimation, DoubleAnimation.ToProperty, widthBinding);
            sampleGridTranslateTransformAnimation.Duration = new Duration(TimeSpan.FromSeconds(1));      

            borderTranslateDoubleAnimation = new DoubleAnimation();
            borderTranslateDoubleAnimation.To = 0;
            borderTranslateDoubleAnimation.Duration = new Duration(TimeSpan.FromSeconds(1));
            borderTranslateDoubleAnimation.BeginTime = TimeSpan.FromSeconds(2);
            borderTranslateDoubleAnimation.FillBehavior = FillBehavior.HoldEnd;
            BindingOperations.SetBinding(borderTranslateDoubleAnimation, DoubleAnimation.FromProperty, widthBinding);          
            
            
        }
        private static Uri _packUri = new Uri("pack://application:,,,/");

        private void selectedSampleChanged(object sender, RoutedEventArgs args)
        {
           
            if (args.Source is RadioButton){
              
                RadioButton theButton = (RadioButton)args.Source;
                Frame theFrame = (Frame)theButton.Content;
                
                if (theFrame.HasContent){

                    Uri source = theFrame.CurrentSource;
                    if ((source != null) && !source.IsAbsoluteUri)
                    {
                        source = new Uri(_packUri, source);
                    }
                    SampleDisplayFrame.Source = source;

                    SampleDisplayBorder.Visibility = Visibility.Visible;
                 
                }
            
            } 
        
        }  
        
        private void sampleDisplayFrameLoaded(object sender, EventArgs args)
        {
            
            SampleGrid.BeginAnimation(Grid.OpacityProperty, sampleGridOpacityAnimation);
            SampleGridTranslateTransform.BeginAnimation(TranslateTransform.XProperty, sampleGridTranslateTransformAnimation);
            SampleDisplayBorderTranslateTransform.BeginAnimation(TranslateTransform.XProperty, borderTranslateDoubleAnimation);
            SampleDisplayBorder.Visibility = Visibility.Visible;

        }  
        
        
     
        
        private void galleryLoaded(object sender, RoutedEventArgs args)
        {
               
            SampleDisplayBorderTranslateTransform.X = this.ActualWidth;
            SampleDisplayBorder.Visibility = Visibility.Hidden; 
        }
        
    
        
        private void pageSizeChanged(object sender, SizeChangedEventArgs args)
        {
            
            SampleDisplayBorderTranslateTransform.X = this.ActualWidth;
        }        

    }
    
    
    
  

}
