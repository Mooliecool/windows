using System;
using System.Data;
using System.Data.SqlClient;
using System.EnterpriseServices;


namespace COMPlusServicesExample
{

	/// <summary>
	/// Summary description for CustomersDB.
	/// </summary>
	[Transaction(TransactionOption.Supported)]
	public class CustomersDB : ServicedComponent
	{

		//this class handles all data retrieval.

		//class constructor
		public CustomersDB()
		{
		}
		
		//method returns a connection object for the Northwind database.
		private SqlConnection CreateConnection()
		{
			SqlConnection databaseConnection = new SqlConnection("server=localhost;Trusted_Connection=false;uid=sa;pwd=;database=Northwind");
			return databaseConnection;
		}


		//gets a dataset with the customers
		public DataSet GetCustomers()
		{	
			//grab the connection object
			SqlConnection currentConnection = CreateConnection();
			
			//open the connection and create 
			//the command object
			currentConnection.Open();
			SqlCommand sqlCommandToExecute = new SqlCommand();
			sqlCommandToExecute.Connection = currentConnection;

			//set the command type and provide
			//the command text
			sqlCommandToExecute.CommandType = CommandType.Text;
			sqlCommandToExecute.CommandText = "Select * from Customers";
			//create and fill the data adapter with the 
			//command object.
			SqlDataAdapter dataadapterCustomer = new SqlDataAdapter(sqlCommandToExecute);
			//create the dataset
			DataSet datasetCustomer = new DataSet("dsCustomers");
			//fill the data set with the results in
			//the data adapter
			dataadapterCustomer.Fill(datasetCustomer);
			//return the dataset
			return datasetCustomer;
		}
		
		//gets a dataset with the customer orders.
		public DataSet GetOrders(string CustomerID)
		{

			//grab the connection object
			SqlConnection currentConnection = CreateConnection();
			
			//open the connection and create 
			//the command object
			currentConnection.Open();
			SqlCommand sqlCommandToExecute = new SqlCommand();
			sqlCommandToExecute.Connection = currentConnection;

			//set the command type and provide
			//the command text
			sqlCommandToExecute.CommandType = CommandType.StoredProcedure;
			sqlCommandToExecute.CommandText = "CustOrdersOrders";

			//add the parameters
			SqlParameter param1 = sqlCommandToExecute.Parameters.Add(new SqlParameter("@CustomerID", SqlDbType.NChar, 5));
			param1.Value = CustomerID;
			
			//create and fill the data adapter with the 
			//command object.
			SqlDataAdapter dataadapterCustomer = new SqlDataAdapter(sqlCommandToExecute);
			//create the dataset
			DataSet datasetCustomer = new DataSet("dsOrders");
			//fill the data set with the results in
			//the data adapter
			dataadapterCustomer.Fill(datasetCustomer);
			//return the dataset
			return datasetCustomer;
		}

		//gets a dataset with the order details.
		public DataSet GetOrderDetails(int OrderID)
		{
			//grab the connection object
			SqlConnection currentConnection = CreateConnection();
			
			//open the connection and create 
			//the command object
			currentConnection.Open();
			SqlCommand sqlCommandToExecute = new SqlCommand();
			sqlCommandToExecute.Connection = currentConnection;

			//set the command type and provide
			//the command text
			sqlCommandToExecute.CommandType = CommandType.StoredProcedure;
			sqlCommandToExecute.CommandText = "CustOrdersDetail";

			//add the parameters
			SqlParameter param1 = sqlCommandToExecute.Parameters.Add(new SqlParameter("@OrderID", SqlDbType.Int, 4));
			param1.Value = OrderID;
			
			//create and fill the data adapter with the 
			//command object.
			SqlDataAdapter dataadapterCustomer = new SqlDataAdapter(sqlCommandToExecute);
			//create the dataset
			DataSet datasetCustomer = new DataSet("dsOrderDetails");
			//fill the data set with the results in
			//the data adapter
			dataadapterCustomer.Fill(datasetCustomer);
			//return the dataset
			return datasetCustomer;
		}
	}		


	
	
	/// <summary>
	/// Summary description for Customers.
	/// </summary>
	[Transaction(TransactionOption.Required)]
	public class Customers : ServicedComponent
	{

		//this class handles data modification.
		
		//class constructor
		public Customers()
		{	
		}
		
		//method returns a connection object for the Northwind database.
		private SqlConnection CreateConnection()
		{
			SqlConnection databaseConnection = new SqlConnection("server=localhost;Trusted_Connection=false;uid=sa;pwd=;database=Northwind");
			return databaseConnection;
		}
		
		//updates the detail quantity for the product.
		public bool UpdateDetailQuantity(int OrderID, string ProductName, int Quantity)
		{
			//grab the connection object
			SqlConnection currentConnection = CreateConnection();
			
			//open the connection object
			currentConnection.Open();

			try
			{
				//try to update the quantity

				//check to see if a transaction exists.
				//if not the throw an exception.
				if(!ContextUtil.IsInTransaction)
					throw new Exception("Requires Transaction");

				//set the command text to be executed.
				string CommandText = "Update [Order Details] set Quantity = "+Quantity+" where OrderID = "+OrderID+" AND [Order Details].ProductID IN";
				CommandText+= "(Select ProductID from Products where ProductName = '"+ProductName+"')";

				//create the command object and 
				//set the connection property.
				SqlCommand sqlCommandToExecute = new SqlCommand();
				sqlCommandToExecute.Connection = currentConnection;

				//set the command type and provide 
				//the command text.
				sqlCommandToExecute.CommandType = CommandType.Text;
				sqlCommandToExecute.CommandText = CommandText;
				//execute the query.
				sqlCommandToExecute.ExecuteNonQuery();
				
				//complete the transaction.
				ContextUtil.SetComplete();
				return true;

			}

			catch (Exception e)
			{
				//if an exception is thrown
				//close the connection object
				//and abort the transaction.
				currentConnection.Close();
				ContextUtil.SetAbort();
				return false;
			}
			finally
			{
				//close the connection object.
				currentConnection.Close();
			}
		}
	}
}
