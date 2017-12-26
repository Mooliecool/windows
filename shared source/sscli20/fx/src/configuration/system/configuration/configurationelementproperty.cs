//------------------------------------------------------------------------------
// <copyright file="ConfigurationElementProperty.cs" company="Microsoft">
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

using System;
using System.Configuration.Internal;
using System.Collections;
using System.Collections.Specialized;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Security.Permissions;
using System.Xml;
using System.Globalization;
using System.ComponentModel;
using System.Security;
using System.Text;

namespace System.Configuration {

    public sealed class ConfigurationElementProperty {
        private ConfigurationValidatorBase _validator;

        public ConfigurationElementProperty(ConfigurationValidatorBase validator) {
            if (validator == null) {
                throw new ArgumentNullException("validator");
            }

            _validator = validator;
        }
        public ConfigurationValidatorBase Validator {
            get {
                return _validator;
            }
        }
    }
}
