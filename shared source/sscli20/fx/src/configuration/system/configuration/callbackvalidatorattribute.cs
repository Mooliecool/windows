//------------------------------------------------------------------------------
// <copyright file="CallbackValidatorAttribute.cs" company="Microsoft">
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
    public sealed class CallbackValidatorAttribute : ConfigurationValidatorAttribute {
        private Type _type = null;
        private String _callbackMethodName = String.Empty;
        private ValidatorCallback _callbackMethod;

        public override ConfigurationValidatorBase ValidatorInstance {
            get {
                if (_callbackMethod == null) {
                    if (_type == null) {
                        throw new ArgumentNullException("Type");
                    }
                    if (!String.IsNullOrEmpty(_callbackMethodName)) {
                        MethodInfo methodInfo = _type.GetMethod(_callbackMethodName, BindingFlags.Public | BindingFlags.Static);
                        if (methodInfo != null) {
                            ParameterInfo[] parameters = methodInfo.GetParameters();
                            if ((parameters.Length == 1) && (parameters[0].ParameterType == typeof(Object))) {
                                _callbackMethod = (ValidatorCallback)Delegate.CreateDelegate(typeof(ValidatorCallback), methodInfo);
                            }
                        }
                    }
                }
                if (_callbackMethod == null) {
                    throw new System.ArgumentException(SR.GetString(SR.Validator_method_not_found, _callbackMethodName));
                }

                return new CallbackValidator(_callbackMethod);
            }
        }

        public CallbackValidatorAttribute() {
        }

        public Type Type {
            get {
                return _type;
            }
            set {
                _type = value;
                _callbackMethod = null;
            }
        }

        public String CallbackMethodName {
            get {
                return _callbackMethodName;
            }
            set {
                _callbackMethodName = value;
                _callbackMethod = null;
            }
        }
    }
}
