=============================================================================
  Visual Studio VSPackage : CSVSPackageCloseOpenedDocument Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This sample demonstrates how to use TypeConverter in Option Page.

A type converter can be used to convert values between data types, and to
assist property configuration at design time by providing text-to-value
conversion or a drop-down list of values to select from.

/////////////////////////////////////////////////////////////////////////////
Prerequisites:

VS 2008 SDK must be installed on the machine. You can download it from:
http://www.microsoft.com/downloads/details.aspx?FamilyID=30402623-93ca-479a-
867c-04dc45164f5b&displaylang=en

Otherwise the project may not be opened by Visual Studio.

NOTE: The Package Load Failure Dialog occurs because there is no
      PLK(Package Load Key) Specified in this package. To obtain a PLK, please
      to go to WebSite:
      http://msdn.microsoft.com/en-us/vsx/cc655795.aspx
      More info:
      http://msdn.microsoft.com/en-us/library/bb165395.aspx


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Create a Visual Studio Integration Package project from the New
Project dialog named CSVSPackageOptionPageWithTypeConverter, choose Visual C#
as the development language.

Step2. Add a new class file named OptionsPage.cs into the project.
In this file:
1). Define an enum type which will be shown in the Option Page:

public enum MyEnumProperty {
    None,
    First,
    Second,
    Third,
}

2). Create a class named OptionsPage which derived from DialogPage class, add
a MyEnumProperty property in it:

class OptionsPage : DialogPage
{
    #region Fields
    private MyEnumProperty myProperty = MyEnumProperty.None;
    #endregion Fields

    #region Properties
    [Category("Enum Options")]
    [Description("My enum option")]
    [TypeConverter(typeof(EnumTypeConverter))]
    public MyEnumProperty MyProperty
    {
        get
        {
            return myProperty;
        }
        set
        {
            myProperty = value;
        }
    }
    #endregion Properties
}

Step3. Add another new class file named EnumTypeConverter.cs into the project,
then create a class named EnumTypeConverter which derived from EnumConverter,
implement the methods of EnumConverter as below:

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

Step4. Open the VSPackage.resx file, add two String resource as below:

<data name="113" xml:space="preserve">
<value>MyOptionsPage</value>
</data>
<data name="114" xml:space="preserve">
<value>OptionsPageWithTypeConverter</value>
</data>

Step5. Assign ProvideOptionPage attribute to the package class for
registering our Option Page:

[ProvideOptionPage(typeof(OptionsPage), "MyOptionsPage",
 "OptionsPageWithTypeConverter", 113, 114, true)]
public sealed class CSVSPackageOptionPageWithTypeConverterPackage : Package

Step6. Build the solution, press F5, you will see our Option Page appears in
Tools -> Options, and the TypeConverter works as expect.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Walkthrough: Creating an Options Page (C#)
http://msdn.microsoft.com/en-us/library/bb166195(VS.80).aspx

MSDN: How to: Implement a Type Converter
http://msdn.microsoft.com/en-us/library/ayybcxe5.aspx


/////////////////////////////////////////////////////////////////////////////
