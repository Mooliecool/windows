#define DEBUG

namespace LayoutToLayout
{
    using System;
    using System.Windows;
    using System.Windows.Data;
    using System.Windows.Media;
    using System.Windows.Media.Animation;
    using System.Windows.Controls;
    using System.Diagnostics;
    using System.Collections;
    using System.Windows.Threading;

    public partial class Page1
    {
        public ArrayList Targets = new ArrayList();
        public ArrayList Hosts = new ArrayList();
        int counter = 0;
        int gridSize = 5;
        int numItems = 8;
        public ArrayList Panels = new ArrayList();
        public ArrayList Buttons = new ArrayList();

        void OnLoad(object sender, RoutedEventArgs e)
        {
            Application app = System.Windows.Application.Current;

            Panels.Add(LTLGrid);
            Panels.Add(LTLGrid2);
            Panels.Add(LTLStackPanel);
            Panels.Add(LTLWrapPanel);

            Debug.WriteLine("Grid children: " + ButtonGrid.Children.Count);

            for (int i = 0; i < numItems; i++)
            {
                LayoutToLayoutTarget target = new LayoutToLayoutTarget();
                Targets.Add(target);
                target.Margin = new Thickness(5);
                target.MinWidth = 80;     target.MinHeight = 50;
                target.BorderThickness = new Thickness(0);

                Grid.SetRow(target, i / 5);
                Grid.SetColumn(target, i % 5);
                LTLGrid.Children.Add(target);

                LayoutToLayoutHost host = new LayoutToLayoutHost();
                Hosts.Add(host);
                host.BorderThickness = new Thickness(0);

                Button demoButton = new Button();
                demoButton.Content = "# " + i;
                demoButton.Style = app.Resources["ButtonStyle"] as Style;
                demoButton.Click += OnAdvanceClick;
                host.Child = demoButton;
                //host.Child = Buttons[i] as Button;
                
                Canvas.SetLeft(host, 0); Canvas.SetTop(host, 0);
                LTLCanvas.Children.Add(host);

                host.BindToTarget(target);
            }
        }

        /*
         * Increase the starting location in the grid by one
         * */
        void OnAdvanceClick(object sender, RoutedEventArgs e)
        {
            counter++;

            for (int i = 0; i < Hosts.Count; i++)
                MoveObject(Hosts[i] as LayoutToLayoutHost, counter + i);
        }

        /*
         * Decrease the starting location in the grid by one
         * */
        void OnRetreatClick(object sender, RoutedEventArgs e)
        {
            counter--;

            for (int i = 0; i < Hosts.Count; i++)
                MoveObject(Hosts[i] as LayoutToLayoutHost, counter + i);
        }

        /*
         * Set the Stackpanel's orientation to vertical
         * */
        void OnStackVertical(object sender, RoutedEventArgs e)
        {
            for (int i = 0; i < Hosts.Count; i++)
                (Hosts[i] as LayoutToLayoutHost).BeginAnimating(false);

            LTLStackPanel.Orientation = System.Windows.Controls.Orientation.Vertical;
        }

        /*
         * Set the stackpanel's orientation to vertical
         * */
        void OnStackHorizontal(object sender, RoutedEventArgs e)
        {
            for (int i = 0; i < Hosts.Count; i++)
                (Hosts[i] as LayoutToLayoutHost).BeginAnimating(false);

            LTLStackPanel.Orientation = System.Windows.Controls.Orientation.Horizontal;
        }

        /*
         * Set the wrappanel's orientation to vertical
         * */
        void OnWrapVertical(object sender, RoutedEventArgs e)
        {
            for (int i = 0; i < Hosts.Count; i++)
                (Hosts[i] as LayoutToLayoutHost).BeginAnimating(false);

            LTLWrapPanel.Orientation = System.Windows.Controls.Orientation.Vertical;
        }

        /*
         * Set the wrappanel's orientation to vertical
         * */
        void OnWrapHorizontal(object sender, RoutedEventArgs e)
        {
            for (int i = 0; i < Hosts.Count; i++)
                (Hosts[i] as LayoutToLayoutHost).BeginAnimating(false);

            LTLWrapPanel.Orientation = System.Windows.Controls.Orientation.Horizontal;
        }

        /*
         * Move all buttons into the 5x5 grid
         * */
        void OnGrid5(object sender, RoutedEventArgs e)
        {
            ClearPanels();
            gridSize = 5;
            counter = 0;

            for (int i = 0; i < numItems; i++)
            {
                LayoutToLayoutTarget target = Targets[i] as LayoutToLayoutTarget;
                (Hosts[i] as LayoutToLayoutHost).BeginAnimating(false);
                Grid.SetRow(target, i/5); 
                Grid.SetColumn(target, i%5);
                LTLGrid.Children.Add(target);
            }
        }

        /*
         * Move all buttons into the 3x3 grid
         * */
        void OnGrid3(object sender, RoutedEventArgs e)
        {
            ClearPanels();
            gridSize = 3;
            counter = 0;

            for (int i = 0; i < numItems; i++)
            {
                LayoutToLayoutTarget target = Targets[i] as LayoutToLayoutTarget;
                (Hosts[i] as LayoutToLayoutHost).BeginAnimating(false);
                Grid.SetRow(target, i / 3);
                Grid.SetColumn(target, i % 3);
                LTLGrid2.Children.Add(target);
            }

        }

        /*
         * Move all buttons into the stack panel
         * */
        void OnStackPanel(object sender, RoutedEventArgs e)
        {
            ClearPanels();
            for (int i = 0; i < numItems; i++)
            {
                LayoutToLayoutTarget target = Targets[i] as LayoutToLayoutTarget;
                (Hosts[i] as LayoutToLayoutHost).BeginAnimating(false);
                LTLStackPanel.Children.Add(target);
            }

        }

        /*
         * Move all buttons into the wrap panel
         * */
        void OnWrapPanel(object sender, RoutedEventArgs e)
        {
            ClearPanels();
            for (int i = 0; i < numItems; i++)
            {
                LayoutToLayoutTarget target = Targets[i] as LayoutToLayoutTarget;
                (Hosts[i] as LayoutToLayoutHost).BeginAnimating(false);
                LTLWrapPanel.Children.Add(target);
            }

        }


        /*
         * move an object from one grid cell to another
         * */
        void MoveObject(LayoutToLayoutHost obj, int position)
        {
            int max = gridSize * gridSize - 1;
            if (position > max)
                position = max;
            if(position < 0)
                position = 0;

            obj.BeginAnimating(false);
            Grid.SetRow(obj.Target, position / gridSize);
            Grid.SetColumn(obj.Target, position % gridSize);
        }

        /*
         * Remove all elements from all panels
         * */
        void ClearPanels()
        {
            for (int i = 0; i < Panels.Count; i++)
            {
                (Panels[i] as Panel).Children.Clear();
            }
        }
    }
}
