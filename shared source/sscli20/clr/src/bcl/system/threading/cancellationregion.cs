// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
/*============================================================
**
** Class:  CancellationRegion
**
**
** Purpose: Defines a block of code where all IO's may be
** cancelled or marked as uncancelable.  Cancellation 
** regions can be nested.
**
** 
===========================================================*/

using System.Collections.Generic;
using System.Security.Permissions;
using System.Runtime.ConstrainedExecution;
using System.Runtime.CompilerServices;

namespace System.Threading {


    public struct CancellationRegion : IDisposable
    {
        private CancellationSignal _signal;

        [ReliabilityContract(Consistency.MayCorruptInstance, Cer.MayFail)]
        private CancellationRegion(CancellationSignal signal)
        {
            BCLDebug.Assert(signal != null, "Must have a cancelation signal instance.");

            _signal = signal;
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        public void Dispose()
        {
            if (_signal == null)
                return;

            BCLDebug.Assert(Thread.CurrentThread == _signal.Thread, "You called Dispose on the wrong thread, or reused your cancellation signal?");
            List<CancellationSignal> signals = _signal.Thread.CancellationSignals;

            bool tookLock = false;
            RuntimeHelpers.PrepareConstrainedRegions();
            try {
                Monitor.ReliableEnter(signals, out tookLock);
                CancellationSignal s = signals[signals.Count - 1];
                signals.RemoveAt(signals.Count - 1);
                if (!Object.ReferenceEquals(s, _signal))
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_CancellationRegionLeak"));
                _signal.Thread = null;
                _signal = null;
            }
            finally {
                if (tookLock)
                    Monitor.Exit(signals);
                Thread.EndThreadAffinity();
            }
        }

        [HostProtection(ExternalThreading=true)]
        [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode=true)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        public static CancellationRegion SetNonCancelable()
        {
            CancellationSignal signal = new CancellationSignal(false);
            Thread t = Thread.CurrentThread;
            signal.Thread = t;
            List<CancellationSignal> signals = t.CancellationSignals;
            CancellationRegion region = new CancellationRegion(signal);

            lock(signals) {
                signals.Add(signal);
                // Note that all failures due to allocations will be above 
                // this point in this method.  Also, note that it's fine to 
                // Add first then get thread affinity later - the Cancel
                // method holds this same lock while cancelling.
                // Ensure that another fiber cannot run on this thread.
                Thread.BeginThreadAffinity();
            }
            return region;
        }

        [HostProtection(ExternalThreading=true)]
        [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode=true)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        public static CancellationRegion SetCancelable(CancellationSignal signal)
        {
            if (signal == null)
                throw new ArgumentNullException("signal");

            Thread t = Thread.CurrentThread;
            signal.Thread = t;
            List<CancellationSignal> signals = t.CancellationSignals;
            CancellationRegion region = new CancellationRegion(signal);

            lock(signals) {
                signals.Add(signal);
                // Note that all failures due to allocations will be above 
                // this point in this method.  Also, note that it's fine to 
                // Add first then get thread affinity later - the Cancel
                // method holds this same lock while cancelling.
                // Ensure that another fiber cannot run on this thread.
                Thread.BeginThreadAffinity();
            }
            return region;
        }

        // To allow CPU-intensive tasks to use our cancellation model to 
        // opt into cancellation via polling.  We could also use this in 
        // select places to help avoid races inherent in the OS's 
        // cancellation model, such as attempting to cancel CopyFile while
        // it is in the middle of some CPU-intensive calculation instead of
        // blocked waiting for IO.  (We could also use this in FileStream
        // and other places to help support cancellation, without having to 
        // allocate a cancellation region).
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        public static void PollForCancellation()
        {
            Thread t = Thread.CurrentThread;
            // Don't go through the property on Thread, but access the field
            // directly.  We don't want to create the stack if no one in the
            // thread is using cancellation.
            List<CancellationSignal> signals = t.m_CancellationSignals;
            if (signals != null) {
                bool tookLock = false;
                RuntimeHelpers.PrepareConstrainedRegions();
                try {
                    Monitor.ReliableEnter(signals, out tookLock);
                    if (signals.Count > 0) {
                        if (signals[signals.Count - 1].CancelRequested)
                            throw new OperationCanceledException();
                    }
                }
                finally {
                    if (tookLock)
                        Monitor.Exit(signals);
                }
            }
        }
    }
}
