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
    using System.Security;
    using System.Security.Permissions;
    using System.Runtime.ConstrainedExecution;

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

        unsafe internal class _IOCompletionCallback
        {
            IOCompletionCallback _ioCompletionCallback;
            ExecutionContext _executionContext;
            uint _errorCode; // Error code
            uint _numBytes; // No. of bytes transferred 
            NativeOverlapped* _pOVERLAP;
            
            internal _IOCompletionCallback(IOCompletionCallback ioCompletionCallback, ref StackCrawlMark stackMark)
            {
                _ioCompletionCallback = ioCompletionCallback;
                // clone the exection context
                _executionContext = ExecutionContext.Capture(ref stackMark);
                ExecutionContext.ClearSyncContext(_executionContext);
            }
            // Context callback: same sig for SendOrPostCallback and ContextCallback
            static internal ContextCallback _ccb = new ContextCallback(IOCompletionCallback_Context);
            static internal void IOCompletionCallback_Context(Object state)
            {
                _IOCompletionCallback helper  = (_IOCompletionCallback)state;
                BCLDebug.Assert(helper != null,"_IOCompletionCallback cannot be null");
                helper._ioCompletionCallback(helper._errorCode, helper._numBytes, helper._pOVERLAP);
            }

                                                        
            // call back helper
            static unsafe internal void PerformIOCompletionCallback(uint errorCode, // Error code
                                                                                uint numBytes, // No. of bytes transferred 
                                                                                NativeOverlapped* pOVERLAP // ptr to OVERLAP structure
                                                                                )
            {
                Overlapped overlapped = OverlappedData.GetOverlappedFromNative(pOVERLAP).m_overlapped;
                _IOCompletionCallback helper  = overlapped.iocbHelper;

                if (helper == null || helper._executionContext == null || helper._executionContext.IsDefaultFTContext())
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
                    ExecutionContext.Run(helper._executionContext.CreateCopy(), _ccb, helper);
                }                    
            }
        };

    sealed internal class OverlappedData : CriticalFinalizerObject
    {
        // ! If you make any change to the layout here, you need to make matching change 
        // ! to OverlappedObject in vm\nativeoverlapped.h
        internal IAsyncResult m_asyncResult;
        internal IOCompletionCallback m_iocb;
        internal _IOCompletionCallback m_iocbHelper;
        internal Overlapped m_overlapped;
        private Object m_userObject;
        internal OverlappedDataCacheLine m_cacheLine;
        private IntPtr m_pinSelf;
        private IntPtr m_userObjectInternal;
        private int m_AppDomainId;
        internal short m_slot;
#pragma warning disable 414  // Field is not used from managed.        
#pragma warning disable 169
        private byte m_isArray;
        private byte m_toBeCleaned;
#pragma warning restore 414        
#pragma warning restore 169
        internal NativeOverlapped m_nativeOverlapped;

        internal OverlappedData(OverlappedDataCacheLine cacheLine)
        {
            m_cacheLine = cacheLine;
        }

        ~OverlappedData()
        {
            if (null != m_cacheLine && false == m_cacheLine.Removed)
            {
                // If user drops reference to Overlapped before calling Pack,
                // let us return the entry to cache
                if (!Environment.HasShutdownStarted &&
                    !AppDomain.CurrentDomain.IsFinalizingForUnload())
                {
                    // If user drops reference to OverlappedData before pack,
                    // we return the object to cache.
                    // We try to keep all items in cache until the whole cache line is not needed.
                    OverlappedDataCache.CacheOverlappedData(this);
                    GC.ReRegisterForFinalize(this);
                }
            }
        }

        internal void ReInitialize()
        {
            m_asyncResult = null;
            m_iocb = null;
            m_iocbHelper = null;
            m_overlapped = null;
            m_userObject = null;
            BCLDebug.Assert(m_pinSelf.IsNull(), "OverlappedData has not been freed: m_pinSelf");
            m_pinSelf = (IntPtr)0;
            m_userObjectInternal = (IntPtr)0;
            BCLDebug.Assert(m_AppDomainId == 0 || m_AppDomainId == AppDomain.CurrentDomain.Id, "OverlappedData is not in the current domain");
            m_AppDomainId = 0;
			m_nativeOverlapped.EventHandle = (IntPtr)0;
            m_isArray = 0;
            m_nativeOverlapped.InternalHigh = (IntPtr)0;
        }

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

        [SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlEvidence = true, ControlPolicy = true )]
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
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        unsafe private extern NativeOverlapped* AllocateNativeOverlapped();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        unsafe internal static extern void FreeNativeOverlapped(NativeOverlapped* nativeOverlappedPtr);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        unsafe internal static extern OverlappedData GetOverlappedFromNative(NativeOverlapped* nativeOverlappedPtr);        
    }

    /// <internalonly/>
[System.Runtime.InteropServices.ComVisible(true)]
    public class Overlapped
    {
        private OverlappedData m_overlappedData;
        
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
            get { return m_overlappedData.m_iocb; }
        }

        /*====================================================================
        *  Packs a managed overlapped class into native Overlapped struct.
        *  Roots the iocb and stores it in the ReservedCOR field of native Overlapped 
        *  Pins the native Overlapped struct and returns the pinned index. 
        ====================================================================*/
        [Obsolete("This method is not safe.  Use Pack (iocb, userData) instead.  http://go.microsoft.com/fwlink/?linkid=14202")]
        [CLSCompliant(false)]
        unsafe public NativeOverlapped* Pack(IOCompletionCallback iocb)
        {
            return Pack (iocb, null);
        }

        [CLSCompliant(false),ComVisible(false)]
        unsafe public NativeOverlapped* Pack(IOCompletionCallback iocb, Object userData)
        {
            return m_overlappedData.Pack(iocb, userData);
        }

        [Obsolete("This method is not safe.  Use UnsafePack (iocb, userData) instead.  http://go.microsoft.com/fwlink/?linkid=14202")]
        [CLSCompliant(false)]
        [SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlEvidence = true, ControlPolicy = true )]
        unsafe public NativeOverlapped* UnsafePack(IOCompletionCallback iocb)
        {
            return UnsafePack (iocb, null);
        }

        [CLSCompliant(false), ComVisible(false)]
        [SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlEvidence = true, ControlPolicy = true )]
        unsafe public NativeOverlapped* UnsafePack(IOCompletionCallback iocb, Object userData)
        {            
            return m_overlappedData.UnsafePack(iocb, userData);
        }

        /*====================================================================
        *  Unpacks an unmanaged native Overlapped struct. 
        *  Unpins the native Overlapped struct
        ====================================================================*/
        [CLSCompliant(false)]
        unsafe public static Overlapped Unpack(NativeOverlapped* nativeOverlappedPtr)
        {
            if (nativeOverlappedPtr == null)
                throw new ArgumentNullException("nativeOverlappedPtr");

            Overlapped overlapped = OverlappedData.GetOverlappedFromNative(nativeOverlappedPtr).m_overlapped;
            
            return overlapped;
        }

        [CLSCompliant(false)]
        unsafe public static void Free(NativeOverlapped* nativeOverlappedPtr)
        {
            if (nativeOverlappedPtr == null)
                throw new ArgumentNullException("nativeOverlappedPtr");

            Overlapped overlapped = OverlappedData.GetOverlappedFromNative(nativeOverlappedPtr).m_overlapped;
            OverlappedData.FreeNativeOverlapped(nativeOverlappedPtr);
            OverlappedData overlappedData = overlapped.m_overlappedData;
            overlapped.m_overlappedData = null;
            OverlappedDataCache.CacheOverlappedData(overlappedData);
        }
    
    }

    // We try to reuse all items in a cache line.
    // When the cache line is not needed, we release all items associated.
    internal sealed class OverlappedDataCacheLine
    {
        internal OverlappedData[] m_items;
        internal OverlappedDataCacheLine m_next;
        private bool m_removed;
        internal const short CacheSize = 16;

        internal OverlappedDataCacheLine()
        {
            m_items = new OverlappedData[OverlappedDataCacheLine.CacheSize];
            // Allocate some dummy objects before and after the cacheLine.
            // These objects will allow GC to move two cacheLine's closer.
            new Object();
            for (short i = 0; i < OverlappedDataCacheLine.CacheSize; i ++)
            {
                m_items[i] = new OverlappedData (this);
                m_items[i].m_slot = i;
            }
            new Object();
        }

        ~OverlappedDataCacheLine()
        {
            m_removed = true;
        }

        internal bool Removed
        {
            get
            {
                return m_removed;
            }
            set
            {
                m_removed = value;
            }
        }
    }

    internal sealed class OverlappedDataCache : CriticalFinalizerObject
    {
        // OverlappedData will be pinned during async io operation.
        // In order to avoid pinning in gen 0, we use a cache to recycle OverlappedData.
        static private OverlappedDataCacheLine m_overlappedDataCache;
        static private int m_overlappedDataCacheAccessed;
        static private int m_cleanupObjectCount;
        static private float m_CleanupThreshold;
        private const float m_CleanupStep = 0.05F;
        private const float m_CleanupInitialThreadhold = 0.3F;

        private int m_gen2GCCount;
        private bool m_ready;

        //static private int m_CollectionCount;

        /*
        static private int m_Create = 0;
        static private int m_Total = 0;
        static private int m_Cache = 0;
        */

        // Setup the cache.
        private static void GrowOverlappedDataCache()
        {
            OverlappedDataCacheLine data = new OverlappedDataCacheLine();
            if (m_overlappedDataCache == null)
            {
                // Add the first node in the list
                if (Interlocked.CompareExchange<OverlappedDataCacheLine>(ref m_overlappedDataCache, data, null) == null)
                {
                    // Use GC to remove items.
                    new OverlappedDataCache();
                    return;
                }
            }

            // If there is already a first node in the list we'll add the new OverlappedDataCacheLine at the end
            if (m_cleanupObjectCount == 0)
            {
                new OverlappedDataCache();
            }

            while (true)
            {
                // Chain the new node
                OverlappedDataCacheLine walk = m_overlappedDataCache;
                while (null != walk && null != walk.m_next)
                {
                    // There's a race with the finalizer here between testing if walk.m_next is null and assigning
                    // Note 1.  
                    //      If walk has been removed from the list by the finalizer thread we still have a valid 
                    //      walk.next chain that will eventually lead us back to the original list, or to NULL.
                    //      If NULL see Note 4 below.
                    // Note 2.
                    //      If walk.next was removed from list the next time through the loop we'll be in the 
                    //      situation described in Note 1.
                    // Note 3.
                    //      If walk.next was set to NULL by the finalizer thread we'll need to test for that in the 
                    //      while condition and after exiting the loop.
                    walk = walk.m_next;
                }
                // if walk has become null (due to finalizer race) after the while test
                // simply return and let GetOverlappedData retry!
                if (null == walk)
                    return;

                // Add the new OverlappedDataCacheLine at the end of the list.  
                // Note 4.
                //      Even if the node that walk points to has been removed from the list we simply 
                //      add the new node to an unreachable graph.  GetOverlappedData() will notice that 
                //      there are still no empty OverlappedData elements and call us again.  The 
                //      "unreachable list" will be reclaimed during the next GC.
                if (Interlocked.CompareExchange<OverlappedDataCacheLine>(ref walk.m_next, data, null) == null)
                    break;
            }
        }


        internal static OverlappedData GetOverlappedData(Overlapped overlapped)
        {
            OverlappedData overlappedData = null;

            Interlocked.Exchange(ref m_overlappedDataCacheAccessed, 1);

            while (true)
            {
                OverlappedDataCacheLine walk = m_overlappedDataCache;
                while (null != walk)
                {
                    for (short i = 0; i < OverlappedDataCacheLine.CacheSize; i ++)
                    {
                        if (walk.m_items[i] != null)
                        {
                            overlappedData = Interlocked.Exchange<OverlappedData>(ref walk.m_items[i], null);
                            if (overlappedData != null)
                            {
                                overlappedData.m_overlapped = overlapped;
                                return overlappedData;
                            }
                        }
                    }

                    walk = walk.m_next;
                }

                GrowOverlappedDataCache();
            }

            /*
            Interlocked.Increment(ref m_Total);
            Console.WriteLine("OverlappedDataCache get " + m_Total +
                              " create " + m_Create +
                              " Cache " + m_Cache);
            */
        }

        // Return a free OverlappedData to cache if the cache has slot available.
        internal static void CacheOverlappedData(OverlappedData data)
        {
            data.ReInitialize();
            
            data.m_cacheLine.m_items[data.m_slot] = data;
        }

        internal OverlappedDataCache()
        {
            if (m_cleanupObjectCount == 0)
            {
                m_CleanupThreshold = m_CleanupInitialThreadhold;
                if (Interlocked.Exchange(ref m_cleanupObjectCount, 1) == 0)
                {
                    m_ready = true;
                }
            }
        }

        // Per GC, if the cache has not been accessed, remove some items from cache.
        ~OverlappedDataCache()
        {
            if (!m_ready)
            {
                return;
            }

            if (null == m_overlappedDataCache)
            {
                Interlocked.Exchange(ref m_cleanupObjectCount, 0);
                return;
            }

            if (!Environment.HasShutdownStarted &&
                !AppDomain.CurrentDomain.IsFinalizingForUnload())
            {
                GC.ReRegisterForFinalize(this);
            }

            int gen2GCCount = GC.CollectionCount(GC.MaxGeneration);
            if (gen2GCCount == m_gen2GCCount)
            {
                // Only do the cleanup when a Gen2 GC happens.
                return;
            }

            m_gen2GCCount = gen2GCCount;

            // reclaim cache
            OverlappedDataCacheLine prev = null;
            OverlappedDataCacheLine walk = m_overlappedDataCache;
            OverlappedDataCacheLine empty = null;
            OverlappedDataCacheLine preempty = prev;
            int total = 0;
            int used = 0;
            while (walk != null)
            {
                total ++;
                bool fUsed = false;
                for (short i = 0; i < OverlappedDataCacheLine.CacheSize; i ++)
                {
                    if (walk.m_items[i] == null)
                    {
                        fUsed = true;
                        used ++;
                    }
                }
                if (!fUsed)
                {
                    preempty = prev;
                    empty = walk;
                }
                prev = walk;
                walk = walk.m_next;
            }
            total *= OverlappedDataCacheLine.CacheSize;

            if (null != empty && total * m_CleanupThreshold > used)
            {
                // We can remove one cache line
                // We only remove a cache line if it is empty.
                if (preempty == null)
                {
                    m_overlappedDataCache = empty.m_next;
                }
                else
                {
                    preempty.m_next = empty.m_next;
                }

                empty.Removed = true;
            }

            if (m_overlappedDataCacheAccessed != 0)
            {
                m_CleanupThreshold = m_CleanupInitialThreadhold;
                Interlocked.Exchange(ref m_overlappedDataCacheAccessed, 0);
            }
            else
            {
                m_CleanupThreshold += m_CleanupStep;
            }
            /*
            OverlappedData[] cache1 = m_overlappedDataCache;
            int length1 = cache1.Length;
            int count1 = 0;
            for (int i1 = 0; i1 < length1; i1 ++)
            {
                if (cache1[i1] != null)
                {
                    count1 ++;
                }
            }
            Console.WriteLine("cleanup left " + count1);
            */
        }
    }
}
