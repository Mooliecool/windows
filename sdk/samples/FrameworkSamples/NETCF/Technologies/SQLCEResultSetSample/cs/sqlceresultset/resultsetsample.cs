//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

#region Using directives

using System;
using System.Drawing;
using System.Collections;
using System.Windows.Forms;
using System.Data;
using System.Data.SqlServerCe;
using System.Text;
using System.IO;
using System.Text.RegularExpressions;

#endregion

namespace Microsoft.Samples.ResultSetSample
{
    /// <summary>
    /// Main Application Form Class.
    /// </summary>
    public partial class ResultSetSample : System.Windows.Forms.Form
    {
        #region Variables
        private SqlCeConnection connection = null;
        private SqlCeCommand command = null;
        private SqlCeResultSet resultSet = null;
        private ResultSetView view1 = null;
        private ResultSetView view2 = null;

        private DataTable table = null;
        #endregion

        //Constructor
        public ResultSetSample()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();

            // The connection is initialized just after the form is created
            if (null == this.connection)
            {
                string _connString = String.Format(System.Globalization.CultureInfo.InvariantCulture,
                @"Data Source = {0}\Northwind.sdf",Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().GetName().CodeBase));
                this.connection = new SqlCeConnection(_connString);
                this.connection.Open();
                this.menuItemUpdatable.Checked = true;
            }
        }


        #region EVENT_HANDLERS
        /// <summary>
        /// Click Event handler for the Clear Menu Item
        /// </summary>
        private void menuItemClear_Click(object sender, EventArgs e)
        {
            //Dispose the existing Result Set
            if (null != this.resultSet)
                this.resultSet.Dispose();

            //Clear the DataSource of the DataGrid.
            this.dataGrid.DataSource = null;

            //Clear any existing Bindings
            this.textBox1.DataBindings.Clear();
            this.textBox2.DataBindings.Clear();
            this.textBox3.DataBindings.Clear();
            this.textBox4.DataBindings.Clear();

            //Clear the text in the Text Boxes
            this.textBox1.Text = string.Empty;
            this.textBox2.Text = string.Empty;
            this.textBox3.Text = string.Empty;
            this.textBox4.Text = string.Empty;

        }

        /// <summary>
        /// Click Event handler for the Exit Menu Item
        ///  - Exits the Application
        /// </summary>
        private void menuItemExit_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        /// <summary>
        /// Click Event handler for the Use DataSet Menu Item
        ///  - Uses a DataSet for binding instead of the SqlCeResultSet
        ///  - if this menu item is checked
        /// </summary>
        private void menuItemUseDataSet_Click(object sender, EventArgs e)
        {
            this.menuItemUseDataSet.Checked = !(this.menuItemUseDataSet.Checked);
            if (menuItemUseDataSet.Checked)
            {
                this.menuItemUpdatable.Enabled = false;
            }
            else
            {
                this.menuItemUpdatable.Enabled = true;
            }

        }

        /// <summary>
        /// Click Event handler for the Use Updateable Menu Item
        ///  - Allows the SqlCeResultSet to be Updateable 
        ///  - if this menu item is checked
        /// </summary>
        private void menuItemUpdatable_Click(object sender, EventArgs e)
        {
            this.menuItemUpdatable.Checked = !(this.menuItemUpdatable.Checked);
        }

        /// <summary>
        /// Click Event handler for the Query and Bind button
        /// </summary>
        private void btnExecute_Click(object sender, EventArgs e)
        {
            //Clear the text in the filter textbox above the datagrid
            tbCommand.Text = String.Empty;
            // Disable the button till we are done quering and binding
            btnExecute.Enabled = false;
            try
            {

                // Dispose previous views bound to the currently active RS
                if (null != view1) ((IDisposable)view1).Dispose();
                if (null != view2) ((IDisposable)view2).Dispose();

                // Dispose previous SqlCeCommand and previous SqlCeResultSet
                if (null != this.command) this.command.Dispose();
                if (null != this.resultSet) this.resultSet.Dispose();

                //Creates a Command with the associated connection
                this.command = this.connection.CreateCommand();

                // Use the SqlCeResultSet if the "Use DataSet" menu item is not
                // checked
                if (false == this.menuItemUseDataSet.Checked)
                {
                    // Queury the Orders table in the Northwind database
                    this.command.CommandText = "SELECT * FROM Orders";
                    ResultSetOptions options = ResultSetOptions.Scrollable | ResultSetOptions.Sensitive;

                    if (this.menuItemUpdatable.Checked) options |= ResultSetOptions.Updatable;

                    this.resultSet = this.command.ExecuteResultSet(options);

                    this.dataGrid.DataSource = null;
                    // Bind the result set to the controls
                    this.BindData();
                }
                else
                {
                    //Retrieve the columns we are interested in from the Orders table
                    //Note that we do not specify this in the SqlCeResultSet queury above
                    // because we demonstrate the use of the Ordinals property in the 
                    // ResultSetView.
                    string query = @"SELECT [Customer ID], [Ship Name],[Ship City]
                                    ,[Ship Country] FROM Orders";
                    this.command.CommandText = query;
                    table = new DataTable("Orders");
                    table.Locale = System.Globalization.CultureInfo.InvariantCulture;
                    SqlCeDataAdapter adapter = new SqlCeDataAdapter(this.command);

                    adapter.FillSchema(table, SchemaType.Source);
                    adapter.Fill(table);

                    this.dataGrid.DataSource = null;
                    this.BindData();
                }
                btnExecute.Enabled = true;
            }
            catch (InvalidOperationException ex)
            {
                btnExecute.Enabled = true;
                MessageBox.Show(String.Format(System.Globalization.CultureInfo.CurrentCulture,
                    "Exception while Performing Query/Bind: \n {0}",ex.ToString()));
            }
            catch (SqlCeException ex)
            {
                btnExecute.Enabled = true;
                ShowErrors(ex);
            }
        }
        /// <summary>
        /// Text Changed event handler for the tbCommand TextBox
        /// - In this handler the data displayed in the grid is filtered
        /// - on the Ship Name column according to what is typed in the TextBox
        /// </summary>
        private void tbCommand_TextChanged(object sender, EventArgs e)
        {
            try
            {
                if (false == this.menuItemUseDataSet.Checked)
                {
                    if (null == this.resultSet)
                    {
                        MessageBox.Show("Command hasn't been executed. Press execute button first");
                        return;
                    }
                    ResultSetOptions options = ResultSetOptions.Scrollable | ResultSetOptions.Sensitive;
                    if (this.menuItemUpdatable.Checked) options |= ResultSetOptions.Updatable;

                    // Query the database again using the WHERE clause to filter according
                    // to the input in the text box
                    string query = String.Format(System.Globalization.CultureInfo.InvariantCulture,
                        "SELECT * FROM Orders WHERE [Ship Name] LIKE '{0}%' ", tbCommand.Text);
                    
                    this.command.CommandText = query;

                    this.resultSet = this.command.ExecuteResultSet(options);
                    this.BindData();
                }
                else
                {
                    if (null == this.table)
                    {
                        MessageBox.Show("Command hasn't been executed. Press execute button first");
                        return;
                    }
                    //Row filters are used to filter the table since the entire table
                    // is already loaded in memory
                    string filterExpression = String.Format(System.Globalization.CultureInfo.InvariantCulture,"[Ship Name] LIKE '{0}%' ", tbCommand.Text);
                    table.DefaultView.RowFilter = filterExpression;
                    this.BindData();
                }
            }
            catch (SqlCeException ex)
            {
                ShowErrors(ex);
            }
        }

        /// <summary>
        /// Click Event handler for the Previous button
        /// Decrements the position of the BindingManagerBase that is associated
        /// with the DataSource in use. If the cursor is already at the beginning 
        /// of the list it remains there
        /// </summary>
        private void btnPrev_Click(object sender, EventArgs e)
        {
            if (false == this.menuItemUseDataSet.Checked)
            {
                this.BindingContext[this.view2].Position -= 1;
            }
            else
            {
                this.BindingContext[this.table].Position -= 1;
            }
        }
        /// <summary>
        /// Click Event handler for the Next button
        /// Increments the position of the BindingManagerBase that is associated
        /// with the DataSource in use. If the cursor is already at the end 
        /// of the list it remains there
        /// </summary>
        private void btnNext_Click(object sender, EventArgs e)
        {
            if (false == this.menuItemUseDataSet.Checked)
            {
                this.BindingContext[this.view2].Position += 1;
            }
            else
            {
                this.BindingContext[this.table].Position += 1;
            }
        }

        #endregion

        /// <summary>
        /// Binds the SqlCeResultSet or DataSet to the controls in the UI
        /// </summary>
        private void BindData()
        {
                //Clear any existing bindings
                this.textBox1.DataBindings.Clear();
                this.textBox2.DataBindings.Clear();
                this.textBox3.DataBindings.Clear();
                this.textBox4.DataBindings.Clear();

                //Clear the text in the text boxes
                this.textBox1.Text = string.Empty;
                this.textBox2.Text = string.Empty;
                this.textBox3.Text = string.Empty;
                this.textBox4.Text = string.Empty;

                if (false == this.menuItemUseDataSet.Checked)
                {
                    if (null == this.resultSet)
                    {
                        MessageBox.Show("SQL Command has not been executed.Press execute button first");
                        return;
                    }

                    // Dispose previous views bound to the currently active RS
                    //
                    if (null != view1) ((IDisposable)view1).Dispose();
                    if (null != view2) ((IDisposable)view2).Dispose();

                    //Bind the data grid control
                    this.view1 = this.resultSet.ResultSetView;

                    //This array contains the ordinals of the columns displayed in the grid
                    //Currently it is set to display only columns 1,3,5 and 8
                    int[] ordinals = new int[] { 1,3,5,8};
                    this.view1.Ordinals = ordinals;

                    this.dataGrid.DataSource = view1;

                    // Bind individual text boxes 
                    this.view2 = this.resultSet.ResultSetView;

                    this.textBox1.DataBindings.Add(
                            "text", view2, resultSet.GetSqlMetaData(1).Name);

                    this.textBox2.DataBindings.Add(
                            "text", view2, resultSet.GetSqlMetaData(3).Name);

                    this.textBox3.DataBindings.Add(
                        "text", view2, resultSet.GetSqlMetaData(5).Name);

                    this.textBox4.DataBindings.Add(
                        "text", view2, resultSet.GetSqlMetaData(8).Name);

                }
                else
                {
                    if (null == this.table)
                    {
                        MessageBox.Show("SQL Command has not been executed.Press execute button first");
                        return;
                    }
                    // Binding the DataGrid to the DefaultView of the DataTable
                    this.dataGrid.DataSource = table.DefaultView;

                    //Bind the individual text boxes
                    this.textBox1.DataBindings.Add(
                        "text", table, table.Columns[0].ColumnName);

                    this.textBox2.DataBindings.Add(
                        "text", table, table.Columns[1].ColumnName);

                    this.textBox3.DataBindings.Add(
                        "text", table, table.Columns[2].ColumnName);

                    this.textBox4.DataBindings.Add(
                        "text", table, table.Columns[3].ColumnName);
                }
        }

        #region HELPER functions
        /// <summary>
        /// Displays the Details of a SqlCeException
        /// </summary>
        public static void ShowErrors(SqlCeException e)
        {
            SqlCeErrorCollection errorCollection = e.Errors;

            StringBuilder bld = new StringBuilder();
            Exception inner = e.InnerException;

            foreach (SqlCeError err in errorCollection)
            {
                bld.Append("\n Error Code: " + err.HResult.ToString("X",
                    System.Globalization.CultureInfo.CurrentCulture));
                bld.Append("\n Message   : " + err.Message);
                bld.Append("\n Minor Err.: " + err.NativeError);
                bld.Append("\n Source    : " + err.Source);

                foreach (int numPar in err.NumericErrorParameters)
                {
                    if (0 != numPar) bld.Append("\n Num. Par. : " + numPar);
                }

                foreach (string errPar in err.ErrorParameters)
                {
                    if (String.Empty != errPar) bld.Append("\n Err. Par. : " + errPar);
                }
            }
            MessageBox.Show(bld.ToString());
        }
        #endregion

    }
}


