using System;
using System.Web.Services;
using IBuySpy;

public class InstantOrder : WebService {

    //*******************************************************
    //
    // InstantOrder.OrderItem() Method 
    //
    // The OrderItem method enables a remote client to programmatically
    // place an order using a webservice. 
    //
    //*******************************************************      

    [WebMethod(Description="The OrderItem method enables a remote client to programmatically place an order using a WebService.", EnableSession=false)]
    public OrderDetails OrderItem(string userName, string password, int productID, int quantity) {
        
        // Login client using provided username and password
        IBuySpy.CustomersDB accountSystem = new IBuySpy.CustomersDB();
        String customerId = accountSystem.Login(userName, password);
        
        if (customerId == null) {
            throw new Exception("Error: Invalid Login!");
        }

        // Wrap in try/catch block to catch errors in the event that someone types in
        // an invalid value for quantity
        int qty = System.Math.Abs(quantity);
        if (qty == quantity && qty < 1000) {
        
            // Add Item to Shopping Cart
            IBuySpy.ShoppingCartDB myShoppingCart = new IBuySpy.ShoppingCartDB();
            myShoppingCart.AddItem(customerId, productID, qty);

            // Place Order
            IBuySpy.OrdersDB orderSystem = new IBuySpy.OrdersDB();
            int orderID = orderSystem.PlaceOrder(customerId, customerId);
            
            // Return OrderDetails
            return orderSystem.GetOrderDetails(orderID, customerId);
        }
        else {
            // invalid input 
            return null;
        }
    }
     
    //*******************************************************
    //
    // InstantOrder.CheckStatus() Method 
    //
    // The CheckStatus method enables a remote client to programmatically
    // query the current status of an order in the IBuySpy System. 
    //
    //*******************************************************  
     
    [WebMethod(Description="The CheckStatus method enables a remote client to programmatically query the current status of an order in the IBuySpy System.", EnableSession=false)]
    public OrderDetails CheckStatus(string userName, string password, int orderID) {
     
        // Login client using provided username and password
        IBuySpy.CustomersDB accountSystem = new IBuySpy.CustomersDB();
        String customerId = accountSystem.Login(userName, password);
        
        if (customerId == null) {
            throw new Exception("Error: Invalid Login!");
        }
        
        // Return OrderDetails Status for Specified Order
        IBuySpy.OrdersDB orderSystem = new IBuySpy.OrdersDB();
        return orderSystem.GetOrderDetails(orderID, customerId);
    }
}