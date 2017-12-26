/****************************** Module Header ******************************\
* Module Name:  MainPage.xaml.cs
* Project:      CSSL4FragmentSearch
* Copyright (c) Microsoft Corporation.
* 
* MainPage.xaml.cs is the primary code behind for the Silverlight component 
* in the sample.  It performs a simple search using the Bing API using 
* fragment navigation from a Silverlight frame, which allows bookmarking from 
* URL for search pages that will subsequently return consistent results.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections.ObjectModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using CSSL4FragmentSearch.Bing;

namespace CSSL4FragmentSearch
{
    public partial class MainPage : UserControl
    {
        ObservableCollection<WebResult> results = new ObservableCollection<WebResult>();

        public MainPage()
        {
            InitializeComponent();
            SearchResults.ItemsSource = results;
        }

        private void Frame_FragmentNavigation(object sender, System.Windows.Navigation.FragmentNavigationEventArgs e)
        {
            results.Clear();

            Bing.SearchRequest sr = new Bing.SearchRequest();
            sr.Query = e.Fragment + " (site:microsoft.com)";
            sr.AppId = "1009092976966EFB6DD6B0F0B98FE5E617990903";
            sr.Sources = new SourceType[] { SourceType.Web };
            sr.Web = new Bing.WebRequest();
            Bing.BingPortTypeClient bing = new BingPortTypeClient();
            bing.SearchCompleted += new EventHandler<SearchCompletedEventArgs>(bing_SearchCompleted);
            bing.SearchAsync(sr);
        }

        void bing_SearchCompleted(object sender, SearchCompletedEventArgs e)
        {
            if (e.Result.Web.Results != null)
            {
                foreach (WebResult wr in e.Result.Web.Results)
                    results.Add(wr);
            }
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            ContentFrame.Navigate(new Uri("#" + SearchText.Text, UriKind.Relative));
        }

        private void Link_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Uri uri = new Uri((sender as StackPanel).Tag as string);
            System.Windows.Browser.HtmlPage.Window.Navigate(uri);
        }
    }
}
