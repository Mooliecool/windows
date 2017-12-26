using System;
using System.Collections.Generic;
using System.Data;
using System.Data.SqlClient;
using System.Globalization;
using System.IO;
using System.Text;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Workflow.ComponentModel;
using System.Workflow.Runtime;
using System.Workflow.Runtime.Hosting;
using System.Workflow.Runtime.Tracking;

namespace Microsoft.Samples.ClassRegistration
{
    public partial class Register : Window, IDisposable
    {
        String registrationConnectionString = String.Empty;
        String trackingConnectionString = String.Empty;

        // Local variable for workflow
        WorkflowRuntime workflowRuntime;

        public Register()
        {
            InitializeComponent();

            // Get Connection Strings from Application Settings
            Properties.Settings settings = Properties.Settings.Default;
            registrationConnectionString = settings.ClassRegistrationConnectionString;
            trackingConnectionString = settings.ClassRegistrationTrackingConnectionString;

            // Get workflow runtime from application object
            workflowRuntime = Application.Current.Properties["WorkflowRuntime"] as WorkflowRuntime;

            if (workflowRuntime == null)
            {
                InitializeWorkflowRuntime();
            }
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                // Dispose managed resources
                workflowRuntime.Dispose();
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(true);
        }

        private void InitializeWorkflowRuntime()
        {
            // Create tracking profile
            CreateAndInsertTrackingProfile();

            // Initialize workflow runtime
            workflowRuntime = new WorkflowRuntime();
            workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;

            // Add Sql Tracking Service
            workflowRuntime.AddService(new SqlTrackingService(trackingConnectionString));

            // Add Sql Persistence Service
            workflowRuntime.AddService(new SqlWorkflowPersistenceService(trackingConnectionString));

            // This service is required to allow the workflow runtime to use both Sql Persistence and Tracking
            workflowRuntime.AddService(new SharedConnectionWorkflowCommitWorkBatchService(trackingConnectionString));

            // Save workflow runtime to application object
            Application.Current.Properties["WorkflowRuntime"] = workflowRuntime;
        }

        private void PageLoad(object sender, EventArgs args)
        {
            // Open SQL Connection
            SqlConnection connection = new SqlConnection(registrationConnectionString);
            connection.Open();

            // Execute SQL Command
            SqlDataAdapter adapter = new SqlDataAdapter("SELECT * FROM Class", connection);
            DataSet dataSet = new DataSet();
            adapter.Fill(dataSet, "Class");

            // Populate list of classes
            ClassList.DataContext = dataSet;

            // Close SQL Connection
            connection.Close();

            // Disable necessary buttons
            AddSession.IsEnabled = false;
            SubmitRegistration.IsEnabled = false;
        }

        private void OnClassChanged(object sender, SelectionChangedEventArgs args)
        {
            ComboBox combo = (ComboBox)sender;
            DataRowView view = (DataRowView)combo.SelectedItem;

            // Open SQL Connection
            SqlConnection connection = new SqlConnection(registrationConnectionString);
            connection.Open();

            // Execute SQL Command
            SqlDataAdapter adapter = new SqlDataAdapter("SELECT * FROM ClassSession WHERE ClassId=@ClassId", connection);
            adapter.SelectCommand.Parameters.AddWithValue("ClassId", view.Row["Id"]);
            DataSet dataSet = new DataSet();
            adapter.Fill(dataSet, "ClassSession");

            // Display lists of session for given class
            SessionList.DataContext = dataSet;
        }

        private void OnSessionChanged(object sender, SelectionChangedEventArgs args)
        {
            AddSession.IsEnabled = ((ListBox)sender).SelectedIndex >= 0;
        }

        private void OnSessionAdded(object sender, EventArgs args)
        {
            DataRowView sessionView = (DataRowView)SessionList.SelectedItem;
            DataRowView classView = (DataRowView)ClassList.SelectedItem;

            // Create new item for list of pending registrations
            ListBoxItem item = new ListBoxItem();
            item.Tag = sessionView.Row["Id"].ToString();
            item.Content = classView.Row["CoursePrefix"] + " " +
                               classView.Row["CourseNumber"] + "-" +
                               sessionView.Row["SessionId"] + " " +
                               classView.Row["Title"];

            PendingRegistrations.Items.Add(item);

            SubmitRegistration.IsEnabled = true;
        }

        private void OnSubmitRegistration(object sender, EventArgs args)
        {
            Type workflowType = typeof(RegistrationWorkflow);

            // Start workflow runtime
            workflowRuntime.StartRuntime();

            foreach (ListBoxItem item in PendingRegistrations.Items)
            {
                // Define parameters
                Dictionary<string, object> parameters = new Dictionary<string, object>();
                parameters.Add("UserId", Application.Current.Properties["UserId"]);
                parameters.Add("SessionId", item.Tag as String);

                // Start workflow instance
                WorkflowInstance workflowInstance = workflowRuntime.CreateWorkflow(workflowType, parameters);
                workflowInstance.Start();
            }

            // Give workflows time to complete
            Thread.Sleep(5000);
            
            // Stop the runtime
            workflowRuntime.StopRuntime();

            // Display dialog confirming registion
            MessageBox.Show("Your registration has been submitted.");

            // Close registration window
            this.Close();
        }

        private void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine(e.Exception.Message);
        }

        private void CreateAndInsertTrackingProfile()
        {
            TrackingProfile profile = new TrackingProfile();
            ActivityTrackPoint trackPoint = new ActivityTrackPoint();
            ActivityTrackingLocation location = new ActivityTrackingLocation(typeof(Activity));

            // Specify tracking locations
            location.MatchDerivedTypes = true;

            foreach (ActivityExecutionStatus status in Enum.GetValues(typeof(ActivityExecutionStatus)))
            {
                location.ExecutionStatusEvents.Add(status);
            }

            trackPoint.MatchingLocations.Add(location);

            // Specify workflow properties to track
            WorkflowDataTrackingExtract dataExtract = new WorkflowDataTrackingExtract();
            dataExtract.Member = "UserId";
            trackPoint.Extracts.Add(dataExtract);

            dataExtract = new WorkflowDataTrackingExtract();
            dataExtract.Member = "SessionId";
            trackPoint.Extracts.Add(dataExtract);

            dataExtract = new WorkflowDataTrackingExtract();
            dataExtract.Member = "RegistrationStatus";
            trackPoint.Extracts.Add(dataExtract);

            // Update profile
            profile.ActivityTrackPoints.Add(trackPoint);
            profile.Version = new Version("3.0.0.1");

            // Serialize the profile
            TrackingProfileSerializer serializer = new TrackingProfileSerializer();
            StringWriter writer = new StringWriter(new StringBuilder(), CultureInfo.InvariantCulture);
            serializer.Serialize(writer, profile);
            string trackingprofile = writer.ToString();
            InsertTrackingProfile(trackingprofile);
        }

        private void InsertTrackingProfile(string profile)
        {
            SqlCommand command = new SqlCommand();

            command.CommandType = CommandType.StoredProcedure;
            command.CommandText = "dbo.UpdateTrackingProfile";
            command.Connection = new SqlConnection(this.trackingConnectionString);

            try
            {
                command.Parameters.Clear();

                SqlParameter param1 = new SqlParameter();
                param1.ParameterName = "@TypeFullName";
                param1.SqlDbType = SqlDbType.NVarChar;
                param1.SqlValue = typeof(RegistrationWorkflow).ToString();
                command.Parameters.Add(param1);

                SqlParameter param2 = new SqlParameter();
                param2.ParameterName = "@AssemblyFullName";
                param2.SqlDbType = SqlDbType.NVarChar;
                param2.SqlValue = typeof(RegistrationWorkflow).Assembly.FullName;
                command.Parameters.Add(param2);

                SqlParameter param3 = new SqlParameter();
                param3.ParameterName = "@Version";
                param3.SqlDbType = SqlDbType.VarChar;
                param3.SqlValue = "3.0.0.0";
                command.Parameters.Add(param3);

                SqlParameter param4 = new SqlParameter();
                param4.ParameterName = "@TrackingProfileXml";
                param4.SqlDbType = SqlDbType.NText;
                param4.SqlValue = profile;
                command.Parameters.Add(param4);

                command.Connection.Open();
                command.ExecuteNonQuery();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                Console.WriteLine("The Tracking Profile Was not Inserted. If You want to add a new one then please increase the version Number\n");
                return;
            }
            finally
            {
                if (command != null && command.Connection != null && command.Connection.State != System.Data.ConnectionState.Closed)
                    command.Connection.Close();
            }
        }
    }

    [ValueConversion(typeof(DateTime), typeof(String))]
    public class DateConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            DateTime date = (DateTime)value;
            return date.ToShortDateString();
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            string strValue = value.ToString();
            DateTime resultDateTime;
            if (DateTime.TryParse(strValue, out resultDateTime))
            {
                return resultDateTime;
            }
            return value;
        }
    }
}