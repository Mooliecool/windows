// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
/*============================================================
**
** Class:  DirectoryInfo
**
**
** Purpose: Exposes routines for enumerating through a 
** directory.
**
**          April 11,2000
**
===========================================================*/

using System;
using System.Collections;
using System.Security;
using System.Security.Permissions;
using Microsoft.Win32;
using System.Text;
using System.Runtime.InteropServices;
using System.Globalization;
using System.Runtime.Serialization;
using System.Runtime.Versioning;

namespace System.IO {
    [Serializable]
    [ComVisible(true)]
    public sealed class DirectoryInfo : FileSystemInfo {
        private String[] demandDir;
        
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public DirectoryInfo(String path)
        {
            if (path==null)
                throw new ArgumentNullException("path");

            // Special case "<DriveLetter>:" to point to "<CurrentDirectory>" instead
            if ((path.Length == 2) && (path[1] == ':'))
                OriginalPath = ".";
            else 
                OriginalPath = path;

            // Must fully qualify the path for the security check
            String fullPath = Path.GetFullPathInternal(path);

            demandDir = new String[] {Directory.GetDemandDir(fullPath, true)};
            new FileIOPermission(FileIOPermissionAccess.Read, demandDir, false, false ).Demand();

            FullPath = fullPath;            
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal DirectoryInfo(String fullPath, bool junk)
        {
            BCLDebug.Assert(Path.GetRootLength(fullPath) > 0, "fullPath must be fully qualified!");
            // Fast path when we know a DirectoryInfo exists.
            OriginalPath = Path.GetFileName(fullPath);
            FullPath = fullPath;
            demandDir = new String[] {Directory.GetDemandDir(fullPath, true)};
        }

        private DirectoryInfo(SerializationInfo info, StreamingContext context) : base(info, context)
        {
            demandDir = new String[] {Directory.GetDemandDir(FullPath, true)};
            new FileIOPermission(FileIOPermissionAccess.Read, demandDir, false, false ).Demand();
        }

        public override String Name {
            [ResourceExposure(ResourceScope.Machine)]
            [ResourceConsumption(ResourceScope.Machine)]
            get {
                // FullPath might be either "c:\bar" or "c:\bar\".  Handle 
                // those cases, as well as avoiding mangling "c:\".
                String s = FullPath;
                if (s.Length > 3) {
                    if (s.EndsWith(Path.DirectorySeparatorChar))
                        s = FullPath.Substring(0, FullPath.Length - 1);
                    return Path.GetFileName(s);
                }
                return FullPath;  // For rooted paths, like "c:\"
            }
        }

        public DirectoryInfo Parent {
            [ResourceExposure(ResourceScope.Machine)]
            [ResourceConsumption(ResourceScope.Machine)]
            get {
                String parentName;
                // FullPath might be either "c:\bar" or "c:\bar\".  Handle 
                // those cases, as well as avoiding mangling "c:\".
                String s = FullPath;
                if (s.Length > 3 && s.EndsWith(Path.DirectorySeparatorChar))
                    s = FullPath.Substring(0, FullPath.Length - 1);                
                parentName = Path.GetDirectoryName(s);
                if (parentName==null)
                    return null;
                DirectoryInfo dir = new DirectoryInfo(parentName,false);
                new FileIOPermission(FileIOPermissionAccess.PathDiscovery | FileIOPermissionAccess.Read, dir.demandDir, false, false).Demand();
                return dir;
            }
        }

      
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]      
        public DirectoryInfo CreateSubdirectory(String path) {
            if (path==null)
                throw new ArgumentNullException("path");
            
            String newDirs = Path.InternalCombine(FullPath, path);
            String fullPath = Path.GetFullPathInternal(newDirs);

            if (0!=String.Compare(FullPath,0,fullPath,0, FullPath.Length,StringComparison.OrdinalIgnoreCase)) {
                String displayPath = __Error.GetDisplayablePath(OriginalPath, false);
                throw new ArgumentException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("Argument_InvalidSubPath"), path, displayPath));
            }

            // Ensure we have permission to create this subdirectory.
            String demandDir = Directory.GetDemandDir(fullPath, true); 
            new FileIOPermission(FileIOPermissionAccess.Write, new String[] { demandDir }, false, false ).Demand();

            Directory.InternalCreateDirectory(fullPath, path);
            // Check for read permission to directory we hand back by calling this constructor.
            return new DirectoryInfo(fullPath);
        }

        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        public void Create()
        {
            Directory.InternalCreateDirectory(FullPath, OriginalPath);
        }


        // Tests if the given path refers to an existing DirectoryInfo on disk.
        // 
        // Your application must have Read permission to the directory's
        // contents.
        //
        public override bool Exists {
            get
            {
                try
                {
                    if (_dataInitialised == -1)
                        Refresh();
                    if (_dataInitialised != 0) // Refresh was unable to initialise the data
                        return false;
                   
                    return _data.fileAttributes != -1 && (_data.fileAttributes & Win32Native.FILE_ATTRIBUTE_DIRECTORY) != 0;
                }
                catch
                {
                    return false;
                }
            }
        }
      

        // Returns an array of Files in the current DirectoryInfo matching the 
        // given search criteria (ie, "*.txt").
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileInfo[] GetFiles(String searchPattern)
        {
            return GetFiles(searchPattern, SearchOption.TopDirectoryOnly);
        }

        
        // Converts the fully qualified user path returned by InternalGetFileDirectoryNames
        // into fullpath by combining the relevant portion of it with the fullpath of this directory 
        // For ex, converts  foo\bar.txt into 'c:\temp\foo\bar.txt', where FullPath is 'c:\temp\foo'
        // and OriginalPath (aka userpath) is 'foo'
        private string FixupFileDirFullPath(String fileDirUserPath)
        {
            BCLDebug.Assert(fileDirUserPath != null,"InternalGetFileDirectoryNames returned paths should not be null!");
            BCLDebug.Assert(fileDirUserPath.StartsWith(OriginalPath, StringComparison.Ordinal),"InternalGetFileDirectoryNames returned paths should start with user path!");
            
            String fileDirFullPath;
            
            if (OriginalPath.Length == 0) {
                fileDirFullPath = Path.InternalCombine(FullPath, fileDirUserPath);
            }
            else if (OriginalPath.EndsWith(Path.DirectorySeparatorChar) || OriginalPath.EndsWith(Path.AltDirectorySeparatorChar)) {
                BCLDebug.Assert((fileDirUserPath[OriginalPath.Length-1] == Path.DirectorySeparatorChar) 
                                 || (fileDirUserPath[OriginalPath.Length-1] == Path.AltDirectorySeparatorChar),"InternalGetFileDirectoryNames returned incorrect user path!");
                fileDirFullPath = Path.InternalCombine(FullPath, fileDirUserPath.Substring(OriginalPath.Length));
            }
            else {
                BCLDebug.Assert((fileDirUserPath[OriginalPath.Length] == Path.DirectorySeparatorChar),"InternalGetFileDirectoryNames returned incorrect user path!");
                fileDirFullPath = Path.InternalCombine(FullPath, fileDirUserPath.Substring(OriginalPath.Length + 1));
            }

            return fileDirFullPath;
        }

        // Returns an array of Files in the current DirectoryInfo matching the 
        // given search criteria (ie, "*.txt").
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileInfo[] GetFiles(String searchPattern, SearchOption searchOption)
        {
            if (searchPattern==null)
                throw new ArgumentNullException("searchPattern");

            String[] fileNames = Directory.InternalGetFileDirectoryNames(FullPath, OriginalPath, searchPattern, true, false, searchOption);
                 
            // We need full path for permission check. 
            // InternalGetFileDirectoryNames returns qualified user path, 
            // i.e, path starts from OriginalPath. We need to convert this to fullpath. 
            for (int i = 0; i < fileNames.Length; i++)  {
                fileNames[i] = FixupFileDirFullPath(fileNames[i]);
            }
       
            if (fileNames.Length != 0)
                new FileIOPermission(FileIOPermissionAccess.Read, fileNames, false, false).Demand();

            FileInfo[] files = new FileInfo[fileNames.Length];
            for(int i=0; i<fileNames.Length; i++)
                files[i] = new FileInfo(fileNames[i], false);
            return files;
        }

        // Returns an array of Files in the DirectoryInfo specified by path
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileInfo[] GetFiles()
        {
            return GetFiles("*");
        }

        // Returns an array of Directories in the current directory.
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public DirectoryInfo[] GetDirectories()
        {
            return GetDirectories("*");
        }

        // Returns an array of strongly typed FileSystemInfo entries in the path with the
        // given search criteria (ie, "*.txt").
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileSystemInfo[] GetFileSystemInfos(String searchPattern)
        {
            return GetFileSystemInfos(searchPattern, SearchOption.TopDirectoryOnly);
        }

        // Returns an array of strongly typed FileSystemInfo entries in the path with the
        // given search criteria (ie, "*.txt").
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        private FileSystemInfo[] GetFileSystemInfos(String searchPattern, SearchOption searchOption)
        {
            if (searchPattern==null)
                throw new ArgumentNullException("searchPattern");

            String [] dirNames = Directory.InternalGetFileDirectoryNames(FullPath, OriginalPath, searchPattern, false, true, searchOption);
            String [] fileNames = Directory.InternalGetFileDirectoryNames(FullPath, OriginalPath, searchPattern, true, false, searchOption);
            FileSystemInfo [] fileSystemEntries = new FileSystemInfo[dirNames.Length + fileNames.Length];
            String[] permissionNames = new String[dirNames.Length];

            // We need full path for permission check. 
            // InternalGetFileDirectoryNames returns qualified user path, 
            // i.e, path starts from OriginalPath. We need to convert this to fullpath. 
            
            for (int i = 0;i<dirNames.Length;i++) { 
                BCLDebug.Assert(!dirNames[i].EndsWith(Path.DirectorySeparatorChar),"InternalGetFileDirectoryNames returned paths should not have trailing slash!");

                dirNames[i] = FixupFileDirFullPath(dirNames[i]);
                permissionNames[i] = dirNames[i] + "\\."; // these will never have a slash at end
            }
            if (dirNames.Length != 0)
                new FileIOPermission(FileIOPermissionAccess.Read,permissionNames,false,false).Demand();
           
            for (int i = 0; i < fileNames.Length; i++)  {
                fileNames[i] = FixupFileDirFullPath(fileNames[i]);
            }
            
            if (fileNames.Length != 0)
                new FileIOPermission(FileIOPermissionAccess.Read,fileNames,false,false).Demand();
           
            int count = 0;
            for (int i = 0;i<dirNames.Length;i++)
                fileSystemEntries[count++] = new DirectoryInfo(dirNames[i], false);
           
            for (int i = 0;i<fileNames.Length;i++)
                fileSystemEntries[count++] = new FileInfo(fileNames[i], false);
           
            return fileSystemEntries;
        }



        // Returns an array of strongly typed FileSystemInfo entries which will contain a listing
        // of all the files and directories.
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileSystemInfo[] GetFileSystemInfos()
        {
           return GetFileSystemInfos("*");
        }

        // Returns an array of Directories in the current DirectoryInfo matching the 
        // given search criteria (ie, "System*" could match the System & System32
        // directories).
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public DirectoryInfo[] GetDirectories(String searchPattern)
        {
            return GetDirectories(searchPattern, SearchOption.TopDirectoryOnly);
        }

        // Returns an array of Directories in the current DirectoryInfo matching the 
        // given search criteria (ie, "System*" could match the System & System32
        // directories).
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public DirectoryInfo[] GetDirectories(String searchPattern, SearchOption searchOption)
        {
            if (searchPattern==null)
                throw new ArgumentNullException("searchPattern");

            String[] dirNames = Directory.InternalGetFileDirectoryNames(FullPath, OriginalPath, searchPattern, false, true, searchOption);
            String[] permissionNames = new String[dirNames.Length];
            
            // We need full path for permission check. 
            // InternalGetFileDirectoryNames returns qualified user path, 
            // i.e, path starts from OriginalPath. We need to convert this to fullpath. 
            for (int i = 0; i<dirNames.Length; i++) {
                BCLDebug.Assert(!dirNames[i].EndsWith(Path.DirectorySeparatorChar),"InternalGetFileDirectoryNames returned paths should not have trailing slash!");
                
                dirNames[i] = FixupFileDirFullPath(dirNames[i]);
                permissionNames[i] = dirNames[i] + "\\."; // these will never have a slash at end
            }
            if (dirNames.Length != 0)
                new FileIOPermission(FileIOPermissionAccess.Read,permissionNames,false, false).Demand();

            DirectoryInfo[] dirs = new DirectoryInfo[dirNames.Length];
            for(int i=0; i<dirNames.Length; i++)
                dirs[i] = new DirectoryInfo(dirNames[i], false);
            return dirs;
        }

    

        
#if !PLATFORM_UNIX
        // Returns the root portion of the given path. The resulting string
        // consists of those rightmost characters of the path that constitute the
        // root of the path. Possible patterns for the resulting string are: An
        // empty string (a relative path on the current drive), "\" (an absolute
        // path on the current drive), "X:" (a relative path on a given drive,
        // where X is the drive letter), "X:\" (an absolute path on a given drive),
        // and "\\server\share" (a UNC path for a given server and share name).
        // The resulting string is null if path is null.
        //
#else
        // Returns the root portion of the given path. The resulting string
        // consists of those rightmost characters of the path that constitute the
        // root of the path. Possible patterns for the resulting string are: An
        // empty string (a relative path on the current drive), "\" (an absolute
        // path on the current drive)
        // The resulting string is null if path is null.
        //
#endif // !PLATFORM_UNIX

        public DirectoryInfo Root { 
            [ResourceExposure(ResourceScope.None)]
            [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
            get
            {
                String demandPath;
                int rootLength = Path.GetRootLength(FullPath);
                String rootPath = FullPath.Substring(0, rootLength);
                demandPath = Directory.GetDemandDir(rootPath, true);

                new FileIOPermission(FileIOPermissionAccess.PathDiscovery, new String[] { demandPath }, false, false ).Demand();
                return new DirectoryInfo(rootPath);
            }
        } 

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public void MoveTo(String destDirName) {
            if (destDirName==null)
                throw new ArgumentNullException("destDirName");
            if (destDirName.Length==0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyFileName"), "destDirName");
            
            new FileIOPermission(FileIOPermissionAccess.Write | FileIOPermissionAccess.Read, demandDir, false, false).Demand();
            String fullDestDirName = Path.GetFullPathInternal(destDirName);
            String demandPath;
            if (!fullDestDirName.EndsWith(Path.DirectorySeparatorChar))
                fullDestDirName = fullDestDirName + Path.DirectorySeparatorChar;

            demandPath = fullDestDirName + '.';

            // Demand read & write permission to destination.  The reason is
            // we hand back a DirectoryInfo to the destination that would allow
            // you to read a directory listing from that directory.  Sure, you 
            // had the ability to read the file contents in the old location,
            // but you technically also need read permissions to the new 
            // location as well, and write is not a true superset of read.
            new FileIOPermission(FileIOPermissionAccess.Write | FileIOPermissionAccess.Read, demandPath).Demand();

            String fullSourcePath;
            if (FullPath.EndsWith(Path.DirectorySeparatorChar))
                fullSourcePath = FullPath;
            else
                fullSourcePath = FullPath + Path.DirectorySeparatorChar;

            if (CultureInfo.InvariantCulture.CompareInfo.Compare(fullSourcePath, fullDestDirName, CompareOptions.IgnoreCase) == 0)
                throw new IOException(Environment.GetResourceString("IO.IO_SourceDestMustBeDifferent"));

            String sourceRoot = Path.GetPathRoot(fullSourcePath);
            String destinationRoot = Path.GetPathRoot(fullDestDirName);

            if (CultureInfo.InvariantCulture.CompareInfo.Compare(sourceRoot, destinationRoot, CompareOptions.IgnoreCase) != 0)
                throw new IOException(Environment.GetResourceString("IO.IO_SourceDestMustHaveSameRoot"));
            
            if (Environment.IsWin9X() && !Directory.InternalExists(FullPath)) // For Win9x
                throw new DirectoryNotFoundException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("IO.PathNotFound_Path"), destDirName));
    
            
            if (!Win32Native.MoveFile(FullPath, destDirName))
            {
                int hr = Marshal.GetLastWin32Error();
                if (hr == Win32Native.ERROR_FILE_NOT_FOUND) // A dubious error code
                {
                    hr = Win32Native.ERROR_PATH_NOT_FOUND;
                    __Error.WinIOError(hr, OriginalPath);
                }
                
                if (hr == Win32Native.ERROR_ACCESS_DENIED) // We did this for Win9x. We can't change it for backcomp. 
                    throw new IOException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("UnauthorizedAccess_IODenied_Path"), OriginalPath));
            
                __Error.WinIOError(hr,String.Empty);
            }
            FullPath = fullDestDirName;
            OriginalPath = destDirName;
            demandDir = new String[] { Directory.GetDemandDir(FullPath, true) };

            // Flush any cached information about the directory.
            _dataInitialised = -1;
        }

        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        public override void Delete()
        {
            Directory.Delete(FullPath, OriginalPath, false);
        }

        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        public void Delete(bool recursive)
        {
            Directory.Delete(FullPath, OriginalPath, recursive);
        }

        // Returns the fully qualified path
        public override String ToString()
        {
            return OriginalPath;
        }
    }       
}

