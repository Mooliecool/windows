using System;
using System.Configuration;
using System.Data;
using System.Data.SqlClient;

namespace IBuySpy {

    //*******************************************************
    //
    // OrderDetails Class
    //
    // A simple data class that encapsulates details about
    // a particular order inside the IBuySpy Orders
    // database.
    //
    //*******************************************************

    public class OrderDetails {

        public DateTime  OrderDate;
        public DateTime  ShipDate;
        public decimal   OrderTotal;
        public DataSet   OrderItems;
    }

    //*******************************************************
    //
    // OrderHistoryDB Class
    //
    // Business/Data Logic Class that encapsulates all data
    // logic necessary to query past orders within the
    // IBuySpy Orders database.
    //
    //*******************************************************

    public class OrdersDB {

        //*******************************************************
        //
        // CustomerDB.GetCustomerOrders() Method <a name="GetCustomerOrders"></a>
        //
        // The GetCustomerOrders method returns a struct containing
        // a forward-only, read-only DataReader.  This displays a list of all
        // past orders placed by a specified customer.
        // The SQLDataReaderResult struct also returns the SQL connection,
        // which must be explicitly closed after the data from the DataReader
        // is bound into the controls.
        //
        // Other relevant sources:
        //     + <a href="OrdersList.htm" style="color:green">OrdersList Stored Procedure</a>
        //
        //*******************************************************

        public SqlDataReader GetCustomerOrders(String customerID) 
        {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(ConfigurationSettings.AppSettings["ConnectionString"]);
            SqlCommand myCommand = new SqlCommand("OrdersList", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterCustomerid = new SqlParameter("@CustomerID", SqlDbType.Int, 4);
            parameterCustomerid.Value = Int32.Parse(customerID);
            myCommand.Parameters.Add(parameterCustomerid);

            // Execute the command
            myConnection.Open();
            SqlDataReader result = myCommand.ExecuteReader(CommandBehavior.CloseConnection);

            // Return the datareader result
            return result;
        }

        //*******************************************************
        //
        // OrdersDB.GetOrderDetails() Method <a name="GetOrderDetails"></a>
        //
        // The GetOrderDetails method returns an OrderDetails
        // struct containing information about the specified
        // order.
        //
        // Other relevant sources:
        //     + <a href="OrdersDetail.htm" style="color:green">OrdersDetail Stored Procedure</a>
        // 
        //*******************************************************

        public OrderDetails GetOrderDetails(int orderID, string customerID) 
        {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(ConfigurationSettings.AppSettings["ConnectionString"]);
            SqlDataAdapter myCommand = new SqlDataAdapter("OrdersDetail", myConnection);

            // Mark the Command as a SPROC
            myCommand.SelectCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterOrderID = new SqlParameter("@OrderID", SqlDbType.Int, 4);
            parameterOrderID.Value = orderID;
            myCommand.SelectCommand.Parameters.Add(parameterOrderID);

            SqlParameter parameterCustomerID = new SqlParameter("@CustomerID", SqlDbType.Int, 4);
            parameterCustomerID.Value = Int32.Parse(customerID);
            myCommand.SelectCommand.Parameters.Add(parameterCustomerID);

            SqlParameter parameterOrderDate = new SqlParameter("@OrderDate", SqlDbType.DateTime, 8);
            parameterOrderDate.Direction = ParameterDirection.Output;
            myCommand.SelectCommand.Parameters.Add(parameterOrderDate);

            SqlParameter parameterShipDate = new SqlParameter("@ShipDate", SqlDbType.DateTime, 8);
            parameterShipDate.Direction = ParameterDirection.Output;
            myCommand.SelectCommand.Parameters.Add(parameterShipDate);

            SqlParameter parameterOrderTotal = new SqlParameter("@OrderTotal", SqlDbType.Money, 8);
            parameterOrderTotal.Direction = ParameterDirection.Output;
            myCommand.SelectCommand.Parameters.Add(parameterOrderTotal);

            // Create and Fill the DataSet
            DataSet myDataSet = new DataSet();
            myCommand.Fill(myDataSet, "OrderItems");
            
            // ship date is null if order doesn't exist, or belongs to a different user
            if (parameterShipDate.Value != DBNull.Value) {
            
                // Create and Populate OrderDetails Struct using
                // Output Params from the SPROC, as well as the
                // populated dataset from the SqlDataAdapter

                OrderDetails myOrderDetails = new OrderDetails();

                myOrderDetails.OrderDate = (DateTime)parameterOrderDate.Value;
                myOrderDetails.ShipDate = (DateTime)parameterShipDate.Value;
                myOrderDetails.OrderTotal = (decimal)parameterOrderTotal.Value;
                myOrderDetails.OrderItems = myDataSet;

                // Return the DataSet
                return myOrderDetails;
            }
            else
                return null;
        }

        //*******************************************************
        //
        // OrdersDB.CalculateShippingDate() Method <a name="CalculateShippingDate"></a>
        //
        // The CalculateShippingDate method would be where you would
        // place all of the code necessary to calculate the shipping
        // ETA.  For now, we are just making up a random date.
        //
        //*******************************************************

        public DateTime CalculateShippingDate(String customerID, string cartID) {

            Random x = new Random();
            double myrandom = (double)x.Next(0,3);
            return DateTime.Now.AddDays(myrandom);
        }

        //*******************************************************
        //
        // OrdersDB.PlaceOrder() Method <a name="PlaceOrder"></a>
        //
        // The PlaceOrder method places an order within the
        // IBuySpy Orders Database and then clears out the current
        // items within the shopping cart.
        //
        // Other relevant sources:
        //     + <a href="OrdersAdd.htm" style="color:green">OrdersAdd Stored Procedure</a>
        //
        //*******************************************************

        public int PlaceOrder(string customerID, string cartID) 
        {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(ConfigurationSettings.AppSettings["ConnectionString"]);
            SqlCommand myCommand = new SqlCommand("OrdersAdd", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterCustomerID = new SqlParameter("@CustomerID", SqlDbType.Int, 4);
            parameterCustomerID.Value = Int32.Parse(customerID);
            myCommand.Parameters.Add(parameterCustomerID);

            SqlParameter parameterCartID = new SqlParameter("@CartID", SqlDbType.NVarChar, 50);
            parameterCartID.Value = cartID;
            myCommand.Parameters.Add(parameterCartID);

            SqlParameter parameterShipDate = new SqlParameter("@ShipDate", SqlDbType.DateTime, 8);
            parameterShipDate.Value = CalculateShippingDate(customerID, cartID);
            myCommand.Parameters.Add(parameterShipDate);

            SqlParameter parameterOrderDate = new SqlParameter("@OrderDate", SqlDbType.DateTime, 8);
            parameterOrderDate.Value = DateTime.Now;
            myCommand.Parameters.Add(parameterOrderDate);

            SqlParameter parameterOrderID = new SqlParameter("@OrderID", SqlDbType.Int, 4);
            parameterOrderID.Direction = ParameterDirection.Output;
            myCommand.Parameters.Add(parameterOrderID);

            // Open the connection and execute the Command
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();

            // Return the OrderID
            return (int)parameterOrderID.Value;
        }
    }
}

