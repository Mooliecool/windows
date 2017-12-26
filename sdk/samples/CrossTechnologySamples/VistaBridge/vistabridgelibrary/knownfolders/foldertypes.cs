using System;
using System.Collections.Generic;
using System.Text;

namespace Microsoft.SDK.Samples.VistaBridge.Library.KnownFolders
{
    internal static class FolderTypes
    {
        internal static Guid NotSpecified = new Guid(
            0x5c4f28b5, 0xf869, 0x4e84, 0x8e, 0x60, 0xf1, 0x1d, 0xb9, 0x7c, 0x5c, 0xc7);
        internal static Guid Invalid = new Guid(
            0x57807898, 0x8c4f, 0x4462, 0xbb, 0x63, 0x71, 0x04, 0x23, 0x80, 0xb1, 0x09);
        internal static Guid Documents = new Guid(
            0x7d49d726, 0x3c21, 0x4f05, 0x99, 0xaa, 0xfd, 0xc2, 0xc9, 0x47, 0x46, 0x56);

        internal static Guid Pictures = new Guid(
            0xb3690e58, 0xe961, 0x423b, 0xb6, 0x87, 0x38, 0x6e, 0xbf, 0xd8, 0x32, 0x39);

        internal static Guid Music = new Guid(
            0xaf9c03d6, 0x7db9, 0x4a15, 0x94, 0x64, 0x13, 0xbf, 0x9f, 0xb6, 0x9a, 0x2a);

        internal static Guid MusicIcons = new Guid(
            0x0b7467fb, 0x84ba, 0x4aae, 0xa0, 0x9b, 0x15, 0xb7, 0x10, 0x97, 0xaf, 0x9e);

        internal static Guid Games = new Guid(
            0xb689b0d0, 0x76d3, 0x4cbb, 0x87, 0xf7, 0x58, 0x5d, 0x0e, 0x0c, 0xe0, 0x70);

        internal static Guid ControlPanelCategory = new Guid(
            0xde4f0660, 0xfa10, 0x4b8f, 0xa4, 0x94, 0x06, 0x8b, 0x20, 0xb2, 0x23, 0x07);

        internal static Guid ControlPanelClassic = new Guid(
            0x0c3794f3, 0xb545, 0x43aa, 0xa3, 0x29, 0xc3, 0x74, 0x30, 0xc5, 0x8d, 0x2a);

        internal static Guid Printers = new Guid(
            0x2c7bbec6, 0xc844, 0x4a0a, 0x91, 0xfa, 0xce, 0xf6, 0xf5, 0x9c, 0xfd, 0xa1);

        internal static Guid RecycleBin = new Guid(
            0xd6d9e004, 0xcd87, 0x442b, 0x9d, 0x57, 0x5e, 0x0a, 0xeb, 0x4f, 0x6f, 0x72);
        internal static Guid SoftwareExplorer = new Guid(
            0xd674391b, 0x52d9, 0x4e07, 0x83, 0x4e, 0x67, 0xc9, 0x86, 0x10, 0xf3, 0x9d);

        internal static Guid CompressedFolder = new Guid(
             0x80213e82, 0xbcfd, 0x4c4f, 0x88, 0x17, 0xbb, 0x27, 0x60, 0x12, 0x67, 0xa9);

        internal static Guid Contacts = new Guid(
             0xde2b70ec, 0x9bf7, 0x4a93, 0xbd, 0x3d, 0x24, 0x3f, 0x78, 0x81, 0xd4, 0x92);
           
        internal static Guid Library = new Guid(
             0x4badfc68, 0xc4ac, 0x4716, 0xa0, 0xa0, 0x4d, 0x5d, 0xaa, 0x6b, 0x0f, 0x3e);
            
        internal static Guid NetworkExplorer = new Guid(
             0x25cc242b, 0x9a7c, 0x4f51, 0x80, 0xe0, 0x7a, 0x29, 0x28, 0xfe, 0xbe, 0x42);
         internal static Guid UserFiles = new Guid(
             0xcd0fc69b, 0x71e2, 0x46e5, 0x96, 0x90, 0x5b, 0xcd, 0x9f, 0x57, 0xaa, 0xb3);

         static Dictionary<Guid, string> types;

         static FolderTypes()
         {
             types = new Dictionary<Guid, string>();
             types.Add(NotSpecified, "Not Specified");            
             types.Add(Invalid, "Invalid");
             types.Add(Documents, "Documents");
             types.Add(Pictures, "Pictures");
             types.Add(Music, "Music");
             types.Add(MusicIcons, "Music Icons");
             types.Add(Games, "Games");
             types.Add(ControlPanelCategory, "ControlPanel Category");
             types.Add(ControlPanelClassic, "ControlPanel Classic");
             types.Add(Printers, "Printers");
             types.Add(RecycleBin, "RecycleBin");
             types.Add(SoftwareExplorer, "Software Explorer");
             types.Add(CompressedFolder, "Compressed Folder");
             types.Add(Contacts, "Contacts");
             types.Add(Library, "Library");
             types.Add(NetworkExplorer, "Network Explorer");
             types.Add(UserFiles, "User Files");
         }
         
         internal static string GetFolderType(Guid typeId)
         {
             if (typeId == Guid.Empty)
                 return String.Empty;

             return types[typeId];
         }
    }
}
