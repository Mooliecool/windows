// ==++==
// 
//   Copyright (c) Microsoft Corporation.  All rights reserved.
// 
// ==--==
//
// <OWNER>[....]</OWNER>

/*
 * This files defines the following types:
 *  - NativeOverlapped
 *  - _IOCompletionCallback
 *  - OverlappedData
 *  - Overlapped
 *  - OverlappedDataCache
 */

/*=============================================================================
**
** Class: Overlapped
**
**
** Purpose: Class for converting information to and from the native 
**          overlapped structure used in asynchronous file i/o
**
**
=============================================================================*/


namespace System.Threading 
{   
    using System;
    using System.Runtime.InteropServices;
    using System.Runtime.CompilerServices;
    using System.Runtime.Versioning;
    using System.Security;
    using System.Security.Permissions;
    using System.Runtime.ConstrainedExecution;
    using System.Diagnostics.Contracts;
    using System.Collections.Concurrent;


    #region struct NativeOverlapped

    // Valuetype that represents the (unmanaged) Win32 OVERLAPPED structure
    // the layout of this structure must be identical to OVERLAPPED.
    // The first five matches OVERLAPPED structure.
    // The remaining are reserved at the end
    [System.Runtime.InteropServices.StructLayout(LayoutKind.Sequential)]
    [System.Runtime.InteropServices.ComVisible(true)]
    public struct NativeOverlapped
    {
        public IntPtr  InternalLow;
        public IntPtr  InternalHigh;
        public int     OffsetLow;
        public int     OffsetHigh;
        public IntPtr  EventHandle;
    }

    #endregion struct NativeOverlapped


    #region class _IOCompletionCallback

    unsafe internal class _IOCompletionCallback
    {
        [System.Security.SecurityCritical] // auto-generated
        IOCompletionCallback _ioCompletionCallback;
        ExecutionContext _executionContext;
        uint _errorCode; // Error code
        uint _numBytes; // No. of bytes transferred 
        [SecurityCritical]
        NativeOverlapped* _pOVERLAP;

        [System.Security.SecuritySafeCritical]  // auto-generated
        static _IOCompletionCallback()
        {
        }

        [System.Security.SecurityCritical]  // auto-generated
        internal _IOCompletionCallback(IOCompletionCallback ioCompletionCallback, ref StackCrawlMark stackMark)
        {
            _ioCompletionCallback = ioCompletionCallback;
            // clone the exection context
            _executionContext = ExecutionContext.Capture(
                ref stackMark, 
                ExecutionContext.CaptureOptions.IgnoreSyncCtx | ExecutionContext.CaptureOptions.OptimizeDefaultCase);
        }
        // Context callback: same sig for SendOrPostCallback and ContextCallback
        #if FEATURE_CORECLR
        [System.Security.SecurityCritical] // auto-generated
        #endif
        static internal ContextCallback _ccb = new ContextCallback(IOCompletionCallback_Context);
        [System.Security.SecurityCritical]
        static internal void IOCompletionCallback_Context(Object state)
        {
            _IOCompletionCallback helper  = (_IOCompletionCallback)state;
            Contract.Assert(helper != null,"_IOCompletionCallback cannot be null");
            helper._ioCompletionCallback(helper._errorCode, helper._numBytes, helper._pOVERLAP);
        }

                                                        
        // call back helper
        [System.Security.SecurityCritical]  // auto-generated
        static unsafe internal void PerformIOCompletionCallback(uint errorCode, // Error code
                                                                            uint numBytes, // No. of bytes transferred 
                                                                            NativeOverlapped* pOVERLAP // ptr to OVERLAP structure
                                                                            )
        {
            Overlapped overlapped;
            _IOCompletionCallback helper;

            do
            {
                overlapped = OverlappedData.GetOverlappedFromNative(pOVERLAP).m_overlapped;
                helper  = overlapped.iocbHelper;

            if (helper == null || helper._executionContext == null || helper._executionContext.IsDefaultFTContext(true))
            {
                // We got here because of UnsafePack (or) Pack with EC flow supressed
                IOCompletionCallback callback = overlapped.UserCallback;
                callback( errorCode,  numBytes,  pOVERLAP);
            }
            else
            {
                // We got here because of Pack
                helper._errorCode = errorCode;
                helper._numBytes = numBytes;
                helper._pOVERLAP = pOVERLAP;
                    using (ExecutionContext executionContext = helper._executionContext.CreateCopy())
                    ExecutionContext.Run(executionContext, _ccb, helper, true);
            }                    

                    //Quickly check the VM again, to see if a packet has arrived.

                OverlappedData.CheckVMForIOPacket(out pOVERLAP, out errorCode, out numBytes);

            } while (pOVERLAP != null);

        }
    }

    #endregion class _IOCompletionCallback


    #region class OverlappedData

    sealed internal class OverlappedData
    {
        // ! If you make any change to the layout here, you need to make matching change 
        // ! to OverlappedObject in vm\nativeoverlapped.h
        internal IAsyncResult m_asyncResult;
        [System.Security.SecurityCritical] // auto-generated
        internal IOCompletionCallback m_iocb;
        internal _IOCompletionCallback m_iocbHelper;
        internal Overlapped m_overlapped;
        private Object m_userObject;
        private IntPtr m_pinSelf;
        private IntPtr m_userObjectInternal;
        private int m_AppDomainId;
#pragma warning disable 414  // Field is not used from managed.        
#pragma warning disable 169
        private byte m_isArray;
        private byte m_toBeCleaned;
#pragma warning restore 414        
#pragma warning restore 169
        internal NativeOverlapped m_nativeOverlapped;

#if FEATURE_CORECLR
        // Adding an empty default ctor for annotation purposes
        [System.Security.SecuritySafeCritical] // auto-generated
        internal OverlappedData(){}
#endif // FEATURE_CORECLR


        [System.Security.SecurityCritical]
        internal void ReInitialize()
        {
            m_asyncResult = null;
            m_iocb = null;
            m_iocbHelper = null;
            m_overlapped = null;
            m_userObject = null;
            Contract.Assert(m_pinSelf.IsNull(), "OverlappedData has not been freed: m_pinSelf");
            m_pinSelf = (IntPtr)0;
            m_userObjectInternal = (IntPtr)0;
            Contract.Assert(m_AppDomainId == 0 || m_AppDomainId == AppDomain.CurrentDomain.Id, "OverlappedData is not in the current domain");
            m_AppDomainId = 0;
            m_nativeOverlapped.EventHandle = (IntPtr)0;
            m_isArray = 0;
            m_nativeOverlapped.InternalLow = (IntPtr)0;
            m_nativeOverlapped.InternalHigh = (IntPtr)0;
        }

        [System.Security.SecurityCritical]  // auto-generated
        [ResourceExposure(ResourceScope.AppDomain)]
        [ResourceConsumption(ResourceScope.AppDomain)]
        [MethodImplAttribute(MethodImplOptions.NoInlining)] // Methods containing StackCrawlMark local var has to be marked non-inlineable
        unsafe internal NativeOverlapped* Pack(IOCompletionCallback iocb, Object userData)
        {
            if (!m_pinSelf.IsNull()) {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_Overlapped_Pack"));
            }
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;

            if (iocb != null)
            {
                m_iocbHelper = new _IOCompletionCallback(iocb, ref stackMark);
                m_iocb = iocb;
            }
            else
            {
                m_iocbHelper = null;
                m_iocb = null;
            }
            m_userObject = userData;
            if (m_userObject != null)
            {
                if (m_userObject.GetType() == typeof(Object[]))
                {
                    m_isArray = 1;
                }
                else
                {
                    m_isArray = 0;
                }
            }
            return AllocateNativeOverlapped();
        }

        [System.Security.SecurityCritical]  // auto-generated_required
        [ResourceExposure(ResourceScope.AppDomain)]
        [ResourceConsumption(ResourceScope.AppDomain)]
        unsafe internal NativeOverlapped* UnsafePack(IOCompletionCallback iocb, Object userData)
        {            
            if (!m_pinSelf.IsNull()) {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_Overlapped_Pack"));
            }
            m_userObject = userData;
            if (m_userObject != null)
            {
                if (m_userObject.GetType() == typeof(Object[]))
                {
                    m_isArray = 1;
                }
                else
                {
                    m_isArray = 0;
                }
            }
            m_iocb = iocb;
            m_iocbHelper = null;
            return AllocateNativeOverlapped();
        }

        [ComVisible(false)]
        internal IntPtr UserHandle
        {
            get { return m_nativeOverlapped.EventHandle; }
            set { m_nativeOverlapped.EventHandle = value; }
        }
        
        [System.Security.SecurityCritical]  // auto-generated
        [ResourceExposure(ResourceScope.AppDomain)]
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        unsafe private extern NativeOverlapped* AllocateNativeOverlapped();

        [System.Security.SecurityCritical]  // auto-generated
        [ResourceExposure(ResourceScope.None)]
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        unsafe internal static extern void FreeNativeOverlapped(NativeOverlapped* nativeOverlappedPtr);

        [System.Security.SecurityCritical]  // auto-generated
        [ResourceExposure(ResourceScope.None)]
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        unsafe internal static extern OverlappedData GetOverlappedFromNative(NativeOverlapped* nativeOverlappedPtr);        

        [System.Security.SecurityCritical]  // auto-generated
        [ResourceExposure(ResourceScope.None)]
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        unsafe internal static extern void CheckVMForIOPacket(out NativeOverlapped* pOVERLAP, out uint errorCode, out uint numBytes);
    }

    #endregion class OverlappedData


    #region class Overlapped

    /// <internalonly/>
    [System.Runtime.InteropServices.ComVisible(true)]
    public class Overlapped
    {
        private OverlappedData m_overlappedData;
   
#if FEATURE_CORECLR
        [System.Security.SecuritySafeCritical]  // auto-generated
#endif
        public Overlapped() 
        {
            m_overlappedData = OverlappedDataCache.GetOverlappedData(this);
        }

        public Overlapped(int offsetLo, int offsetHi, IntPtr hEvent, IAsyncResult ar)
        {
            m_overlappedData = OverlappedDataCache.GetOverlappedData(this);
            m_overlappedData.m_nativeOverlapped.OffsetLow = offsetLo;
            m_overlappedData.m_nativeOverlapped.OffsetHigh = offsetHi;
            m_overlappedData.UserHandle = hEvent;
            m_overlappedData.m_asyncResult = ar;
        }

        [Obsolete("This constructor is not 64-bit compatible.  Use the constructor that takes an IntPtr for the event handle.  http://go.microsoft.com/fwlink/?linkid=14202")]
        public Overlapped(int offsetLo, int offsetHi, int hEvent, IAsyncResult ar) : this(offsetLo, offsetHi, new IntPtr(hEvent), ar)
        {
        }

        public IAsyncResult AsyncResult
        {
            get { return m_overlappedData.m_asyncResult; }
            set { m_overlappedData.m_asyncResult = value; }
        }

        public int OffsetLow
        {
            get { return m_overlappedData.m_nativeOverlapped.OffsetLow; }
            set { m_overlappedData.m_nativeOverlapped.OffsetLow = value; }
        }

        public int OffsetHigh
        {
            get { return m_overlappedData.m_nativeOverlapped.OffsetHigh; }
            set { m_overlappedData.m_nativeOverlapped.OffsetHigh = value; }
        }

        [Obsolete("This property is not 64-bit compatible.  Use EventHandleIntPtr instead.  http://go.microsoft.com/fwlink/?linkid=14202")]
        public int EventHandle
        {
            get { return m_overlappedData.UserHandle.ToInt32(); }
            set { m_overlappedData.UserHandle = new IntPtr(value); }
        }

        [ComVisible(false)]
        public IntPtr EventHandleIntPtr
        {
            get { return m_overlappedData.UserHandle; }
            set { m_overlappedData.UserHandle = value; }
        }

        internal _IOCompletionCallback iocbHelper
        {
            get { return m_overlappedData.m_iocbHelper; }
        }

        internal IOCompletionCallback UserCallback
        {
            [System.Security.SecurityCritical]
            get { return m_overlappedData.m_iocb; }
        }

        /*====================================================================
        *  Packs a managed overlapped class into native Overlapped struct.
        *  Roots the iocb and stores it in the ReservedCOR field of native Overlapped 
        *  Pins the native Overlapped struct and returns the pinned index. 
        ====================================================================*/
        [System.Security.SecurityCritical]  // auto-generated
        [Obsolete("This method is not safe.  Use Pack (iocb, userData) instead.  http://go.microsoft.com/fwlink/?linkid=14202")]
        [CLSCompliant(false)]
        [ResourceExposure(ResourceScope.AppDomain)]
        [ResourceConsumption(ResourceScope.AppDomain)]
        unsafe public NativeOverlapped* Pack(IOCompletionCallback iocb)
        {
            return Pack (iocb, null);
        }

        [System.Security.SecurityCritical]  // auto-generated
        [CLSCompliant(false),ComVisible(false)]
        [ResourceExposure(ResourceScope.AppDomain)]
        [ResourceConsumption(ResourceScope.AppDomain)]
        unsafe public NativeOverlapped* Pack(IOCompletionCallback iocb, Object userData)
        {
            return m_overlappedData.Pack(iocb, userData);
        }

        [System.Security.SecurityCritical]  // auto-generated_required
        [Obsolete("This method is not safe.  Use UnsafePack (iocb, userData) instead.  http://go.microsoft.com/fwlink/?linkid=14202")]
        [CLSCompliant(false)]
        [ResourceExposure(ResourceScope.AppDomain)]
        [ResourceConsumption(ResourceScope.AppDomain)]
        unsafe public NativeOverlapped* UnsafePack(IOCompletionCallback iocb)
        {
            return UnsafePack (iocb, null);
        }

        [System.Security.SecurityCritical]  // auto-generated_required
        [CLSCompliant(false), ComVisible(false)]
        [ResourceExposure(ResourceScope.AppDomain)]
        [ResourceConsumption(ResourceScope.AppDomain)]
        unsafe public NativeOverlapped* UnsafePack(IOCompletionCallback iocb, Object userData)
        {            
            return m_overlappedData.UnsafePack(iocb, userData);
        }

        /*====================================================================
        *  Unpacks an unmanaged native Overlapped struct. 
        *  Unpins the native Overlapped struct
        ====================================================================*/
        [System.Security.SecurityCritical]  // auto-generated
        [CLSCompliant(false)]
        unsafe public static Overlapped Unpack(NativeOverlapped* nativeOverlappedPtr)
        {
            if (nativeOverlappedPtr == null)
                throw new ArgumentNullException("nativeOverlappedPtr");
            Contract.EndContractBlock();

            Overlapped overlapped = OverlappedData.GetOverlappedFromNative(nativeOverlappedPtr).m_overlapped;
            
            return overlapped;
        }

        [System.Security.SecurityCritical]  // auto-generated
        [CLSCompliant(false)]
        unsafe public static void Free(NativeOverlapped* nativeOverlappedPtr)
        {
            if (nativeOverlappedPtr == null)
                throw new ArgumentNullException("nativeOverlappedPtr");
            Contract.EndContractBlock();

            Overlapped overlapped = OverlappedData.GetOverlappedFromNative(nativeOverlappedPtr).m_overlapped;
            OverlappedData.FreeNativeOverlapped(nativeOverlappedPtr);
            OverlappedData overlappedData = overlapped.m_overlappedData;
            overlapped.m_overlappedData = null;
            OverlappedDataCache.CacheOverlappedData(overlappedData);
        }
    
    }

    #endregion class Overlapped


    #region class OverlappedDataCache

    internal sealed class OverlappedDataCache : CriticalFinalizerObject
    {
        // OverlappedData will be pinned during async io operation.
        // In order to avoid pinning in gen 0, we use a cache to recycle OverlappedData objects.  We also allocate
        // them in batches to avoid fragmentation.
        const int BatchSize = 16;

        // We keep two stacks of OverlappedData objects.  This one holds the objects that have been returned to 
        // the cache since the last Gen2 GC.  We will prefer to re-use these ones, to allow the ones in the other
        // stack to eventually be collected.
        static volatile ConcurrentStack<OverlappedData> s_usedSinceLastGC = new ConcurrentStack<OverlappedData>();

        // This stack holds objects that have not been touched since the last Gen2 GC.  The ones that remain untouched
        // at the next Gen2 GC will be discarded.
        static volatile ConcurrentStack<OverlappedData> s_notUsedSinceLastGC;

        // We keep exactly one dummy instance of OverlappedDataCache around at all times, so that we can use its finalizer
        // to detect when a GC has occurred.  
        static int s_finalizerRegistered;
        private static void EnsureFinalizerRegistered()
        {
            if (s_finalizerRegistered == 0)
                if (Interlocked.Exchange(ref s_finalizerRegistered, 1) == 0)
                    new OverlappedDataCache();
        }

#if FEATURE_CORECLR
        [System.Security.SecuritySafeCritical]
        internal OverlappedDataCache()
        {
        }
#endif

#if FEATURE_LEGACYNETCF
        [System.Security.SecuritySafeCritical]  // because it calls GC.GetGeneration
#endif
        ~OverlappedDataCache()
        {
            //
            // First, we need to keep this instance alive so it will get future "notifications" of GCs
            //
            if (!Environment.HasShutdownStarted &&
                !AppDomain.CurrentDomain.IsFinalizingForUnload())
            {
                GC.ReRegisterForFinalize(this);
            }

            //
            // If the "recently used" stack has made it to Gen2, then it's time to throw out the old
            // objects
            //
            if (GC.GetGeneration(s_usedSinceLastGC) >= GC.MaxGeneration)
            {
                //
                // Throw away the objects that haven't been touched in a while,
                // and move the rest of the objects so they become candidates for 
                // discarding at the next GC.  The ones that get used before that happens
                // will not be discarded, since they'll be added to the new "recently-used"
                // list.
                // 
                s_notUsedSinceLastGC = s_usedSinceLastGC;
                s_usedSinceLastGC = new ConcurrentStack<OverlappedData>();
            }
        }

        //
        // Retrieve an OverlappedData from the cache, or create a new one
        //
        internal static OverlappedData GetOverlappedData(Overlapped overlapped)
        {
            EnsureFinalizerRegistered();

            OverlappedData result = null;

            //
            // First, try to pop a recently-used object.
            //
            ConcurrentStack<OverlappedData> stack = s_usedSinceLastGC;
            stack.TryPop(out result);

            //
            // If we didn't get one, try to resurrect an older one.
            //
            if (result == null)
            {
                stack = s_notUsedSinceLastGC;
                if (stack != null)
                    stack.TryPop(out result);
            }

            //
            // We didn't find anything in the cache, so we need to create a new instance.
            //
            if (result == null)
            {
                //
                // To reduce fragmentation, we'll create a whole batch of these, and cache all but the one
                // we return.  Because we're allocating the batch all at once, it is likely that all instances
                // will be contiguous in memory.
                //
                for (int i = 0; i < BatchSize - 1; i++)
                    s_usedSinceLastGC.Push(new OverlappedData());

                result = new OverlappedData();
            }

            result.m_overlapped = overlapped;
            return result;
        }

        //
        // Return a free OverlappedData to the cache
        //
        [System.Security.SecurityCritical]
        internal static void CacheOverlappedData(OverlappedData data)
        {
            data.ReInitialize();

            // this is, by definition, a recently-used object
            s_usedSinceLastGC.Push(data);
        }
    }

    #endregion class OverlappedDataCache

}  // namespace
