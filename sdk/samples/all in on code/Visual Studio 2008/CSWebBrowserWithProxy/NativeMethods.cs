/****************************** Module Header ******************************\
 Module Name:  NativeMethods.cs
 Project:      CSWebBrowserWithProxy
 Copyright (c) Microsoft Corporation.
 
 This class is a simple .NET wrapper of wininet.dll. It contains 4 extern
 methods in wininet.dll. They are InternetOpen, InternetCloseHandle, 
 InternetSetOption and InternetQueryOption.
 
 This source is subject to the Microsoft Public License.
 See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 All other rights reserved.
 
 THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Runtime.InteropServices;

namespace CSWebBrowserWithProxy
{
    internal static class NativeMethods
    {
        /// <summary>
        /// Initialize an application's use of the WinINet functions.
        /// See 
        /// </summary>
        [DllImport("wininet.dll", SetLastError = true, CharSet = CharSet.Auto)]
        internal static extern IntPtr InternetOpen(
            string lpszAgent,
            int dwAccessType,
            string lpszProxyName,
            string lpszProxyBypass,
            int dwFlags);

        /// <summary>
        /// Close a single Internet handle.
        /// </summary>
        [DllImport("wininet.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool InternetCloseHandle(IntPtr hInternet);

        /// <summary>
        /// Sets an Internet option.
        /// </summary>
        [DllImport("wininet.dll", CharSet = CharSet.Ansi, SetLastError = true)]
        internal static extern bool InternetSetOption(
            IntPtr hInternet,
            INTERNET_OPTION dwOption,
            IntPtr lpBuffer,
            int lpdwBufferLength);

        /// <summary>
        /// Queries an Internet option on the specified handle. The Handle will be always 0.
        /// </summary>
        [DllImport("wininet.dll", CharSet = CharSet.Ansi, SetLastError = true)]
        internal extern static bool InternetQueryOption(
            IntPtr hInternet,
            INTERNET_OPTION dwOption,
            ref INTERNET_PER_CONN_OPTION_LIST OptionList,
            ref int lpdwBufferLength);
    }
}
