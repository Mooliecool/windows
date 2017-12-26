//------------------------------------------------------------------------------
// <copyright file="Configuration.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

using ClassConfiguration = System.Configuration.Configuration;
using System.Collections;
using System.Configuration;
using System.Configuration.Internal;
using System.IO;
using System.Reflection;
using System.Security;
using System.Security.Permissions;
using System.Threading;

namespace System.Configuration {

    //
    // An instance of the Configuration class represents a single level
    // in the configuration hierarchy. Its contents can be edited and
    // saved to disk. 
    //
    // It is not thread safe for writing.
    //
    public sealed class Configuration {
        private Type                                _typeConfigHost;                // type of config host
        private object[]                            _hostInitConfigurationParams;   // params to init config host
        private IInternalConfigRoot                 _configRoot;        // root of this configuration hierarchy
        private MgmtConfigurationRecord             _configRecord;      // config record for this level in the hierarchy
        private ConfigurationSectionGroup           _rootSectionGroup;  // section group for the root of all sections
        private ConfigurationLocationCollection     _locations;         // support for ConfigurationLocationsCollection
        private ContextInformation                  _evalContext;       // evaluation context

        internal Configuration(string locationSubPath, Type typeConfigHost, params object[] hostInitConfigurationParams) {
            _typeConfigHost = typeConfigHost;
            _hostInitConfigurationParams = hostInitConfigurationParams;

            _configRoot = new InternalConfigRoot();

            IInternalConfigHost configHost = (IInternalConfigHost) TypeUtil.CreateInstanceWithReflectionPermission(typeConfigHost);

            // Wrap the host with the UpdateConfigHost to support SaveAs.
            IInternalConfigHost updateConfigHost = new UpdateConfigHost(configHost);

            _configRoot.Init(updateConfigHost, true);

            //
            // Set the configuration paths for this Configuration.
            // We do this in a separate step so that the WebConfigurationHost
            // can use this object's _configRoot to get the <sites> section,
            // which is used in it's MapPath implementation.
            //
            string configPath, locationConfigPath;
            configHost.InitForConfiguration(ref locationSubPath, out configPath, out locationConfigPath, _configRoot, hostInitConfigurationParams);

            if (!String.IsNullOrEmpty(locationSubPath) && !updateConfigHost.SupportsLocation) {
                throw ExceptionUtil.UnexpectedError("Configuration::ctor");
            }

            if (String.IsNullOrEmpty(locationSubPath) != String.IsNullOrEmpty(locationConfigPath)) {
                throw ExceptionUtil.UnexpectedError("Configuration::ctor");
            }

            // Get the configuration record for this config file.
            _configRecord = (MgmtConfigurationRecord) _configRoot.GetConfigRecord(configPath);

            //
            // Create another MgmtConfigurationRecord for the location that is a child of the above record.
            // Note that this does not match the resolution hiearchy that is used at runtime.
            //
            if (!String.IsNullOrEmpty(locationSubPath)) {
                _configRecord = MgmtConfigurationRecord.Create(
                    _configRoot, _configRecord, locationConfigPath, locationSubPath);
            }

            //
            // Throw if the config record we created contains global errors.
            //
            _configRecord.ThrowIfInitErrors();
        }

        //
        // Create a new instance of Configuration for the locationSubPath, 
        // with the initialization parameters that were used to create this configuration.
        //
        internal Configuration OpenLocationConfiguration(string locationSubPath) {
            return new Configuration(locationSubPath, _typeConfigHost, _hostInitConfigurationParams);
        }

        // public properties
        public AppSettingsSection AppSettings {
            get {
                return (AppSettingsSection) GetSection("appSettings");
            }
        }

        public ConnectionStringsSection ConnectionStrings {
            get {
                return (ConnectionStringsSection) GetSection("connectionStrings");
            }
        }

        public string FilePath {
            get {
                return _configRecord.ConfigurationFilePath;
            }
        }

        public bool HasFile {
            get {
                return _configRecord.HasStream;
            }
        }

        public ConfigurationLocationCollection Locations {
            get {
                if (_locations == null) {
                    _locations = _configRecord.GetLocationCollection(this);
                }

                return _locations;
            }
        }

        public ContextInformation EvaluationContext {
            get {
                if (_evalContext == null) {
                    _evalContext = new ContextInformation(_configRecord);
                }

                return _evalContext;
            }
        }

        public ConfigurationSectionGroup RootSectionGroup {
            get {
                if (_rootSectionGroup == null) {
                    _rootSectionGroup = new ConfigurationSectionGroup();
                    _rootSectionGroup.RootAttachToConfigurationRecord(_configRecord);
                }

                return _rootSectionGroup;
            }
        }

        public ConfigurationSectionCollection Sections {
            get {
                return RootSectionGroup.Sections;
            }
        }

        public ConfigurationSectionGroupCollection SectionGroups {
            get {
                return RootSectionGroup.SectionGroups;
            }
        }

        // public methods
        public ConfigurationSection GetSection(string sectionName) {
            ConfigurationSection section = (ConfigurationSection) _configRecord.GetSection(sectionName);

            return section;
        }

        public ConfigurationSectionGroup GetSectionGroup(string sectionGroupName) {
            ConfigurationSectionGroup sectionGroup = _configRecord.GetSectionGroup(sectionGroupName);

            return sectionGroup;
        }

        // NamespaceDeclared
        //
        // Is the namespace declared in the file or not?
        //
        // ie. xmlns="http://schemas.microsoft.com/.NetConfiguration/v2.0"
        //     (currently this is the only one we allow)
        //
        // get - Return if it was declared in the file.
        // set - Set if we should save the namespace or not
        //
        public bool NamespaceDeclared {
            get {
                return _configRecord.NamespacePresent;
            }
            set {
                _configRecord.NamespacePresent = value;
            }
        }

        public void Save() {
            SaveAsImpl(null, ConfigurationSaveMode.Modified, false);
        }

        public void Save(ConfigurationSaveMode saveMode) {
            SaveAsImpl(null, saveMode, false);
        }

        public void Save(ConfigurationSaveMode saveMode, bool forceSaveAll) {
            SaveAsImpl(null, saveMode, forceSaveAll);
        }

        public void SaveAs(string filename) {
            SaveAs(filename, ConfigurationSaveMode.Modified, false);
        }

        public void SaveAs(string filename, ConfigurationSaveMode saveMode) {
            SaveAs(filename, saveMode, false);
        }

        public void SaveAs(string filename, ConfigurationSaveMode saveMode, bool forceSaveAll) {
            if (String.IsNullOrEmpty(filename)) {
                throw ExceptionUtil.ParameterNullOrEmpty("filename");                
            }
            
            SaveAsImpl(filename, saveMode, forceSaveAll);
        }

        private void SaveAsImpl(string filename, ConfigurationSaveMode saveMode, bool forceSaveAll) {
            if (String.IsNullOrEmpty(filename)) {
                filename = null;
            }
            else {
                filename = System.IO.Path.GetFullPath(filename);
            }

            if (forceSaveAll) {
                ForceGroupsRecursive(RootSectionGroup);
            }

            _configRecord.SaveAs(filename, saveMode, forceSaveAll);
        }

        // Force all sections and section groups to be instantiated.
        private void ForceGroupsRecursive(ConfigurationSectionGroup group) {
            foreach (ConfigurationSection configSection in group.Sections) {
                // Force the section to be read into the cache                
                ConfigurationSection section = group.Sections[configSection.SectionInformation.Name];
            }

            foreach (ConfigurationSectionGroup sectionGroup in group.SectionGroups) {                
                ForceGroupsRecursive(group.SectionGroups[sectionGroup.Name]);
            }
        }
    }
}

