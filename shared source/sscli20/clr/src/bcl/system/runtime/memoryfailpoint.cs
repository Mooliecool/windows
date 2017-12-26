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
** Class:  MemoryFailPoint
**
**
** Provides a way for an app to not start an operation unless
** there's a reasonable chance there's enough memory 
** available for the operation to succeed.
**
** 
===========================================================*/

using System;
using System.IO;
using Microsoft.Win32;
using System.Runtime.InteropServices;
using System.Threading;
using System.Runtime.CompilerServices;
using System.Runtime.ConstrainedExecution;
using System.Security.Permissions;
using System.Runtime.Versioning;

/* 
   This class allows an application to fail before starting certain 
   activities.  The idea is to fail early instead of failing in the middle
   of some long-running operation to increase the survivability of the 
   application and ensure you don't have to write tricky code to handle an 
   OOM anywhere in your app's code (which implies state corruption, meaning you
   should unload the appdomain, if you have a transacted environment to ensure
   rollback of individual transactions).  This is an incomplete tool to attempt
   hoisting all your OOM failures from anywhere in your worker methods to one 
   particular point where it is easier to handle an OOM failure, and you can
   optionally choose to not start a workitem if it will likely fail.  This does 
   not help the performance of your code directly (other than helping to avoid 
   AD unloads).  The point is to avoid starting work if it is likely to fail.  
   The Enterprise Services team has used these memory gates effectively in the 
   unmanaged world for a decade.

   In Whidbey, we will simply check to see if there is enough memory available
   in the OS's page file & attempt to ensure there might be enough space free
   within the process's address space (checking for address space fragmentation
   as well).  We will not commit or reserve any memory.  To avoid races with
   other threads using MemoryFailPoints, we'll also keep track of a 
   process-wide amount of memory "reserved" via all currently-active 
   MemoryFailPoints.  This has two problems:
      1) This can account for memory twice.  If a thread creates a 
         MemoryFailPoint for 100 MB then allocates 99 MB, we'll see 99 MB 
         less free memory and 100 MB less reserved memory.  Yet, subtracting 
         off the 100 MB is necessary because the thread may not have started
         allocating memory yet.  Disposing of this class immediately after 
         front-loaded allocations have completed is a great idea.
      2) This is still vulnerable to races with other threads that don't use 
         MemoryFailPoints.
   So this class is far from perfect.  But it may be good enough to 
   meaningfully reduce the frequency of OutOfMemoryExceptions in managed apps.

   In Orcas or later, we might allocate some memory from the OS and add it
   to a allocation context for this thread.  Obviously, at that point we need
   some way of conveying when we release this block of memory.  So, we 
   implemented IDisposable on this type in Whidbey and expect all users to call
   this from within a using block to provide lexical scope for their memory 
   usage.  The call to Dispose (implicit with the using block) will give us an
   opportunity to release this memory, perhaps.  We anticipate this will give 
   us the possibility of a more effective design in a future version.

   In Orcas, we may also need to differentiate between allocations that would
   go into the normal managed heap vs. the large object heap, or we should 
   consider checking for enough free space in both locations (with any 
   appropriate adjustments to ensure the memory is contiguous).
*/

namespace System.Runtime
{
    public sealed class MemoryFailPoint : CriticalFinalizerObject, IDisposable
    {
        // Find the top section of user mode memory.  Avoid the last 64K.
        // Windows reserves that block for the kernel, apparently, and doesn't
        // let us ask about that memory.  But since we ask for memory in 1 MB
        // chunks, we don't have to special case this.  Also, we need to
        // deal with 32 bit machines in 3 GB mode.
        // Using Win32's GetSystemInfo should handle all this for us.
        private static readonly ulong TopOfMemory;

        // Walking the address space is somewhat expensive, taking around half
        // a millisecond.  Doing that per transaction limits us to a max of 
        // ~2000 transactions/second.  Instead, let's do this address space 
        // walk once every 10 seconds, or when we will likely fail.  This
        // amortization scheme can reduce the cost of a memory gate by about
        // a factor of 100.
        private static long LastKnownFreeAddressSpace = 0;
        private static long LastTimeCheckingAddressSpace = 0;
        private const int CheckThreshold = 10 * 1000;  // 10 seconds

        // When allocating memory segment by segment, we've hit some cases
        // where there are only 22 MB of memory available on the machine,
        // we need 1 16 MB segment, and the OS does not succeed in giving us
        // that memory.  Reasons for this could include:
        // 1) The GC does allocate memory when doing a collection.
        // 2) Another process on the machine could grab that memory.
        // 3) Some other part of the runtime might grab this memory.
        // If we build in a little padding, we can help protect
        // ourselves against some of these cases, and we want to err on the
        // conservative side with this class.
        private const int LowMemoryFudgeFactor = 16 << 20;

        // Note: This may become dynamically tunable in the future.
        // Also note that we can have different segment sizes for the normal vs. 
        // large object heap.  We currently use the max of the two.
        private static readonly uint GCSegmentSize;

        // For multi-threaded workers, we want to ensure that if two workers
        // use a MemoryFailPoint at the same time, and they both succeed, that
        // they don't trample over each other's memory.  Keep a process-wide
        // count of "reserved" memory, and decrement this in Dispose and
        // in the critical finalizer.  See 
        // SharedStatics.MemoryFailPointReservedMemory
        
        private ulong _reservedMemory;  // The size of this request (from user)
        private bool _mustSubtractReservation; // Did we add data to SharedStatics?

        static MemoryFailPoint()
        {
            GetMemorySettings(out GCSegmentSize, out TopOfMemory);
        }

        // We can remove this link demand in a future version - we will
        // have scenarios for this in partial trust in the future, but
        // we're doing this just to restrict this in case the code below
        // is somehow incorrect.
        [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode=true)]
        [ResourceExposure(ResourceScope.None)] // Just gives info about free mem.
        [ResourceConsumption(ResourceScope.Process, ResourceScope.Process)]
        public MemoryFailPoint(int sizeInMegabytes)
        {
            if (sizeInMegabytes <= 0)
                throw new ArgumentOutOfRangeException("sizeInMegabytes", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));

            ulong size = ((ulong)sizeInMegabytes) << 20;
            _reservedMemory = size;
            // Allow the fail point to succeed, unless we have a 
            // platform-independent way of checking the amount of free memory.
            // We could allocate a byte[] then discard it, but we don't want
            // to generate garbage like that.
        }


        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetMemorySettings(out uint maxGCSegmentSize, out ulong topOfMemory);

        ~MemoryFailPoint()
        {
            Dispose(false);
        }

        // Applications must call Dispose, which conceptually "releases" the
        // memory that was "reserved" by the MemoryFailPoint.  This affects a
        // global count of reserved memory in this version (helping to throttle
        // future MemoryFailPoints) in this version.  We may in the 
        // future create an allocation context and release it in the Dispose
        // method.  While the finalizer will eventually free this block of 
        // memory, apps will help their performance greatly by calling Dispose.
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        private void Dispose(bool disposing)
        {
            // This is just bookkeeping to ensure multiple threads can really
            // get enough memory, and this does not actually reserve memory
            // within the GC heap.
            if (_mustSubtractReservation) {
                RuntimeHelpers.PrepareConstrainedRegions();
                try {
                }
                finally {
                    SharedStatics.AddMemoryFailPointReservation(-((long)_reservedMemory));
                    _mustSubtractReservation = false;
                }
            }

            /*
            //                                   
            Interlocked.Add(ref LastKnownFreeAddressSpace, _reservedMemory);
            */
        }

#if _DEBUG
        [Serializable]
        internal sealed class MemoryFailPointState
        {
            private int _allocationSizeInMB;
            private ulong _segmentSize;
            private bool _needPageFile;
            private bool _needAddressSpace;
            private bool _needContiguousVASpace;
            private ulong _availPageFile;
            private ulong _totalFreeAddressSpace;
            private long _lastKnownFreeAddressSpace;
            private ulong _reservedMem;

            internal MemoryFailPointState(int allocationSizeInMB, ulong segmentSize, bool needPageFile, bool needAddressSpace, bool needContiguousVASpace, ulong availPageFile, ulong totalFreeAddressSpace, long lastKnownFreeAddressSpace, ulong reservedMem)
            {
                _allocationSizeInMB = allocationSizeInMB;
                _segmentSize = segmentSize;
                _needPageFile = needPageFile;
                _needAddressSpace = needAddressSpace;
                _needContiguousVASpace = needContiguousVASpace;
                _availPageFile = availPageFile;
                _totalFreeAddressSpace = totalFreeAddressSpace;
                _lastKnownFreeAddressSpace = lastKnownFreeAddressSpace;
                _reservedMem = reservedMem;
            }

            public override String ToString()
            {
                return String.Format(System.Globalization.CultureInfo.InvariantCulture, "MemoryGate: Checking for {0} MB, for allocation size of {1} MB.  Need page file? {2}  Need Address Space? {3}  Need Contiguous address space? {4}  Avail page file: {5} MB  Total free VA space: {6} MB  Contiguous free address space (found): {7} MB  Reserved space: {8} MB",
                    _segmentSize >> 20, _allocationSizeInMB, _needPageFile, 
                    _needAddressSpace, _needContiguousVASpace, 
                    _availPageFile >> 20, _totalFreeAddressSpace >> 20, 
                    _lastKnownFreeAddressSpace >> 20, _reservedMem);
            }
        }
#endif
    }
}
