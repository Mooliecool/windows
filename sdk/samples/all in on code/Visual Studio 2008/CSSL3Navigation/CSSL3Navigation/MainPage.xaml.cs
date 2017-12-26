/****************************** Module Header ******************************\
* Module Name:              MainPage.xaml.cs
* Project:                  CSSL3Navigation
* Copyright (c) Microsoft Corporation.
* 
* MainPage.xaml code behind file.
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
using CSSL3Navigation.Views;

namespace CSSL3Navigation
{
    public partial class MainPage : UserControl
    {
        public MainPage()
        {
            InitializeComponent();
        }

        // When Frame navigated, traverse the mainpage's hyperlink
        // button and highlight current visible page's link button.
        private void Frame_Navigated(object sender, System.Windows.Navigation.NavigationEventArgs e)
        {
            foreach (var ctl in LinkStackPanel.Children)
            {
                if (ctl is HyperlinkButton)
                {
                    var hlb = ctl as HyperlinkButton;
                    if (hlb.NavigateUri.ToString().Equals(e.Uri.ToString()))
                    {
                        hlb.FontWeight = FontWeights.Bold;
                    }
                    else
                    {
                        hlb.FontWeight = FontWeights.Normal;
                    }
                }
            }
        }

        // If frame naviagate failed, popup an error window.
        private void Frame_NavigationFailed(object sender, System.Windows.Navigation.NavigationFailedEventArgs e)
        {
            e.Handled = true;
            new ErrorWindow(e.Exception.Message).Show();
        }
    }
}
