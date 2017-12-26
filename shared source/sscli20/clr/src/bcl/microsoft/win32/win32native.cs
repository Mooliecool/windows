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
** Class:  Microsoft.Win32.Win32Native
**
**
** Purpose: The CLR wrapper for all Win32 (Win2000, NT4, Win9x, 
**          as well as ROTOR-style Unix PAL, etc.) native operations
**
**
===========================================================*/
/**
 * Notes to PInvoke users:  Getting the syntax exactly correct is crucial, and
 * more than a little confusing.  Here's some guidelines.
 *
 * For handles, you should use a SafeHandle subclass specific to your handle
 * type.  For files, we have the following set of interesting definitions:
 *
 *  [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
 *  private static extern SafeFileHandle CreateFile(...);
 *
 *  [DllImport(KERNEL32, SetLastError=true)]
 *  unsafe internal static extern int ReadFile(SafeFileHandle handle, ...);
 *
 *  [DllImport(KERNEL32, SetLastError=true)]
 *  internal static extern bool CloseHandle(IntPtr handle);
 * 
 * P/Invoke will create the SafeFileHandle instance for you and assign the 
 * return value from CreateFile into the handle atomically.  When we call 
 * ReadFile, P/Invoke will increment a ref count, make the call, then decrement
 * it (preventing handle recycling vulnerabilities).  Then SafeFileHandle's
 * ReleaseHandle method will call CloseHandle, passing in the handle field
 * as an IntPtr.
 *
 * If for some reason you cannot use a SafeHandle subclass for your handles,
 * then use IntPtr as the handle type (or possibly HandleRef - understand when
 * to use GC.KeepAlive).  If your code will run in SQL Server (or any other
 * long-running process that can't be recycled easily), use a constrained 
 * execution region to prevent thread aborts while allocating your 
 * handle, and consider making your handle wrapper subclass 
 * CriticalFinalizerObject to ensure you can free the handle.  As you can 
 * probably guess, SafeHandle  will save you a lot of headaches if your code 
 * needs to be robust to thread aborts and OOM.
 *
 *
 * If you have a method that takes a native struct, you have two options for
 * declaring that struct.  You can make it a value type ('struct' in CSharp),
 * or a reference type ('class').  This choice doesn't seem very interesting, 
 * but your function prototype must use different syntax depending on your 
 * choice.  For example, if your native method is prototyped as such:
 *
 *    bool GetVersionEx(OSVERSIONINFO & lposvi);
 *
 *
 * you must use EITHER THIS OR THE NEXT syntax:
 *
 *    [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
 *    internal struct OSVERSIONINFO {  ...  }
 *
 *    [DllImport(KERNEL32, CharSet=CharSet.Auto)]
 *    internal static extern bool GetVersionEx(ref OSVERSIONINFO lposvi);
 *
 * OR:
 *
 *    [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
 *    internal class OSVERSIONINFO {  ...  }
 *
 *    [DllImport(KERNEL32, CharSet=CharSet.Auto)]
 *    internal static extern bool GetVersionEx([In, Out] OSVERSIONINFO lposvi);
 *
 * Note that classes require being marked as [In, Out] while value types must
 * be passed as ref parameters.
 *
 * Also note the CharSet.Auto on GetVersionEx - while it does not take a String
 * as a parameter, the OSVERSIONINFO contains an embedded array of TCHARs, so
 * the size of the struct varies on different platforms, and there's a
 * GetVersionExA & a GetVersionExW.  Also, the OSVERSIONINFO struct has a sizeof
 * field so the OS can ensure you've passed in the correctly-sized copy of an
 * OSVERSIONINFO.  You must explicitly set this using Marshal.SizeOf(Object);
 *
 * For security reasons, if you're making a P/Invoke method to a Win32 method
 * that takes an ANSI String (or will be ANSI on Win9x) and that String is the
 * name of some resource you've done a security check on (such as a file name), 
 * you want to disable best fit mapping in WideCharToMultiByte.  Do this by
 * setting BestFitMapping=false in your DllImportAttribute.
 */

namespace Microsoft.Win32 {
    using System;
    using System.Security;
    using System.Security.Principal;
    using System.Text;
    using System.Configuration.Assemblies;
    using System.Runtime.Remoting;
    using System.Runtime.InteropServices;
    using System.Threading;
    using Microsoft.Win32.SafeHandles;
    using System.Runtime.ConstrainedExecution;
    using System.Runtime.Versioning;

    using BOOL = System.Int32;
    using DWORD = System.UInt32;
    using ULONG = System.UInt32;
    
    /**
     * Win32 encapsulation for MSCORLIB.
     */
    // Remove the default demands for all N/Direct methods with this
    // global declaration on the class.
    //
    [SuppressUnmanagedCodeSecurityAttribute()]
    internal static class Win32Native {


        // Win32 ACL-related constants:
        internal const int READ_CONTROL                    = 0x00020000;
        internal const int SYNCHRONIZE                     = 0x00100000;

        internal const int STANDARD_RIGHTS_READ            = READ_CONTROL;
        internal const int STANDARD_RIGHTS_WRITE           = READ_CONTROL;
    
        // STANDARD_RIGHTS_REQUIRED  (0x000F0000L)
        // SEMAPHORE_ALL_ACCESS          (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3) 

        // SEMAPHORE and Event both use 0x0002
        // MUTEX uses 0x001 (MUTANT_QUERY_STATE)

        // Note that you may need to specify the SYNCHRONIZE bit as well
        // to be able to open a synchronization primitive.
        internal const int SEMAPHORE_MODIFY_STATE = 0x00000002;
        internal const int EVENT_MODIFY_STATE     = 0x00000002;
        internal const int MUTEX_MODIFY_STATE     = 0x00000001;
        internal const int MUTEX_ALL_ACCESS       = 0x001F0001;


        internal const int LMEM_FIXED    = 0x0000;
        internal const int LMEM_ZEROINIT = 0x0040;
        internal const int LPTR          = (LMEM_FIXED | LMEM_ZEROINIT);

        [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
        internal class OSVERSIONINFO {
            internal OSVERSIONINFO() {
                OSVersionInfoSize = (int)Marshal.SizeOf(this);
            }

            // The OSVersionInfoSize field must be set to Marshal.SizeOf(this)
            internal int OSVersionInfoSize = 0;
            internal int MajorVersion = 0;
            internal int MinorVersion = 0;
            internal int BuildNumber = 0;
            internal int PlatformId = 0;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst=128)]
            internal String CSDVersion = null;
        }

        [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
        internal class OSVERSIONINFOEX {
        
            public OSVERSIONINFOEX() {
                OSVersionInfoSize = (int)Marshal.SizeOf(this);
            }

            // The OSVersionInfoSize field must be set to Marshal.SizeOf(this)
            internal int OSVersionInfoSize = 0;
            internal int MajorVersion = 0;
            internal int MinorVersion = 0;
            internal int BuildNumber = 0;
            internal int PlatformId = 0;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst=128)]
            internal string CSDVersion = null;
            internal ushort ServicePackMajor = 0;
            internal ushort ServicePackMinor = 0;
            internal short SuiteMask = 0;
            internal byte ProductType = 0;
            internal byte Reserved = 0; 
        }

        [StructLayout(LayoutKind.Sequential)]
            internal struct SYSTEM_INFO {  
            internal int dwOemId;    // This is a union of a DWORD and a struct containing 2 WORDs.
            internal int dwPageSize;  
            internal IntPtr lpMinimumApplicationAddress;  
            internal IntPtr lpMaximumApplicationAddress;  
            internal IntPtr dwActiveProcessorMask;  
            internal int dwNumberOfProcessors;  
            internal int dwProcessorType;  
            internal int dwAllocationGranularity;  
            internal short wProcessorLevel;  
            internal short wProcessorRevision;
        }

        [StructLayout(LayoutKind.Sequential)]
        internal class SECURITY_ATTRIBUTES {
            internal int nLength = 0;
            internal unsafe byte * pSecurityDescriptor = null;
            internal int bInheritHandle = 0;
        }

        [StructLayout(LayoutKind.Sequential), Serializable]
        internal struct WIN32_FILE_ATTRIBUTE_DATA {
            internal int fileAttributes;
            internal uint ftCreationTimeLow;
            internal uint ftCreationTimeHigh;
            internal uint ftLastAccessTimeLow;
            internal uint ftLastAccessTimeHigh;
            internal uint ftLastWriteTimeLow;
            internal uint ftLastWriteTimeHigh;
            internal int fileSizeHigh;
            internal int fileSizeLow;
        }

        [StructLayout(LayoutKind.Sequential)]
        internal struct FILE_TIME {
            public FILE_TIME(long fileTime) {
                ftTimeLow = (uint) fileTime;
                ftTimeHigh = (uint) (fileTime >> 32);
            }

            public long ToTicks() {
                return ((long) ftTimeHigh << 32) + ftTimeLow;
            }

            internal uint ftTimeLow;
            internal uint ftTimeHigh;
        }



 #if !PLATFORM_UNIX
        internal const String DLLPREFIX = "";
        internal const String DLLSUFFIX = ".dll";
 #else // !PLATFORM_UNIX
  #if __APPLE__
        internal const String DLLPREFIX = "lib";
        internal const String DLLSUFFIX = ".dylib";
  #else
        internal const String DLLPREFIX = "lib";
        internal const String DLLSUFFIX = ".so";
  #endif
 #endif // !PLATFORM_UNIX

        internal const String KERNEL32 = DLLPREFIX + "rotor_pal" + DLLSUFFIX;
        internal const String USER32   = DLLPREFIX + "rotor_pal" + DLLSUFFIX;
        internal const String ADVAPI32 = DLLPREFIX + "rotor_pal" + DLLSUFFIX;
        internal const String OLE32    = DLLPREFIX + "rotor_pal" + DLLSUFFIX;
        internal const String OLEAUT32 = DLLPREFIX + "rotor_palrt" + DLLSUFFIX;
        internal const String SHIM     = DLLPREFIX + "sscoree" + DLLSUFFIX;
        internal const String MSCORWKS = DLLPREFIX + "mscorwks" + DLLSUFFIX;


        internal const String LSTRCPY  = "lstrcpy";
        internal const String LSTRCPYN = "lstrcpyn";
        internal const String LSTRLEN  = "lstrlen";
        internal const String LSTRLENA = "lstrlenA";
        internal const String LSTRLENW = "lstrlenW";
        internal const String MOVEMEMORY = "RtlMoveMemory";


        // From WinBase.h
        internal const int SEM_FAILCRITICALERRORS = 1;

        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern void SetLastError(int errorCode);

        [DllImport(KERNEL32, CharSet=CharSet.Auto, SetLastError=true, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern bool GetVersionEx([In, Out] OSVERSIONINFO ver);

        [DllImport(KERNEL32, CharSet=CharSet.Auto, SetLastError=true, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern bool GetVersionEx([In, Out] OSVERSIONINFOEX ver);

        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern void GetSystemInfo(ref SYSTEM_INFO lpSystemInfo);

        [DllImport(KERNEL32, CharSet=CharSet.Auto, BestFitMapping=true)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern int FormatMessage(int dwFlags, IntPtr lpSource,
                    int dwMessageId, int dwLanguageId, StringBuilder lpBuffer,
                    int nSize, IntPtr va_list_arguments);

        // Gets an error message for a Win32 error code.
        internal static String GetMessage(int errorCode) {
            StringBuilder sb = new StringBuilder(512);
            int result = Win32Native.FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS |
                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY,
                Win32Native.NULL, errorCode, 0, sb, sb.Capacity, Win32Native.NULL);
            if (result != 0) {
                // result is the # of characters copied to the StringBuilder on NT,
                // but on Win9x, it appears to be the number of MBCS bytes.
                // Just give up and return the String as-is...
                String s = sb.ToString();
                return s;
            }
            else {
                return Environment.GetResourceString("UnknownError_Num", errorCode);
            }
        }
        
        [DllImport(KERNEL32, EntryPoint="LocalAlloc")]
        [ResourceExposure(ResourceScope.None)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        internal static extern IntPtr LocalAlloc_NoSafeHandle(int uFlags, IntPtr sizetdwBytes);


        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal static extern IntPtr LocalFree(IntPtr handle);

        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal static extern void ZeroMemory(IntPtr handle, uint length);


        [DllImport(KERNEL32, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern uint GetTempPath(int bufferLen, StringBuilder buffer);

        [DllImport(KERNEL32, CharSet=CharSet.Auto, EntryPoint=LSTRCPY, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern IntPtr lstrcpy(IntPtr dst, String src);

        [DllImport(KERNEL32, CharSet=CharSet.Auto, EntryPoint=LSTRCPY, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern IntPtr lstrcpy(StringBuilder dst, IntPtr src);

        [DllImport(KERNEL32, CharSet=CharSet.Auto, EntryPoint=LSTRLEN)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern int lstrlen(sbyte [] ptr);

        [DllImport(KERNEL32, CharSet=CharSet.Auto, EntryPoint=LSTRLEN)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern int lstrlen(IntPtr ptr);

        [DllImport(KERNEL32, CharSet=CharSet.Ansi, EntryPoint=LSTRLENA)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern int lstrlenA(IntPtr ptr);

        [DllImport(KERNEL32, CharSet=CharSet.Unicode, EntryPoint=LSTRLENW)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern int lstrlenW(IntPtr ptr);

        [DllImport(Win32Native.OLEAUT32, CharSet=CharSet.Unicode)]
        [ResourceExposure(ResourceScope.None)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]            
        internal static extern IntPtr SysAllocStringLen(String src, int len);  // BSTR

        [DllImport(Win32Native.OLEAUT32)]
        [ResourceExposure(ResourceScope.None)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal static extern int SysStringLen(IntPtr bstr);

        [DllImport(Win32Native.OLEAUT32)]
        [ResourceExposure(ResourceScope.None)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal static extern void SysFreeString(IntPtr bstr);

        [DllImport(KERNEL32, CharSet=CharSet.Unicode, EntryPoint=MOVEMEMORY)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern void CopyMemoryUni(IntPtr pdst, String psrc, IntPtr sizetcb);

        [DllImport(KERNEL32, CharSet=CharSet.Unicode, EntryPoint=MOVEMEMORY)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern void CopyMemoryUni(StringBuilder pdst,
                    IntPtr psrc, IntPtr sizetcb);

        [DllImport(KERNEL32, CharSet=CharSet.Ansi, EntryPoint=MOVEMEMORY, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern void CopyMemoryAnsi(IntPtr pdst, String psrc, IntPtr sizetcb);

        [DllImport(KERNEL32, CharSet=CharSet.Ansi, EntryPoint=MOVEMEMORY, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern void CopyMemoryAnsi(StringBuilder pdst,
                    IntPtr psrc, IntPtr sizetcb);


        [DllImport(KERNEL32)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern int GetACP();

        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern bool SetEvent(SafeWaitHandle handle);

        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern bool ResetEvent(SafeWaitHandle handle);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)] // Machine or none based on the value of "name"
        internal static extern SafeWaitHandle CreateEvent(SECURITY_ATTRIBUTES lpSecurityAttributes, bool isManualReset, bool initialState, String name);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern SafeWaitHandle OpenEvent(/* DWORD */ int desiredAccess, bool inheritHandle, String name);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]            
        internal static extern SafeWaitHandle CreateMutex(SECURITY_ATTRIBUTES lpSecurityAttributes, bool initialOwner, String name);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern SafeWaitHandle OpenMutex(/* DWORD */ int desiredAccess, bool inheritHandle, String name);
  
        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]            
        internal static extern bool ReleaseMutex(SafeWaitHandle handle);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern int GetFullPathName([In] char[] path, int numBufferChars, [Out] char[] buffer, IntPtr mustBeZero);
        
        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal unsafe static extern int GetFullPathName(char* path, int numBufferChars, char* buffer, IntPtr mustBeZero);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern int GetLongPathName(String path, StringBuilder longPathBuffer, int bufferLength);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern int GetLongPathName([In] char[] path, [Out] char [] longPathBuffer, int bufferLength);


        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal unsafe static extern int GetLongPathName(char* path, char* longPathBuffer, int bufferLength);

        // Disallow access to all non-file devices from methods that take
        // a String.  This disallows DOS devices like "con:", "com1:", 
        // "lpt1:", etc.  Use this to avoid security problems, like allowing
        // a web client asking a server for "http://server/com1.aspx" and
        // then causing a worker process to hang.
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal static SafeFileHandle SafeCreateFile(String lpFileName,
                    int dwDesiredAccess, System.IO.FileShare dwShareMode,
                    SECURITY_ATTRIBUTES securityAttrs, System.IO.FileMode dwCreationDisposition,
                    int dwFlagsAndAttributes, IntPtr hTemplateFile)
        {
            SafeFileHandle handle = CreateFile( lpFileName, dwDesiredAccess, dwShareMode,
                                securityAttrs, dwCreationDisposition,
                                dwFlagsAndAttributes, hTemplateFile );

            if (!handle.IsInvalid)
            {
                int fileType = Win32Native.GetFileType(handle);
                if (fileType != Win32Native.FILE_TYPE_DISK) {
                    handle.Dispose();
                    throw new NotSupportedException(Environment.GetResourceString("NotSupported_FileStreamOnNonFiles"));
                }
            }

            return handle;
        }            

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal static SafeFileHandle UnsafeCreateFile(String lpFileName,
                    int dwDesiredAccess, System.IO.FileShare dwShareMode,
                    SECURITY_ATTRIBUTES securityAttrs, System.IO.FileMode dwCreationDisposition,
                    int dwFlagsAndAttributes, IntPtr hTemplateFile)
        {
            SafeFileHandle handle = CreateFile( lpFileName, dwDesiredAccess, dwShareMode,
                                securityAttrs, dwCreationDisposition,
                                dwFlagsAndAttributes, hTemplateFile );

            return handle;
        }            
    
        // Do not use these directly, use the safe or unsafe versions above.
        // The safe version does not support devices (aka if will only open
        // files on disk), while the unsafe version give you the full semantic
        // of the native version.
        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        private static extern SafeFileHandle CreateFile(String lpFileName,
                    int dwDesiredAccess, System.IO.FileShare dwShareMode,
                    SECURITY_ATTRIBUTES securityAttrs, System.IO.FileMode dwCreationDisposition,
                    int dwFlagsAndAttributes, IntPtr hTemplateFile);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern SafeFileMappingHandle CreateFileMapping(SafeFileHandle hFile, IntPtr lpAttributes, uint fProtect, uint dwMaximumSizeHigh, uint dwMaximumSizeLow, String lpName);

        [DllImport(KERNEL32, SetLastError=true, ExactSpelling=true)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern SafeViewOfFileHandle MapViewOfFile(
            SafeFileMappingHandle handle, uint dwDesiredAccess, uint dwFileOffsetHigh, uint dwFileOffsetLow, UIntPtr dwNumerOfBytesToMap);

        [DllImport(KERNEL32, ExactSpelling=true)]
        [ResourceExposure(ResourceScope.Machine)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal static extern bool UnmapViewOfFile(IntPtr lpBaseAddress );

        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.Machine)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal static extern bool CloseHandle(IntPtr handle);

        [DllImport(KERNEL32)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern int GetFileType(SafeFileHandle handle);

        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern bool SetEndOfFile(SafeFileHandle hFile);

        [DllImport(KERNEL32, SetLastError=true, EntryPoint="SetFilePointer")]
        [ResourceExposure(ResourceScope.None)]
        private unsafe static extern int SetFilePointerWin32(SafeFileHandle handle, int lo, int * hi, int origin);
        
        [ResourceExposure(ResourceScope.None)]
        internal unsafe static long SetFilePointer(SafeFileHandle handle, long offset, System.IO.SeekOrigin origin, out int hr) {
            hr = 0;
            int lo = (int) offset;
            int hi = (int) (offset >> 32);
            lo = SetFilePointerWin32(handle, lo, &hi, (int) origin);

            if (lo == -1 && ((hr = Marshal.GetLastWin32Error()) != 0))
                return -1;
            return (long) (((ulong) ((uint) hi)) << 32) | ((uint) lo);
        }

        // Note there are two different ReadFile prototypes - this is to use 
        // the type system to force you to not trip across a "feature" in 
        // Win32's async IO support.  You can't do the following three things
        // simultaneously: overlapped IO, free the memory for the overlapped 
        // struct in a callback (or an EndRead method called by that callback), 
        // and pass in an address for the numBytesRead parameter.  

        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        unsafe internal static extern int ReadFile(SafeFileHandle handle, byte* bytes, int numBytesToRead, IntPtr numBytesRead_mustBeZero, NativeOverlapped* overlapped);

        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        unsafe internal static extern int ReadFile(SafeFileHandle handle, byte* bytes, int numBytesToRead, out int numBytesRead, IntPtr mustBeZero);
        
        // Note there are two different WriteFile prototypes - this is to use 
        // the type system to force you to not trip across a "feature" in 
        // Win32's async IO support.  You can't do the following three things
        // simultaneously: overlapped IO, free the memory for the overlapped 
        // struct in a callback (or an EndWrite method called by that callback),
        // and pass in an address for the numBytesRead parameter.  

        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        internal static unsafe extern int WriteFile(SafeFileHandle handle, byte* bytes, int numBytesToWrite, IntPtr numBytesWritten_mustBeZero, NativeOverlapped* lpOverlapped);

        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        internal static unsafe extern int WriteFile(SafeFileHandle handle, byte* bytes, int numBytesToWrite, out int numBytesWritten, IntPtr mustBeZero);


        // NOTE: The out parameters are PULARGE_INTEGERs and may require
        // some byte munging magic.
        [DllImport(KERNEL32, CharSet=CharSet.Auto, SetLastError=true, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern bool GetDiskFreeSpaceEx(String drive, out long freeBytesForUser, out long totalBytes, out long freeBytes);

        [DllImport(KERNEL32, CharSet=CharSet.Auto, SetLastError=true, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern int GetDriveType(String drive);

        [DllImport(KERNEL32, CharSet=CharSet.Auto, SetLastError=true, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern bool GetVolumeInformation(String drive, StringBuilder volumeName, int volumeNameBufLen, out int volSerialNumber, out int maxFileNameLen, out int fileSystemFlags, StringBuilder fileSystemName, int fileSystemNameBufLen);

        [DllImport(KERNEL32, CharSet=CharSet.Auto, SetLastError=true, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern bool SetVolumeLabel(String driveLetter, String volumeName);


        [DllImport(KERNEL32, CharSet=CharSet.Unicode, SetLastError=true, EntryPoint="PAL_GetPALDirectoryW", BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern int GetSystemDirectory(StringBuilder sb, int length);

        [DllImport(OLEAUT32, CharSet=CharSet.Unicode, SetLastError=true, EntryPoint="PAL_FetchConfigurationStringW")]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern bool FetchConfigurationString(bool perMachine, String parameterName, StringBuilder parameterValue, int parameterValueLength);

        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        internal unsafe static extern bool SetFileTime(SafeFileHandle hFile, FILE_TIME* creationTime,
                    FILE_TIME* lastAccessTime, FILE_TIME* lastWriteTime);

        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern int GetFileSize(SafeFileHandle hFile, out int highSize);

        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern bool LockFile(SafeFileHandle handle, int offsetLow, int offsetHigh, int countLow, int countHigh);
        
        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern bool UnlockFile(SafeFileHandle handle, int offsetLow, int offsetHigh, int countLow, int countHigh);
  
        internal static readonly IntPtr INVALID_HANDLE_VALUE = new IntPtr(-1);  // WinBase.h
        internal static readonly IntPtr NULL = IntPtr.Zero;

        // Note, these are #defines used to extract handles, and are NOT handles.
        internal const int STD_INPUT_HANDLE = -10;
        internal const int STD_OUTPUT_HANDLE = -11;
        internal const int STD_ERROR_HANDLE = -12;

        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.Process)]
        internal static extern IntPtr GetStdHandle(int nStdHandle);  // param is NOT a handle, but it returns one!

        // From wincon.h
        internal const int CTRL_C_EVENT = 0;
        internal const int CTRL_BREAK_EVENT = 1;
        internal const int CTRL_CLOSE_EVENT = 2;
        internal const int CTRL_LOGOFF_EVENT = 5;
        internal const int CTRL_SHUTDOWN_EVENT = 6;
        internal const short KEY_EVENT = 1;

        // From WinBase.h
        internal const int FILE_TYPE_DISK = 0x0001;
        internal const int FILE_TYPE_CHAR = 0x0002;
        internal const int FILE_TYPE_PIPE = 0x0003;

        internal const int REPLACEFILE_WRITE_THROUGH = 0x1;
        internal const int REPLACEFILE_IGNORE_MERGE_ERRORS = 0x2;

        private const int FORMAT_MESSAGE_IGNORE_INSERTS = 0x00000200;
        private const int FORMAT_MESSAGE_FROM_SYSTEM    = 0x00001000;
        private const int FORMAT_MESSAGE_ARGUMENT_ARRAY = 0x00002000;

        // Constants from WinNT.h
        internal const int FILE_ATTRIBUTE_READONLY      = 0x00000001;
        internal const int FILE_ATTRIBUTE_DIRECTORY     = 0x00000010;
        internal const int FILE_ATTRIBUTE_REPARSE_POINT = 0x00000400;

        internal const int IO_REPARSE_TAG_MOUNT_POINT = unchecked((int)0xA0000003);

        internal const int PAGE_READWRITE = 0x04;

        internal const int MEM_COMMIT  =  0x1000;
        internal const int MEM_RESERVE =  0x2000;
        internal const int MEM_RELEASE =  0x8000;
        internal const int MEM_FREE    = 0x10000;

        // Error codes from WinError.h
        internal const int ERROR_SUCCESS = 0x0;
        internal const int ERROR_INVALID_FUNCTION = 0x1;
        internal const int ERROR_FILE_NOT_FOUND = 0x2;
        internal const int ERROR_PATH_NOT_FOUND = 0x3;
        internal const int ERROR_ACCESS_DENIED  = 0x5;
        internal const int ERROR_INVALID_HANDLE = 0x6;
        internal const int ERROR_NOT_ENOUGH_MEMORY = 0x8;
        internal const int ERROR_INVALID_DATA = 0xd;
        internal const int ERROR_INVALID_DRIVE = 0xf;
        internal const int ERROR_NO_MORE_FILES = 0x12;
        internal const int ERROR_NOT_READY = 0x15;
        internal const int ERROR_BAD_LENGTH = 0x18;
        internal const int ERROR_SHARING_VIOLATION = 0x20;
        internal const int ERROR_NOT_SUPPORTED = 0x32;
        internal const int ERROR_FILE_EXISTS = 0x50;
        internal const int ERROR_INVALID_PARAMETER = 0x57;
        internal const int ERROR_CALL_NOT_IMPLEMENTED = 0x78;
        internal const int ERROR_INSUFFICIENT_BUFFER = 0x7A;
        internal const int ERROR_INVALID_NAME = 0x7B;
        internal const int ERROR_BAD_PATHNAME = 0xA1;
        internal const int ERROR_ALREADY_EXISTS = 0xB7;
        internal const int ERROR_ENVVAR_NOT_FOUND = 0xCB;
        internal const int ERROR_FILENAME_EXCED_RANGE = 0xCE;  // filename too long.
        internal const int ERROR_NO_DATA = 0xE8;
        internal const int ERROR_PIPE_NOT_CONNECTED = 0xE9;
        internal const int ERROR_MORE_DATA = 0xEA;
        internal const int ERROR_OPERATION_ABORTED = 0x3E3;  // 995; For IO Cancellation
        internal const int ERROR_NO_TOKEN = 0x3f0;
        internal const int ERROR_DLL_INIT_FAILED = 0x45A;
        internal const int ERROR_NON_ACCOUNT_SID = 0x4E9;
        internal const int ERROR_NOT_ALL_ASSIGNED = 0x514;
        internal const int ERROR_UNKNOWN_REVISION = 0x519;
        internal const int ERROR_INVALID_OWNER = 0x51B;
        internal const int ERROR_INVALID_PRIMARY_GROUP = 0x51C;
        internal const int ERROR_NO_SUCH_PRIVILEGE = 0x521;
        internal const int ERROR_PRIVILEGE_NOT_HELD = 0x522;
        internal const int ERROR_NONE_MAPPED = 0x534;
        internal const int ERROR_INVALID_ACL = 0x538;
        internal const int ERROR_INVALID_SID = 0x539;
        internal const int ERROR_INVALID_SECURITY_DESCR = 0x53A;
        internal const int ERROR_BAD_IMPERSONATION_LEVEL = 0x542;
        internal const int ERROR_CANT_OPEN_ANONYMOUS = 0x543;
        internal const int ERROR_NO_SECURITY_ON_OBJECT = 0x546;
        internal const int ERROR_TRUSTED_RELATIONSHIP_FAILURE = 0x6FD;

        // Error codes from ntstatus.h
        internal const uint STATUS_SOME_NOT_MAPPED = 0x00000107;
        internal const uint STATUS_NO_MEMORY = 0xC0000017;
        internal const uint STATUS_NONE_MAPPED = 0xC0000073;
        internal const uint STATUS_INSUFFICIENT_RESOURCES = 0xC000009A;
        internal const uint STATUS_ACCESS_DENIED = 0xC0000022;

        internal const int INVALID_FILE_SIZE     = -1;

        // From WinStatus.h
        internal const int STATUS_ACCOUNT_RESTRICTION = unchecked((int) 0xC000006E);

        // Use this to translate error codes like the above into HRESULTs like
        // 0x80070006 for ERROR_INVALID_HANDLE
        internal static int MakeHRFromErrorCode(int errorCode)
        {
            BCLDebug.Assert((0xFFFF0000 & errorCode) == 0, "This is an HRESULT, not an error code!");
            return unchecked(((int)0x80070000) | errorCode);
        }

        // Win32 Structs in N/Direct style
        [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto), Serializable]
        [BestFitMapping(false)]
        internal class WIN32_FIND_DATA {
            internal int  dwFileAttributes = 0;
            // ftCreationTime was a by-value FILETIME structure
            internal int  ftCreationTime_dwLowDateTime = 0 ;
            internal int  ftCreationTime_dwHighDateTime = 0;
            // ftLastAccessTime was a by-value FILETIME structure
            internal int  ftLastAccessTime_dwLowDateTime = 0;
            internal int  ftLastAccessTime_dwHighDateTime = 0;
            // ftLastWriteTime was a by-value FILETIME structure
            internal int  ftLastWriteTime_dwLowDateTime = 0;
            internal int  ftLastWriteTime_dwHighDateTime = 0;
            internal int  nFileSizeHigh = 0;
            internal int  nFileSizeLow = 0;
            // If the file attributes' reparse point flag is set, then
            // dwReserved0 is the file tag (aka reparse tag) for the 
            // reparse point.  Use this to figure out whether something is
            // a volume mount point or a symbolic link.
            internal int  dwReserved0 = 0;
            internal int  dwReserved1 = 0;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst=260)]
            internal String   cFileName = null;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst=14)]
            internal String   cAlternateFileName = null;
        }

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern bool CopyFile(
                    String src, String dst, bool failIfExists);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern bool CreateDirectory(
                    String path, SECURITY_ATTRIBUTES lpSecurityAttributes);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern bool DeleteFile(String path);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern bool ReplaceFile(String replacedFileName, String replacementFileName, String backupFileName, int dwReplaceFlags, IntPtr lpExclude, IntPtr lpReserved);

        [DllImport(ADVAPI32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern bool DecryptFile(String path, int reservedMustBeZero);

        [DllImport(ADVAPI32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern bool EncryptFile(String path);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern SafeFindHandle FindFirstFile(String fileName, [In, Out] Win32Native.WIN32_FIND_DATA data);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern bool FindNextFile(
                    SafeFindHandle hndFindFile,
                    [In, Out, MarshalAs(UnmanagedType.LPStruct)]
                    WIN32_FIND_DATA lpFindFileData);

        [DllImport(KERNEL32)]
        [ResourceExposure(ResourceScope.None)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal static extern bool FindClose(IntPtr handle);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern int GetCurrentDirectory(
                  int nBufferLength,
                  StringBuilder lpBuffer);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern bool GetFileAttributesEx(String name, int fileInfoLevel, ref WIN32_FILE_ATTRIBUTE_DATA lpFileInformation);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern bool SetFileAttributes(String name, int attr);

#if !PLATFORM_UNIX
        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern int GetLogicalDrives();
#endif // !PLATFORM_UNIX

        [DllImport(KERNEL32, CharSet=CharSet.Auto, SetLastError=true, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern uint GetTempFileName(String tmpPath, String prefix, uint uniqueIdOrZero, StringBuilder tmpFileName);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern bool MoveFile(String src, String dst);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern bool DeleteVolumeMountPoint(String mountPoint);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern bool RemoveDirectory(String path);

        [DllImport(KERNEL32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern bool SetCurrentDirectory(String path);

        [DllImport(KERNEL32, SetLastError=false)]
        [ResourceExposure(ResourceScope.Process)]
        internal static extern int SetErrorMode(int newMode);

        internal const int LCID_SUPPORTED = 0x00000002;  // supported locale ids

        [DllImport(KERNEL32)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern unsafe int WideCharToMultiByte(uint cp, uint flags, char* pwzSource, int cchSource, byte* pbDestBuffer, int cbDestBuffer, IntPtr null1, IntPtr null2);

        // A Win32 HandlerRoutine
        internal delegate bool ConsoleCtrlHandlerRoutine(int controlType);

        [DllImport(KERNEL32, SetLastError=true)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        [ResourceExposure(ResourceScope.Process)]
        internal static extern bool SetConsoleCtrlHandler(ConsoleCtrlHandlerRoutine handler, bool addOrRemove);

        [DllImport(KERNEL32, CharSet=CharSet.Auto, SetLastError=true, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Process)]
        internal static extern bool SetEnvironmentVariable(string lpName, string lpValue);
        
        [DllImport(KERNEL32, CharSet=CharSet.Auto, SetLastError=true, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.Machine)]
        internal static extern int GetEnvironmentVariable(string lpName, StringBuilder lpValue, int size);

        [DllImport(KERNEL32, CharSet=CharSet.Auto, SetLastError=true)]
        [ResourceExposure(ResourceScope.Process)]
        internal static extern uint GetCurrentProcessId();

        [DllImport(ADVAPI32, CharSet=CharSet.Auto)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern bool GetUserName(StringBuilder lpBuffer, ref int nSize);

        [DllImport(KERNEL32, CharSet=CharSet.Auto, BestFitMapping=false)]
        internal extern static int GetComputerName(StringBuilder nameBuffer, ref int bufferSize);



        [DllImport(KERNEL32, SetLastError=false)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern uint GetConsoleCP();

        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.Process)]
        internal static extern bool SetConsoleCP(uint codePage);

        [DllImport(KERNEL32, SetLastError=false)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern uint GetConsoleOutputCP();

        [DllImport(KERNEL32, SetLastError=true)]
        [ResourceExposure(ResourceScope.Process)]
        internal static extern bool SetConsoleOutputCP(uint codePage);


        // managed cryptography wrapper around the PALRT cryptography api
        internal const int PAL_HCRYPTPROV = 123;

        internal const int CALG_MD2         = ((4 << 13) | 1);
        internal const int CALG_MD4         = ((4 << 13) | 2);
        internal const int CALG_MD5         = ((4 << 13) | 3);
        internal const int CALG_SHA         = ((4 << 13) | 4);
        internal const int CALG_SHA1        = ((4 << 13) | 4);
        internal const int CALG_MAC         = ((4 << 13) | 5);
        internal const int CALG_SSL3_SHAMD5 = ((4 << 13) | 8);
        internal const int CALG_HMAC        = ((4 << 13) | 9);

        internal const int HP_ALGID         = 0x0001;
        internal const int HP_HASHVAL       = 0x0002;
        internal const int HP_HASHSIZE      = 0x0004;

        [DllImport(OLEAUT32, CharSet=CharSet.Unicode, EntryPoint="CryptAcquireContextW")]
        [ResourceExposure(ResourceScope.Machine)]
        internal extern static bool CryptAcquireContext(out IntPtr hProv,
                           [MarshalAs(UnmanagedType.LPWStr)] string container,
                           [MarshalAs(UnmanagedType.LPWStr)] string provider,
                           int provType,
                           int flags);

        [DllImport(OLEAUT32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        internal extern static bool CryptReleaseContext( IntPtr hProv, int flags);

        [DllImport(OLEAUT32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        internal extern static bool CryptCreateHash(IntPtr hProv, int Algid, IntPtr hKey, int flags, out IntPtr hHash);

        [DllImport(OLEAUT32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        internal extern static bool CryptDestroyHash(IntPtr hHash);

        [DllImport(OLEAUT32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        internal extern static bool CryptHashData(IntPtr hHash,
                           [In, MarshalAs(UnmanagedType.LPArray)] byte[] data,
                           int length,
                           int flags);

        [DllImport(OLEAUT32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        internal extern static bool CryptGetHashParam(IntPtr hHash,
                           int param,
                           [Out, MarshalAs(UnmanagedType.LPArray)] byte[] digest,
                           ref int length,
                           int flags);

        [DllImport(OLEAUT32, SetLastError=true)]
        [ResourceExposure(ResourceScope.None)]
        internal extern static bool CryptGetHashParam(IntPtr hHash,
                           int param,
                           out int data,
                           ref int length,
                           int flags);

        [DllImport(KERNEL32, EntryPoint="PAL_Random")]
        [ResourceExposure(ResourceScope.None)]
        internal extern static bool Random(bool bStrong,
                           [Out, MarshalAs(UnmanagedType.LPArray)] byte[] buffer, int length);

    // Fusion APIs

        [DllImport(MSCORWKS, CharSet=CharSet.Unicode)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern int CreateAssemblyNameObject(out SafeFusionHandle ppEnum, String szAssemblyName, uint dwFlags, IntPtr pvReserved);

        [DllImport(MSCORWKS, CharSet=CharSet.Auto)]
        [ResourceExposure(ResourceScope.None)]
        internal static extern int CreateAssemblyEnum(out SafeFusionHandle ppEnum, SafeFusionHandle pAppCtx, SafeFusionHandle pName, uint dwFlags, IntPtr pvReserved);

    // Globalization APIs
        [DllImport(KERNEL32, CharSet=CharSet.Auto, BestFitMapping=false)]
        [ResourceExposure(ResourceScope.None)]
        internal extern static int GetCalendarInfo(
                                      int           Locale,     // locale
                                      int           Calendar,   // calendar identifier
                                      int           CalType,    // calendar type
                                      StringBuilder lpCalData,  // information buffer
                                      int           cchData,    // information buffer size
                                      IntPtr        lpValue     // data
                                    );
    }
}
