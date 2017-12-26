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
/*=============================================================================
**
** Class: AppDomainSetup
**
** Purpose: Defines the settings that the loader uses to find assemblies in an
**          AppDomain
**
** Date: Dec 22, 2000
**
=============================================================================*/

namespace System {
    using System;
    using System.Runtime.CompilerServices;
    using System.Runtime;
    using System.Runtime.Hosting;
    using System.Text;
    using System.Threading;
    using System.Runtime.InteropServices;
    using System.Security;
    using System.Security.Permissions;
    using System.Security.Policy;
    using System.Globalization;
    using Path = System.IO.Path;
    using System.Runtime.Versioning;

    // Only statics, does not need to be marked with the serializable attribute
    [Serializable]
    [ClassInterface(ClassInterfaceType.None)]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class AppDomainSetup : IAppDomainSetup
    {
        [Serializable]
        internal enum LoaderInformation
        {
            // If you add a new value, add the corresponding property
            // to AppDomain.GetData() and SetData()'s switch statements.
            ApplicationBaseValue = LOADER_APPLICATION_BASE,
            ConfigurationFileValue = LOADER_CONFIGURATION_BASE,
            DynamicBaseValue = LOADER_DYNAMIC_BASE,
            DevPathValue = LOADER_DEVPATH,
            ApplicationNameValue = LOADER_APPLICATION_NAME,
            PrivateBinPathValue = LOADER_PRIVATE_PATH,
            PrivateBinPathProbeValue = LOADER_PRIVATE_BIN_PATH_PROBE,
            ShadowCopyDirectoriesValue = LOADER_SHADOW_COPY_DIRECTORIES,
            ShadowCopyFilesValue = LOADER_SHADOW_COPY_FILES,
            CachePathValue = LOADER_CACHE_PATH,
            LicenseFileValue = LOADER_LICENSE_FILE,
            DisallowPublisherPolicyValue = LOADER_DISALLOW_PUBLISHER_POLICY,
            DisallowCodeDownloadValue = LOADER_DISALLOW_CODE_DOWNLOAD,
            DisallowBindingRedirectsValue = LOADER_DISALLOW_BINDING_REDIRECTS,
            DisallowAppBaseProbingValue = LOADER_DISALLOW_APPBASE_PROBING,
            ConfigurationBytesValue = LOADER_CONFIGURATION_BYTES,
            LoaderMaximum = LOADER_MAXIMUM,
        }

        private string[] _Entries;
        private LoaderOptimization _LoaderOptimization;
#pragma warning disable 169
        private String _AppBase; // for compat with v1.1
#pragma warning restore 169
        [System.Runtime.Serialization.OptionalField(VersionAdded = 2)]
        private AppDomainInitializer  _AppDomainInitializer;
        [System.Runtime.Serialization.OptionalField(VersionAdded = 2)]
        private string[] _AppDomainInitializerArguments;
        [System.Runtime.Serialization.OptionalField(VersionAdded = 2)]
        private ActivationArguments _ActivationArguments;
        [System.Runtime.Serialization.OptionalField(VersionAdded = 2)]
        private byte[] _ConfigurationBytes;

        internal AppDomainSetup(AppDomainSetup copy, bool copyDomainBoundData)
        {
            string[] mine = Value;
            if(copy != null) {
                string[] other = copy.Value;
                int mineSize = _Entries.Length;
                int otherSize = other.Length;
                int size = (otherSize < mineSize) ? otherSize : mineSize;

                for (int i = 0; i < size; i++)
                    mine[i] = other[i];

                if (size < mineSize)
                {
                    for (int i = size; i < mineSize; i++)
                        mine[i] = null;
                }

                _LoaderOptimization = copy._LoaderOptimization;

                _AppDomainInitializerArguments = copy.AppDomainInitializerArguments;
                _ActivationArguments = copy.ActivationArguments;
                if (copyDomainBoundData)
                    _AppDomainInitializer = copy.AppDomainInitializer;
                else
                    _AppDomainInitializer = null;

                _ConfigurationBytes = copy.GetConfigurationBytes();
            }
            else 
                _LoaderOptimization = LoaderOptimization.NotSpecified;
        }

        public AppDomainSetup()
        { 
            _LoaderOptimization = LoaderOptimization.NotSpecified;
        }


        internal void SetupDefaultApplicationBase(string imageLocation) {
            StringBuilder config = null;

            char[] sep = {'\\', '/'};
            int i = imageLocation.LastIndexOfAny(sep);

            string appBase = null;
            if (i == -1) 
                config = new StringBuilder(imageLocation);
            else {
                appBase = imageLocation.Substring(0, i+1);
                config = new StringBuilder(imageLocation.Substring(i+1));
            }

            string appName;
            if (i == -1)
                appName = imageLocation;
            else
                appName = imageLocation.Substring(i+1);

            config.Append(AppDomainSetup.ConfigurationExtension);
            // If there was no configuration file but we built
            // the appbase from the module name then add the
            // default configuration file.
            if (config != null)
                this.ConfigurationFile = config.ToString();
            if (appBase != null)
                this.ApplicationBase = appBase;
            if (appName != null)
                this.ApplicationName = appName;
        }

        internal string[] Value
        {
            get {
                if( _Entries == null)
                    _Entries = new String[LOADER_MAXIMUM];
                return _Entries;
            }
        }

        public String ApplicationBase
        {
            [ResourceExposure(ResourceScope.Machine)]
            [ResourceConsumption(ResourceScope.Machine)]
            get {
                return VerifyDir(Value[(int) LoaderInformation.ApplicationBaseValue], false);
            }

            [ResourceExposure(ResourceScope.Machine)]
            [ResourceConsumption(ResourceScope.Machine)]
            set {
                Value[(int) LoaderInformation.ApplicationBaseValue] = NormalizePath(value, false);
            }
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        private String NormalizePath(String path, bool useAppBase)
        {
            if(path == null)
                return null;

            if (!useAppBase)
                path = System.Security.Util.URLString.PreProcessForExtendedPathRemoval(path, false);

            int len = path.Length;
            if (len == 0)
                return null;

#if !PLATFORM_UNIX
            bool UNCpath = false;
#endif // !PLATFORM_UNIX

            if ((len > 7) &&
                (String.Compare( path, 0, "file:", 0, 5, StringComparison.OrdinalIgnoreCase) == 0)) {
                int trim;
                
                if (path[6] == '\\') {
                    if ((path[7] == '\\') || (path[7] == '/')) {

                        // Don't allow "file:\\\\", because we can't tell the difference
                        // with it for "file:\\" + "\\server" and "file:\\\" + "\localpath"
                        if ( (len > 8) && 
                             ((path[8] == '\\') || (path[8] == '/')) )
                            throw new ArgumentException(Environment.GetResourceString("Argument_InvalidPathChars"));
                        
                        // file:\\\ means local path
                        else
#if !PLATFORM_UNIX
                            trim = 8;
#else
                            // For Unix platform, trim the first 7 charcaters only.
                            // Trimming the first 8 characters will cause
                            // the root path separator to be trimmed away,
                            // and the absolute local path becomes a relative local path.
                            trim = 7;
#endif // !PLATFORM_UNIX
                    }

                    // file:\\ means remote server
                    else {
                        trim = 5;
#if !PLATFORM_UNIX
                        UNCpath = true;
#endif // !PLATFORM_UNIX
                    }
                }

                // local path
                else if (path[7] == '/')
#if !PLATFORM_UNIX
                    trim = 8;
#else
                    // For Unix platform, trim the first 7 characters only.
                    // Trimming the first 8 characters will cause
                    // the root path separator to be trimmed away,
                    // and the absolute local path becomes a relative local path.
                    trim = 7;
#endif // !PLATFORM_UNIX

                // remote
                else {
                    // file://\\remote
                    if ( (len > 8) && (path[7] == '\\') && (path[8] == '\\') )
                        trim = 7;
                    else { // file://remote
                        trim = 5;
#if !PLATFORM_UNIX
                        // Create valid UNC path by changing
                        // all occurences of '/' to '\\' in path
                        System.Text.StringBuilder winPathBuilder =
                            new System.Text.StringBuilder(len);
                        for (int i = 0; i < len; i++) {
                            char c = path[i];
                            if (c == '/')
                                winPathBuilder.Append('\\');
                            else
                                winPathBuilder.Append(c);
                        }
                        path = winPathBuilder.ToString();
#endif // !PLATFORM_UNIX
                    }
#if !PLATFORM_UNIX
                    UNCpath = true;
#endif // !PLATFORM_UNIX
                }

                path = path.Substring(trim);
                len -= trim;
            }

#if !PLATFORM_UNIX
            bool localPath;

            // UNC
            if (UNCpath ||
                ( (len > 1) &&
                  ( (path[0] == '/') || (path[0] == '\\') ) &&
                  ( (path[1] == '/') || (path[1] == '\\') ) ))
                localPath = false;

            else {
                int colon = path.IndexOf(':') + 1;

                // protocol other than file:
                if ((colon != 0) &&
                    (len > colon+1) &&
                    ( (path[colon] == '/') || (path[colon] == '\\') ) &&
                    ( (path[colon+1] == '/') || (path[colon+1] == '\\') ))
                    localPath = false;

                else
                    localPath = true;
            }

            if (localPath) {
#else
            if ( (len == 1) ||
                 ( (path[0] != '/') && (path[0] != '\\') ) ) {
#endif // !PLATFORM_UNIX


                if (useAppBase &&
                    ( (len == 1) || (path[1] != ':') )) {
                    String appBase = Value[(int) LoaderInformation.ApplicationBaseValue];

                    if ((appBase == null) || (appBase.Length == 0))
                        throw new MemberAccessException(Environment.GetResourceString("AppDomain_AppBaseNotSet"));

                    StringBuilder result = new StringBuilder();

                    bool slash = false;
                    if ((path[0] == '/') || (path[0] == '\\')) {
                        String pathRoot = Path.GetPathRoot(appBase);
                        if (pathRoot.Length == 0) { // URL
                            int index = appBase.IndexOf(":/", StringComparison.Ordinal);
                            if (index == -1)
                                index = appBase.IndexOf(":\\", StringComparison.Ordinal);

                            // Get past last slashes of "url:http://"
                            int urlLen = appBase.Length;
                            for (index += 1;
                                 (index < urlLen) && ((appBase[index] == '/') || (appBase[index] == '\\'));
                                 index++);

                            // Now find the next slash to get domain name
                            for(; (index < urlLen) && (appBase[index] != '/') && (appBase[index] != '\\');
                                index++);

                            pathRoot = appBase.Substring(0, index);
                        }

                        result.Append(pathRoot);
                        slash = true;
                    }
                    else
                        result.Append(appBase);

                    // Make sure there's a slash separator (and only one)
                    int aLen = result.Length - 1;
                    if ((result[aLen] != '/') &&
                        (result[aLen] != '\\')) {
                        if (!slash) {
#if !PLATFORM_UNIX
                            if (appBase.IndexOf(":/", StringComparison.Ordinal) == -1)
                                result.Append('\\');
                            else
#endif // !PLATFORM_UNIX
                                result.Append('/');
                        }
                    }
                    else if (slash)
                        result.Remove(aLen, 1);

                    result.Append(path);
                    path = result.ToString();
                }
                else
                    path = Path.GetFullPathInternal(path);
            }

            return path;
        }

        private bool IsFilePath(String path)
        {
#if !PLATFORM_UNIX
            return (path[1] == ':') || ( (path[0] == '\\') && (path[1] == '\\') );
#else
            return (path[0] == '/');
#endif // !PLATFORM_UNIX
        }

        internal static String ApplicationBaseKey
        {
            get {
                return ACTAG_APP_BASE_URL;
            }
        }

        public String ConfigurationFile
        {
            get {
                return VerifyDir(Value[(int) LoaderInformation.ConfigurationFileValue], true);
            }

            set {
                Value[(int) LoaderInformation.ConfigurationFileValue] = value;
            }
        }

        // Used by the ResourceManager internally.  This must not do any 
        // security checks to avoid infinite loops.
        internal String ConfigurationFileInternal
        {
            get {
                return NormalizePath(Value[(int) LoaderInformation.ConfigurationFileValue], true);
            }
        }

        internal static String ConfigurationFileKey
        {
            get {
                return ACTAG_APP_CONFIG_FILE;
            }
        }

        public byte[] GetConfigurationBytes()
        {
            if (_ConfigurationBytes == null)
                return null;

            return (byte[]) _ConfigurationBytes.Clone();
        }

        public void SetConfigurationBytes(byte[] value)
        {
            _ConfigurationBytes = value;
        }

        private static String ConfigurationBytesKey
        {
            get {
                return ACTAG_APP_CONFIG_BLOB;
            }
        }

        public String DynamicBase
        {
            get {
                return VerifyDir(Value[(int) LoaderInformation.DynamicBaseValue], true);
            }

            set {
                if (value == null)
                    Value[(int) LoaderInformation.DynamicBaseValue] = null;
                else {
                    if(ApplicationName == null)
                        throw new MemberAccessException(Environment.GetResourceString("AppDomain_RequireApplicationName"));
                    
                    StringBuilder s = new StringBuilder( NormalizePath(value, false) );
                    s.Append('\\');
                    string h = ParseNumbers.IntToString(ApplicationName.GetHashCode(),
                                                        16, 8, '0', ParseNumbers.PrintAsI4);
                    s.Append(h);
                    
                    Value[(int) LoaderInformation.DynamicBaseValue] = s.ToString();
                }
            }
        }

        internal static String DynamicBaseKey
        {
            get {
                return ACTAG_APP_DYNAMIC_BASE;
            }
        }


        public bool DisallowPublisherPolicy
        {
            get 
            {
                return (Value[(int) LoaderInformation.DisallowPublisherPolicyValue] != null);
            }
            set
            {
                if (value)
                    Value[(int) LoaderInformation.DisallowPublisherPolicyValue]="true";
                else
                    Value[(int) LoaderInformation.DisallowPublisherPolicyValue]=null;
            }
        }


        public bool DisallowBindingRedirects
        {
            get 
            {
                return (Value[(int) LoaderInformation.DisallowBindingRedirectsValue] != null);
            }
            set
            {
                if (value)
                    Value[(int) LoaderInformation.DisallowBindingRedirectsValue] = "true";
                else
                    Value[(int) LoaderInformation.DisallowBindingRedirectsValue] = null;
            }
        }

        public bool DisallowCodeDownload
        {
            get 
            {
                return (Value[(int) LoaderInformation.DisallowCodeDownloadValue] != null);
            }
            set
            {
                if (value)
                    Value[(int) LoaderInformation.DisallowCodeDownloadValue] = "true";
                else
                    Value[(int) LoaderInformation.DisallowCodeDownloadValue] = null;
            }
        }


        public bool DisallowApplicationBaseProbing
        {
            get 
            {
                return (Value[(int) LoaderInformation.DisallowAppBaseProbingValue] != null);
            }
            set
            {
                if (value)
                    Value[(int) LoaderInformation.DisallowAppBaseProbingValue] = "true";
                else
                    Value[(int) LoaderInformation.DisallowAppBaseProbingValue] = null;
            }
        }

        private String VerifyDir(String dir, bool normalize)
        {
            if (dir != null) {
                if (dir.Length == 0)
                    dir = null;
                else {
                    if (normalize)
                        dir = NormalizePath(dir, true);
                    if (IsFilePath(dir))
                        new FileIOPermission( FileIOPermissionAccess.PathDiscovery, dir ).Demand();
                }
            }

            return dir;
        }

        private void VerifyDirList(String dirs)
        {
            if (dirs != null) {
                String[] dirArray = dirs.Split(';');
                int len = dirArray.Length;
                
                for (int i = 0; i < len; i++)
                    VerifyDir(dirArray[i], true);
            }
        }

        internal String DeveloperPath
        {
            [ResourceExposure(ResourceScope.Machine)]
            [ResourceConsumption(ResourceScope.Machine)]
            get {
                String dirs = Value[(int) LoaderInformation.DevPathValue];
                VerifyDirList(dirs);
                return dirs;
            }

            [ResourceExposure(ResourceScope.Machine)]
            [ResourceConsumption(ResourceScope.Machine)]
            set {
                if(value == null)
                    Value[(int) LoaderInformation.DevPathValue] = null;
                else {
                    String[] directories = value.Split(';');
                    int size = directories.Length;
                    StringBuilder newPath = new StringBuilder();
                    bool fDelimiter = false;
                        
                    for(int i = 0; i < size; i++) {
                        if(directories[i].Length != 0) {
                            if(fDelimiter) 
                                newPath.Append(";");
                            else
                                fDelimiter = true;
                            
                            newPath.Append(Path.GetFullPathInternal(directories[i]));
                        }
                    }
                    
                    String newString = newPath.ToString();
                    if (newString.Length == 0)
                        Value[(int) LoaderInformation.DevPathValue] = null;
                    else
                        Value[(int) LoaderInformation.DevPathValue] = newPath.ToString();
                }
            }
        }
        
        internal static String DisallowPublisherPolicyKey
        {
            get
            {
                return ACTAG_DISALLOW_APPLYPUBLISHERPOLICY;
            }
        }

        internal static String DisallowCodeDownloadKey
        {
            get
            {
                return ACTAG_CODE_DOWNLOAD_DISABLED;
            }
        }

        internal static String DisallowBindingRedirectsKey
        {
            get
            {
                return ACTAG_DISALLOW_APP_BINDING_REDIRECTS;
            }
        }

        internal static String DeveloperPathKey
        {
            get {
                return ACTAG_DEV_PATH;
            }
        }

        internal static String DisallowAppBaseProbingKey
        {
            get
            {
                return ACTAG_DISALLOW_APP_BASE_PROBING;
            }
        }

        public String ApplicationName
        {
            get {
                return Value[(int) LoaderInformation.ApplicationNameValue];
            }

            set {
                Value[(int) LoaderInformation.ApplicationNameValue] = value;
            }
        }

        internal static String ApplicationNameKey
        {
            get {
                return ACTAG_APP_NAME;
            }
        }

        [XmlIgnoreMember]
        public AppDomainInitializer AppDomainInitializer
        {
            get {
                return _AppDomainInitializer;
            }

            set {
                _AppDomainInitializer = value;
            }
        }
        public string[] AppDomainInitializerArguments
        {
            get {
                return _AppDomainInitializerArguments;
            }

            set {
                _AppDomainInitializerArguments = value;
            }
        }

        [XmlIgnoreMember]
        public ActivationArguments ActivationArguments {
            get {
                return _ActivationArguments;
            }
            set {
                lock(this) {
                    _ActivationArguments = value;
                }
            }
        }


        public String PrivateBinPath
        {
            get {
                String dirs = Value[(int) LoaderInformation.PrivateBinPathValue];
                VerifyDirList(dirs);
                return dirs;
            }

            set {
                Value[(int) LoaderInformation.PrivateBinPathValue] = value;
            }
        }

        internal static String PrivateBinPathKey
        {
            get {
                return ACTAG_APP_PRIVATE_BINPATH;
            }
        }


        public String PrivateBinPathProbe
        {
            get {
                return Value[(int) LoaderInformation.PrivateBinPathProbeValue];
            }

            set {
                Value[(int) LoaderInformation.PrivateBinPathProbeValue] = value;
            }
        }

        internal static String PrivateBinPathProbeKey
        {
            get {
                return ACTAG_BINPATH_PROBE_ONLY;
            }
        }

        public String ShadowCopyDirectories
        {
            get {
                String dirs = Value[(int) LoaderInformation.ShadowCopyDirectoriesValue];
                VerifyDirList(dirs);
                return dirs;
            }

            set {
                Value[(int) LoaderInformation.ShadowCopyDirectoriesValue] = value;
            }
        }

        internal static String ShadowCopyDirectoriesKey
        {
            get {
                return ACTAG_APP_SHADOW_COPY_DIRS;
            }
        }

        public String ShadowCopyFiles
        {
            get {
                return Value[(int) LoaderInformation.ShadowCopyFilesValue];
            }

            set {
                if((value != null) && 
                   (String.Compare(value, "true", StringComparison.OrdinalIgnoreCase) == 0))
                    Value[(int) LoaderInformation.ShadowCopyFilesValue] = value;
                else
                    Value[(int) LoaderInformation.ShadowCopyFilesValue] = null;
            }
        }

        internal static String ShadowCopyFilesKey
        {
            get {
                return ACTAG_FORCE_CACHE_INSTALL;
            }
        }

        public String CachePath
        {
            get {
                return VerifyDir(Value[(int) LoaderInformation.CachePathValue], false);
            }

            set {
                Value[(int) LoaderInformation.CachePathValue] = NormalizePath(value, false);
            }
        }

        internal static String CachePathKey
        {
            get {
                return ACTAG_APP_CACHE_BASE;
            }
        }

        public String LicenseFile
        {
            get {
                return VerifyDir(Value[(int) LoaderInformation.LicenseFileValue], true);
            }

            set {
                Value[(int) LoaderInformation.LicenseFileValue] = value;
            }
        }

        public LoaderOptimization LoaderOptimization
        {
            get {
                return _LoaderOptimization;
            }

            set {
                _LoaderOptimization = value;
            }
        }

        internal static string LoaderOptimizationKey
        {
            get {
                return LOADER_OPTIMIZATION;
            }
        }

        internal static string ConfigurationExtension
        {
            get {
                return CONFIGURATION_EXTENSION;
            }
        }

        internal static String PrivateBinPathEnvironmentVariable
        {
            get {
                return APPENV_RELATIVEPATH;
            }
        }

        internal static string RuntimeConfigurationFile
        {
            get {
                return MACHINE_CONFIGURATION_FILE;
            }
        }

        internal static string MachineConfigKey
        {
            get {
                return ACTAG_MACHINE_CONFIG;
            }
        }

        internal static string HostBindingKey
        {
            get {
                return ACTAG_HOST_CONFIG_FILE;
            }
        }

        internal void SetupFusionContext(IntPtr fusionContext)
        {
            String appbase = Value[(int) LoaderInformation.ApplicationBaseValue];
            if(appbase != null)
                UpdateContextProperty(fusionContext, ApplicationBaseKey, appbase);

            String privBinPath = Value[(int) LoaderInformation.PrivateBinPathValue];
            if(privBinPath != null)
                UpdateContextProperty(fusionContext, PrivateBinPathKey, privBinPath);

            String devpath = Value[(int) LoaderInformation.DevPathValue];
            if(devpath != null)
                UpdateContextProperty(fusionContext, DeveloperPathKey, devpath);

            if (DisallowPublisherPolicy)
                UpdateContextProperty(fusionContext, DisallowPublisherPolicyKey, "true");

            if (DisallowCodeDownload)
                UpdateContextProperty(fusionContext, DisallowCodeDownloadKey, "true");

            if (DisallowBindingRedirects)
                UpdateContextProperty(fusionContext, DisallowBindingRedirectsKey, "true");

            if (DisallowApplicationBaseProbing)
                UpdateContextProperty(fusionContext, DisallowAppBaseProbingKey, "true");

            if(ShadowCopyFiles != null) {
                UpdateContextProperty(fusionContext, ShadowCopyFilesKey, ShadowCopyFiles);

                // If we are asking for shadow copy directories then default to
                // only to the ones that are in the private bin path.
                if(Value[(int) LoaderInformation.ShadowCopyDirectoriesValue] == null)
                    ShadowCopyDirectories = BuildShadowCopyDirectories();

                String shadowDirs = Value[(int) LoaderInformation.ShadowCopyDirectoriesValue];
                if(shadowDirs != null)
                    UpdateContextProperty(fusionContext, ShadowCopyDirectoriesKey, shadowDirs);
            }

            String cache = Value[(int) LoaderInformation.CachePathValue];
            if(cache != null)
                UpdateContextProperty(fusionContext, CachePathKey, cache);

            if(PrivateBinPathProbe != null)
                UpdateContextProperty(fusionContext, PrivateBinPathProbeKey, PrivateBinPathProbe); 

            String config = Value[(int) LoaderInformation.ConfigurationFileValue];
            if (config != null)
                UpdateContextProperty(fusionContext, ConfigurationFileKey, config);

            if (_ConfigurationBytes != null)
                UpdateContextProperty(fusionContext, ConfigurationBytesKey, _ConfigurationBytes);

            if(ApplicationName != null)
                UpdateContextProperty(fusionContext, ApplicationNameKey, ApplicationName);

            String dynbase = Value[(int) LoaderInformation.DynamicBaseValue];
            if(dynbase != null)
                UpdateContextProperty(fusionContext, DynamicBaseKey, dynbase);

            // Always add the runtime configuration file to the appdomain
            StringBuilder configFile = new StringBuilder();
            configFile.Append(RuntimeEnvironment.GetRuntimeDirectoryImpl());
            configFile.Append(RuntimeConfigurationFile);
            UpdateContextProperty(fusionContext, MachineConfigKey, configFile.ToString());

            String hostBindingFile = RuntimeEnvironment.GetHostBindingFile();
            if(hostBindingFile != null) 
                UpdateContextProperty(fusionContext, HostBindingKey, hostBindingFile);   
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void UpdateContextProperty(IntPtr fusionContext, string key, Object value);

        static internal int Locate(String s)
        {
            if(String.IsNullOrEmpty(s))
                return -1;

            // verify assumptions hardcoded into the switch below
            BCLDebug.Assert('A' == ACTAG_APP_CONFIG_FILE[0]     , "Assumption violated");
            BCLDebug.Assert('A' == ACTAG_APP_NAME[0]            , "Assumption violated");
            BCLDebug.Assert('A' == ACTAG_APP_BASE_URL[0]        , "Assumption violated");
            BCLDebug.Assert('B' == ACTAG_BINPATH_PROBE_ONLY[0]  , "Assumption violated");
            BCLDebug.Assert('C' == ACTAG_APP_CACHE_BASE[0]      , "Assumption violated");
            BCLDebug.Assert('D' == ACTAG_DEV_PATH[0]            , "Assumption violated");
            BCLDebug.Assert('D' == ACTAG_APP_DYNAMIC_BASE[0]    , "Assumption violated");
            BCLDebug.Assert('F' == ACTAG_FORCE_CACHE_INSTALL[0] , "Assumption violated");
            BCLDebug.Assert('L' == LICENSE_FILE[0]              , "Assumption violated");
            BCLDebug.Assert('P' == ACTAG_APP_PRIVATE_BINPATH[0] , "Assumption violated");
            BCLDebug.Assert('S' == ACTAG_APP_SHADOW_COPY_DIRS[0], "Assumption violated");
            BCLDebug.Assert('D' == ACTAG_DISALLOW_APPLYPUBLISHERPOLICY[0], "Assumption violated");
            BCLDebug.Assert('C' == ACTAG_CODE_DOWNLOAD_DISABLED[0], "Assumption violated");
            BCLDebug.Assert('D' == ACTAG_DISALLOW_APP_BINDING_REDIRECTS[0], "Assumption violated");
            BCLDebug.Assert('D' == ACTAG_DISALLOW_APP_BASE_PROBING[0], "Assumption violated");
            BCLDebug.Assert('A' == ACTAG_APP_CONFIG_BLOB[0], "Assumption violated");

            switch (s[0]) {
                case 'A':
                    if (s == ACTAG_APP_CONFIG_FILE)     return (int)LoaderInformation.ConfigurationFileValue;
                    if (s == ACTAG_APP_NAME)            return (int)LoaderInformation.ApplicationNameValue;
                    if (s == ACTAG_APP_BASE_URL)        return (int)LoaderInformation.ApplicationBaseValue;
                    if (s == ACTAG_APP_CONFIG_BLOB)     return (int)LoaderInformation.ConfigurationBytesValue;
                    break;
                case 'B':
                    if (s == ACTAG_BINPATH_PROBE_ONLY)  return (int)LoaderInformation.PrivateBinPathProbeValue;
                    break;
                case 'C':
                    if (s == ACTAG_APP_CACHE_BASE)      return (int)LoaderInformation.CachePathValue;
                    if (s == ACTAG_CODE_DOWNLOAD_DISABLED) return (int)LoaderInformation.DisallowCodeDownloadValue;
                    break;
                case 'D':
                    if (s == ACTAG_DEV_PATH)            return (int)LoaderInformation.DevPathValue;
                    if (s == ACTAG_APP_DYNAMIC_BASE)    return (int)LoaderInformation.DynamicBaseValue;
                    if (s == ACTAG_DISALLOW_APPLYPUBLISHERPOLICY) return (int)LoaderInformation.DisallowPublisherPolicyValue;
                    if (s == ACTAG_DISALLOW_APP_BINDING_REDIRECTS) return (int)LoaderInformation.DisallowBindingRedirectsValue;
                    if (s == ACTAG_DISALLOW_APP_BASE_PROBING) return (int)LoaderInformation.DisallowAppBaseProbingValue;
                   break;
                case 'F':
                    if (s == ACTAG_FORCE_CACHE_INSTALL) return (int)LoaderInformation.ShadowCopyFilesValue;
                    break;
                case 'L':
                    if (s == LICENSE_FILE)              return (int)LoaderInformation.LicenseFileValue;
                    break;
                case 'P':
                    if (s == ACTAG_APP_PRIVATE_BINPATH) return (int)LoaderInformation.PrivateBinPathValue;
                    break;
                case 'S':
                    if (s == ACTAG_APP_SHADOW_COPY_DIRS) return (int)LoaderInformation.ShadowCopyDirectoriesValue;
                    break;
            }

            return -1;
        }

        private string BuildShadowCopyDirectories()
        {
            // Default to only to the ones that are in the private bin path.
            String binPath = Value[(int) LoaderInformation.PrivateBinPathValue];
            if(binPath == null)
                return null;

            StringBuilder result = new StringBuilder();
            String appBase = Value[(int) LoaderInformation.ApplicationBaseValue];
            if(appBase != null) {
                char[] sep = {';'};
                string[] directories = binPath.Split(sep);
                int size = directories.Length;
                bool appendSlash = !( (appBase[appBase.Length-1] == '/') ||
                                      (appBase[appBase.Length-1] == '\\') );

                if (size == 0) {
                    result.Append(appBase);
                    if (appendSlash)
                        result.Append('\\');
                    result.Append(binPath);
                }
                else {
                    for(int i = 0; i < size; i++) {
                        result.Append(appBase);
                        if (appendSlash)
                            result.Append('\\');
                        result.Append(directories[i]);
                        
                        if (i < size-1)
                            result.Append(';');
                    }
                }
            }
            
            return result.ToString();
        }
    }
}
