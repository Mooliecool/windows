//------------------------------------------------------------------------------
// <copyright file="Process.cs" company="Microsoft">
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

namespace System.Diagnostics {
    using System.Text;
    using System.Threading;
    using System.Runtime.InteropServices;
    using System.ComponentModel;
    using System.ComponentModel.Design;
    using System.Runtime.CompilerServices;
    using System.Runtime.ConstrainedExecution;
    using System.Diagnostics;
    using System;
    using System.Collections;
    using System.IO;
    using Microsoft.Win32;        
    using Microsoft.Win32.SafeHandles;
    using System.Collections.Specialized;
    using System.Globalization;
    using System.Security;
    using System.Security.Permissions;
    using System.Security.Principal;
    using System.Runtime.Versioning;
    
    /// <devdoc>
    ///    <para>
    ///       Provides access to local and remote
    ///       processes. Enables you to start and stop system processes.
    ///    </para>
    /// </devdoc>
    [
    MonitoringDescription(SR.ProcessDesc),
    DefaultEvent("Exited"), 
    DefaultProperty("StartInfo"),
    Designer("System.Diagnostics.Design.ProcessDesigner, " + AssemblyRef.SystemDesign),
    // Disabling partial trust scenarios
    PermissionSet(SecurityAction.LinkDemand, Name="FullTrust"),
    PermissionSet(SecurityAction.InheritanceDemand, Name="FullTrust"),
    HostProtection(SharedState=true, Synchronization=true, ExternalProcessMgmt=true, SelfAffectingProcessMgmt=true)
    ]
    public class Process : Component {
        //
        // FIELDS
        //

        bool haveProcessId;
        int processId;
        bool haveProcessHandle;
        SafeProcessHandle m_processHandle;
        bool isRemoteMachine;
        string machineName;
        ProcessInfo processInfo;


        bool haveMainWindow;
        IntPtr mainWindowHandle;  // no need to use SafeHandle for window        
        string mainWindowTitle;
        
        bool haveWorkingSetLimits;
        IntPtr minWorkingSet;
        IntPtr maxWorkingSet;
        
        bool haveProcessorAffinity;
        IntPtr processorAffinity;

        bool havePriorityClass;
        ProcessPriorityClass priorityClass;

        ProcessStartInfo startInfo;
        
        bool watchForExit;
        bool watchingForExit;
        EventHandler onExited;
        bool exited;
        int exitCode;
        bool signaled;
		
        DateTime exitTime;
        bool haveExitTime;
        
        bool responding;
        bool haveResponding;
        
        bool priorityBoostEnabled;
        bool havePriorityBoostEnabled;
        
        bool raisedOnExited;
        RegisteredWaitHandle registeredWaitHandle;
        WaitHandle waitHandle;
        ISynchronizeInvoke synchronizingObject;
        StreamReader standardOutput;
        StreamWriter standardInput;
        StreamReader standardError;
        OperatingSystem operatingSystem;
        bool disposed;
        
        // This enum defines the operation mode for redirected process stream.
        // We don't support switching between synchronous mode and asynchronous mode.
        private enum StreamReadMode 
        {
            undefined, 
            syncMode, 
            asyncMode
        }
        
        StreamReadMode outputStreamReadMode;
        StreamReadMode errorStreamReadMode;
        
       
        // Support for asynchrously reading streams
        [Browsable(true), MonitoringDescription(SR.ProcessAssociated)]
        //[System.Runtime.InteropServices.ComVisible(false)]        
        public event DataReceivedEventHandler OutputDataReceived;
        [Browsable(true), MonitoringDescription(SR.ProcessAssociated)]
        //[System.Runtime.InteropServices.ComVisible(false)]        
        public event DataReceivedEventHandler ErrorDataReceived;
        // Abstract the stream details
        internal AsyncStreamReader output;
        internal AsyncStreamReader error;
        internal bool pendingOutputRead;
        internal bool pendingErrorRead;


        private static SafeFileHandle InvalidPipeHandle = new SafeFileHandle(IntPtr.Zero, false);
#if DEBUG
        internal static TraceSwitch processTracing = new TraceSwitch("processTracing", "Controls debug output from Process component");
#else
        internal static TraceSwitch processTracing = null;
#endif

        //
        // CONSTRUCTORS
        //

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of the <see cref='System.Diagnostics.Process'/> class.
        ///    </para>
        /// </devdoc>
        public Process() {
            machineName = ".";
            outputStreamReadMode = StreamReadMode.undefined;
            errorStreamReadMode = StreamReadMode.undefined;            
        }        
        
        Process(string machineName, bool isRemoteMachine, int processId, ProcessInfo processInfo) : base() {
            Debug.Assert(SyntaxCheck.CheckMachineName(machineName), "The machine name should be valid!");
            this.processInfo = processInfo;
            this.machineName = machineName;
            this.isRemoteMachine = isRemoteMachine;
            this.processId = processId;
            this.haveProcessId = true;
            outputStreamReadMode = StreamReadMode.undefined;
            errorStreamReadMode = StreamReadMode.undefined;            
        }

        //
        // PROPERTIES
        //

        /// <devdoc>
        ///     Returns whether this process component is associated with a real process.
        /// </devdoc>
        /// <internalonly/>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden), MonitoringDescription(SR.ProcessAssociated)]
        bool Associated {
            get {
                return haveProcessId || haveProcessHandle;
            }
        }


        /// <devdoc>
        ///    <para>
        ///       Gets
        ///       the
        ///       value that was specified by the associated process when it was terminated.
        ///    </para>
        /// </devdoc>
        [Browsable(false),DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden), MonitoringDescription(SR.ProcessExitCode)]
        public int ExitCode {
            get {
                EnsureState(State.Exited);
                return exitCode;
            }
          }

        /// <devdoc>
        ///    <para>
        ///       Gets a
        ///       value indicating whether the associated process has been terminated.
        ///    </para>
        /// </devdoc>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden), MonitoringDescription(SR.ProcessTerminated)]
        public bool HasExited {
            get {
                if (!exited) {
                    EnsureState(State.Associated);
                    SafeProcessHandle handle = null;
                    try {
                        handle = GetProcessHandle(NativeMethods.PROCESS_QUERY_INFORMATION | NativeMethods.SYNCHRONIZE, false);
                        if (handle.IsInvalid) {
                            exited = true;
                        }
                        else {
                            int exitCode;
                            
                            if (NativeMethods.GetExitCodeProcess(handle, out exitCode) && exitCode != NativeMethods.STILL_ACTIVE) {
                                this.exited = true;
                                this.exitCode = exitCode;                                
                            }
                            else {                                                        

                                // The best check for exit is that the kernel process object handle is invalid, 
                                // or that it is valid and signaled.  Checking if the exit code != STILL_ACTIVE 
                                // does not guarantee the process is closed,
                                // since some process could return an actual STILL_ACTIVE exit code (259).
                                if (!signaled) // if we just came from WaitForExit, don't repeat
                                {
                                    ProcessWaitHandle wh = null;
                                    try 
                                    {
                                        wh = new ProcessWaitHandle(handle);
                                        this.signaled = wh.WaitOne(0, false);					
                                    }
                                    finally
                                    {

                                        if (wh != null)
                                        wh.Close();
                                    }
                                }
                                if (signaled) 
                                {
                                    if (!NativeMethods.GetExitCodeProcess(handle, out exitCode))                               
                                        throw new Win32Exception();
                                
                                    this.exited = true;
                                    this.exitCode = exitCode;
                                }
                            }
                        }	
                    }
                    finally 
                    {
                        ReleaseProcessHandle(handle);
                    }

                    if (exited) {
                        RaiseOnExited();
                    }
                }
                return exited;
            }
        }

        private ProcessThreadTimes GetProcessTimes() {
            ProcessThreadTimes processTimes = new ProcessThreadTimes();
            SafeProcessHandle handle = null;
            try {
                handle = GetProcessHandle(NativeMethods.PROCESS_QUERY_INFORMATION, false);
                if( handle.IsInvalid) {
                    // On OS older than XP, we will not be able to get the handle for a process
                    // after it terminates. 
                    // On Windows XP and newer OS, the information about a process will stay longer. 
                    throw new InvalidOperationException(SR.GetString(SR.ProcessHasExited, processId.ToString(CultureInfo.CurrentCulture)));
                }

                if (!NativeMethods.GetProcessTimes(handle, 
                                                   out processTimes.create, 
                                                   out processTimes.exit, 
                                                   out processTimes.kernel, 
                                                   out processTimes.user)) {
                    throw new Win32Exception();
                }

            }
            finally {
                ReleaseProcessHandle(handle);                
            }
            return processTimes;
        }


        /// <devdoc>
        ///    <para>
        ///       Returns the native handle for the associated process. The handle is only available
        ///       if this component started the process.
        ///    </para>
        /// </devdoc>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden), MonitoringDescription(SR.ProcessHandle)]
        public IntPtr Handle {
            [ResourceExposure(ResourceScope.Machine)]
            [ResourceConsumption(ResourceScope.Machine)]
            get {
                EnsureState(State.Associated);
                return OpenProcessHandle().DangerousGetHandle();
            }
        }


        /// <devdoc>
        ///    <para>
        ///       Gets
        ///       the unique identifier for the associated process.
        ///    </para>
        /// </devdoc>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden), MonitoringDescription(SR.ProcessId)]
        public int Id {
            get {
                EnsureState(State.HaveId);
                return processId;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets
        ///       the name of the computer on which the associated process is running.
        ///    </para>
        /// </devdoc>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden), MonitoringDescription(SR.ProcessMachineName)]
        public string MachineName {
            get {
                EnsureState(State.Associated);
                return machineName;
            }
        }


        /// <devdoc>
        ///    <para>
        ///       Gets
        ///       the friendly name of the process.
        ///    </para>
        /// </devdoc>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden), MonitoringDescription(SR.ProcessProcessName)]
        public string ProcessName {
            get {
                return "ProcessName not implemented for PAL";
            }
        }


        /// <devdoc>
        ///    <para>
        ///       Gets or sets the properties to pass into the <see cref='System.Diagnostics.Process.Start'/> method for the <see cref='System.Diagnostics.Process'/>
        ///       .
        ///    </para>
        /// </devdoc>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Content), MonitoringDescription(SR.ProcessStartInfo)]
        public ProcessStartInfo StartInfo {
            get {
                if (startInfo == null) {
                    startInfo = new ProcessStartInfo(this);
                }                
                return startInfo;
            }
            set {
                if (value == null) { 
                    throw new ArgumentNullException("value");
                }
                startInfo = value;
            }
        }


        /// <devdoc>
        ///   Represents the object used to marshal the event handler
        ///   calls issued as a result of a Process exit. Normally 
        ///   this property will  be set when the component is placed 
        ///   inside a control or  a from, since those components are 
        ///   bound to a specific thread.
        /// </devdoc>
        [
        Browsable(false),
        DefaultValue(null),         
        MonitoringDescription(SR.ProcessSynchronizingObject)
        ]
        public ISynchronizeInvoke SynchronizingObject {
            get {
               if (this.synchronizingObject == null && DesignMode) {
                    IDesignerHost host = (IDesignerHost)GetService(typeof(IDesignerHost));
                    if (host != null) {
                        object baseComponent = host.RootComponent;
                        if (baseComponent != null && baseComponent is ISynchronizeInvoke)
                            this.synchronizingObject = (ISynchronizeInvoke)baseComponent;
                    }                        
                }

                return this.synchronizingObject;
            }
            
            set {
                this.synchronizingObject = value;
            }
        }


        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden), MonitoringDescription(SR.ProcessVirtualMemorySize)]
        [System.Runtime.InteropServices.ComVisible(false)]        
        public long VirtualMemorySize64 {
            get {
                EnsureState(State.HaveNtProcessInfo);
                return processInfo.virtualBytes;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets whether the <see cref='System.Diagnostics.Process.Exited'/>
        ///       event is fired
        ///       when the process terminates.
        ///    </para>
        /// </devdoc>
        [Browsable(false), DefaultValue(false), MonitoringDescription(SR.ProcessEnableRaisingEvents)]
        public bool EnableRaisingEvents {
            get {
                return watchForExit;
            }
            set {
                if (value != watchForExit) {
                    if (Associated) {
                        if (value) {
                            OpenProcessHandle();
                            EnsureWatchingForExit();
                        }
                        else {
                            StopWatchingForExit();
                        }
                    }
                    watchForExit = value;
                }
            }
        }


        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden), MonitoringDescription(SR.ProcessStandardInput)]
        public StreamWriter StandardInput {
            get { 
                if (standardInput == null) {
                    throw new InvalidOperationException(SR.GetString(SR.CantGetStandardIn));
                }

                return standardInput;
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden), MonitoringDescription(SR.ProcessStandardOutput)]
        public StreamReader StandardOutput {
            get {
                if (standardOutput == null) {
                    throw new InvalidOperationException(SR.GetString(SR.CantGetStandardOut));
                }

                if(outputStreamReadMode == StreamReadMode.undefined) {
                    outputStreamReadMode = StreamReadMode.syncMode;
                }
                else if (outputStreamReadMode != StreamReadMode.syncMode) {
                    throw new InvalidOperationException(SR.GetString(SR.CantMixSyncAsyncOperation));                    
                }
                    
                return standardOutput;
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden), MonitoringDescription(SR.ProcessStandardError)]
        public StreamReader StandardError {
            get { 
                if (standardError == null) {
                    throw new InvalidOperationException(SR.GetString(SR.CantGetStandardError));
                }

                if(errorStreamReadMode == StreamReadMode.undefined) {
                    errorStreamReadMode = StreamReadMode.syncMode;
                }
                else if (errorStreamReadMode != StreamReadMode.syncMode) {
                    throw new InvalidOperationException(SR.GetString(SR.CantMixSyncAsyncOperation));                    
                }

                return standardError;
            }
        }


        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden), MonitoringDescription(SR.ProcessWorkingSet)]
        [System.Runtime.InteropServices.ComVisible(false)]        
        public long WorkingSet64 {
            get {
                EnsureState(State.HaveNtProcessInfo);
                return processInfo.workingSet;
            }
        }

        [Category("Behavior"), MonitoringDescription(SR.ProcessExited)]
        public event EventHandler Exited {
            add {
                onExited += value;
            }
            remove {
                onExited -= value;
            }
        }


        /// <devdoc>
        ///     Release the temporary handle we used to get process information.
        ///     If we used the process handle stored in the process object (we have all access to the handle,) don't release it.
        /// </devdoc>
        /// <internalonly/>
        void ReleaseProcessHandle(SafeProcessHandle handle) {
            if (handle == null) { 
                return; 
            }

            if (haveProcessHandle && handle == m_processHandle) {
                return;
            }
            Debug.WriteLineIf(processTracing.TraceVerbose, "Process - CloseHandle(process)");
            handle.Close();
        }

        /// <devdoc>
        ///     This is called from the threadpool when a proces exits.
        /// </devdoc>
        /// <internalonly/>
        private void CompletionCallback(object context, bool wasSignaled) {
            StopWatchingForExit();
            RaiseOnExited();      
        }

        /// <internalonly/>
        /// <devdoc>
        ///    <para>
        ///       Free any resources associated with this component.
        ///    </para>
        /// </devdoc>
        protected override void Dispose(bool disposing) {
            if( !disposed) {
                if (disposing) {
                    //Dispose managed and unmanaged resources
                    Close();
                }
                this.disposed = true;
                base.Dispose(disposing);                
            }            
        }

        /// <devdoc>
        ///    <para>
        ///       Frees any resources associated with this component.
        ///    </para>
        /// </devdoc>
        public void Close() {
            if (Associated) {
                if (haveProcessHandle) {
                    StopWatchingForExit();
                    Debug.WriteLineIf(processTracing.TraceVerbose, "Process - CloseHandle(process) in Close()");
                    m_processHandle.Close();
                    m_processHandle = null;
                    haveProcessHandle = false;
                }
                haveProcessId = false;
                isRemoteMachine = false;
                machineName = ".";
                raisedOnExited = false;

                //Don't call close on the Readers and writers
                //since they might be referenced by somebody else while the 
                //process is still alive but this method called.
                standardOutput = null;
                standardInput = null;
                standardError = null;
	

                Refresh();
            }
        }

        /// <devdoc>
        ///     Helper method for checking preconditions when accessing properties.
        /// </devdoc>
        /// <internalonly/>
        void EnsureState(State state) {
            if ((state & State.IsWin2k) != (State)0) {
                    throw new PlatformNotSupportedException(SR.GetString(SR.Win2kRequired));
            }

            if ((state & State.IsNt) != (State)0) {
                    throw new PlatformNotSupportedException(SR.GetString(SR.WinNTRequired));
            }

            if ((state & State.Associated) != (State)0)
                if (!Associated)
                    throw new InvalidOperationException(SR.GetString(SR.NoAssociatedProcess));

            if ((state & State.HaveId) != (State)0) {
                if (!haveProcessId) {
                    EnsureState(State.Associated);
                    throw new InvalidOperationException(SR.GetString(SR.ProcessIdRequired));
                }
            }

            if ((state & State.IsLocal) != (State)0 && isRemoteMachine) {
                    throw new NotSupportedException(SR.GetString(SR.NotSupportedRemote));
            }
            
            if ((state & State.HaveProcessInfo) != (State)0) {
                throw new InvalidOperationException(SR.GetString(SR.NoProcessInfo));
            }

            if ((state & State.Exited) != (State)0) {
                if (!HasExited) {
                    throw new InvalidOperationException(SR.GetString(SR.WaitTillExit));
                }
                
                if (!haveProcessHandle) {
                    throw new InvalidOperationException(SR.GetString(SR.NoProcessHandle));
                }
            }
        }
            
        /// <devdoc>
        ///     Make sure we are watching for a process exit.
        /// </devdoc>
        /// <internalonly/>
        void EnsureWatchingForExit() {
            if (!watchingForExit) {
                Debug.Assert(haveProcessHandle, "Process.EnsureWatchingForExit called with no process handle");
                Debug.Assert(Associated, "Process.EnsureWatchingForExit called with no associated process");
                watchingForExit = true;
                try {
                    this.waitHandle = new ProcessWaitHandle(m_processHandle);
                    this.registeredWaitHandle = ThreadPool.RegisterWaitForSingleObject(this.waitHandle,
                        new WaitOrTimerCallback(this.CompletionCallback), null, -1, true);                    
                }
                catch {
                    watchingForExit = false;
                    throw;
                }
            }
        }


        /// <devdoc>
        ///    <para>
        ///       Returns a new <see cref='System.Diagnostics.Process'/>
        ///       component and associates it with the current active process.
        ///    </para>
        /// </devdoc>
        public static Process GetCurrentProcess() {
            return new Process(".", false, NativeMethods.GetCurrentProcessId(), null);
        }

        /// <devdoc>
        ///    <para>
        ///       Raises the <see cref='System.Diagnostics.Process.Exited'/> event.
        ///    </para>
        /// </devdoc>
        protected void OnExited() {
            EventHandler exited = onExited;
            if (exited != null) {
                if (this.SynchronizingObject != null && this.SynchronizingObject.InvokeRequired)
                    this.SynchronizingObject.BeginInvoke(exited, new object[]{this, EventArgs.Empty});
                else                        
                   exited(this, EventArgs.Empty);                
            }               
        }

        /// <devdoc>
        ///     Gets a short-term handle to the process, with the given access.  
        ///     If a handle is stored in current process object, then use it.
        ///     Note that the handle we stored in current process object will have all access we need.
        /// </devdoc>
        /// <internalonly/>
        SafeProcessHandle GetProcessHandle(int access, bool throwIfExited) {
            Debug.WriteLineIf(processTracing.TraceVerbose, "GetProcessHandle(access = 0x" + access.ToString("X8", CultureInfo.InvariantCulture) + ", throwIfExited = " + throwIfExited + ")");
#if DEBUG
            if (processTracing.TraceVerbose) {
                StackFrame calledFrom = new StackTrace(true).GetFrame(0);
                Debug.WriteLine("   called from " + calledFrom.GetFileName() + ", line " + calledFrom.GetFileLineNumber());
            }
#endif
            if (haveProcessHandle) {
                if (throwIfExited) {
                    // Since haveProcessHandle is true, we know we have the process handle
                    // open with at least SYNCHRONIZE access, so we can wait on it with 
                    // zero timeout to see if the process has exited.
                    ProcessWaitHandle waitHandle = null;
                    try {
                        waitHandle = new ProcessWaitHandle(m_processHandle);             
                        if (waitHandle.WaitOne(0, false)) {
                            if (haveProcessId)
                                throw new InvalidOperationException(SR.GetString(SR.ProcessHasExited, processId.ToString(CultureInfo.CurrentCulture)));
                            else
                                throw new InvalidOperationException(SR.GetString(SR.ProcessHasExitedNoId));
                        }
                    }
                    finally {
                        if( waitHandle != null) {
                            waitHandle.Close();
                        }
                    }            
                }
                return m_processHandle;
            }
            else {
                EnsureState(State.HaveId | State.IsLocal);
                SafeProcessHandle handle = SafeProcessHandle.InvalidHandle;
                IntPtr pseudohandle = NativeMethods.GetCurrentProcess();
                // Get a real handle
                if (!NativeMethods.DuplicateHandle (new HandleRef(this, pseudohandle), 
                                                    new HandleRef(this, pseudohandle), 
                                                    new HandleRef(this, pseudohandle), 
                                                    out handle,
                                                    0, 
                                                    false, 
                                                    NativeMethods.DUPLICATE_SAME_ACCESS | 
                                                    NativeMethods.DUPLICATE_CLOSE_SOURCE)) {
                    throw new Win32Exception();
                }
                if (throwIfExited && (access & NativeMethods.PROCESS_QUERY_INFORMATION) != 0) {         
                    if (NativeMethods.GetExitCodeProcess(handle, out exitCode) && exitCode != NativeMethods.STILL_ACTIVE) {
                        throw new InvalidOperationException(SR.GetString(SR.ProcessHasExited, processId.ToString(CultureInfo.CurrentCulture)));
                    }
                }
                return handle;
            }

        }

        /// <devdoc>
        ///     Gets a short-term handle to the process, with the given access.  If a handle exists,
        ///     then it is reused.  If the process has exited, it throws an exception.
        /// </devdoc>
        /// <internalonly/>
        SafeProcessHandle GetProcessHandle(int access) {
            return GetProcessHandle(access, true);
        }

        /// <devdoc>
        ///     Opens a long-term handle to the process, with all access.  If a handle exists,
        ///     then it is reused.  If the process has exited, it throws an exception.
        /// </devdoc>
        /// <internalonly/>
        SafeProcessHandle OpenProcessHandle() {
            if (!haveProcessHandle) {
                //Cannot open a new process handle if the object has been disposed, since finalization has been suppressed.            
                if (this.disposed) {
                    throw new ObjectDisposedException(GetType().Name);
                }
                        
                SetProcessHandle(GetProcessHandle(NativeMethods.PROCESS_ALL_ACCESS));
            }                
            return m_processHandle;
        }

        /// <devdoc>
        ///     Raise the Exited event, but make sure we don't do it more than once.
        /// </devdoc>
        /// <internalonly/>
        void RaiseOnExited() {
            if (!raisedOnExited) {
                lock (this) {
                    if (!raisedOnExited) {
                        raisedOnExited = true;
                        OnExited();
                    }
                }
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Discards any information about the associated process
        ///       that has been cached inside the process component. After <see cref='System.Diagnostics.Process.Refresh'/> is called, the
        ///       first request for information for each property causes the process component
        ///       to obtain a new value from the associated process.
        ///    </para>
        /// </devdoc>
        public void Refresh() {
            processInfo = null;
            mainWindowTitle = null;
            exited = false;
            signaled = false;
            haveMainWindow = false;
            haveWorkingSetLimits = false;
            haveProcessorAffinity = false;
            havePriorityClass = false;
            haveExitTime = false;
            haveResponding = false;
            havePriorityBoostEnabled = false;
        }

        /// <devdoc>
        ///     Helper to associate a process handle with this component.
        /// </devdoc>
        /// <internalonly/>
        void SetProcessHandle(SafeProcessHandle processHandle) {
            this.m_processHandle = processHandle;
            this.haveProcessHandle = true;
            if (watchForExit) {
                EnsureWatchingForExit();
            }
        }

        /// <devdoc>
        ///     Helper to associate a process id with this component.
        /// </devdoc>
        /// <internalonly/>
        void SetProcessId(int processId) {
            this.processId = processId;
            this.haveProcessId = true;
        }


        /// <devdoc>
        ///    <para>
        ///       Starts a process specified by the <see cref='System.Diagnostics.Process.StartInfo'/> property of this <see cref='System.Diagnostics.Process'/>
        ///       component and associates it with the
        ///    <see cref='System.Diagnostics.Process'/> . If a process resource is reused 
        ///       rather than started, the reused process is associated with this <see cref='System.Diagnostics.Process'/>
        ///       component.
        ///    </para>
        /// </devdoc>
        public bool Start() {
            Close();
            ProcessStartInfo startInfo = StartInfo;
            if (startInfo.FileName.Length == 0) 
                throw new InvalidOperationException(SR.GetString(SR.FileNameMissing));

            if (startInfo.UseShellExecute) {
                throw new InvalidOperationException(SR.GetString(SR.net_perm_invalid_val, "StartInfo.UseShellExecute", true));
            } else {
                return StartWithCreateProcess(startInfo);
            }
        }


        private static void CreatePipeWithSecurityAttributes(out SafeFileHandle hReadPipe, out SafeFileHandle hWritePipe, NativeMethods.SECURITY_ATTRIBUTES lpPipeAttributes, int nSize) {
            bool ret = NativeMethods.CreatePipe(out hReadPipe, out hWritePipe, lpPipeAttributes, nSize);
            if (!ret || hReadPipe.IsInvalid || hWritePipe.IsInvalid) {
                throw new Win32Exception();
            }
        }

        // Using synchronous Anonymous pipes for process input/output redirection means we would end up 
        // wasting a worker threadpool thread per pipe instance. Overlapped pipe IO is desirable, since 
        // it will take advantage of the NT IO completion port infrastructure. But we can't really use 
        // Overlapped I/O for process input/output as it would break Console apps (managed Console class 
        // methods such as WriteLine as well as native CRT functions like printf) which are making an
        // assumption that the console standard handles (obtained via GetStdHandle()) are opened
        // for synchronous I/O and hence they can work fine with ReadFile/WriteFile synchrnously!
        private void CreatePipe(out SafeFileHandle parentHandle, out SafeFileHandle childHandle, bool parentInputs) {
            NativeMethods.SECURITY_ATTRIBUTES securityAttributesParent = new NativeMethods.SECURITY_ATTRIBUTES();
            securityAttributesParent.bInheritHandle = true;
            
            SafeFileHandle hTmp = null;
            try {
                if (parentInputs) {
                    CreatePipeWithSecurityAttributes(out childHandle, out hTmp, securityAttributesParent, 0);                                                          
                } 
                else {
                    CreatePipeWithSecurityAttributes(out hTmp, 
                                                          out childHandle, 
                                                          securityAttributesParent, 
                                                          0);                                                                              
                }
                // Duplicate the parent handle to be non-inheritable so that the child process 
                // doesn't have access. This is done for correctness sake, exact reason is unclear.
                // One potential theory is that child process can do something brain dead like 
                // closing the parent end of the pipe and there by getting into a blocking situation
                // as parent will not be draining the pipe at the other end anymore. 
                if (!NativeMethods.DuplicateHandle(new HandleRef(this, NativeMethods.GetCurrentProcess()), 
                                                                   hTmp,
                                                                   new HandleRef(this, NativeMethods.GetCurrentProcess()), 
                                                                   out parentHandle,
                                                                   0, 
                                                                   false, 
                                                                   NativeMethods.DUPLICATE_SAME_ACCESS)) {                                                                       
                    throw new Win32Exception();
                }
            }
            finally {
                if( hTmp != null && !hTmp.IsInvalid) {
                    hTmp.Close();
                }
            }
        }            

        private static StringBuilder BuildCommandLine(string executableFileName, string arguments) {
            // Construct a StringBuilder with the appropriate command line
            // to pass to CreateProcess.  If the filename isn't already 
            // in quotes, we quote it here.  This prevents some security
            // problems (it specifies exactly which part of the string
            // is the file to execute).
            StringBuilder commandLine = new StringBuilder();
            string fileName = executableFileName.Trim();
            bool fileNameIsQuoted = (fileName.StartsWith("\"", StringComparison.Ordinal) && fileName.EndsWith("\"", StringComparison.Ordinal));
            if (!fileNameIsQuoted) { 
                commandLine.Append("\"");
            }
            
            commandLine.Append(fileName);
            
            if (!fileNameIsQuoted) {
                commandLine.Append("\"");
            }
            
            if (!String.IsNullOrEmpty(arguments)) {
                commandLine.Append(" ");
                commandLine.Append(arguments);                
            }                        

            return commandLine;
        }
        
        private bool StartWithCreateProcess(ProcessStartInfo startInfo) {
            if( startInfo.StandardOutputEncoding != null && !startInfo.RedirectStandardOutput) {
                throw new InvalidOperationException(SR.GetString(SR.StandardOutputEncodingNotAllowed));
            }

            if( startInfo.StandardErrorEncoding != null && !startInfo.RedirectStandardError) {
                throw new InvalidOperationException(SR.GetString(SR.StandardErrorEncodingNotAllowed));
            }            
            
            // See knowledge base article Q190351 for an explanation of the following code.  Noteworthy tricky points:
            //    * The handles are duplicated as non-inheritable before they are passed to CreateProcess so
            //      that the child process can not close them
            //    * CreateProcess allows you to redirect all or none of the standard IO handles, so we use
            //      GetStdHandle for the handles that are not being redirected

            //Cannot start a new process and store its handle if the object has been disposed, since finalization has been suppressed.            
            if (this.disposed) {
                throw new ObjectDisposedException(GetType().Name);
            }

            StringBuilder commandLine = BuildCommandLine(startInfo.FileName, startInfo.Arguments);

            NativeMethods.STARTUPINFO startupInfo = new NativeMethods.STARTUPINFO();
            SafeNativeMethods.PROCESS_INFORMATION processInfo = new SafeNativeMethods.PROCESS_INFORMATION();
            SafeProcessHandle procSH = new SafeProcessHandle();
            SafeThreadHandle threadSH = new SafeThreadHandle();
            bool retVal;
            int errorCode = 0;
            // handles used in parent process
            SafeFileHandle standardInputWritePipeHandle = null;
            SafeFileHandle standardOutputReadPipeHandle = null;
            SafeFileHandle standardErrorReadPipeHandle = null;
            
            GCHandle environmentHandle = new GCHandle();            
            try {
                // set up the streams
                if (startInfo.RedirectStandardInput || startInfo.RedirectStandardOutput || startInfo.RedirectStandardError) {                        
                    if (startInfo.RedirectStandardInput) {
                        CreatePipe(out standardInputWritePipeHandle, out startupInfo.hStdInput, true);
                    } 
                    else {
                        startupInfo.hStdInput  =  new SafeFileHandle(NativeMethods.GetStdHandle(NativeMethods.STD_INPUT_HANDLE), false);
                    }
    
                    if (startInfo.RedirectStandardOutput) {                        
                        CreatePipe(out standardOutputReadPipeHandle, out startupInfo.hStdOutput, false);
                    } 
                    else {
                        startupInfo.hStdOutput = new SafeFileHandle(NativeMethods.GetStdHandle(NativeMethods.STD_OUTPUT_HANDLE), false);
                    }
    
                    if (startInfo.RedirectStandardError) {
                        CreatePipe(out standardErrorReadPipeHandle, out startupInfo.hStdError, false);
                    } 
                    else {
                        startupInfo.hStdError = new SafeFileHandle(NativeMethods.GetStdHandle(NativeMethods.STD_ERROR_HANDLE), false);
                    }
    
                    startupInfo.dwFlags = NativeMethods.STARTF_USESTDHANDLES;
                }
    
                // set up the creation flags paramater
                int creationFlags = 0;

                // set up the environment block parameter
                IntPtr environmentPtr = (IntPtr)0;
                if (startInfo.environmentVariables != null) {
                    bool unicode = false;
                    
                    byte[] environmentBytes = EnvironmentBlock.ToByteArray(startInfo.environmentVariables, unicode);
                    environmentHandle = GCHandle.Alloc(environmentBytes, GCHandleType.Pinned);
                    environmentPtr = environmentHandle.AddrOfPinnedObject();
                }

                string workingDirectory = startInfo.WorkingDirectory;
                if (workingDirectory == string.Empty)
                    workingDirectory = Environment.CurrentDirectory;

                    RuntimeHelpers.PrepareConstrainedRegions();
                    try {} finally {
                       retVal = NativeMethods.CreateProcess (
                               null,               // we don't need this since all the info is in commandLine
                               commandLine,        // pointer to the command line string
                               null,               // pointer to process security attributes, we don't need to inheriat the handle
                               null,               // pointer to thread security attributes
                               true,               // handle inheritance flag
                               creationFlags,      // creation flags
                               environmentPtr,     // pointer to new environment block
                               workingDirectory,   // pointer to current directory name
                               startupInfo,        // pointer to STARTUPINFO
                               processInfo         // pointer to PROCESS_INFORMATION
                           );
                       if (!retVal)                            
                              errorCode = Marshal.GetLastWin32Error();
                       if ( processInfo.hProcess!= (IntPtr)0 && processInfo.hProcess!= (IntPtr)NativeMethods.INVALID_HANDLE_VALUE)
                           procSH.InitialSetHandle(processInfo.hProcess);  
                       if ( processInfo.hThread != (IntPtr)0 && processInfo.hThread != (IntPtr)NativeMethods.INVALID_HANDLE_VALUE)
                          threadSH.InitialSetHandle(processInfo.hThread);                    
                    }
                    if (!retVal) {
                       if( errorCode == NativeMethods.ERROR_BAD_EXE_FORMAT) {
                          throw new Win32Exception(errorCode, SR.GetString(SR.InvalidApplication));
                       }
                        throw new Win32Exception(errorCode);
                    }
            }
            finally {
                // free environment block
                if (environmentHandle.IsAllocated) {
                    environmentHandle.Free();   
                }

                startupInfo.Dispose();
            }

            if (startInfo.RedirectStandardInput) {
                standardInput = new StreamWriter(new FileStream(standardInputWritePipeHandle, FileAccess.Write, 4096, false), Encoding.GetEncoding(NativeMethods.GetConsoleCP()), 4096);
                standardInput.AutoFlush = true;
            }
            if (startInfo.RedirectStandardOutput) {
                Encoding enc = (startInfo.StandardOutputEncoding != null) ? startInfo.StandardOutputEncoding : Encoding.GetEncoding(NativeMethods.GetConsoleOutputCP());
                standardOutput = new StreamReader(new FileStream(standardOutputReadPipeHandle, FileAccess.Read, 4096, false), enc, true, 4096);
            }
            if (startInfo.RedirectStandardError) {
                Encoding enc = (startInfo.StandardErrorEncoding != null) ? startInfo.StandardErrorEncoding : Encoding.GetEncoding(NativeMethods.GetConsoleOutputCP());
                standardError = new StreamReader(new FileStream(standardErrorReadPipeHandle, FileAccess.Read, 4096, false), enc, true, 4096);
            }
            
            bool ret = false;
            if (!procSH.IsInvalid) {
                SetProcessHandle(procSH);
                SetProcessId(processInfo.dwProcessId);
                threadSH.Close();
                ret = true;
            }

            return ret;

        }


        /// <devdoc>
        ///    <para>
        ///       Starts a process resource by specifying the name of a
        ///       document or application file. Associates the process resource with a new <see cref='System.Diagnostics.Process'/>
        ///       component.
        ///    </para>
        /// </devdoc>
        public static Process Start(string fileName) {
            return Start(new ProcessStartInfo(fileName));
        }

        /// <devdoc>
        ///    <para>
        ///       Starts a process resource by specifying the name of an
        ///       application and a set of command line arguments. Associates the process resource
        ///       with a new <see cref='System.Diagnostics.Process'/>
        ///       component.
        ///    </para>
        /// </devdoc>
        public static Process Start(string fileName, string arguments) {
            return Start(new ProcessStartInfo(fileName, arguments));
        }

        /// <devdoc>
        ///    <para>
        ///       Starts a process resource specified by the process start
        ///       information passed in, for example the file name of the process to start.
        ///       Associates the process resource with a new <see cref='System.Diagnostics.Process'/>
        ///       component.
        ///    </para>
        /// </devdoc>
        public static Process Start(ProcessStartInfo startInfo) {
            Process process = new Process();
            if (startInfo == null) throw new ArgumentNullException("startInfo");
            process.StartInfo = startInfo;
            if (process.Start()) {
                return process;
            }
            return null;
        }

        /// <devdoc>
        ///    <para>
        ///       Stops the
        ///       associated process immediately.
        ///    </para>
        /// </devdoc>
        public void Kill() {
            SafeProcessHandle handle = null;
            try {
                handle = GetProcessHandle(NativeMethods.PROCESS_TERMINATE);
                if (!NativeMethods.TerminateProcess(handle, -1))
                    throw new Win32Exception();
            }
            finally {
                ReleaseProcessHandle(handle);
            }
        }

        /// <devdoc>
        ///     Make sure we are not watching for process exit.
        /// </devdoc>
        /// <internalonly/>
        void StopWatchingForExit() {
            if (watchingForExit) {
                lock (this) {
                    if (watchingForExit) {
                        watchingForExit = false;
                        registeredWaitHandle.Unregister(null);                
                        waitHandle = null;
                        registeredWaitHandle = null;
                    }
                }
            }
        }

        public override string ToString() {
                return base.ToString();
        }

        /// <devdoc>
        ///    <para>
        ///       Instructs the <see cref='System.Diagnostics.Process'/> component to wait the specified number of milliseconds for the associated process to exit.
        ///    </para>
        /// </devdoc>
        public bool WaitForExit(int milliseconds) {
            SafeProcessHandle handle = null;
	     bool exited;
            ProcessWaitHandle processWaitHandle = null;
            try {
                handle = GetProcessHandle(NativeMethods.SYNCHRONIZE, false);                
                if (handle.IsInvalid) {
                    exited = true;
                }
                else {
                    processWaitHandle = new ProcessWaitHandle(handle);
                    if( processWaitHandle.WaitOne(milliseconds, false)) {
                        exited = true;
                        signaled = true;
                    }
                    else {
                        exited = false;
                        signaled = false;
                    }
                }
            }
            finally {
                if( processWaitHandle != null) {
                    processWaitHandle.Close();
                }

                // If we have a hard timeout, we cannot wait for the streams
                if( output != null && milliseconds == Int32.MaxValue) {
                    output.WaitUtilEOF();
                }

                if( error != null && milliseconds == Int32.MaxValue) {
                    error.WaitUtilEOF();
                }

                ReleaseProcessHandle(handle);

            }
            
            if (exited && watchForExit) {
                RaiseOnExited();
            }
			
            return exited;
        }

        /// <devdoc>
        ///    <para>
        ///       Instructs the <see cref='System.Diagnostics.Process'/> component to wait
        ///       indefinitely for the associated process to exit.
        ///    </para>
        /// </devdoc>
        public void WaitForExit() {
            WaitForExit(Int32.MaxValue);
        }


        // Support for working asynchronously with streams
        /// <devdoc>
        /// <para>
        /// Instructs the <see cref='System.Diagnostics.Process'/> component to start
        /// reading the StandardOutput stream asynchronously. The user can register a callback
        /// that will be called when a line of data terminated by \n,\r or \r\n is reached, or the end of stream is reached
        /// then the remaining information is returned. The user can add an event handler to OutputDataReceived.
        /// </para>
        /// </devdoc>
        [System.Runtime.InteropServices.ComVisible(false)]        
        public void BeginOutputReadLine() {

            if(outputStreamReadMode == StreamReadMode.undefined) {
                outputStreamReadMode = StreamReadMode.asyncMode;
            }
            else if (outputStreamReadMode != StreamReadMode.asyncMode) {
                throw new InvalidOperationException(SR.GetString(SR.CantMixSyncAsyncOperation));                    
            }
            
            if (pendingOutputRead)
                throw new InvalidOperationException(SR.GetString(SR.PendingAsyncOperation));

            pendingOutputRead = true;
            // We can't detect if there's a pending sychronous read, tream also doesn't.
            if (output == null) {
                if (standardOutput == null) {
                    throw new InvalidOperationException(SR.GetString(SR.CantGetStandardOut));
                }

                Stream s = standardOutput.BaseStream;
                output = new AsyncStreamReader(this, s, new UserCallBack(this.OutputReadNotifyUser), standardOutput.CurrentEncoding);
            }
            output.BeginReadLine();
        }


        /// <devdoc>
        /// <para>
        /// Instructs the <see cref='System.Diagnostics.Process'/> component to start
        /// reading the StandardError stream asynchronously. The user can register a callback
        /// that will be called when a line of data terminated by \n,\r or \r\n is reached, or the end of stream is reached
        /// then the remaining information is returned. The user can add an event handler to ErrorDataReceived.
        /// </para>
        /// </devdoc>
        [System.Runtime.InteropServices.ComVisible(false)]        
        public void BeginErrorReadLine() {

            if(errorStreamReadMode == StreamReadMode.undefined) {
                errorStreamReadMode = StreamReadMode.asyncMode;
            }
            else if (errorStreamReadMode != StreamReadMode.asyncMode) {
                throw new InvalidOperationException(SR.GetString(SR.CantMixSyncAsyncOperation));                    
            }
            
            if (pendingErrorRead) {
                throw new InvalidOperationException(SR.GetString(SR.PendingAsyncOperation));
            }

            pendingErrorRead = true;
            // We can't detect if there's a pending sychronous read, stream also doesn't.
            if (error == null) {
                if (standardError == null) {
                    throw new InvalidOperationException(SR.GetString(SR.CantGetStandardError));
                }

                Stream s = standardError.BaseStream;
                error = new AsyncStreamReader(this, s, new UserCallBack(this.ErrorReadNotifyUser), standardError.CurrentEncoding);
            }
            error.BeginReadLine();
        }

        /// <devdoc>
        /// <para>
        /// Instructs the <see cref='System.Diagnostics.Process'/> component to cancel the asynchronous operation
        /// specified by BeginOutputReadLine().
        /// </para>
        /// </devdoc>
        [System.Runtime.InteropServices.ComVisible(false)]
        public void CancelOutputRead() {        
            if (output != null) {
                output.CancelOperation();
            }
            else {
                throw new InvalidOperationException(SR.GetString(SR.NoAsyncOperation));
            }

            pendingOutputRead = false;
        }

        /// <devdoc>
        /// <para>
        /// Instructs the <see cref='System.Diagnostics.Process'/> component to cancel the asynchronous operation
        /// specified by BeginErrorReadLine().
        /// </para>
        /// </devdoc>
        [System.Runtime.InteropServices.ComVisible(false)]        
        public void CancelErrorRead() {
            if (error != null) {
                error.CancelOperation();
            }
            else {
                throw new InvalidOperationException(SR.GetString(SR.NoAsyncOperation));
            }

            pendingErrorRead = false;
        }

        internal void OutputReadNotifyUser(String data) {
            // To avoid race between remove handler and raising the event
            DataReceivedEventHandler outputDataReceived = OutputDataReceived;
            if (outputDataReceived != null) {
                DataReceivedEventArgs e = new DataReceivedEventArgs(data);
                if (SynchronizingObject != null && SynchronizingObject.InvokeRequired) {
                    SynchronizingObject.Invoke(outputDataReceived, new object[] {this, e});
                }
                else {
                    outputDataReceived(this,e);  // Call back to user informing data is available.
                }
            }
        }

        internal void ErrorReadNotifyUser(String data) {
            // To avoid race between remove handler and raising the event
            DataReceivedEventHandler errorDataReceived = ErrorDataReceived;
            if (errorDataReceived != null) {
                DataReceivedEventArgs e = new DataReceivedEventArgs(data);
                if (SynchronizingObject != null && SynchronizingObject.InvokeRequired) {
                    SynchronizingObject.Invoke(errorDataReceived, new object[] {this, e});
                }
                else {
                    errorDataReceived(this,e); // Call back to user informing data is available.
                }
            }
        }

        /// <summary>
        ///     A desired internal state.
        /// </summary>
        /// <internalonly/>
        enum State {
            HaveId = 0x1,
            IsLocal = 0x2,
            IsNt = 0x4,
            HaveProcessInfo = 0x8,
            Exited = 0x10,
            Associated = 0x20,
            IsWin2k = 0x40,
            HaveNtProcessInfo = HaveProcessInfo | IsNt
        }
    }

    /// <devdoc>
    ///     This data structure contains information about a process that is collected
    ///     in bulk by querying the operating system.  The reason to make this a separate
    ///     structure from the process component is so that we can throw it away all at once
    ///     when Refresh is called on the component.
    /// </devdoc>
    /// <internalonly/>
    internal class ProcessInfo {
        public ArrayList threadInfoList = new ArrayList();
        public int basePriority;
        public string processName;
        public int processId;
        public int handleCount;
        public long poolPagedBytes;
        public long poolNonpagedBytes;
        public long virtualBytes;
        public long virtualBytesPeak;
        public long workingSetPeak;
        public long workingSet;
        public long pageFileBytesPeak;
        public long pageFileBytes;
        public long privateBytes;
        public int mainModuleId; // used only for win9x - id is only for use with CreateToolHelp32
        public int sessionId; 
    }

    /// <devdoc>
    ///     This data structure contains information about a thread in a process that
    ///     is collected in bulk by querying the operating system.  The reason to
    ///     make this a separate structure from the ProcessThread component is so that we
    ///     can throw it away all at once when Refresh is called on the component.
    /// </devdoc>
    /// <internalonly/>
    internal class ThreadInfo {
        public int threadId;
        public int processId;
        public int basePriority;
        public int currentPriority;
        public IntPtr startAddress;
        public ThreadState threadState;
    }

    /// <devdoc>
    ///     This data structure contains information about a module in a process that
    ///     is collected in bulk by querying the operating system.  The reason to
    ///     make this a separate structure from the ProcessModule component is so that we
    ///     can throw it away all at once when Refresh is called on the component.
    /// </devdoc>
    /// <internalonly/>
    internal class ModuleInfo {
        public string baseName;
        public string fileName;
        public IntPtr baseOfDll;
        public IntPtr entryPoint;
        public int sizeOfImage;
        public int Id; // used only on win9x - for matching up with ProcessInfo.mainModuleId
    }

    internal static class EnvironmentBlock {
        public static byte[] ToByteArray(StringDictionary sd, bool unicode) {
            // get the keys
            string[] keys = new string[sd.Count];
            byte[] envBlock = null;
            sd.Keys.CopyTo(keys, 0);
            
            // get the values
            string[] values = new string[sd.Count];
            sd.Values.CopyTo(values, 0);
            
            // sort both by the keys
            // Windows 2000 requires the environment block to be sorted by the key
            // It will first converting the case the strings and do ordinal comparison.
            Array.Sort(keys, values, OrdinalCaseInsensitiveComparer.Default);

            // create a list of null terminated "key=val" strings
            StringBuilder stringBuff = new StringBuilder();
            for (int i = 0; i < sd.Count; ++ i) {
                stringBuff.Append(keys[i]);
                stringBuff.Append('=');
                stringBuff.Append(values[i]);
                stringBuff.Append('\0');
            }
            // an extra null at the end indicates end of list.
            stringBuff.Append('\0');
                        
            if( unicode) {
                envBlock = Encoding.Unicode.GetBytes(stringBuff.ToString());                        
            }
            else
                envBlock = Encoding.Default.GetBytes(stringBuff.ToString());

            if (envBlock.Length > UInt16.MaxValue)
                throw new InvalidOperationException(SR.GetString(SR.EnvironmentBlockTooLong, envBlock.Length));

            return envBlock;
        }        
    }

    internal class OrdinalCaseInsensitiveComparer : IComparer {
        internal static readonly OrdinalCaseInsensitiveComparer Default = new OrdinalCaseInsensitiveComparer();
        
        public int Compare(Object a, Object b) {
            String sa = a as String;
            String sb = b as String;
            if (sa != null && sb != null) {
                return String.CompareOrdinal(sa.ToUpperInvariant(), sb.ToUpperInvariant()); 
            }
            return Comparer.Default.Compare(a,b);
        }
    }

    internal class ProcessThreadTimes {
        internal long create;
        internal long exit; 
        internal long kernel; 
        internal long user;

        public DateTime StartTime {   
            get {             
                return DateTime.FromFileTime(create);
            }
        }

        public DateTime ExitTime {
            get {
                return DateTime.FromFileTime(exit);
            }
        }

        public TimeSpan PrivilegedProcessorTime {
            get {
                return new TimeSpan(kernel);
            }
        }

        public TimeSpan UserProcessorTime {
            get {
                return new TimeSpan(user);
            }
        }
        
        public TimeSpan TotalProcessorTime {
            get {
                return new TimeSpan(user + kernel);
            }
        }
    }


}
