//------------------------------------------------------------------------------
// <copyright file="ExeConfigurationFileMap.cs" company="Microsoft">
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
    // Holds the configuration file mapping for an Exe.
    //
    public sealed class ExeConfigurationFileMap : ConfigurationFileMap {
        string  _exeConfigFilename;
        string  _roamingUserConfigFilename;
        string  _localUserConfigFilename;

        public ExeConfigurationFileMap() {
            _exeConfigFilename = String.Empty;
            _roamingUserConfigFilename = String.Empty;
            _localUserConfigFilename = String.Empty;
        }

        ExeConfigurationFileMap(string machineConfigFilename, string exeConfigFilename, string roamingUserConfigFilename, string localUserConfigFilename) 
                : base(machineConfigFilename) {

            _exeConfigFilename = exeConfigFilename;
            _roamingUserConfigFilename = roamingUserConfigFilename;
            _localUserConfigFilename = localUserConfigFilename;
        }

        public override object Clone() {
            return new ExeConfigurationFileMap(MachineConfigFilename, _exeConfigFilename, _roamingUserConfigFilename, _localUserConfigFilename);
        }

        //
        // The name of the config file for the exe.
        //
        public string ExeConfigFilename {
            get {
                return _exeConfigFilename;
            }

            set {
                _exeConfigFilename = value;
            }
        }

        //
        // The name of the config file for the roaming user.
        //
        public string RoamingUserConfigFilename {
            get {
                return _roamingUserConfigFilename;
            }

            set {
                _roamingUserConfigFilename = value;
            }
        }

        //
        // The name of the config file for the local user.
        //
        public string LocalUserConfigFilename {
            get {
                return _localUserConfigFilename;
            }

            set {
                _localUserConfigFilename = value;
            }
        }
    }
}
