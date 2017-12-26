using System;
using System.Activities;
using System.Activities.Statements;
using System.Activities.DurableInstancing;
using System.Configuration;
using System.Threading;
using System.ServiceModel.Activities;

namespace CSWF4ActivitiesCorrelation
{
    class Program
    {
        static void Main(string[] args)
        {
            Uri wfAddress = new Uri(@"http://localhost:8000/WFServices");
            AutoResetEvent waitHandler = new AutoResetEvent(false);
            using (WorkflowServiceHost host =
                new WorkflowServiceHost(new Workflow1(), wfAddress))
            {
                host.WorkflowExtensions.Add(SetupSimplySqlPersistenceStore());
                host.Closed += (obj, arg) =>
                {
                    waitHandler.Set();
                };
                host.Open();
                Console.WriteLine(@"http://localhost:8000/WFServices is opening");
                waitHandler.WaitOne();
            }
        }

        private static SqlWorkflowInstanceStore SetupSimplySqlPersistenceStore()
        {
            string connectionString =
                ConfigurationManager.AppSettings["SqlWF4PersistenceConnectionString"].ToString();
            SqlWorkflowInstanceStore sqlInstanceStore =
                new SqlWorkflowInstanceStore(connectionString);
            sqlInstanceStore.HostLockRenewalPeriod = TimeSpan.FromSeconds(30);
            return sqlInstanceStore;
        }
    }
}
