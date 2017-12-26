namespace URIParamSample
{
    using System;
    using System.Collections;
    using System.Windows;
    using System.Windows.Controls;
    
    public partial class HomePage : Page
    {
        public HomePage()
        {
            InitializeComponent();

            // Retrieve and display each URI query string parameter that was provided
            Hashtable uriParameters = ((App)Application.Current).UriParameters;
            this.Content = "URI Parameters:\n";
            foreach(string key in uriParameters.Keys ) {
              this.Content += key + "=" + uriParameters[key] + ";";
            }
        }

    }
}