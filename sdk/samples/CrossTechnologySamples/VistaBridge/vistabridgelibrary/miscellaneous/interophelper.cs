using System;
using System.Collections.Generic;
using System.Text;
using System.Security.Permissions;
using Microsoft.SDK.Samples.VistaBridge.Interop;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Helper class that exposes various Win32 functions as managed methods and properties.
    /// </summary>
    public static class InteropHelper
    {
        // Note: since these are public APIs that call immediately into native 
        // APIs, we need to make sure we require full-trust, 
        // as the unmanaged code check is only a 
        // link demand - and since this code is the caller and 
        // it has full-trust, the check would always succeed.
        /// <summary>
        /// Sends a window message.
        /// </summary>
        /// <param name="hWnd">The window handle.</param>
        /// <param name="msg">The window message.</param>
        /// <param name="wParam">The wParam argument.</param>
        /// <param name="lParam">The lParam argument.</param>
        /// <returns>The value returned by the win32 call.</returns>
        /// <permission cref="T:System.Security.Permissions.SecurityPermission">Demand value: <see cref="F:System.Security.Permissions.SecurityAction.Demand"/>; Named Permission Sets: <b>FullTrust</b>.</permission>
        [PermissionSetAttribute(SecurityAction.Demand, Name = "FullTrust")]
        public static int SendMessage(
            IntPtr hWnd, 
            uint msg, 
            int wParam, 
            bool lParam)
        {
            return (int) UnsafeNativeMethods.SendMessage(hWnd, msg, wParam, lParam);   
        }
        /// <summary>
        /// Sends a window message.
        /// </summary>
        /// <param name="hWnd">The window handle.</param>
        /// <param name="msg">The window message.</param>
        /// <param name="wParam">The wParam argument.</param>
        /// <param name="lParam">The lParam argument.</param>
        /// <returns>The value returned by the win32 call.</returns>
        /// <permission cref="T:System.Security.Permissions.SecurityPermission">Demand value: <see cref="F:System.Security.Permissions.SecurityAction.Demand"/>; Named Permission Sets: <b>FullTrust</b>.</permission>
        [PermissionSetAttribute(SecurityAction.Demand, Name = "FullTrust")]
        public static IntPtr SendMessage(
            IntPtr hWnd, 
            uint msg, 
            IntPtr wParam, 
            IntPtr lParam)
        {
            return UnsafeNativeMethods.SendMessage(hWnd, msg, wParam, lParam);
        }
        /// <summary>
        /// Sends a window message.
        /// </summary>
        /// <param name="hWnd">The window handle.</param>
        /// <param name="msg">The window message.</param>
        /// <param name="wParam">The wParam argument.</param>
        /// <param name="lParam">The lParam argument.</param>
        /// <returns>The value returned by the win32 call.</returns>
        /// <permission cref="T:System.Security.Permissions.SecurityPermission">Demand value: <see cref="F:System.Security.Permissions.SecurityAction.Demand"/>; Named Permission Sets: <b>FullTrust</b>.</permission>
        [PermissionSetAttribute(SecurityAction.Demand, Name = "FullTrust")]
        public static int SendMessage(
            IntPtr hWnd, 
            uint msg, 
            int wParam, 
            string lParam)
        {
            return (int)UnsafeNativeMethods.SendMessage(hWnd, msg, wParam, lParam);
        }
        /// <summary>
        /// Sends a window message.
        /// </summary>
        /// <param name="hWnd">The window handle.</param>
        /// <param name="msg">The window message.</param>
        /// <param name="wParam">The wParam argument.</param>
        /// <param name="lParam">The lParam argument.</param>
        /// <returns>The value returned by the win32 call.</returns>
        /// <permission cref="T:System.Security.Permissions.SecurityPermission">Demand value: <see cref="F:System.Security.Permissions.SecurityAction.Demand"/>; Named Permission Sets: <b>FullTrust</b>.</permission>
        [PermissionSetAttribute(SecurityAction.Demand, Name = "FullTrust")]
        public static IntPtr SendMessage(
            IntPtr hWnd, 
            uint msg, 
            ref int wParam, 
            StringBuilder lParam)
        {
            return UnsafeNativeMethods.SendMessage(hWnd, msg, ref wParam, lParam);
        }
        /// <summary>
        /// Controls whether the shield 
        /// icon is displayed for a button.
        /// </summary>
        /// <param name="button">The button to set.</param>
        /// <param name="showShield">A value that indicates whether the shield icon is displayed.</param>
        public static void SetWindowsFormsButtonShield(
          System.Windows.Forms.Button button,
          bool showShield)
        {
            InteropHelper.SendMessage(
                button.Handle, 
                SafeNativeMethods.BCM_SETSHIELD, 
                0, 
                showShield);
        }
    }
}
