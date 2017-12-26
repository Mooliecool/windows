//------------------------------------------------------------------------------
// <copyright file="ConfigurationManagerHelperFactory.cs" company="Microsoft">
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

    using System.Configuration.Internal;
    using System.Security.Permissions;

    //
    // class ConfigurationManagerHelperFactory manages access to a 
    // single instance of ConfigurationManagerHelper.
    //
    static internal class ConfigurationManagerHelperFactory {
        private const string ConfigurationManagerHelperTypeString = "System.Configuration.Internal.ConfigurationManagerHelper, " + AssemblyRef.System;

        static private IConfigurationManagerHelper    s_instance;

        static internal IConfigurationManagerHelper Instance {
            get {
                if (s_instance == null) {
                    s_instance = (IConfigurationManagerHelper) TypeUtil.CreateInstanceWithReflectionPermission(ConfigurationManagerHelperTypeString);
                }

                return s_instance;
            }
        }
    }
}
