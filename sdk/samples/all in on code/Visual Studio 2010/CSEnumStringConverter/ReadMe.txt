=============================================================================
       CONSOLE APPLICATION : CSEnumStringConverter Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary: 

This sample demonstrates how to convert enum to comma separated string & vice
 versa. It also covers the description attribute for enum while conversion. 


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the enum-string 
convertion sample.

Step 1: Build and run the sample solution in Visual Studio 2010

Step 2: During first conversion process it will use the .NET framework built-in 
EnumConverter class to convert string to enum & back to string.

Step 3: During second conversion process it will use the EnumDescriptionConverter
class to convert the description string to enum & back to string.
/////////////////////////////////////////////////////////////////////////////
Implementation:

Following enum of ProgrammingLanguage has been used for example.
 [Flags]
    enum ProgrammingLanguage
    {
        [Description("Visual C#")]
        CS = 0x1,
        [Description("Visual Basic")]
        VB = 0x2, 
        [Description("Visual C++")]
        Cpp = 0x4,
        [Description("Javascript")]
        JS = 0x8,
        // XAML
        XAML = 0x10
    }

System.ComponentModel.EnumConverter supports the conversion of one type to 
another which has been used in the first conversion process.
    EnumConverter converter = new EnumConverter(typeof(ProgrammingLanguage));
    // Convert string to enum.
    string langStr = "CS, Cpp, XAML";    
    ProgrammingLanguage lang 
        = (ProgrammingLanguage)converter.ConvertFromString(langStr);
    // Convert enum to string.
    langStr = converter.ConvertToString(lang);
    
EnumDescriptionConverter class inherits EnumConverter to support Description 
attributes in the second conversion process.

1. It will split the comma separated string into string[] and then convert to 
    long & do bitwise ORing to get the long value representing the enum value. 
        // convert from string to enum
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

2. It will read the custom attributes from the given enum, in this case Description 
 attribute & prepare the corresponding string value. In case of flags enum, it will fetch
 Description attribute[] & prepare comma separated string as output.
        // convert to string
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

/////////////////////////////////////////////////////////////////////////////
References:

EnumConverter Class
http://msdn.microsoft.com/en-us/library/system.componentmodel.enumconverter.aspx

DescriptionAttribute Members
http://msdn.microsoft.com/en-us/library/system.componentmodel.descriptionattribute_members(v=VS.85).aspx

/////////////////////////////////////////////////////////////////////////////