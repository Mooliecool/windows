using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace WinFormsSearchApp
{
    internal static class NativeMethods
    {
        // ********************************************************
        // SHGetKnownFolderPath
        // Defined in shlobj.h, exported from shell32.dll
        // HRESULT SHGetKnownFolderPath(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken,PWSTR *ppszPath);
        // REFKNOWNFOLDER is a pointer to a KNOWNFOLDERID, which is a GUID
        [DllImport("shell32.dll", CharSet=CharSet.Unicode, CallingConvention=CallingConvention.Winapi)]
        public static extern uint SHGetKnownFolderPath([In, MarshalAs(UnmanagedType.LPStruct)]Guid FolderID, [In]uint Flags, [In]IntPtr hToken, [Out] StringBuilder ppszPath);

    }

    // Defined in WinDef.h
    // Q. Is there any class in the .NETFx that exposes this?
    // #define MAX_PATH          260

    [Flags] 
    public enum KnownFolderFlags : uint
    {
        // KF_FLAG_CREATE (0x00008000) Forces the creation of the specified folder if that folder does not 
        // already exist. The security provisions predefined for that folder are applied. If the folder does 
        // not exist and cannot be created, the function returns a failure code and no path is returned.
        Create = 0x00008000, 

        // KF_FLAG_DONT_VERIFY (0x00004000) Do not verify the folder's existence before attempting to 
        // retrieve the path. If this flag is not set, an attempt to verify that the folder is truly present at 
        // the path. If that verification fails due to the folder being absent or inaccessable, the function 
        // returns a failure code and no path is returned. If the folder is located on a network, the function 
        // might take some time to execute. Setting this flag can reduce that lag.
        DoNotVerify = 0x00004000,

        // KF_FLAG_DONT_UNEXPAND (0x00002000) Retrieves the path as provided. If this flag is not set, 
        // portions of the path may be represented by environment strings such as %USERPROFILE%.
        DoNotExpand = 0x00002000,

        // KF_FLAG_NO_ALIAS (0x00001000) Gets the true system path for the folder, free of any aliased 
        // placeholders such as %USERPROFILE%. By default, known folder retrieval functions and methods 
        // return the aliased path if an alias exists.
        NoAlias = 0x00001000,

        // KF_FLAG_INIT (0x00000800) Initializes the folder using its Desktop.ini settings. If the folder 
        // cannot be initialized, the function returns a failure code and no path is returned. If the folder 
        // is located on a network, the function might take longer to execute.
        Init = 0x00000800, 

        // KF_FLAG_DEFAULT_PATH (0x00000400) Gets the default path for a known folder that is redirected 
        // elsewhere. If this flag is not set, the function retrieves the current—and possibly redirected—path 
        // of the folder. This flag includes a verification of the folder's existence unless 
        // KF_FLAG_DONT_VERIFY is also set.
        DefaultPath = 0x00000400,

        // KF_FLAG_NOT_PARENT_RELATIVE (0x00000200) Gets the folder's default path independent of the 
        // current location of its parent. KF_FLAG_DEFAULT_PATH must also be set.
        NotParentRelative = 0x00000200,
    }

    public static class KnownFolders
    {
        const int MAX_PATH = 260;
        static Guid Windows = new Guid("F38BF404-1D43-42F2-9305-67DE0B28FC23");
        static Guid ResourceDir = new Guid("{8AD10C31-2ADB-4296-A8F7-E4701232C972}");

        public static string GetWindowsFolderPath()
        {
            string systemFolder = System.IO.Path.GetDirectoryName(Environment.SystemDirectory);
            string folderPath = @"g:\windows";
            //// Can't get this to work. Returning Windows Title
            //uint flags = (uint)KnownFolderFlags.NoAlias;
            //StringBuilder outputStr = new StringBuilder(MAX_PATH);
            //outputStr.EnsureCapacity(MAX_PATH);
            //uint retVal = NativeMethods.SHGetKnownFolderPath(Windows, flags, IntPtr.Zero, outputStr);

            //string folderPath = outputStr.ToString();

            //outputStr.EnsureCapacity(MAX_PATH);
            //retVal = NativeMethods.SHGetKnownFolderPath(ResourceDir, flags, IntPtr.Zero, outputStr);
            
            return (folderPath);
        }
    }    
}
