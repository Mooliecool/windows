using System; // EventArgs, EventHandler
using System.Globalization; // CultureInfo
using System.Windows; // Window, RoutedEventArgs
using System.Windows.Controls; // SelectionChangedEventHandler, ListBoxItem
using System.Windows.Data; // XmlDataProvider
using System.Windows.Input; // RoutedUICommand, KeyGesture
using System.Windows.Media.Imaging; // BitmapImage

namespace ExpenseItStandalone
{
    public partial class MainWindow : Window
    {
        #region Commands
        public static RoutedUICommand CreateExpenseReportCommand;
        public static RoutedUICommand ExitCommand;
        public static RoutedUICommand AboutCommand;
        #endregion

        static MainWindow()
        {
            // Define CreateExpenseReportCommand
            CreateExpenseReportCommand = new RoutedUICommand("_Create Expense Report...", "CreateExpenseReport", typeof(MainWindow));
            CreateExpenseReportCommand.InputGestures.Add(new KeyGesture(Key.C, ModifierKeys.Control | ModifierKeys.Shift));

            // Define ExitCommand
            ExitCommand = new RoutedUICommand("E_xit", "Exit", typeof(MainWindow));

            // Define AboutCommand
            AboutCommand = new RoutedUICommand("_About ExpenseIt Standalone", "About", typeof(MainWindow));
        }

        public MainWindow()
        {
            this.Initialized += new EventHandler(MainWindow_Initialized);

            InitializeComponent();

            this.employeeTypeRadioButtons.SelectionChanged += new SelectionChangedEventHandler(employeeTypeRadioButtons_SelectionChanged);

            // Bind CreateExpenseReportCommand
            CommandBinding commandBindingCreateExpenseReport = new CommandBinding(CreateExpenseReportCommand);
            commandBindingCreateExpenseReport.Executed += new ExecutedRoutedEventHandler(commandBindingCreateExpenseReport_Executed);
            this.CommandBindings.Add(commandBindingCreateExpenseReport);

            // Bind ExitCommand
            CommandBinding commandBindingExitCommand = new CommandBinding(ExitCommand);
            commandBindingExitCommand.Executed += new ExecutedRoutedEventHandler(commandBindingExitCommand_Executed);
            this.CommandBindings.Add(commandBindingExitCommand);

            // Bind AboutCommand
            CommandBinding commandBindingAboutCommand = new CommandBinding(AboutCommand);
            commandBindingAboutCommand.Executed += new ExecutedRoutedEventHandler(commandBindingAboutCommand_Executed);
            this.CommandBindings.Add(commandBindingAboutCommand);
        }

        void MainWindow_Initialized(object sender, EventArgs e)
        {
            // Select the first employee type radio button
            this.employeeTypeRadioButtons.SelectedIndex = 0;
            RefreshEmployeeList();
        }

        void commandBindingCreateExpenseReport_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            CreateExpenseReportDialogBox dlg = new CreateExpenseReportDialogBox();
            dlg.Owner = this;
            dlg.ShowDialog();
        }
        void commandBindingExitCommand_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            this.Close();
        }
        void commandBindingAboutCommand_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            MessageBox.Show(
                "ExpenseIt Standalone Sample Application, by the WPF SDK",
                "ExpenseIt Standalone",
                MessageBoxButton.OK,
                MessageBoxImage.Information);
        }

        void employeeTypeRadioButtons_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            RefreshEmployeeList();
        }

        /// <summary>
        /// Select the employees who have the employment type that is specified 
        /// by the currently checked employee type radio button
        /// </summary>
        void RefreshEmployeeList()
        {
            ListBoxItem selectedItem = (ListBoxItem)this.employeeTypeRadioButtons.SelectedItem;

            // Get employees data source
            XmlDataProvider employeesDataSrc = (XmlDataProvider)this.FindResource("Employees");

            // Select the employees who have of the specified employment type
            string query = string.Format(CultureInfo.InvariantCulture, "/Employees/Employee[@Type='{0}']", selectedItem.Content);
            employeesDataSrc.XPath = query;

            // Apply the selection
            employeesDataSrc.Refresh();
        }
    }
}