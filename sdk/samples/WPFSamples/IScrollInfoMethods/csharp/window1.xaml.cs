using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Documents;
using System.Windows.Navigation;
using System.Text;

namespace ScrollViewer_Methods
{
    public partial class Window1 : Window
    {
        private void onLoad(object sender, System.EventArgs e)
        {
            ((IScrollInfo)sp1).CanVerticallyScroll = true;
            ((IScrollInfo)sp1).CanHorizontallyScroll = true;
            ((IScrollInfo)sp1).ScrollOwner = sv1;
        }

        private void spLineUp(object sender, RoutedEventArgs e)
        {
            ((IScrollInfo)sp1).LineUp();
        }
        private void spLineDown(object sender, RoutedEventArgs e)
        {
            ((IScrollInfo)sp1).LineDown();
        }

        private void spLineRight(object sender, RoutedEventArgs e)
        {
            ((IScrollInfo)sp1).LineRight();
        }

        private void spLineLeft(object sender, RoutedEventArgs e)
        {
            ((IScrollInfo)sp1).LineLeft();
        }

        private void spPageUp(object sender, RoutedEventArgs e)
        {
            ((IScrollInfo)sp1).PageUp();
        }

        private void spPageDown(object sender, RoutedEventArgs e)
        {
            ((IScrollInfo)sp1).PageDown();
        }

        private void spPageRight(object sender, RoutedEventArgs e)
        {
            ((IScrollInfo)sp1).PageRight();
        }

        private void spPageLeft(object sender, RoutedEventArgs e)
        {
            ((IScrollInfo)sp1).PageLeft();
        }

        private void spMouseWheelDown(object sender, RoutedEventArgs e)
        {
            ((IScrollInfo)sp1).MouseWheelDown();
        }

        private void spMouseWheelUp(object sender, RoutedEventArgs e)
        {
            ((IScrollInfo)sp1).MouseWheelUp();
        }

        private void spMouseWheelLeft(object sender, RoutedEventArgs e)
        {
            ((IScrollInfo)sp1).MouseWheelLeft();
        }

        private void spMouseWheelRight(object sender, RoutedEventArgs e)
        {
            ((IScrollInfo)sp1).MouseWheelRight();
        }
    }
}