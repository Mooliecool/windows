/****************************** Module Header ******************************\
* Module Name:                MainPage.xaml.cs
* Project:                    CSSL4MEF
* Copyright (c) Microsoft Corporation.
* 
* MainPage's code behind file.
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
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.ComponentModel.Composition;
using System.ComponentModel;
using System.Windows.Data;

namespace CSSL4MEF
{
    public partial class MainPage : UserControl
    {
        public MainPage()
        {
            InitializeComponent();

            // Initialize and bind text's datamodel
            var config = new TextConfig
            {
                Text = "TEST",
                FontColor = Colors.Blue,
                FontSize = 16
            };
            this.DataContext = config;

            // Import the parts related to current instance.
            CompositionInitializer.SatisfyImports(this);
        }

        // Import CatalogService, utilize it for dynamic xap loading.
        [Import]
        public IDeploymentCatalogService CatalogService { set; get; }

        // Import ColorConfigControl when clicking button.
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            CatalogService.AddXap("ConfigControl.Extension.xap");
        }
    }
}
