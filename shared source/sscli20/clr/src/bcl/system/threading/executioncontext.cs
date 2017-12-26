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
/*============================================================
**
** Class:  ExecutionContext
**
**
** Purpose: Capture execution  context for a thread
**
** 
===========================================================*/
namespace System.Threading
{    
    using System;
    using System.Security;
    using System.Runtime.Remoting;
    using System.Security.Principal;
    using System.Collections;
    using System.Reflection;
    using System.Runtime.Serialization;
    using System.Security.Permissions;  
    using System.Runtime.Remoting.Contexts;
    using System.Runtime.Remoting.Messaging;
    using System.Runtime.InteropServices;
    using System.Runtime.CompilerServices;
    using System.Runtime.ConstrainedExecution;

    internal enum ExceptionType
    {
        InvalidOperation = 0,
        Security = 1,
        EE = 2,
        Generic = 3
    }
    // helper delegate to statically bind to Wait method
    internal delegate int WaitDelegate(IntPtr[] waitHandles, bool waitAll, int millisecondsTimeout);


    internal struct ExecutionContextSwitcher: IDisposable
    {
        internal ExecutionContext prevEC; // previous EC we need to restore on Undo
        internal ExecutionContext currEC; // current EC that we store for checking correctness
        internal SecurityContextSwitcher scsw;
        internal SynchronizationContextSwitcher sysw;
        internal Object hecsw;
        internal Thread thread;

        public override bool Equals(Object obj)
        {
            if (obj == null || !(obj is ExecutionContextSwitcher))
                return false;
            ExecutionContextSwitcher sw = (ExecutionContextSwitcher)obj;
            return (this.prevEC == sw.prevEC && this.currEC == sw.currEC && this.scsw == sw.scsw && 
                this.sysw == sw.sysw && this.hecsw == sw.hecsw && this.thread == sw.thread);
        }
        
        public override int GetHashCode()
        {
            return ToString().GetHashCode();
        }
            
        public static bool operator ==(ExecutionContextSwitcher c1, ExecutionContextSwitcher c2) 
        {
            return c1.Equals(c2);
        }

        public static bool operator !=(ExecutionContextSwitcher c1, ExecutionContextSwitcher c2) 
        {
            return !c1.Equals(c2);
        }   
            
        /// <internalonly/>
        void IDisposable.Dispose()
        {
            Undo();
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        internal bool UndoNoThrow()
        {
            try
            {
                Undo();
            }
            catch
            {
                return false;
            }
            return true;
        }
        
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        public void Undo()
        {
            if (thread == null)
            {
                return; // Don't do anything
            }  
            if (thread != Thread.CurrentThread)
            {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_CannotUseSwitcherOtherThread"));
            }        
            if ( currEC != Thread.CurrentThread.GetExecutionContextNoCreate())
            {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_SwitcherCtxMismatch"));
            }
            BCLDebug.Assert(currEC != null, " ExecutionContext can't be null");


            // Any critical failure inside scsw will cause FailFast
            scsw.Undo();

            try 
            {
                HostExecutionContextSwitcher.Undo(hecsw);
            }
            finally
            {
                // Even if HostExecutionContextSwitcher.Undo(hecsw) throws, we need to revert
                // synchronizationContext. If that throws, we'll be throwing an ex during an exception
                // unwind. That's OK - we'll just have nested exceptions.
                sysw.Undo();
            }

            // restore the saved Execution Context
            Thread.CurrentThread.SetExecutionContext(prevEC);
            thread = null; // this will prevent the switcher object being used again


        }
    }


    public struct AsyncFlowControl: IDisposable
    {
        private bool useEC;
        private ExecutionContext _ec;
        private SecurityContext _sc;
        private Thread _thread;
        internal void Setup(SecurityContextDisableFlow flags)
        {
            useEC = false;
            _sc = Thread.CurrentThread.ExecutionContext.SecurityContext;
            _sc._disableFlow = flags;
            _thread = Thread.CurrentThread;
        }
        internal void Setup()
        {
            useEC = true;
            _ec = Thread.CurrentThread.ExecutionContext;
            _ec.isFlowSuppressed = true;
            _thread = Thread.CurrentThread;
        }
        
        /// <internalonly/>
        void IDisposable.Dispose()
        {
            Undo();
        }
        
        public void Undo()
        {
            if (_thread == null)
            {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_CannotUseAFCMultiple"));
            }  
            if (_thread != Thread.CurrentThread)
            {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_CannotUseAFCOtherThread"));
            }
            if (useEC) 
            {
                if (Thread.CurrentThread.ExecutionContext != _ec)
                {
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_AsyncFlowCtrlCtxMismatch"));
                }      
                ExecutionContext.RestoreFlow();
            }
            else
            {
                if (Thread.CurrentThread.ExecutionContext.SecurityContext != _sc)
                {
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_AsyncFlowCtrlCtxMismatch"));
                }      
                SecurityContext.RestoreFlow();
            }
            _thread = null;
        }
        
        public override int GetHashCode()
        {
            return _thread == null ? ToString().GetHashCode() : _thread.GetHashCode();
        }
        
        public override bool Equals(Object obj)
        {
            if (obj is AsyncFlowControl)
                return Equals((AsyncFlowControl)obj);
            else
                return false;
        }
    
        public bool Equals(AsyncFlowControl obj)
        {
            return obj.useEC == useEC && obj._ec == _ec &&
                obj._sc == _sc && obj._thread == _thread;
        }
    
        public static bool operator ==(AsyncFlowControl a, AsyncFlowControl b)
        {
            return a.Equals(b);
        }
        
        public static bool operator !=(AsyncFlowControl a, AsyncFlowControl b)
        {
            return !(a == b);
        }
    	
    }
    
    [System.Runtime.InteropServices.ComVisible(true)]
    public delegate void ContextCallback(Object state);


    [Serializable()] 
    public sealed class ExecutionContext : ISerializable
    {
        /*=========================================================================
        ** Data accessed from managed code that needs to be defined in 
        ** ExecutionContextObject  to maintain alignment between the two classes.
        ** DON'T CHANGE THESE UNLESS YOU MODIFY ExecutionContextObject in vm\object.h
        =========================================================================*/
        private HostExecutionContext _hostExecutionContext;
        private SynchronizationContext _syncContext;
        private SecurityContext     _securityContext;
        private LogicalCallContext  _logicalCallContext;
        private IllogicalCallContext _illogicalCallContext;  // this call context follows the physical thread
        private Thread          _thread;
        internal bool isNewCapture = false;
        internal bool isFlowSuppressed = false;

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal ExecutionContext()
        {            
        }

        internal LogicalCallContext LogicalCallContext
        {
            get
            {
                if (_logicalCallContext == null)
                {
                _logicalCallContext = new LogicalCallContext();
                }
                return _logicalCallContext;
            }
            set
            {
                _logicalCallContext = value;
            }
        }

        internal IllogicalCallContext IllogicalCallContext
        {
            get
            {
                if (_illogicalCallContext == null)
                {
                _illogicalCallContext = new IllogicalCallContext();
                }
                return _illogicalCallContext;
            }
            set
            {
                _illogicalCallContext = value;
            }
        }
        internal Thread Thread
        {
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            set
            {               
                _thread = value;
            }
        }

        internal SynchronizationContext SynchronizationContext
        {
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            get
            {
                return _syncContext;
            }
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            set
            {
                _syncContext = value;
            }
        }

    internal HostExecutionContext HostExecutionContext
    {
            get 
            {
                return _hostExecutionContext;
            }
            set 
            {
                _hostExecutionContext = value;
            }
    }
        
        internal  SecurityContext SecurityContext
        {
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            get
            {
                return _securityContext;
            }
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            set
            {
                        // store the new security context 
                        _securityContext = value;
                        // perform the reverse link too
                        if (value != null)
                            _securityContext.ExecutionContext = this;
            }
        }

        
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure),
         DynamicSecurityMethodAttribute()]
        public static void Run(ExecutionContext executionContext, ContextCallback callback,  Object state)
        {
            if (executionContext == null )
            {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NullContext"));
            }

            if (!executionContext.isNewCapture)
            {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NotNewCaptureContext"));
            }
            
            executionContext.isNewCapture = false;
            
            ExecutionContext ec = Thread.CurrentThread.GetExecutionContextNoCreate();
            if ( (ec == null || ec.IsDefaultFTContext()) && SecurityContext.CurrentlyInDefaultFTSecurityContext(ec)
                && executionContext.IsDefaultFTContext())
            {
                callback(state);
            }
            else
            {
                RunInternal(executionContext, callback, state);
            }
        }

        internal static void RunInternal(ExecutionContext executionContext, ContextCallback callback,  Object state)
        {
            if (cleanupCode == null)
            {
                tryCode = new RuntimeHelpers.TryCode(runTryCode);
                cleanupCode = new RuntimeHelpers.CleanupCode(runFinallyCode);
            }

            ExecutionContextRunData runData = new ExecutionContextRunData(executionContext, callback, state);            
            RuntimeHelpers.ExecuteCodeWithGuaranteedCleanup(tryCode, cleanupCode, runData);
        }

        internal class ExecutionContextRunData
        {
            internal ExecutionContext ec;
            internal ContextCallback callBack;
            internal Object state;
            internal ExecutionContextSwitcher ecsw;
            internal ExecutionContextRunData(ExecutionContext executionContext, ContextCallback cb, Object state)
            {
                this.ec = executionContext;
                this.callBack = cb;
                this.state = state;
                ecsw = new ExecutionContextSwitcher();
            }
        }
        
        static internal void runTryCode(Object userData)
        {
            ExecutionContextRunData rData = (ExecutionContextRunData) userData;
            rData.ecsw = SetExecutionContext(rData.ec);
            rData.callBack(rData.state);
            
        }

        [PrePrepareMethod]
        static internal void runFinallyCode(Object userData, bool exceptionThrown)
        {
            ExecutionContextRunData rData = (ExecutionContextRunData) userData;
            rData.ecsw.Undo();
        }
                    
        static internal RuntimeHelpers.TryCode tryCode;
        static internal RuntimeHelpers.CleanupCode cleanupCode;

            
        // Sets the given execution context object on the thread.
        // Returns the previous one.
        [DynamicSecurityMethodAttribute()]
        internal  static ExecutionContextSwitcher SetExecutionContext(ExecutionContext executionContext)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            BCLDebug.Assert(executionContext != null, "ExecutionContext cannot be null here.");
         

            // Set up the switcher object to return;
            ExecutionContextSwitcher ecsw = new ExecutionContextSwitcher();
            
            ecsw.thread = Thread.CurrentThread;
            ecsw.prevEC = Thread.CurrentThread.GetExecutionContextNoCreate(); // prev
            ecsw.currEC = executionContext; //current

            // Update the EC on thread
            Thread.CurrentThread.SetExecutionContext(executionContext);

            RuntimeHelpers.PrepareConstrainedRegions();
            try
            {
                if (executionContext != null)
                {
                    //set the security context
                    SecurityContext sc = executionContext.SecurityContext;
                    if (sc != null)
                    {
                        // non-null SC: needs to be set
                        SecurityContext prevSeC = (ecsw.prevEC != null) ? ecsw.prevEC.SecurityContext : null;
                        ecsw.scsw = SecurityContext.SetSecurityContext(sc, prevSeC, ref stackMark);
                    }
                    else if (!SecurityContext.CurrentlyInDefaultFTSecurityContext(ecsw.prevEC))
                    {
                        // null incoming SC, but we're currently not in FT: use static FTSC to set
                        SecurityContext prevSeC = (ecsw.prevEC != null) ? ecsw.prevEC.SecurityContext : null;
                        ecsw.scsw = SecurityContext.SetSecurityContext(SecurityContext.FullTrustSecurityContext, prevSeC, ref stackMark);
                    }

                    // set the sync context
                    SynchronizationContext syncContext = executionContext.SynchronizationContext;
                    if (syncContext != null)
                    {
                            SynchronizationContext prevSyC = (ecsw.prevEC != null) ? ecsw.prevEC.SynchronizationContext : null;
                            ecsw.sysw = SynchronizationContext.SetSynchronizationContext(syncContext, prevSyC);
                    }                    
                
                    // set the Host Context
                    HostExecutionContext hostContext = executionContext.HostExecutionContext;
                    if (hostContext != null)
                    {
                        ecsw.hecsw = HostExecutionContextManager.SetHostExecutionContextInternal(hostContext);
                    }   
                    
                }   
            }
            catch
            {
                ecsw.UndoNoThrow();
                throw;
            }
            return ecsw;    
        }

        public ExecutionContext CreateCopy()
        {
            if (!isNewCapture)
            {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_CannotCopyUsedContext"));
            }
            ExecutionContext ec = new ExecutionContext();
            ec.isNewCapture = true;
            ec._syncContext = _syncContext == null?null:_syncContext.CreateCopy();
            // capture the host execution context
            ec._hostExecutionContext = _hostExecutionContext == null ? null : _hostExecutionContext.CreateCopy();
            if (_securityContext != null)
            {
                ec._securityContext = _securityContext.CreateCopy();
                ec._securityContext.ExecutionContext = ec;
            }
            if (this._logicalCallContext != null)
            {
                LogicalCallContext lc = (LogicalCallContext)this.LogicalCallContext;
                ec.LogicalCallContext = (LogicalCallContext)lc.Clone();
            }
            if (this._illogicalCallContext != null)
            {
                IllogicalCallContext ilcc = (IllogicalCallContext)this.IllogicalCallContext;
                ec.IllogicalCallContext = (IllogicalCallContext)ilcc.Clone();
            }

            return ec;
        }

        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]   
        public static AsyncFlowControl SuppressFlow()
        {
            if (IsFlowSuppressed())
            {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_CannotSupressFlowMultipleTimes"));
            }
            AsyncFlowControl afc = new AsyncFlowControl();
            afc.Setup();
            return afc;
        }

        public static void RestoreFlow()
        {
            ExecutionContext ec = Thread.CurrentThread.GetExecutionContextNoCreate();
            if (ec == null || !ec.isFlowSuppressed)
            {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_CannotRestoreUnsupressedFlow"));
            }
            ec.isFlowSuppressed = false;
        }

        public static bool IsFlowSuppressed()
        {
            ExecutionContext ec = Thread.CurrentThread.GetExecutionContextNoCreate();
            if (ec == null)
                return false;
            else
                return ec.isFlowSuppressed;
        }

        public static ExecutionContext Capture()
        {
            // set up a stack mark for finding the caller
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return ExecutionContext.Capture(ref stackMark);            
        }

    // internal helper to capture the current execution context using a passed in stack mark
        static internal ExecutionContext Capture(ref StackCrawlMark stackMark)
        {                   
            // check to see if Flow is suppressed
            if (IsFlowSuppressed()) 
                return null;

            ExecutionContext ecCurrent = Thread.CurrentThread.GetExecutionContextNoCreate();        
            ExecutionContext ecNew = new ExecutionContext();
            ecNew.isNewCapture = true;
            
            // capture the security context
            ecNew.SecurityContext = SecurityContext.Capture(ecCurrent, ref stackMark);
            if (ecNew.SecurityContext != null)
                ecNew.SecurityContext.ExecutionContext = ecNew;

             // capture the host execution context
            ecNew._hostExecutionContext = HostExecutionContextManager.CaptureHostExecutionContext();    		 

            
            if (ecCurrent != null)  
            {
                // capture the sync context
                ecNew._syncContext = (ecCurrent._syncContext == null) ?null: ecCurrent._syncContext.CreateCopy();

                // copy over the Logical Call Context
                if (ecCurrent._logicalCallContext != null)
                {
                    LogicalCallContext lc = (LogicalCallContext)ecCurrent.LogicalCallContext;
                    ecNew.LogicalCallContext = (LogicalCallContext)lc.Clone();
                }

            }
            return ecNew;
        }

        //
        // Implementation of ISerializable
        //

        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)] 		
        public void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            if (info==null) 
                throw new ArgumentNullException("info");

            if (_logicalCallContext != null)
            {
                info.AddValue("LogicalCallContext", _logicalCallContext, typeof(LogicalCallContext));
            }
        }

        private ExecutionContext(SerializationInfo info, StreamingContext context) 
        {
            SerializationInfoEnumerator e = info.GetEnumerator();
            while (e.MoveNext())
            {
                if (e.Name.Equals("LogicalCallContext"))
                {
                    _logicalCallContext = (LogicalCallContext) e.Value;
                }
            }
            this.Thread = Thread.CurrentThread;
        } // ObjRef .ctor
        static internal void ClearSyncContext(ExecutionContext ec)
        {
            if (ec != null)
                ec.SynchronizationContext = null;
        }
        internal bool IsDefaultFTContext()
        {
            if (_hostExecutionContext != null)
                return false;
            if (_syncContext != null)
                return false;
            if (_securityContext != null && !_securityContext.IsDefaultFTSecurityContext())
                return false;
            if (_logicalCallContext != null && _logicalCallContext.HasInfo)
                return false;
            if (_illogicalCallContext != null && _illogicalCallContext.HasUserData)
                return false;
            return true;
        }
    } // class ExecutionContext
}
