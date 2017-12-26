using System;
using System.Configuration;
using System.Data;
using System.Data.SqlClient;

namespace IBuySpy {

    //*******************************************************
    //
    // CustomerDetails Class
    //
    // A simple data class that encapsulates details about
    // a particular customer inside the IBuySpy Customer
    // database.
    //
    //*******************************************************

    public class CustomerDetails {

        public String FullName;
        public String Email;
        public String Password;
    }

    //*******************************************************
    //
    // CustomersDB Class
    //
    // Business/Data Logic Class that encapsulates all data
    // logic necessary to add/login/query customers within
    // the IBuySpy Customer database.
    //
    //*******************************************************

    public class CustomersDB {

        //*******************************************************
        //
        // CustomersDB.GetCustomerDetails() Method <a name="GetCustomerDetails"></a>
        //
        // The GetCustomerDetails method returns a CustomerDetails
        // struct that contains information about a specific
        // customer (name, email, password, etc).
        //
        // Other relevant sources:
        //     + <a href="CustomerDetail.htm" style="color:green">CustomerDetail Stored Procedure</a>
        //
        //*******************************************************

        public CustomerDetails GetCustomerDetails(String customerID) 
        {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(ConfigurationSettings.AppSettings["ConnectionString"]);
            SqlCommand myCommand = new SqlCommand("CustomerDetail", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterCustomerID = new SqlParameter("@CustomerID", SqlDbType.Int, 4);
            parameterCustomerID.Value = Int32.Parse(customerID);
            myCommand.Parameters.Add(parameterCustomerID);

            SqlParameter parameterFullName = new SqlParameter("@FullName", SqlDbType.NVarChar, 50);
            parameterFullName.Direction = ParameterDirection.Output;
            myCommand.Parameters.Add(parameterFullName);

            SqlParameter parameterEmail = new SqlParameter("@Email", SqlDbType.NVarChar, 50);
            parameterEmail.Direction = ParameterDirection.Output;
            myCommand.Parameters.Add(parameterEmail);

            SqlParameter parameterPassword = new SqlParameter("@Password", SqlDbType.NVarChar, 50);
            parameterPassword.Direction = ParameterDirection.Output;
            myCommand.Parameters.Add(parameterPassword);

            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();

            // Create CustomerDetails Struct
            CustomerDetails myCustomerDetails = new CustomerDetails();

            // Populate Struct using Output Params from SPROC
            myCustomerDetails.FullName = (string)parameterFullName.Value;
            myCustomerDetails.Password = (string)parameterPassword.Value;
            myCustomerDetails.Email = (string)parameterEmail.Value;

            return myCustomerDetails;
        }

        //*******************************************************
        //
        // CustomersDB.AddCustomer() Method <a name="AddCustomer"></a>
        //
        // The AddCustomer method inserts a new customer record
        // into the customers database.  A unique "CustomerId"
        // key is then returned from the method.  This can be
        // used later to place orders, track shopping carts,
        // etc within the ecommerce system.
        //
        // Other relevant sources:
        //     + <a href="CustomerAdd.htm" style="color:green">CustomerAdd Stored Procedure</a>
        //
        //*******************************************************

        public String AddCustomer(string fullName, string email, string password) 
        {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(ConfigurationSettings.AppSettings["ConnectionString"]);
            SqlCommand myCommand = new SqlCommand("CustomerAdd", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterFullName = new SqlParameter("@FullName", SqlDbType.NVarChar, 50);
            parameterFullName.Value = fullName;
            myCommand.Parameters.Add(parameterFullName);

            SqlParameter parameterEmail = new SqlParameter("@Email", SqlDbType.NVarChar, 50);
            parameterEmail.Value = email;
            myCommand.Parameters.Add(parameterEmail);

            SqlParameter parameterPassword = new SqlParameter("@Password", SqlDbType.NVarChar, 50);
            parameterPassword.Value = password;
            myCommand.Parameters.Add(parameterPassword);

            SqlParameter parameterCustomerID = new SqlParameter("@CustomerID", SqlDbType.Int, 4);
            parameterCustomerID.Direction = ParameterDirection.Output;
            myCommand.Parameters.Add(parameterCustomerID);

            try {
                myConnection.Open();
                myCommand.ExecuteNonQuery();
                myConnection.Close();

                // Calculate the CustomerID using Output Param from SPROC
                int customerId = (int)parameterCustomerID.Value;

                return customerId.ToString();
            }
            catch {
                return String.Empty;
            }
        }

        //*******************************************************
        //
        // CustomersDB.Login() Method <a name="Login"></a>
        //
        // The Login method validates a email/password pair
        // against credentials stored in the customers database.
        // If the email/password pair is valid, the method returns
        // the "CustomerId" number of the customer.  Otherwise
        // it will throw an exception.
        //
        // Other relevant sources:
        //     + <a href="CustomerLogin.htm" style="color:green">CustomerLogin Stored Procedure</a>
        //
        //*******************************************************

        public String Login(string email, string password) 
        {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(ConfigurationSettings.AppSettings["ConnectionString"]);
            SqlCommand myCommand = new SqlCommand("CustomerLogin", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterEmail = new SqlParameter("@Email", SqlDbType.NVarChar, 50);
            parameterEmail.Value = email;
            myCommand.Parameters.Add(parameterEmail);

            SqlParameter parameterPassword = new SqlParameter("@Password", SqlDbType.NVarChar, 50);
            parameterPassword.Value = password;
            myCommand.Parameters.Add(parameterPassword);

            SqlParameter parameterCustomerID = new SqlParameter("@CustomerID", SqlDbType.Int, 4);
            parameterCustomerID.Direction = ParameterDirection.Output;
            myCommand.Parameters.Add(parameterCustomerID);

            // Open the connection and execute the Command
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();

            int customerId = (int)(parameterCustomerID.Value);

            if (customerId == 0) {
                return null;
            }
            else {
                return customerId.ToString();
            }
        }
    }
}

