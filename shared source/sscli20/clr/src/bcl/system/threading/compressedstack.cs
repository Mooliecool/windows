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
** Class: CompressedStack
**
** Purpose: Managed wrapper for the security stack compression implementation
**
=============================================================================*/

namespace System.Threading
{
    using System.Security;
    using System.Security.Permissions;
    using System.Runtime.InteropServices;
    using System.Runtime.CompilerServices;
    using System.Runtime.ConstrainedExecution;
    using System.Reflection;
    using System.Collections;    
    using System.Threading;    
    using System.Runtime.Serialization;


    internal struct CompressedStackSwitcher: IDisposable 
    {
        internal CompressedStack curr_CS;
        internal CompressedStack prev_CS;
        internal IntPtr prev_ADStack;

        
        public override bool Equals(Object obj)
        {
            if (obj == null || !(obj is CompressedStackSwitcher))
                return false;
            CompressedStackSwitcher sw = (CompressedStackSwitcher)obj;
            return (this.curr_CS == sw.curr_CS && this.prev_CS == sw.prev_CS && this.prev_ADStack == sw.prev_ADStack);
        }

        public override int GetHashCode()
        {
        	return ToString().GetHashCode();
        }

        public static bool operator ==(CompressedStackSwitcher c1, CompressedStackSwitcher c2) 
        {
            return c1.Equals(c2);
        }

        public static bool operator !=(CompressedStackSwitcher c1, CompressedStackSwitcher c2) 
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
            if (curr_CS == null && prev_CS == null)
                return;
            if (prev_ADStack != (IntPtr)0)
                CompressedStack.RestoreAppDomainStack(prev_ADStack);
            CompressedStack.SetCompressedStackThread(prev_CS);

            prev_CS = null;
            curr_CS = null;
            prev_ADStack = (IntPtr)0;
        }
    }

     internal class SafeCompressedStackHandle : SafeHandle
     {
        public SafeCompressedStackHandle() : base(IntPtr.Zero, true)
        {       
        }

        public override bool IsInvalid {
            get { return handle == IntPtr.Zero; }
        }

        override protected bool ReleaseHandle()
        {
            CompressedStack.DestroyDelayedCompressedStack(handle);
            handle = IntPtr.Zero;
            return true;
        }
     }   



     [Serializable()]
    public sealed class CompressedStack:ISerializable
    {

        private PermissionListSet m_pls;
        private SafeCompressedStackHandle m_csHandle;


        internal PermissionListSet PLS
        {
            get
            {
                return m_pls;
            }
        }

        internal CompressedStack( SafeCompressedStackHandle csHandle )
        {
            m_csHandle = csHandle;            
        }

        private CompressedStack(SafeCompressedStackHandle csHandle, PermissionListSet pls)
        {
            this.m_csHandle = csHandle;
            this.m_pls = pls;
        }

        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)] 		
        public void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            if (info==null) 
                throw new ArgumentNullException("info");
            CompleteConstruction(null);
            info.AddValue("PLS", this.m_pls);
        }

        private CompressedStack(SerializationInfo info, StreamingContext context) 
        {
            this.m_pls = (PermissionListSet)info.GetValue("PLS", typeof(PermissionListSet));
        }

        internal SafeCompressedStackHandle CompressedStackHandle
        {
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            get
            {
                return m_csHandle;
            }
        }

        [StrongNameIdentityPermissionAttribute(SecurityAction.LinkDemand, PublicKey = "0x00000000000000000400000000000000"),
         SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
        public static CompressedStack GetCompressedStack()
        {
            // This is a Capture()
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return CompressedStack.GetCompressedStack(ref stackMark);
        }

        internal static CompressedStack GetCompressedStack(ref StackCrawlMark stackMark)
        {
            CompressedStack cs;
            CompressedStack innerCS = null;
            if (CodeAccessSecurityEngine.QuickCheckForAllDemands())
            {
                cs = new CompressedStack(null);
            }
            else if (CodeAccessSecurityEngine.AllDomainsHomogeneousWithNoStackModifiers())
            {
                cs = new CompressedStack(null);
                cs.m_pls = PermissionListSet.CreateCompressedState_HG();
            }
            else
            {
                // regular stackwalking case
                cs = new CompressedStack(GetDelayedCompressedStack( ref stackMark ));
                if (cs.CompressedStackHandle != null && IsImmediateCompletionCandidate(cs.CompressedStackHandle, out innerCS))
                {
                    cs.CompleteConstruction(innerCS);
                    DestroyDCSList(cs.CompressedStackHandle);
                }
            }
            return cs;
        }

        public static CompressedStack Capture()
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return GetCompressedStack(ref stackMark);
        }

        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure),
         DynamicSecurityMethodAttribute()]
        public static void Run(CompressedStack compressedStack, ContextCallback callback, Object state)
        {
            
            if (compressedStack == null )
            {
                throw new ArgumentException(Environment.GetResourceString("Arg_NamedParamNull"),"compressedStack");
            }
            if (cleanupCode == null)
            {
                tryCode = new RuntimeHelpers.TryCode(runTryCode);
                cleanupCode = new RuntimeHelpers.CleanupCode(runFinallyCode);
            }
            
            CompressedStackRunData runData = new CompressedStackRunData(compressedStack, callback, state);
            RuntimeHelpers.ExecuteCodeWithGuaranteedCleanup(tryCode, cleanupCode, runData);
        }
            
        internal class CompressedStackRunData
        {
            internal CompressedStack cs;
            internal ContextCallback callBack;
            internal Object state;
            internal CompressedStackSwitcher cssw;
            internal CompressedStackRunData(CompressedStack cs, ContextCallback cb, Object state)
            {
                this.cs = cs;
                this.callBack = cb;
                this.state = state;
                this.cssw = new CompressedStackSwitcher();
            }
        }
        static internal void runTryCode(Object userData)
        {
            CompressedStackRunData rData = (CompressedStackRunData) userData;
            rData.cssw = SetCompressedStack(rData.cs, GetCompressedStackThread());
            rData.callBack(rData.state);

        }

        [PrePrepareMethod]
        static internal void runFinallyCode(Object userData, bool exceptionThrown)
        {
            CompressedStackRunData rData = (CompressedStackRunData) userData;
            rData.cssw.Undo();
        }

        static internal RuntimeHelpers.TryCode tryCode;
        static internal RuntimeHelpers.CleanupCode cleanupCode;

        
        internal static CompressedStackSwitcher SetCompressedStack(CompressedStack cs, CompressedStack prevCS)
        {
            CompressedStackSwitcher cssw = new CompressedStackSwitcher();
            RuntimeHelpers.PrepareConstrainedRegions();
            try
            {
                // Order is important in this block.
                // Also, we dont want any THreadAborts happening when we try to set it
                RuntimeHelpers.PrepareConstrainedRegions();
                try
                {
                    // Empty try block to ensure no ThreadAborts in the finally block
                }
                finally
                {
                    // SetCompressedStackThread can throw - only if it suceeds we shd update the switcher and overrides
                    SetCompressedStackThread(cs);
                    cssw.prev_CS = prevCS;
                    cssw.curr_CS = cs;
                    cssw.prev_ADStack = SetAppDomainStack(cs);                
                }
            }
            catch
            {
                cssw.UndoNoThrow();
                throw; // throw the original exception
            }
            return cssw;
        }
        

        [ComVisible(false)]
        public CompressedStack CreateCopy()
        {
            return new CompressedStack(this.m_csHandle, this.m_pls);
        }
                
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal static IntPtr SetAppDomainStack(CompressedStack cs)
        {
            //Update the AD Stack on the thread and return the previous AD Stack
            return Thread.CurrentThread.SetAppDomainStack((cs == null ? null:cs.CompressedStackHandle)); 
        }

        
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal static void RestoreAppDomainStack(IntPtr appDomainStack)
        {
            Thread.CurrentThread.RestoreAppDomainStack(appDomainStack); //Restore the previous AD Stack
        }
        internal static CompressedStack GetCompressedStackThread()
        {
            ExecutionContext ec = Thread.CurrentThread.GetExecutionContextNoCreate();
            if (ec != null && ec.SecurityContext != null)
                return ec.SecurityContext.CompressedStack;
            return null;
        }
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        internal static void SetCompressedStackThread(CompressedStack cs)
        {
            ExecutionContext ec = Thread.CurrentThread.ExecutionContext;
            if (ec.SecurityContext != null)
                ec.SecurityContext.CompressedStack = cs;
            else if (cs != null)
            {
                SecurityContext sc = new SecurityContext();
                sc.CompressedStack = cs;
                ec.SecurityContext = sc;
            }
        }
        

        internal bool CheckDemand(CodeAccessPermission demand, PermissionToken permToken, RuntimeMethodHandle rmh)
        {
            CompleteConstruction(null);

            if (PLS == null)
                return SecurityRuntime.StackHalt;
            else
                return PLS.CheckDemand(demand, permToken, rmh);

        }

        internal bool CheckSetDemand(PermissionSet pset , RuntimeMethodHandle rmh)
        {
            CompleteConstruction(null);

            if (PLS == null)
                return SecurityRuntime.StackHalt;
            else
                return PLS.CheckSetDemand(pset, rmh);
        }


        internal void GetZoneAndOrigin(ArrayList zoneList, ArrayList originList, PermissionToken zoneToken, PermissionToken originToken)
        {


            CompleteConstruction(null);
            if (PLS != null)
                PLS.GetZoneAndOrigin(zoneList,originList,zoneToken,originToken);
            return;
        }

        internal void CompleteConstruction(CompressedStack innerCS)
        {
            if (PLS != null)
                return;
            PermissionListSet pls = PermissionListSet.CreateCompressedState(this, innerCS);
            lock (this)
            {
                if (PLS == null)
                    m_pls = pls;
            }
        }
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static SafeCompressedStackHandle GetDelayedCompressedStack(ref StackCrawlMark stackMark);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void DestroyDelayedCompressedStack( IntPtr unmanagedCompressedStack );
       
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void DestroyDCSList( SafeCompressedStackHandle compressedStack );
        

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static int GetDCSCount(SafeCompressedStackHandle compressedStack);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool IsImmediateCompletionCandidate(SafeCompressedStackHandle compressedStack, out CompressedStack innerCS);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static DomainCompressedStack GetDomainCompressedStack(SafeCompressedStackHandle compressedStack, int index);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GetHomogeneousPLS(PermissionListSet hgPLS);

        
    }

    //**********************************************************
    // New Implementation of CompressedStack creation/demand eval - NewCompressedStack/DomainCompressedStack
    //**********************************************************
    [Serializable()] 
    internal sealed class DomainCompressedStack
    {
        // Managed equivalent of DomainCompressedStack - used to perform demand evaluation
        private PermissionListSet m_pls;
        // Did we terminate construction on this DCS and therefore, should we terminate construction on the rest of the CS?
        private bool m_bHaltConstruction;


        // CompresedStack interacts with this class purely through the three properties marked internal
        // Zone, Origin, AGRList.
        internal PermissionListSet PLS 
        {
            get
            {
                 return m_pls;
            }
        }

        internal bool ConstructionHalted 
        {
            get
            {
                 return m_bHaltConstruction;
            }
        }



        // Called from the VM only.
        private static DomainCompressedStack CreateManagedObject(IntPtr unmanagedDCS)
        {
            DomainCompressedStack newDCS = new DomainCompressedStack();
            newDCS.m_pls = PermissionListSet.CreateCompressedState(unmanagedDCS, out newDCS.m_bHaltConstruction);
            // return the created object
            return newDCS;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static int GetDescCount(IntPtr dcs);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GetDomainPermissionSets(IntPtr dcs, out PermissionSet granted, out PermissionSet refused);

        // returns true if the descriptor is a FrameSecurityDescriptor
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool GetDescriptorInfo(IntPtr dcs, int index, out PermissionSet granted, out PermissionSet refused, out Assembly assembly, out FrameSecurityDescriptor fsd);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool IgnoreDomain(IntPtr dcs);
    }      
    
}
