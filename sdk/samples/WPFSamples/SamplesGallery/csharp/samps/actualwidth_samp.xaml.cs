using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Navigation;
using System.Diagnostics;
using System.Windows.Media;
using System.Windows.Automation.Provider;

namespace ActualWidth_Samp
{
	public partial class Page1 : Page
	{
        ToolTip ttp;

        // This function checks the language filter settings to see which code to filter and also grays out tabs with no content
        public void checkLang(object sender, EventArgs e)
        {
            if (xcsharpCheck.Content == null) // grays out xaml + c# tab
            {
                xamlcsharp.Background = Brushes.Gainsboro;
                xamlcsharp.Foreground = Brushes.White;
                ttp = new ToolTip();
                ToolTipService.SetShowOnDisabled(xamlcsharp, true);
                ttp.Content = "This sample is not available in XAML + C#.";
                xamlcsharp.ToolTip = (ttp);
                xamlcsharp.IsEnabled = false;
            }
            else if (xcsharpCheck.Content != null)
            {
                xamlcsharp.IsEnabled = true;
            }

            if (xvbCheck.Content == null) // grays out xaml + vb tab
            {
                xamlvb.Background = Brushes.Gainsboro;
                xamlvb.Foreground = Brushes.White;
                ttp = new ToolTip();
                ToolTipService.SetShowOnDisabled(xamlvb, true);
                ttp.Content = "This sample is not available in XAML + Visual Basic.NET";
                xamlvb.ToolTip = (ttp);
                xamlvb.IsEnabled = false;
            }
            else if (xvbCheck.Content != null)
            {
                xamlvb.IsEnabled = true;
            }

            if (xaml.Content == null) // grays out xaml
            {
                xaml.IsEnabled = false;
                xaml.Background = Brushes.Gainsboro;
                xaml.Foreground = Brushes.White;
                ttp = new ToolTip();
                ToolTipService.SetShowOnDisabled(xaml, true);
                ttp.Content = "This sample is not available in XAML.";
                xaml.ToolTip = (ttp);
            }
            else if (xaml.Content != null)
            {
                xaml.IsEnabled = true;
            }

            if (csharp.Content == null) // grays out c#
            {
                csharp.IsEnabled = false;
                csharp.Background = Brushes.Gainsboro;
                csharp.Foreground = Brushes.White;
                ttp = new ToolTip();
                ToolTipService.SetShowOnDisabled(csharp, true);
                ttp.Content = "This sample is not available in C#.";
                csharp.ToolTip = (ttp);
            }
            else if (csharp.Content != null)
            {
                csharp.IsEnabled = true;
            }

            if (vb.Content == null) // grays out vb
            {
                vb.IsEnabled = false;
                vb.Background = Brushes.Gainsboro;
                vb.Foreground = Brushes.White;
                ttp = new ToolTip();
                ToolTipService.SetShowOnDisabled(vb, true);
                ttp.Content = "This sample is not available in Visual Basic.NET.";
                vb.ToolTip = (ttp);
            }
            else if (vb.Content != null)
            {
                vb.IsEnabled = true;
            }

            if (managedcpp.Content == null) // grays out cpp
            {
                managedcpp.IsEnabled = false;
                managedcpp.Background = Brushes.Gainsboro;
                managedcpp.Foreground = Brushes.White;
                ttp = new ToolTip();
                ToolTipService.SetShowOnDisabled(managedcpp, true);
                ttp.Content = "This sample is not available in Managed C++.";
                managedcpp.ToolTip = (ttp);
            }
            else if (managedcpp.Content != null)
            {
                managedcpp.IsEnabled = true;
            }
            if (Welcome.Page1.myDouble == 1) // XAML only filtering
               {
               xaml.Visibility = Visibility.Visible;
               csharp.Visibility = Visibility.Collapsed;
               vb.Visibility = Visibility.Collapsed;
               managedcpp.Visibility = Visibility.Collapsed;
               xamlcsharp.Visibility = Visibility.Collapsed;
               xamlvb.Visibility = Visibility.Collapsed;
               }
               else if (Welcome.Page1.myDouble == 2) // CSharp
               {
               csharp.Visibility = Visibility.Visible;
               xaml.Visibility = Visibility.Collapsed;
               vb.Visibility = Visibility.Collapsed;
               managedcpp.Visibility = Visibility.Collapsed;
               xamlcsharp.Visibility = Visibility.Collapsed;
               xamlvb.Visibility = Visibility.Collapsed;
               }
               else if (Welcome.Page1.myDouble == 3) // Visual Basic
               {
               vb.Visibility = Visibility.Visible;
               xaml.Visibility = Visibility.Collapsed;
               csharp.Visibility = Visibility.Collapsed;
               managedcpp.Visibility = Visibility.Collapsed;
               xamlcsharp.Visibility = Visibility.Collapsed;
               xamlvb.Visibility = Visibility.Collapsed;
               }
               else if (Welcome.Page1.myDouble == 4) // Managed CPP
               {
               managedcpp.Visibility = Visibility.Visible;
               xaml.Visibility = Visibility.Collapsed;
               csharp.Visibility = Visibility.Collapsed;
               vb.Visibility = Visibility.Collapsed;
               xamlcsharp.Visibility = Visibility.Collapsed;
               xamlvb.Visibility = Visibility.Collapsed;
               }
               else if (Welcome.Page1.myDouble == 5) // No Filter
               {
               xaml.Visibility = Visibility.Visible;
               csharp.Visibility = Visibility.Visible;
               vb.Visibility = Visibility.Visible;
               managedcpp.Visibility = Visibility.Visible;
               xamlcsharp.Visibility = Visibility.Visible;
               xamlvb.Visibility = Visibility.Visible;
               }
               else if (Welcome.Page1.myDouble == 6) // XAML + CSharp
               {
               xaml.Visibility = Visibility.Collapsed;
               csharp.Visibility = Visibility.Collapsed;
               vb.Visibility = Visibility.Collapsed;
               managedcpp.Visibility = Visibility.Collapsed;
               xamlcsharp.Visibility = Visibility.Visible;
               xamlvb.Visibility = Visibility.Collapsed;
               }
               else if (Welcome.Page1.myDouble == 7) // XAML + VB
               {
               xaml.Visibility = Visibility.Collapsed;
               csharp.Visibility = Visibility.Collapsed;
               vb.Visibility = Visibility.Collapsed;
               managedcpp.Visibility = Visibility.Collapsed;
               xamlcsharp.Visibility = Visibility.Collapsed;
               xamlvb.Visibility = Visibility.Visible;
               }
         }
        // End mega function

        public void changeWidth(object sender, RoutedEventArgs args)
        {
            ListBoxItem li = ((sender as ListBox).SelectedItem as ListBoxItem);
            Double sz1 = Double.Parse(li.Content.ToString());
            rect1.Width = sz1;
            rect1.UpdateLayout();
            txt1.Text = "ActualWidth is set to " + rect1.ActualWidth;
            txt2.Text = "Width is set to " + rect1.Width;
            txt3.Text = "MinWidth is set to " + rect1.MinWidth;
            txt4.Text = "MaxWidth is set to " + rect1.MaxWidth;
        }
        public void changeMinWidth(object sender, RoutedEventArgs args)
        {
            ListBoxItem li = ((sender as ListBox).SelectedItem as ListBoxItem);
            Double sz1 = Double.Parse(li.Content.ToString());
            rect1.MinWidth = sz1;
            rect1.UpdateLayout();
            txt1.Text = "ActualWidth is set to " + rect1.ActualWidth;
            txt2.Text = "Width is set to " + rect1.Width;
            txt3.Text = "MinWidth is set to " + rect1.MinWidth;
            txt4.Text = "MaxWidth is set to " + rect1.MaxWidth;
        }
        public void changeMaxWidth(object sender, RoutedEventArgs args)
        {
            ListBoxItem li = ((sender as ListBox).SelectedItem as ListBoxItem);
            Double sz1 = Double.Parse(li.Content.ToString());
            rect1.MaxWidth = sz1;
            rect1.UpdateLayout();
            txt1.Text = "ActualWidth is set to " + rect1.ActualWidth;
            txt2.Text = "Width is set to " + rect1.Width;
            txt3.Text = "MinWidth is set to " + rect1.MinWidth;
            txt4.Text = "MaxWidth is set to " + rect1.MaxWidth;
        }

        public void clipRect(object sender, RoutedEventArgs args)
        {
            myCanvas.ClipToBounds = true;
            txt5.Text = "Canvas.ClipToBounds is set to " + myCanvas.ClipToBounds;
        }
        public void unclipRect(object sender, RoutedEventArgs args)
        {
            myCanvas.ClipToBounds = false;
            txt5.Text = "Canvas.ClipToBounds is set to " + myCanvas.ClipToBounds;
        }
    }
}