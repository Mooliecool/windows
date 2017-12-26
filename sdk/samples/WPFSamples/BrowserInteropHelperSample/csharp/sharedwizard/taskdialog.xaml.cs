namespace SharedPages
{
    using System;
    using System.Windows;

    public partial class TaskDialog : System.Windows.Window
    {
        public TaskDialog(TaskData taskData)
        {
            InitializeComponent();

            // Bind task state to UI
            this.DataContext = taskData;
        }

        void okButton_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = true;
        }
    }
}