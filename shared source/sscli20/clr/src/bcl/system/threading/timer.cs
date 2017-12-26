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
/*=============================================================================
**
** Class: TimerQueue
**
**
** Purpose: Class for creating and managing a threadpool
**
**
=============================================================================*/

namespace System.Threading {
    using System.Threading;
    using System;
    using System.Security;
    using System.Security.Permissions;
    using Microsoft.Win32;
    using System.Runtime.CompilerServices;
    using System.Runtime.InteropServices;
    using System.Runtime.ConstrainedExecution;

    internal class _TimerCallback
    {
        TimerCallback _timerCallback;       
        ExecutionContext _executionContext;
        Object _state;
        static internal ContextCallback _ccb = new ContextCallback(TimerCallback_Context);
        static internal void TimerCallback_Context(Object state)
        {
            _TimerCallback helper = (_TimerCallback) state;
            helper._timerCallback(helper._state);

        }

        internal _TimerCallback(TimerCallback timerCallback, Object state, ref StackCrawlMark stackMark)
        {
            _timerCallback = timerCallback;
            _state = state;
            if (!ExecutionContext.IsFlowSuppressed())
            {
                _executionContext = ExecutionContext.Capture(ref stackMark);
                ExecutionContext.ClearSyncContext(_executionContext);
            }
        }

        // call back helper
        static internal void PerformTimerCallback(Object state)
        {
            _TimerCallback helper = (_TimerCallback)state; 

            BCLDebug.Assert(helper != null, "Null state passed to PerformTimerCallback!");
            // call directly if EC flow is suppressed
            if (helper._executionContext == null)
            {
                TimerCallback callback = helper._timerCallback;
                callback(helper._state);
            }
            else
            {
                // From this point on we can use useExecutionContext for this callback
                ExecutionContext.Run(helper._executionContext.CreateCopy(), _ccb, helper);
            }
        }
    }
        
[System.Runtime.InteropServices.ComVisible(true)]
    public delegate void TimerCallback(Object state);
    
    [HostProtection(Synchronization=true, ExternalThreading=true)]
    internal sealed class TimerBase : CriticalFinalizerObject, IDisposable
    {
#pragma warning disable 169
        private IntPtr     timerHandle;
        private IntPtr     delegateInfo;
#pragma warning restore 169
        private int        timerDeleted;
        private int        m_lock = 0;

        ~TimerBase()
        {
            // lock(this) cannot be used reliably in Cer since thin lock could be
            // promoted to syncblock and that is not a guaranteed operation
            bool bLockTaken = false;
            do 
            {
                if (Interlocked.CompareExchange(ref m_lock, 1, 0) == 0)
                {
                    bLockTaken = true;
                    try
                    {
                        DeleteTimerNative(null);
                    }
                    finally
                    {
                        m_lock = 0;
                    }
                }
                Thread.SpinWait(1);     // yield to processor
            }
            while (!bLockTaken);
        }
    
        internal void AddTimer(TimerCallback   callback,
                                           Object          state, 
                                           UInt32      dueTime,
                                           UInt32          period,
                                           ref StackCrawlMark  stackMark
                                           )
        {
            if (callback != null)
            {
                _TimerCallback callbackHelper = new _TimerCallback(callback, state, ref stackMark);
                state = (Object)callbackHelper;
                AddTimerNative(state, dueTime, period, ref stackMark);
                timerDeleted = 0;
            }
            else
            {
                throw new ArgumentNullException("TimerCallback");
            }
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        internal bool ChangeTimer(UInt32 dueTime,UInt32 period)
        {
            bool status = false;
            bool bLockTaken = false;

            // prepare here to prevent threadabort from occuring which could
            // destroy m_lock state.  lock(this) can't be used due to critical
            // finalizer and thinlock/syncblock escalation.
            RuntimeHelpers.PrepareConstrainedRegions();
            try
            {
            }
            finally
            {
                do 
                {
                    if (Interlocked.CompareExchange(ref m_lock, 1, 0) == 0)
                    {
                        bLockTaken = true;
                        try
                        {
                            if (timerDeleted != 0)
                                throw new ObjectDisposedException(null, Environment.GetResourceString("ObjectDisposed_Generic"));
                            status = ChangeTimerNative(dueTime,period);
                        }
                        finally
                        {
                            m_lock = 0;
                        }
                    }
                    Thread.SpinWait(1);     // yield to processor
                }
                while (!bLockTaken);
            }
            return status;

        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        internal bool Dispose(WaitHandle notifyObject)
        {
            bool status = false;
            bool bLockTaken = false;
            RuntimeHelpers.PrepareConstrainedRegions();
            try
            {
            }
            finally
            {
                do 
                {
                    if (Interlocked.CompareExchange(ref m_lock, 1, 0) == 0)
                    {
                        bLockTaken = true;
                        try
                        {
                            status = DeleteTimerNative(notifyObject.SafeWaitHandle);
                        }
                        finally
                        {
                            m_lock = 0;
                        }
                    }
                    Thread.SpinWait(1);     // yield to processor
                }
                while (!bLockTaken);
                GC.SuppressFinalize(this);
            }

            return status;
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        public void Dispose()
        {
            bool bLockTaken = false;
            RuntimeHelpers.PrepareConstrainedRegions();
            try
            {
            }
            finally
            {
                do 
                {
                    if (Interlocked.CompareExchange(ref m_lock, 1, 0) == 0)
                    {
                        bLockTaken = true;
                        try
                        {
                            DeleteTimerNative(null);
                        }
                        finally
                        {
                            m_lock = 0;
                        }
                    }
                    Thread.SpinWait(1);     // yield to processor
                }
                while (!bLockTaken);
                GC.SuppressFinalize(this);
            }
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void AddTimerNative(Object   state, 
                                           UInt32      dueTime,
                                           UInt32          period,
                                           ref StackCrawlMark  stackMark
                                           );
         
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private  extern bool ChangeTimerNative(UInt32 dueTime,UInt32 period);
    
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private  extern bool DeleteTimerNative(SafeHandle notifyObject);

    }

    [HostProtection(Synchronization=true, ExternalThreading=true)]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class Timer : MarshalByRefObject, IDisposable
    {
        private const UInt32 MAX_SUPPORTED_TIMEOUT = (uint)0xfffffffe;
        private TimerBase timerBase;

        public Timer(TimerCallback callback, 
                     Object        state,  
                     int           dueTime,
                     int           period)
        {
            if (dueTime < -1)
                throw new ArgumentOutOfRangeException("dueTime", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegOrNegative1"));
            if (period < -1 )
                throw new ArgumentOutOfRangeException("period", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegOrNegative1"));
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;

            TimerSetup(callback,state,(UInt32)dueTime,(UInt32)period,ref stackMark);
        }

        public Timer(TimerCallback callback, 
                     Object        state,  
                     TimeSpan      dueTime,
                     TimeSpan      period)
        {                
            long dueTm = (long)dueTime.TotalMilliseconds;
            if (dueTm < -1)
                throw new ArgumentOutOfRangeException("dueTm",Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegOrNegative1"));
            if (dueTm > MAX_SUPPORTED_TIMEOUT)
                throw new ArgumentOutOfRangeException("dueTm",Environment.GetResourceString("ArgumentOutOfRange_TimeoutTooLarge"));

            long periodTm = (long)period.TotalMilliseconds;
            if (periodTm < -1)
                throw new ArgumentOutOfRangeException("periodTm",Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegOrNegative1"));
            if (periodTm > MAX_SUPPORTED_TIMEOUT)
                throw new ArgumentOutOfRangeException("periodTm",Environment.GetResourceString("ArgumentOutOfRange_PeriodTooLarge"));

            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            TimerSetup(callback,state,(UInt32)dueTm,(UInt32)periodTm,ref stackMark);
        }

        [CLSCompliant(false)]
        public Timer(TimerCallback callback, 
                     Object        state,  
                     UInt32        dueTime,
                     UInt32        period)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            TimerSetup(callback,state,dueTime,period,ref stackMark);
        }
                                        
        public Timer(TimerCallback callback, 
                     Object        state,  
                     long          dueTime,
                     long          period)
        {
            if (dueTime < -1)
                throw new ArgumentOutOfRangeException("dueTime",Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegOrNegative1"));
            if (period < -1)
                throw new ArgumentOutOfRangeException("period",Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegOrNegative1"));
            if (dueTime > MAX_SUPPORTED_TIMEOUT)
                throw new ArgumentOutOfRangeException("dueTime",Environment.GetResourceString("ArgumentOutOfRange_TimeoutTooLarge"));
            if (period > MAX_SUPPORTED_TIMEOUT)
                throw new ArgumentOutOfRangeException("period",Environment.GetResourceString("ArgumentOutOfRange_PeriodTooLarge"));
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            TimerSetup(callback,state,(UInt32) dueTime, (UInt32) period,ref stackMark);
        }

        public Timer(TimerCallback callback)
        {
            int dueTime = -1;	// we want timer to be registered, but not activated.  Requires caller to call
            int period = -1;	// Change after a timer instance is created.  This is to avoid the potential
                                // for a timer to be fired before the returned value is assigned to the variable,
                                // potentially causing the callback to reference a bogus value (if passing the timer to the callback). 
			
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            TimerSetup(callback, this, (UInt32)dueTime, (UInt32)period, ref stackMark);
        }

        private void TimerSetup(TimerCallback   callback,
                                                      Object          state, 
                                                      UInt32      dueTime,
                                                      UInt32          period,
                                                      ref StackCrawlMark  stackMark
                                                      )
        {
            timerBase = new TimerBase();
            timerBase.AddTimer(callback, state,(UInt32) dueTime, (UInt32) period, ref stackMark);
        }
    
        public bool Change(int dueTime, int period)
        {
            if (dueTime < -1 )
                throw new ArgumentOutOfRangeException("dueTime",Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegOrNegative1"));
            if (period < -1)
                throw new ArgumentOutOfRangeException("period",Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegOrNegative1"));

            return timerBase.ChangeTimer((UInt32)dueTime,(UInt32)period);
        }

        public bool Change(TimeSpan dueTime, TimeSpan period)
        {
            return Change((long) dueTime.TotalMilliseconds, (long) period.TotalMilliseconds);
        }

        [CLSCompliant(false)]
        public bool Change(UInt32 dueTime, UInt32 period)
        {
            return timerBase.ChangeTimer(dueTime,period);
        }

        public bool Change(long dueTime, long period)
        {
            if (dueTime < -1 )
                throw new ArgumentOutOfRangeException("dueTime", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegOrNegative1"));
            if (period < -1)
                throw new ArgumentOutOfRangeException("period", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegOrNegative1"));
            if (dueTime > MAX_SUPPORTED_TIMEOUT)
                throw new ArgumentOutOfRangeException("dueTime", Environment.GetResourceString("ArgumentOutOfRange_TimeoutTooLarge"));
            if (period > MAX_SUPPORTED_TIMEOUT)
                throw new ArgumentOutOfRangeException("period", Environment.GetResourceString("ArgumentOutOfRange_PeriodTooLarge"));
            
            return timerBase.ChangeTimer((UInt32)dueTime,(UInt32)period);
        }
    
        public bool Dispose(WaitHandle notifyObject)
        {
            if (notifyObject==null)
                throw new ArgumentNullException("notifyObject");
            return timerBase.Dispose(notifyObject);
        }
         

        public void Dispose()
        {
            timerBase.Dispose();
        }
    }
}
