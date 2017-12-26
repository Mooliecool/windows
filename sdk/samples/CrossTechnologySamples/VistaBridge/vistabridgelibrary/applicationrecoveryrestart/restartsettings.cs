using System;

namespace Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart
{
    /// <summary>
    /// Specifies options for an application to be automatically
    /// restarted by Windows Error Reporting. 
    /// </summary>
    /// <remarks>Regardless of these 
    /// settings, the application
    /// will not be restarted if it executed for less than 60 seconds before
    /// terminating.</remarks>
    public class RestartSettings
    {
        string command;

        RestartRestrictions restrictions;

        /// <summary>
        /// Creates a new instance of the RestartSettings class.
        /// </summary>
        /// <param name="commandLine">The commandline arguments 
        /// used to restart the application.</param>
        /// <param name="restrict">One or more RestartRestrictions 
        /// values that specify the 
        /// conditions when the application should not be restarted.
        /// </param>
        public RestartSettings(string commandLine, RestartRestrictions restrict)
        {
            command = commandLine;
            restrictions = restrict;
        }
        /// <summary>
        /// Gets the commandline arguments used to restart the application.
        /// </summary>
        /// <value>A <see cref="System.String"/> object.</value>
        public string Command
        {
            get { return command; }
        }
        /// <summary>
        /// Gets the set of conditions when the application 
        /// should not be restarted.
        /// </summary>
        /// <value>A set of <see cref="Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.RestartRestrictions"/> values.</value>
        public RestartRestrictions Restrictions
        {
            get { return restrictions; }
        }
        /// <summary>
        /// Provide a representation of the current state
        /// of this object.
        /// </summary>
        /// <returns>A <see cref="System.String"/> that displays 
        /// the commandline arguments 
        /// and restrictions for restarting the application.</returns>
        public override string ToString()
        {
            return String.Format("command: {0} restrictions: {1}",
                command, restrictions.ToString());
        }
    }
}

