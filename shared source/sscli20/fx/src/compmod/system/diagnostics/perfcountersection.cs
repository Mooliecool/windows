//------------------------------------------------------------------------------
// <copyright file="PerfCounterSection.cs" company="Microsoft Corporation">
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

using System.Configuration;

namespace System.Diagnostics {
    internal class PerfCounterSection : ConfigurationElement {
        private static readonly ConfigurationPropertyCollection _properties;
        private static readonly ConfigurationProperty _propFileMappingSize = new ConfigurationProperty("filemappingsize", typeof(int), 524288, ConfigurationPropertyOptions.None);

        static PerfCounterSection(){
            _properties = new ConfigurationPropertyCollection();
            _properties.Add(_propFileMappingSize);
        }

        [ConfigurationProperty("filemappingsize", DefaultValue = 524288)]
        public int FileMappingSize {
            get { 
                return (int) this[_propFileMappingSize]; 
            }
        }

        protected override ConfigurationPropertyCollection Properties {
            get {
                return _properties;
            }
        }
    }
}

