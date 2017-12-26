/******************************** Module Header ********************************\
* Module Name:  WTSHelper.cs
* Project:      CSInteractiveWindowsService
* Copyright (c) Microsoft Corporation.
* 
* To be finished - Jialiang Ge
* Ref: http://files.codes-sources.com/fichier.aspx?id=47023&f=WTSLib/WTSLib/WTSHelper.cs
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*******************************************************************************/

using System;
using System.Text;
using System.Runtime.InteropServices;


namespace CSInteractiveWindowsService
{
    class WTSHelper
    {
        /// <summary>
        /// Retrieves the Remote Desktop Services session that is currently 
        /// attached to the physical console. The physical console is the monitor, 
        /// keyboard, and mouse. If there is no session attached to the physical 
        /// console, (for example, if the physical console session is in the 
        /// process of being attached or detached), this function returns 
        /// 0xFFFFFFFF.
        /// </summary>
        public static uint ActiveConsoleSessionId
        {
            get { return WTSGetActiveConsoleSessionId(); }
        }

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern uint WTSGetActiveConsoleSessionId();

        [DllImport("wtsapi32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool WTSEnumerateSessions(IntPtr hServer, 
            uint reserved, uint version, out IntPtr ppSessionInfo, out uint count);

        [DllImport("wtsapi32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern bool WTSQuerySessionInformation(IntPtr hServer,
            uint sessionId, WTS_INFO_CLASS wtsInfoClass, out IntPtr ppBuffer, 
            out uint bytesReturned);

        [DllImport("wtsapi32.dll", CharSet = CharSet.Auto, SetLastError = false)]
        private static extern void WTSFreeMemory(IntPtr memory);


        public static readonly IntPtr WTS_CURRENT_SERVER_HANDLE = IntPtr.Zero;
    }


    /// <summary>
    /// The WTS_SESSION_INFO structure contains information about a client 
    /// session on a Remote Desktop Session Host (RD Session Host) server.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    struct WTS_SESSION_INFO
    {
        public uint SessionID;

        [MarshalAs(UnmanagedType.LPTStr)]
        public string WinStationName;

        public WTS_CONNECTSTATE_CLASS State;
    }


    /// <summary>
    /// Specifies the connection state of a Remote Desktop Services session.
    /// </summary>
    enum WTS_CONNECTSTATE_CLASS
    {
        WTSActive,
        WTSConnected,
        WTSConnectQuery,
        WTSShadow,
        WTSDisconnected,
        WTSIdle,
        WTSListen,
        WTSReset,
        WTSDown,
        WTSInit
    }


    /// <summary>
    /// The WTS_INFO_CLASS enumeration type contains values that indicate the 
    /// type of session information to retrieve in a call to the 
    /// WTSQuerySessionInformation function.
    /// </summary>
    enum WTS_INFO_CLASS
    {
        WTSInitialProgram,
        WTSApplicationName,
        WTSWorkingDirectory,
        WTSOEMId,
        WTSSessionId,
        WTSUserName,
        WTSWinStationName,
        WTSDomainName,
        WTSConnectState,
        WTSClientBuildNumber,
        WTSClientName,
        WTSClientDirectory,
        WTSClientProductId,
        WTSClientHardwareId,
        WTSClientAddress,
        WTSClientDisplay,
        WTSClientProtocolType,
        WTSIdleTime,
        WTSLogonTime,
        WTSIncomingBytes,
        WTSOutgoingBytes,
        WTSIncomingFrames,
        WTSOutgoingFrames,
        WTSClientInfo,
        WTSSessionInfo
    }
}
