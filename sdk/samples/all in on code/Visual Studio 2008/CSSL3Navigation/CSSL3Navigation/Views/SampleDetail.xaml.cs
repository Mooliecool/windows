using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Navigation;

namespace CSSL3Navigation.Views
{
    public partial class SampleDetail : Page
    {
        public SampleDetail()
        {
            InitializeComponent();
        }

        // Executes when the user navigates to this page.
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Retrieve the sample entity according to 'sname' querystring
            // value, then bind to view.
            if (NavigationContext.QueryString.ContainsKey("sname"))
            {
                this.DataContext = ((App)App.Current).SContext.Samples.FirstOrDefault(
                    s => s.Name == NavigationContext.QueryString["sname"]);
            }
        }

    }
}
