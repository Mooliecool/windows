using System;
using System.Text;
using System.Runtime.InteropServices;
using System.Security;
using Microsoft.SDK.Samples.VistaBridge.Library;
using Microsoft.SDK.Samples.VistaBridge.Library.StockIcons;
namespace Microsoft.SDK.Samples.VistaBridge.Interop
{
    /// <summary>
    /// Internal class that contains interop declarations for 
    /// performance critical native functions. 
    /// Note that NO security checking is done by the CLR, 
    /// because the unmanaged code security checks are disabled; 
    /// ANY usage of these methods must be properly 
    /// protected with the appropriate security demands.
    /// </summary>
    /// 
    [SuppressUnmanagedCodeSecurity]
    internal static class UnsafeNativeMethods
    {
        // Various overloads of SendMessage.
        [DllImport(ExternDll.User32, 
            CharSet = CharSet.Auto, 
            SetLastError = true)]
        internal static extern IntPtr SendMessage(
            IntPtr hWnd,
            uint msg,
            IntPtr wParam,
            IntPtr lParam
        );

        [DllImport(ExternDll.User32, 
            CharSet = CharSet.Auto, 
            SetLastError = true)]
        internal static extern IntPtr SendMessage(
            IntPtr hWnd,
            uint msg,
            int wParam,
            [MarshalAs(UnmanagedType.Bool)] bool lParam
        );

        [DllImport(ExternDll.User32, 
            CharSet = CharSet.Auto, 
            SetLastError = true)]
        internal static extern IntPtr SendMessage(
           IntPtr hWnd, 
            uint Msg,
           int wParam,
           [MarshalAs(UnmanagedType.LPWStr)] string lParam);

        [DllImport(ExternDll.User32, 
            CharSet = CharSet.Auto, 
            SetLastError = true)]
        internal static extern IntPtr SendMessage(
          IntPtr hWnd, 
            uint Msg,
          ref int wParam, 
            [MarshalAs(UnmanagedType.LPWStr)] StringBuilder lParam);

        [DllImport("ole32.dll", 
            CharSet = CharSet.Unicode, 
            ExactSpelling = true, 
            PreserveSig = false)]
        [return: MarshalAs(UnmanagedType.Interface)]
        internal static extern object CoGetObject(
           string pszName,
           [In] ref SafeNativeMethods.BIND_OPTS3 pBindOptions,
           [In, MarshalAs(UnmanagedType.LPStruct)] Guid riid);

        #region GDI and DWM Declarations

        [DllImport("user32.dll")]
        internal static extern IntPtr GetDC(IntPtr hWnd);

        [DllImport("user32.dll")]
        internal static extern int ReleaseDC(IntPtr hWnd, IntPtr hdc);

        [DllImport("user32.dll")]
        internal static extern int DrawCaption(
            // Handle to window.
            IntPtr hwnd,
            // Handle to device context.
            IntPtr hdc,
            // Rectangle to draw in.
            ref SafeNativeMethods.RECT lprc,
            // drawing options.
            int uFlags   
        );

        [DllImport("user32.dll")]
        internal static extern int GetClientRect(
            IntPtr hwnd,
            ref SafeNativeMethods.RECT rect);

        [DllImport("gdi32.dll")]
        internal static extern IntPtr CreateRectRgn(
            int left,
            int top,
            int right,
            int bottom);

        [DllImport("gdi32.dll")]
        internal static extern IntPtr CreateRoundRectRgn(
            int left,
            int top,
            int right,
            int bottom,
            // Width of the ellipse used to create the rounded corners.
            int widthOfEllipse,   
            int heightOfEllipse);

        [DllImport("user32.dll")]
        internal static extern int SetWindowRgn(
            IntPtr hwnd,
            IntPtr hrgn,
            [MarshalAs(UnmanagedType.Bool)] bool redrawWindow);

        [DllImport("DwmApi.dll")]
        internal static extern int DwmEnableComposition(
           [MarshalAs(UnmanagedType.Bool)] bool fEnabled);

        [DllImport("DwmApi.dll")]
        internal static extern int DwmIsCompositionEnabled(
            [MarshalAs(UnmanagedType.Bool)] ref bool fEnabled);

        [DllImport("DwmApi.dll")]
        internal static extern int DwmRegisterThumbnail(
            IntPtr hwndDestination,
            IntPtr hwndSource,
            ref SafeNativeMethods.SIZE minimizedSize,
            ref IntPtr hThumbnailId);

        [DllImport("DwmApi.dll")]
        internal static extern int DwmUnregisterThumbnail(
            IntPtr hThumbnailId);

        [DllImport("DwmApi.dll")]
        internal static extern int DwmUpdateThumbnailProperties(
            IntPtr hThumbnailId,
            ref SafeNativeMethods.DWM_THUMBNAIL_PROPERTIES tp);

        [DllImport("DwmApi.dll")]
        internal static extern int DwmQueryThumbnailSourceSize(
            IntPtr hThumbnailId,
            ref SafeNativeMethods.SIZE size);

        [DllImport("DwmApi.dll")]
        internal static extern int DwmSetWindowAttribute(
            IntPtr hwnd,
            //DWMWA_* values.
            uint dwAttributeToSet, 
            IntPtr pvAttributeValue,
            uint cbAttribute);

        [DllImport("DwmApi.dll")]
        internal static extern int DwmGetWindowAttribute(
            IntPtr hwnd,
            //DWMWA_* values.
            uint dwAttributeToGet, 
            IntPtr pvAttributeValue,
            uint cbAttribute);

        [DllImport("DwmApi.dll")]
        public static extern int DwmSetPresentParameters(
            IntPtr hwnd,
            ref SafeNativeMethods.DWM_PRESENT_PARAMETERS pPresentParams);

        [DllImport("DwmApi.dll")]
        internal static extern int DwmSetDxFrameDuration(
            IntPtr hwnd,
            int refreshes);

        [DllImport("DwmApi.dll")]
        internal static extern int DwmModifyPreviousDxFrameDuration(
            IntPtr hwnd,
            int refreshes,
            [MarshalAs(UnmanagedType.Bool)] bool fRelative);

        [DllImport("DwmApi.dll")]
        internal static extern int DwmGetCompositionTimingInfo(
            IntPtr hwnd,
            IntPtr timingInfo);

        [DllImport("DwmApi.dll")]
        internal static extern int DwmEnableMMCSS(
            [MarshalAs(UnmanagedType.Bool)] bool fEnableMMCSS);


        [DllImport("DwmApi.dll")]
        internal static extern int DwmEnableBlurBehindWindow(
            IntPtr hwnd,
            ref SafeNativeMethods.DWM_BLURBEHIND bb);

        [DllImport("DwmApi.dll")]
        internal static extern int DwmExtendFrameIntoClientArea(
            IntPtr hwnd,
            ref SafeNativeMethods.MARGINS m);

        [DllImport("DwmApi.dll")]
        internal static extern int DwmGetColorizationColor(
            ref int color);

        #endregion

        #region StockIcon interop

        [DllImport("Shell32.dll", CharSet = CharSet.Unicode,
        ExactSpelling = true, SetLastError = false)]
        internal static extern int SHGetStockIconInfo(
            StockIconIdentifier identifier,
            StockIconOptions flags,
            ref SafeNativeMethods.StockIconInfo info);

        [DllImport("User32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool DestroyIcon(IntPtr handle);

        #endregion
    }
}
