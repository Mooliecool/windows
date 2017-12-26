/****************************** Module Header ******************************\
 Module Name:  WinINet.cs
 Project:      CSWebBrowserWithProxy
 Copyright (c) Microsoft Corporation.
 
 This class is used to set the proxy. or restore to the system proxy for the
 current application
 
 This source is subject to the Microsoft Public License.
 See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 All other rights reserved.
 
 THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace CSWebBrowserWithProxy
{
    public static class WinINet
    {
        static string agent = Process.GetCurrentProcess().ProcessName;

        /// <summary>
        /// Set the LAN connection proxy server for current process.
        /// </summary>
        /// <param name="proxyServer">
        /// The Proxy Server.
        /// </param>
        /// <returns></returns>
        public static bool SetConnectionProxy(bool isMachineSetting, string proxyServer)
        {
            if (isMachineSetting)
            {
                return SetConnectionProxy(null, proxyServer);
            }
            else
            {
                return SetConnectionProxy(agent, proxyServer);
            }
        }

        /// <summary>
        /// Set the LAN connection proxy server.
        /// </summary>
        /// <param name="agentName">
        /// If agentName is null or empty, this function will set the Lan proxy for
        /// the machine, else for the current process.
        /// </param>
        /// <param name="proxyServer">The Proxy Server.</param>
        /// <returns></returns>
        public static bool SetConnectionProxy(string agentName, string proxyServer)
        {
            IntPtr hInternet = IntPtr.Zero;
            try
            {
                if (!string.IsNullOrEmpty(agentName))
                {
                    hInternet = NativeMethods.InternetOpen(
                        agentName,
                        (int)INTERNET_OPEN_TYPE.INTERNET_OPEN_TYPE_DIRECT,
                        null,
                        null,
                        0);
                }

                return SetConnectionProxyInternal(hInternet, proxyServer);
            }
            finally
            {
                if (hInternet != IntPtr.Zero)
                {
                    NativeMethods.InternetCloseHandle(hInternet);
                }
            }
        }

        /// <summary>
        /// Set the proxy server for LAN connection.
        /// </summary>
        static bool SetConnectionProxyInternal(IntPtr hInternet, string proxyServer)
        {

            // Create 3 options.
            INTERNET_PER_CONN_OPTION[] Options = new INTERNET_PER_CONN_OPTION[3];

            // Set PROXY flags.
            Options[0] = new INTERNET_PER_CONN_OPTION();
            Options[0].dwOption = (int)INTERNET_PER_CONN_OptionEnum.INTERNET_PER_CONN_FLAGS;
            Options[0].Value.dwValue = (int)INTERNET_OPTION_PER_CONN_FLAGS.PROXY_TYPE_PROXY;

            // Set proxy name.
            Options[1] = new INTERNET_PER_CONN_OPTION();
            Options[1].dwOption =
                (int)INTERNET_PER_CONN_OptionEnum.INTERNET_PER_CONN_PROXY_SERVER;
            Options[1].Value.pszValue = Marshal.StringToHGlobalAnsi(proxyServer);

            // Set proxy bypass.
            Options[2] = new INTERNET_PER_CONN_OPTION();
            Options[2].dwOption =
                (int)INTERNET_PER_CONN_OptionEnum.INTERNET_PER_CONN_PROXY_BYPASS;
            Options[2].Value.pszValue = Marshal.StringToHGlobalAnsi("local");

            // Allocate a block of memory of the options.
            System.IntPtr buffer = Marshal.AllocCoTaskMem(Marshal.SizeOf(Options[0])
                + Marshal.SizeOf(Options[1]) + Marshal.SizeOf(Options[2]));

            System.IntPtr current = buffer;

            // Marshal data from a managed object to an unmanaged block of memory.
            for (int i = 0; i < Options.Length; i++)
            {
                Marshal.StructureToPtr(Options[i], current, false);
                current = (System.IntPtr)((int)current + Marshal.SizeOf(Options[i]));
            }

            // Initialize a INTERNET_PER_CONN_OPTION_LIST instance.
            INTERNET_PER_CONN_OPTION_LIST option_list = new INTERNET_PER_CONN_OPTION_LIST();

            // Point to the allocated memory.
            option_list.pOptions = buffer;

            // Return the unmanaged size of an object in bytes.
            option_list.Size = Marshal.SizeOf(option_list);

            // IntPtr.Zero means LAN connection.
            option_list.Connection = IntPtr.Zero;

            option_list.OptionCount = Options.Length;
            option_list.OptionError = 0;
            int size = Marshal.SizeOf(option_list);

            // Allocate memory for the INTERNET_PER_CONN_OPTION_LIST instance.
            IntPtr intptrStruct = Marshal.AllocCoTaskMem(size);

            // Marshal data from a managed object to an unmanaged block of memory.
            Marshal.StructureToPtr(option_list, intptrStruct, true);

            // Set internet settings.
            bool bReturn = NativeMethods.InternetSetOption(
                hInternet,
                INTERNET_OPTION.INTERNET_OPTION_PER_CONNECTION_OPTION,
                intptrStruct, size);

            // Free the allocated memory.
            Marshal.FreeCoTaskMem(buffer);
            Marshal.FreeCoTaskMem(intptrStruct);

            // Throw an exception if this operation failed.
            if (!bReturn)
            {
                throw new ApplicationException(" Set Internet Option Failed!");
            }

            // Notify the system that the registry settings have been changed and cause
            // the proxy data to be reread from the registry for a handle.
            NativeMethods.InternetSetOption(
                hInternet,
                INTERNET_OPTION.INTERNET_OPTION_SETTINGS_CHANGED,
                IntPtr.Zero, 0);

            NativeMethods.InternetSetOption(
                hInternet,
                INTERNET_OPTION.INTERNET_OPTION_REFRESH,
                IntPtr.Zero, 0);

            return bReturn;
        }

        /// <summary>
        /// Get the current system options for LAN connection.
        /// Make sure free the memory after restoration. 
        /// </summary>
        public static INTERNET_PER_CONN_OPTION_LIST GetSystemProxy()
        {

            // Query following options. 
            INTERNET_PER_CONN_OPTION[] Options = new INTERNET_PER_CONN_OPTION[3];

            Options[0] = new INTERNET_PER_CONN_OPTION();
            Options[0].dwOption = (int)INTERNET_PER_CONN_OptionEnum.INTERNET_PER_CONN_FLAGS;
            Options[1] = new INTERNET_PER_CONN_OPTION();
            Options[1].dwOption = (int)INTERNET_PER_CONN_OptionEnum.INTERNET_PER_CONN_PROXY_SERVER;
            Options[2] = new INTERNET_PER_CONN_OPTION();
            Options[2].dwOption = (int)INTERNET_PER_CONN_OptionEnum.INTERNET_PER_CONN_PROXY_BYPASS;

            // Allocate a block of memory of the options.
            System.IntPtr buffer = Marshal.AllocCoTaskMem(Marshal.SizeOf(Options[0])
                + Marshal.SizeOf(Options[1]) + Marshal.SizeOf(Options[2]));

            System.IntPtr current = (System.IntPtr)buffer;

            // Marshal data from a managed object to an unmanaged block of memory.
            for (int i = 0; i < Options.Length; i++)
            {
                Marshal.StructureToPtr(Options[i], current, false);
                current = (System.IntPtr)((int)current + Marshal.SizeOf(Options[i]));
            }

            // Initialize a INTERNET_PER_CONN_OPTION_LIST instance.
            INTERNET_PER_CONN_OPTION_LIST Request = new INTERNET_PER_CONN_OPTION_LIST();

            // Point to the allocated memory.
            Request.pOptions = buffer;

            Request.Size = Marshal.SizeOf(Request);

            // IntPtr.Zero means LAN connection.
            Request.Connection = IntPtr.Zero;

            Request.OptionCount = Options.Length;
            Request.OptionError = 0;
            int size = Marshal.SizeOf(Request);

            // Query system internet options. 
            bool result = NativeMethods.InternetQueryOption(
                IntPtr.Zero,
                INTERNET_OPTION.INTERNET_OPTION_PER_CONNECTION_OPTION,
                ref Request,
                ref size);

            if (!result)
            {
                throw new ApplicationException("Get System Internet Option Failed! ");
            }

            return Request;
        }

        /// <summary>
        /// Restore to the system proxy settings.
        /// </summary>
        public static bool RestoreSystemProxy()
        {
            return RestoreSystemProxy(agent);
        }

        /// <summary>
        /// Restore to the system proxy settings.
        /// </summary>
        public static bool RestoreSystemProxy(string agentName)
        {
            if (string.IsNullOrEmpty(agentName))
            {
                throw new ArgumentNullException("Agent name cannot be null or empty!");
            }

            IntPtr hInternet = IntPtr.Zero;
            try
            {
                if (!string.IsNullOrEmpty(agentName))
                {
                    hInternet = NativeMethods.InternetOpen(
                        agentName,
                        (int)INTERNET_OPEN_TYPE.INTERNET_OPEN_TYPE_DIRECT,
                        null,
                        null,
                        0);
                }

                return RestoreSystemProxyInternal(hInternet);
            }
            finally
            {
                if (hInternet != IntPtr.Zero)
                {
                    NativeMethods.InternetCloseHandle(hInternet);
                }
            }
        }

        /// <summary>
        /// Restore to the system proxy settings.
        /// </summary>
        static bool RestoreSystemProxyInternal(IntPtr hInternet)
        {
            var request = GetSystemProxy();

            int size = Marshal.SizeOf(request);

            // Allocate memory. 
            IntPtr intptrStruct = Marshal.AllocCoTaskMem(size);

            // Convert structure to IntPtr 
            Marshal.StructureToPtr(request, intptrStruct, true);

            // Set internet options.
            bool bReturn = NativeMethods.InternetSetOption(
                hInternet,
                INTERNET_OPTION.INTERNET_OPTION_PER_CONNECTION_OPTION,
                intptrStruct,
                size);

            // Free the allocated memory.
            Marshal.FreeCoTaskMem(request.pOptions);
            Marshal.FreeCoTaskMem(intptrStruct);

            if (!bReturn)
            {
                throw new ApplicationException(" Set Internet Option Failed! ");
            }

            // Notify the system that the registry settings have been changed and cause
            // the proxy data to be reread from the registry for a handle.
            NativeMethods.InternetSetOption(
                hInternet,
                INTERNET_OPTION.INTERNET_OPTION_SETTINGS_CHANGED,
                IntPtr.Zero,
                0);

            NativeMethods.InternetSetOption(
                hInternet,
                INTERNET_OPTION.INTERNET_OPTION_REFRESH,
                IntPtr.Zero,
                0);
            return bReturn;
        }
    }
}
