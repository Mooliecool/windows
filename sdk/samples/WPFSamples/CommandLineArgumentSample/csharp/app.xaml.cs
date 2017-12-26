//<SnippetCommandLineArgumentCODE>
namespace CommandLineArgumentSample
{
    using System;
    using System.Collections;
    using System.Text.RegularExpressions;
    using System.Windows;

    public partial class App : Application
    {
        // Indexed command line args using hash table
        public static Hashtable CommandLineArgs = new Hashtable();

        void app_Startup(object sender, StartupEventArgs e)
        {
            // Don't bother if no command line args were passed
            // NOTE: e.Args is never null - if no command line args were passed, 
            //       the length of e.Args is 0.
            if (e.Args.Length == 0) return;

            // Parse command line args for args in the following format:
            //   /argname:argvalue /argname:argvalue /argname:argvalue ...
            //
            // Note: This sample uses regular expressions to parse the command line arguments.
            // For regular expressions, see:
            // http://msdn.microsoft.com/library/en-us/cpgenref/html/cpconRegularExpressionsLanguageElements.asp
            string pattern = @"(?<argname>/\w+):(?<argvalue>\w+)";
            foreach (string arg in e.Args)
            {
                Match match = Regex.Match(arg, pattern);

                // If match not found, command line args are improperly formed.
                if (!match.Success) throw new ArgumentException("The command line arguments are improperly formed. Use /argname:argvalue.");

                // Store command line arg and value
                CommandLineArgs[match.Groups["argname"].Value] = match.Groups["argvalue"].Value;
            }
        }
    }
}
//</SnippetCommandLineArgumentCODE>