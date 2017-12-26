//------------------------------------------------------------------------------
// <copyright file="PrivilegedConfigurationManager.cs" company="Microsoft">
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

    using System.Collections.Specialized;
    using System.Security;
    using System.Security.Permissions;
    
    [ConfigurationPermission(SecurityAction.Assert, Unrestricted=true)]
    internal static class PrivilegedConfigurationManager {
        internal static ConnectionStringSettingsCollection ConnectionStrings { 
            [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance", "CA1811:AvoidUncalledPrivateCode")]
            get {
                return ConfigurationManager.ConnectionStrings;
            }
        }

        internal static object GetSection(string sectionName) {
            return ConfigurationManager.GetSection(sectionName);
        }
    }
}
