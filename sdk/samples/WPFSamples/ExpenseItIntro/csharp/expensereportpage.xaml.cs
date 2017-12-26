using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
namespace ExpenseIt
{
    public partial class ExpenseReportPage : Page
    {
        public ExpenseReportPage(object data)
        {
            InitializeComponent();

            // Bind to expense report data
            this.DataContext = data;
        }
    }
}