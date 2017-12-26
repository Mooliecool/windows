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
 * 
 * Class:  IsolatedStorageFile
 *
 *
 * Purpose: Provides access to Application files and folders
 *
 * Date:  Feb 18, 2000
 *
 ===========================================================*/
namespace System.IO.IsolatedStorage {
    using System;
    using System.Text;
    using System.IO;
    using Microsoft.Win32;
    using Microsoft.Win32.SafeHandles;
    using System.Collections;
    using System.Security;
    using System.Threading;
    using System.Security.Policy;
    using System.Security.Permissions;
    using System.Security.Cryptography;
    using System.Runtime.InteropServices;
    using System.Runtime.CompilerServices;
    using System.Runtime.Versioning;
    using System.Globalization;

[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class IsolatedStorageFile : IsolatedStorage, IDisposable
    {
        private  const int    s_BlockSize = 1024;
        private  const int    s_DirSize   = s_BlockSize;
        private  const String s_name      = "file.store";
        internal const String s_Files     = "Files";
        internal const String s_AssemFiles= "AssemFiles";
        internal const String s_AppFiles= "AppFiles";        
        internal const String s_IDFile    = "identity.dat";
        internal const String s_InfoFile  = "info.dat";
        internal const String s_AppInfoFile  = "appinfo.dat";

        private static String s_RootDirUser;
        private static String s_RootDirMachine;
        private static String s_RootDirRoaming;
        private static String s_appDataDir;

        private static FileIOPermission              s_PermUser;
        private static FileIOPermission              s_PermMachine;
        private static FileIOPermission              s_PermRoaming;
        private static IsolatedStorageFilePermission s_PermAdminUser;

        private FileIOPermission m_fiop;
        private String           m_RootDir;
        private String           m_InfoFile;
        private String           m_SyncObjectName;
        private IntPtr           m_handle;
        private bool             m_closed;
        private bool             m_bDisposed = false;

#if _DEBUG
        private static bool s_fDebug;
#endif

        internal IsolatedStorageFile() {}

        [ResourceExposure(ResourceScope.AppDomain | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly, ResourceScope.AppDomain | ResourceScope.Assembly)]
        public static IsolatedStorageFile GetUserStoreForDomain()
        {
            return GetStore(
                IsolatedStorageScope.Assembly|
                IsolatedStorageScope.Domain|
                IsolatedStorageScope.User, 
                null, null);
        }

        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly, ResourceScope.Machine | ResourceScope.Assembly)]
        public static IsolatedStorageFile GetUserStoreForAssembly()
        {
            return GetStore(
                IsolatedStorageScope.Assembly|
                IsolatedStorageScope.User, 
                null, null);
        }

        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly, ResourceScope.Machine | ResourceScope.Assembly)]
        public static IsolatedStorageFile GetUserStoreForApplication()
        {
            return GetStore(
                IsolatedStorageScope.Application|
                IsolatedStorageScope.User, 
                null);
        }

        [ResourceExposure(ResourceScope.AppDomain | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly, ResourceScope.AppDomain | ResourceScope.Assembly)]
        public static IsolatedStorageFile GetMachineStoreForDomain()
        {
            return GetStore(
                IsolatedStorageScope.Assembly|
                IsolatedStorageScope.Domain|
                IsolatedStorageScope.Machine, 
                null, null);
        }

        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly, ResourceScope.Machine | ResourceScope.Assembly)]
        public static IsolatedStorageFile GetMachineStoreForAssembly()
        {
            return GetStore(
                IsolatedStorageScope.Assembly|
                IsolatedStorageScope.Machine, 
                null, null);
        }

        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly, ResourceScope.Machine | ResourceScope.Assembly)]
        public static IsolatedStorageFile GetMachineStoreForApplication()
        {
            return GetStore(
                IsolatedStorageScope.Application|
                IsolatedStorageScope.Machine, 
                null);
        }

        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly)]
        public static IsolatedStorageFile GetStore(IsolatedStorageScope scope, 
            Type domainEvidenceType, Type assemblyEvidenceType)
        {
            if (domainEvidenceType != null)
                DemandAdminPermission();

            IsolatedStorageFile sf = new IsolatedStorageFile();
            sf.InitStore(scope, domainEvidenceType, assemblyEvidenceType);
            sf.Init(scope);
            return sf;
        }

        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly)]
        public static IsolatedStorageFile GetStore(IsolatedStorageScope scope, 
            Object domainIdentity, Object assemblyIdentity)
        {
            // Verify input params.
            if (IsDomain(scope) && (domainIdentity == null))
                throw new ArgumentNullException("domainIdentity");

            if (assemblyIdentity == null)
                throw new ArgumentNullException("assemblyIdentity");

            DemandAdminPermission();

            IsolatedStorageFile sf = new IsolatedStorageFile();
            sf.InitStore(scope, domainIdentity, assemblyIdentity, null); 
            sf.Init(scope);

            return sf;
        }

        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly)]
        public static IsolatedStorageFile GetStore(IsolatedStorageScope scope, 
            Evidence domainEvidence, Type  domainEvidenceType,
            Evidence assemblyEvidence, Type assemblyEvidenceType)
        {
            // Verify input params.
            if (IsDomain(scope) && (domainEvidence == null))
                throw new ArgumentNullException("domainEvidence");

            if (assemblyEvidence == null)
                throw new ArgumentNullException("assemblyEvidence");

            DemandAdminPermission();

            IsolatedStorageFile sf = new IsolatedStorageFile();
            sf.InitStore(scope, domainEvidence, domainEvidenceType,
                assemblyEvidence, assemblyEvidenceType, null, null);
            sf.Init(scope);

            return sf;
        }

        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly)]
        public static IsolatedStorageFile GetStore(IsolatedStorageScope scope, 
                                                                    Type applicationEvidenceType)
        {
            if (applicationEvidenceType != null)
                DemandAdminPermission();

            IsolatedStorageFile sf = new IsolatedStorageFile();
            sf.InitStore(scope, applicationEvidenceType);
            sf.Init(scope);
            return sf;
        }

        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly)]
        public static IsolatedStorageFile GetStore(IsolatedStorageScope scope, 
                                                                    Object applicationIdentity)
        {
            if (applicationIdentity == null)
                throw new ArgumentNullException("applicationIdentity");

            DemandAdminPermission();

            IsolatedStorageFile sf = new IsolatedStorageFile();
            sf.InitStore(scope, null, null, applicationIdentity); 
            sf.Init(scope);

            return sf;
        }

        [CLSCompliant(false)]
        public override ulong CurrentSize
        {
            get { 
                if (IsRoaming())
                    throw new InvalidOperationException(
                        Environment.GetResourceString(
                            "IsolatedStorage_CurrentSizeUndefined"));

                lock (this)
                {
                    if (m_bDisposed)
                        throw new ObjectDisposedException(null, Environment.GetResourceString("IsolatedStorage_StoreNotOpen"));

                    if (m_closed)
                        throw new InvalidOperationException(
                            Environment.GetResourceString(
                                "IsolatedStorage_StoreNotOpen"));

                    if (m_handle == Win32Native.NULL)
                        m_handle = nOpen(m_InfoFile, GetSyncObjectName());

                    return nGetUsage(m_handle); 
                }
            }
        }

        [CLSCompliant(false)]
        public override ulong MaximumSize
        {
            get 
            { 
                if (IsRoaming())
                    return Int64.MaxValue;

                return base.MaximumSize;
            }
        }

        internal unsafe void Reserve(ulong lReserve)
        {
            if (IsRoaming())  // No Quota enforcement for roaming
                return;

            ulong quota = this.MaximumSize;
            ulong reserve = lReserve;

            lock (this)
            {
                if (m_bDisposed)
                    throw new ObjectDisposedException(null, Environment.GetResourceString("IsolatedStorage_StoreNotOpen"));

                if (m_closed)
                    throw new InvalidOperationException(
                        Environment.GetResourceString(
                            "IsolatedStorage_StoreNotOpen"));

                if (m_handle == Win32Native.NULL)
                    m_handle = nOpen(m_InfoFile, GetSyncObjectName());

                nReserve(m_handle, &quota, &reserve, false);
            }
        }

        internal unsafe void Unreserve(ulong lFree)
        {
            if (IsRoaming())  // No Quota enforcement for roaming
                return;

            ulong quota = this.MaximumSize;
            ulong free = lFree;

            lock (this)
            {
                if (m_bDisposed)
                    throw new ObjectDisposedException(null, Environment.GetResourceString("IsolatedStorage_StoreNotOpen"));

                if (m_closed)
                    throw new InvalidOperationException(
                        Environment.GetResourceString(
                            "IsolatedStorage_StoreNotOpen"));

                if (m_handle == Win32Native.NULL)
                    m_handle = nOpen(m_InfoFile, GetSyncObjectName());

                nReserve(m_handle, &quota, &free, true);
            }
        }

        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        public void DeleteFile(String file)
        {
            if (file == null)
                throw new ArgumentNullException("file");

            m_fiop.Assert();
            m_fiop.PermitOnly();

            FileInfo f = new FileInfo(GetFullPath(file));
            long oldLen = 0;

            Lock(); // protect oldLen

            try {
                try {
                    oldLen = f.Length;
                    f.Delete();
                } catch {
                    throw new IsolatedStorageException(
                        Environment.GetResourceString(
                            "IsolatedStorage_DeleteFile"));
                }
                Unreserve(RoundToBlockSize((ulong)oldLen));
            } finally {
                Unlock();
            }
            CodeAccessPermission.RevertAll();
            
        }

        [ResourceExposure(ResourceScope.None)]  // Scoping should be done when opening isolated storage
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        public void CreateDirectory(String dir)
        {
            if (dir == null)
                throw new ArgumentNullException("dir");

            String isPath = GetFullPath(dir); // Prepend IS root

            String fullPath = Path.GetFullPathInternal(isPath);

            String [] dirList = DirectoriesToCreate(fullPath);
            
            if (dirList == null || dirList.Length == 0)
            {
                if (Directory.Exists(isPath))
                    return;
                else
                    throw new IsolatedStorageException(
                        Environment.GetResourceString(
                            "IsolatedStorage_CreateDirectory"));
            }
            Reserve(s_DirSize*((ulong)dirList.Length));
            m_fiop.Assert();
            m_fiop.PermitOnly();
            try {
                Directory.CreateDirectory(dirList[dirList.Length-1]);
            } catch {
                Unreserve(s_DirSize*((ulong)dirList.Length));
                // force delete any new directories we created
                Directory.Delete(dirList[0],true);
                throw new IsolatedStorageException(
                    Environment.GetResourceString(
                        "IsolatedStorage_CreateDirectory"));
            }
            CodeAccessPermission.RevertAll();
            
        }

        // Given a path to a dir to create, will return the list of directories to create and the last one in the array is the actual dir to create.
        // for example if dir is a\\b\\c and none of them exist, the list returned will be a, a\\b, a\\b\\c.
        private String[] DirectoriesToCreate(String fullPath)
        {
            
            ArrayList list = new ArrayList();
            int length = fullPath.Length;

            // We need to trim the trailing slash or the code will try to create 2 directories of the same name.
            if (length >= 2 && fullPath[length - 1] == SeparatorExternal)
                length--;
            int i = Path.GetRootLength(fullPath);

            // Attempt to figure out which directories don't exist
            while (i < length) {
                i++;
                while (i < length && fullPath[i] != SeparatorExternal) 
                    i++;
                String currDir = fullPath.Substring(0, i);
                    
                if (!Directory.InternalExists(currDir)) 
                { // Create only the ones missing
                    list.Add(currDir);
                }
            }

            if (list.Count != 0)
            {
                return (String[])list.ToArray(typeof(String));
            }
            return null;
        }

        public void DeleteDirectory(String dir)
        {
            if (dir == null)
                throw new ArgumentNullException("dir");

            m_fiop.Assert();
            m_fiop.PermitOnly();

            Lock(); // Delete *.*, will beat quota enforcement without this lock

            try {
                try {
                    new DirectoryInfo(GetFullPath(dir)).Delete(false);
                } catch {
                    throw new IsolatedStorageException(
                        Environment.GetResourceString(
                            "IsolatedStorage_DeleteDirectory"));
                }
                Unreserve(s_DirSize);
            } finally {
                Unlock();
            }
            CodeAccessPermission.RevertAll();
        }

        /*
         * foo\abc*.txt will give all abc*.txt files in foo directory
         */
        public String[] GetFileNames(String searchPattern)
        {
            if (searchPattern == null)
                throw new ArgumentNullException("searchPattern");

            m_fiop.Assert();
            m_fiop.PermitOnly();
            String[] retVal = GetFileDirectoryNames(GetFullPath(searchPattern), searchPattern, true);
            CodeAccessPermission.RevertAll();
            return retVal;
            
        }

        /*
         * foo\data* will give all directory names in foo directory that 
         * starts with data
         */
        [ResourceExposure(ResourceScope.None)]  // Scoping should be done when opening isolated storage.
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        public String[] GetDirectoryNames(String searchPattern)
        {
            if (searchPattern == null)
                throw new ArgumentNullException("searchPattern");

            m_fiop.Assert();
            m_fiop.PermitOnly();
            String[] retVal = GetFileDirectoryNames(GetFullPath(searchPattern), searchPattern, false);
            CodeAccessPermission.RevertAll();
            return retVal;
        }

        // Remove this individual store
        [ResourceExposure(ResourceScope.None)]  // Scoping should be done when opening isolated storage.
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        public override void Remove()
        {
            // No security check required here since we have already done
            // that during creation

            String rootDir, domainRoot = null;

            // First remove the logical root directory of this store, this
            // will not delete the quota file. Removes all the files and dirs
            // that applications see.

            RemoveLogicalDir();

            Close();

            // Now try to remove other files folders that become unnecessary
            // if the application directory is deleted.

            StringBuilder sb = new StringBuilder();

            sb.Append(GetRootDir(this.Scope));

            if (IsApp())
            {
                sb.Append(this.AppName);
                sb.Append(this.SeparatorExternal);
            }
            else 
            {
                if (IsDomain())
                {
                    sb.Append(this.DomainName);
                    sb.Append(this.SeparatorExternal);
                    domainRoot = sb.ToString();
                }

                sb.Append(this.AssemName);
                sb.Append(this.SeparatorExternal);
            }
            rootDir = sb.ToString();

            new FileIOPermission(
                FileIOPermissionAccess.AllAccess, rootDir).Assert();

            if (ContainsUnknownFiles(rootDir))
                return;

            try {

                Directory.Delete(rootDir, true);
#if _DEBUG
            } catch (Exception e) {

                if (s_fDebug)
                {
                    Console.WriteLine(e);
                    Console.WriteLine("Delete failed on rootdir");
                }
#else
            } catch {
#endif
                return; // OK to ignore this exception.
            }

            // If this was a domain store, and if this happens to be
            // the only store around, then delete the root store for this
            // domain

            if (IsDomain())
            {
                CodeAccessPermission.RevertAssert();

                new FileIOPermission(
                    FileIOPermissionAccess.AllAccess, domainRoot).Assert();

                if (!ContainsUnknownFiles(domainRoot))
                {

                    try {

                        Directory.Delete(domainRoot, true);
#if _DEBUG
                    } catch (Exception e) {

                        if (s_fDebug)
                        {
                            Console.WriteLine(e);
                            Console.WriteLine("Delete failed on basedir");
                        }
#else
                    } catch {
#endif
                        return; // OK to ignore this exception.
                    }
                }
            }
        }

        [ResourceExposure(ResourceScope.None)]  // Scoping should be done when opening isolated storage.
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        private void RemoveLogicalDir()
        {
            m_fiop.Assert();

            ulong oldLen;

            Lock(); // A race here with delete dir/delete file can get around 
                    // quota enforcement.

            try {
                oldLen = IsRoaming() ? 0 : CurrentSize;
    
                try {
    
                    Directory.Delete(RootDirectory, true);
#if _DEBUG
                } catch (Exception e) {
    
                    if (s_fDebug)
                    {
                        Console.WriteLine(e);
                        Console.WriteLine("Delete failed on LogicalRooDir");
                    }
#else
                    } catch {
#endif
                    throw new IsolatedStorageException(
                        Environment.GetResourceString(
                            "IsolatedStorage_DeleteDirectories"));
                }
    
                Unreserve(oldLen);
            } finally {
                Unlock();
            }
        }

        private bool ContainsUnknownFiles(String rootDir)
        {
            String[] dirs, files;

            // Delete everything in the root directory of this store
            // if there are no Domain Stores / other files
            // Make sure that there are no other subdirs present here other
            // than the ones used by IsolatedStorageFile (Cookies in future
            // releases ?)

            try {
                files = GetFileDirectoryNames(rootDir + "*", "*", true);
                dirs = GetFileDirectoryNames(rootDir + "*", "*", false);
            } catch {
                throw new IsolatedStorageException(
                    Environment.GetResourceString(
                        "IsolatedStorage_DeleteDirectories"));
            }

            // First see if there are any unkonwn Folders
            if ((dirs != null) && (dirs.Length > 0))
            {
                if (dirs.Length > 1)
                {
                    // More than one directory present
                    return true;
                }

                if (IsApp())
                {
                    if (NotAppFilesDir(dirs[0]))
                        return true;
                }
                else if (IsDomain())
                {
                    if (NotFilesDir(dirs[0]))
                        return true;
                }
                else
                {
                    if (NotAssemFilesDir(dirs[0]))
                        return true;
                }
            }

            // Now look at the files

            if ((files == null) || (files.Length == 0))
                return false;

            if (IsRoaming())
            {
                if ((files.Length > 1) || NotIDFile(files[0]))
                {
                    // There is one or more files unknown to this version
                    // of IsoStoreFile

                    return true;
                }

                return false;
            }

            if ((files.Length > 2) ||
                (NotIDFile(files[0]) && NotInfoFile(files[0])) ||
                ((files.Length == 2) &&
                NotIDFile(files[1]) && NotInfoFile(files[1])))
            {
                // There is one or more files unknown to this version
                // of IsoStoreFile

                return true;
            }

            return false; 
        }

        public void Close()
        {
            if (IsRoaming())
                return;
            
            lock (this) {

                if (!m_closed) {
                    m_closed = true;

                    IntPtr handle = m_handle;

                    m_handle = Win32Native.NULL;
                    nClose(handle);

                    GC.nativeSuppressFinalize(this);
                }

            }
        }

        public void Dispose()
        {
            Close();
            m_bDisposed = true;
        }

        ~IsolatedStorageFile()
        {
            Dispose();
        }

        // Macros, expect JIT to expand this
        private static bool NotIDFile(String file)
        {
            return (String.Compare(
                file, IsolatedStorageFile.s_IDFile, StringComparison.Ordinal) != 0); 
        }

        private static bool NotInfoFile(String file)
        {
            return (
                String.Compare(file, IsolatedStorageFile.s_InfoFile, StringComparison.Ordinal) != 0 && 
                String.Compare(file, IsolatedStorageFile.s_AppInfoFile, StringComparison.Ordinal) != 0);
        }

        private static bool NotFilesDir(String dir)
        {
            return (String.Compare(
                dir, IsolatedStorageFile.s_Files, StringComparison.Ordinal) != 0);
        }

        internal static bool NotAssemFilesDir(String dir)
        {
            return (String.Compare(
                dir, IsolatedStorageFile.s_AssemFiles, StringComparison.Ordinal) != 0);
        }

        internal static bool NotAppFilesDir(String dir)
        {
            return (String.Compare(
                dir, IsolatedStorageFile.s_AppFiles, StringComparison.Ordinal) != 0);
        }

        // Remove store for all identities
        [ResourceExposure(ResourceScope.None)]  // Scoping should be done when opening isolated storage.
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        public static void Remove(IsolatedStorageScope scope)
        {
            VerifyGlobalScope(scope);
            DemandAdminPermission();

            String rootDir = GetRootDir(scope);

            new FileIOPermission(
                FileIOPermissionAccess.Write, rootDir).Assert();

            try {
                Directory.Delete(rootDir, true);    // Remove all sub dirs and files
                Directory.CreateDirectory(rootDir); // Recreate the root dir
            } catch {
                throw new IsolatedStorageException(
                    Environment.GetResourceString(
                        "IsolatedStorage_DeleteDirectories"));
            }
        }

        public static IEnumerator GetEnumerator(IsolatedStorageScope scope)
        {
            VerifyGlobalScope(scope);
            DemandAdminPermission();

            return new IsolatedStorageFileEnumerator(scope);
        }

        // Internal & private methods

        internal String RootDirectory
        {
            get { return m_RootDir; }
        }

        // RootDirectory has been scoped already.
        internal String GetFullPath(String path)
        {
            StringBuilder sb = new StringBuilder();

            sb.Append(this.RootDirectory);

            if (path[0] == SeparatorExternal)
                sb.Append(path.Substring(1));
            else
                sb.Append(path);

            return sb.ToString();
        }


        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine | ResourceScope.Assembly)]
        internal void Init(IsolatedStorageScope scope)
        {
            GetGlobalFileIOPerm(scope).Assert();

            StringBuilder sb = new StringBuilder();

            // Create the root directory if it is not already there


            if (IsApp(scope))
            {
                throw new IsolatedStorageException(
                        Environment.GetResourceString(
                            "IsolatedStorage_ApplicationMissingIdentity"));
    
                sb.Append(GetRootDir(scope));
                if (s_appDataDir == null)
                {
                    // We're not using the App Data directory...so we need to append AppName 
                    sb.Append(this.AppName);
                    sb.Append(this.SeparatorExternal);
                }

                try {

                    Directory.CreateDirectory(sb.ToString());

                    // No exception implies this directory was created now


                } catch {
                    // Ok to ignore IO exception

                }
                // Create the Identity blob file in the root 
                // directory. OK if there are more than one created
                // last one wins

                CreateIDFile(sb.ToString(), scope);

                // For App Stores, accounting is done in the app root
                this.m_InfoFile = sb.ToString() + s_AppInfoFile;

                sb.Append(s_AppFiles);
                
            }
            else
            {
                sb.Append(GetRootDir(scope));               
                if (IsDomain(scope))
                {
                    sb.Append(this.DomainName);
                    sb.Append(this.SeparatorExternal);
        
                    try {

                        Directory.CreateDirectory(sb.ToString());

                        // No exception implies this directory was created now

                        // Create the Identity blob file in the root 
                        // directory. OK if there are more than one created
                        // last one wins

                        CreateIDFile(sb.ToString(), scope);

                    } catch {

                        // Ok to ignore IO exception

                    }

                    // For Domain Stores, accounting is done in the domain root
                    this.m_InfoFile = sb.ToString() + s_InfoFile;
                }

                sb.Append(this.AssemName);
                sb.Append(this.SeparatorExternal);

                try {

                    Directory.CreateDirectory(sb.ToString());

                    // No exception implies this directory was created now

                    // Create the Identity blob file in the root 
                    // directory. OK if there are more than one created
                    // last one wins

                    CreateIDFile(sb.ToString(), scope);

                } catch {
                    // Ok to ignore IO exception

                }

                if (IsDomain(scope))
                {
                    sb.Append(s_Files);
                }
                else
                {
                    // For Assem Stores, accounting is done in the assem root
                    this.m_InfoFile = sb.ToString() + s_InfoFile;

                    sb.Append(s_AssemFiles);
                }
            }
            sb.Append(this.SeparatorExternal);

            String rootDir = sb.ToString();

            try {
                Directory.CreateDirectory(rootDir);
            } catch {
                // Ok to ignore IO exception
            }

            this.m_RootDir = rootDir;

            // Use the "new" RootDirectory to create the permission.
            // This instance of permission is not the same as the
            // one we just asserted. It uses this.base.RootDirectory.

            m_fiop = new FileIOPermission(
                FileIOPermissionAccess.AllAccess, rootDir);
        }

        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly)]
        internal bool InitExistingStore(IsolatedStorageScope scope)
        {
            FileIOPermission fp;
            StringBuilder sb = new StringBuilder();

            sb.Append(GetRootDir(scope));

            if (IsApp(scope))
            {
                sb.Append(this.AppName);
                sb.Append(this.SeparatorExternal);
                // For App Stores, accounting is done in the app root
                this.m_InfoFile = sb.ToString() + s_AppInfoFile;

                sb.Append(s_AppFiles);
            }
            else 
            {
                if (IsDomain(scope))
                {
                    sb.Append(this.DomainName);
                    sb.Append(this.SeparatorExternal);

                    // For Domain Stores, accounting is done in the domain root
                    this.m_InfoFile = sb.ToString() + s_InfoFile;
                }

                sb.Append(this.AssemName);
                sb.Append(this.SeparatorExternal);

                if (IsDomain(scope))
                {
                    sb.Append(s_Files);
                }
                else
                {
                    // For Assem Stores, accounting is done in the assem root
                    this.m_InfoFile = sb.ToString() + s_InfoFile;

                    sb.Append(s_AssemFiles);
                }
            }
            sb.Append(this.SeparatorExternal);

            fp = new FileIOPermission(
                FileIOPermissionAccess.AllAccess, sb.ToString());

            fp.Assert();

            if (!Directory.Exists(sb.ToString()))
                return false;

            this.m_RootDir = sb.ToString();
            this.m_fiop = fp;

            return true;
        }

        protected override IsolatedStoragePermission GetPermission(
                PermissionSet ps)
        {
            if (ps == null)
                return null;
            else if (ps.IsUnrestricted())
                return new IsolatedStorageFilePermission(
                        PermissionState.Unrestricted);

            return (IsolatedStoragePermission) ps.
                    GetPermission(typeof(IsolatedStorageFilePermission));
        }

        internal void UndoReserveOperation(ulong oldLen, ulong newLen)
        {
            oldLen = RoundToBlockSize(oldLen);
            if (newLen > oldLen)
                Unreserve(RoundToBlockSize(newLen - oldLen));
        }

        internal void Reserve(ulong oldLen, ulong newLen)
        {
            oldLen = RoundToBlockSize(oldLen);
            if (newLen > oldLen)
                Reserve(RoundToBlockSize(newLen - oldLen));
        }

        internal void ReserveOneBlock()
        {
            Reserve(s_BlockSize);
        }

        internal void UnreserveOneBlock()
        {
            Unreserve(s_BlockSize);
        }

        internal static ulong RoundToBlockSize(ulong num)
        {
            if (num < s_BlockSize)
                return s_BlockSize;

            ulong rem = (num % s_BlockSize);

            if (rem != 0)
                num += (s_BlockSize - rem);

            return num;
        }

        // Helper static methods
        internal static String GetRootDir(IsolatedStorageScope scope)
        {
            if (IsRoaming(scope))
            {
                if (s_RootDirRoaming == null)
                    s_RootDirRoaming = nGetRootDir(scope);

                return s_RootDirRoaming;
            }

            if (IsMachine(scope))
            {
                if (s_RootDirMachine == null)
                    InitGlobalsMachine(scope);

                return s_RootDirMachine;    
            }

            // This is then the non-roaming user store.
            if (s_RootDirUser == null)
                InitGlobalsNonRoamingUser(scope);
                
            return s_RootDirUser;
        }

        private static void InitGlobalsMachine(IsolatedStorageScope scope)
        {
            String rootDir = nGetRootDir(scope);
            new FileIOPermission(FileIOPermissionAccess.AllAccess, rootDir).Assert();
            String rndName = GetMachineRandomDirectory(rootDir);
            if (rndName == null) {  // Create a random directory
                Mutex m = CreateMutexNotOwned(rootDir);
                if (!m.WaitOne())
                    throw new IsolatedStorageException(Environment.GetResourceString("IsolatedStorage_Init"));
                try {   // finally...
                    rndName = GetMachineRandomDirectory(rootDir);  // try again with lock
                    if (rndName == null) {
                        string relRandomDirectory1 = Path.GetRandomFileName();
                        string relRandomDirectory2 = Path.GetRandomFileName();
                        try {
                            nCreateDirectoryWithDacl(rootDir + relRandomDirectory1);
                            // Now create the root directory with the correct DACL
                            nCreateDirectoryWithDacl(rootDir + relRandomDirectory1 + "\\" + relRandomDirectory2);
                        } catch {
                            // We don't want to leak any information here
                            // Throw a store initialization exception instead
                            throw new IsolatedStorageException(Environment.GetResourceString("IsolatedStorage_Init"));
                        }
                        rndName = relRandomDirectory1 + "\\" + relRandomDirectory2;
                    }
                } finally {
                    m.ReleaseMutex();
                }
            }
            s_RootDirMachine = rootDir + rndName + "\\";
        }

        private static void InitGlobalsNonRoamingUser(IsolatedStorageScope scope)
        {
            String rootDir = null;
            // Non App Data directory case or non-App case:
            rootDir = nGetRootDir(scope);
            new FileIOPermission(FileIOPermissionAccess.AllAccess, rootDir).Assert();
            bool bMigrateNeeded = false;
            string sOldStoreLocation = null;
            String rndName = GetRandomDirectory(rootDir, out bMigrateNeeded, out sOldStoreLocation);
            if (rndName == null) {  // Create a random directory
                Mutex m = CreateMutexNotOwned(rootDir);
                if (!m.WaitOne())
                    throw new IsolatedStorageException(Environment.GetResourceString("IsolatedStorage_Init"));
                try {   // finally...
                    rndName = GetRandomDirectory(rootDir, out bMigrateNeeded, out sOldStoreLocation);  // try again with lock
                    if (rndName == null) {
                        if (bMigrateNeeded) {
                            // We have a store directory in the old format; we need to migrate it
                            rndName = MigrateOldIsoStoreDirectory(rootDir, sOldStoreLocation);
                        } else {
                            rndName = CreateRandomDirectory(rootDir);                   
                        }
                    }
                } finally {
                    m.ReleaseMutex();
                }
            }
            s_RootDirUser = rootDir + rndName + "\\";
        }

        // Migrates the old store location to a new one and returns the new location without the path separator
        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine | ResourceScope.Assembly)]
        internal static string MigrateOldIsoStoreDirectory(string rootDir, string oldRandomDirectory) {
            // First create the new random directory
            string relRandomDirectory1 = Path.GetRandomFileName();
            string relRandomDirectory2 = Path.GetRandomFileName();
            string firstRandomDirectory  = rootDir + relRandomDirectory1;
            string newRandomDirectory = firstRandomDirectory + "\\" + relRandomDirectory2;
            // Move the old directory to the new location, throw an exception and revert
            // the transaction if the operation is not successful
            try {
                // Create the first level of the new random directory
                Directory.CreateDirectory(firstRandomDirectory);
                // Move the old directory under the newly created random directory
                Directory.Move(rootDir + oldRandomDirectory, newRandomDirectory);
            } catch {
                // We don't want to leak any information here. 
                // Throw a store initialization exception instead
                throw new IsolatedStorageException(Environment.GetResourceString("IsolatedStorage_Init"));
            }
            return (relRandomDirectory1 + "\\" + relRandomDirectory2);
        }

        // creates and returns the relative path to the random directory string without the path separator
        [ResourceExposure(ResourceScope.Assembly | ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Assembly | ResourceScope.Machine)]
        internal static string CreateRandomDirectory(String rootDir) {
            string rndName = Path.GetRandomFileName() + "\\" + Path.GetRandomFileName();
            try {
                Directory.CreateDirectory(rootDir + rndName);
            } catch {
                // We don't want to leak any information here
                // Throw a store initialization exception instead
                throw new IsolatedStorageException(Environment.GetResourceString("IsolatedStorage_Init"));
            }
            return rndName;
        }

        // returns the relative path to the current random directory string if one is there without the path separator
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal static string GetRandomDirectory(String rootDir, out bool bMigrateNeeded, out string sOldStoreLocation)
        {
            // Initialize Out Parameters 
            bMigrateNeeded = false; sOldStoreLocation = null;
            String[] nodes1 = GetFileDirectoryNames(rootDir + "*", "*", false);
            // First see if there is a new store 
            for (int i=0; i<nodes1.Length; ++i) {
                if (nodes1[i].Length == 12) {
                    String[] nodes2 = GetFileDirectoryNames(rootDir + nodes1[i] + "\\" + "*", "*", false);
                    for (int j=0; j<nodes2.Length; ++j) {
                        if (nodes2[j].Length == 12) {
                            return (nodes1[i] +  "\\" + nodes2[j]); // Get the first directory
                        }
                    }
                }
            }
            // We look for directories of length 24: if we find one
            // it means we are still using the old random directory format.
            // In that case, migrate to a new store 
            for (int i=0; i<nodes1.Length; ++i) {
                if (nodes1[i].Length == 24) {
                    bMigrateNeeded = true;
                    sOldStoreLocation = nodes1[i]; // set the old store location
                    return null;
                }
            }
            // Neither old or new store formats have been encountered, return null
            return null;
        }

        // returns the relative path to the current random directory string if one is there without the path separator
        internal static string GetMachineRandomDirectory(string rootDir)
        {
            String[] nodes1 = GetFileDirectoryNames(rootDir + "*", "*", false);
            // First see if there is a new store 
            for (int i=0; i<nodes1.Length; ++i) {
                if (nodes1[i].Length == 12) {
                    String[] nodes2 = GetFileDirectoryNames(rootDir + nodes1[i] + "\\" + "*", "*", false);
                    for (int j=0; j<nodes2.Length; ++j) {
                        if (nodes2[j].Length == 12) {
                            return (nodes1[i] +  "\\" + nodes2[j]); // Get the first directory
                        }
                    }
                }
            }

            // No store has been encountered, return null
            return null;
        }

        internal static Mutex CreateMutexNotOwned(string pathName)
        {
            return new Mutex(false, "Global\\" + GetStrongHashSuitableForObjectName(pathName));
        }

        internal static String GetStrongHashSuitableForObjectName(string name)
        {
            MemoryStream ms  = new MemoryStream();
            new BinaryWriter(ms).Write(name.ToUpper(CultureInfo.InvariantCulture));
            ms.Position = 0;
            return GetHash(ms);
        }

        private String GetSyncObjectName()
        {
            if (m_SyncObjectName == null)
            {
                // Don't take a lock here,  ok to create multiple times
                m_SyncObjectName = GetStrongHashSuitableForObjectName(m_InfoFile);
            }
            return m_SyncObjectName;
        }

        internal void Lock()
        {
            if (IsRoaming())     // don't lock Roaming stores
                return;

            lock (this)
            {
                if (m_bDisposed)
                    throw new ObjectDisposedException(null, Environment.GetResourceString("IsolatedStorage_StoreNotOpen"));

                if (m_closed)
                    throw new InvalidOperationException(
                        Environment.GetResourceString(
                            "IsolatedStorage_StoreNotOpen"));

                if (m_handle == Win32Native.NULL)
                    m_handle = nOpen(m_InfoFile, GetSyncObjectName());

                nLock(m_handle, true);
            }
        }

        internal void Unlock()
        {
            if (IsRoaming())     // don't lock Roaming stores
                return;

            lock (this)
            {
                if (m_bDisposed)
                    throw new ObjectDisposedException(null, Environment.GetResourceString("IsolatedStorage_StoreNotOpen"));

                if (m_closed)
                    throw new InvalidOperationException(
                        Environment.GetResourceString(
                            "IsolatedStorage_StoreNotOpen"));

                if (m_handle == Win32Native.NULL)
                    m_handle = nOpen(m_InfoFile, GetSyncObjectName());

                nLock(m_handle, false);
            }
        }

        internal static FileIOPermission GetGlobalFileIOPerm(
                IsolatedStorageScope scope)
        {
            if (IsRoaming(scope))
            {
                // no sync needed, ok to create multiple instances.
                if (s_PermRoaming == null)
                {
                    s_PermRoaming =  new FileIOPermission(
                        FileIOPermissionAccess.AllAccess, GetRootDir(scope));
                }

                return s_PermRoaming;
            }

            if (IsMachine(scope))
            {
                // no sync needed, ok to create multiple instances.
                if (s_PermMachine == null)
                {
                    s_PermMachine =  new FileIOPermission(
                        FileIOPermissionAccess.AllAccess, GetRootDir(scope));
                }

                return s_PermMachine;
            }

            // no sync needed, ok to create multiple instances.
            if (s_PermUser == null)
            {
                s_PermUser =  new FileIOPermission(
                    FileIOPermissionAccess.AllAccess, GetRootDir(scope));
            }

            return s_PermUser;
        }

        private static void DemandAdminPermission()
        {
            // Ok if more than one instance is created, no need to sync.
            if (s_PermAdminUser == null)
            {
                s_PermAdminUser = new IsolatedStorageFilePermission(
                    IsolatedStorageContainment.AdministerIsolatedStorageByUser,
                        0, false);
            }

            s_PermAdminUser.Demand();
        }

        internal static void VerifyGlobalScope(IsolatedStorageScope scope)
        {
            if ((scope != IsolatedStorageScope.User) && 
                (scope != (IsolatedStorageScope.User|
                          IsolatedStorageScope.Roaming)) &&
                (scope != IsolatedStorageScope.Machine))
            {
                throw new ArgumentException(
                    Environment.GetResourceString(
                        "IsolatedStorage_Scope_U_R_M"));
            }
        }


        internal void CreateIDFile(String path, IsolatedStorageScope scope)
        {
            try {
                // the default DACL is fine here since we've already set it on the root
                using(FileStream fs = new FileStream(path + s_IDFile, FileMode.OpenOrCreate)) {                    
                    MemoryStream s = GetIdentityStream(scope);
                    byte[] b = s.GetBuffer();
                    fs.Write(b, 0, (int)s.Length);
                    s.Close();
                }

            } catch {
                // OK to ignore. It is possible that another thread / process
                // is writing to this file with the same data.
            }
        }

        // From IO.Directory class (make that internal if possible)
        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly)]
        private static String[] GetFileDirectoryNames(String path, String msg, bool file)
        {
            int hr;

            if (path==null) throw new ArgumentNullException("path", Environment.GetResourceString("ArgumentNull_Path"));
            
            bool fEndsWithDirectory = false;
            char lastChar = path[path.Length-1];
            if (lastChar == Path.DirectorySeparatorChar || 
                lastChar == Path.AltDirectorySeparatorChar || 
                lastChar == '.')
                fEndsWithDirectory = true;
                

            // Get an absolute path and do a security check
            String fullPath = Path.GetFullPathInternal(path);

            // GetFullPath() removes '\', "\." etc from path, we will restore 
            // it here. If path ends in a trailing slash (\), append a * 
            // or we'll  get a "Cannot find the file specified" exception
            if ((fEndsWithDirectory) && 
                (fullPath[fullPath.Length - 1] != lastChar))
               fullPath += "\\*";

            // Check for read permission to the directory, not to the contents.
            String dir = Path.GetDirectoryName(fullPath);

            if (dir != null)
                dir += "\\";
    
#if _DEBUG
            if (s_fDebug)
            {
                Console.WriteLine("path = " + path);
                Console.WriteLine("fullPath = " + fullPath);
                Console.WriteLine("dir = " + dir);
            }
#endif

            new FileIOPermission(FileIOPermissionAccess.Read, dir == null ? fullPath : dir).Demand();
            
    
            String[] list = new String[10];
            int listSize = 0;
            Win32Native.WIN32_FIND_DATA data = new Win32Native.WIN32_FIND_DATA();
                    
            // Open a Find handle 
            SafeFindHandle hnd = Win32Native.FindFirstFile(fullPath, data);
            if (hnd.IsInvalid) {
                // Calls to GetLastWin32Error overwrites HResult.  Store HResult.
                hr = Marshal.GetLastWin32Error();
                if (hr==Win32Native.ERROR_FILE_NOT_FOUND)
                    return new String[0];
                __Error.WinIOError(hr, msg);
            }
    
            // Keep asking for more matching files, adding file names to list
            int numEntries = 0;  // Number of directory entities we see.
            do {
                bool includeThis;  // Should this file/directory be included in the output?
                if (file)
                    includeThis = (0==(data.dwFileAttributes & Win32Native.FILE_ATTRIBUTE_DIRECTORY));
                else {
                    includeThis = (0!=(data.dwFileAttributes & Win32Native.FILE_ATTRIBUTE_DIRECTORY));
                    // Don't add "." nor ".."
                    if (includeThis && (data.cFileName.Equals(".") || data.cFileName.Equals(".."))) 
                        includeThis = false;
                }
                
                if (includeThis) {
                    numEntries++;
                    if (listSize==list.Length) {
                        String[] newList = new String[list.Length*2];
                        Array.Copy(list, 0, newList, 0, listSize);
                        list = newList;
                    }
                    list[listSize++] = data.cFileName;
                }
     
            } while (Win32Native.FindNextFile(hnd, data));
            
            // Make sure we quit with a sensible error.
            hr = Marshal.GetLastWin32Error();
            hnd.Close();  // Close Find handle in all cases.
            if (hr!=0 && hr!=Win32Native.ERROR_NO_MORE_FILES) __Error.WinIOError(hr, msg);
            
            // Check for a string such as "C:\tmp", in which case we return
            // just the directory name.  FindNextFile fails first time, and
            // data still contains a directory.
            if (!file && numEntries==1 && (0!=(data.dwFileAttributes & Win32Native.FILE_ATTRIBUTE_DIRECTORY))) {
                String[] sa = new String[1];
                sa[0] = data.cFileName;
                return sa;
            }
            
            // Return list of files/directories as an array of strings
            if (listSize == list.Length)
                return list;
            String[] items = new String[listSize];
            Array.Copy(list, 0, items, 0, listSize);
            return items;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern ulong nGetUsage(IntPtr handle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern IntPtr nOpen(String infoFile, String syncName);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void nClose(IntPtr handle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal unsafe static extern void nReserve(IntPtr handle, 
            ulong *plQuota, ulong *plReserve, bool fFree);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern String nGetRootDir(IsolatedStorageScope scope);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void nLock(IntPtr handle, bool fLock);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void nCreateDirectoryWithDacl(string path);
                
    }

    internal sealed class IsolatedStorageFileEnumerator : IEnumerator
    {
        private static readonly char s_SepExternal = System.IO.Path.DirectorySeparatorChar;

        private IsolatedStorageFile  m_Current;
        private IsolatedStorageScope m_Scope;
        private FileIOPermission     m_fiop;
        private String               m_rootDir;

        private TwoLevelFileEnumerator  m_fileEnum;
        private bool                    m_fReset;
        private bool                    m_fEnd;

#if _DEBUG
        private static bool s_fDebug;
#endif

        internal IsolatedStorageFileEnumerator(IsolatedStorageScope scope) 
        {
            m_Scope    = scope;
            m_fiop     = IsolatedStorageFile.GetGlobalFileIOPerm(scope);
            m_rootDir  = IsolatedStorageFile.GetRootDir(scope);
            m_fileEnum = new TwoLevelFileEnumerator(m_rootDir);
            Reset();
        }

        public bool MoveNext()
        {
            IsolatedStorageFile  isf;
            IsolatedStorageScope scope;
            bool     fDomain;
            TwoPaths tp;
            Stream   domain, assem, app;
            String   domainName, assemName, appName;

            m_fiop.Assert();

            m_fReset = false;

            do {

                if (m_fileEnum.MoveNext() == false)
                {
                    m_fEnd = true;
                    break;
                }

                // Create the store
                isf = new IsolatedStorageFile();
    
                tp   = (TwoPaths) m_fileEnum.Current;
                fDomain = false;

#if _DEBUG
                if (s_fDebug)
                {
                    Console.Write(tp.Path1 + " ");
                    Console.WriteLine(tp.Path2);
                }
#endif

                if (IsolatedStorageFile.NotAssemFilesDir(tp.Path2) &&
                    IsolatedStorageFile.NotAppFilesDir(tp.Path2))
                    fDomain = true;

                // Create Roaming Store
                domain   = null; 
                assem = null;
                app = null;

                if (fDomain)
                {
                    if (!GetIDStream(tp.Path1, out domain))
                        continue;

                    if (!GetIDStream(tp.Path1 + s_SepExternal + tp.Path2, 
                            out assem))
                        continue;

                    domain.Position = 0;

                    if (IsolatedStorage.IsRoaming(m_Scope))
                        scope = IsolatedStorage.c_DomainRoaming;
                    else if (IsolatedStorage.IsMachine(m_Scope))
                        scope = IsolatedStorage.c_MachineDomain;
                    else
                        scope = IsolatedStorage.c_Domain;

                    domainName = tp.Path1;
                    assemName = tp.Path2;
                    appName = null;
                }
                else
                {
                    if (IsolatedStorageFile.NotAppFilesDir(tp.Path2))
                    {
                        // Assembly
                        if (!GetIDStream(tp.Path1, out assem))
                            continue;

                        if (IsolatedStorage.IsRoaming(m_Scope))
                            scope = IsolatedStorage.c_AssemblyRoaming;
                        else if(IsolatedStorage.IsMachine(m_Scope))
                            scope = IsolatedStorage.c_MachineAssembly;
                        else
                            scope = IsolatedStorage.c_Assembly;

                        domainName   = null;
                        assemName = tp.Path1;
                        appName = null;
                        assem.Position = 0;                        
                    }
                    else
                    {
                        // Application
                        if (!GetIDStream(tp.Path1, out app))
                            continue;

                        if (IsolatedStorage.IsRoaming(m_Scope))
                            scope = IsolatedStorage.c_AppUserRoaming;
                        else if(IsolatedStorage.IsMachine(m_Scope))
                            scope = IsolatedStorage.c_AppMachine;
                        else
                            scope = IsolatedStorage.c_AppUser;

                        domainName   = null;
                        assemName = null;
                        appName = tp.Path1;
                        app.Position = 0;
                    }
                        
                }



                if (!isf.InitStore(scope, domain, assem, app, domainName, assemName, appName))
                    continue;

                if (!isf.InitExistingStore(scope))
                    continue;

                m_Current = isf;

                return true;

            } while (true);
            return false;
        }

        public Object Current 
        {
            get { 

                if (m_fReset)
                {
                    throw new InvalidOperationException(
                        Environment.GetResourceString(
                            "InvalidOperation_EnumNotStarted"));
                }
                else if (m_fEnd)
                {
                    throw new InvalidOperationException(
                        Environment.GetResourceString(
                            "InvalidOperation_EnumEnded"));
                }
    
                return (Object) m_Current; 
            }
        }

        public void Reset()
        {
            m_Current = null;
            m_fReset  = true;
            m_fEnd    = false;
            m_fileEnum.Reset();
        }

        private bool GetIDStream(String path, out Stream s)
        {
            StringBuilder sb = new StringBuilder();
            byte[]        b;

            sb.Append(m_rootDir);
            sb.Append(path);
            sb.Append(s_SepExternal);
            sb.Append(IsolatedStorageFile.s_IDFile);

            s = null;

            try {
                using(FileStream fs = new FileStream(sb.ToString(), FileMode.Open)) {
                    int length = (int) fs.Length;
                    b = new byte[length];
                    int offset = 0;
                    while(length > 0) {
                        int n = fs.Read(b, offset, length);
                        if (n == 0)
                            __Error.EndOfFile();
                        offset += n;
                        length -= n;
                    }
                }
                s = new MemoryStream(b);
            } catch {
                return false;
            }                    

            return true;
        }
    }

    internal sealed class TwoPaths
    {
        public String Path1;
        public String Path2;

    }

    // Given a directory, enumerates all subdirs of upto depth 2
    internal sealed class TwoLevelFileEnumerator : IEnumerator
    {
        private String   m_Root;
        private TwoPaths m_Current;
        private bool     m_fReset;
    
        private String[] m_RootDir;
        private int      m_nRootDir;
    
        private String[] m_SubDir;
        private int      m_nSubDir;
    
    
        public TwoLevelFileEnumerator(String root)
        {
            m_Root = root;
            Reset();
        }

        public bool MoveNext()
        {
            lock (this)
            {
                // Sepecial case the Reset State
                if (m_fReset)
                {
                    m_fReset = false;
                    return AdvanceRootDir();
                }
        
                // Don't move anything if RootDir is empty
                if (m_RootDir.Length == 0)
                    return false;
    
    
                // Get Next SubDir
    
                ++m_nSubDir;
        
                if (m_nSubDir >= m_SubDir.Length)
                {
                    m_nSubDir = m_SubDir.Length;    // to avoid wrap aournd.
                    return AdvanceRootDir();
                }
    
                UpdateCurrent();
            }
    
            return true;
        }
    

        [ResourceExposure(ResourceScope.None)]  // Scoping should be done when opening isolated storage.
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        private bool AdvanceRootDir()
        {
            ++m_nRootDir;
    
            if (m_nRootDir >= m_RootDir.Length)
            {
                m_nRootDir = m_RootDir.Length;  // to prevent wrap around
                return false;                   // We are at the very end.
            }
    
            m_SubDir = Directory.GetDirectories(m_RootDir[m_nRootDir]);

            if (m_SubDir.Length == 0)
                return AdvanceRootDir();        // recurse here.

            m_nSubDir  = 0;

            // Set m_Current
            UpdateCurrent();
    
            return true;
        }
    
        [ResourceExposure(ResourceScope.None)]  // Scoping should be done when opening isolated storage.
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        private void UpdateCurrent()
        {
            m_Current.Path1 = Path.GetFileName(m_RootDir[m_nRootDir]);
            m_Current.Path2 = Path.GetFileName(m_SubDir[m_nSubDir]);
        }
    
        public Object Current
        {
            get {
    
                if (m_fReset)
                {
                    throw new InvalidOperationException(
                        Environment.GetResourceString(
                            "InvalidOperation_EnumNotStarted"));
                }
                else if (m_nRootDir >= m_RootDir.Length)
                {
                    throw new InvalidOperationException(
                        Environment.GetResourceString(
                            "InvalidOperation_EnumEnded"));
                }
    
                return (Object) m_Current; 
            }
        }
    
        [ResourceExposure(ResourceScope.None)]  // Scoping should be done when opening isolated storage.
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        public void Reset()
        {
            m_RootDir  = null;
            m_nRootDir = -1;
    
            m_SubDir   = null;
            m_nSubDir  = -1;
    
            m_Current  = new TwoPaths();
            m_fReset   = true;
    
            m_RootDir  = Directory.GetDirectories(m_Root);
        }
    }
}

