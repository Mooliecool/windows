//------------------------------------------------------------------------------
// <copyright file="TimeSpanMinutesConverter.cs" company="Microsoft">
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

    public class TimeSpanMinutesConverter : ConfigurationConverterBase {

        public override object ConvertTo(ITypeDescriptorContext ctx, CultureInfo ci, object value, Type type) {
            ValidateType(value, typeof(TimeSpan));

            long data = (long)(((TimeSpan)value).TotalMinutes);

            return data.ToString(CultureInfo.InvariantCulture);
        }

        public override object ConvertFrom(ITypeDescriptorContext ctx, CultureInfo ci, object data) {
            Debug.Assert(data is string, "data is string");

            long min = long.Parse((string)data, CultureInfo.InvariantCulture);

            return TimeSpan.FromMinutes((double)min);
        }
    }
}
