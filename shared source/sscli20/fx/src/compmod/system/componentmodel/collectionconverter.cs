//------------------------------------------------------------------------------
// <copyright file="CollectionConverter.cs" company="Microsoft">
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
    using System.Collections;
    using System.ComponentModel;
    using System.Diagnostics;
    using System.Globalization;
    using System.Runtime.Serialization.Formatters;
    using System.Runtime.Remoting;
    using System.Runtime.InteropServices;
    using System.Security.Permissions;

    /// <devdoc>
    ///    <para>Provides a type converter to convert
    ///       collection objects to and from various other representations.</para>
    /// </devdoc>
    [HostProtection(SharedState = true)]
    public class CollectionConverter : TypeConverter
    {
    
        /// <devdoc>
        ///    <para>Converts the given
        ///       value object to the
        ///       specified destination type.</para>
        /// </devdoc>
        public override object ConvertTo(ITypeDescriptorContext context, CultureInfo culture, object value, Type destinationType) {
            if (destinationType == null) {
                throw new ArgumentNullException("destinationType");
            }

            if (destinationType == typeof(string)) {
                if (value is ICollection) {
                    return SR.GetString(SR.CollectionConverterText);
                }
            }
            
            return base.ConvertTo(context, culture, value, destinationType);
        }
        
        /// <devdoc>
        ///    <para>Gets a collection of properties for
        ///       the type of array specified by the value parameter using the specified context and
        ///       attributes.</para>
        /// </devdoc>
        public override PropertyDescriptorCollection GetProperties(ITypeDescriptorContext context, object value, Attribute[] attributes) {
            //return new PropertyDescriptorCollection(null);
            return null;
        }
       
        /// <devdoc>
        ///    <para>Gets a value indicating whether this object
        ///       supports properties.</para>
        /// </devdoc>
        public override bool GetPropertiesSupported(ITypeDescriptorContext context) {
            return false;
        }
        
        
    }
}

