using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Navigation;

namespace IPCCSSampleCSharp
{
    public partial class HomePage: Page, IProvideCustomContentState
    {
        public HomePage()
        {
            InitializeComponent();

            // Display initial read count
            this.readCountLabel.Content = 1;
        }

        CustomContentState IProvideCustomContentState.GetContentState()
        {
            // Store vertical position of scroll viewer with navigation history entry
            // before being navigated to next page.
            return new CustomPageContentState(this.documentScrollViewer.VerticalOffset, int.Parse(this.readCountLabel.Content.ToString()));
        }
    }
}