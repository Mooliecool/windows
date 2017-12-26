using System.Windows.Browser; // ScriptableMemberAttribute

namespace SilverlightApplication
{
    public class ScriptableClass
    {
        // Method that can be called from HTML script (see HTMLDocumentWithScript.html)
        [ScriptableMember]
        public void DisplayMessage(string msg)
        {
            msg = "Your Silverlight application has recieved a message: " + msg;
            ((Page)App.Current.RootVisual).msgTextBlock.Text = msg;
        }
    }
}
