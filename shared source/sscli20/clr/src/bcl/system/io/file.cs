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
** Class:  File
**
**
** Purpose: A collection of methods for manipulating Files.
**
**        April 09,2000 (some design refactorization)
**
===========================================================*/

using System;
using System.Security.Permissions;
using PermissionSet = System.Security.PermissionSet;
using Win32Native = Microsoft.Win32.Win32Native;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;
using Microsoft.Win32.SafeHandles;
using System.Collections;
using System.Globalization;
using System.Runtime.Versioning;
    
namespace System.IO {    
    // Class for creating FileStream objects, and some basic file management
    // routines such as Delete, etc.
    [ComVisible(true)]
    public static class File
    {
        private const int GetFileExInfoStandard = 0;

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static StreamReader OpenText(String path)
        {
            if (path == null)
                throw new ArgumentNullException("path");
            return new StreamReader(path);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static StreamWriter CreateText(String path)
        {
            if (path == null)
                throw new ArgumentNullException("path");
            return new StreamWriter(path,false);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static StreamWriter AppendText(String path)
        {
            if (path == null)
                throw new ArgumentNullException("path");
            return new StreamWriter(path,true);
        }


        // Copies an existing file to a new file. An exception is raised if the
        // destination file already exists. Use the 
        // Copy(String, String, boolean) method to allow 
        // overwriting an existing file.
        //
        // The caller must have certain FileIOPermissions.  The caller must have
        // Read permission to sourceFileName and Create
        // and Write permissions to destFileName.
        // 
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void Copy(String sourceFileName, String destFileName) {
            Copy(sourceFileName, destFileName, false);
        }
    
        // Copies an existing file to a new file. If overwrite is 
        // false, then an IOException is thrown if the destination file 
        // already exists.  If overwrite is true, the file is 
        // overwritten.
        //
        // The caller must have certain FileIOPermissions.  The caller must have
        // Read permission to sourceFileName 
        // and Write permissions to destFileName.
        // 
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void Copy(String sourceFileName, String destFileName, bool overwrite) {
            InternalCopy(sourceFileName, destFileName,overwrite);
        }

        /// <devdoc>
        ///    Note: This returns the fully qualified name of the destination file.
        /// </devdoc>
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal static String InternalCopy(String sourceFileName, String destFileName, bool overwrite) {
            if (sourceFileName==null || destFileName==null)
                throw new ArgumentNullException((sourceFileName==null ? "sourceFileName" : "destFileName"), Environment.GetResourceString("ArgumentNull_FileName"));
            if (sourceFileName.Length==0 || destFileName.Length==0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyFileName"), (sourceFileName.Length==0 ? "sourceFileName" : "destFileName"));
            
            String fullSourceFileName = Path.GetFullPathInternal(sourceFileName);
            new FileIOPermission(FileIOPermissionAccess.Read, new String[] { fullSourceFileName }, false, false ).Demand();
            String fullDestFileName = Path.GetFullPathInternal(destFileName);
            new FileIOPermission(FileIOPermissionAccess.Write, new String[] { fullDestFileName }, false, false ).Demand();
            
            bool r = Win32Native.CopyFile(fullSourceFileName, fullDestFileName, !overwrite);
            if (!r) {
                // Save Win32 error because subsequent checks will overwrite this HRESULT.
                int errorCode = Marshal.GetLastWin32Error();
                String fileName = destFileName;

                if (errorCode != Win32Native.ERROR_FILE_EXISTS) {
                    // For a number of error codes (sharing violation, path 
                    // not found, etc) we don't know if the problem was with
                    // the source or dest file.  Try reading the source file.
                    using(SafeFileHandle handle = Win32Native.UnsafeCreateFile(fullSourceFileName, FileStream.GENERIC_READ, FileShare.Read, null, FileMode.Open, 0, IntPtr.Zero)) {
                        if (handle.IsInvalid)
                            fileName = sourceFileName;
                    }

                    if (errorCode == Win32Native.ERROR_ACCESS_DENIED) {
                        if (Directory.InternalExists(fullDestFileName))
                            throw new IOException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("Arg_FileIsDirectory_Name"), destFileName), Win32Native.ERROR_ACCESS_DENIED, fullDestFileName);
                    }
                }

                __Error.WinIOError(errorCode, fileName);
            }
                
            return fullDestFileName;
        }


        // Creates a file in a particular path.  If the file exists, it is replaced.
        // The file is opened with ReadWrite accessand cannot be opened by another 
        // application until it has been closed.  An IOException is thrown if the 
        // directory specified doesn't exist.
        //
        // Your application must have Create, Read, and Write permissions to
        // the file.
        // 
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static FileStream Create(String path) {
            return Create(path, FileStream.DefaultBufferSize, FileOptions.None);
        }
        
        // Creates a file in a particular path.  If the file exists, it is replaced.
        // The file is opened with ReadWrite access and cannot be opened by another 
        // application until it has been closed.  An IOException is thrown if the 
        // directory specified doesn't exist.
        //
        // Your application must have Create, Read, and Write permissions to
        // the file.
        // 
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static FileStream Create(String path, int bufferSize) {
            return Create(path, bufferSize, FileOptions.None);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static FileStream Create(String path, int bufferSize, FileOptions options) {
            return new FileStream(path, FileMode.Create, FileAccess.ReadWrite,
                                  FileShare.None, bufferSize, options);
        }


        // Deletes a file. The file specified by the designated path is deleted.
        // If the file does not exist, Delete succeeds without throwing
        // an exception.
        // 
        // On NT, Delete will fail for a file that is open for normal I/O
        // or a file that is memory mapped.  On Win95, the file will be 
        // deleted irregardless of whether the file is being used.
        // 
        // Your application must have Delete permission to the target file.
        // 
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void Delete(String path) {
            if (path==null)
                throw new ArgumentNullException("path");
            
            String fullPath = Path.GetFullPathInternal(path);

            // For security check, path should be resolved to an absolute path.
            new FileIOPermission(FileIOPermissionAccess.Write, new String[] { fullPath }, false, false ).Demand();

            if (Environment.IsWin9X() && Directory.InternalExists(fullPath)) // Win9x fails silently for directories.
                throw new UnauthorizedAccessException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("UnauthorizedAccess_IODenied_Path"), path));
    
            bool r = Win32Native.DeleteFile(fullPath);
            if (!r) {
                int hr = Marshal.GetLastWin32Error();
                if (hr==Win32Native.ERROR_FILE_NOT_FOUND)
                    return;
                else
                    __Error.WinIOError(hr, fullPath);
            }
        }


        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void Decrypt(String path)
        {
            if (path == null)
                throw new ArgumentNullException("path");

            throw new PlatformNotSupportedException(Environment.GetResourceString("PlatformNotSupported_RequiresNT"));
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void Encrypt(String path)
        {
            if (path == null)
                throw new ArgumentNullException("path");

            throw new PlatformNotSupportedException(Environment.GetResourceString("PlatformNotSupported_RequiresNT"));
        }

        // Tests if a file exists. The result is true if the file
        // given by the specified path exists; otherwise, the result is
        // false.  Note that if path describes a directory,
        // Exists will return true.
        //
        // Your application must have Read permission for the target directory.
        // 
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static bool Exists(String path) {
            try
            {
                if (path==null)
                    return false;
                if (path.Length==0)
                    return false;
            
                path = Path.GetFullPathInternal(path);
                    
                new FileIOPermission(FileIOPermissionAccess.Read, new String[] { path }, false, false ).Demand();
            
                return InternalExists(path);
            }
            catch(ArgumentException) {} 
            catch(NotSupportedException) {} // Security can throw this on ":"
            catch(SecurityException) {}
            catch(IOException) {}
            catch(UnauthorizedAccessException) {}

            return false;
        }

        internal static bool InternalExists(String path) {
            Win32Native.WIN32_FILE_ATTRIBUTE_DATA data = new Win32Native.WIN32_FILE_ATTRIBUTE_DATA();
            int dataInitialised = FillAttributeInfo(path, ref data, false, true);

            return (dataInitialised == 0) && (data.fileAttributes != -1) 
                    && ((data.fileAttributes  & Win32Native.FILE_ATTRIBUTE_DIRECTORY) == 0);
        }


        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static FileStream Open(String path, FileMode mode) {
            return Open(path, mode, (mode == FileMode.Append ? FileAccess.Write : FileAccess.ReadWrite), FileShare.None);
        }
    
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static FileStream Open(String path, FileMode mode, FileAccess access) {
            return Open(path,mode, access, FileShare.None);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static FileStream Open(String path, FileMode mode, FileAccess access, FileShare share) {
            return new FileStream(path, mode, access, share);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void SetCreationTime(String path, DateTime creationTime)
        {
            SetCreationTimeUtc(path, creationTime.ToUniversalTime());
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public unsafe static void SetCreationTimeUtc(String path, DateTime creationTimeUtc)
        {
            SafeFileHandle handle;
            using(OpenFile(path, FileAccess.Write, out handle)) {
                Win32Native.FILE_TIME fileTime = new Win32Native.FILE_TIME(creationTimeUtc.ToFileTimeUtc());
                bool r = Win32Native.SetFileTime(handle, &fileTime, null, null);
                if (!r)
                {
                     int errorCode = Marshal.GetLastWin32Error();
                    __Error.WinIOError(errorCode, path);
                }
            }
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static DateTime GetCreationTime(String path)
        {
            return GetCreationTimeUtc(path).ToLocalTime();
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static DateTime GetCreationTimeUtc(String path)
        {
            String fullPath = Path.GetFullPathInternal(path);
            new FileIOPermission(FileIOPermissionAccess.Read, new String[] { fullPath }, false, false ).Demand();

            Win32Native.WIN32_FILE_ATTRIBUTE_DATA data = new Win32Native.WIN32_FILE_ATTRIBUTE_DATA();
            int dataInitialised = FillAttributeInfo(fullPath, ref data, false, false);
            if (dataInitialised != 0)
                __Error.WinIOError(dataInitialised, fullPath);

            long dt = ((long)(data.ftCreationTimeHigh) << 32) | ((long)data.ftCreationTimeLow);
            return DateTime.FromFileTimeUtc(dt);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]      
        public static void SetLastAccessTime(String path, DateTime lastAccessTime)
        {
            SetLastAccessTimeUtc(path, lastAccessTime.ToUniversalTime());
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public unsafe static void SetLastAccessTimeUtc(String path, DateTime lastAccessTimeUtc)
        {
            SafeFileHandle handle;
            using(OpenFile(path, FileAccess.Write, out handle)) {
                Win32Native.FILE_TIME fileTime = new Win32Native.FILE_TIME(lastAccessTimeUtc.ToFileTimeUtc());
                bool r = Win32Native.SetFileTime(handle, null, &fileTime,  null);
                if (!r)
                {
                     int errorCode = Marshal.GetLastWin32Error();
                    __Error.WinIOError(errorCode, path);
                }
            }
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static DateTime GetLastAccessTime(String path)
        {
            return GetLastAccessTimeUtc(path).ToLocalTime();
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static DateTime GetLastAccessTimeUtc(String path)
        {
            String fullPath = Path.GetFullPathInternal(path);
            new FileIOPermission(FileIOPermissionAccess.Read, new String[] { fullPath }, false, false ).Demand();

            Win32Native.WIN32_FILE_ATTRIBUTE_DATA data = new Win32Native.WIN32_FILE_ATTRIBUTE_DATA();
            int dataInitialised = FillAttributeInfo(fullPath, ref data, false, false);
            if (dataInitialised != 0)
                __Error.WinIOError(dataInitialised, fullPath);

            long dt = ((long)(data.ftLastAccessTimeHigh) << 32) | ((long)data.ftLastAccessTimeLow);
            return DateTime.FromFileTimeUtc(dt);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void SetLastWriteTime(String path, DateTime lastWriteTime)
        {
            SetLastWriteTimeUtc(path, lastWriteTime.ToUniversalTime());
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public unsafe static void SetLastWriteTimeUtc(String path, DateTime lastWriteTimeUtc)
        {
            SafeFileHandle handle;
            using(OpenFile(path, FileAccess.Write, out handle)) {
                Win32Native.FILE_TIME fileTime = new Win32Native.FILE_TIME(lastWriteTimeUtc.ToFileTimeUtc());
                bool r = Win32Native.SetFileTime(handle, null, null, &fileTime);
                if (!r)
                {
                     int errorCode = Marshal.GetLastWin32Error();
                    __Error.WinIOError(errorCode, path);
                }
            }
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static DateTime GetLastWriteTime(String path)
        {
            return GetLastWriteTimeUtc(path).ToLocalTime();
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static DateTime GetLastWriteTimeUtc(String path)
        {
            String fullPath = Path.GetFullPathInternal(path);
            new FileIOPermission(FileIOPermissionAccess.Read, new String[] { fullPath }, false, false ).Demand();

            Win32Native.WIN32_FILE_ATTRIBUTE_DATA data = new Win32Native.WIN32_FILE_ATTRIBUTE_DATA();
            int dataInitialised = FillAttributeInfo(fullPath, ref data, false, false);
            if (dataInitialised != 0)
                __Error.WinIOError(dataInitialised, fullPath);

            long dt = ((long)data.ftLastWriteTimeHigh << 32) | ((long)data.ftLastWriteTimeLow);
            return DateTime.FromFileTimeUtc(dt);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static FileAttributes GetAttributes(String path) 
        {
            String fullPath = Path.GetFullPathInternal(path);
            new FileIOPermission(FileIOPermissionAccess.Read, new String[] { fullPath }, false, false).Demand();

            Win32Native.WIN32_FILE_ATTRIBUTE_DATA data = new Win32Native.WIN32_FILE_ATTRIBUTE_DATA();
            int dataInitialised = FillAttributeInfo(fullPath, ref data, false, true);
            if (dataInitialised != 0)
                __Error.WinIOError(dataInitialised, fullPath);

            return (FileAttributes) data.fileAttributes;
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void SetAttributes(String path, FileAttributes fileAttributes) 
        {
            String fullPath = Path.GetFullPathInternal(path);
            new FileIOPermission(FileIOPermissionAccess.Write, new String[] { fullPath }, false, false).Demand();
            bool r = Win32Native.SetFileAttributes(fullPath, (int) fileAttributes);
            if (!r) {
                int hr = Marshal.GetLastWin32Error();
                if (hr==ERROR_INVALID_PARAMETER || (hr==ERROR_ACCESS_DENIED && Environment.IsWin9X())) // Win9x returns Access denied sometimes
                    throw new ArgumentException(Environment.GetResourceString("Arg_InvalidFileAttrs"));
                 __Error.WinIOError(hr, fullPath);
            }
        }


        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static FileStream OpenRead(String path) {
            return new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.Read);
        }


        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static FileStream OpenWrite(String path) {
            return new FileStream(path, FileMode.OpenOrCreate, 
                                  FileAccess.Write, FileShare.None);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static String ReadAllText(String path)
        {
            return ReadAllText(path, Encoding.UTF8);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static String ReadAllText(String path, Encoding encoding)
        {
            using(StreamReader sr = new StreamReader(path, encoding))
                return sr.ReadToEnd();
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void WriteAllText(String path, String contents)
        {
            WriteAllText(path, contents, StreamWriter.UTF8NoBOM);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void WriteAllText(String path, String contents, Encoding encoding)
        {
            using(StreamWriter sw = new StreamWriter(path, false, encoding))
                sw.Write(contents);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static byte[] ReadAllBytes(String path)
        {
            byte[] bytes;
            using(FileStream fs = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.Read)) {
                // Do a blocking read
                int index = 0;
                long fileLength = fs.Length;
                if (fileLength > Int32.MaxValue)
                    throw new IOException(Environment.GetResourceString("IO.IO_FileTooLong2GB"));
                int count = (int) fileLength;
                bytes = new byte[count];
                while(count > 0) {
                    int n = fs.Read(bytes, index, count);
                    if (n == 0)
                        __Error.EndOfFile();
                    index += n;
                    count -= n;
                }
            }
            return bytes;
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void WriteAllBytes(String path, byte[] bytes)
        {
            if (bytes == null)
                throw new ArgumentNullException("bytes");

            using(FileStream fs = new FileStream(path, FileMode.Create, FileAccess.Write, FileShare.Read))
                fs.Write(bytes, 0, bytes.Length);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static String[] ReadAllLines(String path)
        {
            return ReadAllLines(path, Encoding.UTF8);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static String[] ReadAllLines(String path, Encoding encoding)
        {
            String line;
            ArrayList lines = new ArrayList();

            using(StreamReader sr = new StreamReader(path, encoding))
                while((line = sr.ReadLine()) != null)
                    lines.Add(line);

            return (String[]) lines.ToArray(typeof(String));
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void WriteAllLines(String path, String[] contents)
        {
            WriteAllLines(path, contents, StreamWriter.UTF8NoBOM);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void WriteAllLines(String path, String[] contents, Encoding encoding)
        {
            if (contents == null)
                throw new ArgumentNullException("contents");

            using(StreamWriter sw = new StreamWriter(path, false, encoding))
                foreach(String line in contents)
                    sw.WriteLine(line);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void AppendAllText(String path, String contents)
        {
            AppendAllText(path, contents, StreamWriter.UTF8NoBOM);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void AppendAllText(String path, String contents, Encoding encoding)
        {
            using(StreamWriter sw = new StreamWriter(path, true, encoding))
                sw.Write(contents);
        }

        // Moves a specified file to a new location and potentially a new file name.
        // This method does work across volumes.
        //
        // The caller must have certain FileIOPermissions.  The caller must
        // have Read and Write permission to 
        // sourceFileName and Write 
        // permissions to destFileName.
        // 
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void Move(String sourceFileName, String destFileName) {
            if (sourceFileName==null || destFileName==null)
                throw new ArgumentNullException((sourceFileName==null ? "sourceFileName" : "destFileName"), Environment.GetResourceString("ArgumentNull_FileName"));
            if (sourceFileName.Length==0 || destFileName.Length==0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyFileName"), (sourceFileName.Length==0 ? "sourceFileName" : "destFileName"));
            
            String fullSourceFileName = Path.GetFullPathInternal(sourceFileName);
            new FileIOPermission(FileIOPermissionAccess.Write | FileIOPermissionAccess.Read, new String[] { fullSourceFileName }, false, false).Demand();
            String fullDestFileName = Path.GetFullPathInternal(destFileName);
            new FileIOPermission(FileIOPermissionAccess.Write, new String[] { fullDestFileName }, false, false).Demand();

            if (!InternalExists(fullSourceFileName))
                __Error.WinIOError(Win32Native.ERROR_FILE_NOT_FOUND, fullSourceFileName);
            
            if (!Win32Native.MoveFile(fullSourceFileName, fullDestFileName))
            {
                __Error.WinIOError();
            }
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void Replace(String sourceFileName, String destinationFileName, String destinationBackupFileName)
        {
            Replace(sourceFileName, destinationFileName, destinationBackupFileName, false);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static void Replace(String sourceFileName, String destinationFileName, String destinationBackupFileName, bool ignoreMetadataErrors)
        {
            if (sourceFileName == null)
                throw new ArgumentNullException("sourceFileName");
            if (destinationFileName == null)
                throw new ArgumentNullException("destinationFileName");

            // Write permission to all three files, read permission to source 
            // and dest.
            String fullSrcPath = Path.GetFullPathInternal(sourceFileName);
            String fullDestPath = Path.GetFullPathInternal(destinationFileName);
            String fullBackupPath = null;
            if (destinationBackupFileName != null)
                fullBackupPath = Path.GetFullPathInternal(destinationBackupFileName);
            FileIOPermission perm = new FileIOPermission(FileIOPermissionAccess.Read | FileIOPermissionAccess.Write, new String[] { fullSrcPath, fullDestPath});
            if (destinationBackupFileName != null)
                perm.AddPathList(FileIOPermissionAccess.Write, fullBackupPath);
            perm.Demand();


            int flags = Win32Native.REPLACEFILE_WRITE_THROUGH;
            if (ignoreMetadataErrors)
                flags |= Win32Native.REPLACEFILE_IGNORE_MERGE_ERRORS;

            bool r = Win32Native.ReplaceFile(fullDestPath, fullSrcPath, fullBackupPath, flags, IntPtr.Zero, IntPtr.Zero);
            if (!r)
                __Error.WinIOError();
        }


        // Returns 0 on success, otherwise a Win32 error code.  Note that
        // classes should use -1 as the uninitialized state for dataInitialized.
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal static int FillAttributeInfo(String path, ref Win32Native.WIN32_FILE_ATTRIBUTE_DATA data, bool tryagain, bool returnErrorOnNotFound)
        {
            int dataInitialised = 0;
            if (tryagain) // someone has a handle to the file open, or other error
            {
                Win32Native.WIN32_FIND_DATA win95data; // We do this only on Win95 machines
                win95data =  new Win32Native.WIN32_FIND_DATA (); 
                
                // Remove trialing slash since this can cause grief to FindFirstFile. You will get an invalid argument error
                String tempPath = path.TrimEnd(new char [] {Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar});

                // For floppy drives, normally the OS will pop up a dialog saying
                // there is no disk in drive A:, please insert one.  We don't want that.
                // SetErrorMode will let us disable this, but we should set the error
                // mode back, since this may have wide-ranging effects.
                int oldMode = Win32Native.SetErrorMode(Win32Native.SEM_FAILCRITICALERRORS);
                try {
                    bool error = false;
                    SafeFindHandle handle = Win32Native.FindFirstFile(tempPath,win95data);
                    try {
                        if (handle.IsInvalid) {
                            error = true;
                            dataInitialised = Marshal.GetLastWin32Error();
                            
                            if (dataInitialised == Win32Native.ERROR_FILE_NOT_FOUND ||
                                dataInitialised == Win32Native.ERROR_PATH_NOT_FOUND ||
                                dataInitialised == Win32Native.ERROR_NOT_READY)  // floppy device not ready
                            {
                                if (!returnErrorOnNotFound) {
                                    // Return default value for backward compbatibility
                                    dataInitialised = 0;
                                    data.fileAttributes = -1;
                                }
                            }
                            return dataInitialised;
                        }
                    }
                    finally {
                        // Close the Win32 handle
                        try {
                            handle.Close();
                        }
                        catch {
                            // if we're already returning an error, don't throw another one. 
                            if (!error) {
                                BCLDebug.Assert(false, "File::FillAttributeInfo - FindClose failed!");
                                __Error.WinIOError();
                            }
                        }
                    }
                }
                finally {
                    Win32Native.SetErrorMode(oldMode);
                }

                // Copy the information to data
                data.fileAttributes = win95data.dwFileAttributes; 
                data.ftCreationTimeLow = (uint)win95data.ftCreationTime_dwLowDateTime; 
                data.ftCreationTimeHigh = (uint)win95data.ftCreationTime_dwHighDateTime; 
                data.ftLastAccessTimeLow = (uint)win95data.ftLastAccessTime_dwLowDateTime; 
                data.ftLastAccessTimeHigh = (uint)win95data.ftLastAccessTime_dwHighDateTime; 
                data.ftLastWriteTimeLow = (uint)win95data.ftLastWriteTime_dwLowDateTime; 
                data.ftLastWriteTimeHigh = (uint)win95data.ftLastWriteTime_dwHighDateTime; 
                data.fileSizeHigh = win95data.nFileSizeHigh; 
                data.fileSizeLow = win95data.nFileSizeLow; 
            }
            else
            {   
                                  
                 // For floppy drives, normally the OS will pop up a dialog saying
                // there is no disk in drive A:, please insert one.  We don't want that.
                // SetErrorMode will let us disable this, but we should set the error
                // mode back, since this may have wide-ranging effects.
                bool success = false;
                int oldMode = Win32Native.SetErrorMode(Win32Native.SEM_FAILCRITICALERRORS);
                try {
                    success = Win32Native.GetFileAttributesEx(path, GetFileExInfoStandard, ref data);
                }
                finally {
                    Win32Native.SetErrorMode(oldMode);
                }

                if (!success) {
                    dataInitialised = Marshal.GetLastWin32Error();
                    if (dataInitialised != Win32Native.ERROR_FILE_NOT_FOUND &&
                        dataInitialised != Win32Native.ERROR_PATH_NOT_FOUND &&
                        dataInitialised != Win32Native.ERROR_NOT_READY)  // floppy device not ready
                    {
                     // In case someone latched onto the file. Take the perf hit only for failure
                        return FillAttributeInfo(path, ref data, true, returnErrorOnNotFound);
                    }
                    else {
                        if (!returnErrorOnNotFound) {
                            // Return default value for backward compbatibility
                            dataInitialised = 0;
                            data.fileAttributes = -1;
                        }
                    }
                }
            }

            return dataInitialised;
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        private static FileStream OpenFile(String path, FileAccess access, out SafeFileHandle handle)
        {
            FileStream fs = new FileStream(path, FileMode.Open, access, FileShare.ReadWrite, 1);
            handle = fs.SafeFileHandle;

            if (handle.IsInvalid) {
                // Return a meaningful error, using the RELATIVE path to
                // the file to avoid returning extra information to the caller.
            
                // NT5 oddity - when trying to open "C:\" as a FileStream,
                // we usually get ERROR_PATH_NOT_FOUND from the OS.  We should
                // probably be consistent w/ every other directory.
                int hr = Marshal.GetLastWin32Error();
                String FullPath = Path.GetFullPathInternal(path);
                if (hr==__Error.ERROR_PATH_NOT_FOUND && FullPath.Equals(Directory.GetDirectoryRoot(FullPath)))
                    hr = __Error.ERROR_ACCESS_DENIED;


                __Error.WinIOError(hr, path);
            }
            return fs;
        }


         // Defined in WinError.h
        private const int ERROR_INVALID_PARAMETER = 87;
        private const int ERROR_ACCESS_DENIED = 0x5;     
    }
}
