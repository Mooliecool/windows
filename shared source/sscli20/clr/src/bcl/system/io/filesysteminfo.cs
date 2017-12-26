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
** Class:  FileSystemInfo    
**
**
** Purpose: 
**
**
===========================================================*/

using System;
using System.Collections;
using System.Security;
using System.Security.Permissions;
using Microsoft.Win32;
using System.Text;
using System.Runtime.InteropServices;
using System.Runtime.Serialization;
using System.Runtime.Versioning;

namespace System.IO {
    [Serializable]
    [FileIOPermissionAttribute(SecurityAction.InheritanceDemand,Unrestricted=true)]
    [ComVisible(true)]
    public abstract class FileSystemInfo : MarshalByRefObject, ISerializable {
        internal Win32Native.WIN32_FILE_ATTRIBUTE_DATA _data; // Cache the file information
        internal int _dataInitialised = -1; // We use this field in conjunction with the Refresh methods, if we succeed
                                           // we store a zero, on failure we store the HResult in it so that we can
                                           // give back a generic error back.

        private const int ERROR_INVALID_PARAMETER = 87;
        internal const int ERROR_ACCESS_DENIED = 0x5;

        protected String FullPath;  // fully qualified path of the directory
        protected String OriginalPath; // path passed in by the user

        protected FileSystemInfo()
        {
        }

        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        protected FileSystemInfo(SerializationInfo info, StreamingContext context)
        {
            if (info == null)
                throw new ArgumentNullException("info");
            
            // Must use V1 field names here, since V1 didn't implement 
            // ISerializable.
            FullPath = Path.GetFullPathInternal(info.GetString("FullPath"));
            OriginalPath = info.GetString("OriginalPath");

            // Lazily initialize the file attributes.
            _dataInitialised = -1;
        }


        // Full path of the direcory/file
        public virtual String FullName {
            get 
            {
                String demandDir;
                if (this is DirectoryInfo)
                    demandDir = Directory.GetDemandDir(FullPath, true);
                else
                    demandDir = FullPath;
                new FileIOPermission(FileIOPermissionAccess.PathDiscovery, demandDir).Demand();
                return FullPath;
            }
        }

        public String Extension 
        {
            get
            {
                // GetFullPathInternal would have already stripped out the terminating "." if present.
               int length = FullPath.Length;
                for (int i = length; --i >= 0;) {
                    char ch = FullPath[i];
                    if (ch == '.')
                        return FullPath.Substring(i, length - i);
                    if (ch == Path.DirectorySeparatorChar || ch == Path.AltDirectorySeparatorChar || ch == Path.VolumeSeparatorChar)
                        break;
                }
                return String.Empty;
            }
        }

        // For files name of the file is returned, for directories the last directory in hierarchy is returned if possible,
        // otherwise the fully qualified name s returned
        public abstract String Name {
            get;
        }
        
        // Whether a file/directory exists
        public abstract bool Exists
        {
            get;
        }

        // Delete a file/directory
           public abstract void Delete();

       public DateTime CreationTime {
            get {
                return CreationTimeUtc.ToLocalTime();
            }
        
            set {
                CreationTimeUtc = value.ToUniversalTime();
            }
        }

       [ComVisible(false)]
       public DateTime CreationTimeUtc {
            get {
                if (_dataInitialised == -1) {
                    _data = new Win32Native.WIN32_FILE_ATTRIBUTE_DATA();
                    Refresh();
                }

                if (_dataInitialised != 0) // Refresh was unable to initialise the data
                    __Error.WinIOError(_dataInitialised, OriginalPath);
                
                long fileTime = ((long)_data.ftCreationTimeHigh << 32) | _data.ftCreationTimeLow;
                return DateTime.FromFileTimeUtc(fileTime);
                
            }
        
            [ResourceExposure(ResourceScope.None)]
            [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
            set {
                if (this is DirectoryInfo)
                    Directory.SetCreationTimeUtc(FullPath,value);
                else
                    File.SetCreationTimeUtc(FullPath,value);
                _dataInitialised = -1;
            }
        }

        public DateTime LastAccessTime {
            get {
                return LastAccessTimeUtc.ToLocalTime();    
            }
            set {
                LastAccessTimeUtc = value.ToUniversalTime();
            }
        }

        [ComVisible(false)]
        public DateTime LastAccessTimeUtc {
            get {
                if (_dataInitialised == -1) {
                    _data = new Win32Native.WIN32_FILE_ATTRIBUTE_DATA();
                    Refresh();
                }

                if (_dataInitialised != 0) // Refresh was unable to initialise the data
                    __Error.WinIOError(_dataInitialised, OriginalPath);
                    
                long fileTime = ((long)_data.ftLastAccessTimeHigh << 32) | _data.ftLastAccessTimeLow;
                return DateTime.FromFileTimeUtc(fileTime);
    
            }

            [ResourceExposure(ResourceScope.None)]
            [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
            set {
                if (this is DirectoryInfo)
                    Directory.SetLastAccessTimeUtc(FullPath,value);
                else
                    File.SetLastAccessTimeUtc(FullPath,value);
                _dataInitialised = -1;
            }
        }

        public DateTime LastWriteTime {
            get {
                return LastWriteTimeUtc.ToLocalTime();
            }
            set {
                LastWriteTimeUtc = value.ToUniversalTime();
            }
        }

        [ComVisible(false)]
        public DateTime LastWriteTimeUtc {
            get {
                if (_dataInitialised == -1) {
                    _data = new Win32Native.WIN32_FILE_ATTRIBUTE_DATA();
                    Refresh();
                }

                if (_dataInitialised != 0) // Refresh was unable to initialise the data
                    __Error.WinIOError(_dataInitialised, OriginalPath);
        
            
                long fileTime = ((long)_data.ftLastWriteTimeHigh << 32) | _data.ftLastWriteTimeLow;
                return DateTime.FromFileTimeUtc(fileTime);
            }

            [ResourceExposure(ResourceScope.None)]
            [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
            set {
                if (this is DirectoryInfo)
                    Directory.SetLastWriteTimeUtc(FullPath,value);
                else
                    File.SetLastWriteTimeUtc(FullPath,value);
                _dataInitialised = -1;
            }
        }

        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        public void Refresh()
        {
            _dataInitialised = File.FillAttributeInfo(FullPath, ref _data, false, false);
        }

        public FileAttributes Attributes {
            get {
                if (_dataInitialised == -1) {
                    _data = new Win32Native.WIN32_FILE_ATTRIBUTE_DATA();
                    Refresh(); // Call refresh to intialise the data
                }

                if (_dataInitialised != 0) // Refresh was unable to initialise the data
                    __Error.WinIOError(_dataInitialised, OriginalPath);

                return (FileAttributes) _data.fileAttributes;
            }
            set {
                new FileIOPermission(FileIOPermissionAccess.Write, FullPath).Demand();
                bool r = Win32Native.SetFileAttributes(FullPath, (int) value);
                if (!r) {
                    int hr = Marshal.GetLastWin32Error();
                    
                    if (hr==ERROR_INVALID_PARAMETER)
                        throw new ArgumentException(Environment.GetResourceString("Arg_InvalidFileAttrs"));
                    
                    // For whatever reason we are turning ERROR_ACCESS_DENIED into 
                    // ArgumentException here (probably done for some 9x code path).
                    // We can't change this now but special casing the error message instead.
                    if (hr == ERROR_ACCESS_DENIED)
                        throw new ArgumentException(Environment.GetResourceString("UnauthorizedAccess_IODenied_NoPathName"));
                    __Error.WinIOError(hr, OriginalPath);
                }
                _dataInitialised = -1;
            }
        }

        [ComVisible(false)]
        [SecurityPermission(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)]
        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            new FileIOPermission(FileIOPermissionAccess.PathDiscovery, FullPath).Demand();

            info.AddValue("OriginalPath", OriginalPath, typeof(String));
            info.AddValue("FullPath", FullPath, typeof(String));
        }
    }       
}
