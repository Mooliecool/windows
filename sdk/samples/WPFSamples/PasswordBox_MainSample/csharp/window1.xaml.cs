using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Shapes;

namespace SDKSample
{

    public partial class Window1 : Window
    {
        int pwChanges = 0;

        public Window1()
        {
            InitializeComponent();
        }

        void WindowLoaded (Object sender, RoutedEventArgs args)
        {
            // Set the starting value of the masking char combobox to the 
            // default masking character for the passwordbox.            
            listMaskChar.Text = pwdBox.PasswordChar.ToString();

            for (int x = 6; x <= 256; x++)
                selectMaxLen.Items.Add(x.ToString());
            selectMaxLen.SelectedIndex = 0;
        }

        void NewMaskChar(Object sender, RoutedEventArgs args)
        {
            pwdBox.PasswordChar = ((ComboBoxItem)listMaskChar.SelectedItem).Content.ToString().ToCharArray()[0];
        }

        void PasswordChanged(Object sender, RoutedEventArgs args)
        {
            pwChangesLabel.Content = ++pwChanges;
        }

        void CopyContents(Object sender, RoutedEventArgs args)
        {
            scratchTextBox.SelectAll();
            scratchTextBox.Copy();
        }

        void PwbClear(Object sender, RoutedEventArgs args) { pwdBox.Clear(); }
        void PwbPaste(Object sender, RoutedEventArgs args) { pwdBox.Paste(); }

        void MaxSelected(Object sender, RoutedEventArgs args)
        {
            if (selectMaxLen.SelectedIndex == 0)
            {
                pwdBox.MaxLength = 0;
                currentMaxLen.Content = "Unlimited";
            }
            else
                currentMaxLen.Content = pwdBox.MaxLength = selectMaxLen.SelectedIndex + 5;
        }
    }
}