using System;
using System.Diagnostics;
using System.Threading;
using MS.Utility;



namespace MS.Internal
{
    /// <summary>
    /// The MemoryPressure class internally replaces the GC.AddMemoryPressure,
    /// GC.RemoveMemoryPressure API from the CLR.  As of Whidbey Beta 2 the performance
    /// of that API is not sufficient for our needs and should not be used in Avalon.
    /// 
    /// Avalon currently only tracks unmanaged memory pressure related to Images.  
    /// The implementation of this class exploits this by using a timer-based
    /// tracking scheme. It assumes that the unmanaged memory it is tracking
    /// is allocated in batches, held onto for a long time, and released all at once
    /// We have profiled a variety of scenarios and found images do work this way
    /// 
    /// Note this class is thread-safe
    /// </summary>
    
    internal static class MemoryPressure
    {
        static MemoryPressure()
        {
            _collectionTimer = new Stopwatch();
            _allocationTimer = new Stopwatch();
        }

        /// <summary>
        /// Add a number of bytes to the total memory pressure
        /// </summary>
        /// <param name="bytesAllocated"></param>
        internal static void Add(long bytesAllocated)
        {
            if (bytesAllocated < 0)
            {
                throw new ArgumentOutOfRangeException("bytesAllocated");
            }

            //
            // Note that AddToTotal and ProcessAdd are not in a critical
            // section.  We actually don't care if other threads change
            // the value of _totalMemory between the calls.
            // 
            AddToTotal(bytesAllocated);
            ProcessAdd();
        }

        /// <summary>
        /// Remove a number of bytes to the total memory pressure
        /// </summary>
        /// <param name="bytesRemoved"></param>
        internal static void Remove(long bytesRemoved)
        {
            if (bytesRemoved < 0)
            {
                throw new ArgumentOutOfRangeException("bytesRemoved");
            }

            AddToTotal(-bytesRemoved);
        }

        /// <summary>
        /// Check the timers and decide if enough time has elapsed to
        /// force a collection
        /// </summary>
        private static void ProcessAdd()
        {
            bool shouldCollect = false;

            if (_totalMemory >= INITIAL_THRESHOLD)
            {
                //
                // need to synchronize access to the timers, both for the integrity
                // of the elapsed time and to ensure they are reset and started
                // properly
                //
                lock (lockObj)
                {
                    //
                    // if it's been long enough since the last allocation
                    // or too long since the last forced collection, collect
                    //
                    if (_allocationTimer.ElapsedMilliseconds >= INTER_ALLOCATION_THRESHOLD
                        || (_collectionTimer.ElapsedMilliseconds > MAX_TIME_BETWEEN_COLLECTIONS))
                    {
                        _collectionTimer.Reset();
                        _collectionTimer.Start();
                    
                        shouldCollect = true;
                    }
                    _allocationTimer.Reset();
                    _allocationTimer.Start();
                }

                //
                // now that we're out of the lock do the collection
                //
                if (shouldCollect)
                {
                    Collect();
                }
            }

            return;
        }

        /// <summary>
        /// Forces a collection.
        /// </summary>
        private static void Collect()
        {
            //
            // for now only force Gen 2 GCs to ensure we clean up memory
            // These will be forced infrequently and the memory we're tracking
            // is very long lived so it's ok
            //
            GC.Collect(2);
        }

        /// <summary>
        /// Adds the given amount to _totalMemory in a thread-safe manner using the
        /// CompareExchange synchronization primitive
        /// </summary>
        private static void AddToTotal(long delta)
        {
            long initialValue;
            long newValue;

            do
            {
                initialValue = _totalMemory;
                newValue = initialValue + delta;

                //
                // Check for overflow of _totalMemory.
                // Nothing can allocate more than Int64.MaxValue bytes
                // so this theoretically can't happen if the caller is
                // well-behaved.  This only applies when delta is positive.
                //
                Debug.Assert(delta < 0 || initialValue < Int64.MaxValue - delta);

                Debug.Assert(newValue >= 0);

                // CompareExchange will replace _totalMemory with newValue only if
                // _totalMemory equals initialValue. It always returns the current 
                // value of _totalMemory.  
                //
                // If initialValue does not equal the return value from CompareExchange
                // this means that another thread has modified _totalMemory and we need 
                // to try again.

            } while (initialValue != Interlocked.CompareExchange(
                ref _totalMemory, newValue, initialValue));
        }

        private static readonly Object lockObj = new Object();
        private static long _totalMemory;
        private static Stopwatch _collectionTimer;  // time since last collection
        private static Stopwatch _allocationTimer;  // time since last allocation


        //
        // About the thresholds:  
        // For the inter-allocation threshold 850ms is the longest time between allocations on a high-end
        // machine for an image application loading many large (several M pixel) images continuously.
        // This falls well below user-interaction time (which is on the order of several seconds) so it
        // differentiates nicely between the two
        //
        // The initial threshold of 1MB is so we don't force GCs when the total amount of unmanaged memory
        // isn't a big deal.  The point of this code is to stop unmanaged memory from spiraling out of control
        // at that point it's typically in the 10s of MBs.  This threshold thus could potentially be increased
        // but current testing shows it is adequate.
        //
        // The max time between collections was set to 30 sec because that is a 'long time' - this is
        // for the case where allocations (and frees) of images are happening continously without 
        // pause - we haven't seen scenarios that do this yet so it's possible this threshold could also 
        // be increased
        // 
        private const int INITIAL_THRESHOLD = 0x100000;          // 1 MB initial threshold
        private const int INTER_ALLOCATION_THRESHOLD = 850;     // ms allowed between allocations
        private const int MAX_TIME_BETWEEN_COLLECTIONS = 30000; // ms between collections
    }
}
