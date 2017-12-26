//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET SDK Code Samples.
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
//-----------------------------------------------------------------------
using System;
using System.Runtime.InteropServices;

namespace Microsoft.Samples.GCDemo
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    internal struct MEMORYSTATUSEX
    {
        internal int dwLength;
        internal int dwMemoryLoad;
        internal long ullTotalPhys;
        internal long ullAvailPhys;
        internal long ullTotalPageFile;
        internal long ullAvailPageFile;
        internal long ullTotalVirtual;
        internal long ullAvailVirtual;
        internal long ullAvailExtendedVirtual;

        internal void Init()
        {
            dwLength = Marshal.SizeOf(typeof(MEMORYSTATUSEX));
        }
    }

    // Helper wrapper for P/Invokes to native methods
    internal sealed class NativeMethods
    {
        private NativeMethods() { }

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode)]
        internal extern static int
                GlobalMemoryStatusEx(ref MEMORYSTATUSEX memoryStatusEx);

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode)]
        internal extern static IntPtr VirtualAlloc(IntPtr ptr, int size, int type, int protect);

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode)]
        internal extern static bool VirtualFree(IntPtr ptr, int size, int type);

    }
}
