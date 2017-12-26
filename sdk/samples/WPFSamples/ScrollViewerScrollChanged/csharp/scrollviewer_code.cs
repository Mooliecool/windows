using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Threading;

namespace ScrollViewer_Sample
{
    public class app : Application
    {
        Rectangle rect1;
        ScrollViewer scrView1;
        StackPanel sPanel;
        Window mainWindow;
        TextBlock txt1;
        TextBlock txt2;
        
        protected override void OnStartup (StartupEventArgs e)
        {
            base.OnStartup(e);
            CreateAndShowMainWindow ();
        }
        
        public void sChanged(object sender, ScrollChangedEventArgs e)
        {
            // Get value of computed scroll values and assign to a TextBlock
            txt1.Text = "ScrollViewer.HorizontalScrollBarVisibility is set to: " + scrView1.ComputedHorizontalScrollBarVisibility.ToString();
            
            //txt2 = new TextBlock();
            txt2.Text = "ScrollViewer.VerticalScrollBarVisibility is set to: " + scrView1.ComputedVerticalScrollBarVisibility.ToString();
        }
        
        public void CreateAndShowMainWindow()
        {
            // Create the application's main window
            mainWindow = new System.Windows.Window();
            mainWindow.Height = 400;
            mainWindow.Width = 400;
            

            // Create a ScrollViewer
            scrView1 = new ScrollViewer();
            txt1 = new TextBlock();
            txt2 = new TextBlock();
            scrView1.CanContentScroll = true;
            scrView1.HorizontalScrollBarVisibility = ScrollBarVisibility.Auto;
            scrView1.VerticalScrollBarVisibility = ScrollBarVisibility.Auto;

            // Create a StackPanel
            sPanel = new StackPanel();
            sPanel.Orientation = Orientation.Vertical;
            sPanel.Children.Add(txt1);
            sPanel.Children.Add(txt2);

            // Add the first rectangle to the StackPanel
            rect1 = new Rectangle();
            rect1.Stroke = Brushes.Black;
            rect1.Fill = Brushes.SkyBlue;
            rect1.Height = 400;
            rect1.Width = 400;
            sPanel.Children.Add (rect1);
            
            scrView1.ScrollChanged += sChanged;
            scrView1.Content= sPanel;
            mainWindow.Content = scrView1;
            mainWindow.Title = "ScrollViewer Sample";
            mainWindow.Show();
        }
    }
    
    internal sealed class EntryClass
    {
        [System.STAThread()]
        private static void Main()
        {
            app app = new app();
            app.Run();
        }
    }
}
