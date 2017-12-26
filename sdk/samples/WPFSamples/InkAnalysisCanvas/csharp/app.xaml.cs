using System;
using System.Windows;
using System.Reflection;

namespace InkAnalysisCanvasSample
{
    public partial class App : System.Windows.Application
    {
        /// <summary>
        /// The Ink Analysis assemblies are installed by the Windows SDK, as reference assemblies, 
        /// but they're not necessarily available at runtime -- display a friendly error message 
        /// if any dependencies are missing.  True Ink Analysis applications (not SDK samples) 
        /// must redistribute the IA runtime, and so needn't bother with this.
        /// </summary>
        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            AppDomain.CurrentDomain.AssemblyResolve += new ResolveEventHandler(CurrentDomain_AssemblyResolve);
        }

        Assembly CurrentDomain_AssemblyResolve(object sender, ResolveEventArgs args)
        {
            string messageText = "This sample application requires the following assembly:\n" + args.Name + 
                "\n\nEither install the assembly in the GAC, or reference it locally.";

            MessageBox.Show(messageText, "Assembly Not Found", 
                MessageBoxButton.OK, MessageBoxImage.Error);

            return null; // let assembly load fail with fnf exception
        }
    }
}