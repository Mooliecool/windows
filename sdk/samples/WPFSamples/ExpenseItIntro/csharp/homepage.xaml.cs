using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
namespace ExpenseIt
{
    public partial class HomePage : Page
    {
        public HomePage()
        {
            InitializeComponent();
        }

        private void viewButton_Click(object sender, RoutedEventArgs args)
        {
            // Create a new expense report page and pass it the selected person
            // by using the non-default constructor.
            ExpenseReportPage expenseReportPage = new ExpenseReportPage(this.peopleListBox.SelectedItem);

            // Navigate to the expense report page
            this.NavigationService.Navigate(expenseReportPage);
        }
    }
}