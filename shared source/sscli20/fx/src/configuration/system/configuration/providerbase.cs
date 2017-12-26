//------------------------------------------------------------------------------
// <copyright file="ProviderBase.cs" company="Microsoft">
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

namespace System.Configuration.Provider {
    using  System.Collections.Specialized;
    using  System.Runtime.Serialization;

    public abstract class ProviderBase {

        private string _name;
        private string _Description;

        public virtual string Name          { get { return _name; } }
        public virtual string Description { get { return string.IsNullOrEmpty(_Description) ? Name : _Description; } }

        public virtual void Initialize(string name, NameValueCollection config) {
            lock (this) {
                if (_Initialized)
                    throw new InvalidOperationException(SR.GetString(SR.Provider_Already_Initialized));
                _Initialized = true;
            }
            if (name == null)
                throw new ArgumentNullException("name");
            if (name.Length == 0)
                throw new ArgumentException(SR.GetString(SR.Config_provider_name_null_or_empty), "name");
            _name = name;
            if (config != null) {
                _Description = config["description"];
                config.Remove("description");
            }
        }
        private bool _Initialized;
    }
}
