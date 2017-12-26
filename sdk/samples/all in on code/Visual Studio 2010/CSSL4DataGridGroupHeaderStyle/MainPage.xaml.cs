/****************************** Module Header ******************************\
Module Name:  MainPage.xaml.cs
Project:      CSSL4DataGridGroupHeaderSyle
Copyright (c) Microsoft Corporation.

DataGridGroupHeader solution codebehind file. 

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/


using System.Collections.ObjectModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Data;

namespace CSSL4DataGridGroupHeaderStyle
{
    public partial class MainPage : UserControl
    {
        public MainPage()
        {
            InitializeComponent();
            Loaded += new RoutedEventHandler(MainPage_Loaded);
        }

        void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            PagedCollectionView pcv = new PagedCollectionView(People.GetPeople());

            // Group person entity according to AgeGroup.
            pcv.GroupDescriptions.Add(new PropertyGroupDescription("AgeGroup"));

            // Group person entity accroding to Gender.
            pcv.GroupDescriptions.Add(new PropertyGroupDescription("Gender"));
            
            // Bind entity to DataGrid.
            PeopleList.ItemsSource = pcv;
        }

        public ObservableCollection<Style> RowGroupHeaderStyles 
        { get; set; }
    }
}
