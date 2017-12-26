/****************************** Module Header ******************************\
 Module Name:  NativeMethods.cs
 Project:      CSCheckProcessType
 Copyright (c) Microsoft Corporation.
 
 This class imports the methods in the kernel32.dll and psapi.dll.
 
 This source is subject to the Microsoft Public License.
 See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 All other rights reserved.
 
 THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Runtime.InteropServices;

namespace CSCheckProcessType
{
    internal static class NativeMethods
    {

        /// <summary>
        /// The standard output device. Initially, this is the active console 
        /// screen buffer, CONOUT$. 
        /// </summary>
        internal const int STD_OUTPUT_HANDLE = -11;

        /// <summary>
        /// Retrieve the current input mode of a console's input buffer or 
        /// the current output mode of a console screen buffer.
        /// </summary>
        /// <param name="hConsoleHandle">
        /// A handle to the console input buffer or the console screen buffer.
        /// </param>
        /// <param name="lpMode">
        /// A pointer to a variable that receives the current mode of the specified buffer. 
        /// </param>
        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        internal static extern bool GetConsoleMode(IntPtr hConsoleHandle, out uint lpMode);

        /// <summary>
        /// Retrieve a handle to the specified standard device (standard input, 
        /// standard output, or standard error).
        /// </summary>
        /// <param name="nStdHandle">
        /// The standard device. This parameter can be one of the following values.
        /// STD_INPUT_HANDLE  -10
        /// STD_OUTPUT_HANDLE -11
        /// STD_ERROR_HANDLE  -12
        /// </param>
        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        internal static extern IntPtr GetStdHandle(int nStdHandle);

        /// <summary>
        /// Attach the calling process to the console of the specified process.
        /// </summary>
        /// <param name="dwProcessId">
        /// The identifier of the process whose console is to be used.
        /// </param>
        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        internal static extern bool AttachConsole(uint dwProcessId);

        /// <summary>
        /// Detach the calling process from its console.
        /// </summary>
        /// <returns></returns>
        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        internal static extern bool FreeConsole();

        /// <summary>
        /// Determines whether the specified process is running under WOW64
        /// </summary>
        /// <param name="hProcess">
        /// A handle to the process. The handle must have the PROCESS_QUERY_INFORMATION
        /// or PROCESS_QUERY_LIMITED_INFORMATION access right.
        /// The Handle property of System.Diagnostics.Process class will open the 
        /// process with dwDesiredAccess = 0x1F0FFF, which means that 
        /// PROCESS_QUERY_INFORMATION is required.
        /// </param>
        /// <param name="wow64Process">
        /// TRUE if the process is running under WOW64.
        /// </param>
        /// <returns>
        /// If the function succeeds, the return value is nonzero.
        /// If the function fails, the return value is zero. To get extended error
        /// information, call GetLastError.
        /// </returns>
        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool IsWow64Process(IntPtr hProcess, out bool wow64Process);

        /// <summary>
        /// Retrieves a handle for each module in the specified process that meets 
        /// the specified filter criteria.
        /// </summary>
        /// <param name="hProcess">
        /// A handle to the process.
        /// </param>
        /// <param name="lphModule">
        /// An array that receives the list of module handles.
        /// </param>
        /// <param name="cb">
        /// The size of the lphModule array, in bytes.
        /// </param>
        /// <param name="lpcbNeeded ">
        /// The number of bytes required to store all module handles in the 
        /// lphModule array.
        /// </param>
        /// <param name="dwFilterFlag">
        /// The filter criteria. This parameter can be one of the following values.
        /// </param>
        /// <returns>
        /// If the function succeeds, the return value is nonzero.
        /// If the function fails, the return value is zero. To get extended error
        /// information, call GetLastError.
        /// </returns>
        [DllImport("psapi.dll", CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool EnumProcessModulesEx(
            [In] IntPtr hProcess,
            [Out] IntPtr[] lphModule,
            [In] int cb,
            [Out] out int lpcbNeeded,
            [In] ModuleFilterFlags dwFilterFlag);

        /// <summary>
        /// Retrieves the fully-qualified path for the file containing the specified module.
        /// </summary>
        /// <param name="hProcess">
        /// A handle to the process that contains the module. 
        /// </param>
        /// <param name="hModule">
        /// A handle to the module. If this parameter is NULL, GetModuleFileNameEx returns 
        /// the path of the executable file of the process specified in hProcess.
        /// </param>
        /// <param name="lpFilename">
        /// A pointer to a buffer that receives the fully-qualified path to the module.
        /// </param>
        /// <param name="nSize">
        /// The size of the lpFilename buffer, in characters.
        /// </param>
        /// <returns>
        /// If the function succeeds, the return value specifies the length of the string
        /// copied to the buffer. 
        /// If the function fails, the return value is zero. To get extended error 
        /// information, call GetLastError.
        /// </returns>
        [DllImport("psapi.dll", CharSet = CharSet.Auto, SetLastError = true)]
        internal static extern uint GetModuleFileNameEx(
            [In] IntPtr hProcess,
            [In] IntPtr hModule,
            [Out] [MarshalAs(UnmanagedType.LPTStr)] System.Text.StringBuilder lpFilename,
            uint nSize);

        [Flags]
        internal enum ModuleFilterFlags
        {
            // List the 32-bit modules.
            LIST_MODULES_32BIT = 0x01,

            // List the 64-bit modules.
            LIST_MODULES_64BIT = 0x02,

            // List all modules.
            LIST_MODULES_ALL = 0x03,

            // Use the default behavior.
            LIST_MODULES_DEFAULT = 0x0
        }
    }
}
