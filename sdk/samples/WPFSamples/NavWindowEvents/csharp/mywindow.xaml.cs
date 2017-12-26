using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace NavWindow_Events
{
  public partial class MyWindow : Page
  {
    NavigationWindow navWindow;
    bool IsNavigate = false;

    private void Init(object sender, EventArgs e)
    {
      app.Current.LoadCompleted += new LoadCompletedEventHandler(App_LoadCompleted);
      app.Current.Navigating += new NavigatingCancelEventHandler(App_Navigating);
      app.Current.Navigated += new NavigatedEventHandler(App_Navigated);
      app.Current.NavigationProgress += new NavigationProgressEventHandler(App_NavigationProgress);
      app.Current.NavigationStopped += new NavigationStoppedEventHandler(App_NavigationStopped);
      navWindow = (NavigationWindow) app.Current.MainWindow;
      navWindow.SizeToContent = SizeToContent.WidthAndHeight;
    }
    private void On_Click(object sender, RoutedEventArgs e)
    {
      if (sender == simplexaml)
      {
        displayFrame.Navigate(new System.Uri("Page1.xaml", UriKind.RelativeOrAbsolute));
      }
      else if (sender == simpleobject)
      {
        Page1 nextPage = new Page1();
        nextPage.InitializeComponent();
        displayFrame.Navigate(nextPage);
      }
      else if (sender == frames)
      {
        displayFrame.Navigate(new System.Uri("Page2.xaml", UriKind.RelativeOrAbsolute));
      }
      else if (sender == large)
      {
        displayFrame.Navigate(new System.Uri("Content.xaml", UriKind.RelativeOrAbsolute));
      }
      else if (sender == fragment)
      {
        displayFrame.Navigate(new System.Uri("content.xaml#SomeTarget", UriKind.RelativeOrAbsolute));
      }
      else if (sender == forward)
      {
        if (navWindow.CanGoForward)
          navWindow.GoForward();
        else
          MessageBox.Show("Nothing in the Forward stack");
      }
      else if (sender == back)
      {
        if (navWindow.CanGoBack)
          navWindow.GoBack();
        else
          MessageBox.Show("Nothing in the Back stack");
      }

      else if (sender == stop)
      {
        navWindow.StopLoading();
      }

      else if (sender == refresh)
      {
        navWindow.Refresh();
      }
    }

    private void displayFrame_Navigate(object sender, RequestNavigateEventArgs e)
    {
      eventText.Children.Clear();
      IsNavigate = true;
      TextBlock p0 = new TextBlock();
      TextBlock p1 = new TextBlock();
      p1.FontWeight = FontWeights.Black;
      p1.Text = "RequestNavigate";
      TextBlock p2 = new TextBlock();
      p2.Text = "Target: " + e.Target.ToString();
      TextBlock p3 = new TextBlock();
      if (e.Uri != null)
        p3.Text = "URI: " + e.Uri.ToString();
      else
        p3.Text = "URI: None";
      eventText.Children.Add(p0);
      eventText.Children.Add(p1);
      eventText.Children.Add(p2);
      eventText.Children.Add(p3);
    }
    private void App_LoadCompleted(object sender, NavigationEventArgs e)
    {
      TextBlock p0 = new TextBlock();
      TextBlock p1 = new TextBlock();
      p1.FontWeight = FontWeights.Black;
      p1.Text = "LoadCompleted";
      TextBlock p2 = new TextBlock();
      p2.Text = "Content: " + e.Content.ToString();
      TextBlock p3 = new TextBlock();
      p3.Text = "Navigator: " + e.Navigator.ToString();
      TextBlock p4 = new TextBlock();
      if (e.Content == null)
        p4.Text = "URI: " + e.Uri.ToString();
      else
        p4.Text = "URI: None";
      eventText.Children.Add(p0);
      eventText.Children.Add(p1);
      eventText.Children.Add(p2);
      eventText.Children.Add(p3);
      eventText.Children.Add(p4);
    }
    private void App_Navigating(object sender, NavigatingCancelEventArgs e)
    {
      if (!IsNavigate)
        eventText.Children.Clear();
      else
        IsNavigate = false;
      TextBlock p1 = new TextBlock();
      p1.FontWeight = FontWeights.Bold;
      p1.Text = "Navigating";
      TextBlock p2 = new TextBlock();
      if (e.Content == null)
        p2.Text = "Content: null";
      else
        p2.Text = "Content: " + e.Content.ToString();
      TextBlock p3 = new TextBlock();
      p3.Text = "Navigation Mode: " + e.NavigationMode.ToString();
      TextBlock p4 = new TextBlock();
      if(e.Content == null)
        p4.Text = "URI: " + e.Uri.ToString();
      else
        p4.Text = "URI: None";      
      eventText.Children.Add(p1);
      eventText.Children.Add(p2);
      eventText.Children.Add(p3);
      eventText.Children.Add(p4);
    }
    private void App_Navigated(object sender, NavigationEventArgs e)
    {
      TextBlock p0 = new TextBlock();
      TextBlock p1 = new TextBlock();
      p1.FontWeight = FontWeights.Black;
      p1.Text = "Navigated";
      TextBlock p2 = new TextBlock();
      p2.Text = "Content: " + e.Content.ToString();
      TextBlock p3 = new TextBlock();
      p3.Text = "Navigator: " + e.Navigator.ToString();
      TextBlock p4 = new TextBlock();
      if (e.Content == null)
        p4.Text = "URI: " + e.Uri.ToString();
      else
        p4.Text = "URI: None";
      eventText.Children.Add(p0);
      eventText.Children.Add(p1);
      eventText.Children.Add(p2);
      eventText.Children.Add(p3);
      eventText.Children.Add(p4);
    }
    private void App_NavigationProgress(object sender, NavigationProgressEventArgs e)
    {
      TextBlock p0 = new TextBlock();
      TextBlock p1 = new TextBlock();
      p1.FontWeight = FontWeights.Bold;
      p1.Text = "NavigationProgress";
      TextBlock p2 = new TextBlock();
      p2.Text = "Bytes Read: " + e.BytesRead.ToString();
      TextBlock p3 = new TextBlock();
      p3.Text = "MaxBytes: " + e.MaxBytes.ToString();
      TextBlock p4 = new TextBlock();
      if (e.Uri == null)
        p4.Text = "URI: " + e.Uri.ToString();
      else
        p4.Text = "URI: None";
      eventText.Children.Add(p0);
      eventText.Children.Add(p1);
      eventText.Children.Add(p2);
      eventText.Children.Add(p3);
      eventText.Children.Add(p4);
    }
    private void App_NavigationStopped(object sender, NavigationEventArgs e)
    {
      TextBlock p0 = new TextBlock();
      TextBlock p1 = new TextBlock();
      p1.FontWeight = FontWeights.Black;
      p1.Text = "NavigationStopped";
      TextBlock p2 = new TextBlock();
      if (e.Content == null)
        p2.Text = "Content: None";
      else
        p2.Text = "Content: " + e.Content.ToString();
      TextBlock p3 = new TextBlock();
      p3.Text = "Navigator: " + e.Navigator.ToString();
      eventText.Children.Add(p0);
      eventText.Children.Add(p1);
      eventText.Children.Add(p2);
      eventText.Children.Add(p3);
    }
  }
}