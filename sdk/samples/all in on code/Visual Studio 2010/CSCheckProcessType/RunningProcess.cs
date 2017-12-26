/****************************** Module Header ******************************\
 Module Name:  RunningProcess.cs
 Project:      CSCheckProcessType
 Copyright (c) Microsoft Corporation.
 
 This class represents a running process, and determines whether this process 
 is a 64bit process,  managed process, .NET 4.0 process, WPF process or console process.
 
 To determine whether a process is a 64bit process on x64 OS, we can use the Windows 
 API IsWow64Process Function.
 
 To determine whether a process is a managed process, we can check whether the 
 .Net Runtime Execution engine MSCOREE.dll is loaded.
 
 To determine whether a process is a managed process, we can check whether the 
 CLR.dll is loaded. Before .Net 4.0, the workstation CLR runtime is called 
 MSCORWKS.DLL. In .Net 4.0, this DLL is replaced by CLR.dll. 
 
 To determine whether a process is a WPF process, we can check whether the 
 PresentationCore.dll is loaded.
 
 To determine whether a process is a console process, we can check whether
 the target process has a console window.
 
 
 This source is subject to the Microsoft Public License.
 See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 All other rights reserved.
 
 THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Security.Permissions;
using System.Runtime.InteropServices;
using System.Text;
using System.IO;
using System.ComponentModel;

namespace CSCheckProcessType
{
    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public class RunningProcess
    {

        public static bool IsOSVersionSupported
        {
            get
            {
                return Environment.OSVersion.Version.Major >= 6;
            }
        }

        // The System.Diagnostics.Process instance.
        Process diagnosticsProcess;

        /// <summary>
        /// The name of the process.
        /// </summary>
        public string ProcessName
        {
            get
            {
                return this.diagnosticsProcess.ProcessName;
            }
        }

        /// <summary>
        /// The ID of the process.
        /// </summary>
        public int Id
        {
            get
            {
                return this.diagnosticsProcess.Id;
            }
        }

        /// <summary>
        /// Specify whether the process is a managed application.
        /// </summary>
        public bool IsManaged { get; private set; }

        /// <summary>
        /// Specify whether the process is a .Net 4.0 application.
        /// </summary>
        public bool IsDotNet4 { get; private set; }

        /// <summary>
        /// Specify whether the process is a console application.
        /// </summary>
        public bool IsConsole { get; private set; }

        /// <summary>
        /// Specify whether the process is a WPF application.
        /// </summary>
        public bool IsWPF { get; private set; }

        /// <summary>
        /// Specify whether the process is a 64bit application.
        /// </summary>
        public bool Is64BitProcess { get; private set; }

        /// <summary>
        /// The remarks of this instance. Normally it is the exception message.
        /// </summary>
        public string Remarks { get; private set; }

        public RunningProcess(Process proc)
        {
            this.diagnosticsProcess = proc;

            try
            {
                CheckProcess();
            }
            catch (Exception ex)
            {
                this.Remarks = ex.Message;
            }
        }

        /// <summary>
        /// Check the properties of the process.
        /// </summary>
        public void CheckProcess()
        {
            uint procID = (uint)this.diagnosticsProcess.Id;

            try
            {
                // Use kernel32.dll attach the process console to the windows form. 
                if (NativeMethods.AttachConsole(procID))
                {

                    // Use Kernel32.dll get the current process (windows form) std handle,
                    // as we attach the console window before.
                    IntPtr handle = NativeMethods.GetStdHandle(NativeMethods.STD_OUTPUT_HANDLE);
                    uint lp = 0;
                    this.IsConsole = NativeMethods.GetConsoleMode(handle, out lp);
                    NativeMethods.FreeConsole();
                }
            }
            catch (Exception ex)
            {
                this.Remarks += string.Format("| Check IsConsole: {0}", ex.Message);
            }


            try
            {
                List<string> loadedModules = this.GetLoadedModules();

                // Check whether the .Net Runtime Execution engine MSCOREE.dll is loaded.
                this.IsManaged = loadedModules.Count(m => m.Equals("MSCOREE.dll",
                    StringComparison.OrdinalIgnoreCase)) > 0;
                if (this.IsManaged)
                {

                    // Check whether the CLR.dll is loaded.
                    this.IsDotNet4 = loadedModules.Count(m => m.Equals("CLR.dll",
                        StringComparison.OrdinalIgnoreCase)) > 0;

                    // Check whether the PresentationCore.dll is loaded.
                    this.IsWPF = loadedModules.Count(m =>
                        m.Equals("PresentationCore.dll", StringComparison.OrdinalIgnoreCase)
                        || m.Equals("PresentationCore.ni.dll", StringComparison.OrdinalIgnoreCase)) > 0;
                }
            }
            catch (Exception ex)
            {
                this.Remarks += string.Format("| Check IsManaged: {0}", ex.Message);
            }

            try
            {

                this.Is64BitProcess = Check64BitProcess();
            }
            catch (Exception ex)
            {
                this.Remarks += string.Format("| Check Is64Bit: {0}", ex.Message);
            }
        }

        /// <summary>
        /// Get all loaded modules using EnumProcessModulesEx Function.
        /// The EnumProcessModulesEx function is only available on Vista or later versions.
        /// </summary>
        /// <returns></returns>
        List<string> GetLoadedModules()
        {
            if (Environment.OSVersion.Version.Major < 6)
            {
                throw new ApplicationException("This application must run on Windows Vista"
                    + " or later versions. ");
            }

            IntPtr[] modulesHandles = new IntPtr[1024];
            int size = 0;

            var success = NativeMethods.EnumProcessModulesEx(
                this.diagnosticsProcess.Handle,
                modulesHandles,
                Marshal.SizeOf(typeof(IntPtr))*modulesHandles.Length,
                out size,
                NativeMethods.ModuleFilterFlags.LIST_MODULES_ALL);

            if (!success)
            {
                throw new Win32Exception();
            }

            List<string> moduleNames = new List<string>();

            for (int i = 0; i < modulesHandles.Length; i++)
            {
                if (modulesHandles[i] == IntPtr.Zero)
                {
                    break;
                }

                StringBuilder moduleName = new StringBuilder(1024);

                uint length = NativeMethods.GetModuleFileNameEx(
                    this.diagnosticsProcess.Handle,
                    modulesHandles[i],
                    moduleName,
                    (uint)moduleName.Capacity);

                if (length <= 0)
                {
                    throw new Win32Exception();
                }
                else
                {
                    var fileName = Path.GetFileName(moduleName.ToString());
                    moduleNames.Add(fileName);
                }
            }

            return moduleNames;
        }

        /// <summary>
        /// Determines whether the specified process is a 64-bit process.
        /// </summary>
        /// <param name="hProcess">The process handle</param>
        /// <returns>
        /// true if the given process is 64-bit; otherwise, false.
        /// </returns>
        bool Check64BitProcess()
        {

            bool flag = false;

            if (Environment.Is64BitOperatingSystem)
            {
                // On 64-bit OS, if a process is not running under Wow64 mode, 
                // the process must be a 64-bit process.
                flag = !(NativeMethods.IsWow64Process(this.diagnosticsProcess.Handle, out flag) && flag);
            }

            return flag;
        }
    }
}
