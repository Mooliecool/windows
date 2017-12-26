/****************************** Module Header ******************************\
 * Module Name: NAVWebServicesSampleCode
 * Project: NAVWebServicesSampleCode
 * Copyright (c) Microsoft Corporation. 
 * 
 * This project shows code samples for implementing the methods that are available via 
 * Dynamics NAV Web Services. Running it requires a running installation of NAV 2009 or a higher version.
 * 
 * This source is subject to the Microsoft Public License. 
 * See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL. 
 * All other rights reserved. 
 * 
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
 * OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE. 
\***************************************************************************/


using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace NAVWebServicesSampleCode
{
    public partial class CSDynamicsNAVWebServices : Form
    {
        public CSDynamicsNAVWebServices()
        {

            InitializeComponent();
            CodeunitURL.Text = "http://lohndorf02:7047/DynamicsNAV/WS/CRONUS International Ltd/Codeunit/NAVCodeunit";
            PageURL.Text = "http://lohndorf02:7047/DynamicsNAV/WS/CRONUS International Ltd/Page/Customer";

        }

        private void btnCodeunitWebService_Click(object sender, EventArgs e)
        {
            #region Initialize
            using (NAVCodeunitWS.NAVCodeunit NAV = new NAVCodeunitWS.NAVCodeunit())
            {
                NAV.Url = CodeunitURL.Text;
                NAV.UseDefaultCredentials = true;
                // Example NAV.RunFunction();
            }
            #endregion
        }

        private void btnPageWebService_Click(object sender, EventArgs e)
        {
            using (NAVPageWS.Customer_Service NAV = new NAVPageWS.Customer_Service())
            {
                NAV.Url = PageURL.Text;
                NAV.UseDefaultCredentials = true;


                Results.Items.Clear();


                switch (Convert.ToString(cmbType.SelectedItem))
                {
                    // Options
                    // Full List
                    // Filtered List Simple
                    // Filtered List Advanced
                    // READ
                    // CREATE
                    // CREATEMULTIPLE
                    // UPDATE
                    // UPDATEMULTIPLE
                    // DELETE

                    case "":
                        MessageBox.Show("Please select a sample type");
                        break;

                    #region READS
                    #region Full List
                    case "Full List":
                        // Read all records
                        NAVPageWS.Customer[] custList = NAV.ReadMultiple(null, null, 0);
                        foreach (NAVPageWS.Customer Cust in custList)
                        {
                            Results.Items.Add(Cust.No + " " + Cust.Name);
                        }
                        break;
                    #endregion


                    #region Filtered List (simple)
                    case "Filtered List Simple":
                        // Adding 2 simple filters
                        List<NAVPageWS.Customer_Filter> filterArray = new List<NAVPageWS.Customer_Filter>();

                        // Filter 1
                        NAVPageWS.Customer_Filter nameFilter = new NAVPageWS.Customer_Filter();
                        nameFilter.Field = NAVPageWS.Customer_Fields.Name;
                        nameFilter.Criteria = "@*s*"; //Name containing s - case insensitive
                        filterArray.Add(nameFilter);

                        // Filter 2
                        NAVPageWS.Customer_Filter addressFilter = new NAVPageWS.Customer_Filter();
                        addressFilter.Field = NAVPageWS.Customer_Fields.Address;
                        addressFilter.Criteria = "P*"; // Address beginning with p - Case sensitive
                        filterArray.Add(addressFilter);

                        // Get list with the two filters applied
                        NAVPageWS.Customer[] filteredCustList = NAV.ReadMultiple(filterArray.ToArray(), null, 0);
                        foreach (NAVPageWS.Customer Cust in filteredCustList)
                        {
                            Results.Items.Add(Cust.No + " : " + Cust.Name + " : " + Cust.Address);
                        }
                        break;
                    #endregion


                    #region Filtered List (Advanced)
                    case "Filtered List Advanced":
                        // More advanced filter structure to make it easier to add multiple filters
                        // requires additional function AddFilter and variable Filters which must be copied from the bottom of this page
                        AddFilter("Name", "@*S*");
                        AddFilter("Location_Code", "BLUE");
                        NAVPageWS.Customer[] filteredCustList2 = NAV.ReadMultiple(Filters.ToArray(), null, 0);
                        foreach (NAVPageWS.Customer Cust in filteredCustList2)
                        {
                            Results.Items.Add(Cust.Name + " : " + Cust.Location_Code);
                        }
                        break;
                    #endregion


                    #region READ
                    case "READ":
                        NAVPageWS.Customer Customer = NAV.Read("10000");
                        MessageBox.Show(Convert.ToString(Customer.Name));
                        break;
                    #endregion
                    #endregion

                    #region UPDATES


                    #region CREATE
                    case "CREATE":
                        NAVPageWS.Customer newCust = new NAVPageWS.Customer();
                        try // Return NAV error if record already exists instead of exception error
                        {
                            newCust.No = null; // Run OnInsert Trigger in NAV to assign number
                            newCust.No = "ABC"; // Assign number manually
                            NAV.Create(ref newCust); //Insert record
                        }
                        catch (Exception NAVerr)
                        {
                            MessageBox.Show(NAVerr.Message);
                        }
                        MessageBox.Show("Customer " + Convert.ToString(newCust.No) + " created.");
                        break;
                    #endregion

                    #region CREATEMULTIPLE
                    case "CREATEMULTIPLE":
                        NAVPageWS.Customer newCust1 = new NAVPageWS.Customer();
                        NAVPageWS.Customer newCust2 = new NAVPageWS.Customer();
                        newCust1.No = null; // assign number from No. Series
                        newCust2.No = null;
                        NAVPageWS.Customer[] NewCustomers = new NAVPageWS.Customer[] { newCust1, newCust2 };
                        NAV.CreateMultiple(ref NewCustomers);

                        MessageBox.Show("Customers " + NewCustomers[0].No + " and " + NewCustomers[1].No + " created.");

                        break;
                    #endregion


                    #region UPDATE
                    case "UPDATE":
                        // Retrieve record to modify
                        // Assumes Customer ABC exists
                        NAVPageWS.Customer modifyCust = NAV.Read("ABC");
                        //Check if read succeeded (Cust ABC Exists)            
                        if (modifyCust == null)
                        {
                            //Error message for information only - no error handling is done here.
                            MessageBox.Show("Attempt to modify customer ABC failed because it does not exist. NullReference exception will follow.");
                        };

                        string OldName = modifyCust.Name;
                        modifyCust.Name = "New Name";
                        NAV.Update(ref modifyCust);
                        MessageBox.Show("Old Name: " + OldName + "\n" + "New Name: " + Convert.ToString(modifyCust.Name));
                        // For some datatypes, also set fieldSpecified = true
                        modifyCust.Credit_Limit_LCY += 5;
                        modifyCust.Credit_Limit_LCYSpecified = true;
                        NAV.Update(ref modifyCust);
                        break;
                    #endregion

                    #region UPDATEMULTIPLE
                    case "UPDATEMULTIPLE":
                        // Update all customers with name "New name"
                        NAVPageWS.Customer_Filter custFilter = new NAVPageWS.Customer_Filter();
                        custFilter.Field = NAVPageWS.Customer_Fields.Name;
                        custFilter.Criteria = "New Name";

                        List<NAVPageWS.Customer_Filter> custFilterArray = new List<NAVPageWS.Customer_Filter>();
                        custFilterArray.Add(custFilter);
                        NAVPageWS.Customer[] Customers = NAV.ReadMultiple(custFilterArray.ToArray(), null, 0);
                        int customerCount = 0;
                        foreach (NAVPageWS.Customer UpdateCust in Customers)
                        {
                            UpdateCust.Address = "New Address";
                            customerCount++;
                        }
                        NAV.UpdateMultiple(ref Customers);
                        MessageBox.Show(Convert.ToString(customerCount) + " customers updated.");

                        break;
                    #endregion


                    #region DELETE
                    case "DELETE":
                        // Assumes Customer ABC exists
                        // Retrieve customer to delete
                        NAVPageWS.Customer deleteCust = NAV.Read("ABC");
                        try
                        {
                            NAV.Delete(deleteCust.Key);
                        }
                        catch (Exception deleteException)
                        {
                            MessageBox.Show("Did not find customer ABC. Exception will now follow.");
                            MessageBox.Show(deleteException.Message);
                        }
                        break;
                    #endregion


                    #endregion
                }
            }
        }

        // For use for Filtered List (Advanced)
        // New Filters and AddFilter for use in Advanced filter region above.
        List<NAVPageWS.Customer_Filter> Filters = new List<NAVPageWS.Customer_Filter>();

        public void AddFilter(string filterField, string filterValue)
        {
            NAVPageWS.Customer_Filter newFilter = new NAVPageWS.Customer_Filter();            
            newFilter.Field = (NAVPageWS.Customer_Fields)Enum.Parse(typeof(NAVPageWS.Customer_Fields), filterField, true);
            newFilter.Criteria = filterValue;
            this.Filters.Add(newFilter);
        }

        private void cmbType_SelectedIndexChanged(object sender, EventArgs e)
        {
            Results.Items.Clear();
            switch (Convert.ToString(cmbType.SelectedItem))
            {

                case "Full List":
                    Results.Items.Add("Lists all customers");
                    break;
                case "Filtered List Simple":
                    Results.Items.Add("Customers whose name contains S and Address begins with P.");
                    break;
                case "Filtered List Advanced":
                    Results.Items.Add("Customers whose name contain S and have Location Code BLUE.");
                    break;
                case "READ":
                    Results.Items.Add("Reads Customer 10000");
                    break;
                case "CREATE":
                    Results.Items.Add("Creates customer with No. ABC.");
                    break;
                case "CREATEMULTIPLE":
                    Results.Items.Add("Creates two customers, using the No. Series.");
                    break;
                case "UPDATE":
                    Results.Items.Add("Updates customer ABC (Customer must exist first).");
                    break;
                case "UPDATEMULTIPLE":
                    Results.Items.Add("For all customers with name New Name, address is updated.");
                    break;
                case "DELETE":
                    Results.Items.Add("Deletes Customer ABC.");
                    break;
            }
        }

    }
}
