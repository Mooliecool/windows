using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Windows.Media.Animation;


namespace _360Timer
{

    public partial class Window1 : Window
    {
        System.Windows.Threading.DispatcherTimer frameTimer;
        private int lastTick;
        private Random rand;

        public Window1()
        {
            InitializeComponent();

            this.WindowState = WindowState.Maximized;
            this.WindowStyle = WindowStyle.None;
            this.ResizeMode = ResizeMode.NoResize;

            frameTimer = new System.Windows.Threading.DispatcherTimer();
            frameTimer.Tick += OnFrame;
            frameTimer.Interval = TimeSpan.FromSeconds(1.0 / 60.0);
            frameTimer.Start();

            this.lastTick = Environment.TickCount;

            rand = new Random(this.GetHashCode());

            this.Show();

            this.KeyDown += new System.Windows.Input.KeyEventHandler(Window1_KeyDown);

            CreateCircles();
        }

        void Window1_KeyDown(object sender, System.Windows.Input.KeyEventArgs e)
        {
            if (e.Key == System.Windows.Input.Key.Escape)
                this.Close();
        }

        private void OnFrame(object sender, EventArgs e)
        {
        }

        private void CreateCircles()
        {
            double centerX = this.MainCanvas.ActualWidth / 2.0;
            double centerY = this.MainCanvas.ActualHeight / 2.0;

            Color[] colors = new Color[] { Colors.White, Colors.Green, Colors.Green, Colors.Lime };

            for (int i = 0; i < 24; ++i)
            {
                Ellipse e = new Ellipse();
                byte alpha = (byte)rand.Next(96,192);
                int colorIndex = rand.Next(4);
                e.Stroke = new SolidColorBrush(Color.FromArgb(alpha, colors[colorIndex].R, colors[colorIndex].G, colors[colorIndex].B));
                e.StrokeThickness = rand.Next(1, 4);
                e.Width = 0.0;
                e.Height = 0.0;
                double offsetX = 16 - rand.Next(32);
                double offsetY = 16 - rand.Next(32);

                this.MainCanvas.Children.Add(e);                

                e.SetValue(Canvas.LeftProperty, centerX + offsetX);
                e.SetValue(Canvas.TopProperty, centerY + offsetY);

                double duration = 6.0 + 10.0 * rand.NextDouble();
                double delay = 16.0 * rand.NextDouble();

                TranslateTransform offsetTransform = new TranslateTransform();
                
                DoubleAnimation offsetXAnimation = new DoubleAnimation(0.0, -256.0, new Duration(TimeSpan.FromSeconds(duration)));
                offsetXAnimation.RepeatBehavior = RepeatBehavior.Forever;
                offsetXAnimation.BeginTime = TimeSpan.FromSeconds(delay);
                offsetTransform.BeginAnimation(TranslateTransform.XProperty, offsetXAnimation);
                offsetTransform.BeginAnimation(TranslateTransform.YProperty, offsetXAnimation);

                e.RenderTransform = offsetTransform;


                DoubleAnimation sizeAnimation = new DoubleAnimation(0.0, 512.0, new Duration(TimeSpan.FromSeconds(duration)));
                sizeAnimation.RepeatBehavior = RepeatBehavior.Forever;
                sizeAnimation.BeginTime = TimeSpan.FromSeconds(delay);
                e.BeginAnimation(Ellipse.WidthProperty, sizeAnimation);
                e.BeginAnimation(Ellipse.HeightProperty, sizeAnimation);

                DoubleAnimation opacityAnimation = new DoubleAnimation(duration-1.0, 0.0, new Duration(TimeSpan.FromSeconds(duration)));
                opacityAnimation.BeginTime = TimeSpan.FromSeconds(delay);
                opacityAnimation.RepeatBehavior = RepeatBehavior.Forever;
                e.BeginAnimation(Ellipse.OpacityProperty, opacityAnimation);

            }
        }
    }
}