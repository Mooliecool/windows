using System;
using System.Diagnostics;
using System.ComponentModel;
using System.Runtime.InteropServices;
using Microsoft.SDK.Samples.VistaBridge.Interop;

namespace Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart
{
    /// <summary>
    /// Provides access to the Application Restart and Recovery
    /// features available in Windows Vista.
    /// </summary>
    public static class ArrManager
    {
        /// <summary>
        /// Registers an application for recovery.
        /// </summary>
        /// <param name="settings">An object that specifies
        /// the callback method, an optional parameter to pass to the callback
        /// method and a time interval (see remarks).</param>
        /// <exception cref="System.ArgumentException">
        /// The registration failed due to an invalid parameter.
        /// </exception>
        /// <exception cref="System.ComponentModel.Win32Exception">
        /// The registration failed.</exception>
        /// <remarks>The time interval indicates the time within 
        /// which the recovery callback method 
        /// calls the <see cref="Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.ArrManager.ApplicationRecoveryInProgress"/> method to indicate
        /// that it is still performing recovery work.</remarks>
        public static void RegisterForApplicationRecovery(
            RecoverySettings settings)
        {
            RegisterForApplicationRecovery(
                settings.RecoveryCallback,
                settings.ParameterValue,
                settings.PingInterval);
        }

        private static void RegisterForApplicationRecovery(
            RecoveryCallback recoveryCallback,
            RecoveryData parameter,
            uint pingInterval)
        {
            uint returnValue = NativeMethods.RegisterApplicationRecoveryCallback(
                recoveryCallback,
                parameter,
                pingInterval,
                (uint)0);

            if (returnValue == SafeNativeMethods.ResultFalse)
            {
                throw new Win32Exception(
                    "Application failed to register for recovery.");
            }
            if (returnValue == SafeNativeMethods.ResultInvalidArgument)
            {
                throw new ArgumentException(
                    "Application was not registered for recovery due to bad parameters.");
            }
        }
        /// <summary>
        /// Removes an application's recovery registration.
        /// </summary>
        /// <exception cref="System.ComponentModel.Win32Exception">
        /// The attempt to unregister for recovery failed.</exception>
        public static void UnregisterApplicationRecovery()
        {
            uint returnValue = NativeMethods.UnregisterApplicationRecoveryCallback();

            if (returnValue == SafeNativeMethods.ResultFailed)
            {
                throw new Win32Exception("Unregister for recovery failed.");
            }
        }
        /// <summary>
        /// Removes an application's restart registration.
        /// </summary>
        /// <exception cref="System.ComponentModel.Win32Exception">
        /// The attempt to unregister for restart failed.</exception>
        public static void UnregisterApplicationRestart()
        {
            uint returnValue = NativeMethods.UnregisterApplicationRestart();

            if (returnValue == SafeNativeMethods.ResultFailed)
            {
                throw new Win32Exception("Unregister for restart failed.");
            }
        }
        /// <summary>
        /// Returns the current settings for application recovery.
        /// </summary>
        /// <param name="processHandle">A handle to the application's
        /// process.</param>
        /// <returns>A <see cref="Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.RecoverySettings"/> object that specifies
        /// the callback method, an optional parameter to pass 
        /// to the callback method and the time interval
        /// within which the callback method
        /// calls the <see cref="Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.ArrManager.ApplicationRecoveryInProgress"/> method to indicate
        /// that it is still performing recovery work.</returns>
        /// <exception cref="System.ArgumentException">Cannot get the settings due to an invalid parameter.</exception>
        /// <exception cref="System.InvalidOperationException">The application is not registered for recovery.</exception>
        public static RecoverySettings ApplicationRecoverySettings(
            IntPtr processHandle)
        {
            RecoveryData parameter = null;
            RecoveryCallback recoveryCallback;
            uint pingInterval;

            ApplicationRecoverySettings(
                processHandle,
            out recoveryCallback,
            out parameter,
            out pingInterval);

            RecoverySettings settings = new RecoverySettings(
                recoveryCallback, parameter, pingInterval);
            return settings;
        }

        private static void ApplicationRecoverySettings(
            IntPtr processHandle,
            out RecoveryCallback recoveryCallback,
            out RecoveryData parameter,
            out uint pingInterval)
        {
            uint flags = 0;
            uint returnValue;

            returnValue = NativeMethods.GetApplicationRecoveryCallback(
            processHandle,
            out recoveryCallback,
            out parameter,
            out pingInterval,
            out flags);

            if (returnValue == SafeNativeMethods.ResultFalse || recoveryCallback == null)
            {
                throw new InvalidOperationException(
                    "Application is not registered for recovery.");
            }
            if (returnValue == SafeNativeMethods.ResultInvalidArgument)
            {
                throw new ArgumentException(
                    "Failed to get application recovery settings due to bad parameters.");
            }
        }
        /// <summary>
        /// Called by an application's <see cref="Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.RecoveryCallback"/> method 
        /// to indicate that it is still performing recovery work.
        /// </summary>
        /// <returns>A <see cref="System.Boolean"/> value indicating whether the user
        /// canceled the recovery.</returns>
        /// <exception cref="System.InvalidOperationException">
        /// This method must be called from a registered callback method.</exception>
        public static bool ApplicationRecoveryInProgress()
        {
            bool canceled;

            uint retval = NativeMethods.ApplicationRecoveryInProgress(
                out canceled);

            if (retval == SafeNativeMethods.ResultFailed)
            {
                throw new InvalidOperationException(
                    "This method must be called from the registered callback method.");
            }

            return canceled;
        }

        /// <summary>
        /// Called by an application's <see cref="Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.RecoveryCallback"/> method to 
        /// indicate that the recovery work is complete. 
        /// </summary>
        /// <remarks>
        /// This should
        /// be the last call made by the <see cref="Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.RecoveryCallback"/> method because
        /// Windows Error Reporting will terminate the application
        /// after this method is invoked.
        /// </remarks>
        /// <param name="success">Specifies
        /// whether the program was able to complete its recovery
        /// work before terminating.</param>
        public static void ApplicationRecoveryFinished(bool success)
        {
            NativeMethods.ApplicationRecoveryFinished(success);
        }

        /// <summary>
        /// Returns the current settings for application restart.
        /// </summary>
        /// <param name="processHandle">A handle to the application's
        /// process.</param>
        /// <returns>An <see cref="Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart.RestartSettings"/> object that specifies
        /// the commandline arguments used to restart the 
        /// application, and 
        /// the conditions when the application should not be 
        /// restarted.</returns>
        /// <exception cref="System.ArgumentException">Cannot get the settings due to an invalid parameter.</exception>
        /// <exception cref="System.InvalidOperationException">The application is not registered for restart.</exception>
        public static RestartSettings ApplicationRestartSettings(
            IntPtr processHandle)
        {
            RestartRestrictions restart;

            string command = ApplicationRestartSettings(
                processHandle,
                out  restart);

            return new RestartSettings(command, restart);
        }

        private static string ApplicationRestartSettings(
            IntPtr processHandle,
            out RestartRestrictions restart)
        {
            IntPtr cmdptr = IntPtr.Zero;
            uint size = 0;

            // Find out how big a buffer to allocate
            // for the command line.
            uint returnValue = NativeMethods.GetApplicationRestartSettings(
                processHandle,
                IntPtr.Zero,
                ref size,
                out restart);

            // Allocate a string buffer.
            cmdptr = Marshal.AllocHGlobal((int)size * sizeof(char));

            // Get the settings using the buffer.
            returnValue = NativeMethods.GetApplicationRestartSettings(
                processHandle,
                cmdptr,
                ref size,
                out restart);

            if (returnValue == SafeNativeMethods.ResultNotFound)
            {
                throw new InvalidOperationException(
                    "Application is not registered for restart.");
            }
            if (returnValue == SafeNativeMethods.ResultInvalidArgument)
            {
                throw new ArgumentException(
                    "Failed to get application restart settings due to bad parameters.");
            }
            // Read the buffer's contents as a unicode string.
            string cmd = Marshal.PtrToStringUni(cmdptr);
            // Free the buffer.
            Marshal.FreeHGlobal(cmdptr);
            return cmd;
        }

        /// <summary>
        /// Registers an application for automatic restart if 
        /// the application 
        /// is terminated by Windows Error Reporting.
        /// </summary>
        /// <param name="settings">An object that specifies
        /// the commandline arguments used to restart the 
        /// application, and 
        /// the conditions wheen the application should not be 
        /// restarted.</param>
        /// <exception cref="System.ArgumentException">Registration failed due to an invalid parameter.</exception>
        /// <exception cref="System.InvalidOperationException">The attempt to register failed.</exception>
        /// <remarks>A registered application will not be restarted if it executed for less than 60 seconds before terminating.</remarks>
        public static void RegisterForApplicationRestart(RestartSettings settings)
        {
            RegisterForApplicationRestart(settings.Command, settings.Restrictions);
        }

        private static void RegisterForApplicationRestart(
            string command,
            RestartRestrictions restrictions)
        {
            uint returnValue = NativeMethods.RegisterApplicationRestart(
                command,
                restrictions);

            if (returnValue == SafeNativeMethods.ResultFailed)
            {
                throw new InvalidOperationException(
                    "Application failed to registered for restart.");
            }
            if (returnValue == SafeNativeMethods.ResultInvalidArgument)
            {
                throw new ArgumentException(
                    "Failed to register application for restart due to bad parameters.");
            }
        }
    }
}

