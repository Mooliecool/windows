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
** Class:  SecurityContext
**
**
** Purpose: Capture security  context for a thread
**
** 
===========================================================*/
namespace System.Security
{    
    using Microsoft.Win32;
    using Microsoft.Win32.SafeHandles;
    using System.Threading;
    using System.Runtime.Remoting;
    using System.Security.Principal;
    using System.Collections;
    using System.Runtime.Serialization;
    using System.Security.Permissions;
    using System.Runtime.InteropServices;
    using System.Runtime.CompilerServices;
    using System.Runtime.ConstrainedExecution;
    using System.Runtime.Versioning;

    internal enum SecurityContextDisableFlow
    {
        Nothing = 0,
        WI = 0x1,
        All = 0x3FFF
    }

    internal enum WindowsImpersonationFlowMode { 
	IMP_FASTFLOW = 0,
       IMP_NOFLOW = 1,
       IMP_ALWAYSFLOW = 2,
       IMP_DEFAULT = IMP_FASTFLOW 
    }


    internal struct SecurityContextSwitcher: IDisposable
    {
        internal SecurityContext prevSC; // prev SC that we restore on an Undo
        internal SecurityContext currSC; //current SC  - SetSecurityContext that created the switcher set this on the Thread
        internal ExecutionContext currEC; // current ExecutionContext on Thread
        internal CompressedStackSwitcher cssw;

        public override bool Equals(Object obj)
        {
            if (obj == null || !(obj is SecurityContextSwitcher))
                return false;
            SecurityContextSwitcher sw = (SecurityContextSwitcher)obj;
            return (this.prevSC == sw.prevSC && this.currSC == sw.currSC && this.currEC == sw.currEC && this.cssw == sw.cssw);

        }

        public override int GetHashCode()
        {
            return ToString().GetHashCode();
        }

        public static bool operator ==(SecurityContextSwitcher c1, SecurityContextSwitcher c2) 
        {
            return c1.Equals(c2);
        }

        public static bool operator !=(SecurityContextSwitcher c1, SecurityContextSwitcher c2) 
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
            if (currEC  == null) 
            {
                return; // mutiple Undo()s called on this switcher object
            }  

            if (currEC != Thread.CurrentThread.GetExecutionContextNoCreate()) 
                System.Environment.FailFast(Environment.GetResourceString("InvalidOperation_SwitcherCtxMismatch"));

            BCLDebug.Assert(currEC != null, " ExecutionContext can't be null");
            BCLDebug.Assert(currSC != null, " SecurityContext can't be null");
            if ( currSC != currEC.SecurityContext) {
                System.Environment.FailFast(Environment.GetResourceString("InvalidOperation_SwitcherCtxMismatch"));
            }

            // restore the saved security context 
            currEC.SecurityContext = prevSC;
            currEC = null; // this will prevent the switcher object being used again        

            bool bNoException = true;
            bNoException &= cssw.UndoNoThrow();


            if (!bNoException)
            {
                // Failfast since we can't continue safely...
                System.Environment.FailFast(Environment.GetResourceString("ExecutionContext_UndoFailed"));                
            }

        }
    }
    

    public sealed class SecurityContext 
    {
        // Note that only one of the following variables will be true. The way we set up the flow mode in the g_pConfig guarantees this.
        static bool _LegacyImpersonationPolicy = (GetImpersonationFlowMode() == WindowsImpersonationFlowMode.IMP_NOFLOW);
        static bool _alwaysFlowImpersonationPolicy = (GetImpersonationFlowMode() == WindowsImpersonationFlowMode.IMP_ALWAYSFLOW);

        /*=========================================================================
        ** Data accessed from managed code that needs to be defined in 
        ** SecurityContextObject  to maintain alignment between the two classes.
        ** DON'T CHANGE THESE UNLESS YOU MODIFY SecurityContextObject in vm\object.h
        =========================================================================*/
        
        private ExecutionContext            _executionContext;
        private CompressedStack          _compressedStack;
        static private SecurityContext _fullTrustSC;
        
        internal bool isNewCapture = false;
        internal SecurityContextDisableFlow _disableFlow = SecurityContextDisableFlow.Nothing;



        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal SecurityContext()
        {
        }
            
        static internal SecurityContext FullTrustSecurityContext
        {
            get
            {
                if (_fullTrustSC == null)
                    _fullTrustSC = CreateFullTrustSecurityContext();
                return _fullTrustSC;
            }
        }

        // link the security context to an ExecutionContext
        internal ExecutionContext ExecutionContext
        {
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        	set
        	{
        		_executionContext = value;
        	}
        }
        		

              
        internal CompressedStack CompressedStack
        {
            get
            {
                return _compressedStack; 
            }
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            set
            {
                _compressedStack =  value;					
            }
        }


        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]	
        public static AsyncFlowControl SuppressFlow()
        {
            return SuppressFlow(SecurityContextDisableFlow.All);
        }
        
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]	
        public static AsyncFlowControl SuppressFlowWindowsIdentity()
        {
            return SuppressFlow(SecurityContextDisableFlow.WI);
        }

        internal static AsyncFlowControl SuppressFlow(SecurityContextDisableFlow flags)
        {
            if (IsFlowSuppressed(flags))
            {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_CannotSupressFlowMultipleTimes"));
            }

            if (Thread.CurrentThread.ExecutionContext.SecurityContext == null)
                Thread.CurrentThread.ExecutionContext.SecurityContext = new SecurityContext();
            AsyncFlowControl afc = new AsyncFlowControl();
            afc.Setup(flags);
            return afc;
        }

        public static void RestoreFlow()
        {
    		SecurityContext sc = GetCurrentSecurityContextNoCreate();
    		if (sc == null || sc._disableFlow == SecurityContextDisableFlow.Nothing)
    		{
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_CannotRestoreUnsupressedFlow"));
    		}
			sc._disableFlow = SecurityContextDisableFlow.Nothing;		
        }

        public static bool IsFlowSuppressed()
        {
            return SecurityContext.IsFlowSuppressed(SecurityContextDisableFlow.All);
        }

        public static bool IsWindowsIdentityFlowSuppressed()
        {
            return (_LegacyImpersonationPolicy|| SecurityContext.IsFlowSuppressed(SecurityContextDisableFlow.WI));
        }
        
        internal static bool IsFlowSuppressed(SecurityContextDisableFlow flags)
        {           
                SecurityContext sc = GetCurrentSecurityContextNoCreate();
                return (sc == null) ? false : ((sc._disableFlow & flags) == flags);
        }             


        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure),
         DynamicSecurityMethodAttribute()]        
        public static void Run(SecurityContext securityContext, ContextCallback callback, Object state)
        {
            if (securityContext == null )
            {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NullContext"));
            }

            if (!securityContext.isNewCapture)
            {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NotNewCaptureContext"));
            }

            securityContext.isNewCapture = false;

            ExecutionContext ec = Thread.CurrentThread.GetExecutionContextNoCreate();
            if ( SecurityContext.CurrentlyInDefaultFTSecurityContext(ec)
                && securityContext.IsDefaultFTSecurityContext())
            {
                callback(state);
            }
            else
            {
                RunInternal(securityContext, callback, state);
            }

        }
        internal static void RunInternal(SecurityContext securityContext, ContextCallback callBack, Object state)
        {
            if (cleanupCode == null)
            {
                tryCode = new RuntimeHelpers.TryCode(runTryCode);
                cleanupCode = new RuntimeHelpers.CleanupCode(runFinallyCode);
            }
            SecurityContextRunData runData = new SecurityContextRunData(securityContext, callBack, state);
            RuntimeHelpers.ExecuteCodeWithGuaranteedCleanup(tryCode, cleanupCode, runData);

        }
        
        internal class SecurityContextRunData
        {
            internal SecurityContext sc;
            internal ContextCallback callBack;
            internal Object state;
            internal SecurityContextSwitcher scsw;
            internal SecurityContextRunData(SecurityContext securityContext, ContextCallback cb, Object state)
            {
                this.sc = securityContext;
                this.callBack = cb;
                this.state = state;
                this.scsw = new SecurityContextSwitcher();
            }
        }

        [ResourceExposure(ResourceScope.Process)]
        [ResourceConsumption(ResourceScope.Process)]
        static internal void runTryCode(Object userData)
        {
            SecurityContextRunData rData = (SecurityContextRunData) userData;
            rData.scsw = SetSecurityContext(rData.sc, Thread.CurrentThread.ExecutionContext.SecurityContext);
            rData.callBack(rData.state);
            
        }

        [PrePrepareMethod]
        static internal void runFinallyCode(Object userData, bool exceptionThrown)
        {
            SecurityContextRunData rData = (SecurityContextRunData) userData;
            rData.scsw.Undo();
        }
                    
        static internal RuntimeHelpers.TryCode tryCode;
        static internal RuntimeHelpers.CleanupCode cleanupCode;



        // Internal API that gets called from public SetSecurityContext and from SetExecutionContext
        [ResourceExposure(ResourceScope.Process)]
        [ResourceConsumption(ResourceScope.Process)]
        [DynamicSecurityMethodAttribute()]
        internal static SecurityContextSwitcher SetSecurityContext(SecurityContext sc, SecurityContext prevSecurityContext)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return SetSecurityContext(sc, prevSecurityContext, ref stackMark);
        }

        internal static SecurityContextSwitcher SetSecurityContext(SecurityContext sc, SecurityContext prevSecurityContext, ref StackCrawlMark stackMark)
        {


            // Save the flow state at capture and reset it in the SC.
            SecurityContextDisableFlow _capturedFlowState = sc._disableFlow;
            sc._disableFlow = SecurityContextDisableFlow.Nothing;
            
            //Set up the switcher object
            SecurityContextSwitcher scsw = new SecurityContextSwitcher();
            scsw.currSC = sc;   
            // save the current Execution Context
            ExecutionContext currEC = Thread.CurrentThread.ExecutionContext;
            scsw.currEC = currEC;
            // save the prev security context
            scsw.prevSC = prevSecurityContext;

            // update the current security context to the new security context
            currEC.SecurityContext = sc;


            if (sc != null)
            {
                RuntimeHelpers.PrepareConstrainedRegions();
                try
                {
                    scsw.cssw = CompressedStack.SetCompressedStack(sc.CompressedStack, 
                                                                   (prevSecurityContext!=null?prevSecurityContext.CompressedStack:null));
                }
                catch 
                {
                    scsw.UndoNoThrow();
                    throw;
                }      
            }
            return scsw;
        }

        /// <internalonly/>
        public SecurityContext CreateCopy()
        {
            if (!isNewCapture)
            {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NotNewCaptureContext"));
            }								

            SecurityContext sc = new SecurityContext();
            sc.isNewCapture = true;
            sc._disableFlow = _disableFlow;


            if (_compressedStack != null)
                sc._compressedStack = _compressedStack.CreateCopy();

            return sc;
        }


         public static SecurityContext Capture( )
        {
            // check to see if Flow is suppressed or Security is off
            if (IsFlowSuppressed() || !SecurityManager._IsSecurityOn()) 
                return null;

            StackCrawlMark stackMark= StackCrawlMark.LookForMyCaller;
            SecurityContext sc = SecurityContext.Capture(Thread.CurrentThread.GetExecutionContextNoCreate(), ref stackMark);
            if (sc == null)
                sc = CreateFullTrustSecurityContext();
            return sc;
         }

        // create a clone from a non-existing SecurityContext
        static internal SecurityContext Capture(ExecutionContext currThreadEC, ref StackCrawlMark stackMark)
        {
	      // check to see if Flow is suppressed or Security is off
		if (IsFlowSuppressed() || !SecurityManager._IsSecurityOn()) return null;

        // If we're in FT right now, return null
        if (CurrentlyInDefaultFTSecurityContext(currThreadEC))
            return null;
		  
        SecurityContext sc = new SecurityContext();
        sc.isNewCapture = true;


		// Force create CompressedStack
		sc.CompressedStack = CompressedStack.GetCompressedStack(ref stackMark);
		return sc;
	}
    static internal SecurityContext CreateFullTrustSecurityContext()
    {
        SecurityContext sc = new SecurityContext();
        sc.isNewCapture = true;
        
        

        // Force create CompressedStack
        sc.CompressedStack = new CompressedStack(null);
        return sc;
    }
	// Check to see if we have a security context and return if we do
	static internal SecurityContext GetCurrentSecurityContextNoCreate()
	{
		ExecutionContext ec = Thread.CurrentThread.GetExecutionContextNoCreate();
		return  (ec == null) ? null : ec.SecurityContext;
	}
        
        internal bool IsDefaultFTSecurityContext()
        {
            return (CompressedStack == null || CompressedStack.CompressedStackHandle == null);
        }
        static internal bool CurrentlyInDefaultFTSecurityContext(ExecutionContext threadEC)
        {
            return (IsDefaultThreadSecurityInfo());
        }
        [MethodImplAttribute(MethodImplOptions.InternalCall), ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal extern static WindowsImpersonationFlowMode GetImpersonationFlowMode();
        [MethodImplAttribute(MethodImplOptions.InternalCall), ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal extern static bool IsDefaultThreadSecurityInfo();
        
    }
}
