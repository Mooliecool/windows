//------------------------------------------------------------------------------
// <copyright file="ConfigurationFileMap.cs" company="Microsoft">
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

using System;
using System.Collections;
using System.Collections.Specialized;
using System.Security;
using System.Security.Permissions;


namespace System.Configuration {

    //
    // Holds the configuration file mapping for 
    // machine.config. It is the base class for
    // ExeConfigurationFileMap and WebConfigurationFileMap.
    //
    public class ConfigurationFileMap : ICloneable {
        string  _machineConfigFilename;
        bool    _requirePathDiscovery;

        public ConfigurationFileMap() {
            _machineConfigFilename = ClientConfigurationHost.MachineConfigFilePath;
            _requirePathDiscovery = true;
        }

        public ConfigurationFileMap(string machineConfigFilename) {
            _machineConfigFilename = machineConfigFilename;
        }

        public virtual object Clone() {
            return new ConfigurationFileMap(_machineConfigFilename);
        }

        //
        // The name of machine.config.
        //
        public string MachineConfigFilename {
            get {
                //
                // Ensure that we use the same string to issue the demand that we use to 
                // return to the caller.
                //
                string filename = _machineConfigFilename;
                if (_requirePathDiscovery) {
                    new FileIOPermission(FileIOPermissionAccess.PathDiscovery, filename).Demand();
                }

                return filename;
            }

            set {
                _requirePathDiscovery = false;
                _machineConfigFilename = value;
            }
        }
    }
}
