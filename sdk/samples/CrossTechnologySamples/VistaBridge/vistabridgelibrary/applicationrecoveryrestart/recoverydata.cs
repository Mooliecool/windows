using System;
using System.Runtime.InteropServices;


namespace Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart
{
    /// <summary>
    /// The <see cref="System.Delegate"/> that represents the callback method invoked
    /// by the system when an application has registered for 
    /// application recovery. 
    /// </summary>
    /// <param name="parameter">An application defined object that is 
    /// passed to the callback method.</param>
    /// <remarks>The callback method will be invoked
    /// prior to the application being terminated by Windows Error Reporting (WER). To keep WER from terminating the application before 
    /// the callback method completes, the callback method must
    /// periodically call the <see cref="Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.ArrManager.ApplicationRecoveryInProgress"/> method. </remarks>
    /// <seealso cref="Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.ArrManager.RegisterForApplicationRecovery(Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.RecoverySettings)"/>
    public delegate int RecoveryCallback(RecoveryData parameter);

    /// <summary>
    /// An example class that is used by the ARR Demo to return
    /// application specific information to the recovery callback method. 
    /// Applications should modify this class to fit their 
    /// recovery data requirements.
    /// </summary>
    [StructLayout(LayoutKind.Auto, CharSet = CharSet.Unicode)]
    public class RecoveryData
    {
        string currentUser;


        /// <summary>
        /// Creates a new instance of this class.
        /// </summary>
        /// <param name="who">Identifies the user session that is to be recovered.</param>
        public RecoveryData(string who)
        {
            currentUser = who;
        }
        /// <summary>
        /// Gets the current user specified when the object was created.
        /// </summary>
        /// <value>A <see cref="System.String"/> object.</value>
        public string CurrentUser
        {
            get { return currentUser; }
        }
        /// <summary>
        /// Returns a string representation for this object.
        /// </summary>
        /// <returns>A <see cref="System.String"/> representing the current state of this object.</returns>
        public override string ToString()
        {
            if (String.IsNullOrEmpty(currentUser))
                return base.ToString();
            else
                return currentUser;
        }
    }
}
