/***************************************** Module Header *****************************\
* Module Name:  EnumTypeConverter.cs
* Project:      CSVSPackageOptionPageWithTypeConverter
* Copyright (c) Microsoft Corporation.
* 
* This sample demonstrates how to use TypeConverter in Option Page.
* A type converter can be used to convert values between data types, and to
* assist property configuration at design time by providing text-to-value
* conversion or a drop-down list of values to select from.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 02/25/2010 04:35 PM Wesley Yao Created
\*************************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Globalization;

namespace Microsoft.CSVSPackageOptionPageWithTypeConverter
{
    ///////////////////////////////////////////////////////////////////////////////
    // Create a class named EnumTypeConverter which derived from EnumConverter,
    // implement the methods of EnumConverter.
    // About how to implement a Type Converter, please check this document:
    // http://msdn.microsoft.com/en-us/library/ayybcxe5.aspx
    // 
    class EnumTypeConverter : EnumConverter
    {
        public EnumTypeConverter()
            : base(typeof(MyEnumProperty))
        {
        }

        public override bool CanConvertFrom(ITypeDescriptorContext context,
            Type sourceType)
        {
            if (sourceType == typeof(string)) return true;

            return base.CanConvertFrom(context, sourceType);
        }

        public override object ConvertFrom(ITypeDescriptorContext context,
            CultureInfo culture, object value)
        {
            string str = value as string;

            if (str != null)
            {
                if (str == "Beautiful None") return MyEnumProperty.None;
                if (str == "Beautiful First") return MyEnumProperty.First;
                if (str == "Beautiful Second") return MyEnumProperty.Second;
                if (str == "Beautiful Third") return MyEnumProperty.Third;
            }

            return base.ConvertFrom(context, culture, value);
        }

        public override object ConvertTo(ITypeDescriptorContext context,
            CultureInfo culture, object value, Type destinationType)
        {
            if (destinationType == typeof(string))
            {
                string result = null;
                if ((int)value == 0) result = "Beautiful None";
                else if ((int)value == 1) result = "Beautiful First";
                else if ((int)value == 2) result = "Beautiful Second";
                else if ((int)value == 3) result = "Beautiful Third";

                if (result != null) return result;
            }

            return base.ConvertTo(context, culture, value, destinationType);
        }
    }
}
