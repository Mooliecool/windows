/******************************** Module Header ********************************\
* Module Name:  Program.cs
* Project:      CSFileMappingClient
* Copyright (c) Microsoft Corporation.
* 
* File mapping is a mechanism for one-way or duplex inter-process communication 
* among two or more processes in the local machine. To share a file or memory, 
* all of the processes must use the name or the handle of the same file mapping
* object.
* 
* To share a file, the first process creates or opens a file by using the 
* CreateFile function. Next, it creates a file mapping object by using the 
* CreateFileMapping function, specifying the file handle and a name for the 
* file mapping object. The names of event, semaphore, mutex, waitable timer, 
* job, and file mapping objects share the same name space. Therefore, the 
* CreateFileMapping and OpenFileMapping functions fail if they specify a name
* that is in use by an object of another type.
* 
* To share memory that is not associated with a file, a process must use the 
* CreateFileMapping function and specify INVALID_HANDLE_VALUE as the hFile 
* parameter instead of an existing file handle. The corresponding file mapping 
* object accesses memory backed by the system paging file. You must specify a 
* size greater than zero when you use an hFile of INVALID_HANDLE_VALUE in a call 
* to CreateFileMapping.
* 
* Processes that share files or memory must create file views by using the 
* MapViewOfFile or MapViewOfFileEx function. They must coordinate their access
* using semaphores, mutexes, events, or some other mutual exclusion technique.
* 
* The VC# code sample demonstrates opening a file mapping object named 
* "Local\SampleMap" and reading the string written to the file mapping by other 
* process. Because the Base Class Library of .NET Framework 2/3/3.5 does not have 
* any public classes to operate on file mapping objects, you have to P/Invoke the 
* Windows APIs as shown in this code sample.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*******************************************************************************/

#region Using directives
using System;
using System.Runtime.InteropServices;
using System.Security;
using System.Security.Permissions;
using Microsoft.Win32.SafeHandles;
using System.Runtime.ConstrainedExecution;
using System.ComponentModel;
#endregion


namespace CSFileMappingClient
{
    class Program
    {
        // In terminal services: The name can have a "Global\" or "Local\" 
        // prefix to explicitly create the object in the global or session 
        // namespace. The remainder of the name can contain any character except
        // the backslash character (\). For more information, see: 
        // http://msdn.microsoft.com/en-us/library/aa366537.aspx
        internal const string MapPrefix = "Local\\";
        internal const string MapName = "SampleMap";
        internal const string FullMapName = MapPrefix + MapName;

        // File offset where the view is to begin.
        internal const uint ViewOffset = 0;

        // The number of bytes of a file mapping to map to the view. All bytes of 
        // the view must be within the maximum size of the file mapping object. 
        // If VIEW_SIZE is 0, the mapping extends from the offset (VIEW_OFFSET) 
        // to the end of the file mapping.
        internal const uint ViewSize = 1024;


        static void Main(string[] args)
        {
            SafeFileMappingHandle hMapFile = null;
            IntPtr pView = IntPtr.Zero;

            try
            {
                // Try to open the named file mapping.
                hMapFile = NativeMethod.OpenFileMapping(
                    FileMapAccess.FILE_MAP_READ,    // Read access
                    false,                          // Do not inherit the name
                    FullMapName                     // File mapping name
                    );

                if (hMapFile.IsInvalid)
                {
                    throw new Win32Exception();
                }

                Console.WriteLine("The file mapping ({0}) is opened", FullMapName);

                // Map a view of the file mapping into the address space of the 
                // current process.
                pView = NativeMethod.MapViewOfFile(
                    hMapFile,                       // Handle of the map object
                    FileMapAccess.FILE_MAP_READ,    // Read access
                    0,                              // High-order DWORD of file offset 
                    ViewOffset,                     // Low-order DWORD of file offset
                    ViewSize                        // Byte# to map to view
                    );

                if (pView == IntPtr.Zero)
                {
                    throw new Win32Exception();
                }

                Console.WriteLine("The file view is mapped");

                // Read and display the content in the view.
                string message = Marshal.PtrToStringUni(pView);
                Console.WriteLine("Read from the file mapping:\n\"{0}\"", message);

                // Wait to clean up resources and stop the process.
                Console.Write("Press ENTER to clean up resources and quit");
                Console.ReadLine();
            }
            catch (Exception ex)
            {
                Console.WriteLine("The process throws the error: {0}", ex.Message);
            }
            finally
            {
                if (hMapFile != null)
                {
                    if (pView != IntPtr.Zero)
                    {
                        // Unmap the file view.
                        NativeMethod.UnmapViewOfFile(pView);
                        pView = IntPtr.Zero;
                    }
                    // Close the file mapping object.
                    hMapFile.Close();
                    hMapFile = null;
                }
            }
        }


        #region Native API Signatures and Types

        /// <summary>
        /// Access rights for file mapping objects
        /// http://msdn.microsoft.com/en-us/library/aa366559.aspx
        /// </summary>
        [Flags]
        public enum FileMapAccess
        {
            FILE_MAP_COPY = 0x0001,
            FILE_MAP_WRITE = 0x0002,
            FILE_MAP_READ = 0x0004,
            FILE_MAP_ALL_ACCESS = 0x000F001F
        }


        /// <summary>
        /// Represents a wrapper class for a file mapping handle. 
        /// </summary>
        [SuppressUnmanagedCodeSecurity,
        HostProtection(SecurityAction.LinkDemand, MayLeakOnAbort = true)]
        internal sealed class SafeFileMappingHandle : SafeHandleZeroOrMinusOneIsInvalid
        {
            [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode = true)]
            private SafeFileMappingHandle()
                : base(true)
            {
            }

            [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode = true)]
            public SafeFileMappingHandle(IntPtr handle, bool ownsHandle)
                : base(ownsHandle)
            {
                base.SetHandle(handle);
            }

            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success),
            DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
            [return: MarshalAs(UnmanagedType.Bool)]
            private static extern bool CloseHandle(IntPtr handle);

            protected override bool ReleaseHandle()
            {
                return CloseHandle(base.handle);
            }
        }


        /// <summary>
        /// The class exposes Windows APIs used in this code sample.
        /// </summary>
        [SuppressUnmanagedCodeSecurity]
        internal class NativeMethod
        {
            /// <summary>
            /// Opens a named file mapping object.
            /// </summary>
            /// <param name="dwDesiredAccess">
            /// The access to the file mapping object. This access is checked against 
            /// any security descriptor on the target file mapping object.
            /// </param>
            /// <param name="bInheritHandle">
            /// If this parameter is TRUE, a process created by the CreateProcess 
            /// function can inherit the handle; otherwise, the handle cannot be 
            /// inherited.
            /// </param>
            /// <param name="lpName">
            /// The name of the file mapping object to be opened.
            /// </param>
            /// <returns>
            /// If the function succeeds, the return value is an open handle to the 
            /// specified file mapping object.
            /// </returns>
            [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
            public static extern SafeFileMappingHandle OpenFileMapping(
                FileMapAccess dwDesiredAccess, bool bInheritHandle, string lpName);


            /// <summary>
            /// Maps a view of a file mapping into the address space of a calling
            /// process.
            /// </summary>
            /// <param name="hFileMappingObject">
            /// A handle to a file mapping object. The CreateFileMapping and 
            /// OpenFileMapping functions return this handle.
            /// </param>
            /// <param name="dwDesiredAccess">
            /// The type of access to a file mapping object, which determines the 
            /// protection of the pages.
            /// </param>
            /// <param name="dwFileOffsetHigh">
            /// A high-order DWORD of the file offset where the view begins.
            /// </param>
            /// <param name="dwFileOffsetLow">
            /// A low-order DWORD of the file offset where the view is to begin.
            /// </param>
            /// <param name="dwNumberOfBytesToMap">
            /// The number of bytes of a file mapping to map to the view. All bytes 
            /// must be within the maximum size specified by CreateFileMapping.
            /// </param>
            /// <returns>
            /// If the function succeeds, the return value is the starting address 
            /// of the mapped view.
            /// </returns>
            [DllImport("Kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
            public static extern IntPtr MapViewOfFile(
                SafeFileMappingHandle hFileMappingObject,
                FileMapAccess dwDesiredAccess,
                uint dwFileOffsetHigh,
                uint dwFileOffsetLow,
                uint dwNumberOfBytesToMap);


            /// <summary>
            /// Unmaps a mapped view of a file from the calling process's address 
            /// space.
            /// </summary>
            /// <param name="lpBaseAddress">
            /// A pointer to the base address of the mapped view of a file that 
            /// is to be unmapped.
            /// </param>
            /// <returns></returns>
            [DllImport("Kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool UnmapViewOfFile(IntPtr lpBaseAddress);
        }

        #endregion
    }
}