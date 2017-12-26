using System;
using System.Runtime.InteropServices;
using System.Security;
using System.Security.Permissions;

namespace MS.Win32.Penimc
{
    /// <SecurityNote>
    ///     Critical: This elevates to unmanaged code permission.
    /// </SecurityNote>
    [SecurityCritical(SecurityCriticalScope.Everything), SuppressUnmanagedCodeSecurity]
    [
    ComImport,
    Guid("9bc79c93-2289-4bb5-abf4-3287fd9cae39"),
    InterfaceType(ComInterfaceType.InterfaceIsIUnknown)
    ]
    interface IPimcContext
    {
        void ShutdownComm();
        void GetPacketDescriptionInfo(out int cProps, out int cButtons);
        void GetPacketPropertyInfo(int iProp, out Guid guid, out int iMin, out int iMax, out int iUnits, out float flResolution);
        void GetPacketButtonInfo(int iButton, out Guid guid);
        void GetLastSystemEventData(out int evt, out int modifier, out int character, out int x, out int y, out int stylusMode, out int buttonState);
    }

    /// <SecurityNote>
    ///     Critical: This elevates to unmanaged code permission.
    /// </SecurityNote>
    [SecurityCritical(SecurityCriticalScope.Everything), SuppressUnmanagedCodeSecurity]
    [
    ComImport,
    Guid("bad4b6e9-99af-42f8-a767-fbcfc5fca397"),
    InterfaceType(ComInterfaceType.InterfaceIsIUnknown)
    ]
    interface IPimcTablet
    {
        void GetKey(out Int32 key);
        void GetName([MarshalAs(UnmanagedType.LPWStr)] out string name);
        void GetPlugAndPlayId([MarshalAs(UnmanagedType.LPWStr)] out string plugAndPlayId);
        void GetTabletAndDisplaySize(out int tabletWidth, out int tabletHeight, out int displayWidth, out int displayHeight);
        void GetHardwareCaps(out int caps);
        void GetDeviceType(out int devType);
        void RefreshCursorInfo();
        void GetCursorCount(out int cCursors);
        void GetCursorInfo(int iCursor, [MarshalAs(UnmanagedType.LPWStr)] out string sName, out int id, [MarshalAs(UnmanagedType.Bool)] out bool fInverted);
        void GetCursorButtonCount(int iCursor, out int cButtons);
        void GetCursorButtonInfo (int iCursor, int iButton, [MarshalAs(UnmanagedType.LPWStr)] out string sName, out Guid guid);
        void IsPropertySupported(Guid guid, [MarshalAs(UnmanagedType.Bool)] out bool fSupported);
        void GetPropertyInfo(Guid guid, out int min, out int max, out int units, out float resolution);
        void CreateContext(IntPtr handle, [MarshalAs(UnmanagedType.Bool)] bool fEnable, uint timeout,
                                out IPimcContext IPimcContext, out Int32 key, out Int64 commHandle);
        void GetPacketDescriptionInfo(out int cProps, out int cButtons);
        void GetPacketPropertyInfo(int iProp, out Guid guid, out int iMin, out int iMax, out int iUnits, out float flResolution);
        void GetPacketButtonInfo(int iButton, out Guid guid);
    }

    /// <SecurityNote>
    ///     Critical: This elevates to unmanaged code permission.
    /// </SecurityNote>
    [SecurityCritical(SecurityCriticalScope.Everything), SuppressUnmanagedCodeSecurity]
    [
    ComImport,
    Guid(PimcConstants.IPimcManagerIID),
    InterfaceType(ComInterfaceType.InterfaceIsIUnknown)
    ]
    interface IPimcManager
    {
        void GetTabletCount(out UInt32 count);
        void GetTablet(UInt32 tablet, out IPimcTablet IPimcTablet);
    }

    internal static class PimcConstants
    {
        internal const string PimcManagerCLSID = "e23b1ced-5e47-4fdb-af66-b20370261b5e";
        internal const string IPimcManagerIID = "af44bf80-36dd-4118-b4cf-8b1e3f4fb9ce";
    }

}

