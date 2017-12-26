using System;
using System.Configuration;
using System.Data.SqlClient;
using System.Windows;
using System.Windows.Controls;

namespace Microsoft.Samples.ClassRegistration
{
    public partial class SignIn : Window
    {
        public SignIn()
        {
            InitializeComponent();
        }

        private void Submit(object sender, EventArgs args)
        {
            if (CheckCurrentStudent() == false)
            {
                MessageBoxResult result = MessageBox.Show("Student ID not found, add new student?", "Student Not Found", MessageBoxButton.YesNo);

                if (result == MessageBoxResult.Yes)
                {
                    AddNewStudent();
                }
            }

            if (String.IsNullOrEmpty(Application.Current.Properties["UserId"] as String) == false)
            {
                OpenMenuWindow();
            }
        }

        private bool CheckCurrentStudent()
        {
            // Execute SQL Command
            SqlCommand command = new SqlCommand();
            command.CommandText = "SELECT UserId FROM Student WHERE UserID=@UserID";
            command.Connection = new SqlConnection(Properties.Settings.Default.ClassRegistrationConnectionString);

            try
            {
                command.Parameters.AddWithValue("UserId", StudentId.Text);
                command.Connection.Open();
                Application.Current.Properties["UserId"] = command.ExecuteScalar() as String;
            }
            catch(Exception e)
            {
                Console.WriteLine(e.Message);
                Console.WriteLine("Unable to Lookup Student ID.");
            }
            finally
            {
                if (command != null && command.Connection != null && command.Connection.State != System.Data.ConnectionState.Closed)
                    command.Connection.Close();
            }
            
            // Determine if current user was found
            return (String.IsNullOrEmpty(Application.Current.Properties["UserId"] as String) == false);
        }

        private void AddNewStudent()
        {
            SqlCommand command = new SqlCommand();
            command.CommandText = "INSERT INTO Student (UserId, Status) VALUES (@UserID, 'New Student')";
            command.Connection = new SqlConnection(Properties.Settings.Default.ClassRegistrationConnectionString);
            
            try
            {
                command.Parameters.AddWithValue("UserId", StudentId.Text);
                command.Connection.Open();
                command.ExecuteNonQuery();
            }
            catch(Exception e)
            {
                Console.WriteLine(e.Message);
                Console.WriteLine("Unable to Lookup Student ID.");
            }
            finally
            {
                if (command != null && command.Connection != null && command.Connection.State != System.Data.ConnectionState.Closed)
                    command.Connection.Close();
            }

            Application.Current.Properties["UserId"] = StudentId.Text;
        }

        private void OpenMenuWindow()
        {
            Menu menuWindow = new Menu();
            menuWindow.InitializeComponent();
            menuWindow.Show();
            this.Close();
        }
    }
}