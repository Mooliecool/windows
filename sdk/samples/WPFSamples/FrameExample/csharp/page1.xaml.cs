using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Input;
using System.IO;
using System.Net;

namespace FrameExample
{
  /// <summary>
  /// Interaction logic for Page1.xaml
  /// </summary>

  public partial class Page1 : Page
  {
    // To use PageLoaded put Loaded="PageLoaded" in root element of .xaml file.
    // private void PageLoaded(object sender, EventArgs e) {}
    // Sample event handler:  
    // private void ButtonClick(object sender, RoutedEventArgs e) {}
    
    private void BrowseAHomePage(object sender, RoutedEventArgs e)
    {
      if ((Boolean)VisualBasic.IsChecked)
        myFrame.Navigate(new System.Uri("http://msdn.microsoft.com/vbasic/")); 
      else if ((Boolean)VisualCSharp.IsChecked)
        myFrame.Navigate(new System.Uri("http://msdn.microsoft.com/vcsharp/")); 
      else if ((Boolean)AnotherPage.IsChecked)
        myFrame.Navigate(new System.Uri("AnotherPage.xaml",
                         UriKind.RelativeOrAbsolute));
                  
     }
  }
}