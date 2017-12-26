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
**          April 09,2000 (some design refactorization)
**
===========================================================*/

using System;
using System.Security.Permissions;
using PermissionSet = System.Security.PermissionSet;
using Win32Native = Microsoft.Win32.Win32Native;
using System.Runtime.InteropServices;
using System.Text;
using System.Runtime.Serialization;
using System.Globalization;
using System.Runtime.Versioning;

namespace System.IO {    
    // Class for creating FileStream objects, and some basic file management
    // routines such as Delete, etc.
    [Serializable]
    [ComVisible(true)]
    public sealed class FileInfo: FileSystemInfo
    {
        private String _name;
     
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileInfo(String fileName)
        {
            if (fileName==null)
                 throw new ArgumentNullException("fileName");
         
            OriginalPath = fileName;
            // Must fully qualify the path for the security check
            String fullPath = Path.GetFullPathInternal(fileName);
            new FileIOPermission(FileIOPermissionAccess.Read, new String[] { fullPath }, false, false).Demand();

            _name = Path.GetFileName(fileName);
            FullPath = fullPath;
        }

        private FileInfo(SerializationInfo info, StreamingContext context) : base(info, context)
        {
            new FileIOPermission(FileIOPermissionAccess.Read, new String[] { FullPath }, false, false).Demand();
            _name = Path.GetFileName(OriginalPath);
        }

        internal FileInfo(String fullPath, bool ignoreThis)
        {
            BCLDebug.Assert(Path.GetRootLength(fullPath) > 0, "fullPath must be fully qualified!");
            _name = Path.GetFileName(fullPath);
            OriginalPath = _name;
            FullPath = fullPath;
        }

        public override String Name {
            get { return _name; }
        }
    
   
        public long Length {
            get {
                if (_dataInitialised == -1)
                    Refresh();
                
                if (_dataInitialised != 0) // Refresh was unable to initialise the data
                    __Error.WinIOError(_dataInitialised, OriginalPath);
        
                if ((_data.fileAttributes & Win32Native.FILE_ATTRIBUTE_DIRECTORY) != 0)
                    __Error.WinIOError(Win32Native.ERROR_FILE_NOT_FOUND,OriginalPath);
                
                return ((long)_data.fileSizeHigh) << 32 | ((long)_data.fileSizeLow & 0xFFFFFFFFL);
            }
        }

        /* Returns the name of the directory that the file is in */
        public String DirectoryName
        {
            get
            {
                String directoryName = Path.GetDirectoryName(FullPath);
                if (directoryName != null)
                    new FileIOPermission(FileIOPermissionAccess.PathDiscovery, new String[] { directoryName }, false, false).Demand();
                return directoryName;
            }
        }

        /* Creates an instance of the the parent directory */
        public DirectoryInfo Directory
        {
            [ResourceExposure(ResourceScope.Machine)]
            [ResourceConsumption(ResourceScope.Machine)]
            get
            {
                String dirName = DirectoryName;
                if (dirName == null)
                    return null;
                return new DirectoryInfo(dirName);    
            }
        } 

        public bool IsReadOnly {
            get {
                return (Attributes & FileAttributes.ReadOnly) != 0;
            }
            set {
                if (value)
                    Attributes |= FileAttributes.ReadOnly;
                else
                    Attributes &= ~FileAttributes.ReadOnly;
            }
        }


        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public StreamReader OpenText()
        {
            return new StreamReader(FullPath, Encoding.UTF8, true, StreamReader.DefaultBufferSize);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public StreamWriter CreateText()
        {
            return new StreamWriter(FullPath,false);
        }


        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public StreamWriter AppendText()
        {
            return new StreamWriter(FullPath,true);
        }

        
        // Copies an existing file to a new file. An exception is raised if the
        // destination file already exists. Use the 
        // Copy(String, String, boolean) method to allow 
        // overwriting an existing file.
        //
        // The caller must have certain FileIOPermissions.  The caller must have
        // Read permission to sourceFileName 
        // and Write permissions to destFileName.
        // 
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileInfo CopyTo(String destFileName) {
            return CopyTo(destFileName, false);
        }


        // Copies an existing file to a new file. If overwrite is 
        // false, then an IOException is thrown if the destination file 
        // already exists.  If overwrite is true, the file is 
        // overwritten.
        //
        // The caller must have certain FileIOPermissions.  The caller must have
        // Read permission to sourceFileName and Create
        // and Write permissions to destFileName.
        // 
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileInfo CopyTo(String destFileName, bool overwrite) {
            destFileName = File.InternalCopy(FullPath, destFileName, overwrite);
            return new FileInfo(destFileName, false);
        }

        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        public FileStream Create() {
            return File.Create(FullPath);
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
        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        public override void Delete() {
            // For security check, path should be resolved to an absolute path.
            new FileIOPermission(FileIOPermissionAccess.Write, new String[] { FullPath }, false, false).Demand();

            if (Environment.IsWin9X() && System.IO.Directory.InternalExists(FullPath)) // Win9x fails silently for directories
                throw new UnauthorizedAccessException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("UnauthorizedAccess_IODenied_Path"), OriginalPath));
    
            bool r = Win32Native.DeleteFile(FullPath);
            if (!r) {
                int hr = Marshal.GetLastWin32Error();
                if (hr==Win32Native.ERROR_FILE_NOT_FOUND)
                    return;
                else
                    __Error.WinIOError(hr, OriginalPath);
            }
        }

        [ComVisible(false)]
        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        public void Decrypt()
        {
            File.Decrypt(FullPath);
        }

        [ComVisible(false)]
        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        public void Encrypt()
        {
            File.Encrypt(FullPath);
        }

        // Tests if the given file exists. The result is true if the file
        // given by the specified path exists; otherwise, the result is
        // false.  
        //
        // Your application must have Read permission for the target directory.
        public override bool Exists {
            get {
                try {
                    if (_dataInitialised == -1)
                        Refresh();
                    if (_dataInitialised != 0) {
                        // Refresh was unable to initialise the data.
                        // We should normally be throwing an exception here, 
                        // but Exists is supposed to return true or false.
                        return false;
                    }
                    return (_data.fileAttributes & Win32Native.FILE_ATTRIBUTE_DIRECTORY) == 0;
                }
                catch
                {
                    return false;
                }
            }
        }

        
      
      
        // User must explicitly specify opening a new file or appending to one.
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileStream Open(FileMode mode) {
            return Open(mode, FileAccess.ReadWrite, FileShare.None);
        }

        
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileStream Open(FileMode mode, FileAccess access) {
            return Open(mode, access, FileShare.None);
        }

        
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileStream Open(FileMode mode, FileAccess access, FileShare share) {
            return new FileStream(FullPath, mode, access, share);
        }

        

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileStream OpenRead() {
            return new FileStream(FullPath, FileMode.Open, FileAccess.Read,
                                  FileShare.Read);
        }

        
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileStream OpenWrite() {
            return new FileStream(FullPath, FileMode.OpenOrCreate, 
                                  FileAccess.Write, FileShare.None);
        }

      

       
        

        // Moves a given file to a new location and potentially a new file name.
        // This method does work across volumes.
        //
        // The caller must have certain FileIOPermissions.  The caller must
        // have Read and Write permission to 
        // sourceFileName and Write 
        // permissions to destFileName.
        // 
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public void MoveTo(String destFileName) {
            if (destFileName==null)
                throw new ArgumentNullException("destFileName");
            if (destFileName.Length==0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyFileName"), "destFileName");
            
            new FileIOPermission(FileIOPermissionAccess.Write | FileIOPermissionAccess.Read, new String[] { FullPath }, false, false).Demand();
            String fullDestFileName = Path.GetFullPathInternal(destFileName);
            new FileIOPermission(FileIOPermissionAccess.Write, new String[] { fullDestFileName }, false, false).Demand();
            
            if (!Win32Native.MoveFile(FullPath, fullDestFileName))
                __Error.WinIOError();
            FullPath = fullDestFileName;
            OriginalPath = destFileName;
            _name = Path.GetFileName(fullDestFileName);

            // Flush any cached information about the file.
            _dataInitialised = -1;
        }

        [ComVisible(false)]
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileInfo Replace(String destinationFileName, String destinationBackupFileName)
        {
            return Replace(destinationFileName, destinationBackupFileName, false);
        }

        [ComVisible(false)]
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileInfo Replace(String destinationFileName, String destinationBackupFileName, bool ignoreMetadataErrors)
        {
            File.Replace(FullPath, destinationFileName, destinationBackupFileName, ignoreMetadataErrors);
            return new FileInfo(destinationFileName);
        }

        // Returns the original path
        public override String ToString()
        {
            return OriginalPath;
        }
    }
}
