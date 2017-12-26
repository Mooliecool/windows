/************************************* Module Header **************************************\
* Module Name:  MainForm.cs
* Project:      CSWinFormDataBinding
* Copyright (c) Microsoft Corporation.
* 
* The CSWinFormDataBinding sample demonstrates the Windows Forms Data Binding technology.
* Data binding in Windows Forms gives you the means to display and make changes to 
* information from a data source in controls on the form. You can bind to both traditional 
* data sources as well as almost any structure that contains data.
* Windows Forms can take advantage of two types of data binding: 
* simple binding and complex binding. You can refer to this document for more information:
* http://msdn.microsoft.com/en-us/library/c8aebh9k.aspx 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 3/13/2009 3:00 PM ZhiXin Ye Created
\******************************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
#endregion


namespace CSWinFormDataBinding
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            ////////////////////////////////////////////////////////////////////////////////
            // Simple Binding Examples
            //


            #region Simple Binding Example 1 (Bind to class property)

            // In this example the CheckBox is the data source
            this.textBox1.DataBindings.Add("Text", this.checkBox1, "Checked");

            #endregion 

            #region Simple Binding Example 2 (Bind to class property)

            // In this example the Form itself is the data source.
            // With the update mode set to DataSourceUpdateMode.Never the data source won't
            // update unless we explicitly call the Binding.WriteValue() method.
            Binding bdSize = new Binding("Text", this, "Size", true, DataSourceUpdateMode.Never);
            this.textBox2.DataBindings.Add(bdSize);
            this.btnSet.Click += delegate(object button, EventArgs args)
            {
                // Force the value to store in the data source
                bdSize.WriteValue();
            };

            #endregion

            #region Simple Binding Example 3 (Bind to DataTable field)

            //  DataSource setup:
            //  
            //  Create a Table named Test and add 2 columns
            //   ID:     int
            //   Name:   string
            //
            DataTable dtTest = new DataTable();
            dtTest.Columns.Add("ID", typeof(int));
            dtTest.Columns.Add("Name", typeof(string));

            dtTest.Rows.Add(1, "John");
            dtTest.Rows.Add(2, "Amy");
            dtTest.Rows.Add(3, "Tony");

            BindingSource bsTest = new BindingSource();
            bsTest.DataSource = dtTest;

            // Bind the TextBoxes
            this.textBox3.DataBindings.Add("Text", bsTest, "ID");
            this.textBox4.DataBindings.Add("Text", bsTest, "Name");

            // Handle the button's click event to navigate the binding.
            this.btnPrev.Click += delegate(object button, EventArgs args)
            {
                bsTest.MovePrevious();
            };
            this.btnNext.Click += delegate(object button, EventArgs args)
            {
                bsTest.MoveNext();
            };

            #endregion


            ////////////////////////////////////////////////////////////////////////////////
            // Complex Binding Examples
            //


            #region Complex Binding Example 1 (Diplaying data from database)

            // This example is done by the Visual Studio designer.
            //
            // Steps:
            // 1. Click the smart tag glyph (Smart Tag Glyph) on the upper-right corner of 
            //    the DataGridView control.
            // 2. Click the drop-down arrow for the Choose Data Source option.
            // 3. If your project does not already have a data source, click 
            //    "Add Project Data Source.." and follow the steps indicated by the wizard. 
            // 4. Expand the Other Data Sources and Project Data Sources nodes if they are 
            //    not already expanded, and then select the data source to bind the control to. 
            // 5. If your data source contains more than one member, such as if you have 
            //    created a DataSet that contains multiple tables, expand the data source, 
            //    and then select the specific member to bind to. 

            #endregion

            #region Complex Binding Example 2 (Displaying data from business objects)

            // Data Source Setup:

            BindingList<Customer> blc = new BindingList<Customer>();

            blc.Add(new Customer(1, "John", 10.0M));
            blc.Add(new Customer(2, "Amy", 15.0M));
            blc.Add(new Customer(3, "Tony", 20.0M));

            //Bind the DataGridView to the list of Customers using complex binding.
            this.dataGridView2.DataSource = blc;

            #endregion

            #region Complex Binding Example 3 (Master/Detail Binding)

            // Data Source Setup:

            DataTable dtMaster = new DataTable("Custotmer");
            DataTable dtDetail = new DataTable("Order");
            dtMaster.Columns.Add("CustomerID", typeof(int));
            dtMaster.Columns.Add("CustomerName",typeof(string));

            dtDetail.Columns.Add("OrderID", typeof(int));
            dtDetail.Columns.Add("OrderDate",typeof(DateTime));
            dtDetail.Columns.Add("CustomerID", typeof(int));

            for (int j = 0; j < 5; j++)
            {
                dtMaster.Rows.Add(j, "Customer " + j.ToString());
                dtDetail.Rows.Add(j, DateTime.Now.AddDays(j), j);
                dtDetail.Rows.Add(j+5, DateTime.Now.AddDays(j+5), j);
            }

            // Create a DataSet to hold the two DataTables
            DataSet ds = new DataSet();
            ds.Tables.Add(dtMaster);
            ds.Tables.Add(dtDetail);

            // Add a relationship to the DataSet 
            ds.Relations.Add("CustomerOrder",
                ds.Tables["Custotmer"].Columns["CustomerID"],
                ds.Tables["Order"].Columns["CustomerID"]);

            BindingSource bsMaster = new BindingSource();
            bsMaster.DataSource = ds;
            bsMaster.DataMember = "Custotmer";

            BindingSource bsDetail = new BindingSource();
            // Bind the details data connector to the master data connector,
            // using the DataRelation name to filter the information in the
            // details table based on the current row in the master table. 
            bsDetail.DataSource = bsMaster;
            bsDetail.DataMember = "CustomerOrder";

            this.dgvMaster.DataSource = bsMaster;
            this.dgvDetail.DataSource = bsDetail;

            #endregion
        }

    }

    #region Customer Class

    public class Customer
    {
        // Private variables
        private int _id;
        private string _name;
        private Decimal _rate;

        // Constructor
        public Customer()
        {
            this.ID = -1;
            this.Name = string.Empty;
            this.Rate = 0.0M;
        }

        public Customer(int id, string name, Decimal rate)
        {
            this.ID = id;
            this.Name = name;
            this.Rate = rate;
        }

        // Properties
        public int ID
        {
            get { return _id; }
            set { _id = value; }
        }

        public string Name
        {
            get { return _name; }
            set { _name = value; }
        }

        public Decimal Rate
        {
            get { return _rate; }
            set { _rate = value; }
        }
    }

    #endregion
}
