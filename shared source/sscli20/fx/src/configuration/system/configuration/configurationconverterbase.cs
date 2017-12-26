//------------------------------------------------------------------------------
// <copyright file="ConfigurationConverterBase.cs" company="Microsoft">
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

namespace System.Configuration {

    public abstract class ConfigurationConverterBase : TypeConverter {

        public override bool CanConvertTo(ITypeDescriptorContext ctx, Type type) {
            return (type == typeof(string));
        }
        
        public override bool CanConvertFrom(ITypeDescriptorContext ctx, Type type) {
            return (type == typeof(string));
        }

        internal void ValidateType(object value, Type expected) {
            if ((value != null) && (value.GetType() != expected)) {
                throw new ArgumentException(SR.GetString(SR.Converter_unsupported_value_type, expected.Name));
            }
        }
    }
}
