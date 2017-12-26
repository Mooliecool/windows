//------------------------------------------------------------------------------
// <copyright file="AssertSection.cs" company="Microsoft Corporation">
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
    internal class AssertSection : ConfigurationElement {
        private static readonly ConfigurationPropertyCollection _properties;
        private static readonly ConfigurationProperty _propAssertUIEnabled  = new ConfigurationProperty("assertuienabled", typeof(bool), true, ConfigurationPropertyOptions.None);
        private static readonly ConfigurationProperty _propLogFile = new ConfigurationProperty("logfilename", typeof(string), String.Empty, ConfigurationPropertyOptions.None);

        static AssertSection()   {
            _properties = new ConfigurationPropertyCollection();
            _properties.Add(_propAssertUIEnabled);
            _properties.Add(_propLogFile);
        }

        [ConfigurationProperty("assertuienabled", DefaultValue = true)]
        public bool AssertUIEnabled {
            get { 
                return (bool) this[_propAssertUIEnabled]; 
            }
        }

        [ConfigurationProperty("logfilename", DefaultValue = "")]
        public string LogFileName {
             get { 
                 return (string) this[_propLogFile]; 
             }
         }

        protected override ConfigurationPropertyCollection Properties {
            get {
                return _properties;
            }
        }

    }
}

