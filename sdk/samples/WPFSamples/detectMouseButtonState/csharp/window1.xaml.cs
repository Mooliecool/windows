using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Input;


namespace SDKSamples
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

        void HandleButtonDown(object sender, MouseButtonEventArgs e)
        {
            //Casting the source to a StackPanel
            StackPanel sourceStackPanel = e.Source as StackPanel;

            //If the button is pressed then make dimensions larger.
            if (e.ButtonState == MouseButtonState.Pressed)
            {
                sourceStackPanel.Width = 200;
                sourceStackPanel.Height = 200;
            }

            //If the button is released then make dimensions smaller.
            else if (e.ButtonState == MouseButtonState.Released)
            {
                sourceStackPanel.Width = 100;
                sourceStackPanel.Height = 100;
            }
        }

    }

}