//-----------------------------------------------------------------------------
//
//  DwmApi
//
//  Managed wrapper of DWM API functions.
//
//-----------------------------------------------------------------------------

using System;
using System.Runtime.InteropServices;
using System.Security;
using System.Security.Permissions;

public class DwmApi
{
    //
    //  Composition
    //

    [DllImport("DwmApi.dll")]
    public static extern int DwmEnableComposition(
        bool fEnabled);

    [DllImport("DwmApi.dll")]
    public static extern int DwmIsCompositionEnabled(
        ref bool fEnabled);

    [DllImport("DwmApi.dll", EntryPoint = "#103")]
    public static extern int DwmpRestartComposition();



    //
    //  Thumbnails
    //

    [StructLayout(LayoutKind.Sequential)]
    public struct DWM_THUMBNAIL_PROPERTIES
    {
        public int dwFlags;
        public Win32.RECT rcDestination;
        public Win32.RECT rcSource;
        public byte opacity;
        public bool fVisible;
        public bool fSourceClientAreaOnly;
    };

    [DllImport("DwmApi.dll")]
    public static extern int DwmRegisterThumbnail(
        IntPtr hwndDestination,
        IntPtr hwndSource,
        ref Win32.SIZE minimizedSize,
        ref IntPtr hThumbnailId);

    [DllImport("DwmApi.dll")]
    public static extern int DwmUnregisterThumbnail(
        IntPtr hThumbnailId);

    [DllImport("DwmApi.dll")]
    public static extern int DwmUpdateThumbnailProperties(
        IntPtr hThumbnailId,
        ref DWM_THUMBNAIL_PROPERTIES tp);

    [DllImport("DwmApi.dll")]
    public static extern int DwmQueryThumbnailSourceSize(
        IntPtr hThumbnailId,
        ref Win32.SIZE size);

    //
    //  Window Attributes
    //
    public const int DWMNCRP_USEWINDOWSTYLE = 0;  // Enable/disable non-client rendering based on window style
    public const int DWMNCRP_DISABLED = 1;        // Disabled non-client rendering; window style is ignored
    public const int DWMNCRP_ENABLED = 2;         // Enabled non-client rendering; window style is ignored

    public const int DWMWA_NCRENDERING_ENABLED = 1;       // Enable/disable non-client rendering Use DWMNCRP_* values
    public const int DWMWA_NCRENDERING_POLICY = 2;        // Non-client rendering policy
    public const int DWMWA_TRANSITIONS_FORCEDISABLED = 3; // Potentially enable/forcibly disable transitions 0 or 1


    [DllImport("DwmApi.dll")]
    public static extern int DwmSetWindowAttribute(
        IntPtr hwnd,
        uint dwAttributeToSet, //DWMWA_* values
        IntPtr pvAttributeValue,
        uint cbAttribute);

    [DllImport("DwmApi.dll")]
    public static extern int DwmGetWindowAttribute(
        IntPtr hwnd,
        uint dwAttributeToGet, //DWMWA_* values
        IntPtr pvAttributeValue,
        uint cbAttribute);


    //
    //  Multi-Media
    //

    [StructLayout(LayoutKind.Sequential)]
    public struct UNSIGNED_RATIO
    {
        public UInt32 uiNumerator;
        public UInt32 uiDenominator;
    };

    [StructLayout(LayoutKind.Sequential)]
    public struct DWM_PRESENT_PARAMETERS
    {
        public int cbSize;
        public bool fQueue;
        public UInt64 cRefreshStart;
        public uint cBuffer;
        public bool fUseSourceRate;
        public UNSIGNED_RATIO uiNumerator;
    };


    [StructLayout(LayoutKind.Explicit)]
    public struct DWM_TIMING_INFO
    {
        [FieldOffset(0)]
        public UInt32 cbSize;
        [FieldOffset(4)]
        public UNSIGNED_RATIO rateRefresh;// Monitor refresh rate
        [FieldOffset(12)]
        public UNSIGNED_RATIO rateCompose;// composition rate     
        [FieldOffset(20)]
        public UInt64 qpcVBlank;          // QPC time at VBlank
        [FieldOffset(28)]
        public UInt64 cRefresh;           // DWM refresh counter
        [FieldOffset(36)]
        public UInt64 qpcCompose;         // QPC time at a compose time
        [FieldOffset(44)]
        public UInt64 cFrame;             // Frame number that was composed at qpcCompose
        [FieldOffset(52)]
        public UInt64 cRefreshFrame;      // Refresh count of the frame that was composed at qpcCompose
        [FieldOffset(60)]
        public UInt64 cRefreshConfirmed;  // The target refresh count of the last
                                          // frame confirmed completed by the GPU
        [FieldOffset(68)]
        public UInt32 cFlipsOutstanding;  // the number of outstanding flips

        //
        // Feedback on previous performance only valid on 2nd and subsequent calls
        //
        [FieldOffset(72)]
        public UInt64 cFrameCurrent;      // Last frame displayed
        [FieldOffset(80)]
        public UInt64 cFramesAvailable;   // number of frames available but not displayed, used or dropped
        [FieldOffset(88)]
        public UInt64 cFrameCleared;      // Source frame number when the following statistics where last cleared
        [FieldOffset(96)]
        public UInt64 cFramesReceived;    // number of new frames received
        [FieldOffset(104)]
        public UInt64 cFramesDisplayed;   // number of unique frames displayed
        [FieldOffset(112)]
        public UInt64 cFramesDropped;     // number of rendered frames that wer  never
        [FieldOffset(120)]                // displayed because composition occured too late
        public UInt64 cFramesMissed;      // number of times an old frame was composed 
        // when a new frame should have been used
        // but was not available
    };

    [DllImport("DwmApi.dll")]
    public static extern int DwmSetPresentParameters(
        IntPtr hwnd,
        ref DWM_PRESENT_PARAMETERS pPresentParams);

    [DllImport("DwmApi.dll")]
    public static extern int DwmSetDxFrameDuration(
        IntPtr hwnd,
        int refreshes);

    [DllImport("DwmApi.dll")]
    public static extern int DwmModifyPreviousDxFrameDuration(
        IntPtr hwnd,
        int refreshes,
        bool fRelative);

    [DllImport("DwmApi.dll")]
    public static extern int DwmGetCompositionTimingInfo(
        IntPtr hwnd,
        IntPtr timingInfo);

    [DllImport("DwmApi.dll")]
    public static extern int DwmEnableMMCSS(
        bool fEnableMMCSS);

    //
    //  Client Area Blur
    //

    public const int DWM_BB_ENABLE = 0x00000001;  // fEnable has been specified
    public const int DWM_BB_BLURREGION = 0x00000002;  // hRgnBlur has been specified
    public const int DWM_BB_TRANSITIONONMAXIMIZED = 0x00000004;  // fTransitionOnMaximized has been specified

    [StructLayout(LayoutKind.Sequential)]
    public struct DWM_BLURBEHIND
    {
        public int dwFlags;
        public bool fEnable;
        public IntPtr hRgnBlur;
        public bool fTransitionOnMaximized;
    };

    [StructLayout(LayoutKind.Sequential)]
    public struct MARGINS
    {
        public int cxLeftWidth;      // width of left border that retains its size
        public int cxRightWidth;     // width of right border that retains its size
        public int cyTopHeight;      // height of top border that retains its size
        public int cyBottomHeight;   // height of bottom border that retains its size
    };

    [DllImport("DwmApi.dll")]
    public static extern int DwmEnableBlurBehindWindow(
        IntPtr hwnd,
        ref DWM_BLURBEHIND bb);

    [DllImport("DwmApi.dll")]
    public static extern int DwmExtendFrameIntoClientArea(
        IntPtr hwnd,
        ref MARGINS m);



    //
    //  Colorization
    //

    [DllImport("DwmApi.dll")]
    public static extern int DwmGetColorizationColor(
        ref int color);

    [DllImport("DwmApi.dll", EntryPoint = "#104")]
    public static extern int DwmpSetColorizationColor(
        int color,
        bool isOpaqueBlend,
        bool isPreviewOnly);

}

