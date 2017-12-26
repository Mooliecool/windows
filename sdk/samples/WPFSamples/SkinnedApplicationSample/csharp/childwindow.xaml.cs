using System;
using System.Windows;

namespace SDKSample
{
    public partial class ChildWindow : Window
    {
        public ChildWindow()
        {
            InitializeComponent();

            // Bind Background property to "background" resource
            //this.SetResourceReference(Window.BackgroundProperty, "background");
        }
    }
}