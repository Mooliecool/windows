//------------------------------------------------------------------------------
// <copyright file="ConfigurationSettings.cs" company="Microsoft">
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

    public sealed class ConfigurationSettings {
        private ConfigurationSettings() {}

        [Obsolete("This method is obsolete, it has been replaced by System.Configuration!System." +
                  "Configuration.ConfigurationManager.AppSettings")]
        public static NameValueCollection AppSettings {
            get {
                return ConfigurationManager.AppSettings;
            }
        }

        [Obsolete("This method is obsolete, it has been replaced by System.Configuration!System." +
                  "Configuration.ConfigurationManager.GetSection")]
        public static object GetConfig(string sectionName) {
            return ConfigurationManager.GetSection( sectionName );
        }
    }
}
