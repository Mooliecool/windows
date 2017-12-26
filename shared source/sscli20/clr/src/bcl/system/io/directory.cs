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
** Class:  Directory
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
using System.Collections.Generic;
using System.Security;
using System.Security.Permissions;
using Microsoft.Win32;
using Microsoft.Win32.SafeHandles;
using System.Text;
using System.Runtime.InteropServices;
using System.Globalization;
using System.Runtime.Versioning;


namespace System.IO {
    [ComVisible(true)]
    public static class Directory {
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static DirectoryInfo GetParent(String path)
        {
            if (path==null)
                throw new ArgumentNullException("path");

            if (path.Length==0)
                throw new ArgumentException(Environment.GetResourceString("Argument_PathEmpty"), "path");

            String fullPath = Path.GetFullPathInternal(path);
                    
            String s = Path.GetDirectoryName(fullPath);
            if (s==null)
                 return null;
            return new DirectoryInfo(s);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static DirectoryInfo CreateDirectory(String path) {
            if (path==null)
                throw new ArgumentNullException("path");
            if (path.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_PathEmpty"));
            
            String fullPath = Path.GetFullPathInternal(path);

            // You need read access to the directory to be returned back and write access to all the directories 
            // that you need to create. If we fail any security checks we will not create any directories at all.
            // We attempt to create directories only after all the security checks have passed. This is avoid doing
            // a demand at every level.
            String demandDir = GetDemandDir(fullPath, true); 
            new FileIOPermission(FileIOPermissionAccess.Read, new String[] { demandDir }, false, false ).Demand();
            InternalCreateDirectory(fullPath, path);
            
            return new DirectoryInfo(fullPath, false);
        }

        // Input to this method should already be fullpath. This method will ensure that we append 
        // the trailing slash only when appropriate and when thisDirOnly is specified append a "." 
        // at the end of the path to indicate that the demand is only for the fullpath and not 
        // everything underneath it.
        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.None, ResourceScope.None)]
        internal static String GetDemandDir(string fullPath, bool thisDirOnly)
        {
            String demandPath;

            if (thisDirOnly) {
                if (fullPath.EndsWith( Path.DirectorySeparatorChar ) 
                    || fullPath.EndsWith( Path.AltDirectorySeparatorChar ) )
                    demandPath = fullPath + '.';
                else
                    demandPath = fullPath + Path.DirectorySeparatorChar + '.';
            }
            else {
                if (!(fullPath.EndsWith( Path.DirectorySeparatorChar ) 
                    || fullPath.EndsWith( Path.AltDirectorySeparatorChar )) )
                    demandPath = fullPath + Path.DirectorySeparatorChar;
                else
                    demandPath = fullPath;
            }
            return demandPath;
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal unsafe static void InternalCreateDirectory(String fullPath, String path
        )
        {
            int length = fullPath.Length;

            // We need to trim the trailing slash or the code will try to create 2 directories of the same name.
            if (length >= 2 && Path.IsDirectorySeparator(fullPath[length - 1]))
                length--;
            
            int lengthRoot = Path.GetRootLength(fullPath);

#if !PLATFORM_UNIX
            // For UNC paths that are only // or /// 
            if (length == 2 && Path.IsDirectorySeparator(fullPath[1]))
                throw new IOException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("IO.IO_CannotCreateDirectory"), path));
#endif // !PLATFORM_UNIX

            List<string> stackDir = new List<string>();

            // Attempt to figure out which directories don't exist, and only
            // create the ones we need.  Note that InternalExists may fail due
            // to Win32 ACL's preventing us from seeing a directory, and this
            // isn't threadsafe.

            bool somepathexists = false;

            if (length > lengthRoot) { // Special case root (fullpath = X:\\)
                int i = length-1;
                while (i >= lengthRoot) {
                    String dir = fullPath.Substring(0, i+1);
                        
                    if (!InternalExists(dir)) // Create only the ones missing
                        stackDir.Add(dir);
                    else
                        somepathexists = true;
                    
                    while (i > lengthRoot && fullPath[i] != Path.DirectorySeparatorChar && fullPath[i] != Path.AltDirectorySeparatorChar) i--;
                    i--;
                }
            }

            int count = stackDir.Count;

            if (stackDir.Count != 0)
            {
                String [] securityList = new String[stackDir.Count];
                stackDir.CopyTo(securityList, 0);
                for (int j = 0 ; j < securityList.Length; j++)
                    securityList[j] += "\\."; // leaf will never have a slash at the end

                // Security check for all directories not present only.
                new FileIOPermission(FileIOPermissionAccess.Write, securityList, false, false ).Demand();
            }

            // If we were passed a DirectorySecurity, convert it to a security
            // descriptor and set it in he call to CreateDirectory.
            Win32Native.SECURITY_ATTRIBUTES secAttrs = null;

            bool r = true;
            int firstError = 0;
            String errorString = path;
            // If all the security checks succeeded create all the directories
            while (stackDir.Count > 0) {
                String name = stackDir[stackDir.Count - 1];
                stackDir.RemoveAt(stackDir.Count - 1);
                if (name.Length > Path.MAX_DIRECTORY_PATH)
                    throw new PathTooLongException(Environment.GetResourceString("IO.PathTooLong"));
                r = Win32Native.CreateDirectory(name, secAttrs);
                if (!r && (firstError == 0)) {
                    int currentError = Marshal.GetLastWin32Error();
                    // While we tried to avoid creating directories that don't
                    // exist above, there are at least two cases that will 
                    // cause us to see ERROR_ALREADY_EXISTS here.  InternalExists 
                    // can fail because we didn't have permission to the 
                    // directory.  Secondly, another thread or process could
                    // create the directory between the time we check and the
                    // time we try using the directory.  Thirdly, it could
                    // fail because the target does exist, but is a file.
                    if (currentError != Win32Native.ERROR_ALREADY_EXISTS)
                        firstError = currentError;
                    else {
                        // If there's a file in this directory's place, throw.
                        if (File.InternalExists(name)) {
                            firstError = currentError;
                            // Give the user a nice error message, but don't leak path information.
                            try {
                                new FileIOPermission(FileIOPermissionAccess.PathDiscovery, GetDemandDir(name, true)).Demand();
                                errorString = name;
                            }
                            catch(SecurityException) {}
                        }
                    }
                }
            }

            // We need this check to mask OS differences
            // Handle CreateDirectory("X:\\foo") when X: doesn't exist. Similarly for n/w paths.
            if ((count == 0) && !somepathexists) {
                String root = InternalGetDirectoryRoot(fullPath);
                if (!InternalExists(root)) {
                    // Extract the root from the passed in path again for security.
                    __Error.WinIOError(Win32Native.ERROR_PATH_NOT_FOUND, InternalGetDirectoryRoot(path));
                }
                return;
            }

            // Only throw an exception if creating the exact directory we 
            // wanted failed to work correctly.
            if (!r && (firstError != 0)) {
                __Error.WinIOError(firstError, errorString);
            }
        }
      
       
        // Tests if the given path refers to an existing DirectoryInfo on disk.
        // 
        // Your application must have Read permission to the directory's
        // contents.
        //
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static bool Exists(String path) {
            try
            {    if (path==null)
                    return false;
                if (path.Length==0)
                    return false;

                // Get fully qualified file name ending in \* for security check
            
                String fullPath = Path.GetFullPathInternal(path);
                String demandPath = GetDemandDir(fullPath, true);

                new FileIOPermission(FileIOPermissionAccess.Read, new String[] { demandPath }, false, false ).Demand();


                return InternalExists(fullPath);
            }
            catch(ArgumentException) {}            
            catch(NotSupportedException) {}  // Security can throw this on ":"
            catch(SecurityException) {}
            catch(IOException) {}
            catch(UnauthorizedAccessException) 
            {
            }
            return false;
        }

        // Determine whether path describes an existing directory
        // on disk, avoiding security checks.
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal static bool InternalExists(String path) {
            Win32Native.WIN32_FILE_ATTRIBUTE_DATA data = new Win32Native.WIN32_FILE_ATTRIBUTE_DATA();
            int dataInitialised = File.FillAttributeInfo(path, ref data, false, true);
                
            return (dataInitialised == 0) && (data.fileAttributes != -1) 
                    && ((data.fileAttributes & Win32Native.FILE_ATTRIBUTE_DIRECTORY) != 0);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void SetCreationTime(String path,DateTime creationTime)
        {
            SetCreationTimeUtc(path, creationTime.ToUniversalTime());
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public unsafe static void SetCreationTimeUtc(String path,DateTime creationTimeUtc)
        {
            {
                using (SafeFileHandle handle = Directory.OpenHandle(path)) {
                    Win32Native.FILE_TIME fileTime = new Win32Native.FILE_TIME(creationTimeUtc.ToFileTimeUtc());
                    bool r = Win32Native.SetFileTime(handle, &fileTime, null, null);
                    if (!r)
                    {
                        int errorCode = Marshal.GetLastWin32Error();
                        __Error.WinIOError(errorCode, path);
                    }
                }
            }
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static DateTime GetCreationTime(String path)
        {
            return File.GetCreationTime(path);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static DateTime GetCreationTimeUtc(String path)
        {
            return File.GetCreationTimeUtc(path);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void SetLastWriteTime(String path,DateTime lastWriteTime)
        {
            SetLastWriteTimeUtc(path, lastWriteTime.ToUniversalTime());
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public unsafe static void SetLastWriteTimeUtc(String path,DateTime lastWriteTimeUtc)
        {
            {
                using (SafeFileHandle handle = Directory.OpenHandle(path)) {
                    Win32Native.FILE_TIME fileTime = new Win32Native.FILE_TIME(lastWriteTimeUtc.ToFileTimeUtc());
                    bool r = Win32Native.SetFileTime(handle,  null, null, &fileTime);
                    if (!r)
                    {
                        int errorCode = Marshal.GetLastWin32Error();
                        __Error.WinIOError(errorCode, path);
                    }
                }
            }
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static DateTime GetLastWriteTime(String path)
        {
            return File.GetLastWriteTime(path);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static DateTime GetLastWriteTimeUtc(String path)
        {
            return File.GetLastWriteTimeUtc(path);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void SetLastAccessTime(String path,DateTime lastAccessTime)
        {
            SetLastAccessTimeUtc(path, lastAccessTime.ToUniversalTime());
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public unsafe static void SetLastAccessTimeUtc(String path,DateTime lastAccessTimeUtc)
        {
            {
                using (SafeFileHandle handle = Directory.OpenHandle(path)) {
                    Win32Native.FILE_TIME fileTime = new Win32Native.FILE_TIME(lastAccessTimeUtc.ToFileTimeUtc());
                    bool r = Win32Native.SetFileTime(handle,  null, &fileTime, null);
                    if (!r)
                    {
                        int errorCode = Marshal.GetLastWin32Error();
                        __Error.WinIOError(errorCode, path);
                    }
                }
            }
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static DateTime GetLastAccessTime(String path)
        {
            return File.GetLastAccessTime(path);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static DateTime GetLastAccessTimeUtc(String path)
        {
            return File.GetLastAccessTimeUtc(path);
        }       
 

        // Returns an array of filenames in the DirectoryInfo specified by path
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static String[] GetFiles(String path)
        {
            return GetFiles(path,"*");
        }

        // Returns an array of Files in the current DirectoryInfo matching the 
        // given search pattern (ie, "*.txt").
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static String[] GetFiles(String path,String searchPattern)
        {
            return GetFiles(path, searchPattern, SearchOption.TopDirectoryOnly);
        }

        // Returns an array of Files in the current DirectoryInfo matching the 
        // given search pattern (ie, "*.txt") and search option
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static String[] GetFiles(String path,String searchPattern, SearchOption searchOption)
        {
            if (path==null)
                throw new ArgumentNullException("path");

            if (searchPattern==null)
                throw new ArgumentNullException("searchPattern");

            return InternalGetFileDirectoryNames(path, path, searchPattern, true, false, searchOption);
        }

        // Returns an array of Directories in the current directory.
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static String[] GetDirectories(String path)
        {
            return GetDirectories(path,"*");
        }

        // Returns an array of Directories in the current DirectoryInfo matching the 
        // given search criteria (ie, "*.txt").
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static String[] GetDirectories(String path,String searchPattern)
        {
            return GetDirectories(path, searchPattern, SearchOption.TopDirectoryOnly);
        }

        // Returns an array of Directories in the current DirectoryInfo matching the 
        // given search criteria (ie, "*.txt").
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static String[] GetDirectories(String path,String searchPattern, SearchOption searchOption)
        {
            if (path==null)
                throw new ArgumentNullException("path");

            if (searchPattern==null)
                throw new ArgumentNullException("searchPattern");

            return InternalGetFileDirectoryNames(path, path, searchPattern, false, true, searchOption);
        }
            
        // Returns an array of strongly typed FileSystemInfo entries in the path
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static String[] GetFileSystemEntries(String path)
        {
            return GetFileSystemEntries(path,"*");
        }

        // Returns an array of strongly typed FileSystemInfo entries in the path with the
        // given search criteria (ie, "*.txt"). We disallow .. as a part of the search criteria
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static String[] GetFileSystemEntries(String path,String searchPattern)
        {
            return GetFileSystemEntries(path, searchPattern, SearchOption.TopDirectoryOnly);
        }

        // Returns an array of strongly typed FileSystemInfo entries in the path with the
        // given search criteria (ie, "*.txt"). We disallow .. as a part of the search criteria
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        private static String[] GetFileSystemEntries(String path,String searchPattern, SearchOption searchOption)
        {
            if (path==null)
                throw new ArgumentNullException("path");

            if (searchPattern==null)
                throw new ArgumentNullException("searchPattern");

            return InternalGetFileDirectoryNames(path, path, searchPattern, true, true, searchOption);
        }

        // Private class that holds search data that is passed around 
        // in the heap based stack recursion
        private sealed class SearchData
        {
            public SearchData() {
            }

            public SearchData(string fullPath, string userPath, SearchOption searchOption) {
                this.fullPath = fullPath;
                this.userPath = userPath;
                this.searchOption = searchOption;
            }
            public string fullPath;     // Fully qualified search path excluding the search criteria in the end (ex, c:\temp\bar\foo)
            public string userPath;     // User specified path (ex, bar\foo)
            public SearchOption searchOption;
        }

        // Returns fully qualified user path of dirs/files that matches the search parameters. 
        // For recursive search this method will search through all the sub dirs  and execute 
        // the given search criteria against every dir.
        // For all the dirs/files returned, it will then demand path discovery permission for 
        // their parent folders (it will avoid duplicate permission checks)
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal static String[] InternalGetFileDirectoryNames(String path, String userPathOriginal, String searchPattern, bool includeFiles, bool includeDirs, SearchOption searchOption)
        {
            int hr = 0;

            if ((searchOption != SearchOption.TopDirectoryOnly) && (searchOption != SearchOption.AllDirectories))
                throw new ArgumentOutOfRangeException("searchOption", Environment.GetResourceString("ArgumentOutOfRange_Enum"));

            searchPattern = searchPattern.TrimEnd();
            if (searchPattern.Length == 0)
                return new String[0];
            
            Path.CheckSearchPattern(searchPattern);

            // Build various paths and query strings

            // Must fully qualify the path for the security check
            String fullPath = Path.GetFullPathInternal(path);

            // Any illegal chars such as *, ? will be caught by FileIOPermission.HasIllegalCharacters
            String[] demandPaths = new String[] {GetDemandDir(fullPath, true)};
            new FileIOPermission(FileIOPermissionAccess.PathDiscovery, demandPaths, false, false).Demand();

            String userPath = userPathOriginal;
            String tempPath = Path.GetDirectoryName(searchPattern);

            if (tempPath != null && tempPath.Length != 0) { // For filters like foo\*.cs we need to verify if the directory foo is not denied access.
                // Do a demand on the combined path so that we can fail early in case of deny
                demandPaths = new String[] {GetDemandDir(Path.InternalCombine(fullPath, tempPath), true)};
                new FileIOPermission(FileIOPermissionAccess.PathDiscovery, demandPaths, false, false).Demand();

                userPath = Path.Combine(userPath, tempPath); // Need to add the searchPath to return correct path and for right security checks
            }
            
            String tempStr = Path.InternalCombine(fullPath, searchPattern);

            // If path ends in a trailing slash (\), append a * or we'll 
            // get a "Cannot find the file specified" exception
            char lastChar = tempStr[tempStr.Length-1];
            if (lastChar == Path.DirectorySeparatorChar || lastChar == Path.AltDirectorySeparatorChar || lastChar == Path.VolumeSeparatorChar)
                tempStr = tempStr + '*';

            fullPath = Path.GetDirectoryName(tempStr);
            BCLDebug.Assert((fullPath != null),"fullpath can't be null!");
            
            String searchCriteria;
            bool trailingSlash = false;
            bool trailingSlashUserPath = false;

            lastChar = fullPath[fullPath.Length-1];
            trailingSlash = (lastChar == Path.DirectorySeparatorChar) || (lastChar == Path.AltDirectorySeparatorChar);

            if (trailingSlash) {
                // Can happen if the path is C:\temp, in which case GetDirectoryName would return C:\
                searchCriteria = tempStr.Substring(fullPath.Length);
            }
            else
                searchCriteria = tempStr.Substring(fullPath.Length + 1);

            Win32Native.WIN32_FIND_DATA data = new Win32Native.WIN32_FIND_DATA();
            SafeFindHandle hnd = null;

            // Heap based search stack
            SearchData searchData = new SearchData(fullPath, userPath, searchOption);
            List<SearchData> searchStack = new List<SearchData>();
            searchStack.Add(searchData);

            List<string> demandPathList = new List<string>();

            int numEntries = 0;  // Number of directory entities we see.
            int listSize = 0;
            String[] list = new String[10];
            String searchPath; 

            int oldMode = Win32Native.SetErrorMode(Win32Native.SEM_FAILCRITICALERRORS);
            try {
                // Traverse directory structure. We need to get '*'
                while(searchStack.Count > 0) {
                    searchData = searchStack[searchStack.Count - 1];
                    searchStack.RemoveAt(searchStack.Count - 1);
                    BCLDebug.Assert((searchData.fullPath != null),"fullpath can't be null!");

                    lastChar = searchData.fullPath[searchData.fullPath.Length -1]; 
                    trailingSlash = (lastChar == Path.DirectorySeparatorChar) || (lastChar == Path.AltDirectorySeparatorChar);
                    
                    if (searchData.userPath.Length > 0) {
                        lastChar = searchData.userPath[searchData.userPath.Length -1]; 
                        trailingSlashUserPath = (lastChar == Path.DirectorySeparatorChar) || (lastChar == Path.AltDirectorySeparatorChar);
                    }

                    // Traverse the subdirs if specified
                    if (searchData.searchOption != SearchOption.TopDirectoryOnly) {
                        try {
                            if (trailingSlash) 
                                searchPath = searchData.fullPath + "*";
                            else
                                searchPath = searchData.fullPath + Path.DirectorySeparatorChar + "*";

                            // Get all files and dirs
                            hnd = Win32Native.FindFirstFile(searchPath, data);

                            if (hnd.IsInvalid) {
                                hr = Marshal.GetLastWin32Error();

                                // This could happen if the dir doesn't contain any files.
                                // Continue with the recursive search though, eventually
                                // searchStack will become empty
                                if (hr==Win32Native.ERROR_FILE_NOT_FOUND)
                                    continue;
                                __Error.WinIOError(hr, searchData.fullPath);
                            }

                            // Add subdirs to searchStack. Exempt ReparsePoints as appropriate
                            do {
                                if ((0 != (data.dwFileAttributes & Win32Native.FILE_ATTRIBUTE_DIRECTORY))
                                    && !data.cFileName.Equals(".") && !data.cFileName.Equals("..")) {

                                    // Setup search data for the sub directory and push it into the stack
                                    SearchData searchDataSubDir = new SearchData();

                                    // FullPath
                                    StringBuilder strBldr = new StringBuilder(searchData.fullPath);
                                    if (!trailingSlash) 
                                        strBldr.Append(Path.DirectorySeparatorChar);
                                    strBldr.Append(data.cFileName);
                                    searchDataSubDir.fullPath = strBldr.ToString();

                                    // UserPath
                                    strBldr.Length = 0;
                                    strBldr.Append(searchData.userPath);
                                    if (!trailingSlashUserPath)
                                        strBldr.Append(Path.DirectorySeparatorChar);
                                    strBldr.Append(data.cFileName);
                                    searchDataSubDir.userPath = strBldr.ToString();

                                    // SearchOption
                                    searchDataSubDir.searchOption = searchData.searchOption;

                                    searchStack.Add(searchDataSubDir);
                                }
                            } while (Win32Native.FindNextFile(hnd, data));
                            // We don't care about any error here
                        }
                        finally {
                            if (hnd != null)
                                hnd.Dispose();
                        }
                    }

                    // Execute searchCriteria against the current directory
                    try{
                        if (trailingSlash) 
                            searchPath = searchData.fullPath + searchCriteria;
                        else
                            searchPath = searchData.fullPath + Path.DirectorySeparatorChar + searchCriteria;

                        // Open a Find handle
                        hnd = Win32Native.FindFirstFile(searchPath, data);
                        if (hnd.IsInvalid) {
                            hr = Marshal.GetLastWin32Error();
                            if (hr == Win32Native.ERROR_FILE_NOT_FOUND)
                                continue;
                            __Error.WinIOError(hr, searchData.fullPath);
                        }

                        numEntries = 0;
                        
                        // Keep asking for more matching files/dirs, add it to the list 
                        do {
                            bool includeThis = false;  // Should this file/directory be included in the output?
                            
                            if (includeFiles)
                                includeThis = (0 == (data.dwFileAttributes & Win32Native.FILE_ATTRIBUTE_DIRECTORY));
                            
                            if (includeDirs) {
                                // Don't add "." nor ".."
                                if ((0 != (data.dwFileAttributes & Win32Native.FILE_ATTRIBUTE_DIRECTORY)) 
                                    && !data.cFileName.Equals(".") && !data.cFileName.Equals("..")) {
                                    
                                    BCLDebug.Assert(!includeThis, data.cFileName + ": current item can't be both file and dir!");
                                    includeThis = true;
                                }
                            }

                            if (includeThis) {
                                numEntries++;
                                if (listSize==list.Length) {
                                    String[] newList = new String[list.Length*2];
                                    Array.Copy(list, 0, newList, 0, listSize);
                                    list = newList;
                                }
                                list[listSize++] = Path.InternalCombine(searchData.userPath, data.cFileName);
                            }
                        } while (Win32Native.FindNextFile(hnd, data));

                        // Make sure we quit with a sensible error.
                        hr = Marshal.GetLastWin32Error();

                        // Demand pathdiscovery for all the parent dirs that are returned
                        // I.e, if C:\temp\foo\bar is returned, demand C:\temp\foo
                        if (numEntries > 0)  {
                            demandPathList.Add(GetDemandDir(searchData.fullPath, true));
                        }
                    }
                    finally {
                        if (hnd != null)
                            hnd.Dispose();
                    }
                } // End while
            }
            finally {
                Win32Native.SetErrorMode(oldMode);
            }

            // ERROR_FILE_NOT_FOUND is valid here because if the top level
            // dir doen't contain any subdirs and matching files then 
            // we will get here with this errorcode from the searchStack walk
            if ((hr != 0) && (hr != Win32Native.ERROR_NO_MORE_FILES) 
                && (hr != Win32Native.ERROR_FILE_NOT_FOUND)) {
                __Error.WinIOError(hr, searchData.fullPath);
            }
            
            // Demand pathdiscovery for all the parent dirs that are returned
            // I.e, if C:\temp\foo\bar is returned, demand C:\temp\foo
            if (demandPathList.Count > 0) {
                demandPaths = new String[demandPathList.Count];
                demandPathList.CopyTo(demandPaths, 0);

                // No need to check for dupls as the demandPathList entries should be unique
                new FileIOPermission(FileIOPermissionAccess.PathDiscovery, demandPaths, false, false ).Demand();
            }
            
            // Check for a string such as "C:\tmp", in which case we return
            // just the DirectoryInfo name.  FindNextFile fails first time, and
            // data still contains a directory.
/*
            if (includeDirs && numEntries==1 && (0!=(data.dwFileAttributes & Win32Native.FILE_ATTRIBUTE_DIRECTORY))) {
                String[] sa = new String[1];
                sa[0] = Path.InternalCombine(searchData.userPath, data.cFileName);
                return sa;
            }
*/
            // Return list of files/directories as an array of strings
            if (listSize == list.Length)
                return list;
            String[] items = new String[listSize];
            Array.Copy(list, 0, items, 0, listSize);
            return items;
        }
        
        // Retrieves the names of the logical drives on this machine in the 
        // form "C:\". 
        // 
        // Your application must have System Info permission.
        // 
#if !PLATFORM_UNIX
        public static String[] GetLogicalDrives()
        {
            new SecurityPermission(SecurityPermissionFlag.UnmanagedCode).Demand();
                                 
            int drives = Win32Native.GetLogicalDrives();
            if (drives==0)
                __Error.WinIOError();
            uint d = (uint)drives;
            int count = 0;
            while (d != 0) {
                if (((int)d & 1) != 0) count++;
                d >>= 1;
            }
            String[] result = new String[count];
            char[] root = new char[] {'A', ':', '\\'};
            d = (uint)drives;
            count = 0;
            while (d != 0) {
                if (((int)d & 1) != 0) {
                    result[count++] = new String(root);
                }
                d >>= 1;
                root[0]++;
            }
            return result;
        }
#else
        public static String[] GetLogicalDrives()
        {   
            // On Unix systems, GetLogicalDrives 
            // should return an empty string array
            return new String[0];
        } 
#endif // !PLATFORM_UNIX
    
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static String GetDirectoryRoot(String path) {
            if (path==null)
                throw new ArgumentNullException("path");
            
            String fullPath = Path.GetFullPathInternal(path);
            String root = fullPath.Substring(0, Path.GetRootLength(fullPath));
            String demandPath = GetDemandDir(root, true);

            new FileIOPermission(FileIOPermissionAccess.PathDiscovery, new String[] { demandPath }, false, false ).Demand();
                     
            return root;
        }

        internal static String InternalGetDirectoryRoot(String path) {
              if (path == null) return null;
            return path.Substring(0, Path.GetRootLength(path));
        }

         /*===============================CurrentDirectory===============================
        **Action:  Provides a getter and setter for the current directory.  The original
        **         current DirectoryInfo is the one from which the process was started.  
        **Returns: The current DirectoryInfo (from the getter).  Void from the setter.
        **Arguments: The current DirectoryInfo to which to switch to the setter.
        **Exceptions: 
        ==============================================================================*/
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static String GetCurrentDirectory()
        {
            StringBuilder sb = new StringBuilder(Path.MAX_PATH + 1);
            if (Win32Native.GetCurrentDirectory(sb.Capacity, sb) == 0)
                __Error.WinIOError();
            String currentDirectory = sb.ToString();
            // Note that if we have somehow put our command prompt into short
            // file name mode (ie, by running edlin or a DOS grep, etc), then
            // this will return a short file name.
            if (currentDirectory.IndexOf('~') >= 0) {
                int r = Win32Native.GetLongPathName(currentDirectory, sb, sb.Capacity);
                if (r == 0 || r >= Path.MAX_PATH) {                    
                    int errorCode = Marshal.GetLastWin32Error();
                    if (r >= Path.MAX_PATH)
                        errorCode = Win32Native.ERROR_FILENAME_EXCED_RANGE;
                    if (errorCode != Win32Native.ERROR_FILE_NOT_FOUND &&
                        errorCode != Win32Native.ERROR_PATH_NOT_FOUND &&
                        errorCode != Win32Native.ERROR_INVALID_FUNCTION &&  // by design - enough said.
                        errorCode != Win32Native.ERROR_ACCESS_DENIED)
                        __Error.WinIOError(errorCode, String.Empty);
                }
                currentDirectory = sb.ToString();
            }

            String demandPath = GetDemandDir(currentDirectory, true);
            new FileIOPermission( FileIOPermissionAccess.PathDiscovery, new String[] { demandPath }, false, false ).Demand();
            return currentDirectory;
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void SetCurrentDirectory(String path)
        {        
            if (path==null)
                throw new ArgumentNullException("value");
            if (path.Length==0)
                throw new ArgumentException(Environment.GetResourceString("Argument_PathEmpty"));
            if (path.Length >= Path.MAX_PATH)
                throw new PathTooLongException(Environment.GetResourceString("IO.PathTooLong"));
                
            // This will have some large effects on the rest of the runtime
            // and other appdomains in this process.  Demand unmanaged code.
            new SecurityPermission(SecurityPermissionFlag.UnmanagedCode).Demand();

            String fulldestDirName = Path.GetFullPathInternal(path);
            if (Environment.IsWin9X() && !InternalExists(Path.GetPathRoot(fulldestDirName))) // For Win9x
                throw new DirectoryNotFoundException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("IO.PathNotFound_Path"), path));
            
            if (!Win32Native.SetCurrentDirectory(fulldestDirName)) {
                // If path doesn't exist, this sets last error to 2 (File 
                // not Found).  This may potentially have worked correctly
                // on Win9x, maybe.
                int errorCode = Marshal.GetLastWin32Error();
                if (errorCode == Win32Native.ERROR_FILE_NOT_FOUND)
                    errorCode = Win32Native.ERROR_PATH_NOT_FOUND;
                __Error.WinIOError(errorCode, fulldestDirName);
            }
        }
       
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void Move(String sourceDirName,String destDirName) {
            if (sourceDirName==null)
                throw new ArgumentNullException("sourceDirName");
            if (sourceDirName.Length==0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyFileName"), "sourceDirName");
            
            if (destDirName==null)
                throw new ArgumentNullException("destDirName");
            if (destDirName.Length==0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyFileName"), "destDirName");

            String fullsourceDirName = Path.GetFullPathInternal(sourceDirName);
            String sourcePath = GetDemandDir(fullsourceDirName, false);
            
            if (sourcePath.Length >= Path.MAX_DIRECTORY_PATH+1)
                throw new PathTooLongException(Environment.GetResourceString("IO.PathTooLong"));

            String fulldestDirName = Path.GetFullPathInternal(destDirName);
            String destPath = GetDemandDir(fulldestDirName, false);

            if (destPath.Length >= Path.MAX_DIRECTORY_PATH+1)
                throw new PathTooLongException(Environment.GetResourceString("IO.PathTooLong"));
            
            new FileIOPermission(FileIOPermissionAccess.Write | FileIOPermissionAccess.Read, new String[] { sourcePath }, false, false ).Demand();
            new FileIOPermission(FileIOPermissionAccess.Write, new String[] { destPath }, false, false ).Demand();
                
            if (CultureInfo.InvariantCulture.CompareInfo.Compare(sourcePath, destPath, CompareOptions.IgnoreCase) == 0)
                throw new IOException(Environment.GetResourceString("IO.IO_SourceDestMustBeDifferent"));

            String sourceRoot = Path.GetPathRoot(sourcePath);
            String destinationRoot = Path.GetPathRoot(destPath);
            if (CultureInfo.InvariantCulture.CompareInfo.Compare(sourceRoot, destinationRoot, CompareOptions.IgnoreCase) != 0)
                throw new IOException(Environment.GetResourceString("IO.IO_SourceDestMustHaveSameRoot"));
            
            if (Environment.IsWin9X() && !Directory.InternalExists(Path.GetPathRoot(fulldestDirName))) // For Win9x
                throw new DirectoryNotFoundException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("IO.PathNotFound_Path"), destDirName));
    
            if (!Win32Native.MoveFile(sourceDirName, destDirName))
            {
                int hr = Marshal.GetLastWin32Error();
                if (hr == Win32Native.ERROR_FILE_NOT_FOUND) // Source dir not found
                {
                    hr = Win32Native.ERROR_PATH_NOT_FOUND;
                    __Error.WinIOError(hr, fullsourceDirName);
                }
                if (hr == Win32Native.ERROR_ACCESS_DENIED) // WinNT throws IOException. This check is for Win9x. We can't change it for backcomp.
                    throw new IOException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("UnauthorizedAccess_IODenied_Path"), sourceDirName), Win32Native.MakeHRFromErrorCode(hr));
                __Error.WinIOError(hr, String.Empty);
            }
        }
      
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void Delete(String path)
        {
            String fullPath = Path.GetFullPathInternal(path);
            Delete(fullPath, path, false);
        }
    
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void Delete(String path, bool recursive)
        {
            String fullPath = Path.GetFullPathInternal(path);
            Delete(fullPath, path, recursive);
        }

        // Called from DirectoryInfo as well.  FullPath is fully qualified,
        // while the user path is used for feedback in exceptions.
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal static void Delete(String fullPath, String userPath, bool recursive)
        {
            String demandPath;
            
            // If not recursive, do permission check only on this directory
            // else check for the whole directory structure rooted below 
            demandPath = GetDemandDir(fullPath, !recursive);
            
            // Make sure we have write permission to this directory
            new FileIOPermission(FileIOPermissionAccess.Write, new String[] { demandPath }, false, false ).Demand();

            // Do not recursively delete through reparse points.  Perhaps in a 
            // future version we will add a new flag to control this behavior, 
            // but for now we're much safer if we err on the conservative side.
            // This applies to symbolic links and mount points.
            Win32Native.WIN32_FILE_ATTRIBUTE_DATA data = new Win32Native.WIN32_FILE_ATTRIBUTE_DATA();
            int dataInitialised = File.FillAttributeInfo(fullPath, ref data, false, true);
            if (dataInitialised != 0) {
                // Ensure we throw a DirectoryNotFoundException.
                if (dataInitialised == Win32Native.ERROR_FILE_NOT_FOUND)
                    dataInitialised = Win32Native.ERROR_PATH_NOT_FOUND;
                __Error.WinIOError(dataInitialised, fullPath);
            }

            if (((FileAttributes)data.fileAttributes & FileAttributes.ReparsePoint) != 0)
                recursive = false;

            DeleteHelper(fullPath, userPath, recursive);
        }

        // Note that fullPath is fully qualified, while userPath may be 
        // relative.  Use userPath for all exception messages to avoid leaking
        // fully qualified path information.
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        private static void DeleteHelper(String fullPath, String userPath, bool recursive)
        {
            bool r;
            int hr;
            Exception ex = null;

            // Do not recursively delete through reparse points.  Perhaps in a 
            // future version we will add a new flag to control this behavior, 
            // but for now we're much safer if we err on the conservative side.
            // This applies to symbolic links and mount points.
            // Note the logic to check whether fullPath is a reparse point is
            // in Delete(String, String, bool), and will set "recursive" to false.
            // Note that Win32's DeleteFile and RemoveDirectory will just delete
            // the reparse point itself.

            if (recursive) {
                Win32Native.WIN32_FIND_DATA data = new Win32Native.WIN32_FIND_DATA();
                
                // Open a Find handle
                using (SafeFindHandle hnd = Win32Native.FindFirstFile(fullPath+Path.DirectorySeparatorChar+"*", data)) {
                    if (hnd.IsInvalid) {
                        hr = Marshal.GetLastWin32Error();
                        __Error.WinIOError(hr, fullPath);
                    }
        
                    do {
                        bool isDir = (0!=(data.dwFileAttributes & Win32Native.FILE_ATTRIBUTE_DIRECTORY));
                        if (isDir) {
                            // Skip ".", "..".
                            if (data.cFileName.Equals(".") || data.cFileName.Equals(".."))
                                continue;

                            // Recurse for all directories, unless they are 
                            // reparse points.  Do not follow mount points nor
                            // symbolic links, but do delete the reparse point 
                            // itself.
                            bool shouldRecurse = (0 == (data.dwFileAttributes & (int) FileAttributes.ReparsePoint));
                            if (shouldRecurse) {
                                String newFullPath = Path.InternalCombine(fullPath, data.cFileName);
                                String newUserPath = Path.InternalCombine(userPath, data.cFileName);                        
                                try {
                                    DeleteHelper(newFullPath, newUserPath, recursive);
                                }
                                catch(Exception e) {
                                    if (ex == null) {
                                        ex = e;
                                    }
                                }
                            }
                            else {
                                // Check to see if this is a mount point, and
                                // unmount it.
                                if (data.dwReserved0 == Win32Native.IO_REPARSE_TAG_MOUNT_POINT) {
                                    // Use full path plus a trailing '\'
                                    String mountPoint = Path.InternalCombine(fullPath, data.cFileName + Path.DirectorySeparatorChar);
                                    r = Win32Native.DeleteVolumeMountPoint(mountPoint);
                                    if (!r) {
                                        hr = Marshal.GetLastWin32Error();
                                        try {
                                            __Error.WinIOError(hr, data.cFileName);
                                        }
                                        catch(Exception e) {
                                            if (ex == null) {
                                                ex = e;
                                            }
                                        }
                                    }
                                }

                                // RemoveDirectory on a symbolic link will
                                // remove the link itself.
                                String reparsePoint = Path.InternalCombine(fullPath, data.cFileName);
                                r = Win32Native.RemoveDirectory(reparsePoint);
                                if (!r) {
                                    hr = Marshal.GetLastWin32Error();
                                    try {
                                        __Error.WinIOError(hr, data.cFileName);
                                    }
                                    catch(Exception e) {
                                        if (ex == null) {
                                            ex = e;
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            String fileName = Path.InternalCombine(fullPath, data.cFileName);
                            r = Win32Native.DeleteFile(fileName);
                            if (!r) {
                                hr = Marshal.GetLastWin32Error();
                                try {
                                    __Error.WinIOError(hr, data.cFileName);
                                }
                                catch(Exception e) {
                                    if (ex == null) {
                                        ex = e;
                                    }
                                }
                            }
                        }
                    } while (Win32Native.FindNextFile(hnd, data));
                    // Make sure we quit with a sensible error.
                    hr = Marshal.GetLastWin32Error();
                }

                if (ex != null) 
                    throw ex;
                if (hr!=0 && hr!=Win32Native.ERROR_NO_MORE_FILES) 
                    __Error.WinIOError(hr, userPath);
            }

            r = Win32Native.RemoveDirectory(fullPath);
            
            if (!r) {
                hr = Marshal.GetLastWin32Error();
                if (hr == Win32Native.ERROR_FILE_NOT_FOUND) // A dubious error code.
                    hr = Win32Native.ERROR_PATH_NOT_FOUND;
                if (hr == Win32Native.ERROR_ACCESS_DENIED) 
                    throw new IOException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("UnauthorizedAccess_IODenied_Path"), userPath));
                __Error.WinIOError(hr, fullPath);
            }
        }


        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        private static SafeFileHandle OpenHandle(String path)
        {
            String fullPath = Path.GetFullPathInternal(path);
            String root = Path.GetPathRoot(fullPath);
            if (root == fullPath && root[1] == Path.VolumeSeparatorChar)
                throw new ArgumentException(Environment.GetResourceString("Arg_PathIsVolume"));

            new FileIOPermission(FileIOPermissionAccess.Write, new String[] { GetDemandDir(fullPath, true) }, false, false ).Demand();

            SafeFileHandle handle = Win32Native.SafeCreateFile (
                fullPath,
                GENERIC_WRITE,
                (FileShare) (FILE_SHARE_WRITE|FILE_SHARE_DELETE),
                null,
                FileMode.Open,
                FILE_FLAG_BACKUP_SEMANTICS,
                Win32Native.NULL
            );

            if (handle.IsInvalid) {
                int hr = Marshal.GetLastWin32Error();
                __Error.WinIOError(hr, fullPath);
            }
            return handle;
        }

        private const int FILE_ATTRIBUTE_DIRECTORY = 0x00000010;    
        private const int GENERIC_WRITE = unchecked((int)0x40000000);
        private const int FILE_SHARE_WRITE = 0x00000002;
        private const int FILE_SHARE_DELETE = 0x00000004;
        private const int OPEN_EXISTING = 0x00000003;
        private const int FILE_FLAG_BACKUP_SEMANTICS = 0x02000000;
    }       
}

