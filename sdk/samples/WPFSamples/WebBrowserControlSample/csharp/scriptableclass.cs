using System.Windows; // Window
using System.Runtime.InteropServices; // ComVisibleAttribute

namespace WebBrowserControlSample
{
    [ComVisible(true)]
    public class ScriptableClass
    {
        private Window host;

        public ScriptableClass(Window host)
        {
            this.host = host;
        }

        #region Properties that can be set from HTML script (see HTMLDocumentWithScript.html)
        public string Title
        {
            get { return this.host.Title; }
            set { this.host.Title = value; }
        }

        public double Width
        {
            get { return this.host.ActualWidth; }
            set { this.host.Width = value; }
        }
        public double Height
        {
            get { return this.host.ActualHeight; }
            set { this.host.Height = value; }
        }
        #endregion

        #region Method that can be called from HTML script (see HTMLDocumentWithScript.html)
        public void SetAllProperties(string title, double width, double height)
        {
            Title = title;
            Width = width;
            Height = height;
        }
        #endregion
    }
}
