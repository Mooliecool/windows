using System;
using Microsoft.SDK.Samples.VistaBridge.Interop;
using System.Windows.Media;
using System.Windows.Interop;

namespace Microsoft.SDK.Samples.VistaBridge.Library.StockIcons
{
    /// <summary>
    /// Static class that provides helper methods for the StockIcon class.
    /// </summary>
    internal static class StockIconHelper
    {
        /// <summary>
        /// Gets the specified icon.
        /// </summary>
        /// <param name="identifier">The StockIdentifier enumeration value that
        /// that identifies the icon to retrieve.
        /// </param>
        /// <param name="flags">
        /// The StockIconOptions enumeration values that control 
        /// the visual representation of the icon.
        /// </param>
        /// <returns></returns>
        internal static ImageSource MakeImage(
            StockIconIdentifier identifier,
            StockIconOptions flags)
        {
            IntPtr iconHandle = GetIcon(identifier, flags);
            ImageSource imageSource;
            try
            {
                imageSource = Imaging.CreateBitmapSourceFromHIcon(iconHandle, System.Windows.Int32Rect.Empty, null);
            }
            finally
            {
                UnsafeNativeMethods.DestroyIcon(iconHandle);
            }
            return imageSource;
        }

        internal static IntPtr GetIcon(StockIconIdentifier identifier,
            StockIconOptions flags)
        {
            SafeNativeMethods.StockIconInfo info = new SafeNativeMethods.StockIconInfo();
            info.StuctureSize = (UInt32)System.Runtime.InteropServices.Marshal.SizeOf(typeof(SafeNativeMethods.StockIconInfo));

            int hResult =
                UnsafeNativeMethods.SHGetStockIconInfo(identifier, flags, ref info);

            if (hResult != 0)
                throw new System.ComponentModel.Win32Exception("SHGetStockIconInfo execution failure " + hResult.ToString());

            return info.Handle;
        }
    }
}
