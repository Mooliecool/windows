/****************************** Module Header ******************************\
* Module Name:  ManagedProcess.cs
* Project:	    CSEnumerateAppDomains
* Copyright (c) Microsoft Corporation.
* 
* This class represents a managed process. It contains a MDbgProcess field
* and a System.Diagnostics.Process field. This class also supplies three 
* static methods: GetManagedProcesses, GetManagedProcessByID and
* GetAppDomainsInCurrentProcess.
*  
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security.Permissions;
using Microsoft.Samples.Debugging.CorDebug;
using Microsoft.Samples.Debugging.MdbgEngine;
using mscoree;

namespace CSEnumerateAppDomains
{
    [PermissionSet(SecurityAction.LinkDemand, Name = "FullTrust")]
    [PermissionSet(SecurityAction.InheritanceDemand, Name = "FullTrust")]
    public class ManagedProcess : IDisposable
    {
        private bool disposed = false;

        // Represents a Process in which code execution context can be controlled.
        private MDbgProcess _mdbgProcess;

        /// <summary>
        /// Do not intialize the _mdbgProcess until it is called.
        /// </summary>
        private MDbgProcess MDbgProcess
        {
            get
            {
                if (_mdbgProcess == null)
                {
                    try
                    {

                        // Intialize a MDbgEngine instance. 
                        MDbgEngine engine = new MDbgEngine();

                        //Attach a debugger to the specified process, and 
                        //return a MDbgProcess instance that represents the Process.
                        if (this.LoadedRuntimes != null && this.LoadedRuntimes.Count() == 1)
                        {
                            _mdbgProcess = engine.Attach(ProcessID, 
                                LoadedRuntimes.First().GetVersionString());
                        }
                        else
                        {
                            _mdbgProcess = engine.Attach(ProcessID, 
                                CorDebugger.GetDefaultDebuggerVersion());
                        }

                        // Wait 2 seconds if the process is not in Synchronized state.
                        // One process can only be attached when it is in Synchronized state.
                        // See http://msdn.microsoft.com/en-us/library/ms404528.aspx
                        bool result = _mdbgProcess.Go().WaitOne(2000);
                        
                        if (!result)
                        {
                            throw new ApplicationException(
                                string.Format(@"The process with an ID {0} could not be "
                                + "attached. Operation time out.", ProcessID));
                        }

                    }
                    catch (COMException)
                    {
                        throw new ApplicationException(
                            string.Format(@"The process with an ID {0} could not be attached. "
                            + "Access is denied or it has already been attached.", ProcessID));
                    }

                }

                return _mdbgProcess;
            }
        }

        private System.Diagnostics.Process _diagnosticsProcess = null;

        /// <summary>
        /// Get System.Diagnostics.Process using ProcessID.
        /// </summary>
        public System.Diagnostics.Process DiagnosticsProcess
        {
            get
            {
                return _diagnosticsProcess;
            }
        }

        /// <summary>
        /// The ID of the process. 
        /// </summary>
        public int ProcessID
        {
            get
            {
                return DiagnosticsProcess.Id;
            }
        }

        /// <summary>
        /// The name of the process. 
        /// </summary>
        public string ProcessName
        {
            get
            {
                return DiagnosticsProcess.ProcessName;
            }
        }

        /// <summary>
        /// Get all Runtimes loaded in the process.
        /// </summary>
        public IEnumerable<CLRRuntimeInfo> LoadedRuntimes
        {
            get
            {
                try
                {
                    CLRMetaHost host = new CLRMetaHost();
                    return host.EnumerateLoadedRuntimes(ProcessID);
                }
                catch (EntryPointNotFoundException)
                {
                    return null;
                }
            }
        }

        /// <summary>
        /// Gets all CorAppDomains in the processes. 
        /// </summary>
        public IEnumerable AppDomains
        {
            get
            {
                var _appDomains = MDbgProcess.CorProcess.AppDomains;
                return _appDomains;
            }
        }


        private ManagedProcess(int processID)
        {
            System.Diagnostics.Process diagnosticsProcess =
                System.Diagnostics.Process.GetProcessById(processID);
            this._diagnosticsProcess = diagnosticsProcess;

            // Make sure that the specified process is a managed process.
            if (this.LoadedRuntimes == null || this.LoadedRuntimes.Count() == 0)
            {
                throw new ArgumentException("This process is not a managed process. ");
            }
        }


        private ManagedProcess(System.Diagnostics.Process diagnosticsProcess)
        {
            if (diagnosticsProcess == null)
            {
                throw new ArgumentNullException("diagnosticsProcess",
                    "The System.Diagnostics.Process could not be null. ");
            }
            this._diagnosticsProcess = diagnosticsProcess;
            if (this.LoadedRuntimes == null || this.LoadedRuntimes.Count() == 0)
            {
                throw new ArgumentException("This process is not a managed process. ");
            }
        }



        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }


        protected virtual void Dispose(bool disposing)
        {
            // Protect from being called multiple times.
            if (disposed) return;

            if (disposing)
            {
                // Clean up all managed resources.
                if (_mdbgProcess != null)
                {

                    // Make sure the underlying CorProcess was stopped before 
                    // detached it.                     
                    var waithandler = _mdbgProcess.AsyncStop();
                    waithandler.WaitOne();
                    _mdbgProcess.Detach();

                }
            }

            disposed = true;
        }


        /// <summary>
        /// Gets all managed processes. 
        /// </summary>
        public static List<ManagedProcess> GetManagedProcesses()
        {
            List<ManagedProcess> managedProcesses = new List<ManagedProcess>();

            // CLRMetaHost implements ICLRMetaHost Interface which provides a method that
            // return list all runtimes that are loaded in a specified process.
            CLRMetaHost host = new CLRMetaHost();

            var processes = System.Diagnostics.Process.GetProcesses();

            foreach (System.Diagnostics.Process diagnosticsProcess in processes)
            {
                try
                {

                    // Lists all runtimes that are loaded in a specified process.
                    var runtimes = host.EnumerateLoadedRuntimes(diagnosticsProcess.Id);

                    // If the process loads CLRs, it could be considered as a managed process.
                    if (runtimes != null && runtimes.Count() > 0)
                    {
                        managedProcesses.Add(new ManagedProcess(diagnosticsProcess));
                    }

                }

                // The method EnumerateLoadedRuntimes will throw Win32Exception when the 
                // file cannot be found or access is denied. For example, the 
                // diagnosticsProcess is System or System Idle Process.
                catch (Win32Exception)
                { }

                // The method EnumerateLoadedRuntimes will throw COMException when it tries
                // to access a 64bit process on 64bit OS if this application is built on 
                // platform x86.
                catch (COMException)
                { }
            }
            return managedProcesses;
        }

        /// <summary>
        /// Gets a managed process by ID. This method is used to get AppDomains in other
        /// process. If you want to get AppDomains in current process, please use the
        /// static method GetAppDomainsInCurrentProcess.
        /// </summary>
        /// <exception cref="ArgumentException">
        /// If there is no managed process with this ID, an ArgumentException will be thrown.
        /// </exception>
        public static ManagedProcess GetManagedProcessByID(int processID)
        {
            if (processID == System.Diagnostics.Process.GetCurrentProcess().Id)
            {
                throw new System.ArgumentException("Cannot debug current process.");
            }
            return new ManagedProcess(processID);
        }

        /// <summary>
        /// Gets all AppDomains in current process.
        /// This method uses ICorRuntimeHost interface to get an enumerator for the domains
        /// in the current process.
        /// </summary>
        public static List<AppDomain> GetAppDomainsInCurrentProcess()
        {
            var appDomains = new List<AppDomain>();
            var hEnum = IntPtr.Zero;

            // The class CorRuntimeHostClass is a coclass, which implements the
            // ICorRuntimeHost interface. 
            var host = new CorRuntimeHost();

            try
            {
                // Gets an enumerator for the domains in the current process.
                host.EnumDomains(out hEnum);
                while (true)
                {
                    object domain;
                    host.NextDomain(hEnum, out domain);
                    if (domain == null)
                    {
                        break;
                    }
                    appDomains.Add(domain as AppDomain);
                }
            }
            finally
            {
                host.CloseEnum(hEnum);
                Marshal.ReleaseComObject(host);
            }
            return appDomains;
        }

    }
}
