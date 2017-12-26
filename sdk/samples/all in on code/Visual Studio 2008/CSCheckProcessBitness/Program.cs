/******************************** Module Header ********************************\
Module Name:  Program.cs
Project:      CSCheckProcessBitness
Copyright (c) Microsoft Corporation.

The code sample demonstrates how to determine whether the given process is a 
64-bit process or not.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*******************************************************************************/

using System;
using System.Runtime.InteropServices;


namespace CSCheckProcessBitness
{
    class Program
    {
        public static void Main(string[] args)
        {
            if (args.Length > 0)
            {
                // If a process ID is specified in the command line, get the 
                // process id, and open the process handle.
                int processId = 0;
                if (int.TryParse(args[0], out processId))
                {
                    IntPtr hProcess = NativeMethods.OpenProcess(
                        NativeMethods.PROCESS_QUERY_INFORMATION, false, processId);
                    if (hProcess != IntPtr.Zero)
                    {
                        try
                        {
                            // Detect whether the specified process is a 64-bit.
                            bool is64bitProc = Is64BitProcess(hProcess);
                            Console.WriteLine("Process {0} is {1}64-bit",
                                processId.ToString(), is64bitProc ? "" : "not ");
                        }
                        finally
                        {
                            NativeMethods.CloseHandle(hProcess);
                        }
                    }
                    else
                    {
                        int errorCode = Marshal.GetLastWin32Error();
                        Console.WriteLine("OpenProcess({0}) failed w/err 0x{1:X}",
                            processId.ToString(), errorCode.ToString());
                    }
                }
                else
                {
                    Console.WriteLine("Invalide process ID: {0}", processId.ToString());
                }
            }
            else
            {
                // If no process id was specified, use the id of the current 
                // process, and determine whether the current process is a 64-bit 
                // process.
                Console.WriteLine("Current process is {0}64-bit",
                    Is64BitProcess() ? "" : "not ");
            }
        }


        /// <summary>
        /// Determines whether the current process is a 64-bit process.
        /// </summary>
        /// <returns>
        /// true if the process is 64-bit; otherwise, false.
        /// </returns>
        static bool Is64BitProcess()
        {
            return (IntPtr.Size == 8);
        }


        /// <summary>
        /// Determines whether the specified process is a 64-bit process.
        /// </summary>
        /// <param name="hProcess">The process handle</param>
        /// <returns>
        /// true if the process is 64-bit; otherwise, false.
        /// </returns>
        static bool Is64BitProcess(IntPtr hProcess)
        {
            bool flag = false;

            if (Is64BitOS())
            {
                // On 64-bit OS, if a process is not running under Wow64 mode, 
                // the process must be a 64-bit process.
                flag = !(NativeMethods.IsWow64Process(hProcess, out flag) && flag);
            }

            return flag;
        }


        /// <summary>
        /// Determines whether the current operating system is a 64-bit operating 
        /// system.
        /// </summary>
        /// <returns>
        /// true if the operating system is 64-bit; otherwise, false.
        /// </returns>
        static bool Is64BitOS()
        {
            if (IntPtr.Size == 8)  // 64-bit programs run only on Win64
            {
                return true;
            }
            else  // 32-bit programs run on both 32-bit and 64-bit Windows
            {
                // Detect whether the current process is a 32-bit process 
                // running on a 64-bit system.
                bool flag;
                return (DoesWin32MethodExist("kernel32.dll", "IsWow64Process")
                    && (NativeMethods.IsWow64Process(NativeMethods.GetCurrentProcess(), out flag)
                    && flag));
            }
        }


        /// <summary>
        /// Determines whether a method exists in the export table of a certain 
        /// module.
        /// </summary>
        /// <param name="moduleName">The name of the module</param>
        /// <param name="methodName">The name of the method</param>
        /// <returns>
        /// The function returns true if the method specified by methodName 
        /// exists in the export table of the module specified by moduleName.
        /// </returns>
        static bool DoesWin32MethodExist(string moduleName, string methodName)
        {
            IntPtr moduleHandle = NativeMethods.GetModuleHandle(moduleName);
            if (moduleHandle == IntPtr.Zero)
            {
                return false;
            }
            return (NativeMethods.GetProcAddress(moduleHandle, methodName) != IntPtr.Zero);
        }
    }
}