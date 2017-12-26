//------------------------------------------------------------------------------
// <copyright file="_Win32.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

namespace System.Net {

    internal static class Win32 {
        internal const  int OverlappedInternalOffset     = 0;
        internal static int OverlappedInternalHighOffset = IntPtr.Size;
        internal static int OverlappedOffsetOffset       = IntPtr.Size*2;
        internal static int OverlappedOffsetHighOffset   = IntPtr.Size*2 + 4;
        internal static int OverlappedhEventOffset       = IntPtr.Size*2 + 8;
        internal static int OverlappedSize               = IntPtr.Size*3 + 8;
    }
}
