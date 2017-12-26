//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace Timer
{
    /// <summary>
    /// Summary description for Stopwatch.
    /// </summary>
    public class Stopwatch
    {
        /// <summary>
        /// This function retrieves the frequency of the high-resolution
        /// performance counter if one is provided by the OEM.
        /// </summary>
        /// <param name="lpFrequency">Pointer to a variable that the function
        /// sets, in counts per second, to the current performance-counter
        /// frequency. If the installed hardware does not support a high-
        /// resolution performance counter, this parameter can be to zero.
        /// </param>
        /// <returns>Nonzero indicates that the installed hardware supports
        /// a high-resolution performance counter. Zero indicates that the
        /// installed hardware does not support a high-resolution performance
        /// counter.</returns>
#if !DESKTOP
        [DllImport("CoreDll.dll")]
        internal static extern int QueryPerformanceFrequency(
            ref Int64 lpFrequency);
#else
        [DllImport("kernel32.dll")]
        internal static extern int QueryPerformanceFrequency(
            ref Int64 lpFrequency);
#endif
        /// <summary>
        /// This function retrieves the current value of the high-resolution
        /// performance counter if one is provided by the OEM.
        /// </summary>
        /// <param name="lpPerformanceCount">Pointer to a variable that the
        /// function sets, in counts, to the current performance-counter
        /// value. If the installed hardware does not support a 
        /// high-resolution performance counter, this parameter can be set
        /// to zero.</param>
        /// <returns>QueryPerformanceFrequency will return 1000 if the
        /// hardware does not support a high frequency counter since the
        /// API defaults to a milliseconds GetTickCount implementation.
        /// </returns>
#if !DESKTOP
        [DllImport("CoreDll.dll")]
        internal static extern int QueryPerformanceCounter(
            ref Int64 lpPerformanceCount);
#else
        [DllImport("kernel32.dll")]
        internal static extern int QueryPerformanceCounter(
            ref Int64 lpPerformanceCount);
#endif
        /// <summary>
        /// Frequency of the counter.
        /// </summary>
        Int64 freq;

        /// <summary>
        /// Specifies if performance counters are in use.  Only set to false
        /// if the OEM does not provide these counters.
        /// </summary>
        bool usePerformanceCounters = true;

        /// <summary>
        /// Initializes a stop watch instances by storing the counter
        /// frequency.
        /// </summary>
        public Stopwatch()
        {
            if (QueryPerformanceFrequency(ref freq) == 0)
            {
                freq = 1000;
                usePerformanceCounters = false;
            }

            Debug.Assert(freq != 0,
                "StopWatch.StopWatch: Invalid frequency");
        }

        /// <summary>
        /// Get the current tick.
        /// </summary>
        /// <returns>Tick</returns>
        public Int64 CurrentTick()
        {
            if (usePerformanceCounters)
            {
                Int64 curTick = 0;
                QueryPerformanceCounter(ref curTick);
                return curTick;
            }

            return (Int64)Environment.TickCount;
        }

        /// <summary>
        /// Calculate the delta, taking into account roll-over.
        /// </summary>
        /// <param name="curTime">Current tick</param>
        /// <param name="prevTime">Previous tick</param>
        /// <param name="maxValue">Maximum tick value (for roll-over)</param>
        /// <returns>Current - Previous</returns>
        private Int64 GetSafeDelta(Int64 curTime, Int64 prevTime,
            Int64 maxValue)
        {
            if (curTime < prevTime)
            {
                return curTime + maxValue - prevTime;
            }

            return curTime - prevTime;
        }

        /// <summary>
        /// Calculate the time, in milliseconds, between ticks. 
        /// </summary>
        /// <param name="curTick">Current tick</param>
        /// <param name="previousTick">Previous tick</param>
        /// <returns>Current - Previous (milliseconds)</returns>
        public Int64 DeltaTimeMilliseconds(Int64 curTick, Int64 previousTick)
        {
            if (usePerformanceCounters)
            {
                return 1000 * GetSafeDelta(curTick, previousTick,
                    Int64.MaxValue) / freq;
            }

            return GetSafeDelta(curTick, previousTick, Int32.MaxValue);
        }
    }
}
