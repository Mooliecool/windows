namespace SharedPages
{
    using System;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Navigation;

    public partial class TaskPage : PageFunction<TaskResult>
    {
        public TaskPage(TaskData taskData)
        {
            InitializeComponent();

            // Bind task state to UI
            this.DataContext = taskData;
        }

        void cancelButton_Click(object sender, RoutedEventArgs e)
        {
            // Cancel the task and don't return any data
            OnReturn(new ReturnEventArgs<TaskResult>(TaskResult.Canceled));
        }

        void okButton_Click(object sender, RoutedEventArgs e)
        {
            // Finish the task and return bound data to calling page
            OnReturn(new ReturnEventArgs<TaskResult>(TaskResult.Finished));
        }
    }
}