using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;

namespace GetReferrerSample
{
    public partial class App : Application
    {
        private static Uri referrer;

        public static Uri Referrer
        {
            get { return referrer; }
        }

        protected override void OnNavigating(NavigatingCancelEventArgs e)
        {
            base.OnNavigating(e);

            referrer = null;

            object navigator = e.Navigator;
            if (navigator is NavigationWindow)
            {
                // Not a frame
                referrer = ((NavigationWindow)navigator).CurrentSource;
            }
            else if (navigator is Frame)
            {
                // A frame
                referrer = ((Frame)navigator).CurrentSource;
            }
        }

        protected override void OnNavigationStopped(NavigationEventArgs e)
        {
            base.OnNavigationStopped(e);
            referrer = null;
        }
    }
}