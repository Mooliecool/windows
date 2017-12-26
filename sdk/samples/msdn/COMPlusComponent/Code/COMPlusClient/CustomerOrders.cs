using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.EnterpriseServices;
using COMPlusServicesExample;
using System.Data;
using System.Data.SqlClient;


namespace COMPlus
{
	/// <summary>
	/// Summary description for CustomerOrders.
	/// </summary>
	public class CustomerOrders : System.Windows.Forms.Form
	{

		private int m_OrderID;
		private string m_ProductName;
		private int m_Quantity;
		private int m_DetailRow;
		private System.Windows.Forms.ComboBox cboCustomers;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button btnGetCust;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Button btnUpdate;
		private System.Windows.Forms.DataGrid dgOrders;
		private System.Windows.Forms.DataGrid dgOrderDetails;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public CustomerOrders()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.cboCustomers = new System.Windows.Forms.ComboBox();
			this.label1 = new System.Windows.Forms.Label();
			this.btnGetCust = new System.Windows.Forms.Button();
			this.dgOrders = new System.Windows.Forms.DataGrid();
			this.label2 = new System.Windows.Forms.Label();
			this.dgOrderDetails = new System.Windows.Forms.DataGrid();
			this.btnUpdate = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.dgOrders)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.dgOrderDetails)).BeginInit();
			this.SuspendLayout();
			// 
			// cboCustomers
			// 
			this.cboCustomers.Location = new System.Drawing.Point(112, 8);
			this.cboCustomers.Name = "cboCustomers";
			this.cboCustomers.Size = new System.Drawing.Size(232, 21);
			this.cboCustomers.TabIndex = 0;
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(16, 8);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(96, 23);
			this.label1.TabIndex = 1;
			this.label1.Text = "Select Customer:";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// btnGetCust
			// 
			this.btnGetCust.Location = new System.Drawing.Point(360, 8);
			this.btnGetCust.Name = "btnGetCust";
			this.btnGetCust.TabIndex = 2;
			this.btnGetCust.Text = "Get Orders";
			this.btnGetCust.Click += new System.EventHandler(this.btnGetCust_Click);
			// 
			// dgOrders
			// 
			this.dgOrders.DataMember = "";
			this.dgOrders.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.dgOrders.Location = new System.Drawing.Point(16, 72);
			this.dgOrders.Name = "dgOrders";
			this.dgOrders.Size = new System.Drawing.Size(424, 144);
			this.dgOrders.TabIndex = 3;
			this.dgOrders.MouseUp += new System.Windows.Forms.MouseEventHandler(this.dgOrders_MouseUp);
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(16, 56);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(100, 16);
			this.label2.TabIndex = 4;
			this.label2.Text = "Orders:";
			// 
			// dgOrderDetails
			// 
			this.dgOrderDetails.DataMember = "";
			this.dgOrderDetails.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.dgOrderDetails.Location = new System.Drawing.Point(16, 248);
			this.dgOrderDetails.Name = "dgOrderDetails";
			this.dgOrderDetails.Size = new System.Drawing.Size(424, 144);
			this.dgOrderDetails.TabIndex = 5;
			this.dgOrderDetails.MouseUp += new System.Windows.Forms.MouseEventHandler(this.dgOrderDetails_MouseUp);
			// 
			// btnUpdate
			// 
			this.btnUpdate.Location = new System.Drawing.Point(176, 416);
			this.btnUpdate.Name = "btnUpdate";
			this.btnUpdate.Size = new System.Drawing.Size(88, 23);
			this.btnUpdate.TabIndex = 6;
			this.btnUpdate.Text = "Update Details";
			this.btnUpdate.Click += new System.EventHandler(this.btnUpdate_Click);
			// 
			// CustomerOrders
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(456, 464);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.btnUpdate,
																		  this.dgOrderDetails,
																		  this.label2,
																		  this.dgOrders,
																		  this.btnGetCust,
																		  this.label1,
																		  this.cboCustomers});
			this.Name = "CustomerOrders";
			this.Text = "Customer Orders";
			this.Load += new System.EventHandler(this.CustomerOrders_Load);
			((System.ComponentModel.ISupportInitialize)(this.dgOrders)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.dgOrderDetails)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion

		private void CustomerOrders_Load(object sender, System.EventArgs e)
		{
			//loads the form with the customer information.

			//create instance of CustomersDB class
			CustomersDB customerData = new CustomersDB();
			
			//retrieve the Customers.
			DataSet datasetOrders = customerData.GetCustomers();
			
			//create the dataview object.
			DataView dataviewOrders = new DataView(datasetOrders.Tables[0]);
			cboCustomers.DataSource = dataviewOrders;

			//set the display properties of the Customers combo box.
			cboCustomers.DisplayMember = dataviewOrders.Table.Columns["CompanyName"].ColumnName;
			cboCustomers.ValueMember = dataviewOrders.Table.Columns["CustomerID"].ColumnName;
		}
		

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main()
		{
			Application.Run(new CustomerOrders());
		}

		private void btnGetCust_Click(object sender, System.EventArgs e)
		{
			//get the customer id
			string customerID = this.cboCustomers.SelectedValue.ToString();
			
			//create an instance of the CustomersDB class.
			CustomersDB customerData = new CustomersDB();
			//get the orders for the selected customer.
			DataSet datasetOrders = customerData.GetOrders(customerID);
			//bind the dataset to the datagrid.
			dgOrders.SetDataBinding(datasetOrders, datasetOrders.Tables[0].TableName);

		}

		private void dgOrders_MouseUp(object sender, MouseEventArgs e)
		{
			//gets the row info when a user clicks inside the orders
			//datagrid.
			DataGrid dg = (DataGrid) sender;
			DataGrid.HitTestInfo hitinfo = dg.HitTest(e.X, e.Y);
			
			//get the order id from the datagrid.
			int OrderID = Convert.ToInt32(this.dgOrders[hitinfo.Row, 0].ToString());
			m_OrderID = OrderID;
			
			//call the helper method to retrieve the
			//order details.
			FillOrderDetails(m_OrderID);

		}

		private void FillOrderDetails(int OrderID)
		{
			//create an instance of the CustomersDB class
			CustomersDB customer = new CustomersDB();
			DataSet datasetOrders = customer.GetOrderDetails(OrderID);
			//bind the data set to the datagrid.
			dgOrderDetails.SetDataBinding(datasetOrders, datasetOrders.Tables[0].TableName);
		}

		private void dgOrderDetails_MouseUp(object sender, MouseEventArgs e)
		{
			//gets the row info when a user clicks inside the orders
			//datagrid.
			DataGrid dg = (DataGrid) sender;
			DataGrid.HitTestInfo hitinfo = dg.HitTest(e.X, e.Y);
			
			//set the class variable.
			m_DetailRow = hitinfo.Row;
			string productName = this.dgOrderDetails[hitinfo.Row, 0].ToString();
			
			//set the class variable.
			m_ProductName = productName;
		}
		
		private void btnUpdate_Click(object sender, EventArgs e)
		{
			//get the quantity from the quantity cell.
			int Quantity = Convert.ToInt32(this.dgOrderDetails[m_DetailRow,2].ToString());
			m_Quantity = Quantity;
			//create an instance of the Customers class
			//to initiate the update.
			Customers customer = new Customers();
			//call the Update method.
			//if the quantity is less than or equal to zero, then 
			//the update will fail.
			bool updateIsSuccessful = customer.UpdateDetailQuantity(m_OrderID, m_ProductName, m_Quantity);
			string Message="";
			
			if (updateIsSuccessful == true)
			{
				Message = "Operation Succeeded and Transaction was Committed!";
			}
			else if (updateIsSuccessful == false)
			{
				Message = "Operation Failed and Transaction was Aborted!";
			}
			
			//show the confirmation message box.
			MessageBox.Show(Message);
			FillOrderDetails(m_OrderID);

		}


	}
}
