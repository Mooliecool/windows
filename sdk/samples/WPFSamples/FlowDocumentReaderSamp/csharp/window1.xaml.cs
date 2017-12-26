using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Controls.Primitives;
using System.Windows.Input;


namespace SDKSample
{

    public partial class Window1 : Window
    {

        public Window1()
        {
            InitializeComponent();
        }

        void nextPageClick(Object sender, RoutedEventArgs args)
        {
            NavigationCommands.NextPage.Execute(null, flowDocRdr);
        }

        void prevPageClick(Object sender, RoutedEventArgs args)
        {
            NavigationCommands.PreviousPage.Execute(null, flowDocRdr);
        }

        void findClick(Object sender, RoutedEventArgs args)
        {
            flowDocRdr.Find();
        }

        void IsFindEnabled_t(Object sender, RoutedEventArgs args)
        {
            if (flowDocRdr == null) return;

            flowDocRdr.IsFindEnabled = true;
        }

        void IsFindEnabled_f(Object sender, RoutedEventArgs args)
        {
            if (flowDocRdr == null) return;

            flowDocRdr.IsFindEnabled = false;
        }

        void printClick(Object sender, RoutedEventArgs args)
        {
            flowDocRdr.Print();
        }

        void IsPageViewEnabled_t(Object sender, RoutedEventArgs args)
        {
            if (flowDocRdr == null) return;

            flowDocRdr.IsPageViewEnabled = true;
        }
        void IsScrollViewEnabled_t(Object sender, RoutedEventArgs args)
        {
            if (flowDocRdr == null) return;

            flowDocRdr.IsScrollViewEnabled = true;
        }

        void IsTwoPageViewEnabled_t(Object sender, RoutedEventArgs args)
        {
            if (flowDocRdr == null) return;

            flowDocRdr.IsTwoPageViewEnabled = true;
        }

        void IsPageViewEnabled_f(Object sender, RoutedEventArgs args)
        {
            if (flowDocRdr == null) return;

            if (flowDocRdr.ViewingMode == FlowDocumentReaderViewingMode.Page)
            {
                MessageBox.Show("Select a different viewing mode before disabling this one.");
                rbPageView_t.IsChecked = true;
                return;
            }

            if (flowDocRdr.IsScrollViewEnabled == false && flowDocRdr.IsTwoPageViewEnabled == false)
            {
                MessageBox.Show("At least one viewing mode must be enabled.");
                rbPageView_t.IsChecked = true;
                return;
            }

            flowDocRdr.IsPageViewEnabled = false;
        }

        void IsScrollViewEnabled_f(Object sender, RoutedEventArgs args)
        {
            if (flowDocRdr == null) return;

            if (flowDocRdr.ViewingMode == FlowDocumentReaderViewingMode.Scroll)
            {
                MessageBox.Show("Select a different viewing mode before disabling this one.");
                rbScrollView_t.IsChecked = true;
                return;
            }

            if (flowDocRdr.IsPageViewEnabled == false && flowDocRdr.IsTwoPageViewEnabled == false)
            {
                MessageBox.Show("At least one viewing mode must be enabled.");
                rbScrollView_t.IsChecked = true;
                return;
            }

            flowDocRdr.IsScrollViewEnabled = false;
        }

        void IsTwoPageViewEnabled_f(Object sender, RoutedEventArgs args)
        {
            if (flowDocRdr == null) return;

            if (flowDocRdr.ViewingMode == FlowDocumentReaderViewingMode.TwoPage)
            {
                MessageBox.Show("Select a different viewing mode before disabling this one.");
                rbTwoPageView_t.IsChecked = true;
                return;
            }

            if (flowDocRdr.IsScrollViewEnabled == false && flowDocRdr.IsPageViewEnabled == false)
            {
                MessageBox.Show("At least one viewing mode must be enabled.");
                rbTwoPageView_t.IsChecked = true;
                return;
            }

            flowDocRdr.IsTwoPageViewEnabled = false;
        }


    }
}