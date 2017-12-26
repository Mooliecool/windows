using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Net;
using System.IO;
using System.Windows.Navigation;
using System.Windows.Media.Imaging;


namespace TrafficAlert
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>

    public partial class Window1 : Window
    {
        public Window1()
        {
            InitializeComponent();
        }

        private void ButtonClick(object sender, RoutedEventArgs e)
        {

            w = new Window();
            StackPanel sp = new StackPanel();
            WrapPanel wp = new WrapPanel();
            tb = new TextBox();
            tb.MaxLength = 6;
           
            Label l = new Label();
            l.Foreground = Brushes.White;
            l.Content = "Enter ZipCode (OnlyDigits):";

            wp.Children.Add(l);
            wp.Children.Add(tb);
            wp.Background = Brushes.Transparent;

            b = new Button();
            b.Content = "OK";
            b.Click += new RoutedEventHandler(b_Click);

            GradientStop gs1 = new GradientStop(System.Windows.Media.Colors.Red, 0);
            GradientStop gs2 = new GradientStop(System.Windows.Media.Colors.Yellow, 1);
            GradientStopCollection gsc = new GradientStopCollection();
            gsc.Add(gs1);
            gsc.Add(gs2);
            LinearGradientBrush lgb = new LinearGradientBrush();
            lgb.StartPoint = new Point(0, 0);
            lgb.EndPoint = new Point(0, 1);
            lgb.GradientStops = gsc;
            b.Background = lgb;
            b.HorizontalAlignment = HorizontalAlignment.Center;
            b.Margin = new Thickness(4);

            sp.Children.Add(wp);
            sp.Children.Add(b);
            sp.Background = Brushes.Beige;

            GradientStop gs01 = new GradientStop(System.Windows.Media.Colors.Red, 0);
            GradientStop gs02 = new GradientStop(System.Windows.Media.Colors.Black, 1);
            GradientStopCollection gsc1 = new GradientStopCollection();
            gsc1.Add(gs01);
            gsc1.Add(gs02);
            LinearGradientBrush lgb1 = new LinearGradientBrush();
            lgb1.StartPoint = new Point(0, 0);
            lgb1.EndPoint = new Point(1, 1);
            lgb1.GradientStops = gsc1;
            sp.Background = lgb1;

            w.Content = sp;
            w.Height = 100; ;
            w.Width = 350;
            w.WindowStyle = WindowStyle.ToolWindow;
            w.Show();
            tb.Focus();
        }


        void b_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                int val = Int32.Parse(tb.Text);
                w.Close();
                
               // w1.Height = 600;
                w1.Height = 700;
                tv.Height = 600;

                _userZipCodeString = "http://maps.yahoo.com/traffic.rss?csz=" + val.ToString() + "&mag=5&minsev=1";
                tv.Items.Clear();
                tv.MinHeight = 200;
                getRSSfeed();
                w1.Width = (tv.ActualWidth > 400)? tv.ActualWidth: 400;
            }
            catch (FormatException)
            {
                tb.Text = "";
                w.Title = "Please Input Only numbers";
            }

        }


        // To use Loaded event put Loaded="WindowLoaded" attribute in root element of .xaml file.
        private void WindowLoaded(object sender, RoutedEventArgs e) 
        {
           getRSSfeed();
           TimerClock = TimerClock = new System.Windows.Threading.DispatcherTimer();
           TimerClock.Interval = new TimeSpan(1,0,0);
           TimerClock.IsEnabled = true;
           TimerClock.Tick += new EventHandler(TimerClock_Tick);
        }

        void TimerClock_Tick(object sender, EventArgs e)
        {
            tv.Items.Clear();
            getRSSfeed();
        }

        void tv_SizeChanged(object sender, SizeChangedEventArgs e)
        {
           // if (count++ != 0)
            {
                //if ((tv.ActualHeight ) > 600)
                //{
                //    tv.Height = 600;
                //}
                //    w1.Height = tv.ActualHeight + 100;
                tv.Height = 600;
                w1.Height = 700; 
                    if (sv.ViewportWidth <= sv.ExtentWidth)
                    {
                        w1.Width = tv.ActualWidth + 50;
                    }
                    w1.Width = (tv.ActualWidth > w1.Width) ? (tv.ActualWidth + 20) : w1.Width;

            }
        }

        void tv_MouseLeftButtonUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            w1.Height = tv.ActualHeight + 50;
        }

        private void getRSSfeed()
        {
            string url = "http://maps.yahoo.com/traffic.rss?csz=98052&mag=5&minsev=1";
            if (_userZipCodeString != "")
            {
                url = _userZipCodeString;
            }
            WebRequest req = WebRequest.Create(url);
            WebResponse res = req.GetResponse();

            Stream rsstream = res.GetResponseStream();
            System.Xml.XmlDataDocument rssdoc = new System.Xml.XmlDataDocument();

            rssdoc.Load(rsstream);

            System.Xml.XmlNodeList rssitems = rssdoc.SelectNodes("rss/channel/item");
            string title = "";
            string description = "";
            string link = "";
            string category = "";
            string severity = "";

            for (int i = 0; i < rssitems.Count; i++)
            {
                System.Xml.XmlNode rssdetail;

                rssdetail = rssitems.Item(i).SelectSingleNode("title");
                if (rssdetail != null)
                {
                    title = rssdetail.InnerText;

                    rssdetail = rssitems.Item(i).SelectSingleNode("description");
                    description = rssdetail.InnerText;

                    rssdetail = rssitems.Item(i).SelectSingleNode("link");


                    link = (rssdetail != null) ? rssdetail.InnerText : "";

                    rssdetail = rssitems.Item(i).SelectSingleNode("category");
                    category = (rssdetail != null) ?rssdetail.InnerText:"";

                    rssdetail = rssitems.Item(i).SelectSingleNode("severity");
                    severity = (rssdetail != null) ?rssdetail.InnerText:"";

                    PopulateList(title, description, link, category, severity);
                }
                else
                {
                    title = "";
                }
            }

            try
            {
                rssitems = rssdoc.SelectNodes("rss/channel");
                System.Xml.XmlNode rssdetail1;
                rssdetail1 = rssitems.Item(0).SelectSingleNode("title");
                title = rssdetail1.InnerText;
                int index = title.IndexOf("--");
                string str = (index > -1)?(title.Substring(index +3)):"Wrong Zip Code";
                Location.Content = str;
            }
            catch (Exception)
            {
            }

        }

        private void PopulateList(string title, string description, string link, string category, string severity)
        {
            title = title.Replace("\n", "");
            int index = title.IndexOf(", On ", 0);
            string str = (index > -1)? title.Substring(index + 5):"NA";

            string[] charr ={ " At " };
            string[] strarray = str.Split(charr, StringSplitOptions.None);
            str = strarray[0];

            int count = tv.Items.Count;
            bool found = false;
            int counter = 0;
            while ((found == false) && (counter < count))
            {
                if (((TreeViewItem)(tv.Items[counter])).Header.ToString() == str)
                {
                    found = true;
                    DockPanel dp = new DockPanel();
                    CreateDockPanelForTreeViewItem(title, description, severity, dp, category, link);
                    ((TreeViewItem)(tv.Items[counter])).Items.Add(dp);
                }
                counter++;
            }
            if (found == false)
            {
                TreeViewItem tvItem = new TreeViewItem();
                tvItem.Header = str;
                tvItem.Foreground = Brushes.White;
                DockPanel dp = new DockPanel();
                CreateDockPanelForTreeViewItem(title, description, severity, dp, category, link);
                tvItem.Items.Add(dp);
                tvItem.Expanded += new RoutedEventHandler(tvItem_Expanded);
                tvItem.Collapsed += new RoutedEventHandler(tvItem_Collapsed);
                tvItem.LostFocus += new RoutedEventHandler(tvItem_LostFocus);
                tv.Items.Add(tvItem);
            }
            return ;
        }

        void tvItem_Collapsed(object sender, RoutedEventArgs e)
        {
            TreeViewItem tvItem = sender as TreeViewItem;
            tvItem.Focus();
        }

        void tvItem_Expanded(object sender, RoutedEventArgs e)
        {
            TreeViewItem tvItem = sender as TreeViewItem;
            tvItem.Focus();
        }

        void tvItem_LostFocus(object sender, RoutedEventArgs e)
        {
            TreeViewItem tvItem = sender as TreeViewItem;
            tvItem.IsSelected = false;
        }

        private void CreateDockPanelForTreeViewItem(string title, string description, string severity, DockPanel dp, string category, string link)
        {
            
            Label tb = CreateNewLabel(title, severity);
            dp.Children.Add(GetImage(severity, category, tb.FontSize));

            ToolTip tp = new ToolTip();
            tp.Background = Brushes.Wheat;
            tp.Padding = new Thickness(4);
            tp.BorderBrush = Brushes.Gray; 
        
            StackPanel sp = new StackPanel(); 
            sp.Width = 250;
            
            description = description.Replace("\n", "");
            string[] strArr = new string[6];
            string[] splitArr = {"From Milepost", "Severity:", "Started: ", "Estimated End: ", "Last Updated: "};
            string _description = description;
            for (int i = splitArr.Length -1; i >= 0; i--)
            {
                string[] temp1 = { splitArr[i] };
                string[] strArr1 = _description.Split(temp1, StringSplitOptions.None);
                int index = _description.IndexOf(splitArr[i]);
                _description = (index > -1) ?_description.Substring(0, index):_description;
                strArr[i+1] = (strArr1.Length > 1) ? strArr1[1] : null;
                if (strArr1.Length > 1)
                {
                    strArr[0] = strArr1[0];
                }
                
            }
            TextBlock top = new TextBlock();
            top.TextWrapping = TextWrapping.Wrap;
            top.Background = Brushes.LightSteelBlue;
            top.FontSize = top.FontSize - 2;
            top.Text = title;

            TextBlock bottom = new TextBlock();
            bottom.TextWrapping = TextWrapping.Wrap;
            bottom.FontSize = bottom.FontSize - 3;
            bottom.Background = Brushes.Wheat;

            bottom.Inlines.Add(strArr[0] + "\r\n");

            for (int i = 1; i < strArr.Length; i++)
            {
                if (strArr[i] != null)
                {
                    bottom.Inlines.Add(new Bold(new Run(splitArr[i-1])));
                    bottom.Inlines.Add(strArr[i] + "\r\n");
                }
            }
            
            sp.Children.Add(top);
            sp.Children.Add(bottom);

            tp.Content = sp;
            tb.ToolTip = tp;

            ToolTipService.SetInitialShowDelay(tb, 600);
            ToolTipService.SetBetweenShowDelay(tb, 600);
            ToolTipService.SetShowDuration(tb, 30000);
            
            

            ((ToolTip)(tb.ToolTip)).Opened += new RoutedEventHandler(Window1_Opened);
            dp.Children.Add(tb);
            dp.Background = Brushes.Green;
            dp.Tag = link;
            dp.GotFocus += new RoutedEventHandler(dp_GotFocus);
            dp.LostFocus += new RoutedEventHandler(dp_LostFocus);
            dp.Focusable = true;
        }


        void dp_LostFocus(object sender, RoutedEventArgs e)
        {
            DockPanel dp = sender as DockPanel;
            ((ToolTip)(((Label)(dp.Children[1])).ToolTip)).IsOpen = false;
        }




        void dp_GotFocus(object sender, RoutedEventArgs e)
        {
            DockPanel dp = sender as DockPanel;
            ((Label)(dp.Children[1])).Focus();
            if (e.Source.ToString().Contains("System.Windows.Controls.Label")) 
            { }
            else
            {
                  ((ToolTip)(((Label)(dp.Children[1])).ToolTip)).IsOpen = true;
            }
        }



        void Window1_Opened(object sender, RoutedEventArgs e)
        {
            ToolTip tp = sender as ToolTip;
            tp.StaysOpen = true;
        }

        private static void SetColorCodeForLabel(string severity, Label label)
        {
            switch (severity)
            {
                case "Minor":
                case "Moderate":
                    LinearGradientBrush lgb = SetGradientColorHelper(System.Windows.Media.Colors.Yellow);
                    label.Background = lgb;
                    label.Tag = "moderate";
                    break;

                case "Major":
                    lgb = SetGradientColorHelper(System.Windows.Media.Colors.Orange);
                    label.Background = lgb;
                    label.Tag = "major";
                    break;

                case "Critical":
                    lgb = SetGradientColorHelper(System.Windows.Media.Colors.OrangeRed);
                    label.Background = lgb;
                    label.Tag = "critical";
                    break;

                default:
                    break;

            }

        }

        private static LinearGradientBrush SetGradientColorHelper( Color color)
        {
            GradientStop gs1 = new GradientStop(color, 0);
            GradientStop gs2 = new GradientStop(System.Windows.Media.Colors.WhiteSmoke, 1);
            GradientStopCollection gsc = new GradientStopCollection();
            gsc.Add(gs1);
            gsc.Add(gs2);
            LinearGradientBrush lgb = new LinearGradientBrush();
            lgb.StartPoint = new Point(0, 0);
            lgb.EndPoint = new Point(0, 1.10);
            lgb.GradientStops = gsc;
            return lgb;
        }

        private Image GetImage(string severity, string category, double Height)
        {
            switch (severity)
            {
                case "Minor":
                case "Moderate":
                    category = category.Trim();
                    if (category == "Construction")
                    {
                        return CreateImage(@"sampleImages\construction.gif", Height);
                    }
                    else
                    {
                        return CreateImage(@"sampleImages\minor.gif", Height);
                    }
                    break;

                case "Major":
                    return CreateImage(@"sampleImages\moderate.gif", Height);
                    break;

                case "Critical":
                    return CreateImage(@"sampleImages\severe.gif", Height);
                    break;

                default:
                    return CreateImage(@"sampleImages\green.gif", Height);
                    break;

            }
            return null;
            
        }

        private static Image CreateImage(string uri, double width)
        {
            Image simpleImage = new Image();
            simpleImage.Width = 200;
            simpleImage.Margin = new Thickness(0);
            BitmapImage bi = new BitmapImage(new Uri(uri, UriKind.Relative));

            //set image source
            simpleImage.Source = bi;
            simpleImage.Width = width;
            return simpleImage;
        }

        private static Label CreateNewLabel(string title, string severity)
        {
            Label label = new Label();
            label.Margin = new Thickness(0);
            label.Padding = new Thickness(0);
           // label.Content = title;
            Hyperlink hl = new Hyperlink(new Run(title));
            hl.Foreground = label.Foreground;
            

            label.Content = title;
            label.IsEnabled = true;
            label.Focusable = true;
            label.Cursor = System.Windows.Input.Cursors.Hand;
                //System.Windows.Input.CursorType.Hand;

            //label.MouseDoubleClick += new System.Windows.Input.MouseButtonEventHandler(label_MouseDoubleClick);
            label.GotFocus+=new RoutedEventHandler(label_GotFocus);
            label.LostFocus+=new RoutedEventHandler(label_LostFocus);
            label.MouseEnter += new System.Windows.Input.MouseEventHandler(label_MouseEnter);
            label.MouseLeave += new System.Windows.Input.MouseEventHandler(label_MouseLeave);
            label.MouseLeftButtonUp += new System.Windows.Input.MouseButtonEventHandler(label_MouseLeftButtonUp);
            SetColorCodeForLabel(severity, label);
            return label;
        }

        static void label_MouseLeave(object sender, System.Windows.Input.MouseEventArgs e)
        {
            Label label = sender as Label;
            ((ToolTip)(label.ToolTip)).IsOpen = false;
        }

        static void label_MouseLeftButtonUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            try
            {
                Label label = sender as Label;
                DockPanel dp = label.Parent as DockPanel;
                Uri url = new Uri(dp.Tag.ToString());
                NavigationWindow nw = new NavigationWindow();
                nw.Source = url;
                nw.Show();
            }
            catch (Exception)
            {
            }
        }

        static void label_MouseEnter(object sender, System.Windows.Input.MouseEventArgs e)
        {
            Label label = sender as Label;
            label.Focus();
        }

        static void label_LostFocus(object sender, RoutedEventArgs e)
        {
            Label label = sender as Label;
            ((ToolTip)(label.ToolTip)).IsOpen = false;
            if (label.Tag != null)
            {
                label.Background = label.Foreground;
                label.Foreground = Brushes.Black;
            }
            else
            {
                label.Background = Brushes.Green;
                label.Foreground = Brushes.Black;

            }
        }

        static void label_GotFocus(object sender, RoutedEventArgs e)
        {
            Label label = sender as Label;
            ((TreeView)(((TreeViewItem)((DockPanel)(label.Parent)).Parent).Parent)).Items.MoveCurrentTo(((TreeViewItem)((DockPanel)(label.Parent)).Parent));
            ((TreeView)(((TreeViewItem)((DockPanel)(label.Parent)).Parent).Parent)).Items.Refresh();
            ((ToolTip)(label.ToolTip)).Placement = System.Windows.Controls.Primitives.PlacementMode.Right;
            ((ToolTip)(label.ToolTip)).PlacementTarget = label;


            label.Foreground = label.Background;
            if (label.Tag != null)
            {
                label.Background = Brushes.Gray;
            }
            else
            {
                label.Background = Brushes.Yellow;
                label.Foreground = Brushes.Black;
            }

        }

        //static void label_MouseDoubleClick(object sender, System.Windows.Input.MouseButtonEventArgs e)
        //{
        //    try
        //    {
        //        Label label = sender as Label;
        //        DockPanel dp = label.Parent as DockPanel;
        //        Uri url = new Uri(dp.Tag.ToString());
        //        NavigationWindow nw = new NavigationWindow();
        //        nw.Source = url;
        //        nw.Show();
        //    }
        //    catch (Exception)
        //    {
        //    }
        //}

        private System.Windows.Threading.DispatcherTimer TimerClock;
        private string _userZipCodeString = "";
        private TextBox tb = null;
        private Window w = null;
        private Button b = null;
        
     }



}