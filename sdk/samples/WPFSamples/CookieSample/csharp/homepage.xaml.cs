using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interop;
using System.Windows.Navigation;

namespace CookiesSampleCSharp
{
    public partial class HomePage : System.Windows.Controls.Page
    {
        public HomePage()
        {
            InitializeComponent();
        }

        void setCookieButton_Click(object sender, RoutedEventArgs e)
        {
            // * Cookie Without Expiration
            // Format: NAME=VALUE
            // Persistence: In-memory only (for same process that created it)
            // Example: "name1=value1"

            // * Cookie With Expiration
            // Format: NAME=VALUE; expires=DAY, DD-MMM-YYYY HH:MM:SS GMT
            // Persistence: Retained on disk, until expiration,
            //              in current Windows version's temporary internet files location
            // Example: "name1=value1; expires=Sat, 01-Jan-2063 00:00:00 GMT"

            try
            {
                // Set cookie
                Application.SetCookie(BrowserInteropHelper.Source, this.setCookieValueTextBox.Text);
            }
            catch (Win32Exception ex)
            {
                // Error creating cookie
                MessageBox.Show(ex.Message + " (Native Error Code=" + ex.NativeErrorCode + ")");
            }
        }

        void getCookieButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                // Retrieve cookie
                this.getCookieValueTextBox.Text = Application.GetCookie(BrowserInteropHelper.Source);
            }
            catch (Win32Exception ex)
            {
                // Error retrieving cookie
                MessageBox.Show(ex.Message + " (Native Error Code=" + ex.NativeErrorCode + ")");
            }
        }
    }
}