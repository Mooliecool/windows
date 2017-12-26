using System;
using System.Windows;
using System.Windows.Input;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Data;


namespace Microsoft.Samples.Animation.AnimationGallery
{


    public partial class InteractiveAnimationsExample : Page
    {
    
        private AnimationTransitionType selectedTransition;
        
        public InteractiveAnimationsExample()
        {
      
        }
        
        
        // Computes the target point when the user clicks the Canvas
        // and starts the appropriate animation.
        private void canvas_MouseLeftButtonDown(object sender, MouseButtonEventArgs args)
        {
            Point targetPoint = args.GetPosition(ContainerCanvas);
            targetPoint.X = targetPoint.X - (MyAnimatedObject.ActualWidth / 2);
            targetPoint.Y = targetPoint.Y - (MyAnimatedObject.ActualHeight / 2); 
            
            
            switch(selectedTransition)
            {
                case AnimationTransitionType.Linear:
                animateToDestinationUsingLinearAnimation(targetPoint);
                break;
                
                case AnimationTransitionType.Bounce:
                animateToDestinationUsingBounceAnimation(targetPoint);              
                break;   
                
                case AnimationTransitionType.Elastic:
                animateToDestinationUsingElasticAnimation(targetPoint);              
                break;                
                     
            }      
        }
   
        // Animates to the target point using a standard
        // DoubleAnimation.
        private void animateToDestinationUsingLinearAnimation(Point targetPoint)
        {
            DoubleAnimation xAnimation = new DoubleAnimation();
            xAnimation.To = targetPoint.X;
            xAnimation.Duration = new Duration(TimeSpan.FromSeconds(5));
            DoubleAnimation yAnimation = new DoubleAnimation();
            yAnimation.To = targetPoint.Y;   
            yAnimation.Duration = new Duration(TimeSpan.FromSeconds(5));            
            MyAnimatedObject.BeginAnimation(Canvas.LeftProperty, xAnimation);         
            MyAnimatedObject.BeginAnimation(Canvas.TopProperty, yAnimation);        
        }
        

        // Animates to the target point using a custom
        // BouncAnimation.
        private void animateToDestinationUsingBounceAnimation(Point targetPoint)
        {
        
            CustomAnimations.BounceDoubleAnimation bounceXAnimation = 
                new  CustomAnimations.BounceDoubleAnimation();
            bounceXAnimation.From = Canvas.GetLeft(MyAnimatedObject);
            bounceXAnimation.To = targetPoint.X; 
            bounceXAnimation.Duration = TimeSpan.FromSeconds(5);
            bounceXAnimation.EdgeBehavior = CustomAnimations.BounceDoubleAnimation.EdgeBehaviorEnum.EaseIn;
            MyAnimatedObject.BeginAnimation(Canvas.LeftProperty, bounceXAnimation);
            
            CustomAnimations.BounceDoubleAnimation bounceYAnimation = 
                new  CustomAnimations.BounceDoubleAnimation();
            bounceYAnimation.From = Canvas.GetTop(MyAnimatedObject);
            bounceYAnimation.To  = targetPoint.Y; 
            bounceYAnimation.Duration = TimeSpan.FromSeconds(5);
            bounceYAnimation.EdgeBehavior = CustomAnimations.BounceDoubleAnimation.EdgeBehaviorEnum.EaseIn;
            MyAnimatedObject.BeginAnimation(Canvas.TopProperty, bounceYAnimation);          
                    
        }
        
        // Animates to the target point using a custom
        // ElasticAnimation.        
        private void animateToDestinationUsingElasticAnimation(Point targetPoint)
        {
        
            CustomAnimations.ElasticDoubleAnimation elasticXAnimation = 
                new  CustomAnimations.ElasticDoubleAnimation();
            elasticXAnimation.From = Canvas.GetLeft(MyAnimatedObject);
            elasticXAnimation.To = targetPoint.X; 
            elasticXAnimation.Duration = TimeSpan.FromSeconds(5);
            elasticXAnimation.EdgeBehavior = CustomAnimations.ElasticDoubleAnimation.EdgeBehaviorEnum.EaseIn;
            MyAnimatedObject.BeginAnimation(Canvas.LeftProperty, elasticXAnimation);
            
            CustomAnimations.ElasticDoubleAnimation elasticYAnimation = 
                new  CustomAnimations.ElasticDoubleAnimation();
            elasticYAnimation.From = Canvas.GetTop(MyAnimatedObject);
            elasticYAnimation.To  = targetPoint.Y; 
            elasticYAnimation.Duration = TimeSpan.FromSeconds(5);
            elasticYAnimation.EdgeBehavior = CustomAnimations.ElasticDoubleAnimation.EdgeBehaviorEnum.EaseIn;
            MyAnimatedObject.BeginAnimation(Canvas.TopProperty, elasticYAnimation);          
                    
        }    
        
        // Sets the default animation transition mode.
        private void pageLoaded(object sender, RoutedEventArgs args)
        {
        
            LinearTransitionRadioButton.IsChecked = true;
        }        
        
        // Updates the cached animation transition.
        private void selectedTransitionChanged(object sender, RoutedEventArgs args)
        {
            string value = (String)((RadioButton)args.Source).Content;   
            selectedTransition = (AnimationTransitionType)Enum.Parse(typeof(AnimationTransitionType), value);
        
        }        
          
        
    }
    
    public enum AnimationTransitionType
    {
        Linear, Bounce, Elastic
    
    }
}
