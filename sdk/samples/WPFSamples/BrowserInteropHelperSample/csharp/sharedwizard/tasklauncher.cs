namespace SharedPages
{
    using System;
    using System.Windows;
    using System.Windows.Interop;
    using System.Windows.Navigation;

    public class TaskLauncher : PageFunction<TaskContext>
    {
        TaskData taskData = new TaskData();

        protected override void Start()
        {
            // Retain instance (and state) in navigation history until task is complete
            this.KeepAlive = true;
            this.WindowTitle = "Task Launcher";

            // Is this assembly running in a browser-hosted application (XBAP)?
            if (BrowserInteropHelper.IsBrowserHosted)
            {
                // If so, use browser-style UI
                UseTaskPageUI();
            }
            else
            {
                // If not, use window-style UI
                UseTaskDialogBoxUI();
            }
        }

        #region Task Page UI
        void UseTaskPageUI()
        {
            // Launch the task
            TaskPage taskPage = new TaskPage(this.taskData);
            taskPage.Return += new ReturnEventHandler<TaskResult>(taskPage_Return);
            this.NavigationService.Navigate(taskPage);
        }
        void taskPage_Return(object sender, ReturnEventArgs<TaskResult> e)
        {
            // Task was completed (finished or canceled), return TaskResult and TaskData
            OnReturn(new ReturnEventArgs<TaskContext>(new TaskContext(e.Result, this.taskData)));
        }
        #endregion

        #region Task Dialog Box UI
        void UseTaskDialogBoxUI()
        {
            // Create and show dialog box
            TaskDialog taskDialog = new TaskDialog(this.taskData);
            taskDialog.Owner = Application.Current.MainWindow;
            bool dialogResult = (bool)taskDialog.ShowDialog();
            TaskResult taskResult = (dialogResult == true ? TaskResult.Finished : TaskResult.Canceled);

            // Return results
            OnReturn(new ReturnEventArgs<TaskContext>(new TaskContext(taskResult, this.taskData)));
        }
        #endregion
    }
}
