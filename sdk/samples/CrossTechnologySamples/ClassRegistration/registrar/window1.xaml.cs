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
using System.Workflow.Runtime.Hosting;


namespace Registrar
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>

    public partial class Window1 : Window
    {
        // Application settings
        Properties.Settings _settings = Properties.Settings.Default;

        public Window1()
        {
            InitializeComponent();

            WorkflowRuntime workflowRuntime = new WorkflowRuntime();

            SqlWorkflowPersistenceService sqlService = new SqlWorkflowPersistenceService(_settings.SqlPersistenceConnectionString);
            workflowRuntime.AddService(sqlService);
            workflowRuntime.StartRuntime();

            foreach (SqlPersistenceWorkflowInstanceDescription desc in sqlService.GetAllWorkflows())
            {
                listWorkflows.Items.Add(desc.WorkflowInstanceId.ToString() + " " + desc.Status);
                Console.WriteLine(desc.WorkflowInstanceId);

                try
                {
                    WorkflowInstance workflowInstance = workflowRuntime.GetWorkflow(desc.WorkflowInstanceId);
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.Message);
                }

                
            }

            workflowRuntime.StopRuntime();
        }
    }
}