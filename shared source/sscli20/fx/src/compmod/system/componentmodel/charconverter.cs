//------------------------------------------------------------------------------
// <copyright file="CharConverter.cs" company="Microsoft">
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

/*
 */
namespace System.ComponentModel {
    using Microsoft.Win32;
    using System.ComponentModel;
    using System.Diagnostics;
    using System.Globalization;
    using System.Runtime.Serialization.Formatters;
    using System.Runtime.Remoting;
    using System.Runtime.InteropServices;
    using System.Security.Permissions;

    /// <devdoc>
    ///    <para>Provides
    ///       a type converter to convert Unicode
    ///       character objects to and from various other representations.</para>
    /// </devdoc>
    [HostProtection(SharedState = true)]
    public class CharConverter : TypeConverter {

        /// <devdoc>
        ///    <para>Gets a value indicating whether this converter can
        ///       convert an object in the given source type to a Unicode character object using
        ///       the specified context.</para>
        /// </devdoc>
        public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType) {
            if (sourceType == typeof(string)) {
                return true;
            }
            return base.CanConvertFrom(context, sourceType);
        }

		/// <devdoc>
		///      Converts the given object to another type.
		/// </devdoc>
		public override object ConvertTo(ITypeDescriptorContext context, CultureInfo culture, object value, Type destinationType) 
		{
			if (destinationType == typeof(string) && value is char)
			{
				if ((char)value == (char)0)
				{
					return "";
				}
			}
            
			return base.ConvertTo(context, culture, value, destinationType);
		}

        /// <devdoc>
        ///    <para>Converts the given object to a Unicode character object.</para>
        /// </devdoc>
        public override object ConvertFrom(ITypeDescriptorContext context, CultureInfo culture, object value) {
            if (value is string) {
                string text = (string)value;
                if (text.Length > 1) {
                    text = text.Trim();
                }

                if (text != null && text.Length > 0) {
                    if (text.Length != 1) {
                        throw new FormatException(SR.GetString(SR.ConvertInvalidPrimitive, text, "Char"));
                    }
                    return text[0];
                }

                return '\0';
            }
            return base.ConvertFrom(context, culture, value);
        }
    }
}

