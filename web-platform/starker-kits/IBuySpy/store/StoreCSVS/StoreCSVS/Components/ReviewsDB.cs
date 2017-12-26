using System;
using System.Configuration;
using System.Data;
using System.Data.SqlClient;

namespace IBuySpy {

    //*******************************************************
    //
    // ReviewsDB Class
    //
    // Business/Data Logic Class that encapsulates all data
    // logic necessary to list/access/add reviews from
    // the IBuySpy Reviews database.
    //
    //*******************************************************

    public class ReviewsDB {

        //*******************************************************
        //
        // ReviewsDB.GetReviews() Method <a name="GetReviews"></a>
        //
        // The GetReviews method returns a struct containing
        // a forward-only, read-only DataReader.  This displays a list of all
        // user-submitted reviews for a specified product.
        // The SQLDataReaderResult struct also returns the SQL connection,
        // which must be explicitly closed after the data from the DataReader
        // is bound into the controls.
        //
        // Other relevant sources:
        //     + <a href="ReviewsList.htm" style="color:green">ReviewsList Stored Procedure</a>
        //
        //*******************************************************

        public SqlDataReader GetReviews(int productID) {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(ConfigurationSettings.AppSettings["ConnectionString"]);
            SqlCommand myCommand = new SqlCommand("ReviewsList", myConnection);

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
        // ReviewsDB.AddReview() Method <a name="AddReview"></a>
        //
        // The AddReview method adds a new review into the
        // IBuySpy Reviews database.
        //
        // Other relevant sources:
        //     + <a href="ReviewsAdd.htm" style="color:green">ReviewsAdd Stored Procedure</a>
        //
        //*******************************************************

        public void AddReview(int productID, string customerName, string customerEmail, int rating, string comments) {

            // Create Instance of Connection and Command Object
            SqlConnection myConnection = new SqlConnection(ConfigurationSettings.AppSettings["ConnectionString"]);
            SqlCommand myCommand = new SqlCommand("ReviewsAdd", myConnection);

            // Mark the Command as a SPROC
            myCommand.CommandType = CommandType.StoredProcedure;

            // Add Parameters to SPROC
            SqlParameter parameterProductID = new SqlParameter("@ProductID", SqlDbType.Int, 4);
            parameterProductID.Value = productID;
            myCommand.Parameters.Add(parameterProductID);

            SqlParameter parameterCustomerName = new SqlParameter("@CustomerName", SqlDbType.NVarChar, 50);
            parameterCustomerName.Value = customerName;
            myCommand.Parameters.Add(parameterCustomerName);

            SqlParameter parameterEmail = new SqlParameter("@CustomerEmail", SqlDbType.NVarChar, 50);
            parameterEmail.Value = customerEmail;
            myCommand.Parameters.Add(parameterEmail);

            SqlParameter parameterRating = new SqlParameter("@Rating", SqlDbType.Int, 4);
            parameterRating.Value = rating;
            myCommand.Parameters.Add(parameterRating);

            SqlParameter parameterComments = new SqlParameter("@Comments", SqlDbType.NVarChar, 3850);
            parameterComments.Value = comments;
            myCommand.Parameters.Add(parameterComments);

            SqlParameter parameterReviewID = new SqlParameter("@ReviewID", SqlDbType.Int, 4);
            parameterReviewID.Direction = ParameterDirection.Output;
            myCommand.Parameters.Add(parameterReviewID);

            // Open the connection and execute the Command
            myConnection.Open();
            myCommand.ExecuteNonQuery();
            myConnection.Close();
        }
    }
}

