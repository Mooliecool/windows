//------------------------------------------------------------------------------
// <copyright file="TraceEventCache.cs" company="Microsoft">
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

using System;
using System.Threading;
using System.Security.Permissions;
using System.Text;
using System.Collections;
using System.Globalization;

namespace System.Diagnostics {
    public class TraceEventCache {

        private static int processId;
        private static string processName;

        private long timeStamp = -1;
        private DateTime dateTime = DateTime.MinValue;
        private string stackTrace = null;

        internal Guid ActivityId {
            get { return Trace.CorrelationManager.ActivityId; }
        }
        
        public string Callstack {
            get {
                if (stackTrace == null)
                    stackTrace = Environment.StackTrace;
                else
                    new EnvironmentPermission(PermissionState.Unrestricted).Demand();

                return stackTrace;
            }
        }

        public Stack LogicalOperationStack {
            get {
                return Trace.CorrelationManager.LogicalOperationStack;
            }
        }

        public DateTime DateTime {
            get {
                if (dateTime == DateTime.MinValue)
                    dateTime = DateTime.UtcNow;
                return dateTime;
            }
        }

        public int ProcessId {
            get {
                return GetProcessId();
            }
        }

        public string ThreadId {
            get {
                return GetThreadId().ToString(CultureInfo.InvariantCulture);
            }
        }

        public long Timestamp {
            get {
                if (timeStamp == -1)
                    timeStamp = Stopwatch.GetTimestamp();
                return timeStamp ;
            }
        }

        internal void Clear() {
            timeStamp = -1;
            dateTime = DateTime.MinValue;
            stackTrace = null;
        }

        private static void InitProcessInfo() {
            // Demand unmanaged code permission
            new SecurityPermission(SecurityPermissionFlag.UnmanagedCode).Demand();

            if (processName == null) {
                Process p = Process.GetCurrentProcess();
                try {
                    processId = p.Id;
                    processName = p.ProcessName;
                }
                finally {
                    p.Dispose();
                }
            }
        }

        internal static int GetProcessId() {
            InitProcessInfo();
            return processId;
        }
        
        internal static string GetProcessName() {
            InitProcessInfo();
            return processName;
        }
        
        internal static int GetThreadId() {
            return Thread.CurrentThread.ManagedThreadId;
        }
    }
}

