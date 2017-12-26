using System;
using System.Windows;

namespace CustomRichTextBoxSample
{
    /// <summary>
    /// Main class for this application.
    /// </summary>
    public partial class MyApp
    {
        public void AppStartup(object sender, EventArgs args)
        {
            MyWindow window = new MyWindow();
            window.InitializeComponent();
            window.Show();
        }
    }
}