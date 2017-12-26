================================================================================
       WPF APPLICATION : VBWPFMasterDetailBinding Project Overview
       
                        WPF Master/Detail DataBinding Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This example demonstrates how to do master/detail data binding in WPF.
   

/////////////////////////////////////////////////////////////////////////////
Code Logic:

    1. Create Order class and Customer class, which has a master/detail 
       relationship.
       
    2. Create a CustomerList class to provide a list of customers for binding.
       
    3. In MainWindow.xaml define the CustomerList object in the window resources.
    
        <Window.Resources>
            <local:CustomerList x:Key="CustomerList"/>
        </Window.Resources>
       
    4. Bind the listViewCustomers to the Customers property.
       
    5. Bind the listViewOrders to the Orders property of the selected customer object.
   
   
/////////////////////////////////////////////////////////////////////////////
References:



/////////////////////////////////////////////////////////////////////////////
Demo

Step1. Build the sample project in Visual Studio 2008.

Step2. Start Without Debugging, and the mian window of the project will show.

Step3. The window contains two listview, the first one shows the customer list,
and the second shows the order lists of the selected customer.

Step4. Select one item of the customer list, the second listview shows the
order list of the selected customer

/////////////////////////////////////////////////////////////////////////////