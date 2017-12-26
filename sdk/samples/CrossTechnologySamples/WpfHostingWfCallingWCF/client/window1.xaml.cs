using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Workflow.Runtime;
using Microsoft.ServiceModel.Samples;
using System.Windows.Threading;

namespace WpfHostingWfCallingWCF
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>

    public partial class Window1 : System.Windows.Window
    {

        public Window1()
        {
            InitializeComponent();
        }

        WorkflowRuntime workflowRuntime = new WorkflowRuntime();

        protected override void OnInitialized(EventArgs e)
        {
            base.OnInitialized(e);

            workflowRuntime.WorkflowCompleted += new EventHandler<WorkflowCompletedEventArgs>(workflowRuntime_WorkflowCompleted);
            workflowRuntime.WorkflowTerminated += new EventHandler<WorkflowTerminatedEventArgs>(workflowRuntime_WorkflowTerminated);
            _workFlowButton.Click += _workFlowButton_Click;
        }

        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);

            workflowRuntime.WorkflowCompleted -= new EventHandler<WorkflowCompletedEventArgs>(workflowRuntime_WorkflowCompleted);
            workflowRuntime.WorkflowTerminated -= new EventHandler<WorkflowTerminatedEventArgs>(workflowRuntime_WorkflowTerminated);
        }

        //when the user clicks the button, we put the input text into a parameter dictionary,
        //pass it to an instance of the workflow,
        //then start the workflow
        void _workFlowButton_Click(object sender, RoutedEventArgs e)
        {
            StringBuilder sb = new StringBuilder();

            string input = _input.Text;
            
            sb.AppendFormat("Input parameter for this workflow is {0}\n", input);
            Dictionary<string, object> parameters = new Dictionary<string, object>();
            parameters.Add("Input", input);

            WorkflowInstance instance = workflowRuntime.CreateWorkflow(typeof(Workflow1), parameters);
            sb.AppendLine("Starting workflow");
            _output.Text = sb.ToString();
            instance.Start();
        
        }

        void workflowRuntime_WorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            //workflow events will not be fired on the UI thread so we need to use the Dispatcher
            Dispatcher.BeginInvoke(DispatcherPriority.Normal, new UpdateOutput(HandleUpdateOutput), string.Format("\n{0}\n", e.Exception.Message));
        }

        void HandleUpdateOutput(string update)
        {
            _output.Text += update;
        }

        void workflowRuntime_WorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine("Workflow completed");
            string returnValue = (string)e.OutputParameters["ReturnValue"];
            sb.AppendFormat("The ReturnValue for this workflow was {0}", returnValue);

            //workflow events will not be fired on the UI thread so we need to use the Dispatcher
            Dispatcher.Invoke(DispatcherPriority.Normal, new UpdateOutput(HandleUpdateOutput), sb.ToString());
        }

        delegate void UpdateOutput(string update);
    }
}