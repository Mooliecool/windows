/******************************** Module Header ********************************\ 
Module Name:    EnumDescriptionConverter.cs 
Project:        CSEnumStringConverter
Copyright (c) Microsoft Corporation. 

The file provides a type converter to convert Enum objects to and from the 
description string specified by the DescriptionAttribute: EnumDescriptionConverter.  
The class derives from the built-in EnumConverter class.  Different from 
EnumConverter which does not take the DescriptionAttribute into consideration, 
EnumDescriptionConverter converts an enum object to its description string if the 
DescriptionAttribute is attached to the object. When the class is used to convert 
a string to an enum object, the class also attemps to match the string with the 
description string specified by the DescriptionAttribute.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE. 
\*******************************************************************************/

using System;
using System.ComponentModel;
using System.Globalization;
using System.ComponentModel.Design.Serialization;
using System.Reflection;
using System.Text;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;


namespace CSEnumStringConverter
{
    /// <summary>
    /// Provides a type converter to convert Enum objects to and from the 
    /// description string specified by the DescriptionAttribute.  The class 
    /// derives from the built-in EnumConverter class.  Different from 
    /// EnumConverter which does not take the DescriptionAttribute into 
    /// consideration, EnumDescriptionConverter converts an enum object to its 
    /// description string if the DescriptionAttribute is attached to the 
    /// object. When the class is used to convert a string to an enum object, 
    /// the class also attemps to match the string with the description string 
    /// specified by the DescriptionAttribute.
    /// </summary>
    public class EnumDescriptionConverter : EnumConverter
    {
        private const char enumSeperator = ',';

        public EnumDescriptionConverter(Type type) : base(type)
        {
        }


        #region ConvertFrom (String)

        /// <summary>
        /// Converts the given value to the enum type of this converter. If the 
        /// given value is a string, the method will attempt to match the string 
        /// with the names, and the description strings specified by the 
        /// DescriptionAttribute of each enum object.
        /// </summary>
        /// <param name="context">
        /// An ITypeDescriptorContext that provides a format context.
        /// </param>
        /// <param name="culture">
        /// An optional CultureInfo. If not supplied, the current culture is 
        /// assumed.
        /// </param>
        /// <param name="value">The enum object to convert. </param>
        /// <returns>
        /// An enum object that represents the converted value.
        /// </returns>
        public override object ConvertFrom(ITypeDescriptorContext context, 
            CultureInfo culture, object value)
        {
            if (value is string)
            {
                string strValue = (string)value;
                if (strValue.IndexOf(enumSeperator) != -1)
                {
                    ulong convertedValue = 0;
                    foreach (string v in strValue.Split(enumSeperator))
                    {
                        convertedValue |= Convert.ToUInt64(Parse(this.EnumType, v), culture);
                    }
                    return Enum.ToObject(this.EnumType, convertedValue);
                }
                else
                {
                    return Parse(this.EnumType, strValue);
                }
            }
            
            return base.ConvertFrom(context, culture, value);
        }

        /// <summary>
        /// Converts the string representation of the name, or the description 
        /// string specified by the DescriptionAttribute, or numeric value of one 
        /// or more enumerated constants to an equivalent enumerated object. 
        /// </summary>
        /// <param name="enumType">An enumeration type. </param>
        /// <param name="value">
        /// A string containing the name or the description or value to convert.
        /// </param>
        /// <returns>
        /// An object of type enumType whose value is represented by value.
        /// </returns>
        /// <remarks>
        /// Different from Enum.Parse that disregards the DescriptionAttribute,
        /// this method can also convert the description string to an equivalent 
        /// enum object.
        /// 
        /// The comparison of strings is not case sensitive.
        /// </remarks>
        private static object Parse(Type enumType, string value)
        {
            if (enumType == null)
                throw new ArgumentNullException("enumType");

            if (!enumType.IsEnum)
                throw new ArgumentException("Type provided must be an Enum.\r\nParameter name: enumType");

            if (value == null)
                throw new ArgumentNullException("value");

            value = value.Trim();
            if (value.Length == 0)
                throw new ArgumentException("Must specify valid information for parsing in the string.");

            if (Char.IsDigit(value[0]) || value[0] == '-' || value[0] == '+')
            {
                Type underlyingType = Enum.GetUnderlyingType(enumType);
                object temp;
                try
                {
                    temp = Convert.ChangeType(value, underlyingType, CultureInfo.InvariantCulture);
                    return Enum.ToObject(enumType, temp);
                }
                catch (FormatException)
                {
                    // We need to Parse this a String instead. There are cases 
                    // when you tlbimp enums that can have values of the form "3D".
                }
            }

            ulong result = 0;
            Dictionary<string, ulong> dict = GetStringToEnumDictionary(enumType);

            foreach (string v in value.Split(enumSeperator))
            {
                string nameOrDesc = v.Trim();
                if (dict.ContainsKey(nameOrDesc))
                {
                    result |= dict[nameOrDesc];
                }
                else
                {
                    throw new ArgumentException(
                        String.Format("Requested value '{0}' was not found.", value));
                }
            }

            return Enum.ToObject(enumType, result);
        }

        private static Hashtable fieldInfoHash = Hashtable.Synchronized(new Hashtable());
        private const int maxHashElements = 100; // To trim the working set

        private static Dictionary<string, ulong> GetStringToEnumDictionary(Type enumType)
        {
            Debug.Assert(enumType != null);
            Debug.Assert(enumType.IsEnum);

            Dictionary<string, ulong> dict = (Dictionary<string, ulong>)fieldInfoHash[enumType];
            if (dict == null)
            {
                // To reduce the workingset we clear the hashtable when a 
                // threshold number of elements are inserted.
                if (fieldInfoHash.Count > maxHashElements)
                {
                    fieldInfoHash.Clear();
                }

                // Create a dictionary with a case-insensitive string comparer.
                dict = new Dictionary<string, ulong>(StringComparer.OrdinalIgnoreCase);

                FieldInfo[] fields = enumType.GetFields(BindingFlags.Static | BindingFlags.Public);
                foreach (FieldInfo info in fields)
                {
                    ulong enumValue = Convert.ToUInt64(info.GetValue(info.Name));

                    // Add the <name, enumValue> pair.
                    dict.Add(info.Name, enumValue);

                    // Add the <description, enumValue> pair if available.
                    DescriptionAttribute[] attrs = (DescriptionAttribute[])
                        info.GetCustomAttributes(typeof(DescriptionAttribute), false);
                    if (attrs.Length > 0)
                    {
                        dict.Add(attrs[0].Description, enumValue);
                    }
                }

                fieldInfoHash.Add(enumType, dict);
            }
            return dict;
        }

        #endregion


        #region ConvertTo (String)

        /// <summary>
        /// Converts the given enum value object to the specified destination 
        /// type. If the destination type is string, the method converts the enum 
        /// value to its description string when the DescriptionAttribute is 
        /// attached to the enum value. 
        /// </summary>
        /// <param name="context">
        /// An ITypeDescriptorContext that provides a format context.
        /// </param>
        /// <param name="culture">
        /// An optional CultureInfo. If not supplied, the current culture is 
        /// assumed.
        /// </param>
        /// <param name="value">The enum object to convert</param>
        /// <param name="destinationType">
        /// The Type to convert the value to.
        /// </param>
        /// <returns>
        /// An object that represents the converted value.
        /// </returns>
        public override object ConvertTo(ITypeDescriptorContext context, 
            CultureInfo culture, object value, Type destinationType)
        {
            if (destinationType == null)
            {
                throw new ArgumentNullException("destinationType");
            }

            if (destinationType == typeof(string) && value != null)
            {
                // Raise an argument exception if the value isn't defined and if 
                // the enum isn't a flags style.
                //
                Type underlyingType = Enum.GetUnderlyingType(this.EnumType);
                if (value is IConvertible && value.GetType() != underlyingType)
                {
                    value = ((IConvertible)value).ToType(underlyingType, culture);
                }
                if (!this.EnumType.IsDefined(typeof(FlagsAttribute), false) && 
                    !Enum.IsDefined(this.EnumType, value))
                {
                    throw new ArgumentException(
                        String.Format("The value '{0}' is not a valid value for the enum '{1}'.", 
                        value.ToString(), this.EnumType.Name));
                }

                // If the enum value is decorated with the Description attribute, 
                // return the Description value; otherwise return the name.
                string enumName = Enum.Format(this.EnumType, value, "G");
                string nameOrDesc;

                if (enumName.IndexOf(enumSeperator) != -1)
                {
                    // This is a flags enum. Split the descriptions with ', '.
                    bool firstTime = true;
                    StringBuilder retval = new StringBuilder();

                    foreach (string v in enumName.Split(enumSeperator))
                    {
                        nameOrDesc = v.Trim();

                        FieldInfo info = this.EnumType.GetField(nameOrDesc);
                        DescriptionAttribute[] attrs = (DescriptionAttribute[])
                            info.GetCustomAttributes(typeof(DescriptionAttribute), false);
                        if (attrs.Length > 0)
                        {
                            nameOrDesc = attrs[0].Description;
                        }

                        if (firstTime)
                        {
                            retval.Append(nameOrDesc);
                            firstTime = false;
                        }
                        else
                        {
                            retval.Append(enumSeperator);
                            retval.Append(' ');
                            retval.Append(nameOrDesc);
                        }
                    }

                    return retval.ToString();
                }
                else
                {
                    FieldInfo info = this.EnumType.GetField(enumName);
                    if (info != null)
                    {
                        DescriptionAttribute[] attrs = (DescriptionAttribute[])
                            info.GetCustomAttributes(typeof(DescriptionAttribute), false);
                        nameOrDesc = (attrs.Length > 0) ? attrs[0].Description : enumName;
                    }
                    else
                    {
                        nameOrDesc = enumName;
                    }
                    return nameOrDesc;
                }
            }

            return base.ConvertTo(context, culture, value, destinationType);
        }

        #endregion

    }
}
