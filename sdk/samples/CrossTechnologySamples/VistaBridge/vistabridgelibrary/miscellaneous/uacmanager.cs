using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using Microsoft.SDK.Samples.VistaBridge.Interop;
using System.Runtime.InteropServices;
using System.Security.Permissions;

namespace Microsoft.SDK.Samples.VistaBridge.Services
{
    /// <summary>
    /// Provides support for the UAC sample. 
    /// </summary>
    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public static class UACManager
    {
        /// <summary>
        /// Executes the specified process.
        /// </summary>
        /// <param name="executablePath">The process to execute.</param>
        /// <returns>The exit code for the executed process.</returns>
        public static int ExecutePrivilegedProcess(string executablePath)
        {
            int exitCode = -1;

            if (String.IsNullOrEmpty(executablePath))
                throw new ArgumentNullException("executablePath","Executable file name must be specified.");

            using (Process process = CreateDefaultProcess(executablePath))
            {
                if (!process.Start())
                    throw new InvalidOperationException("Couldn't start process '" + executablePath + "'.");

                // Synchronously block until process is complete, 
                // then return exit code from process.
                process.WaitForExit();
                exitCode = process.ExitCode;
            }
            return exitCode;
        }

        /// <summary>
        /// Executes the specified process asynchronously.
        /// </summary>
        /// <param name="executablePath">The process to execute.</param>
        /// <param name="exitedEventHandler">The event handler that receives notification when the process exits.</param>
        public static void ExecutePrivilegedProcessAsync(string executablePath, EventHandler exitedEventHandler)
        {
            if (String.IsNullOrEmpty(executablePath))
                throw new ArgumentNullException(
                    "executablePath",
                    "Executable file name must be specified.");

            using (Process process = CreateDefaultProcess(executablePath))
            {
                if (exitedEventHandler != null)
                    process.Exited += exitedEventHandler;
                if (!process.Start())
                    throw new InvalidOperationException(
                        "Couldn't start process '" + executablePath + "'."); 
            }
            return;
        }

        private static Process CreateDefaultProcess(string executablePath)
        {
            Process process = new Process();
            process.StartInfo = new ProcessStartInfo();
            process.StartInfo.FileName = executablePath;
            process.StartInfo.CreateNoWindow = true;
            process.StartInfo.UseShellExecute = true;

            // Must enable Exited event for both sync and async scenarios.
            process.EnableRaisingEvents = true;
            return process;
        }

        [return: MarshalAs(UnmanagedType.Interface)]
        static internal object LaunchElevatedCOMObject(Guid Clsid, Guid InterfaceID)
        {
            // B formatting directive: returns 
            // {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}.
            string CLSID = Clsid.ToString("B");  
            string monikerName = "Elevation:Administrator!new:" + CLSID;

            SafeNativeMethods.BIND_OPTS3 bo = new SafeNativeMethods.BIND_OPTS3();
            bo.cbStruct = (uint)Marshal.SizeOf(bo);
            bo.hwnd = IntPtr.Zero;
            bo.dwClassContext = (int)SafeNativeMethods.CLSCTX.CLSCTX_ALL;

            object retVal = UnsafeNativeMethods.CoGetObject(
                monikerName, 
                ref bo, 
                InterfaceID);

            return (retVal);
        }
    }
}
