//------------------------------------------------------------------------------
// <copyright file="CallbackValidator.cs" company="Microsoft">
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

    public sealed class CallbackValidator : ConfigurationValidatorBase {
        Type _type;
        ValidatorCallback _callback;

        public CallbackValidator(Type type, ValidatorCallback callback) : this(callback) {
            if (type == null) {
                throw new ArgumentNullException("type");
            }
            _type = type;
        }

        // Do not check for null type here to handle the callback attribute case
        internal CallbackValidator(ValidatorCallback callback) {
            if (callback == null) {
                throw new ArgumentNullException("callback");
            }
            _type = null;
            _callback = callback;
        }

        public override bool CanValidate(Type type) {
            return (type == _type || _type == null);
        }
        public override void Validate(object value) {
            _callback(value);
        }
    }
}
