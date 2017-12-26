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

namespace System.Globalization {
    using System.IO;
    using Microsoft.Win32;
    using Microsoft.Win32.SafeHandles;
    using System.Runtime.InteropServices;
    using System.Runtime.CompilerServices;
    using System.Security;
    using System.Security.Permissions;
    using System.Runtime.Versioning;
    
    ////////////////////////////////////////////////////////////////////////////
    //
    // Open a Memory mapped file using read-only attribute.
    // The memory mapped file can be named or unnamed.
    // The end result will be a byte pointer pointing to the beginning of the specified file.
    //
    // DANGER DANGER DANGER
    // NECESSARY PERMISSIONS TO CREATE MEMORY MAPPED FILE ARE BYPASSED IN THIS CLASS.
    // PLEASE CHECK THAT YOU WON'T HAVE SECURITY PROBLEM IF YOU USE THIS CLASS IN UNTRUSTED 
    // SCENARIO.
    // 
    // ALSO, WE DON'T PROVIDE A MECHANISM FOR EXPLICITLY UNMAPPING THE MEMORY,
    // BUT THAT HAPPENS IF WE EVER FINALIZE THE SAFEHANDLE STORED IN AN INSTANCE
    // OF THIS CLASS.
    //
    ////////////////////////////////////////////////////////////////////////////
    internal sealed class MemoryMapFile {
        
        private unsafe byte* bytes;

        //
        // The only handle we keep it open is the mapped memory section and we close
        // both the stream and mapped file handle as the OS keep the memory section 
        // mapped even when closing the files.
        // The benefit for closing the file and stream handle is have flexability 
        // to rename the file while it is in use.
        //
        private SafeViewOfFileHandle viewOfFileHandle = null;
        private long                 fileSize         = 0;

        private const int PAGE_READONLY         = 0x02;
        private const int SECTION_MAP_READ    = 0x0004;

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal unsafe MemoryMapFile(String fileName) : this(fileName, null) {}

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal unsafe MemoryMapFile(String fileName, String fileMappingName) {
            //
            // Use native API to create the file directly.
            //
            SafeFileHandle fileHandle = Win32Native.UnsafeCreateFile(fileName, FileStream.GENERIC_READ, FileShare.Read, null, FileMode.Open, 0, IntPtr.Zero);
            int lastError = Marshal.GetLastWin32Error();
            if (fileHandle.IsInvalid) 
            {
                BCLDebug.Assert(false, "Failed to create file " + fileName + ", GetLastError = " + lastError);
                throw new InvalidOperationException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("InvalidOperation_UnexpectedWin32Error"), lastError));
            }

            int highSize;
            int lowSize = Win32Native.GetFileSize(fileHandle, out highSize);
            if (lowSize == Win32Native.INVALID_FILE_SIZE)
            {
                BCLDebug.Assert(false, "Failed to get the file size of " + fileName + ", GetLastError = " + lastError);
                throw new InvalidOperationException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("InvalidOperation_UnexpectedWin32Error"), lastError));
            }

            fileSize = (((long) highSize) << 32) | ((uint) lowSize);

            if (fileSize == 0)
            {
                // we cannot map zero size file. the caller should check for the file size.
                fileHandle.Close();
                return;
            }
            
            SafeFileMappingHandle fileMapHandle = Win32Native.CreateFileMapping(fileHandle, IntPtr.Zero, PAGE_READONLY, 0, 0, fileMappingName);
            fileHandle.Close();
            lastError = Marshal.GetLastWin32Error();
            if (fileMapHandle.IsInvalid) {
                BCLDebug.Assert(false, "Failed to create file mapping for file " + fileName + ", GetLastError = " + lastError);
                throw new InvalidOperationException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("InvalidOperation_UnexpectedWin32Error"), lastError));
            }
                                    
            viewOfFileHandle = Win32Native.MapViewOfFile(fileMapHandle, SECTION_MAP_READ, 0, 0, UIntPtr.Zero);
            lastError = Marshal.GetLastWin32Error();
            if (viewOfFileHandle.IsInvalid) {
                BCLDebug.Assert(false, "Failed to map a view of file " + fileName + ", GetLastError = " + lastError);
                throw new InvalidOperationException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("InvalidOperation_UnexpectedWin32Error"), lastError));
            }

            bytes = (byte*)viewOfFileHandle.DangerousGetHandle ();

            fileMapHandle.Close();
        }

        internal long FileSize { get { return fileSize; } }

        internal unsafe byte* GetBytePtr() {
            BCLDebug.Assert(bytes != null, "bytes should not be null");
            return (bytes);
        }
        
    }
}
