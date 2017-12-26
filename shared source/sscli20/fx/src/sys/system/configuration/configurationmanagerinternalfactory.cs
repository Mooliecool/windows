//------------------------------------------------------------------------------
// <copyright file="ConfigurationManagerInternalFactory.cs" company="Microsoft">
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

    //
    // class ConfigurationManagerInternalFactory manages access to a 
    // single instance of ConfigurationManagerInternal.
    //
    internal static class ConfigurationManagerInternalFactory {
        private const string ConfigurationManagerInternalTypeString = "System.Configuration.Internal.ConfigurationManagerInternal, " + AssemblyRef.SystemConfiguration;

        static private IConfigurationManagerInternal    s_instance;

        static internal IConfigurationManagerInternal Instance {
            get {
                if (s_instance == null) {
                    s_instance = (IConfigurationManagerInternal) TypeUtil.CreateInstanceWithReflectionPermission(ConfigurationManagerInternalTypeString);
                }

                return s_instance;
            }
        }
    }
}
