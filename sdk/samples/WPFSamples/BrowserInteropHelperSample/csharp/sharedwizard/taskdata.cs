namespace SharedPages
{
    using System;
    
    /// <summary>
    /// Data that is collected by the task
    /// </summary>
    public class TaskData
    {
        string dataItem;
        
        public string DataItem {
            get { return this.dataItem; }
            set { this.dataItem = value; }
        }
    }
}
