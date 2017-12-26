/****************************** Module Header ******************************\
* Module Name:  MainPage.xaml.cs
* Project:      CSSL3FullScreen
* Copyright (c) Microsoft Corporation.
* 
* This example illustrates how to use the full screen feature of Silverlight 3 and
* what the keyboard limitation is in full screen mode.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 7/13/2009 02:00 PM Allen Chen Created
\***************************************************************************/

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

namespace CSSL3FullScreen
{
    public partial class MainPage : UserControl
    {
        public MainPage()
        {
            InitializeComponent();

            // Attach the Loaded event to hook up events on load stage.    
            this.Loaded += new RoutedEventHandler(MainPage_Loaded);

        }

        void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            // Attach events of SilverlightHost to subscribe the 
            // FullScreenChanged and Resized event.
            App.Current.Host.Content.FullScreenChanged += new EventHandler(Content_FullScreenChanged);
            App.Current.Host.Content.Resized += new EventHandler(Content_Resized);
        }

        void Content_Resized(object sender, EventArgs e)
        {
            // When content get resized, refresh TextBlockShowSize control to
            // show the size of the Silverlight plug-in.           
            RefreshTextBlockShowSize();
        }

        void Content_FullScreenChanged(object sender, EventArgs e)
        { 
            // When full screen mode changed, refresh TextBlockShowSize 
            // control to show the size of the Silverlight plug-in.
            RefreshTextBlockShowSize();
        }
        private void RefreshTextBlockShowSize()
        {
            // Show the size of the Silverlight plug-in on TextBlockShowSize 
            // control.
            this.TextBlockShowSize.Text = string.Format("{0}*{1}",
                    App.Current.Host.Content.ActualWidth,
                    App.Current.Host.Content.ActualHeight);
        }
        private void UserControl_KeyDown(object sender, KeyEventArgs e)
        {
            // Show the input key on TextBlockShowKeyboardInput control.
            this.TextBlockShowKeyboardInput.Text = e.Key.ToString();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            // Switch to full screen mode or embeded mode.
            App.Current.Host.Content.IsFullScreen =
                !App.Current.Host.Content.IsFullScreen;
        }
    }
}
