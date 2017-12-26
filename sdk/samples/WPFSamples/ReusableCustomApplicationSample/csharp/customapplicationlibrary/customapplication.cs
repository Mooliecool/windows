using System;
using System.Windows;

namespace CustomApplicationLibrary
{
    public class CustomApplication : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);
            MessageBox.Show("Hello, reusable custom application!");
        }
    }
}