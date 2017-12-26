using System;
using System.Collections;
using System.ComponentModel;
using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using Microsoft.SDK.Samples.VistaBridge.Interop;

namespace Microsoft.SDK.Samples.VistaBridge.Library.PowerManagement
{
    internal static class Power
    {
        internal static SafeNativeMethods.SystemPowerCapabilities 
            GetSystemPowerCapabilities()
        {
            IntPtr status = Marshal.AllocCoTaskMem(
                Marshal.SizeOf(typeof(SafeNativeMethods.SystemPowerCapabilities)));

            uint retval = NativeMethods.CallNtPowerInformation(
              4,  // SystemPowerCapabilities
              (IntPtr)null,
              0,
              status,
              (UInt32)Marshal.SizeOf(typeof(SafeNativeMethods.SystemPowerCapabilities))
              );

            if (retval == SafeNativeMethods.STATUS_ACCESS_DENIED)
            {
                throw new UnauthorizedAccessException("The caller had insufficient access rights to get the system power capabilities.");
            }

            SafeNativeMethods.SystemPowerCapabilities powerCap = (SafeNativeMethods.SystemPowerCapabilities)Marshal.PtrToStructure(status, typeof(SafeNativeMethods.SystemPowerCapabilities));
            Marshal.FreeCoTaskMem(status);

            return powerCap;
        }

        internal static SafeNativeMethods.SystemBatteryState GetSystemBatteryState()
        {
            IntPtr status = Marshal.AllocCoTaskMem(Marshal.SizeOf(typeof(SafeNativeMethods.SystemBatteryState)));
            uint retval = NativeMethods.CallNtPowerInformation(
              5,  // SystemBatteryState
              (IntPtr)null,
              0,
              status,
              (UInt32)Marshal.SizeOf(typeof(SafeNativeMethods.SystemBatteryState))
              );

            if (retval == SafeNativeMethods.STATUS_ACCESS_DENIED) 
            {
                throw new UnauthorizedAccessException("The caller had insufficient access rights to get the system battery state.");
            }

            SafeNativeMethods.SystemBatteryState batt_status = (SafeNativeMethods.SystemBatteryState)Marshal.PtrToStructure(status, typeof(SafeNativeMethods.SystemBatteryState));
            Marshal.FreeCoTaskMem(status);

            return batt_status;
        }

        /// <summary>
        /// Registers the application to receive power setting notifications 
        /// for the specific power setting event.
        /// </summary>
        /// <param name="handle">Handle indicating where the power setting 
        /// notifications are to be sent.</param>
        /// <param name="powerSetting">The GUID of the power setting for 
        /// which notifications are to be sent.</param>
        /// <returns>Returns a notification handle for unregistering 
        /// power notifications.</returns>
        internal static int RegisterPowerSettingNotification(
            long handle, Guid powerSetting)
        {
            int outHandle = NativeMethods.RegisterPowerSettingNotification(
                new IntPtr(handle), 
                ref powerSetting, 
                0);

            return outHandle;
        }

        /// <summary>
        /// Allows an application to inform the system that it 
        /// is in use, thereby preventing the system from entering 
        /// the sleeping power state or turning off the display 
        /// while the application is running.
        /// </summary>
        /// <param name="flags">The thread's execution requirements.</param>
        /// <exception cref="Win32Exception">Thrown if the SetThreadExecutionState call fails.</exception>
        internal static void SetThreadExecutionState(ExecutionState flags)
        {
            ExecutionState? ret = NativeMethods.SetThreadExecutionState(flags);
            if (ret == null)
                throw new Win32Exception("SetThreadExecutionState call failed.");
        }

        internal static void SetScreenSaverActive(bool active)
        {
            NativeMethods.SystemParametersInfoSet(
                SafeNativeMethods.SPI_SETSCREENSAVEACTIVE,
                (UInt32) (active ? 1 : 0), 
                IntPtr.Zero, 
                SafeNativeMethods.SPIF_SENDCHANGE | 
                    SafeNativeMethods.SPIF_UPDATEINIFILE);
        }
    }
}