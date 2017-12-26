//------------------------------------------------------------------------------
// <copyright file="Executor.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace System.CodeDom.Compiler {

    using System.Diagnostics;
    using System;
    using System.ComponentModel;
    using System.Text;
    using System.Threading;
    using System.IO;
    using System.Collections;
    using System.Collections.Specialized;
    using System.Reflection;
    using System.Runtime.InteropServices;
    using System.CodeDom;
    using System.Security;
    using System.Security.Permissions;
    using System.Security.Principal;
    using Microsoft.Win32;
    using Microsoft.Win32.SafeHandles;
    using System.Globalization;
    using System.Runtime.CompilerServices;
    using System.Runtime.ConstrainedExecution;
        
    /// <devdoc>
    ///    <para>
    ///       Provides command execution functions for the CodeDom compiler.
    ///    </para>
    /// </devdoc>
    [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
    [PermissionSet(SecurityAction.InheritanceDemand, Name="FullTrust")]
    public sealed class Executor {

        // How long (in milliseconds) do we wait for the program to terminate
        private const int ProcessTimeOut = 600000;

        private Executor() {
        }

        /// <devdoc>
        ///    <para>
        ///       Gets the runtime install directory.
        ///    </para>
        /// </devdoc>
        internal static string GetRuntimeInstallDirectory() {
            return RuntimeEnvironment.GetRuntimeDirectory();
        }

        private static FileStream CreateInheritedFile(string file) {
            return new FileStream(file, FileMode.CreateNew, FileAccess.Write, FileShare.Read | FileShare.Inheritable);
        }

        /// <devdoc>
        /// </devdoc>
        public static void ExecWait(string cmd, TempFileCollection tempFiles) {
            string outputName = null;
            string errorName = null;
            ExecWaitWithCapture(null, cmd, tempFiles, ref outputName, ref errorName);
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static int ExecWaitWithCapture(string cmd, TempFileCollection tempFiles, ref string outputName, ref string errorName) {
            return ExecWaitWithCapture(null, cmd, Environment.CurrentDirectory, tempFiles, ref outputName, ref errorName, null);
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static int ExecWaitWithCapture(string cmd, string currentDir, TempFileCollection tempFiles, ref string outputName, ref string errorName) {
            return ExecWaitWithCapture(null, cmd, currentDir, tempFiles, ref outputName, ref errorName, null);
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static int ExecWaitWithCapture(IntPtr userToken, string cmd, TempFileCollection tempFiles, ref string outputName, ref string errorName) {
            return ExecWaitWithCapture(new SafeUserTokenHandle(userToken, false), cmd, Environment.CurrentDirectory, tempFiles, ref outputName, ref errorName, null);
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static int ExecWaitWithCapture(IntPtr userToken, string cmd, string currentDir, TempFileCollection tempFiles, ref string outputName, ref string errorName) {
            return ExecWaitWithCapture(new SafeUserTokenHandle(userToken, false), cmd, Environment.CurrentDirectory, tempFiles, ref outputName, ref errorName, null);
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        internal static int ExecWaitWithCapture(SafeUserTokenHandle userToken, string cmd, string currentDir, TempFileCollection tempFiles, ref string outputName, ref string errorName, string trueCmdLine) {
            int retValue = 0;
            // Undo any current impersonation, call ExecWaitWithCaptureUnimpersonated, and reimpersonate
                    
                    // Execute the process
                    retValue = ExecWaitWithCaptureUnimpersonated(userToken, cmd, currentDir, tempFiles, ref outputName, ref errorName, trueCmdLine);
            return retValue;
        }

        private static unsafe int ExecWaitWithCaptureUnimpersonated(SafeUserTokenHandle userToken, string cmd, string currentDir, TempFileCollection tempFiles, ref string outputName, ref string errorName, string trueCmdLine) {

            IntSecurity.UnmanagedCode.Demand();

            FileStream output;
            FileStream error;

            int retValue = 0;

            if (outputName == null || outputName.Length == 0)
                outputName = tempFiles.AddExtension("out");

            if (errorName == null || errorName.Length == 0)
                errorName = tempFiles.AddExtension("err");

            // Create the files
            output = CreateInheritedFile(outputName);
            error = CreateInheritedFile(errorName);

            bool success = false;
            SafeNativeMethods.PROCESS_INFORMATION pi = new SafeNativeMethods.PROCESS_INFORMATION();
            SafeProcessHandle procSH = new SafeProcessHandle();
            SafeThreadHandle threadSH = new SafeThreadHandle();
            SafeUserTokenHandle primaryToken = null;

            try {
                // Output the command line...
                StreamWriter sw = new StreamWriter(output, Encoding.UTF8);
                sw.Write(currentDir);
                sw.Write("> ");
                // 'true' command line is used in case the command line points to
                // a response file
                sw.WriteLine(trueCmdLine != null ? trueCmdLine : cmd);
                sw.WriteLine();
                sw.WriteLine();
                sw.Flush();

                NativeMethods.STARTUPINFO si = new NativeMethods.STARTUPINFO();

                si.cb = Marshal.SizeOf(si);
                si.dwFlags = NativeMethods.STARTF_USESTDHANDLES;
                si.hStdOutput = output.SafeFileHandle;
                si.hStdError = error.SafeFileHandle;
                si.hStdInput = new SafeFileHandle(UnsafeNativeMethods.GetStdHandle(NativeMethods.STD_INPUT_HANDLE), false);

                //
                // Prepare the environment
                //
#if PLATFORM_UNIX

                StringDictionary environment = new CaseSensitiveStringDictionary();

#else

                StringDictionary environment = new StringDictionary ();

#endif // PLATFORM_UNIX



                // Add the current environment

                foreach ( DictionaryEntry entry in Environment.GetEnvironmentVariables () )

                    environment.Add ( (string) entry.Key, (string) entry.Value );



                // Add the flag to indicate restricted security in the process
                environment["_ClrRestrictSecAttributes"] = "1";

                #if DEBUG
                environment["OANOCACHE"] = "1";
                #endif

                // set up the environment block parameter
                byte[] environmentBytes = EnvironmentBlock.ToByteArray(environment, false);
                fixed (byte* environmentBytesPtr = environmentBytes) {
                    IntPtr environmentPtr = new IntPtr((void*)environmentBytesPtr);

                    if (userToken == null || userToken.IsInvalid) {
                        RuntimeHelpers.PrepareConstrainedRegions();
                        try {} finally {
                           success = NativeMethods.CreateProcess(
                                                       null,       // String lpApplicationName, 
                                                       new StringBuilder(cmd), // String lpCommandLine, 
                                                       null,       // SECURITY_ATTRIBUTES lpProcessAttributes, 
                                                       null,       // SECURITY_ATTRIBUTES lpThreadAttributes, 
                                                       true,       // bool bInheritHandles, 
                                                       0,          // int dwCreationFlags, 
                                                       environmentPtr, // int lpEnvironment, 
                                                       currentDir, // String lpCurrentDirectory, 
                                                       si,         // STARTUPINFO lpStartupInfo, 
                                                       pi);        // PROCESS_INFORMATION lpProcessInformation);
                            if ( pi.hProcess!= (IntPtr)0 && pi.hProcess!= (IntPtr)NativeMethods.INVALID_HANDLE_VALUE)
                                procSH.InitialSetHandle(pi.hProcess);  
                            if ( pi.hThread != (IntPtr)0 && pi.hThread != (IntPtr)NativeMethods.INVALID_HANDLE_VALUE)
                               threadSH.InitialSetHandle(pi.hThread);
                        }
                    }
                    else {
                        throw new NotSupportedException();
                    }
                }
            }
            finally {
                // Close the file handles
                if (!success && (primaryToken != null && !primaryToken.IsInvalid)) {
                    primaryToken.Close();
                    primaryToken = null;
                }

                output.Close();
                error.Close();
            }
            
            if (success) {

                try {
                    int ret = NativeMethods.WaitForSingleObject(procSH, ProcessTimeOut);

                    // Check for timeout
                    if (ret == NativeMethods.WAIT_TIMEOUT) {
                        throw new ExternalException(SR.GetString(SR.ExecTimeout, cmd), NativeMethods.WAIT_TIMEOUT);
                    }

                    if (ret != NativeMethods.WAIT_OBJECT_0) {
                        throw new ExternalException(SR.GetString(SR.ExecBadreturn, cmd), Marshal.GetLastWin32Error());
                    }

                    // Check the process's exit code
                    int status = NativeMethods.STILL_ACTIVE;
                    if (!NativeMethods.GetExitCodeProcess(procSH, out status)) {
                        throw new ExternalException(SR.GetString(SR.ExecCantGetRetCode, cmd), Marshal.GetLastWin32Error());
                    }

                    retValue = status;
                }
                finally {
                    procSH.Close();
                    threadSH.Close();
                    if (primaryToken != null && !primaryToken.IsInvalid)
                        primaryToken.Close();
                }
            }
            else {
                throw new ExternalException(SR.GetString(SR.ExecCantExec, cmd), Marshal.GetLastWin32Error());
            }

            return retValue;
        }



    }

}

