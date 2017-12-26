//------------------------------------------------------------------------------
// <copyright file="SectionInformation.cs" company="Microsoft">
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

namespace System.Configuration {
    using System;
    using System.Collections.Specialized;
    using System.Configuration.Internal;
    using System.IO;
    using System.Reflection;
    using System.Security;
    using System.Text;
    using System.Xml;
    using System.Globalization;

    public sealed class SectionInformation {
        // Flags
        private const int Flag_Attached                     = 0x00000001;
        private const int Flag_Declared                     = 0x00000002;
        private const int Flag_DeclarationRequired          = 0x00000004;
        private const int Flag_AllowLocation                = 0x00000008;
        private const int Flag_RestartOnExternalChanges     = 0x00000010;
        private const int Flag_RequirePermission            = 0x00000020;                                 
        private const int Flag_LocationLocked               = 0x00000040;                                 
        private const int Flag_AllowOverride                = 0x00000080;                                 
        private const int Flag_InheritInChildApps           = 0x00000100;                                 
        private const int Flag_IsParentSection              = 0x00000200;                                 
        private const int Flag_Removed                      = 0x00000400;                                 
        private const int Flag_ProtectionProviderDetermined = 0x00000800;                                 
        private const int Flag_ForceSave                    = 0x00001000;
        private const int Flag_IsUndeclared                 = 0x00002000;
        private const int Flag_AllowExeDefinitionModified   = 0x00010000;
        private const int Flag_AllowDefinitionModified      = 0x00020000;
        private const int Flag_ConfigSourceModified         = 0x00040000;
        private const int Flag_ProtectionProviderModified   = 0x00080000;

        private ConfigurationSection            _configurationSection;
        private SafeBitVector32                 _flags;
        private SimpleBitVector32               _modifiedFlags;
        private ConfigurationAllowDefinition    _allowDefinition;
        private ConfigurationAllowExeDefinition _allowExeDefinition;
        private MgmtConfigurationRecord         _configRecord;
        private string                          _configKey;
        private string                          _group;
        private string                          _name;
        private string                          _typeName;
        private string                          _rawXml;
        private string                          _configSource;
        private string                          _configSourceStreamName;
        private ProtectedConfigurationProvider  _protectionProvider;
        private string                          _protectionProviderName;

        //
        // Constructor
        //
        internal SectionInformation(ConfigurationSection associatedConfigurationSection) {
            _configKey = String.Empty;
            _group = String.Empty;
            _name = String.Empty;

            _configurationSection     = associatedConfigurationSection;
            _allowDefinition          = ConfigurationAllowDefinition.Everywhere;
            _allowExeDefinition       = ConfigurationAllowExeDefinition.MachineToApplication;

            _flags[ Flag_AllowLocation            ] = true;
            _flags[ Flag_RestartOnExternalChanges ] = true;
            _flags[ Flag_RequirePermission        ] = true;
            _flags[ Flag_AllowOverride            ] = true;
            _flags[ Flag_InheritInChildApps       ] = true;
            _flags[ Flag_ForceSave                ] = false;

            _modifiedFlags = new SimpleBitVector32();
        }

        internal void ResetModifiedFlags() {
            _modifiedFlags = new SimpleBitVector32();
        }

        internal bool IsModifiedFlags() {
            return (_modifiedFlags.Data != 0);
        }
        // for instantiation of a ConfigurationSection from GetConfig
        internal void AttachToConfigurationRecord(MgmtConfigurationRecord configRecord, FactoryRecord factoryRecord, SectionRecord sectionRecord) {
            SetRuntimeConfigurationInformation(configRecord, factoryRecord, sectionRecord);
            _configRecord = configRecord;
        }

        internal void SetRuntimeConfigurationInformation(BaseConfigurationRecord configRecord, FactoryRecord factoryRecord, SectionRecord sectionRecord) {
            _flags[ Flag_Attached ] = true;

            // factory info
            _configKey          = factoryRecord.ConfigKey;
            _group              = factoryRecord.Group;
            _name               = factoryRecord.Name;
            _typeName           = factoryRecord.FactoryTypeName;
            _allowDefinition    = factoryRecord.AllowDefinition;
            _allowExeDefinition = factoryRecord.AllowExeDefinition;
            _flags[ Flag_AllowLocation            ] = factoryRecord.AllowLocation;
            _flags[ Flag_RestartOnExternalChanges ] = factoryRecord.RestartOnExternalChanges;
            _flags[ Flag_RequirePermission        ] = factoryRecord.RequirePermission;

            if (factoryRecord.IsUndeclared) {
                _flags[ Flag_IsUndeclared             ] = true;
                _flags[ Flag_Declared                 ] = false;
                _flags[ Flag_DeclarationRequired      ] = false;
            }
            else {
                _flags[ Flag_IsUndeclared             ] = false;
                _flags[ Flag_Declared                 ] = configRecord.GetFactoryRecord(factoryRecord.ConfigKey, false) != null;
                _flags[ Flag_DeclarationRequired      ] = configRecord.IsRootDeclaration(factoryRecord.ConfigKey, false);
            }

            // section info
            _flags[ Flag_LocationLocked ]           = sectionRecord.Locked;

            if (sectionRecord.HasFileInput) {
                SectionInput fileInput = sectionRecord.FileInput;

                _flags[ Flag_ProtectionProviderDetermined ] = fileInput.IsProtectionProviderDetermined;
                _protectionProvider                         = fileInput.ProtectionProvider;
                
                SectionXmlInfo sectionXmlInfo = fileInput.SectionXmlInfo;

                _configSource                       = sectionXmlInfo.ConfigSource;
                _configSourceStreamName             = sectionXmlInfo.ConfigSourceStreamName;
                _flags[ Flag_AllowOverride ]        = !sectionXmlInfo.LockChildren;
                _flags[ Flag_InheritInChildApps ]   = !sectionXmlInfo.SkipInChildApps;
                _protectionProviderName             = sectionXmlInfo.ProtectionProviderName;
            }
            else {
                _flags[ Flag_ProtectionProviderDetermined ] = false;
                _protectionProvider = null;
            }

            // element context information
            _configurationSection.AssociateContext( configRecord );
        }

        internal void DetachFromConfigurationRecord() {
            RevertToParent();
            _flags[ Flag_Attached ] = false;
            _configRecord = null;
        }

        private bool IsRuntime {
            get {
                return ( _flags[ Flag_Attached ] ) && 
                       ( _configRecord == null   );
            }
        }

        internal bool Attached {
            get {return _flags[ Flag_Attached ];}
        }

        private void VerifyDesigntime() {
            if (IsRuntime) {
                throw new InvalidOperationException(SR.GetString(SR.Config_operation_not_runtime));
            }
        }

        private void VerifyIsAttachedToConfigRecord() {
            if (_configRecord == null) {
                throw new InvalidOperationException(SR.GetString(SR.Config_cannot_edit_configurationsection_when_not_attached));
            }
        }

        // VerifyIsEditable 
        //
        // Verify the section is Editable.  
        // It may not be editable for the following reasons:
        //   - We are in Runtime mode, not Design time
        //   - The section is not attached to a _configRecord.
        //   - We are locked (ie. allowOveride = false )
        //   - We are a parent section (ie. Retrieved from GetParentSection)
        //
        internal void VerifyIsEditable() {
            VerifyDesigntime();
            if (IsLocked) {
                throw new InvalidOperationException(SR.GetString(SR.Config_cannot_edit_configurationsection_when_locked));
            }

            if (_flags[Flag_IsParentSection]) {
                throw new InvalidOperationException(SR.GetString(SR.Config_cannot_edit_configurationsection_parentsection));
            }

            if ( !_flags[ Flag_AllowLocation ] &&
                 ( _configRecord != null ) && 
                 ( _configRecord.IsLocationConfig ) ) {
                throw new InvalidOperationException(SR.GetString(SR.Config_cannot_edit_configurationsection_when_location_locked));
            }
        }

        // VerifyNotParentSection
        //
        // Verify that this is not a parent section
        //
        private void VerifyNotParentSection() {
            if ( _flags[Flag_IsParentSection] ) {
                throw new InvalidOperationException(SR.GetString(SR.Config_configsection_parentnotvalid));
           }
        }
        
        // VerifySupportsLocation
        //
        // Verify that we support the location tag.  This is true either
        // in machine.config, or in any place for the web config system
        //
        private void VerifySupportsLocation() {
            if ( ( _configRecord != null ) &&
                 !_configRecord.RecordSupportsLocation ) {
                throw new InvalidOperationException(SR.GetString(SR.Config_cannot_edit_locationattriubtes));
            }
        }

        internal void VerifyIsEditableFactory() {
            if (_configRecord != null && _configRecord.IsLocationConfig) {
                throw new InvalidOperationException(SR.GetString(SR.Config_cannot_edit_configurationsection_in_location_config));
            }

            // Can't edit factory if the section is built-in
            if (BaseConfigurationRecord.IsImplicitSection(ConfigKey)) {
                throw new InvalidOperationException(SR.GetString(SR.Config_cannot_edit_configurationsection_when_it_is_implicit));
            }

            // Can't edit undeclared section
            if (_flags[Flag_IsUndeclared]) {
                throw new InvalidOperationException(SR.GetString(SR.Config_cannot_edit_configurationsection_when_it_is_undeclared));
            }
        }

        internal string ConfigKey {
            get {return _configKey;}
        }

        internal bool Removed {
            get {
                return _flags[ Flag_Removed ];
            }
            set {
                _flags[ Flag_Removed ] = value;
            }
        }

        private FactoryRecord FindParentFactoryRecord(bool permitErrors) {
            FactoryRecord factoryRecord = null;

            if (_configRecord != null && !_configRecord.Parent.IsRootConfig) {
                factoryRecord = _configRecord.Parent.FindFactoryRecord(_configKey, permitErrors);
            }

            return factoryRecord;
        }

        //
        // public properties and methods
        //
        public string SectionName {
            get {return _configKey;}
        }

        public string Name {
            get {return _name;}
        }

        public ConfigurationAllowDefinition AllowDefinition {
            get {return _allowDefinition;}
            set {
                VerifyIsEditable();
                VerifyIsEditableFactory();

                // allow AllowDefinition to be different from current type,
                // so long as it doesn't conflict with a type already defined
                FactoryRecord factoryRecord = FindParentFactoryRecord(false);
                if (factoryRecord != null && factoryRecord.AllowDefinition != value) {
                    throw new ConfigurationErrorsException(SR.GetString(SR.Config_tag_name_already_defined, _configKey));
                }

                _allowDefinition = value;
                _modifiedFlags[ Flag_AllowDefinitionModified       ] = true;
            }
        }

        internal bool AllowDefinitionModified {
            get {
                return _modifiedFlags[ Flag_AllowDefinitionModified ];
            }
        }

        public ConfigurationAllowExeDefinition AllowExeDefinition {
            get {return _allowExeDefinition;}
            set {
                VerifyIsEditable();
                VerifyIsEditableFactory();

                // allow AllowDefinition to be different from current type,
                // so long as it doesn't conflict with a type already defined
                FactoryRecord factoryRecord = FindParentFactoryRecord(false);
                if (factoryRecord != null && factoryRecord.AllowExeDefinition != value) {
                    throw new ConfigurationErrorsException(SR.GetString(SR.Config_tag_name_already_defined, _configKey));
                }

                _allowExeDefinition = value;
                _modifiedFlags[ Flag_AllowExeDefinitionModified       ] = true;
            }
        }

        internal bool AllowExeDefinitionModified {
            get {
                return _modifiedFlags[ Flag_AllowExeDefinitionModified ];
            }
        }

        public bool AllowLocation {
            get {
                return _flags[ Flag_AllowLocation ];
            }
            set {
                VerifyIsEditable();
                VerifyIsEditableFactory();

                // allow AllowLocation to be different from current type,
                // so long as it doesn't conflict with a type already defined
                FactoryRecord factoryRecord = FindParentFactoryRecord(false);
                if (factoryRecord != null && factoryRecord.AllowLocation != value) {
                    throw new ConfigurationErrorsException(SR.GetString(SR.Config_tag_name_already_defined, _configKey));
                }

                _flags[ Flag_AllowLocation ] = value;
                _modifiedFlags[ Flag_AllowLocation ] = true;
            }
        }

        internal bool AllowLocationModified {
            get {
                return _modifiedFlags[ Flag_AllowLocation ];
            }
        }

        public bool AllowOverride {
            get {
                return _flags[ Flag_AllowOverride ];
            }
            set {
                VerifyIsEditable();
                VerifySupportsLocation();
                _flags[ Flag_AllowOverride ] = value;
                _modifiedFlags[ Flag_AllowOverride ] = true;
            }
        }


        // LocationAttributesAreDefault
        //
        // Are the location attributes for this section set to the
        // default settings?
        //
        internal bool LocationAttributesAreDefault {
            get {
                return ( ( _flags[ Flag_AllowOverride      ] == true ) &&
                         ( _flags[ Flag_InheritInChildApps ] == true ) );
            }
        }
        
        public string ConfigSource {
            get {
               if (_configSource != null) {
                   return _configSource;
               }
               else {
                   return String.Empty;
               }
            }

            set {
                string configSource;
                
                VerifyIsEditable();

                if (!String.IsNullOrEmpty(value)) {
                    configSource = BaseConfigurationRecord.NormalizeConfigSource(value, null);
                }
                else {
                    configSource = null;
                }
                
                // return early if there is no change
                if (configSource == _configSource)
                    return;

                if (_configRecord != null) {
                    _configRecord.ChangeConfigSource(this, _configSource, _configSourceStreamName, configSource);
                }

                _configSource = configSource;
                _modifiedFlags[Flag_ConfigSourceModified] = true;
            }
        }

        internal bool ConfigSourceModified {
            get {
                return _modifiedFlags[Flag_ConfigSourceModified];
            }
        }

        internal string ConfigSourceStreamName {
            get { 
                return _configSourceStreamName;
            }

            set {
                _configSourceStreamName = value;
            }
        }

        public bool InheritInChildApplications {
            get {
                return _flags[ Flag_InheritInChildApps ];
            }
            set {
                VerifyIsEditable();
                VerifySupportsLocation();
                _flags[ Flag_InheritInChildApps ] = value;
            }
        }

        // True if the section is declared at the current level
        public bool IsDeclared {
            get {
                VerifyNotParentSection();
                
                return _flags[ Flag_Declared ];
            }
        }

        // IsDeclarationRequired
        //
        // Is the Declaration Required.  It is required if it is not set
        // in a parent, or the parent entry does not have the type
        //
        public bool IsDeclarationRequired {
            get { 
                VerifyNotParentSection();

                return _flags[Flag_DeclarationRequired]; 
            }
        }

        // Force the section declaration to be written out during Save.
        public void ForceDeclaration() {
            ForceDeclaration( true );
        }

        // If force==false, it actually means don't declare it at
        // the current level.
        public void ForceDeclaration(bool force) {
            VerifyIsEditable();

            if ( ( force == false ) &&
                 _flags[ Flag_DeclarationRequired ] )
            {
                // Since it is required, we can not remove it
            }
            else
            {
                if (force == true && BaseConfigurationRecord.IsImplicitSection(SectionName)) {
                    throw new ConfigurationErrorsException(SR.GetString(SR.Cannot_declare_or_remove_implicit_section));
                }

                if (force == true && _flags[ Flag_IsUndeclared ] ) {
                    throw new ConfigurationErrorsException(SR.GetString(SR.Config_cannot_edit_configurationsection_when_it_is_undeclared));
                }

                _flags[ Flag_Declared ] = force;
            }
        }

        // IsDefinitionAllowed
        //
        // Is the Definition Allowed at this point.  This is all depending
        // on the Definition that is allowed, and what context we are 
        // writing the file
        //
        private bool IsDefinitionAllowed {
            get {
                if (_configRecord == null) {
                    return true;
                }
                else {
                    return _configRecord.IsDefinitionAllowed(_allowDefinition, _allowExeDefinition);
                }
            }
        }

        public bool IsLocked {
            get {
                return _flags[Flag_LocationLocked] || !IsDefinitionAllowed || 
                    _configurationSection.ElementInformation.IsLocked;
            }
        }

        public bool IsProtected {
            get {return (ProtectionProvider != null);}
        }

        public ProtectedConfigurationProvider ProtectionProvider {
            get {
                if (!_flags[ Flag_ProtectionProviderDetermined] && _configRecord != null) {
                    _protectionProvider = _configRecord.GetProtectionProviderFromName(_protectionProviderName, false);
                    _flags[ Flag_ProtectionProviderDetermined ] = true;
                }
                
                return _protectionProvider;
            }
        }

        // method to cause a section to be protected using the specified provider
        public void ProtectSection(string protectionProvider) {
            ProtectedConfigurationProvider protectedConfigurationProvider = null;

            VerifyIsEditable();

            // Do not encrypt sections that will be read by a native reader.
            // These sections are be marked with allowLocation=false.
            // Also, the configProtectedData section cannot be encrypted!
            if (!AllowLocation || _configKey == BaseConfigurationRecord.RESERVED_SECTION_PROTECTED_CONFIGURATION) {
                throw new InvalidOperationException(SR.GetString(SR.Config_not_allowed_to_encrypt_this_section));
            }

            if (_configRecord != null) {
                if (String.IsNullOrEmpty(protectionProvider)) {
                    protectionProvider = _configRecord.DefaultProviderName;
                }

                protectedConfigurationProvider = _configRecord.GetProtectionProviderFromName(protectionProvider, true);
            }
            else {
                throw new InvalidOperationException(SR.GetString(SR.Must_add_to_config_before_protecting_it));
            }

            _protectionProviderName = protectionProvider;
            _protectionProvider = protectedConfigurationProvider;

            _flags[ Flag_ProtectionProviderDetermined ] = true;
            _modifiedFlags[ Flag_ProtectionProviderModified ] = true;
        }

        public void UnprotectSection() {
            VerifyIsEditable();

            _protectionProvider = null;
            _protectionProviderName = null;
            _flags[ Flag_ProtectionProviderDetermined ] = true;
            _modifiedFlags[ Flag_ProtectionProviderModified ] = true;
        }

        internal string ProtectionProviderName {
            get { return _protectionProviderName; }
        }

        public bool RestartOnExternalChanges {
            get {
                return _flags[ Flag_RestartOnExternalChanges ];
            }
            set {
                VerifyIsEditable();
                VerifyIsEditableFactory();

                // allow RestartOnExternalChanges to be different from current type,
                // so long as it doesn't conflict with a type already defined
                FactoryRecord factoryRecord = FindParentFactoryRecord(false);
                if (factoryRecord != null && factoryRecord.RestartOnExternalChanges != value) {
                    throw new ConfigurationErrorsException(SR.GetString(SR.Config_tag_name_already_defined, _configKey));
                }

                _flags[ Flag_RestartOnExternalChanges ] = value;
                _modifiedFlags[ Flag_RestartOnExternalChanges ] = true;
            }
        }

        internal bool RestartOnExternalChangesModified {
            get {
                return _modifiedFlags[ Flag_RestartOnExternalChanges ];
            }
        }

        public bool RequirePermission {
            get {
                return _flags[ Flag_RequirePermission ];
            }
            set {
                VerifyIsEditable();
                VerifyIsEditableFactory();

                // allow RequirePermission to be different from current type,
                // so long as it doesn't conflict with a type already defined
                FactoryRecord factoryRecord = FindParentFactoryRecord(false);
                if (factoryRecord != null && factoryRecord.RequirePermission != value) {
                    throw new ConfigurationErrorsException(SR.GetString(SR.Config_tag_name_already_defined, _configKey));
                }

                _flags[ Flag_RequirePermission ] = value;
                _modifiedFlags[ Flag_RequirePermission ] = true;
            }
        }

        internal bool RequirePermissionModified {
            get {
                return _modifiedFlags[ Flag_RequirePermission ];
            }
        }


        public string Type {
            get {return _typeName;}
            set {
                if (String.IsNullOrEmpty(value)) {
                    throw ExceptionUtil.PropertyNullOrEmpty("Type");
                }

                VerifyIsEditable();
                VerifyIsEditableFactory();

                // allow type to be different from current type,
                // so long as it doesn't conflict with a type already defined
                FactoryRecord factoryRecord = FindParentFactoryRecord(false);
                if (factoryRecord != null) {
                    IInternalConfigHost host = null;
                    if (_configRecord != null) {
                        host = _configRecord.Host;
                    }

                    if (!factoryRecord.IsEquivalentType(host, value)) {
                        throw new ConfigurationErrorsException(SR.GetString(SR.Config_tag_name_already_defined, _configKey));
                    }
                }

                _typeName = value;
            }
        }

        public ConfigurationSection GetParentSection() {
            VerifyDesigntime();

            if ( _flags[ Flag_IsParentSection ] ) {
                throw new InvalidOperationException(SR.GetString(SR.Config_getparentconfigurationsection_first_instance));
            }

            // if a users create a configsection with : sectionType sec  = new sectionType();
            // the config record will be null.  Return null for the parent in this case.
            ConfigurationSection ancestor = null;
            if (_configRecord != null) {  
                ancestor = _configRecord.FindAndCloneImmediateParentSection(_configurationSection);
                if (ancestor != null) {
                    ancestor.SectionInformation._flags[Flag_IsParentSection]     = true;
                    ancestor.SetReadOnly();
                }
            }

            return ancestor;
        }

        public string GetRawXml() {
            VerifyDesigntime();
            VerifyNotParentSection();

            if (RawXml != null) {
                return RawXml;
            }
            else if (_configRecord != null) {
                return _configRecord.GetRawXml(_configKey);
            }
            else {
                return null;
            }
        }

        public void SetRawXml(string rawXml) {
            VerifyIsEditable();

            if (_configRecord != null) {
                _configRecord.SetRawXml(_configurationSection, rawXml);
            }
            else {
                RawXml = (String.IsNullOrEmpty(rawXml)) ? null : rawXml;
            }
        }

        internal string RawXml {
            get {
                return _rawXml;
            }

            set {
                _rawXml = value;
            }
        }

        // True if the section will be serialized to the current hierarchy level, regardless of 
        // ConfigurationSaveMode.
        public bool ForceSave
        {
            get { 
                return _flags[ Flag_ForceSave ];
            }
            set { 
                VerifyIsEditable();

                _flags[ Flag_ForceSave ] = value; 
            }
        }

        public void RevertToParent() {
            VerifyIsEditable();
            VerifyIsAttachedToConfigRecord();

            _configRecord.RevertToParent(_configurationSection);
        }

    }
}
