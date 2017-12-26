using System;
using System.Collections;
using System.Windows;

// To get and parse the URI query string parameters
using System.Deployment.Application;
using System.Text.RegularExpressions;

namespace URIParamSample
{
    public partial class App : Application
    {
        // Store the URI parameters, indexed by URI parameter name
        public Hashtable UriParameters = new Hashtable();
        
        public App()
        {
            InitializeComponent();
        }

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            ((App)Application.Current).UriParameters = new Hashtable();

            // If this application has been launched using ClickOnce eg from a browser
            if (ApplicationDeployment.IsNetworkDeployed)
            {
                if (ApplicationDeployment.CurrentDeployment.ActivationUri == null) return;

                // Retrieve the URI that was used to launch this application, 
                // including URI query string parameters
                string uri = ApplicationDeployment.CurrentDeployment.ActivationUri.ToString();
                
                // Extract and store each query string parameter using regex
                MatchCollection matches = Regex.Matches(uri, @"(\w*)=(\w*)", RegexOptions.IgnoreCase);
                foreach (Match match in matches)
                {
                    ((App)Application.Current).UriParameters[match.Groups[1].Value] = match.Groups[2].Value;
                }
            }
        }
    }
}