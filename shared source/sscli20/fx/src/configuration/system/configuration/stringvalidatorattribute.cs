//------------------------------------------------------------------------------
// <copyright file="StringValidatorAttribute.cs" company="Microsoft">
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

    [AttributeUsage(AttributeTargets.Property)]
    public sealed class StringValidatorAttribute : ConfigurationValidatorAttribute {
        private int _minLength = 0;
        private int _maxLength = int.MaxValue;
        private string _invalidChars;

        public StringValidatorAttribute() {
        }

        public override ConfigurationValidatorBase ValidatorInstance {
            get {
                return new StringValidator(_minLength, _maxLength, _invalidChars);
            }
        }

        public int MinLength {
            get {
                return _minLength;
            }
            set {
                if (_maxLength < value) {
                    throw new ArgumentOutOfRangeException("value", SR.GetString(SR.Validator_min_greater_than_max));
                }

                _minLength = value;
            }
        }

        public int MaxLength {
            get {
                return _maxLength;
            }
            set {
                if (_minLength > value) {
                    throw new ArgumentOutOfRangeException("value", SR.GetString(SR.Validator_min_greater_than_max));
                }

                _maxLength = value;
            }
        }

        public string InvalidCharacters {
            get {
                return _invalidChars;
            }
            set {
                _invalidChars = value;
            }
        }
    }
}
