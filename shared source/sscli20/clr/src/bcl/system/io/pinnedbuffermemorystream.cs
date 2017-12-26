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
** Class:  PinnedBufferMemoryStream
**
**
** Purpose: Pins a byte[], exposing it as an unmanaged memory 
**          stream.  Used in ResourceReader for corner cases.
**
**
===========================================================*/
using System;
using System.Runtime.InteropServices;

namespace System.IO {
    internal sealed unsafe class PinnedBufferMemoryStream : UnmanagedMemoryStream
    {
        private byte[] _array;
        private GCHandle _pinningHandle;

        internal PinnedBufferMemoryStream(byte[] array)
        {
            BCLDebug.Assert(array != null, "Array can't be null");

            int len = array.Length;
            // Handle 0 length byte arrays specially.
            if (len == 0) {
                array = new byte[1];
                len = 0;
            }

            _array = array;
            _pinningHandle = new GCHandle(array, GCHandleType.Pinned);
            // Now the byte[] is pinned for the lifetime of this instance.
            // But I also need to get a pointer to that block of memory...
            fixed(byte* ptr = _array)
                Initialize(ptr, len, len, FileAccess.Read, true);
        }

        ~PinnedBufferMemoryStream()
        {
            Dispose(false);
        }

        protected override void Dispose(bool disposing)
        {
            if (_isOpen) {
                _pinningHandle.Free();
                _isOpen = false;
            }
#if _DEBUG
            // To help track down lifetime issues on checked builds, force 
            //a full GC here.
            if (disposing) {
                GC.Collect();
                GC.WaitForPendingFinalizers();
            }
#endif
            base.Dispose(disposing);
        }
    }
}
