//------------------------------------------------------------------------------
// <copyright file="ConfigurationLocation.cs" company="Microsoft">
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
    using System.Collections;
    using System.Collections.Specialized;
    using System.Configuration;
    using System.Text;

    public class ConfigurationLocation {
        Configuration   _config;
        string          _locationSubPath;

        internal ConfigurationLocation(Configuration config, string locationSubPath) {
            _config = config;
            _locationSubPath = locationSubPath;
        }

        public string Path {
            get {return _locationSubPath;}
        }

        public Configuration OpenConfiguration() {
            return _config.OpenLocationConfiguration(_locationSubPath);
        }
    }
}
