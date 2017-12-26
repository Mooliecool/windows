//<SnippetCode>
using System;
using System.Windows;
using System.Windows.Data;

namespace GroupingSample
{
    public partial class Window1 : System.Windows.Window
    {

        public Window1()
        {
            InitializeComponent();
        }

        CollectionView myView;
        private void AddGrouping(object sender, RoutedEventArgs e)
        {
            myView = (CollectionView)CollectionViewSource.GetDefaultView(myItemsControl.ItemsSource);
            if (myView.CanGroup == true)
            {
                PropertyGroupDescription groupDescription
                    = new PropertyGroupDescription("@Type");
                myView.GroupDescriptions.Add(groupDescription);
            }
            else
                return;
        }

        private void RemoveGrouping(object sender, RoutedEventArgs e)
        {
            myView = (CollectionView)CollectionViewSource.GetDefaultView(myItemsControl.ItemsSource);
            myView.GroupDescriptions.Clear();
        }
    }
}
//</SnippetCode>