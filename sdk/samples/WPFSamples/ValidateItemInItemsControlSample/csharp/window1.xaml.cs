using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;

namespace ValidateItemInItemsControlSample
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window
    {
        Customers customerData;
        BindingGroup bindingGroupInError = null;

        public Window1()
        {
            InitializeComponent();

            customerData = new Customers();
            customerList.DataContext = customerData;

        }

        void AddCustomer_Click(object sender, RoutedEventArgs e)
        {

            if (bindingGroupInError == null)
            {
                customerData.Add(new Customer());
            }
            else
            {
                MessageBox.Show("Please correct the data in error before adding a new customer.");
            }
        }

        void saveCustomer_Click(object sender, RoutedEventArgs e)
        {
            Button btn = sender as Button;
            FrameworkElement container = (FrameworkElement) customerList.ContainerFromElement(btn);

            // If the user is trying to change an items, when another item has an error,
            // display a message and cancel the currently edited item.
            if (bindingGroupInError != null && bindingGroupInError != container.BindingGroup)
            {
                MessageBox.Show("Please correct the data in error before changing another customer");
                container.BindingGroup.CancelEdit();
                return;
            }

            if (container.BindingGroup.ValidateWithoutUpdate())
            {
                container.BindingGroup.UpdateSources();
                bindingGroupInError = null;
                MessageBox.Show("Item Saved");
            }
            else
            {
                bindingGroupInError = container.BindingGroup;
            }


            
        }
    }
}
