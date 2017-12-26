using System;
using System.Media;
using System.Globalization;
using System.Windows;
using System.ComponentModel;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Windows.Threading;
using System.Windows.Media.Animation;
using System.Windows.Media.Effects;
using System.Windows.Media.Imaging;


namespace SDKSample
{

    public partial class MediaTimelineExample : INotifyPropertyChanged
    {

        public MediaTimelineExample()
        {
            this.InitializeComponent();
        }

        DispatcherTimer timer;

        public void OnWindowLoaded(object sender, RoutedEventArgs e)
        {

            Binding b2 = new Binding();
            b2.Source = this;
            b2.Path = new PropertyPath("MyProp");

            // Bind to the slider and the textbox
            BindingOperations.SetBinding(ClockSlider, Slider.ValueProperty, b2);
            BindingOperations.SetBinding(PositionTextBox, TextBox.TextProperty, b2);

            timer = new DispatcherTimer();
            timer.Interval = new TimeSpan(0, 0, 0, 0, 100);

            // Every tick, the timer_Tick event handler is fired.
            timer.Tick += new EventHandler(timer_Tick);

        }

        void timer_Tick(object sender, EventArgs e)
        {
            OnPropertyChanged("MyProp");
        }


        public double MyProp
        {
            get
            {
                return ClickedBSB.Storyboard.GetCurrentTime(DocumentRoot).Value.TotalSeconds;
            }

            set
            {
                ClickedBSB.Storyboard.SeekAlignedToLastTick(DocumentRoot, new TimeSpan((long)Math.Floor(value * TimeSpan.TicksPerSecond)), TimeSeekOrigin.BeginTime);
                OnPropertyChanged("MyProp");
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private void OnPropertyChanged(string name)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(name));
            }
        }

        public void OnMediaOpened(object sender, RoutedEventArgs e)
        {
            if (MediaElement1.Clock != null)
            {
                StatusBar.Text = MediaElement1.Clock.NaturalDuration.ToString();
                ClockSlider.Maximum = MediaElement1.Clock.NaturalDuration.TimeSpan.TotalSeconds + 10;
            }
            timer.Start();
        }

        void mc_CurrentTimeInvalidated(object sender, EventArgs e)
        {
            StatusBar.Text = "CurrentStateInvalidated";
        }


        public object Convert(object o, Type type, object param, CultureInfo cul)
        {
            TimeSpan currPosition = (TimeSpan)o;
            double NumMSecs = currPosition.Milliseconds;

            double NewValue = (NumMSecs / MediaElement1.Clock.NaturalDuration.TimeSpan.TotalMilliseconds) * ClockSlider.Maximum;
            return NewValue;
        }

        public object ConvertBack(object o, Type type, object param, CultureInfo cul)
        {
            return null;
        }
    }

    public class PositionConverter : System.Windows.Data.IValueConverter
    {
        public object Convert(object o, Type type, object param, CultureInfo cul)
        {
            TimeSpan currPosition = (TimeSpan)o;
            return currPosition.Seconds;
        }

        public object ConvertBack(object o, Type type, object param, CultureInfo cul)
        {
            return null;
        }
    }

}