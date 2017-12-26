//------------------------------------------------------------------------------
// <copyright file="_LoggingObject.cs" company="Microsoft">
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

//
//  We have function based stack and thread based logging of basic behavior.  We
//  also now have the ability to run a "watch thread" which does basic hang detection
//  and error-event based logging.   The logging code buffers the callstack/picture
//  of all COMNET threads, and upon error from an assert or a hang, it will open a file
//  and dump the snapsnot.  Future work will allow this to be configed by registry and
//  to use Runtime based logging.  We'd also like to have different levels of logging.
//

namespace System.Net {
    using System.Collections;
    using System.Collections.Generic;
    using System.IO;
    using System.Threading;
    using System.Diagnostics;
    using System.Security.Permissions;
    using System.Security.Principal;
    using System.Security;
    using Microsoft.Win32;
    using System.Runtime.ConstrainedExecution;
    using System.Globalization;
    using System.Configuration;

    //
    // BaseLoggingObject - used to disable logging,
    //  this is just a base class that does nothing.
    //

    internal class BaseLoggingObject {

        internal BaseLoggingObject() {
        }

        internal virtual void EnterFunc(string funcname) {
        }

        internal virtual void LeaveFunc(string funcname) {
        }

        internal virtual void DumpArrayToConsole() {
        }

        internal virtual void PrintLine(string msg) {
        }

        internal virtual void DumpArray(bool shouldClose) {
        }

        internal virtual void DumpArrayToFile(bool shouldClose) {
        }

        internal virtual void Flush() {
        }

        internal virtual void Flush(bool close) {
        }

        internal virtual void LoggingMonitorTick() {
        }

        internal virtual void Dump(byte[] buffer) {
        }

        internal virtual void Dump(byte[] buffer, int length) {
        }

        internal virtual void Dump(byte[] buffer, int offset, int length) {
        }

        internal virtual void Dump(IntPtr pBuffer, int offset, int length) {
        }

    } // class BaseLoggingObject



    [Flags]
    internal enum ThreadKinds
    {
        Unknown        = 0x0000,

        // Mutually exclusive.
        User           = 0x0001,     // Thread has entered via an API.
        System         = 0x0002,     // Thread has entered via a system callback (e.g. completion port) or is our own thread.

        // Mutually exclusive.
        Sync           = 0x0004,     // Thread should block.
        Async          = 0x0008,     // Thread should not block.

        // Mutually exclusive, not always known for a user thread.  Never changes.
        Timer          = 0x0010,     // Thread is the timer thread.  (Can't call user code.)
        CompletionPort = 0x0020,     // Thread is a ThreadPool completion-port thread.
        Worker         = 0x0040,     // Thread is a ThreadPool worker thread.
        Finalization   = 0x0080,     // Thread is the finalization thread.
        Other          = 0x0100,     // Unknown source.

        OwnerMask      = User | System,
        SyncMask       = Sync | Async,
        SourceMask     = Timer | CompletionPort | Worker | Finalization | Other,

        // Useful "macros"
        SafeSources    = SourceMask & ~(Timer | Finalization),  // Methods that "unsafe" sources can call must be explicitly marked.
        ThreadPool     = CompletionPort | Worker,               // Like Thread.CurrentThread.IsThreadPoolThread
    }

    /// <internalonly/>
    /// <devdoc>
    /// </devdoc>
    internal static class GlobalLog {

        //
        // Logging Initalization - I need to disable Logging code, and limit
        //  the effect it has when it is dissabled, so I use a bool here.
        //
        //  This can only be set when the logging code is built and enabled.
        //  By specifing the "CSC_DEFINES=/D:TRAVE" in the build environment,
        //  this code will be built and then checks against an enviroment variable
        //  and a BooleanSwitch to see if any of the two have enabled logging.
        //

        private static BaseLoggingObject Logobject = GlobalLog.LoggingInitialize();
        [ReliabilityContract(Consistency.MayCorruptAppDomain, Cer.None)]
        private static BaseLoggingObject LoggingInitialize() {

#if DEBUG
            if (GetSwitchValue("SystemNetLogging", "System.Net logging module", false) &&
                GetSwitchValue("SystemNetLog_ConnectionMonitor", "System.Net connection monitor thread", false)) {
                InitConnectionMonitor();
            }
#endif // DEBUG
            return new BaseLoggingObject();
        }



#if TRAVE || DEBUG
        private static bool GetSwitchValue(string switchName, string switchDescription, bool defaultValue) {
            BooleanSwitch theSwitch = new BooleanSwitch(switchName, switchDescription);
            new EnvironmentPermission(PermissionState.Unrestricted).Assert();
            try {
                if (theSwitch.Enabled) {
                    return true;
                }
                string environmentVar = Environment.GetEnvironmentVariable(switchName);
                defaultValue = environmentVar!=null && environmentVar.Trim()=="1";
            }
            catch (ConfigurationException) { }
            finally {
                EnvironmentPermission.RevertAssert();
            }
            return defaultValue;
        }
#endif // TRAVE || DEBUG


        // Enables thread tracing, detects mis-use of threads.
#if DEBUG
        [ThreadStatic]
        private static Stack<ThreadKinds> t_ThreadKindStack;

        private static Stack<ThreadKinds> ThreadKindStack
        {
            get
            {
                if (t_ThreadKindStack == null)
                {
                    t_ThreadKindStack = new Stack<ThreadKinds>();
                }
                return t_ThreadKindStack;
            }
        }
#endif
                
        internal static ThreadKinds CurrentThreadKind
        {
            get
            {
#if DEBUG
                return ThreadKindStack.Count > 0 ? ThreadKindStack.Peek() : ThreadKinds.Other;
#else
                return ThreadKinds.Unknown;
#endif
            }
        }

#if DEBUG
        // ifdef'd instead of conditional since people are forced to handle the return value.
        // [Conditional("DEBUG")]
        [ReliabilityContract(Consistency.MayCorruptAppDomain, Cer.None)]
        internal static IDisposable SetThreadKind(ThreadKinds kind)
        {
            if ((kind & ThreadKinds.SourceMask) != ThreadKinds.Unknown)
            {
                throw new InternalException();
            }

            // Ignore during shutdown.
            if (NclUtilities.HasShutdownStarted)
            {
                return null;
            }

            ThreadKinds threadKind = CurrentThreadKind;
            ThreadKinds source = threadKind & ThreadKinds.SourceMask;


            ThreadKindStack.Push(
                (((kind & ThreadKinds.OwnerMask) == 0 ? threadKind : kind) & ThreadKinds.OwnerMask) |
                (((kind & ThreadKinds.SyncMask) == 0 ? threadKind : kind) & ThreadKinds.SyncMask) |
                (kind & ~(ThreadKinds.OwnerMask | ThreadKinds.SyncMask)) |
                source);


            return new ThreadKindFrame();
        }

        private class ThreadKindFrame : IDisposable
        {
            private int m_FrameNumber;

            internal ThreadKindFrame()
            {
                m_FrameNumber = ThreadKindStack.Count;
            }

            void IDisposable.Dispose()
            {
                // Ignore during shutdown.
                if (NclUtilities.HasShutdownStarted)
                {
                    return;
                }

                if (m_FrameNumber != ThreadKindStack.Count)
                {
                    throw new InternalException();
                }

                ThreadKinds previous = ThreadKindStack.Pop();

            }
        }
#endif

        [Conditional("DEBUG")]
        [ReliabilityContract(Consistency.MayCorruptAppDomain, Cer.None)]
        internal static void SetThreadSource(ThreadKinds source)
        {
#if DEBUG
            if ((source & ThreadKinds.SourceMask) != source || source == ThreadKinds.Unknown)
            {
                throw new ArgumentException("Must specify the thread source.", "source");
            }

            if (ThreadKindStack.Count == 0)
            {
                ThreadKindStack.Push(source);
                return;
            }

            if (ThreadKindStack.Count > 1)
            {
                Print("WARNING: SetThreadSource must be called at the base of the stack, or the stack has been corrupted.");
                while (ThreadKindStack.Count > 1)
                {
                    ThreadKindStack.Pop();
                }
            }

            if (ThreadKindStack.Peek() != source)
            {
                // SQL can fail to clean up the stack, leaving the default Other at the bottom.  Replace it.
                Print("WARNING: The stack has been corrupted.");
                ThreadKinds last = ThreadKindStack.Pop() & ThreadKinds.SourceMask;
                Assert(last == source || last == ThreadKinds.Other, "Thread source changed.|Was:({0}) Now:({1})", last, source);
                ThreadKindStack.Push(source);
            }
#endif
        }

        [Conditional("DEBUG")]
        [ReliabilityContract(Consistency.MayCorruptAppDomain, Cer.None)]
        internal static void ThreadContract(ThreadKinds kind, string errorMsg)
        {
            ThreadContract(kind, ThreadKinds.SafeSources, errorMsg);
        }

        [Conditional("DEBUG")]
        [ReliabilityContract(Consistency.MayCorruptAppDomain, Cer.None)]
        internal static void ThreadContract(ThreadKinds kind, ThreadKinds allowedSources, string errorMsg)
        {
            if ((kind & ThreadKinds.SourceMask) != ThreadKinds.Unknown || (allowedSources & ThreadKinds.SourceMask) != allowedSources)
            {
                throw new InternalException();
            }

            ThreadKinds threadKind = CurrentThreadKind;
            Assert((threadKind & allowedSources) != 0, errorMsg, "Thread Contract Violation.|Expected source:({0}) Actual source:({1})", allowedSources , threadKind & ThreadKinds.SourceMask);
            Assert((threadKind & kind) == kind, errorMsg, "Thread Contract Violation.|Expected kind:({0}) Actual kind:({1})", kind, threadKind & ~ThreadKinds.SourceMask);
        }

#if DEBUG
        // Enables auto-hang detection, which will "snap" a log on hang
        internal static bool EnableMonitorThread = false;

        // Default value for hang timer
        public const int DefaultTickValue = 1000*60*5; // 5 minutes
#endif // DEBUG

        [System.Diagnostics.Conditional("TRAVE")]
        public static void AddToArray(string msg) {
        }

        [System.Diagnostics.Conditional("TRAVE")]
        public static void Ignore(object msg) {
        }

        [System.Diagnostics.Conditional("TRAVE")]
        [ReliabilityContract(Consistency.MayCorruptAppDomain, Cer.None)]
        public static void Print(string msg) {
        }

        [System.Diagnostics.Conditional("TRAVE")]
        public static void PrintHex(string msg, object value) {
        }

        [System.Diagnostics.Conditional("TRAVE")]
        public static void Enter(string func) {
        }

        [System.Diagnostics.Conditional("TRAVE")]
        public static void Enter(string func, string parms) {
        }

        [Conditional("DEBUG")]
        [Conditional("_FORCE_ASSERTS")]
        [ReliabilityContract(Consistency.MayCorruptAppDomain, Cer.None)]
        public static void Assert(bool condition, string messageFormat, params object[] data)
        {
            if (!condition)
            {
                string fullMessage = string.Format(CultureInfo.InvariantCulture, messageFormat, data);
                int pipeIndex = fullMessage.IndexOf('|');
                if (pipeIndex == -1)
                {
                    Assert(fullMessage);
                }
                else
                {
                    int detailLength = fullMessage.Length - pipeIndex - 1;
                    Assert(fullMessage.Substring(0, pipeIndex), detailLength > 0 ? fullMessage.Substring(pipeIndex + 1, detailLength) : null);
                }
            }
        }

        [Conditional("DEBUG")]
        [Conditional("_FORCE_ASSERTS")]
        [ReliabilityContract(Consistency.MayCorruptAppDomain, Cer.None)]
        public static void Assert(string message)
        {
            Assert(message, null);
        }

        [Conditional("DEBUG")]
        [Conditional("_FORCE_ASSERTS")]
        [ReliabilityContract(Consistency.MayCorruptAppDomain, Cer.None)]
        public static void Assert(string message, string detailMessage)
        {
            try
            {
                Print("Assert: " + message + (!string.IsNullOrEmpty(detailMessage) ? ": " + detailMessage : ""));
                Print("*******");
                Logobject.DumpArray(false);
            }
            finally
            {
#if DEBUG && !STRESS
                Debug.Assert(false, message, detailMessage);
#else
                UnsafeNclNativeMethods.DebugBreak();
                Debugger.Break();
#endif
            }
        }


        [System.Diagnostics.Conditional("TRAVE")]
        public static void LeaveException(string func, Exception exception) {
        }

        [System.Diagnostics.Conditional("TRAVE")]
        public static void Leave(string func) {
        }

        [System.Diagnostics.Conditional("TRAVE")]
        public static void Leave(string func, string result) {
        }

        [System.Diagnostics.Conditional("TRAVE")]
        public static void Leave(string func, int returnval) {
        }

        [System.Diagnostics.Conditional("TRAVE")]
        public static void Leave(string func, bool returnval) {
        }

        [System.Diagnostics.Conditional("TRAVE")]
        public static void DumpArray() {
        }

        [System.Diagnostics.Conditional("TRAVE")]
        public static void Dump(byte[] buffer) {
        }

        [System.Diagnostics.Conditional("TRAVE")]
        public static void Dump(byte[] buffer, int length) {
        }

        [System.Diagnostics.Conditional("TRAVE")]
        public static void Dump(byte[] buffer, int offset, int length) {
        }

        [System.Diagnostics.Conditional("TRAVE")]
        public static void Dump(IntPtr buffer, int offset, int length) {
        }

#if DEBUG
        private class HttpWebRequestComparer : IComparer {
            public int Compare(
                   object x1,
                   object y1
                   ) {

                HttpWebRequest x = (HttpWebRequest) x1;
                HttpWebRequest y = (HttpWebRequest) y1;

                if (x.GetHashCode() == y.GetHashCode()) {
                    return 0;
                } else if (x.GetHashCode() < y.GetHashCode()) {
                    return -1;
                } else if (x.GetHashCode() > y.GetHashCode()) {
                    return 1;
                }

                return 0;
            }
        }

        private class ConnectionMonitorEntry {
            public HttpWebRequest m_Request;
            public int m_Flags;
            public DateTime m_TimeAdded;
            public Connection m_Connection;

            public ConnectionMonitorEntry(HttpWebRequest request, Connection connection, int flags) {
                m_Request = request;
                m_Connection = connection;
                m_Flags = flags;
                m_TimeAdded = DateTime.Now;
            }
        }

        private static ManualResetEvent s_ShutdownEvent;
        private static SortedList s_RequestList;

        internal const int WaitingForReadDoneFlag = 0x1;
#endif

#if DEBUG
        private static void ConnectionMonitor() {
            while(! s_ShutdownEvent.WaitOne(DefaultTickValue, false)) {
                if (GlobalLog.EnableMonitorThread) {
                }

                int hungCount = 0;
                lock (s_RequestList) {
                    DateTime dateNow = DateTime.Now;
                    DateTime dateExpired = dateNow.AddSeconds(-DefaultTickValue);
                    foreach (ConnectionMonitorEntry monitorEntry in s_RequestList.GetValueList() ) {
                        if (monitorEntry != null &&
                            (dateExpired > monitorEntry.m_TimeAdded))
                        {
                            hungCount++;
                            monitorEntry.m_Connection.Debug(monitorEntry.m_Request.GetHashCode());
                        }
                    }
                }
                Assert(hungCount == 0, "Warning: Hang Detected on Connection(s) of greater than {0} ms.  {1} request(s) hung.|Please Dump System.Net.GlobalLog.s_RequestList for pending requests, make sure your streams are calling Close(), and that your destination server is up.", DefaultTickValue, hungCount);
            }
        }
#endif // DEBUG

#if DEBUG
        [ReliabilityContract(Consistency.MayCorruptAppDomain, Cer.None)]
        internal static void AppDomainUnloadEvent(object sender, EventArgs e) {
            s_ShutdownEvent.Set();
        }
#endif

#if DEBUG
        [System.Diagnostics.Conditional("DEBUG")]
        private static void InitConnectionMonitor() {
            s_RequestList = new SortedList(new HttpWebRequestComparer(), 10);
            s_ShutdownEvent = new ManualResetEvent(false);
            AppDomain.CurrentDomain.DomainUnload += new EventHandler(AppDomainUnloadEvent);
            AppDomain.CurrentDomain.ProcessExit += new EventHandler(AppDomainUnloadEvent);
            Thread threadMonitor = new Thread(new ThreadStart(ConnectionMonitor));
            threadMonitor.IsBackground = true;
            threadMonitor.Start();
        }
#endif

        [System.Diagnostics.Conditional("DEBUG")]
        internal static void DebugAddRequest(HttpWebRequest request, Connection connection, int flags) {
#if DEBUG
            // null if the connection monitor is off
            if(s_RequestList == null)
                return;

            lock(s_RequestList) {
                Assert(!s_RequestList.ContainsKey(request), "s_RequestList.ContainsKey(request)|A HttpWebRequest should not be submitted twice.");

                ConnectionMonitorEntry requestEntry =
                    new ConnectionMonitorEntry(request, connection, flags);

                try {
                    s_RequestList.Add(request, requestEntry);
                } catch {
                }
            }
#endif
        }

        [System.Diagnostics.Conditional("DEBUG")]
        internal static void DebugRemoveRequest(HttpWebRequest request) {
#if DEBUG
            // null if the connection monitor is off
            if(s_RequestList == null)
                return;

            lock(s_RequestList) {
                Assert(s_RequestList.ContainsKey(request), "!s_RequestList.ContainsKey(request)|A HttpWebRequest should not be removed twice.");

                try {
                    s_RequestList.Remove(request);
                } catch {
                }
            }
#endif
        }

        [System.Diagnostics.Conditional("DEBUG")]
        internal static void DebugUpdateRequest(HttpWebRequest request, Connection connection, int flags) {
#if DEBUG
            // null if the connection monitor is off
            if(s_RequestList == null)
                return;

            lock(s_RequestList) {
                if(!s_RequestList.ContainsKey(request)) {
                    return;
                }

                ConnectionMonitorEntry requestEntry =
                    new ConnectionMonitorEntry(request, connection, flags);

                try {
                    s_RequestList.Remove(request);
                    s_RequestList.Add(request, requestEntry);
                } catch {
                }
            }
#endif
        }
    } // class GlobalLog
} // namespace System.Net
