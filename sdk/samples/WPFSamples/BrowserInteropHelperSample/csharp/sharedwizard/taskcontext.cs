namespace SharedPages
{
    using System;
    using System.Collections.ObjectModel;
    using System.Windows.Navigation;

    /// <summary>
    /// Used to manage the state of a particular task, including:
    ///    1) If completed, whether it was accepted or canceled (.Result)
    ///    2) The data that was collected by the task (.Data).
    /// </summary>
    public class TaskContext
    {
        TaskResult result;
        object data;

        public TaskContext(TaskResult result, object data) {
            this.result = result;
            this.data = data;
        }
        
        public TaskResult Result
        {
            get { return this.result; }
            set { this.result = value; }
        }

        public object Data
        {
            get { return this.data; }
            set { this.data = value; }
        }
    }
}
