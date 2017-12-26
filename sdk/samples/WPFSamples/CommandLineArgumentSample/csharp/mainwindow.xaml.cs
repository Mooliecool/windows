namespace CommandLineArgumentSample
{
    using System;
    using System.Windows;

    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            foreach (string key in App.CommandLineArgs.Keys)
            {
                this.commandLineArgsListBox.Items.Add(key + ": " + App.CommandLineArgs[key]);
            }
        }
    }
}