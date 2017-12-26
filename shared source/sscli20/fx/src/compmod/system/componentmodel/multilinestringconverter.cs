//------------------------------------------------------------------------------
// <copyright file="MultilineStringConverter.cs" company="Microsoft">
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

namespace System.ComponentModel {

    using System.ComponentModel;
    using System.ComponentModel.Design;
    using System.Diagnostics;
    using System.Globalization;
    using System.Security.Permissions;

    /// <devdoc>
    /// Provides a type converter to convert multiline strings to a simple string.
    /// </devdoc>
    [HostProtection(SharedState = true)]
    [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Naming", "CA1704:IdentifiersShouldBeSpelledCorrectly")]
    public class MultilineStringConverter : TypeConverter {

        /// <devdoc>
        /// Converts the given value object to the specified destination type.
        /// </devdoc>
        public override object ConvertTo(ITypeDescriptorContext context, CultureInfo culture, object value, Type destinationType) {
            if (destinationType == null) {
                throw new ArgumentNullException("destinationType");
            }

            if (destinationType == typeof(string)) {
                if (value is string) {
                    return SR.GetString(SR.MultilineStringConverterText);
                }
            }

            return base.ConvertTo(context, culture, value, destinationType);
        }
        
        /// <devdoc>
        /// Gets a collection of properties for the type of array specified by the value
        /// parameter using the specified context and attributes.
        /// </devdoc>
        public override PropertyDescriptorCollection GetProperties(ITypeDescriptorContext context, object value, Attribute[] attributes) {
            return null;
        }
       
        /// <devdoc>
        /// Gets a value indicating whether this object supports properties.
        /// </devdoc>
        public override bool GetPropertiesSupported(ITypeDescriptorContext context) {
            return false;
        }
    }
}

