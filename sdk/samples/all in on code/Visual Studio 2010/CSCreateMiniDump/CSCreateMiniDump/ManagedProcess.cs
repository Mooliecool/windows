/****************************** Module Header ******************************\
 * Module Name:  ManagedProcess.cs
 * Project:      CSCreateMiniDump
 * Copyright (c) Microsoft Corporation.
 * 
 * This class represents a managed process. It contains a MDbgProcess field
 * and a System.Diagnostics.Process field. When a new instance is initialized,
 * it will attach a debugger to the target process.
 * 
 * When the target process stops, this class will check the stop reason to 
 * continue or quite.
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
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security.Permissions;
using Microsoft.Samples.Debugging.CorDebug;
using Microsoft.Samples.Debugging.MdbgEngine;
using System.ComponentModel;

namespace CSCreateMiniDump
{
    [PermissionSet(SecurityAction.LinkDemand, Name = "FullTrust")]
    [PermissionSet(SecurityAction.InheritanceDemand, Name = "FullTrust")]
    public class ManagedProcess : IDisposable
    {
        private bool disposed = false;

        // The reason why the process that is debugging stopped.
        private ManagedCallbackType stopReason;

        // Specify whether there is an unhandled exception in the process.
        private bool isExceptionUnhandled = false;

        private MDbgEngine debugger;

        public MDbgEngine Debugger
        {
            get
            {
                if (debugger == null)
                {
                    debugger = new MDbgEngine();
                }

                return debugger;
            }
        }

        /// <summary>
        /// Represents a Process in which code execution context can be controlled.
        /// </summary>
        public MDbgProcess MDbgProcess { get; private set; }


        private System.Diagnostics.Process diagnosticsProcess = null;

        /// <summary>
        /// Get System.Diagnostics.Process using ProcessID.
        /// </summary>
        public System.Diagnostics.Process DiagnosticsProcess
        {
            get
            {
                return diagnosticsProcess;
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

        // This event occurs when the watch dog start to handle the exception. If it is 
        // cancelled, the watch dog will not continue to handle the exception.
        public event EventHandler<CancelEventArgs> StartHandleException;

        // This event occurs when the watch dog has got the information of the 
        // unhandled exception.
        public event EventHandler<ManagedProcessUnhandledExceptionOccurredEventArgs> UnhandledExceptionOccurred;

        public ManagedProcess(System.Diagnostics.Process diagnosticsProcess)
        {
            if (diagnosticsProcess == null)
            {
                throw new ArgumentNullException("diagnosticsProcess",
                    "The System.Diagnostics.Process could not be null. ");
            }
            this.diagnosticsProcess = diagnosticsProcess;

            AttachDebuggerToProcess();

        }

        /// <summary>
        /// Attach a debugger to the target process.
        /// </summary>
        void AttachDebuggerToProcess()
        {
            string version =
                MdbgVersionPolicy.GetDefaultAttachVersion(this.DiagnosticsProcess.Id);
            if (string.IsNullOrEmpty(version))
            {
                throw new ApplicationException("Can't determine what version of the CLR to " +
                    "attach to the process.");
            }
            this.MDbgProcess = this.Debugger.Attach(this.DiagnosticsProcess.Id, null, version);

            bool result = this.MDbgProcess.Go().WaitOne();

            if (!result)
            {
                throw new ApplicationException(
                    string.Format(@"The process with an ID {0} could not be "
                    + "attached. Operation time out.", this.DiagnosticsProcess.Id));
            }

            this.MDbgProcess.PostDebugEvent +=
                new PostCallbackEventHandler(MDbgProcess_PostDebugEvent);
        }


        /// <summary>
        /// Handle the Debug Event of the MDbgProcess.
        /// </summary>
        void MDbgProcess_PostDebugEvent(object sender, CustomPostCallbackEventArgs e)
        {
            this.stopReason = e.CallbackType;

            switch (this.stopReason)
            {
                case ManagedCallbackType.OnException:
                    CorExceptionEventArgs exceptionEventArgs =
                        e.CallbackArgs as CorExceptionEventArgs;
                    this.isExceptionUnhandled =
                        exceptionEventArgs != null && exceptionEventArgs.Unhandled;
                    break;
                default:
                    break;
            }
        }

        /// <summary>
        /// When the target process stops, check the stop reason and determine to 
        /// continue or quite. 
        /// </summary>
        public void StartWatch()
        {
            while (true)
            {
                this.MDbgProcess.Go().WaitOne();

                if (this.stopReason == ManagedCallbackType.OnException
                    && this.isExceptionUnhandled)
                {
                    this.HandleException();
                    break;
                }
                else if (this.stopReason == ManagedCallbackType.OnProcessExit)
                {
                    break;
                }
            }
        }

        /// <summary>
        /// Initialize a ManagedProcessUnhandledExceptionOccurredEventArgs object and raise the 
        /// UnhandledExceptionOccurred event.
        /// </summary>
        private void HandleException()
        {
            CancelEventArgs e = new CancelEventArgs();
            this.OnStartHandleException(e);
            if (e.Cancel)
            {
                return;
            }

            MDbgValue ex = this.MDbgProcess.Threads.Active.CurrentException;
            if (ex.IsNull)
            {
                // No current exception is available.  Perhaps the user switched to a different
                // thread which was not throwing an exception.
                return;
            }

            IntPtr exceptionPointers = IntPtr.Zero;

            foreach (MDbgValue f in ex.GetFields())
            {
                if (f.Name == "_xptrs")
                {
                    string outputValue = f.GetStringValue(0);
                    exceptionPointers = (IntPtr)int.Parse(outputValue);
                }
            }

            if (exceptionPointers == IntPtr.Zero)
            {
                // Get the Exception Pointer in the target process
                MDbgValue value = FunctionEval(
                    "System.Runtime.InteropServices.Marshal.GetExceptionPointers");
                if (value != null)
                {
                    exceptionPointers = (IntPtr)int.Parse(value.GetStringValue(1));
                }
            }

            this.OnUnhandledExceptionOccurred(
                new ManagedProcessUnhandledExceptionOccurredEventArgs
                {
                    ProcessID = this.MDbgProcess.CorProcess.Id,
                    ThreadID = this.MDbgProcess.Threads.Active.Id,
                    ExceptionPointers = exceptionPointers
                });
        }

        /// <summary>
        ///  Eval a function in the target process.
        /// </summary>
        /// <param name="functionNameFromScope">The full function name.</param>
        MDbgValue FunctionEval(string functionNameFromScope)
        {
            CorAppDomain corAD = this.MDbgProcess.Threads.Active.CorThread.AppDomain;
            MDbgFunction function = this.MDbgProcess.ResolveFunctionNameFromScope(
                functionNameFromScope, corAD);
            CorEval eval = this.MDbgProcess.Threads.Active.CorThread.CreateEval();
            eval.CallFunction(function.CorFunction, new CorValue[0]);
            MDbgProcess.Go().WaitOne();

            MDbgValue value = null;
            if (MDbgProcess.StopReason is EvalCompleteStopReason)
            {
                CorValue result =
                    (MDbgProcess.StopReason as EvalCompleteStopReason).Eval.Result;
                if (result != null)
                {
                    value = new MDbgValue(MDbgProcess, result);
                }
            }
            return value;
        }

        /// <summary>
        /// Raise the UnhandledExceptionOccurred event.
        /// </summary>
        /// <param name="e"></param>
        protected virtual void OnUnhandledExceptionOccurred(
            ManagedProcessUnhandledExceptionOccurredEventArgs e)
        {
            if (UnhandledExceptionOccurred != null)
            {
                UnhandledExceptionOccurred(this, e);
            }
        }

        protected virtual void OnStartHandleException(CancelEventArgs e)
        {
            if (StartHandleException != null)
            {
                StartHandleException(this, e);
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
                if (this.MDbgProcess != null)
                {

                    // Make sure the underlying CorProcess was stopped before 
                    // detached it.  
                    if (this.MDbgProcess.IsAlive)
                    {

                        var waithandler = this.MDbgProcess.AsyncStop();
                        waithandler.WaitOne();
                        this.MDbgProcess.Detach();
                    }

                }
            }

            disposed = true;
        }
    }
}
