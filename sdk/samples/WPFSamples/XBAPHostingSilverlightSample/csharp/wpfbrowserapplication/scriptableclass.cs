using System.ComponentModel; // INotifyPropertyChanged
using System.Windows; // MessageBox
using System.Runtime.InteropServices; // ComVisibleAttribute

namespace WPFBrowserApplication
{
    [ComVisible(true)]
    public class ScriptableClass
    {
        // Method that can be called from HTML script (see HTMLDocumentWithScript.html)
        public void DisplayMessage(string msg)
        {
            MessageBox.Show("Your WPF application has received a message: " + msg);
        }
    }
}
