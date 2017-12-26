/******************************** Module Header ********************************\
Module Name:  Program.cs
Project:      CSSetPowerAvailabilityRequest
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
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;
using System.ComponentModel;
#endregion


namespace CSSetPowerAvailabilityRequest
{
    public class PowerRequest : IDisposable
    {
        private SafePowerRequestHandle powerRequestHandle;

        public PowerRequest(string simpleReason)
        {
            // Set up the diagnostic string.
            POWER_REQUEST_CONTEXT context;
            context.Version = POWER_REQUEST_CONTEXT_VERSION;
            context.Flags = POWER_REQUEST_CONTEXT_SIMPLE_STRING;
            context.SimpleReasonString = simpleReason;

            // Create the request and get a handle.
            powerRequestHandle = PowerCreateRequest(ref context);
            if (powerRequestHandle.IsInvalid)
            {
                throw new Win32Exception();
            }
        }


        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                if ((this.powerRequestHandle != null) &&
                !this.powerRequestHandle.IsClosed)
                {
                    this.powerRequestHandle.Dispose();
                }
            }
        }


        private bool displayRequired = false;

        public bool DisplayRequired
        {
            get { return displayRequired; }
            set
            {
                if (value && !displayRequired)
                {
                    // Set a display request to prevent display power management, 
                    // including automatic display dimming.
                    if (!PowerSetRequest(powerRequestHandle,
                        PowerRequestType.PowerRequestDisplayRequired))
                    {
                        throw new Win32Exception();
                    }
                }
                else if (!value && displayRequired)
                {
                    // Clear the display power request.
                    if (!PowerClearRequest(powerRequestHandle,
                        PowerRequestType.PowerRequestDisplayRequired))
                    {
                        throw new Win32Exception();
                    }
                }
            }
        }


        private bool systemRequired = false;

        public bool SystemRequired
        {
            get { return systemRequired; }
            set
            {
                if (value && !systemRequired)
                {
                    // Set a system request to prevent automatic sleep.
                    if (!PowerSetRequest(powerRequestHandle,
                        PowerRequestType.PowerRequestSystemRequired))
                    {
                        throw new Win32Exception();
                    }
                }
                else if (!value && systemRequired)
                {
                    // Clear the system power request.
                    if (!PowerClearRequest(powerRequestHandle,
                        PowerRequestType.PowerRequestSystemRequired))
                    {
                        throw new Win32Exception();
                    }
                }
            }
        }


        private bool awayModeRequired = false;

        public bool AwayModeRequired
        {
            get { return awayModeRequired; }
            set
            {
                if (value && !awayModeRequired)
                {
                    // Enable Away Mode.
                    if (!PowerSetRequest(powerRequestHandle,
                        PowerRequestType.PowerRequestAwayModeRequired))
                    {
                        throw new Win32Exception();
                    }
                }
                else if (!value && awayModeRequired)
                {
                    // Disable Away Mode.
                    if (!PowerClearRequest(powerRequestHandle,
                        PowerRequestType.PowerRequestAwayModeRequired))
                    {
                        throw new Win32Exception();
                    }
                }
            }
        }


        /// <summary>
        /// Creates a new power request object.
        /// </summary>
        /// <param name="Context">
        /// A REASON_CONTEXT structure that contains information about the power 
        /// request.
        /// </param>
        /// <returns></returns>
        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern SafePowerRequestHandle PowerCreateRequest(
            ref POWER_REQUEST_CONTEXT Context);


        /// <summary>
        /// Increments the count of power requests of the specified type for a 
        /// power request object.
        /// </summary>
        /// <param name="hPowerRequest">
        /// A handle to a power request object.
        /// </param>
        /// <param name="RequestType">
        /// The power request type to be incremented. 
        /// </param>
        /// <returns></returns>
        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern bool PowerSetRequest(
            SafePowerRequestHandle hPowerRequest, PowerRequestType RequestType);


        /// <summary>
        /// Decrements the count of power requests of the specified type for a 
        /// power request object. For each request that an application initiates 
        /// by using PowerSetRequest, the application must call PowerClearRequest 
        /// to clear the request when it is no longer required. 
        /// </summary>
        /// <param name="hPowerRequest">
        /// A handle to a power request object.
        /// </param>
        /// <param name="RequestType">
        /// The power request type to be decremented. 
        /// </param>
        /// <returns></returns>
        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern bool PowerClearRequest(
            SafePowerRequestHandle hPowerRequest, PowerRequestType RequestType);


        private const int POWER_REQUEST_CONTEXT_VERSION = 0;
        private const int POWER_REQUEST_CONTEXT_SIMPLE_STRING = 0x1;
        private const int POWER_REQUEST_CONTEXT_DETAILED_STRING = 0x2;
    }


    internal class SafePowerRequestHandle : SafeHandleZeroOrMinusOneIsInvalid
    {
        private SafePowerRequestHandle()
            : base(true)
        {
        }

        public SafePowerRequestHandle(IntPtr preexistingHandle, bool ownsHandle)
            : base(ownsHandle)
        {
            base.SetHandle(preexistingHandle);
        }

        [DllImport("kernel32.dll", SetLastError = true)]
        internal static extern bool CloseHandle(IntPtr handle);

        protected override bool ReleaseHandle()
        {
            return CloseHandle(base.handle);
        }
    }


    internal enum PowerRequestType
    {
        PowerRequestDisplayRequired = 0,
        PowerRequestSystemRequired,
        PowerRequestAwayModeRequired,
        PowerRequestMaximum
    }


    #region Availablity Request Structures

    // Windows defines the POWER_REQUEST_CONTEXT structure with an internal 
    // union of SimpleReasonString and Detailed information. To avoid runtime 
    // interop issues, this version of POWER_REQUEST_CONTEXT only supports 
    // SimpleReasonString. To use the detailed information, define the 
    // PowerCreateRequest function with the first parameter of type 
    // POWER_REQUEST_CONTEXT_DETAILED.

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    internal struct POWER_REQUEST_CONTEXT
    {
        public UInt32 Version;
        public UInt32 Flags;
        [MarshalAs(UnmanagedType.LPWStr)]
        public string SimpleReasonString;
    }


    [StructLayout(LayoutKind.Sequential)]
    internal struct PowerRequestContextDetailedInformation
    {
        public IntPtr LocalizedReasonModule;
        public UInt32 LocalizedReasonId;
        public UInt32 ReasonStringCount;
        [MarshalAs(UnmanagedType.LPWStr)]
        public string[] ReasonStrings;
    }


    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    internal struct POWER_REQUEST_CONTEXT_DETAILED
    {
        public UInt32 Version;
        public UInt32 Flags;
        public PowerRequestContextDetailedInformation DetailedInformation;
    }

    #endregion

}