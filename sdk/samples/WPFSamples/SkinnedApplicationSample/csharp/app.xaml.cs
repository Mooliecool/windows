using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Media;

namespace SDKSample
{
    public partial class App : Application
    {
        void App_Startup(object sender, StartupEventArgs e)
        {
            Properties["Blue"] = (ResourceDictionary)Application.LoadComponent(new Uri("BlueSkin.xaml", UriKind.Relative));
            Properties["Yellow"] = (ResourceDictionary)Application.LoadComponent(new Uri("YellowSkin.xaml", UriKind.Relative));

            // Note: you can also use the following syntax:
            //   Skins["Yellow"] = new YellowSkin()
            // But only as long as you implement the ResourceDictionary using markup and code-behind,
            // use the x:Class attribute in markup, and call InitializeComponent() from code-behind, eg:
            //
            //   <!-- Markup -->
            //   <ResourceDictionary
            //     xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
            //     xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
            //     xmlns:local="clr-namespace:SDKSample" 
            //     x:Class="SDKSample.YellowSkin">
            //        ...
            //   </ResourceDictionary>
            //
            //   // Code-behind
            //   public partial class YellowSkin : ResourceDictionary
            //   {
            //     public YellowSkin() { InitializeComponent(); }
            //   }
        }
    }
}