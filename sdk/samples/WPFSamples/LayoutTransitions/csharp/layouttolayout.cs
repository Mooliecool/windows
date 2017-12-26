using System;
using System.Windows;
using System.Windows.Data;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Controls;
using System.Diagnostics;
using System.Collections;
using System.Windows.Threading;

namespace LayoutToLayout
{

    public class LayoutToLayoutTarget : Border
    {
        /*
         * Create a dependency property for binding the host to the target
         * */
        public static readonly DependencyProperty HostProperty =
            DependencyProperty.Register("Host", typeof(LayoutToLayoutHost), typeof(LayoutToLayoutTarget), null);

        public LayoutToLayoutHost Host
        {
            get
            {   return (LayoutToLayoutHost)GetValue(HostProperty);  }
            set
            {   SetValue(HostProperty, value);  }
        }

        /*
         * If the layout changes for the target (the object that actually gets passed between layout schemes)
         * then let the host know so that it can update itself if it wants to (i.e. if it's not in the middle of
         * an animation).
         * */
        public LayoutToLayoutTarget()
        {
            LayoutUpdated += UpdateHost;
        }

        void UpdateHost(object sender, EventArgs e)
        {
            if (Host != null)
                Host.UpdateFromTarget();
        }
    }

    public class LayoutToLayoutHost : Border
    {
        //Should I animate my way to future layout changes?
        public bool AnimateChanges = false;

        //Am I currently in the middle of an animation?
        public bool Animating = false;

        //Animation duration
        int timeSpan = 500;

        //Is this a fade-out animation?
        private bool DisappearOnCompletion = false;

        //This is kept as a member object so that local animations can affect it
        private TranslateTransform translation;

        //This keeps track of time for when all the animations finish
        private DispatcherTimer Refresher;

        public static readonly DependencyProperty TargetProperty =
            DependencyProperty.Register("Target", typeof(LayoutToLayoutTarget), typeof(LayoutToLayoutHost), null);

        public LayoutToLayoutTarget Target
        {
            get
            {   return (LayoutToLayoutTarget)GetValue(TargetProperty);  }
            set
            {   SetValue(TargetProperty, value);    }
        }


        public LayoutToLayoutHost()
        {
            Loaded += OnLoad;
        }

        void OnLoad(object sender, RoutedEventArgs e)
        {
            if (Target != null)
            {
                Debug.WriteLine("Binding Target!!!");
                BindToTarget(Target);
            }
            else
                Debug.WriteLine("Target was NULL!");

            Unloaded += OnUnload;   //this gets done here rather in the ctor to avoid a frame bug (Windows OS 1224171)
        }

        /*
         * If the host gets unloaded, break the cyclical reference to make sure the GC does its job
         * */
        void OnUnload(object sender, RoutedEventArgs e)
        {
            Target.Host = null;
            Target = null;
        }

        /*
         * Link up the Host to the Target and initialize but do not start the timer
         * */
        public void BindToTarget(LayoutToLayoutTarget t)
        {
            Target = t;
            t.Host = this;
            translation = new TranslateTransform(0, 0);
            RenderTransform = translation;

            Refresher = new DispatcherTimer();
            Refresher.Interval = TimeSpan.FromMilliseconds(timeSpan);
            Refresher.Tick += OnAnimStateInvalidated;

            UpdateFromTarget();
        }

        /*
         * Start a new animation
         * */
        public void BeginAnimating(bool Disappear)
        {
            if (Animating == true)
            {
                EndAnimations();
                //numActive -= 4;
            }

            Animating = false;
            AnimateChanges = true;

            DisappearOnCompletion = Disappear;

            if (Visibility != Visibility.Visible)
            {
                Visibility = Visibility.Visible;
                Opacity = 0.0;
            }
        }

        /*
         * Do an immediate update, as long as there is not an animation running
         * */
        public void UpdateFromTarget()
        {
            if ((Target == null) || (Animating == true))
                return;

            if (AnimateChanges)
                AnimateFromTarget();
            else
                MatchLayout();

        }

        /*
         * The double-check might not be necessary anymore, but this fixed a layout infinite loop
         * */
        private void MatchLayout()
        {
            if (Width != Target.ActualWidth)
                Width = Target.ActualWidth;

            if (Height != Target.ActualHeight)
                Height = Target.ActualHeight;

            System.Windows.Point pt = Target.TranslatePoint(new System.Windows.Point(0, 0), Parent as UIElement);

            TranslateTransform t = RenderTransform as TranslateTransform;

            if (Math.Abs(t.X - pt.X) > 1)
                t.X = pt.X;

            if (Math.Abs(t.Y - pt.Y) > 1)
                t.Y = pt.Y;
        }

        /*
         * Make a local animation for each animated property
         * Base the destination on the new layout position of the target
         * */
        private void AnimateFromTarget()
        {
            int time = timeSpan;
            Animating = true;
            AnimateChanges = false;

            System.Windows.Point pt = Target.TranslatePoint(new System.Windows.Point(0, 0), Parent as UIElement);
            TranslateTransform t = RenderTransform as TranslateTransform;

            BeginAnimation(LayoutToLayoutHost.WidthProperty, SetupDoubleAnimation(Width, Target.ActualWidth, time));
            BeginAnimation(LayoutToLayoutHost.HeightProperty, SetupDoubleAnimation(Height, Target.ActualHeight, time));
            translation.BeginAnimation(TranslateTransform.XProperty, SetupDoubleAnimation(t.X, pt.X, time));
            translation.BeginAnimation(TranslateTransform.YProperty, SetupDoubleAnimation(t.Y, pt.Y, time));

            if (DisappearOnCompletion == true)
                BeginAnimation(LayoutToLayoutHost.OpacityProperty, SetupDoubleAnimation(Opacity, 0.0, time));
            else
                BeginAnimation(LayoutToLayoutHost.OpacityProperty, SetupDoubleAnimation(Opacity, 1.0, time));


            Refresher.IsEnabled = false;        //this restarts the timer
            Refresher.IsEnabled = true;
            Refresher.Start();
        }

        /*
         * This gets called by the DispatcherTimer Refresher when it is done
         * */
        void OnAnimStateInvalidated(object sender, EventArgs e)
        {
            Animating = false;
            Refresher.IsEnabled = false;
            Refresher.Stop();

            EndAnimations();

            if (DisappearOnCompletion == true)
                Visibility = Visibility.Hidden;
        }

        /*
         * Explicitly replace all of the local animations will null
         * */
        public void EndAnimations()
        {
            double xBuffer = translation.X;
            double yBuffer = translation.Y;
            double widthBuffer = Width;
            double heightBuffer = Height;
            double opacityBuffer = Opacity;

            BeginAnimation(LayoutToLayoutHost.WidthProperty, null);
            BeginAnimation(LayoutToLayoutHost.HeightProperty, null);
            translation.BeginAnimation(TranslateTransform.XProperty, null);
            translation.BeginAnimation(TranslateTransform.YProperty, null);
            BeginAnimation(LayoutToLayoutHost.OpacityProperty, null);

            translation.X = xBuffer;
            translation.Y = yBuffer;
            Height = heightBuffer;
            Width = widthBuffer;
            Opacity = opacityBuffer;
        }

        /*
         * Helper function to create a DoubleAnimation
         * */
        public DoubleAnimation SetupDoubleAnimation(double From, double To, int time)
        {
            DoubleAnimation myDoubleAnimation = new DoubleAnimation();
            myDoubleAnimation.From = From;
            myDoubleAnimation.To = To;
            myDoubleAnimation.Duration = new Duration(TimeSpan.FromMilliseconds(time));
            myDoubleAnimation.AutoReverse = false;

            return myDoubleAnimation;
        }

        /*
         * Break the cyclical reference
         * */
        public void ReleaseFromTarget()
        {
            Target.Host = null;
            Target = null;
        }
    }
}