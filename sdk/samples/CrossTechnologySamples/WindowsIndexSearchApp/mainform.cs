using System;
using System.Collections;
using System.Collections.Specialized;
using System.Drawing;
using System.Windows.Forms;
using System.Data;
using System.Data.OleDb;
using System.Configuration;
using WinFormsSearchApp.Properties;

namespace WinFormsSearchApp
{
   public partial class MainForm : Form
	{
		#region Private Member Definitions
		
		private bool isLoading = true;
		private DataTable propDefsTable = null;
		private StringCollection selectedColumns = new StringCollection();
		private int sortColumn = 0;
		
		#endregion

		public MainForm()
      {
         InitializeComponent();
      }

      private void OnFormLoad(object sender, EventArgs e)
      {
         // Read configuration information
         // For demo purposes, to enable running with a larger font size 
         if (Settings.Default.FontSize != 0)
         {
            this.Font = new Font(this.Font.Name, Settings.Default.FontSize,
                this.Font.Style, this.Font.Unit);
         }

         // Load previously saved queries
         LoadQueries(this.cmbQueries, Settings.Default.Queries);

			this.propDefsTable = PropertySystemHelper.GetPropertyDefinitions();

         // Initialize scope information
         InitScopes(this.cmbScope, this.propDefsTable);

         this.dataGridView1.AutoGenerateColumns = true;
         this.dataGridView1.AllowUserToResizeColumns = true;
         this.txtSQL.Text = "SELECT <columns> FROM SYSTEMINDEX..SCOPE() [WHERE <predicates>]";
         this.isLoading = false;
		}

		// Run the query defined in the query text box and display the results
		private void OnRunQuery(object sender, EventArgs e)
		{
			DataTable dt = ExecuteWindowsSearchQuery(this.txtSQL.Text);
			this.dataGridView1.DataSource = dt;

			this.tabControl.SelectTab("tabPageResults");
			this.statusStrip.Items[0].Text = string.Format("Number of items: {0}", this.dataGridView1.Rows.Count);
		}

		DataTable ExecuteWindowsSearchQuery(string QueryText)
		{
			string connectionString =
				@"Provider=Search.CollatorDSO;Extended Properties='Application=Windows'";
			DataTable dt = new DataTable("QueryResults");

			try
			{
				using (OleDbConnection connection =
					new OleDbConnection(connectionString))
				{
					connection.Open();

					OleDbCommand cmd = new OleDbCommand(
						QueryText, connection);
					OleDbDataReader reader = cmd.ExecuteReader();
					if (!reader.HasRows) { return (null); }

					int numColumns = reader.FieldCount;

					// Create a column in the datatable for each column returned in the query
					for (int i = 0; i < numColumns; i++)
					{
						dt.Columns.Add(reader.GetName(i));
					}

					object[] values = new object[numColumns];

					while (reader.Read())
					{
						reader.GetValues(values);
						string[] rowTextValues = GetRowTextValues(values);

						dt.Rows.Add(rowTextValues);
					}
				}
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.Message);
			}

			return (dt);
		}

		// Process a row from the query results, and combine any multi-valued columns
		string[] GetRowTextValues(object[] QueryResultValues)
		{
			string[] textValues = new string[QueryResultValues.Length];
			for (int i = 0; i < QueryResultValues.Length; i++)
			{
				if (!QueryResultValues[i].GetType().IsArray)
				{
					textValues[i] = QueryResultValues[i].ToString();
				}
				else
				{
					int count = 0;
					string colValue = "";

					foreach (object o in QueryResultValues[i] as Array)
					{
						if (count++ > 0)
						{
							colValue += ";";
						}
						colValue += o.ToString();
					}
					textValues[i] = colValue;
				}
			}

			return (textValues);
		}

		#region Query ComboBox UI
		
		// Loads query selected in drop-down list into the query text box
		private void LoadQueries(ComboBox QueriesCmb, StringCollection Queries)
		{
			QueriesCmb.Items.Clear();

			foreach (string s in Queries)
			{
				QueriesCmb.Items.Add(s);
			}
		}
		
		private void OnLoadQuery(object sender, EventArgs e)
      {
         if (this.cmbQueries.SelectedItem == null) { return; }
         this.txtSQL.Text = this.cmbQueries.SelectedItem.ToString();
      }

      // Stores the contents of the query text box in the config file and in the query drop-down list
      private void OnSaveQuery(object sender, EventArgs e)
      {
         Settings.Default.Queries.Add(this.txtSQL.Text);
         Settings.Default.Save();
         this.cmbQueries.Items.Add(txtSQL.Text);
		}
		
		#endregion

		#region Property DataGridView UI

		int LoadPropertiesInScope(ListView Properties, string Scope, DataTable PropDefs)
		{
			DataRow[] rows = PropDefs.Select(string.Format("scope = '{0}'", Scope));
			foreach (DataRow row in rows)
			{
				ListViewItem lvItem = Properties.Items.Add(row["Name"].ToString());
				lvItem.SubItems.Add(row["Type"].ToString());
				lvItem.SubItems.Add(row["MultipleValues"].ToString());
				lvItem.SubItems.Add(row["IsViewable"].ToString());
				lvItem.SubItems.Add(row["IsColumn"].ToString());
			}

			return (rows.Length);
		}

		// Refresh the contents of the Columns grid to contain the properties contained within the selected scope
		private void OnPropertyScopeChanged(object sender, EventArgs e)
		{
			this.lvProperties.Items.Clear();
			this.sortColumn = 0;
			this.lvProperties.ListViewItemSorter = null;
			this.lvProperties.Sorting = SortOrder.Ascending;

			string scope = this.cmbScope.SelectedItem.ToString();
			int count = LoadPropertiesInScope(this.lvProperties, scope, this.propDefsTable);
			this.statusStrip.Items[0].Text = string.Format("Number of columns: {0}", count);
		}

		// Insert each selected property into the query text box at the current insertion point
      private void OnInsertColumns(object sender, EventArgs e)
      {
         InsertColumnNames();
      }

      // Adds/Removes a column name to selectedColumns; 
      // The contents of this list is inserted when the user clicks on the Insert button, or
      // selects and item with double click
      private void OnColumnChecked(object sender, ItemCheckedEventArgs e)
      {
         if (this.isLoading) { return; } // do nothing, during form initialization
         string columnText = string.Format("\"{0}\"", e.Item.Text);

         if (e.Item.Checked)
         {
            selectedColumns.Add(columnText);
         }
         else
         {
            if (selectedColumns.IndexOf(columnText) >= 0)
            {
               selectedColumns.Remove(columnText);
            }
         }
      }

      private void OnColumnHeaderClicked(object sender, ColumnClickEventArgs e)
      {
			ListView lv = sender as ListView;

			if (e.Column == this.sortColumn)
			{
				lv.Sorting = (lv.Sorting == SortOrder.Ascending) ? SortOrder.Descending : SortOrder.Ascending;
			}
			else
			{
				lv.Sorting = SortOrder.Ascending;
			}

			lv.ListViewItemSorter = new ListViewItemComparer(e.Column, lv.Sorting);

         this.sortColumn = e.Column;
		}

		private void OnColumnsDoubleClick(object sender, EventArgs e)
		{
			InsertColumnNames();
		}

		private void InsertColumnNames()
		{
			string colText = "";
			for (int i = 0; i < this.selectedColumns.Count; i++)
			{
				if (i > 0)
				{
					colText += ", ";
				}
				colText += this.selectedColumns[i];
			}

			string queryText = this.txtSQL.Text.Remove(this.txtSQL.SelectionStart, this.txtSQL.SelectionLength);
			string s = queryText.Insert(this.txtSQL.SelectionStart, colText);
			int selIndex = this.txtSQL.SelectionStart + colText.Length;

			// Insert text into current insertion point
			this.txtSQL.Text = s;
			this.txtSQL.SelectionStart = selIndex;
			this.txtSQL.SelectionLength = 0;

			this.txtSQL.Focus();

			// Clear checked items and list of selected column names
			this.selectedColumns.Clear();
			int numCheckedColumns = this.lvProperties.CheckedIndices.Count;
			for (int i = 0; i < numCheckedColumns; i++)
			{
				this.lvProperties.Items[this.lvProperties.CheckedIndices[0]].Checked = false;
			}
		}

		// Implements the manual sorting of items by columns.
		class ListViewItemComparer : IComparer
		{
			private int sortColumn;
			private SortOrder sortOrder;
			public ListViewItemComparer()
			{
				this.sortColumn = 0;
				this.sortOrder = SortOrder.Ascending;
			}
			public ListViewItemComparer(int Column, SortOrder Order)
			{
				this.sortOrder = Order;
				this.sortColumn = Column;
			}
			public int Compare(object x, object y)
			{
				int sort = String.Compare(((ListViewItem)x).SubItems[this.sortColumn].Text,
					 ((ListViewItem)y).SubItems[this.sortColumn].Text);

				if (this.sortOrder == SortOrder.Descending)
				{
					sort *= -1;
				}
				return (sort);
			}
		}
		#endregion

		#region Property Scope ComboBox UI 

		// Initializes property scope information and fill the Scopes combo box
		void InitScopes(ComboBox Scopes, DataTable PropDefs)
		{
			Scopes.Items.Clear();

			StringCollection scopeNames = LoadPropertyScopes(PropDefs);
			if (scopeNames.Count == 0) { return; }

			foreach (string s in scopeNames)
			{
				Scopes.Items.Add(s);
			}

			Scopes.SelectedIndex = 0;
		}
		
		// Calculates List of Property Scopes
      private StringCollection LoadPropertyScopes(DataTable PropDefs)
      {
         StringCollection scopeNames = new StringCollection();

         foreach (DataRow row in PropDefs.Rows)
         {
            string scopeName = "";
            string propertyName = row["name"].ToString();
            string[] propertyParts = propertyName.Split(new char[] { '.' });

            if (propertyParts.Length == 2)
            {
               scopeName = propertyParts[0];
            }
            else if (propertyParts.Length >= 3)
            {
               scopeName = string.Format("{0}.{1}", propertyParts[0], propertyParts[1]);
            }

				row["scope"] = scopeName;

            if (scopeNames.IndexOf(scopeName) < 0)
            {
               scopeNames.Add(scopeName);
            }
         }
         return (scopeNames);
      }

		#endregion

		private void OnSelectedTab(object sender, TabControlEventArgs e)
		{
			if (this.tabControl.SelectedIndex == 0)
			{
				this.statusStrip.Items[0].Text = string.Format("Number of columns: {0}", this.lvProperties.Items.Count);
			}
			else if (tabControl.SelectedIndex == 1)
			{
				this.statusStrip.Items[0].Text = string.Format("Number of items: {0}", this.dataGridView1.Rows.Count);
			}
		}
   }
}