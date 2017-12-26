using System.Windows; // Window


namespace ExpenseItStandalone
{
    public partial class CreateExpenseReportDialogBox : Window
    {
        public CreateExpenseReportDialogBox()
        {
            InitializeComponent();
        }

        void addExpenseButton_Click(object sender, RoutedEventArgs e)
        {
            Application app = (Application)System.Windows.Application.Current;
            ExpenseReport expenseReport = (ExpenseReport)app.FindResource("ExpenseData");
            expenseReport.LineItems.Add(new LineItem());
        }

        void viewChartButton_Click(object sender, RoutedEventArgs e)
        {
            ViewChartWindow dlg = new ViewChartWindow();
            dlg.Owner = this;
            dlg.Show();
        }

        void okButton_Click(object sender, RoutedEventArgs e)
        {
            MessageBox.Show(
                "Expense Report Created!",
                "ExpenseIt Standalone",
                MessageBoxButton.OK,
                MessageBoxImage.Information);

            this.DialogResult = true;
        }

        void cancelButton_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = false;
        }
    }
}