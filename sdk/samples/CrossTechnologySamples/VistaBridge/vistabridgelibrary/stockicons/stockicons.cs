using System;
using System.Runtime.InteropServices; 
using Freezable = System.Windows.Freezable;
using InteropImaging = System.Windows.Interop.Imaging;
using MarkupExtension = System.Windows.Markup.MarkupExtension;
using ImageSource = System.Windows.Media.ImageSource;
using BitmapSource = System.Windows.Media.Imaging.BitmapSource;
using TypeConverter = System.ComponentModel.TypeConverter;

using TypeConverterAttribute = System.ComponentModel.TypeConverterAttribute;
using ITypeDescriptorContext = System.ComponentModel.ITypeDescriptorContext;
using EnumConverter = System.ComponentModel.EnumConverter;
using CultureInfo = System.Globalization.CultureInfo;
using Microsoft.SDK.Samples.VistaBridge.Interop;

namespace Microsoft.SDK.Samples.VistaBridge.Library.StockIcons 
{
    /// <summary>
    /// Static class that provides access to the standard system icons.
    /// </summary>
    public static class StockIcons
    {
        /// <summary>
        /// Retrieves the bitmap for the specified stock icon.
        /// </summary>
        /// <param name="identifier"></param>
        /// <returns>The icon to retrieve.</returns>
        public static BitmapSource GetBitmapSource(StockIconIdentifier identifier)
        {
            return StockIcon.GetBitmapSource(identifier, 0);
        }
        public static BitmapSource DocumentNotAssociated { get { return GetBitmapSource(StockIconIdentifier.DocumentNotAssociated); } }
        public static BitmapSource DocumentAssociated { get { return GetBitmapSource(StockIconIdentifier.DocumentAssociated); } }
        public static BitmapSource Application { get { return GetBitmapSource(StockIconIdentifier.Application); } }
        public static BitmapSource Folder { get { return GetBitmapSource(StockIconIdentifier.Folder); } }
        public static BitmapSource FolderOpen { get { return GetBitmapSource(StockIconIdentifier.FolderOpen); } }
        public static BitmapSource Drive525 { get { return GetBitmapSource(StockIconIdentifier.Drive525); } }
        public static BitmapSource Drive35 { get { return GetBitmapSource(StockIconIdentifier.Drive35); } }
        public static BitmapSource DriveRemove { get { return GetBitmapSource(StockIconIdentifier.DriveRemove); } }
        public static BitmapSource DriveFixed { get { return GetBitmapSource(StockIconIdentifier.DriveFixed); } }
        public static BitmapSource DriveNetwork { get { return GetBitmapSource(StockIconIdentifier.DriveNetwork); } }
        public static BitmapSource DriveNetworkDisabled { get { return GetBitmapSource(StockIconIdentifier.DriveNetworkDisabled); } }
        public static BitmapSource DriveCD { get { return GetBitmapSource(StockIconIdentifier.DriveCD); } }
        public static BitmapSource DriveRam { get { return GetBitmapSource(StockIconIdentifier.DriveRam); } }
        public static BitmapSource World { get { return GetBitmapSource(StockIconIdentifier.World); } }
        public static BitmapSource Server { get { return GetBitmapSource(StockIconIdentifier.Server); } }
        public static BitmapSource Printer { get { return GetBitmapSource(StockIconIdentifier.Printer); } }
        public static BitmapSource MyNetwork { get { return GetBitmapSource(StockIconIdentifier.MyNetwork); } }
        public static BitmapSource Find { get { return GetBitmapSource(StockIconIdentifier.Find); } }
        public static BitmapSource Help { get { return GetBitmapSource(StockIconIdentifier.Help); } }
        public static BitmapSource Share { get { return GetBitmapSource(StockIconIdentifier.Share); } }
        public static BitmapSource Link { get { return GetBitmapSource(StockIconIdentifier.Link); } }
        public static BitmapSource SlowFile { get { return GetBitmapSource(StockIconIdentifier.SlowFile); } }
        public static BitmapSource Recycler { get { return GetBitmapSource(StockIconIdentifier.Recycler); } }
        public static BitmapSource RecyclerFull { get { return GetBitmapSource(StockIconIdentifier.RecyclerFull); } }
        public static BitmapSource MediaCDAudio { get { return GetBitmapSource(StockIconIdentifier.MediaCDAudio); } }
        public static BitmapSource Lock { get { return GetBitmapSource(StockIconIdentifier.Lock); } }
        public static BitmapSource AutoList { get { return GetBitmapSource(StockIconIdentifier.AutoList); } }
        public static BitmapSource PrinterNet { get { return GetBitmapSource(StockIconIdentifier.PrinterNet); } }
        public static BitmapSource ServerShare { get { return GetBitmapSource(StockIconIdentifier.ServerShare); } }
        public static BitmapSource PrinterFax { get { return GetBitmapSource(StockIconIdentifier.PrinterFax); } }
        public static BitmapSource PrinterFaxNet { get { return GetBitmapSource(StockIconIdentifier.PrinterFaxNet); } }
        public static BitmapSource PrinterFile { get { return GetBitmapSource(StockIconIdentifier.PrinterFile); } }
        public static BitmapSource Stack { get { return GetBitmapSource(StockIconIdentifier.Stack); } }
        public static BitmapSource MediaSvcd { get { return GetBitmapSource(StockIconIdentifier.MediaSvcd); } }
        public static BitmapSource StuffedFolder { get { return GetBitmapSource(StockIconIdentifier.StuffedFolder); } }
        public static BitmapSource DriveUnknown { get { return GetBitmapSource(StockIconIdentifier.DriveUnknown); } }
        public static BitmapSource DriveDvd { get { return GetBitmapSource(StockIconIdentifier.DriveDvd); } }
        public static BitmapSource MediaDvd { get { return GetBitmapSource(StockIconIdentifier.MediaDvd); } }
        public static BitmapSource MediaDvdRam { get { return GetBitmapSource(StockIconIdentifier.MediaDvdRam); } }
        public static BitmapSource MediaDvdRW { get { return GetBitmapSource(StockIconIdentifier.MediaDvdRW); } }
        public static BitmapSource MediaDvdR { get { return GetBitmapSource(StockIconIdentifier.MediaDvdR); } }
        public static BitmapSource MediaDvdRom { get { return GetBitmapSource(StockIconIdentifier.MediaDvdRom); } }
        public static BitmapSource MediaCDAudioPlus { get { return GetBitmapSource(StockIconIdentifier.MediaCDAudioPlus); } }
        public static BitmapSource MediaCDRW { get { return GetBitmapSource(StockIconIdentifier.MediaCDRW); } }
        public static BitmapSource MediaCDR { get { return GetBitmapSource(StockIconIdentifier.MediaCDR); } }
        public static BitmapSource MediaCDBurn { get { return GetBitmapSource(StockIconIdentifier.MediaCDBurn); } }
        public static BitmapSource MediaBlankCD { get { return GetBitmapSource(StockIconIdentifier.MediaBlankCD); } }
        public static BitmapSource MediaCDRom { get { return GetBitmapSource(StockIconIdentifier.MediaCDRom); } }
        public static BitmapSource AudioFiles { get { return GetBitmapSource(StockIconIdentifier.AudioFiles); } }
        public static BitmapSource ImageFiles { get { return GetBitmapSource(StockIconIdentifier.ImageFiles); } }
        public static BitmapSource VideoFiles { get { return GetBitmapSource(StockIconIdentifier.VideoFiles); } }
        public static BitmapSource MixedFiles { get { return GetBitmapSource(StockIconIdentifier.MixedFiles); } }
        public static BitmapSource FolderBack { get { return GetBitmapSource(StockIconIdentifier.FolderBack); } }
        public static BitmapSource FolderFront { get { return GetBitmapSource(StockIconIdentifier.FolderFront); } }
        public static BitmapSource Shield { get { return GetBitmapSource(StockIconIdentifier.Shield); } }
        public static BitmapSource Warning { get { return GetBitmapSource(StockIconIdentifier.Warning); } }
        public static BitmapSource Info { get { return GetBitmapSource(StockIconIdentifier.Info); } }
        public static BitmapSource Error { get { return GetBitmapSource(StockIconIdentifier.Error); } }
        public static BitmapSource Key { get { return GetBitmapSource(StockIconIdentifier.Key); } }
        public static BitmapSource Software { get { return GetBitmapSource(StockIconIdentifier.Software); } }
        public static BitmapSource Rename { get { return GetBitmapSource(StockIconIdentifier.Rename); } }
        public static BitmapSource Delete { get { return GetBitmapSource(StockIconIdentifier.Delete); } }
        public static BitmapSource MediaAudioDvd { get { return GetBitmapSource(StockIconIdentifier.MediaAudioDvd); } }
        public static BitmapSource MediaMovieDvd { get { return GetBitmapSource(StockIconIdentifier.MediaMovieDvd); } }
        public static BitmapSource MediaEnhancedCD { get { return GetBitmapSource(StockIconIdentifier.MediaEnhancedCD); } }
        public static BitmapSource MediaEnhancedDvd { get { return GetBitmapSource(StockIconIdentifier.MediaEnhancedDvd); } }
        public static BitmapSource MediaHDDvd { get { return GetBitmapSource(StockIconIdentifier.MediaHDDvd); } }
        public static BitmapSource MediaBluRay { get { return GetBitmapSource(StockIconIdentifier.MediaBluRay); } }
        public static BitmapSource MediaVcd { get { return GetBitmapSource(StockIconIdentifier.MediaVcd); } }
        public static BitmapSource MediaDvdPlusR { get { return GetBitmapSource(StockIconIdentifier.MediaDvdPlusR); } }
        public static BitmapSource MediaDvdPlusRW { get { return GetBitmapSource(StockIconIdentifier.MediaDvdPlusRW); } }
        public static BitmapSource DesktopPC { get { return GetBitmapSource(StockIconIdentifier.DesktopPC); } }
        public static BitmapSource MobilePC { get { return GetBitmapSource(StockIconIdentifier.MobilePC); } }
        public static BitmapSource Users { get { return GetBitmapSource(StockIconIdentifier.Users); } }
        public static BitmapSource MediaSmartMedia { get { return GetBitmapSource(StockIconIdentifier.MediaSmartMedia); } }
        public static BitmapSource MediaCompactFlash { get { return GetBitmapSource(StockIconIdentifier.MediaCompactFlash); } }
        public static BitmapSource DeviceCellPhone { get { return GetBitmapSource(StockIconIdentifier.DeviceCellPhone); } }
        public static BitmapSource DeviceCamera { get { return GetBitmapSource(StockIconIdentifier.DeviceCamera); } }
        public static BitmapSource DeviceVideoCamera { get { return GetBitmapSource(StockIconIdentifier.DeviceVideoCamera); } }
        public static BitmapSource DeviceAudioPlayer { get { return GetBitmapSource(StockIconIdentifier.DeviceAudioPlayer); } }
        public static BitmapSource NetworkConnect { get { return GetBitmapSource(StockIconIdentifier.NetworkConnect); } }
        public static BitmapSource Internet { get { return GetBitmapSource(StockIconIdentifier.Internet); } }
        public static BitmapSource ZipFile { get { return GetBitmapSource(StockIconIdentifier.ZipFile); } }
        public static BitmapSource Settings { get { return GetBitmapSource(StockIconIdentifier.Settings); } }
    }
}
