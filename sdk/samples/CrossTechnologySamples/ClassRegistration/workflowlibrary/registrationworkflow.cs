using System;
using System.Data;
using System.Data.SqlClient;
using System.Drawing;
using System.Workflow.Runtime;
using System.Workflow.Activities;
using System.Workflow.Activities.Rules;

namespace Microsoft.Samples.ClassRegistration
{
    public sealed partial class RegistrationWorkflow : SequentialWorkflowActivity
    {
        public RegistrationWorkflow()
        {
            InitializeComponent();
        }

        private bool IsClassFull = true;
        private bool IsRegistered = false;
        
        // Application settings
        static Properties.Settings _settings = Properties.Settings.Default;

        // Workflow properties
        private String userIdValue;
        public String UserId
        {
            get { return this.userIdValue; }
            set { this.userIdValue = value; }
        }

        private String sessionIdValue;
        public String SessionId
        {
            get { return this.sessionIdValue; }
            set { this.sessionIdValue = value; }
        }

        private String registrationStatus;
        public String RegistrationStatus
        {
            get { return this.registrationStatus; }
        }

        private void validateRegistration_ExecuteCode(object sender, EventArgs e)
        {
            Console.WriteLine("Validating registration.");

            int studentCount = 0;
            int seatCount = 0;

            SqlConnection connection = new SqlConnection(_settings.ClassRegistrationConnectionString);
            connection.Open();

            // Get number of students registered for current session
            SqlCommand command = new SqlCommand("SELECT Count(*) FROM Registration where SessionId=@SessionId", connection);
            command.Parameters.AddWithValue("@SessionId", this.sessionIdValue);
            studentCount = (int)command.ExecuteScalar();

            // Get seat count for current session
            command = new SqlCommand("SELECT SeatCount FROM ClassSession WHERE Id=@SessionId", connection);
            command.Parameters.AddWithValue("@SessionId", this.sessionIdValue);
            seatCount = (int)command.ExecuteScalar();

            // Check if class is full
            this.IsClassFull = studentCount >= seatCount;

            // Check for duplicate registration
            command = new SqlCommand("SELECT Count(*) FROM Registration WHERE SessionId=@SessionId AND StudentID=@StudentID", connection);
            command.Parameters.AddWithValue("@SessionId", this.sessionIdValue);
            command.Parameters.AddWithValue("@StudentId", this.userIdValue);
            IsRegistered = (int)command.ExecuteScalar() > 0;

            // Close SQL Connection
            connection.Close();
        }

        private void notifyStudentAlreadyRegistered_ExecuteCode(object sender, EventArgs e)
        {
            Console.WriteLine("Notify student that they've already registered for this session.");
        }

        private void notifyStudentClassFull_ExecuteCode(object sender, EventArgs e)
        {
            Console.WriteLine("Nofify student that the requested session is already full.");
        }

        private void updateRegistrationStatusWaitlisted_ExecuteCode(object sender, EventArgs e)
        {
            this.registrationStatus = "Waitlisted";
        }

        private void updateRegistrationStatusApproved_ExecuteCode(object sender, EventArgs e)
        {
            this.registrationStatus = "Approved";
        }

        private void updateRegistrationStatusRejected_ExecuteCode(object sender, EventArgs e)
        {
            this.registrationStatus = "Rejected";
        }

        private void updateRegistrationStatusWaitingForApproval_ExecuteCode(object sender, EventArgs e)
        {
            this.registrationStatus = "Waiting for Approval";
        }

        private void addRegistrationToDatabase_ExecuteCode(object sender, EventArgs e)
        {
            Console.WriteLine("Adding registration to database.");

            // Create SQL Connection
            SqlConnection connection = new SqlConnection(_settings.ClassRegistrationConnectionString);
            connection.Open();

            // Execute SQL Command
            string commandText = "INSERT INTO Registration (StudentId, SessionID, Status) " +
                                 "VALUES (@StudentId, @SessionID, @Status)";

            SqlCommand command = new SqlCommand(commandText, connection);
            command.Parameters.AddWithValue("@StudentId", this.userIdValue);
            command.Parameters.AddWithValue("@SessionId", this.sessionIdValue);
            command.Parameters.AddWithValue("@Status", this.registrationStatus);
            command.ExecuteNonQuery();
        }

        private void notifyStudentOfSessionStatus_ExecuteCode(object sender, EventArgs e)
        {
            Console.WriteLine("Notify student of session status.");
        }
    }
}
