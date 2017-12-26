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
    public partial class AuthorDetail : Page
    {
        public AuthorDetail()
        {
            InitializeComponent();
        }

        // Executes when the user navigates to this page.
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Retrieve the Auther entity according to 'aname' querystring
            // value, then bind to view.
            if (NavigationContext.QueryString.ContainsKey("aname"))
            {
                this.DataContext = ((App)App.Current).SContext.AuthorInfos.FirstOrDefault(
                    a => a.Name == NavigationContext.QueryString["aname"]);
            }
        }

    }
}
