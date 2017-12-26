/******************************** Module Header ********************************\
Module Name:  HandleLeaks.cs
Project:      CSResourceLeaks
Copyright (c) Microsoft Corporation.



This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*******************************************************************************/

#region Using directives
using System;
using System.Threading;
using System.IO;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.ComponentModel;
#endregion


namespace CSResourceLeaks
{
    class HandleLeaks
    {
        #region File Handle Leak

        /// <summary>
        /// 
        /// </summary>
        public static void LeakFileHandle()
        {
            // Get and print the current process handle count.
            Console.WriteLine("Current process handle count: {0}",
                Process.GetCurrentProcess().HandleCount);

            Console.Write("Press ENTER to leak file handles ...");
            Console.ReadLine();

            // Get the temp path and the temp file name.
            string fileName = Path.GetTempFileName();
            Console.WriteLine("Temp file name: {0}", fileName);

            // Create the temp file and get the file handle.
            IntPtr hFile = CreateFile(fileName, 
                FileAccess.ReadWrite, 
                FileShare.None, 
                IntPtr.Zero, 
                FileMode.Create, 
                0x80, 
                IntPtr.Zero);
            if (hFile == new IntPtr(-1))
            {
                throw new Win32Exception();
            }

            // Read or write the file through the file handle.
            //ReadFile(hFile, ...
            //WriteFile(hFile, ...

            // Create another handle to the file object.
            // the handle has read-only access.
            Console.WriteLine("Duplicate the file handle");
            IntPtr hFileRO = IntPtr.Zero;
            if (!DuplicateHandle(
                GetCurrentProcess(), hFile, 
                GetCurrentProcess(), ref hFileRO, 
                FileAccess.Read, false, 0))
            {
                throw new Win32Exception();
            }

            // Read the file using hFileRO.
            //ReadFile(hFileRO, ...

            // Leak the file handles.
            //CloseHandle(hFile);
            //CloseHandle(hFileRO);

            // Get and print the current process handle count.
            Console.WriteLine("Current process handle count: {0}",
                Process.GetCurrentProcess().HandleCount);
        }

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        static extern IntPtr CreateFile(
            string fileName,
            [MarshalAs(UnmanagedType.U4)]FileAccess desiredAccess,
            [MarshalAs(UnmanagedType.U4)]FileShare fileShare,
            IntPtr securityAttributes,
            [MarshalAs(UnmanagedType.U4)]FileMode creationDisposition,
            uint flags,
            IntPtr template);

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool DuplicateHandle(
            IntPtr hSourceProcessHandle, IntPtr hSourceHandle,
            IntPtr hTargetProcessHandle, ref IntPtr lpTargetHandle,
            FileAccess desiredAccess, bool bInheritHandle, uint dwOptions);

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        static extern IntPtr GetCurrentProcess();
 
        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool CloseHandle(IntPtr hObject);

        #endregion


        #region GC Handle Leak

        /// <summary>
        /// 
        /// </summary>
        public static void LeakGCHandle()
        {
            Console.Write("Press ENTER to leak GC handles ...");
            Console.ReadLine();

            Console.WriteLine("Press CTRL+C to stop the execution");
            while (true)
            {
                MyObject obj = new MyObject();
                GCHandle h = GCHandle.Alloc(obj);

                Thread.Sleep(1); // Simulate a busy task.

                // Forget to free the GC handle.
                //h.Free();
            }
        }

        class MyObject
        {
        }

        #endregion
    }
}