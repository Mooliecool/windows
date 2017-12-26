using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Microsoft.Samples.Animation
{

    public partial class SplineExample : Page
    {
        private Point controlPoint1 = new Point(0,100);
        private Point controlPoint2 = new Point(0,100);
        
        private void OnSliderChanged (object sender, RoutedEventArgs e)
        {
        
            

            // Retrieve the name of slider.
            string name = ((Slider)sender).Name;

            RoutedPropertyChangedEventArgs<double> args = e as RoutedPropertyChangedEventArgs<double>;
            
            switch (name)
            {
                case "SliderControlPoint1X":
                    mySplineKeyFrame.KeySpline.ControlPoint1 = new Point((double)args.NewValue, mySplineKeyFrame.KeySpline.ControlPoint1.Y);
                    controlPoint1.X = 100 * (double)args.NewValue;
                    break;
                case "SliderControlPoint1Y":
                    mySplineKeyFrame.KeySpline.ControlPoint1 = new Point(mySplineKeyFrame.KeySpline.ControlPoint1.X, (double)args.NewValue);
                    controlPoint1.Y = 100 - (100 * (double)args.NewValue);  
                    break;
                case "SliderControlPoint2X":
                    mySplineKeyFrame.KeySpline.ControlPoint2 = new Point((double)args.NewValue, mySplineKeyFrame.KeySpline.ControlPoint2.Y);
                    controlPoint2.X = 100 * (double)args.NewValue;
                    break;
                case "SliderControlPoint2Y":
                    mySplineKeyFrame.KeySpline.ControlPoint2 = new Point(mySplineKeyFrame.KeySpline.ControlPoint2.X, (double)args.NewValue);
                    controlPoint2.Y = 100 - (100 * (double)args.NewValue);
                    break;
                    
                
            }
            

            // Update the animations and illustrations.
            myVector3DSplineKeyFrame.KeySpline.ControlPoint1 = mySplineKeyFrame.KeySpline.ControlPoint1;
            myVector3DSplineKeyFrame.KeySpline.ControlPoint2 = mySplineKeyFrame.KeySpline.ControlPoint2;
            
            
            SplineIllustrationSegment.Point1 = controlPoint1;
            SplineIllustrationSegment.Point2 = controlPoint2;   
            SplineControlPoint1Marker.Center = controlPoint1;
            SplineControlPoint2Marker.Center = controlPoint2;
            
            keySplineText.Text = 
                "KeySpline=\"" + mySplineKeyFrame.KeySpline.ControlPoint1.X.ToString("N") + "," + 
                mySplineKeyFrame.KeySpline.ControlPoint1.Y.ToString("N") + " " + 
                mySplineKeyFrame.KeySpline.ControlPoint2.X.ToString("N") + "," + 
                mySplineKeyFrame.KeySpline.ControlPoint2.Y.ToString("N") + "\"";
           
           
            
            // Determine the storyboard's current time.
           TimeSpan? oldTime = (TimeSpan)ExampleStoryboard.GetCurrentTime(this);
           if (oldTime == null)
            oldTime = TimeSpan.FromSeconds(0);
            
           // Generate new clocks for the animations by calling 
           // the Begin method.
           ExampleStoryboard.Begin(this, true);
           
           // Because the storyboard was reset, advance it to its previous
           // position using the Seek method.
           ExampleStoryboard.Seek(this, (TimeSpan)oldTime, TimeSeekOrigin.BeginTime);
        }

    }
}