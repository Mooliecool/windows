using System;
using System.Windows;
using System.Windows.Controls;

namespace SDKSample
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            // Add choices to combo box
            this.skinComboBox.Items.Add("Blue");
            this.skinComboBox.Items.Add("Yellow");
            this.skinComboBox.SelectedIndex = 0;

            // Set initial skin
            App.Current.Resources = (ResourceDictionary)App.Current.Properties["Blue"];

            // Detect when skin changes
            this.skinComboBox.SelectionChanged += new SelectionChangedEventHandler(skinComboBox_SelectionChanged);
        }

        void newChildWindowButton_Click(object sender, RoutedEventArgs e)
        {
            // Create a new skind child window
            ChildWindow window = new ChildWindow();
            window.Show();
        }

        void skinComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // Change the skin
            string selectedValue = (string)e.AddedItems[0];
            App.Current.Resources = (ResourceDictionary)App.Current.Properties[selectedValue];
        }
    }
}