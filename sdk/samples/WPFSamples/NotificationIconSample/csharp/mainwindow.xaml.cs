namespace NotificationIconSample
{
    using System;
    using System.Windows;

    using System.Windows.Forms; // NotifyIcon control
    using System.Drawing; // Icon
   
    public partial class MainWindow : Window
    {
        NotifyIcon notifyIcon;

        public MainWindow()
        {
            InitializeComponent();
        }

        void click(object sender, RoutedEventArgs e)
        {
            // Configure and show a notification icon in the system tray
            this.notifyIcon = new NotifyIcon();
            this.notifyIcon.BalloonTipText = "Hello, NotifyIcon!";
            this.notifyIcon.Text = "Hello, NotifyIcon!";
            this.notifyIcon.Icon = new System.Drawing.Icon("NotifyIcon.ico");
            this.notifyIcon.Visible = true;
            this.notifyIcon.ShowBalloonTip(1000);
        }
    }
}