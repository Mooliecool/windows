//------------------------------------------------------------------------------
// <copyright file="NativeMethods.cs" company="Microsoft">
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
    using System;
    using System.Text;
    using System.Threading;
    using System.Globalization;
    using System.Runtime.Remoting;
    using System.Runtime.InteropServices;
    using System.Runtime.CompilerServices;
    using System.Runtime.ConstrainedExecution;
    using System.Diagnostics;
    using System.Diagnostics.CodeAnalysis;
    using System.ComponentModel;
    using System.Security.Permissions;
    using Microsoft.Win32.SafeHandles;

    // not public!
    [HostProtection(MayLeakOnAbort = true)]
    internal static class NativeMethods {
        public static readonly IntPtr INVALID_HANDLE_VALUE = new IntPtr(-1);

        public const int GENERIC_READ = unchecked(((int)0x80000000));
        public const int GENERIC_WRITE = (0x40000000);

        public const int FILE_SHARE_READ = 0x00000001;
        public const int FILE_SHARE_WRITE = 0x00000002;
        public const int FILE_SHARE_DELETE = 0x00000004;

        public const int S_OK = 0x0;
        public const int E_ABORT = unchecked ((int)0x80004004);
        public const int E_NOTIMPL = unchecked((int)0x80004001);

        public const int CREATE_ALWAYS = 2;

        public const int FILE_ATTRIBUTE_NORMAL = 0x00000080;

        public const int STARTF_USESTDHANDLES = 0x00000100;

        public const int STD_INPUT_HANDLE = -10;
        public const int STD_OUTPUT_HANDLE = -11;
        public const int STD_ERROR_HANDLE = -12;

        public const int STILL_ACTIVE = 0x00000103;
        public const int SW_HIDE = 0;

        public const int WAIT_OBJECT_0    = 0x00000000;
        public const int WAIT_FAILED      = unchecked((int)0xFFFFFFFF);
        public const int WAIT_TIMEOUT     = 0x00000102;
        public const int WAIT_ABANDONED   = 0x00000080;
        public const int WAIT_ABANDONED_0 = WAIT_ABANDONED;

        // MoveFile Parameter
        public const int MOVEFILE_REPLACE_EXISTING = 0x00000001;

        // copied from winerror.h
        public const int ERROR_CLASS_ALREADY_EXISTS = 1410;
        public const int ERROR_NONE_MAPPED = 1332;
        public const int ERROR_INSUFFICIENT_BUFFER      = 122;
        public const int ERROR_PROC_NOT_FOUND           = 127;
        public const int ERROR_BAD_EXE_FORMAT           = 193;
        public const int ERROR_INVALID_NAME             = 0x7B;
        public const int MAX_PATH                       = 260;


        [StructLayout(LayoutKind.Sequential)]
        internal class STARTUPINFO {
            public int cb;
            public IntPtr lpReserved = IntPtr.Zero;
            public IntPtr lpDesktop = IntPtr.Zero;
            public IntPtr lpTitle = IntPtr.Zero;
            public int dwX = 0;
            public int dwY = 0;
            public int dwXSize = 0;
            public int dwYSize = 0;
            public int dwXCountChars = 0;
            public int dwYCountChars = 0;
            public int dwFillAttribute = 0;
            public int dwFlags = 0;
            public short wShowWindow = 0;
            public short cbReserved2 = 0;
            public IntPtr lpReserved2 = IntPtr.Zero;
            public SafeFileHandle hStdInput = new SafeFileHandle(IntPtr.Zero, false);
            public SafeFileHandle hStdOutput = new SafeFileHandle(IntPtr.Zero, false);
            public SafeFileHandle hStdError = new SafeFileHandle(IntPtr.Zero, false);

            public STARTUPINFO() {
                cb = Marshal.SizeOf(this);
            } 
            
            public void Dispose() {                
                // close the handles created for child process
                if(hStdInput != null && !hStdInput.IsInvalid) {
                    hStdInput.Close();
                    hStdInput = null;
                }

                if(hStdOutput != null && !hStdOutput.IsInvalid) {
                    hStdOutput.Close();
                    hStdOutput = null;
                }

                if(hStdError != null && !hStdError.IsInvalid) {
                    hStdError.Close();
                    hStdError = null;
                }                
            }
        }

        //
        // DACL related stuff
        //
        [StructLayout(LayoutKind.Sequential)]
        internal class SECURITY_ATTRIBUTES {
                public int nLength = 12;
                public SafeLocalMemHandle lpSecurityDescriptor = new SafeLocalMemHandle(IntPtr.Zero, false);
                public bool bInheritHandle = false;
        }

        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern bool GetExitCodeProcess(SafeProcessHandle processHandle, out int exitCode);

        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern bool GetProcessTimes(SafeProcessHandle handle, out long creation, out long exit, out long kernel, out long user);

        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Ansi, SetLastError=true)]
        public static extern IntPtr GetStdHandle(int whichHandle);

        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern int GetConsoleCP();
        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern int GetConsoleOutputCP();
        [DllImport(ExternDll.Kernel32, ExactSpelling=true, SetLastError=true)]
        public static extern int WaitForSingleObject(SafeProcessHandle handle, int timeout);

        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern bool CreatePipe(out SafeFileHandle hReadPipe, out SafeFileHandle hWritePipe, SECURITY_ATTRIBUTES lpPipeAttributes, int nSize);

        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true, BestFitMapping=false)]
        public static extern bool CreateProcess(
            [MarshalAs(UnmanagedType.LPTStr)]
            string lpApplicationName,                   // LPCTSTR
            StringBuilder lpCommandLine,                // LPTSTR - note: CreateProcess might insert a null somewhere in this string
            SECURITY_ATTRIBUTES lpProcessAttributes,    // LPSECURITY_ATTRIBUTES
            SECURITY_ATTRIBUTES lpThreadAttributes,     // LPSECURITY_ATTRIBUTES
            bool bInheritHandles,                        // BOOL
            int dwCreationFlags,                        // DWORD
            IntPtr lpEnvironment,                       // LPVOID
            [MarshalAs(UnmanagedType.LPTStr)]           
            string lpCurrentDirectory,                  // LPCTSTR
            STARTUPINFO lpStartupInfo,                  // LPSTARTUPINFO
            SafeNativeMethods.PROCESS_INFORMATION lpProcessInformation    // LPPROCESS_INFORMATION
        );
           
        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern bool TerminateProcess(SafeProcessHandle processHandle, int exitCode);


        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern int GetCurrentProcessId();
        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Ansi, SetLastError=true)]
        public static extern IntPtr GetCurrentProcess();

        public readonly static HandleRef NullHandleRef = new HandleRef(null, IntPtr.Zero);

        
        // copied from winbase.h
        public const int FORMAT_MESSAGE_ALLOCATE_BUFFER = 0x00000100;
        public const int FORMAT_MESSAGE_IGNORE_INSERTS  = 0x00000200;
        public const int FORMAT_MESSAGE_FROM_STRING     = 0x00000400;
        public const int FORMAT_MESSAGE_FROM_HMODULE    = 0x00000800;
        public const int FORMAT_MESSAGE_FROM_SYSTEM     = 0x00001000;
        public const int FORMAT_MESSAGE_ARGUMENT_ARRAY  = 0x00002000;
        public const int FORMAT_MESSAGE_MAX_WIDTH_MASK  = 0x000000FF;
        public const int LOAD_WITH_ALTERED_SEARCH_PATH  = 0x00000008;
        public const int LOAD_LIBRARY_AS_DATAFILE       = 0x00000002;

        public const int SEEK_READ = 0x2;
        public const int FORWARDS_READ = 0x4;
        public const int BACKWARDS_READ = 0x8;
        public const int ERROR_EVENTLOG_FILE_CHANGED = 1503;


        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Ansi, SetLastError=true, BestFitMapping=false)]    
        public static extern bool DuplicateHandle(
            HandleRef hSourceProcessHandle,
            HandleRef hSourceHandle,
            HandleRef hTargetProcess,
            out SafeProcessHandle targetHandle,
            int dwDesiredAccess,
            bool bInheritHandle,
            int dwOptions
        );

        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Ansi, SetLastError=true, BestFitMapping=false)]    
        public static extern bool DuplicateHandle(
            HandleRef hSourceProcessHandle,
            SafeHandle hSourceHandle,
            HandleRef hTargetProcess,
            out SafeFileHandle targetHandle,
            int dwDesiredAccess,
            bool bInheritHandle,
            int dwOptions
        );

        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Ansi, SetLastError=true, BestFitMapping=false)]    
        public static extern bool DuplicateHandle(
            HandleRef hSourceProcessHandle,
            SafeHandle hSourceHandle,
            HandleRef hTargetProcess,
            out SafeWaitHandle targetHandle,
            int dwDesiredAccess,
            bool bInheritHandle,
            int dwOptions
        );
        


        public const int PROCESS_TERMINATE = 0x0001;
        public const int PROCESS_CREATE_THREAD = 0x0002;
        public const int PROCESS_SET_SESSIONID = 0x0004;
        public const int PROCESS_VM_OPERATION = 0x0008;
        public const int PROCESS_VM_READ = 0x0010;
        public const int PROCESS_VM_WRITE = 0x0020;
        public const int PROCESS_DUP_HANDLE = 0x0040;
        public const int PROCESS_CREATE_PROCESS = 0x0080;
        public const int PROCESS_SET_QUOTA = 0x0100;
        public const int PROCESS_SET_INFORMATION = 0x0200;
        public const int PROCESS_QUERY_INFORMATION = 0x0400;
        public const int STANDARD_RIGHTS_REQUIRED = 0x000F0000;
        public const int SYNCHRONIZE = 0x00100000;
        public const int PROCESS_ALL_ACCESS = STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0xFFF;

        public const int SEMAPHORE_MODIFY_STATE = 0x00000002;
        public const int EVENT_MODIFY_STATE     = 0x00000002;
        public const int MUTEX_MODIFY_STATE     = 0x00000001;       


        public const int ERROR_BROKEN_PIPE = 109;
        public const int ERROR_NO_DATA = 232;
        public const int ERROR_HANDLE_EOF = 38;
        public const int ERROR_IO_INCOMPLETE = 996;
        public const int ERROR_IO_PENDING = 997;
        public const int ERROR_FILE_EXISTS = 0x50;
        public const int ERROR_FILENAME_EXCED_RANGE = 0xCE;  // filename too long.
        public const int ERROR_MORE_DATA = 234;
        public const int ERROR_CANCELLED = 1223;
        public const int ERROR_FILE_NOT_FOUND = 2;
        public const int ERROR_PATH_NOT_FOUND = 3;
        public const int ERROR_ACCESS_DENIED = 5;
        public const int ERROR_INVALID_HANDLE = 6;
        public const int ERROR_NOT_ENOUGH_MEMORY = 8;
        public const int ERROR_SHARING_VIOLATION = 32;
        public const int ERROR_OPERATION_ABORTED = 995;
        public const int ERROR_NO_ASSOCIATION = 1155;
        public const int ERROR_DLL_NOT_FOUND = 1157;
        public const int ERROR_DDE_FAIL = 1156;
        public const int ERROR_INVALID_PARAMETER = 87;
        public const int ERROR_PARTIAL_COPY = 299;
        public const int ERROR_SUCCESS = 0;
        public const int ERROR_ALREADY_EXISTS = 183;
        public const int ERROR_COUNTER_TIMEOUT = 1121;
        public const int RPC_S_SERVER_UNAVAILABLE = 1722;
        public const int RPC_S_CALL_FAILED = 1726;


        public const int DUPLICATE_CLOSE_SOURCE = 1;
        public const int DUPLICATE_SAME_ACCESS  = 2;



    }
}

