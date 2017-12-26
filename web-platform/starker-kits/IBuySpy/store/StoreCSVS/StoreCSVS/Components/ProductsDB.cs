using System;
using System.Configuration;
using System.Data;
using System.Data.SqlClient;

namespace IBuySpy {

    //*******************************************************
    //
    // ProductDetails Class
    //
    // A simple data class that encapsulates details about
    // a particular product inside the IBuySpy Product
    // database.
    //
    //*******************************************************

    public class ProductDetails {

        public String  ModelNumber;
        public String  ModelName;
        public String  ProductImage;
        public decimal UnitCost;
        public String  Description;
    }

    //*******************************************************
    //
    // ProductsDB Class
    //
    // Business/Data Logic Class that encapsulates all data
    // logic necessary to query products within
    // the IBuySpy Products database.
    //
    //*******************************************************

    public class ProductsDB {

        //*******************************************************
        //
        // ProductsDB.GetProductCategories() Method <a name="GetProductCategories"></a>
        //
        // The GetProductCategories method returns a DataReader that exposes all 
        // product categories (and their CategoryIDs) within the IBuySpy Products   
        // database.  The SQLDataReaderResult struct also returns the
        // SQL connection, which must be explicitly closed after the
        // data from the DataReader is bound into the controls.
        //
        // Other relevant sources:
        //     + <a href="ProductCategoryList.htm" style="color:green">ProductCategoryList Stored Procedure</a>
        //
        //*******************************************************

        public SqlDataReader GetProductCategories() 
        {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(ConfigurationSettings.AppSettings["ConnectionString"]);
            SqlCommand myCommand = new SqlCommand("ProductCategoryList", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Execute the command
            myConnection.Open();
            SqlDataReader result = myCommand.ExecuteReader(CommandBehavior.CloseConnection);

            // Return the datareader result
            return result;
        }

        //*******************************************************
        //
        // ProductsDB.GetProducts() Method <a name="GetProducts"></a>
        //
        // The GetProducts method returns a struct containing a forward-only,
        // read-only DataReader. This displays all products within a specified
        // product category.  The SQLDataReaderResult struct also returns the
        // SQL connection, which must be explicitly closed after the
        // data from the DataReader is bound into the controls.
        //
        // Other relevant sources:
        //     + <a href="ProductsByCategory.htm" style="color:green">ProductsByCategory Stored Procedure</a>
        //
        //*******************************************************

        public SqlDataReader GetProducts(int categoryID) {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(ConfigurationSettings.AppSettings["ConnectionString"]);
            SqlCommand myCommand = new SqlCommand("ProductsByCategory", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterCategoryID = new SqlParameter("@CategoryID", SqlDbType.Int, 4);
            parameterCategoryID.Value = categoryID;
            myCommand.Parameters.Add(parameterCategoryID);

            // Execute the command
            myConnection.Open();
            SqlDataReader result = myCommand.ExecuteReader(CommandBehavior.CloseConnection);
            
            // Return the datareader result
            return result;
        }


        //*******************************************************
        //
        // ProductsDB.GetProductDetails() Method <a name="GetProductDetails"></a>
        //
        // The GetProductDetails method returns a ProductDetails
        // struct containing specific details about a specified
        // product within the IBuySpy Products Database.
        //
        // Other relevant sources:
        //     + <a href="ProductDetail.htm" style="color:green">ProductDetail Stored Procedure</a>
        //
        //*******************************************************

        public ProductDetails GetProductDetails(int productID) {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(ConfigurationSettings.AppSettings["ConnectionString"]);
            SqlCommand myCommand = new SqlCommand("ProductDetail", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterProductID = new SqlParameter("@ProductID", SqlDbType.Int, 4);
            parameterProductID.Value = productID;
            myCommand.Parameters.Add(parameterProductID);

            SqlParameter parameterUnitCost = new SqlParameter("@UnitCost", SqlDbType.Money, 8);
            parameterUnitCost.Direction = ParameterDirection.Output;
            myCommand.Parameters.Add(parameterUnitCost);

            SqlParameter parameterModelNumber = new SqlParameter("@ModelNumber", SqlDbType.NVarChar, 50);
            parameterModelNumber.Direction = ParameterDirection.Output;
            myCommand.Parameters.Add(parameterModelNumber);

            SqlParameter parameterModelName = new SqlParameter("@ModelName", SqlDbType.NVarChar, 50);
            parameterModelName.Direction = ParameterDirection.Output;
            myCommand.Parameters.Add(parameterModelName);

            SqlParameter parameterProductImage = new SqlParameter("@ProductImage", SqlDbType.NVarChar, 50);
            parameterProductImage.Direction = ParameterDirection.Output;
            myCommand.Parameters.Add(parameterProductImage);

            SqlParameter parameterDescription = new SqlParameter("@Description", SqlDbType.NVarChar, 3800);
            parameterDescription.Direction = ParameterDirection.Output;
            myCommand.Parameters.Add(parameterDescription);

            // Open the connection and execute the Command
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();

            // Create and Populate ProductDetails Struct using
            // Output Params from the SPROC
            ProductDetails myProductDetails = new ProductDetails();

            myProductDetails.ModelNumber = (string)parameterModelNumber.Value;
            myProductDetails.ModelName = (string)parameterModelName.Value;
            myProductDetails.ProductImage = ((string)parameterProductImage.Value).Trim();
            myProductDetails.UnitCost = (decimal)parameterUnitCost.Value;
            myProductDetails.Description = ((string)parameterDescription.Value).Trim();

            return myProductDetails;
        }

        //*******************************************************
        //
        // ProductsDB.GetProductsAlsoPurchased() Method <a name="GetProductsAlsoPurchased"></a>
        //
        // The GetPGetProductsAlsoPurchasedroducts method returns a struct containing
        // a forward-only, read-only DataReader.  This displays a list of other products
        // also purchased with a specified product  The SQLDataReaderResult struct also
        // returns the SQL connection, which must be explicitly closed after the
        // data from the DataReader is bound into the controls.
        //
        // Other relevant sources:
        //     + <a href="CustomerAlsoBought.htm" style="color:green">CustomerAlsoBought Stored Procedure</a>
        //
        //*******************************************************

        public SqlDataReader GetProductsAlsoPurchased(int productID) {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(ConfigurationSettings.AppSettings["ConnectionString"]);
            SqlCommand myCommand = new SqlCommand("CustomerAlsoBought", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterProductID = new SqlParameter("@ProductID", SqlDbType.Int, 4);
            parameterProductID.Value = productID;
            myCommand.Parameters.Add(parameterProductID);

            // Execute the command
            myConnection.Open();
            SqlDataReader result = myCommand.ExecuteReader(CommandBehavior.CloseConnection);

            // Return the datareader result
            return result;
        }

        //*******************************************************
        //
        // ProductsDB.GetMostPopularProductsOfWeek() Method <a name="GetMostPopularProductsOfWeek"></a>
        //
        // The GetMostPopularProductsOfWeek method returns a struct containing a 
        // forward-only, read-only DataReader containing the most popular products 
        // of the week within the IBuySpy Products database.  
        // The SQLDataReaderResult struct also returns the
        // SQL connection, which must be explicitly closed after the
        // data from the DataReader is bound into the controls.
        //
        // Other relevant sources:
        //     + <a href="ProductsMostPopular.htm" style="color:green">ProductsMostPopular Stored Procedure</a>
        //
        //*******************************************************

        public SqlDataReader GetMostPopularProductsOfWeek() {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(ConfigurationSettings.AppSettings["ConnectionString"]);
            SqlCommand myCommand = new SqlCommand("ProductsMostPopular", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Execute the command
            myConnection.Open();
            SqlDataReader result = myCommand.ExecuteReader(CommandBehavior.CloseConnection);

            // Return the datareader result
            return result;
        }

        //*******************************************************
        //
        // ProductsDB.SearchProductDescriptions() Method <a name="SearchProductDescriptions"></a>
        //
        // The SearchProductDescriptions method returns a struct containing
        // a forward-only, read-only DataReader.  This displays a list of all
        // products whose name and/or description contains the specified search
        // string. The SQLDataReaderResult struct also returns the SQL connection,
        // which must be explicitly closed after the data from the DataReader
        // is bound into the controls.
        //
        // Other relevant sources:
        //     + <a href="ProductSearch.htm" style="color:green">ProductSearch Stored Procedure</a>
        //
        //*******************************************************

        public SqlDataReader SearchProductDescriptions(string searchString) {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(ConfigurationSettings.AppSettings["ConnectionString"]);
            SqlCommand myCommand = new SqlCommand("ProductSearch", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterSearch = new SqlParameter("@Search", SqlDbType.NVarChar, 255);
            parameterSearch.Value = searchString;
            myCommand.Parameters.Add(parameterSearch);

            // Execute the command
            myConnection.Open();
            SqlDataReader result = myCommand.ExecuteReader(CommandBehavior.CloseConnection);

            // Return the datareader result
            return result;
        }
    }
}