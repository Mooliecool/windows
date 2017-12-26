//------------------------------------------------------------------------------
// <copyright file="DefaultValidator.cs" company="Microsoft">
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
using System.Collections;
using System.IO;
using System.Reflection;
using System.Security.Permissions;
using System.Xml;
using System.Collections.Specialized;
using System.Globalization;
using System.ComponentModel;
using System.Security;
using System.Text;
using System.Text.RegularExpressions;

namespace System.Configuration {

    // Default configuration validator
    // Can validate everything and never complains
    public sealed class DefaultValidator : ConfigurationValidatorBase {

        public override bool CanValidate(Type type) {
            return true;
        }
        
        public override void Validate(object value) {
            // Everything is OK with this validator
        }
    }
}
