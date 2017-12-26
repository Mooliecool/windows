//------------------------------------------------------------------------------
// <copyright file="FactoryId.cs" company="Microsoft">
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

    // Identifies a factory
    [System.Diagnostics.DebuggerDisplay("FactoryId {ConfigKey}")]
    internal class FactoryId {
        private string  _configKey;
        private string  _group;
        private string  _name;

        internal FactoryId(string configKey, string group, string name) {
            _configKey = configKey;
            _group = group;
            _name = name;
        }

        internal string ConfigKey {
            get {return _configKey;}
        }

        internal string Group {
            get {return _group;}
        }

        internal string Name {
            get {return _name;}
        }
    }
}
