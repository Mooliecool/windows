using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;

namespace GetReferrerSample
{
    public partial class ReferredPage : Page
    {
        public ReferredPage()
        {
            InitializeComponent();

            this.referringPageTextBlock.Text = App.Referrer.OriginalString;
        }
    }
}