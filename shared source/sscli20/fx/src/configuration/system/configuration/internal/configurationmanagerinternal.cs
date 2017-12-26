//------------------------------------------------------------------------------
// <copyright file="ConfigurationManagerInternal.cs" company="Microsoft">
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

namespace System.Configuration.Internal {

    using System.Configuration;

    internal sealed class ConfigurationManagerInternal : IConfigurationManagerInternal {

        // Created only through reflection
        private ConfigurationManagerInternal() {
        }

        bool IConfigurationManagerInternal.SupportsUserConfig {
            get {
                return ConfigurationManager.SupportsUserConfig;
            }
        }

        bool IConfigurationManagerInternal.SetConfigurationSystemInProgress {
            get {
                return ConfigurationManager.SetConfigurationSystemInProgress;
            }
        }

        string IConfigurationManagerInternal.MachineConfigPath {
            get {
                return ClientConfigurationHost.MachineConfigFilePath;
            }
        }

        string IConfigurationManagerInternal.ApplicationConfigUri {
            get {
                return ClientConfigPaths.Current.ApplicationConfigUri;
            }
        }

        string IConfigurationManagerInternal.ExeProductName {
            get {
                return ClientConfigPaths.Current.ProductName;
            }
        }

        string IConfigurationManagerInternal.ExeProductVersion {
            get {
                return ClientConfigPaths.Current.ProductVersion;
            }
        }

        string IConfigurationManagerInternal.ExeRoamingConfigDirectory {
            get {
                return ClientConfigPaths.Current.RoamingConfigDirectory;
            }
        }

        string IConfigurationManagerInternal.ExeRoamingConfigPath {
            get {
                return ClientConfigPaths.Current.RoamingConfigFilename;
            }
        }

        string IConfigurationManagerInternal.ExeLocalConfigDirectory {
            get {
                return ClientConfigPaths.Current.LocalConfigDirectory;
            }
        }

        string IConfigurationManagerInternal.ExeLocalConfigPath {
            get {
                return ClientConfigPaths.Current.LocalConfigFilename;
            }
        }

        string IConfigurationManagerInternal.UserConfigFilename {
            get {
                return ClientConfigPaths.UserConfigFilename;
            }
        }
    }
}
