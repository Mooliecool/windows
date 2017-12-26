//------------------------------------------------------------------------------
// <copyright file="SafeNativeMethods.cs" company="Microsoft">
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

namespace Microsoft.Win32 {
    using System.Runtime.InteropServices;
    using System;
    using System.Diagnostics.CodeAnalysis;
    using System.Security.Permissions;
    using System.Collections;
    using System.IO;
    using System.Text;
    using System.Threading;
    using Microsoft.Win32.SafeHandles;    
    using System.Runtime.ConstrainedExecution;
    using System.Runtime.Versioning;
    using System.Diagnostics;
    
    [HostProtection(MayLeakOnAbort = true)]
    [System.Security.SuppressUnmanagedCodeSecurity]
    internal static class SafeNativeMethods {

        public const int
            FORMAT_MESSAGE_ALLOCATE_BUFFER = 0x00000100,
            FORMAT_MESSAGE_IGNORE_INSERTS = 0x00000200,
            FORMAT_MESSAGE_FROM_STRING = 0x00000400,
            FORMAT_MESSAGE_FROM_SYSTEM = 0x00001000,
            FORMAT_MESSAGE_ARGUMENT_ARRAY = 0x00002000;

        public const int 
            MB_RIGHT = 0x00080000,
            MB_RTLREADING = 0x00100000;
        
        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true, BestFitMapping=true)]            
        [SuppressMessage("Microsoft.Security", "CA2101:SpecifyMarshalingForPInvokeStringArguments")]
        [ResourceExposure(ResourceScope.None)]
        public static unsafe extern int FormatMessage(int dwFlags, SafeHandle lpSource, uint dwMessageId,
            int dwLanguageId, StringBuilder lpBuffer, int nSize, IntPtr[] arguments);        
        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true, BestFitMapping=true)]
        [ResourceExposure(ResourceScope.None)]
        public static extern int FormatMessage(int dwFlags, HandleRef lpSource, int dwMessageId,
            int dwLanguageId, StringBuilder lpBuffer, int nSize, IntPtr arguments);

        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Auto, BestFitMapping=true)]
        [ResourceExposure(ResourceScope.None)]
        public static extern void OutputDebugString(String message);

        [DllImport(ExternDll.User32, CharSet=System.Runtime.InteropServices.CharSet.Auto, BestFitMapping=true)]
        public static extern int MessageBox(HandleRef hWnd, string text, string caption, int type);
        
        [DllImport(ExternDll.Kernel32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public static extern bool CloseHandle(IntPtr handle);

        [DllImport(ExternDll.Kernel32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern bool CloseHandle(HandleRef handle);        

        [DllImport(ExternDll.Kernel32)]
        [ResourceExposure(ResourceScope.None)]
        public static extern bool QueryPerformanceCounter(out long value);
        
        [DllImport(ExternDll.Kernel32)]
        [ResourceExposure(ResourceScope.None)]
        public static extern bool QueryPerformanceFrequency(out long value);


        [DllImport(ExternDll.Kernel32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern SafeWaitHandle CreateSemaphore(NativeMethods.SECURITY_ATTRIBUTES lpSecurityAttributes, int initialCount, int maximumCount, String name);

        [DllImport(ExternDll.Kernel32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern SafeWaitHandle OpenSemaphore(/* DWORD */ int desiredAccess, bool inheritHandle, String name);

        [DllImport(ExternDll.Kernel32, SetLastError=true)]
        [ResourceExposure(ResourceScope.Machine)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal static extern bool ReleaseSemaphore(SafeWaitHandle handle, int releaseCount, out int previousCount);

        [StructLayout(LayoutKind.Sequential)]
        internal class PROCESS_INFORMATION {
            // The handles in PROCESS_INFORMATION are initialized in unmanaged functions.
            // We can't use SafeHandle here because Interop doesn't support [out] SafeHandles in structures/classes yet.            
            public IntPtr               hProcess = IntPtr.Zero;
            public IntPtr               hThread = IntPtr.Zero;
            public int                  dwProcessId = 0;
            public int                  dwThreadId = 0;

            // Note this class makes no attempt to free the handles
            // Use InitialSetHandle to copy to handles into SafeHandles

        }

        /* The following code has been removed to prevent FXCOP violations.
           The code is left here incase it needs to be resurrected.

        // From file src\services\timers\system\timers\safenativemethods.cs
        public delegate void TimerAPCProc(IntPtr argToCompletionRoutine, int timerLowValue, int timerHighValue);
        */
    }
    }
