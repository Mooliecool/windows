/******************************** Module Header ********************************\
Module Name:  Program.cs
Project:      CSPlatformDetector
Copyright (c) Microsoft Corporation.

The CSPlatformDetector code sample demonstrates the following tasks related 
to platform detection:

1. Detect the name of the current operating system. 
   (e.g. "Microsoft Windows 7 Enterprise")
2. Detect the version of the current operating system.
   (e.g. "Microsoft Windows NT 6.1.7600.0")
3. Determine whether the current operating system is a 64-bit operating 
   system. 
4. Determine whether the current process is a 64-bit process. 
5. Determine whether an arbitrary process running on the system is 64-bit. 

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*******************************************************************************/

using System;
using System.Runtime.InteropServices;
using System.Management;


namespace CSPlatformDetector
{
    class Program
    {
        static void Main(string[] args)
        {
            // Print the name of the current operating system.
            Console.WriteLine("Current OS: " + GetOSName());

            // Print the version string of the current operating system.
            Console.WriteLine("Version: " + Environment.OSVersion.VersionString);

            // Determine the whether the current OS is a 64-bit operating system. 
            Console.WriteLine("Current OS is {0}64-bit",
                Environment.Is64BitOperatingSystem ? "" : "not ");

            // Determine whether the current process is a 64-bit process. 
            Console.WriteLine("Current process is {0}64-bit",
                Environment.Is64BitProcess ? "" : "not ");

            // Determine whether an arbitrary process running on the system is 
            // a 64-bit process.
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
        }


        /// <summary>
        /// Gets the name of the currently running operating system. For example, 
        /// "Microsoft Windows 7 Enterprise".
        /// </summary>
        /// <returns>The name of the currently running OS</returns>
        static string GetOSName()
        {
            ManagementObjectSearcher searcher = new ManagementObjectSearcher(
                "root\\CIMV2", "SELECT Caption FROM Win32_OperatingSystem");

            foreach (ManagementObject queryObj in searcher.Get())
            {
                return queryObj["Caption"] as string;
            }

            return null;
        }


        /// <summary>
        /// Determines whether the specified process is a 64-bit process.
        /// </summary>
        /// <param name="hProcess">The process handle</param>
        /// <returns>
        /// true if the given process is 64-bit; otherwise, false.
        /// </returns>
        static bool Is64BitProcess(IntPtr hProcess)
        {
            bool flag = false;

            if (Environment.Is64BitOperatingSystem)
            {
                // On 64-bit OS, if a process is not running under Wow64 mode, 
                // the process must be a 64-bit process.
                flag = !(NativeMethods.IsWow64Process(hProcess, out flag) && flag);
            }

            return flag;
        }
    }
}