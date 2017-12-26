using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Data;
using Microsoft.Windows.Controls;

namespace CSWPFCascadeDataGridComboBoxColumns
{  

    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        public enum Country
        {
            China,
            UnitedStates
        }
        public enum ChinaCity
        {
            Beijing,
            Shanghai
        }
        public enum UnitedStatesCity
        {
            NewYork,
            Washington
        }

        DataTable table = null;
        string[] strChinaCities, strUnitedStateCities;

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            /////////////////////////////////////////////////////////////////
            // get all enumeration values of type enum Country
            //
            Array countries = Enum.GetValues(typeof(Country));

            /////////////////////////////////////////////////////////////////
            // copy all Country enumeration values to a string array
            //
            string[] strCountries = new string[countries.Length];
            for (int i = 0; i < countries.Length; i++)
            {
                strCountries[i] = (countries as Country[])[i].ToString();
            }

            /////////////////////////////////////////////////////////////////
            // get all enumeration values of type enum ChinaCity
            //
            Array chinaCities = Enum.GetValues(typeof(ChinaCity));

            /////////////////////////////////////////////////////////////////
            // copy all ChinaCity enumeration values to a string array
            //
            strChinaCities = new string[chinaCities.Length];
            for (int i = 0; i < chinaCities.Length; i++)
            {
                strChinaCities[i] = (chinaCities as ChinaCity[])[i].ToString();
            }

            /////////////////////////////////////////////////////////////////
            // get all enumeration values of type enum UnitedStatesCity
            //
            Array unitedStateCities = Enum.GetValues(typeof(UnitedStatesCity));

            /////////////////////////////////////////////////////////////////
            //copy all UnitedStateCity enumeration values to a string array
            //
            strUnitedStateCities = new string[unitedStateCities.Length];
            for (int i = 0; i < unitedStateCities.Length; i++)
            {
                strUnitedStateCities[i] = (unitedStateCities as UnitedStatesCity[])[i].ToString();
            }

            //////////////////////////////////////////////////////////////////
            // combine both the two city enumeration value into one string array
            //
            string[] strAllCities = new string[strChinaCities.Length + strUnitedStateCities.Length];
            strChinaCities.CopyTo(strAllCities, 0);
            strUnitedStateCities.CopyTo(strAllCities, strChinaCities.Length);

            ///////////////////////////////////////////////////////////////////////////////
            // data bind the two DataGridComboBoxColumn's ItemsSource property respectively
            //
            BindingOperations.SetBinding(this.column1, DataGridComboBoxColumn.ItemsSourceProperty,
                new Binding() { Source = strCountries });
            BindingOperations.SetBinding(this.column2, DataGridComboBoxColumn.ItemsSourceProperty,
                new Binding() { Source = strAllCities });

            /////////////////////////////////////////////////////////////////
            // create a DataTable and add two DataColumn into it
            //
            table = new DataTable();
            table.Columns.Add("Country");
            table.Columns.Add("City");

            /////////////////////////////////////////////////////////////////
            // add a DataRow into this DataTable
            //
            table.Rows.Add(new object[] { "China", "Beijing" });

            /////////////////////////////////////////////////////////////////
            // set the DataContext property of the DataGrid to the DataTable
            //
            this.dataGrid.DataContext = table;

            /////////////////////////////////////////////////////////////////
            // set the Header of both DataGridComboBoxColumn and bind the
            // SelectedItemBinding property of both DataGridComboBoxColumn
            this.column1.Header = "Country";
            this.column1.SelectedItemBinding = new Binding("Country");
            this.column2.Header = "City";
            this.column2.SelectedItemBinding = new Binding("City");

        }

        /// <summary>
        /// this PreparingCellForEdit event handler gets the hosted editing ComboBox control 
        /// and bind its ItemsSource property according to the value of the Country
        /// </summary>             
        private void datagrid_PreparingCellForEdit(object sender, DataGridPreparingCellForEditEventArgs e)
        {
            if (e.Column.Header.Equals("City"))
            {
                ComboBox cboEditingElement = e.EditingElement as ComboBox;
                if ((e.Row.Item as DataRowView)["Country"].Equals("China"))
                {
                    //////////////////////////////////////////////////////////////////////////
                    // bind the ItemsSource property of the cmbEditingElement to China city
                    // string array if the selected country is China
                    //
                    BindingOperations.SetBinding(cboEditingElement, ComboBox.ItemsSourceProperty,
                        new Binding() { Source = strChinaCities });
                }
                else
                {
                    //////////////////////////////////////////////////////////////////////////
                    // bind the ItemsSource property of the cmbEditingElement to United State
                    // city string array if the selected country is United State
                    //
                    BindingOperations.SetBinding(cboEditingElement, ComboBox.ItemsSourceProperty,
                        new Binding() { Source = strUnitedStateCities });
                }
            }
        }
    }
}
