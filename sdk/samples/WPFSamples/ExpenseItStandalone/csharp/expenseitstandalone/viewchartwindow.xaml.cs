using System.Windows; // Window

namespace ExpenseItStandalone
{
    public partial class ViewChartWindow : Window
    {
        public ViewChartWindow()
        {
            InitializeComponent();
        }

        void closeButton_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}