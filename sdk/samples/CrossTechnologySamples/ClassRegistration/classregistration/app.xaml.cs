using System;
using System.Windows;
using System.Windows.Navigation;
using System.Data;
using System.Xml;
using System.Configuration;
using System.Workflow.Runtime;

namespace Microsoft.Samples.ClassRegistration
{
    public partial class App : Application
    {
        public App()
        {
            InitializeComponent();
        }

        private string userId;

        public string UserId
        {
            get { return userId; }
            set { userId = value; }
        }

        private WorkflowRuntime workflowRuntime;

        public WorkflowRuntime WorkflowRuntime
        {
            get { return workflowRuntime; }
            set { workflowRuntime = value; }
        }
    }
}