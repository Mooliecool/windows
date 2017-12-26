using System;
using System.Threading;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Data;

namespace SDKSample
{
  internal class MyApp : Application
  {
    public TextBlock myText;
    public Button button;
    public Button button2;
    public MyData myDataObject;
    public Binding myBinding;
    public DockPanel dp;


    public void OnClick(Object obj, RoutedEventArgs args)
    {
      FrameworkElement fe = (FrameworkElement) obj;
      switch(fe.Name)
      {
        case "Clear":
          BindingOperations.ClearBinding(myText, TextBlock.TextProperty);
          break;

        case "Refresh":

            BindingOperations.ClearBinding(myText, TextBlock.TextProperty);
          //make a new source
            MyData myDataObject = new MyData(DateTime.Now);      
            Binding myBinding = new Binding("MyDataProperty");
            myBinding.Source = myDataObject;
            myText.SetBinding(TextBlock.TextProperty, myBinding);
          break;
      }
    }

    protected override void OnStartup(StartupEventArgs e)
    {
      RoutedEventHandler clickHandler = new RoutedEventHandler(OnClick);

      Window win = new Window();
      win.Width = 250;
      win.Height = 200;
      DockPanel root = new DockPanel();
      win.Content = root;
      root.Width = 200;
      root.Height = 150;

      dp = new DockPanel();
      DockPanel.SetDock(dp, Dock.Top);
      root.Children.Add(dp);

      button = new Button();
      button.Name = "Clear";
      button.Content = "Clear Binding";
      button.Width = 120;
      button.Height = 30;
      button.Click += clickHandler;
      DockPanel.SetDock(button, Dock.Top);
      dp.Children.Add(button);

      button2 = new Button();
      button2.Name = "Refresh";
      button2.Content = "Refresh Binding";
      button2.Width = 120;
      button2.Height = 30;
      button2.Click += clickHandler;
      DockPanel.SetDock(button2, Dock.Top);
      dp.Children.Add(button2);

      myText = new TextBlock();
      myText.Text = "no binding yet...";
      myText.Height = 35;
      myText.HorizontalAlignment = HorizontalAlignment.Center;
      DockPanel.SetDock(myText, Dock.Top);
      dp.Children.Add(myText);

      win.Show();
    }
  }

  

  internal sealed class MainEntry
  {
    [System.STAThread()]
    public static void Main()
    {
      MyApp thisApp = new MyApp();
      thisApp.Run();
    }
  }
}
