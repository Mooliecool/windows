//------------------------------------------------------------------------------
// <copyright file="ConfigurationManagerHelper.cs" company="Microsoft">
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


    [
        // FXCOP: The correct fix would be to make this class static.
        // But a class can't be static and sealed at the same time.
        System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance", "CA1812:AvoidUninstantiatedInternalClasses")
    ]
    internal sealed class ConfigurationManagerHelper : IConfigurationManagerHelper {
        // Created only through reflection.
        private ConfigurationManagerHelper() {
        }

        void IConfigurationManagerHelper.EnsureNetConfigLoaded() {
            System.Net.Configuration.SettingsSection.EnsureConfigLoaded();
        }
    }
}
