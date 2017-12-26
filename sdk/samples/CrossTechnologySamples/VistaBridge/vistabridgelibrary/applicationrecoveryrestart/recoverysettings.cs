using System;

namespace Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart
{
    /// <summary>
    /// Specifies options for an application that attempts
    /// to perform final actions after a fatal event such as an
    /// unhandled exception.
    /// </summary>
    /// <remarks>This class is used to register for application recovery.
    /// See the <see cref="Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.ArrManager"/> class.
    /// </remarks>
    /// 
    public class RecoverySettings
    {
        RecoveryCallback recoveryCallback;
        RecoveryData parameter;
        uint pingInterval;

        /// <summary>
        /// Creates a new instance of this class.
        /// </summary>
        /// <param name="callbackDelegate">The callback method that will be invoked by 
        /// the system before Windows Error Reporting (WER) terminates the 
        /// application.</param>
        /// <param name="theParameter">An optional argument for the callback method. </param>
        /// <param name="thePingInterval">The time interval within which the 
        /// callback method must invoke <see cref="Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.ArrManager.ApplicationRecoveryInProgress"/> to 
        /// prevent WER from terminating the application.</param>
        /// <seealso cref="Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.ArrManager"/>
        public RecoverySettings(
            RecoveryCallback callbackDelegate,
            RecoveryData theParameter,
            uint thePingInterval)
        {
            this.recoveryCallback = callbackDelegate;
            this.parameter = theParameter;
            this.pingInterval = thePingInterval;
        }
        /// <summary>
        /// Gets the callback method that will be invoked by 
        /// the system before Windows Error Reporting (WER) 
        /// terminates the application.
        /// </summary>
        /// <value>A <see cref="Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.RecoveryCallback"/> delegate.</value>
        /// <seealso cref="Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.ArrManager"/>
        public RecoveryCallback RecoveryCallback
        {
            get { return recoveryCallback; }
        }

        /// <summary>
        /// Gets an optional argument for the <see cref="Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.RecoveryCallback"/> method.
        /// </summary>
        /// <value>A <see cref="Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.RecoveryData"/> object.</value>
        public RecoveryData ParameterValue
        {
            get { return parameter; }
        }

        /// <summary>
        /// Gets the time interval within which the 
        /// <see cref="Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.RecoveryCallback"/> method must invoke <see cref="Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.ArrManager.ApplicationRecoveryInProgress"/> to 
        /// prevent WER from terminating the application.
        /// </summary>
        /// <remarks>        
        /// The recovery ping interval is in milliseconds. 
        /// By default, the interval is 5 seconds. 
        /// If you specify zero, the default interval is used. 
        /// </remarks>
        public uint PingInterval
        {
            get { return pingInterval; }
        }
        /// <summary>
        /// Returns a string representation of the current state
        /// of this object.
        /// </summary>
        /// <returns>A <see cref="System.String"/> object.</returns>
        public override string ToString()
        {
            return String.Format("delegate: {0}, parameter: {1}, ping: {2}",
                this.RecoveryCallback.Method.ToString(),
                this.ParameterValue.ToString(),
                this.PingInterval);
        }
    }
}

