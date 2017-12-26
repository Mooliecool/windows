//---------------------------------------------------------------------------
//
// <copyright file="SafeMILHandleMemoryPressure.cs" company="Microsoft">
//    Copyright (C) Microsoft Corporation.  All rights reserved.
// </copyright>
// 
//
// Description: 
//      Tracks the amount of native memory used by SafeMILHandle objects.
//---------------------------------------------------------------------------

using System;
using System.Security;
using MS.Internal;
using System.Threading;

namespace System.Windows.Media
{
    internal class SafeMILHandleMemoryPressure
    {
        /// <SecurityNote>
        ///    Critical: This code calls into AddMemoryPressure which has a link demand.
        /// </SecurityNote>
        [SecurityCritical]
        internal SafeMILHandleMemoryPressure(long gcPressure)
        {
            _gcPressure = gcPressure;
            _refCount = 0;

            if (_gcPressure > SMALL_BITMAP_MEM)
            {
                MemoryPressure.Add(_gcPressure);
            }
            else
            {
                GC.AddMemoryPressure(_gcPressure);
            }
        }

        internal void AddRef()
        {
            Interlocked.Increment(ref _refCount);
        }

        /// <SecurityNote>
        ///    Critical: This code calls into RemoveMemoryPressure which has a link demand.
        /// </SecurityNote>
        [SecurityCritical]
        internal void Release()
        {
            if (Interlocked.Decrement(ref _refCount) == 0)
            {
                if (_gcPressure > SMALL_BITMAP_MEM)
                {
                    MemoryPressure.Remove(_gcPressure);
                }
                else
                {
                    GC.RemoveMemoryPressure(_gcPressure);
                }

                _gcPressure = 0;
            }
        }

        // Estimated size in bytes of the unmanaged memory
        private long _gcPressure;

        //
        // SafeMILHandleMemoryPressure does its own ref counting in managed code, because the
        // associated memory pressure should be removed when there are no more managed
        // references to the unmanaged resource. There can still be references to it from
        // unmanaged code elsewhere, but that should not prevent the memory pressure from being
        // released.
        //
        private int _refCount;

        //
        // The memory usage for a "small" bitmap
        //
        // Small bitmaps will be handled by GC.AddMemoryPressure() rather than WPF's own
        // MemoryPressure algorithm. GC's algorithm is less aggressive than WPF's, and will
        // result in fewer expensive GC.Collect(2) operations.
        //
        // For now, a small bitmap is 32x32 or less. It's at 4 bytes per pixel, and the *2 is
        // to account for the doubled estimate in
        //     BitmapSourceMILSafeHandle.ComputeEstimatedSize().
        //
        private const long SMALL_BITMAP_MEM = 32 * 32 * 4 * 2;
    }
}

