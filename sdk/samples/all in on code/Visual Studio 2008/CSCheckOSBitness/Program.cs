/****************************** Module Header ******************************\
Module Name:  Program.cs
Project:      CSCheckOSBitness
Copyright (c) Microsoft Corporation.

The code sample demonstrates how to determine whether the operating system 
of the current machine or any remote machine is a 64-bit operating system.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Runtime.InteropServices;
using System.Security;
using System.Management;


namespace CSCheckOSBitness
{
    class Program
    {
        #region Is64BitOS (IsWow64Process)

        /// <summary>
        /// The function determines whether the current operating system is a 
        /// 64-bit operating system.
        /// </summary>
        /// <returns>
        /// The function returns true if the operating system is 64-bit; 
        /// otherwise, it returns false.
        /// </returns>
        public static bool Is64BitOS()
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
                return ((DoesWin32MethodExist("kernel32.dll", "IsWow64Process") &&
                    IsWow64Process(GetCurrentProcess(), out flag)) && flag);
            }
        }

        /// <summary>
        /// The function determins whether a method exists in the export 
        /// table of a certain module.
        /// </summary>
        /// <param name="moduleName">The name of the module</param>
        /// <param name="methodName">The name of the method</param>
        /// <returns>
        /// The function returns true if the method specified by methodName 
        /// exists in the export table of the module specified by moduleName.
        /// </returns>
        static bool DoesWin32MethodExist(string moduleName, string methodName)
        {
            IntPtr moduleHandle = GetModuleHandle(moduleName);
            if (moduleHandle == IntPtr.Zero)
            {
                return false;
            }
            return (GetProcAddress(moduleHandle, methodName) != IntPtr.Zero);
        }

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        static extern IntPtr GetCurrentProcess();

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        static extern IntPtr GetModuleHandle(string moduleName);

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        static extern IntPtr GetProcAddress(IntPtr hModule,
            [MarshalAs(UnmanagedType.LPStr)]string procName);

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool IsWow64Process(IntPtr hProcess, out bool wow64Process);

        #endregion


        #region Is64BitOS (WMI)

        /// <summary>
        /// The function determines whether the operating system of the 
        /// current machine of any remote machine is a 64-bit operating 
        /// system through Windows Management Instrumentation (WMI).
        /// </summary>
        /// <param name="machineName">
        /// The full computer name or IP address of the target machine. "." 
        /// or null means the local machine. 
        /// </param>
        /// <param name="userName">
        /// The user name you need for a connection. A null value indicates 
        /// the current security context. If the user name is from a domain 
        /// other than the current domain, the string should contain the 
        /// domain name and user name, separated by a backslash: string 
        /// 'username' = "DomainName\\UserName". 
        /// </param>
        /// <param name="password">
        /// The password for the specified user.
        /// </param>
        /// <returns>
        /// The function returns true if the operating system is 64-bit; 
        /// otherwise, it returns false.
        /// </returns>
        /// <exception cref="System.Management.ManagementException">
        /// The ManagementException exception is generally thrown with the  
        /// error message "User credentials cannot be used for local 
        /// connections". To solve it, do not specify userName and password
        /// when machineName refers to the local computer.
        /// </exception>
        /// <exception cref="System.UnauthorizedAccessException">
        /// This exception is usually caused by incorrect user name or 
        /// password.
        /// </exception>
        /// <exception cref="System.Runtime.InteropServices.COMException">
        /// A common error accompanied with the COMException is "The RPC 
        /// server is unavailable. (Exception from HRESULT: 0x800706BA)". 
        /// This is usually caused by the firewall on the target machine that 
        /// blocks the WMI connection or some network problem.
        /// </exception>
        public static bool Is64BitOS(string machineName, string userName, 
            string password)
        {
            ConnectionOptions options = null;

            // Build a ConnectionOptions object for the remote connection 
            // if you plan to connect to the remote with a different user 
            // name and password than the one you are currently using.
            if (!string.IsNullOrEmpty(userName))
            {
                options = new ConnectionOptions();
                options.Username = userName;
                options.Password = password;
            }
            // Else the connection will use the current user token.

            // Make a connection to the target computer.
            if (string.IsNullOrEmpty(machineName))
            {
                machineName = ".";
            }
            string path = @"\\" + machineName + @"\root\cimv2";
            ManagementScope scope = new ManagementScope(path, options);
            scope.Connect();

            // Query Win32_Processor.AddressWidth which dicates the current 
            // operating mode of the processor (on a 32-bit OS, it would be 
            // "32"; on a 64-bit OS, it would be "64").
            // Note: Win32_Processor.DataWidth indicates the capability of 
            // the processor. On a 64-bit processor, it is "64".
            // Note: Win32_OperatingSystem.OSArchitecture tells the bitness
            // of OS too. On a 32-bit OS, it would be "32-bit". However, it 
            // is only available on Windows Vista and newer OS.
            ObjectQuery query = new ObjectQuery(
                "SELECT AddressWidth FROM Win32_Processor");

            // Perform the query and get the result.
            ManagementObjectSearcher searcher = new ManagementObjectSearcher(scope, query);
            ManagementObjectCollection queryCollection = searcher.Get();
            foreach (ManagementObject queryObj in queryCollection)
            {
                if (queryObj["AddressWidth"].ToString() == "64")
                {
                    return true;
                }
            }

            return false;
        }

        #endregion


        static void Main(string[] args)
        {
            // Solution 1. Is64BitOS (IsWow64Process)
            // Determine whether the current operating system is a 64 bit 
            // operating system.
            bool is64bitOS = Is64BitOS();
            Console.WriteLine("Current OS is {0}64-bit", is64bitOS ? "" : "not ");

            // Solution 2. Is64BitOS (WMI)
            // Determine whether the current operating system is a 64 bit 
            // operating system through WMI. Note: The first solution of 
            // using IsWow64Process is the preferred way to detect OS bitness 
            // of the current system because it is much easier and faster. 
            // The WMI solution is useful when you want to find this 
            // information on a remote system. 
            try
            {
                // If you want to get the OS bitness information of a remote 
                // system, configure the system for remote connections of WMI 
                // (http://msdn.microsoft.com/en-us/library/aa389290.aspx), 
                // and replace the parameters (".", null, null) with the 
                // remote computer name and credentials for the connection.
                is64bitOS = Is64BitOS(".", null, null);
                Console.WriteLine("Current OS is {0}64-bit", is64bitOS ? "" : "not ");
            }
            catch (Exception ex)
            {
                Console.WriteLine("Is64BitOS throws the exception: {0}", ex.Message);
            }
        }
    }
}