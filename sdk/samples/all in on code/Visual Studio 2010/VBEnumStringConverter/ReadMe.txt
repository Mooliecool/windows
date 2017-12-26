=========================================================================================
       CONSOLE APPLICATION : VBEnumStringConverter Project Overview
=========================================================================================

/////////////////////////////////////////////////////////////////////////////////////////
Summary: 

This sample demonstrates how to convert enum to comma separated string & vice versa. It
also covers the description attribute for enum while conversion. 


/////////////////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the enum-string 
convertion sample.

Step 1. Build and run the sample solution in Visual Studio 2010.

Step 2. During first conversion process it will use the .NET framework built-in 
        EnumConverter class to convert string to enum & back to string.

Step 3. During second conversion process it will use the EnumDescriptionConverter
        class to convert the description string to enum & back to string.


/////////////////////////////////////////////////////////////////////////////////////////
Implementation:

Following enum of ProgrammingLanguage has been used for example.
		<Flags()> _
		Enum ProgrammingLanguage
			<Description("Visual Basic")> _
			VB = &H1
			<Description("Visual C#")> _
			CS = &H2
			<Description("Visual C++")> _
			Cpp = &H4
			<Description("Javascript")> _
			JS = &H8
			' XAML
			XAML = &H10
		End Enum

System.ComponentModel.EnumConverter supports the conversion of one type to another which
has been used in the first conversion process.

		Dim converter As New EnumConverter(GetType(ProgrammingLanguage))

        ' Convert string to enum.
        Dim langStr As String = "VB, CS, Cpp, XAML"
        Dim lang As ProgrammingLanguage =
            DirectCast(converter.ConvertFromString(langStr), ProgrammingLanguage)

		' Convert enum to string.
		langStr = converter.ConvertToString(lang)
    
 EnumDescriptionConverter class inherits EnumConverter to support Description  attributes
 in the second conversion process.

1. It will split the comma separated string into string array and then convert to
   long & do bitwise ORing to get the long value representing the enum value.
    
        // convert from string to enum
		Public Overrides Function ConvertFrom(context As ITypeDescriptorContext,
											  culture As CultureInfo,
											  value As Object) As Object
			If TypeOf value Is String Then
				Dim strValue As String = DirectCast(value, String)
				If strValue.IndexOf(enumSeperator) <> -1 Then
					Dim convertedValue As ULong = 0
					For Each v As String In strValue.Split(enumSeperator)
						convertedValue = convertedValue Or
							Convert.ToUInt64(Parse(Me.EnumType, v), culture)
					Next
					Return [Enum].ToObject(Me.EnumType, convertedValue)
				Else
					Return Parse(Me.EnumType, strValue)
				End If
			End If

			Return MyBase.ConvertFrom(context, culture, value)
		End Function


2. It will read the custom attributes from the given enum, in this case Description 
   attribute & prepare the corresponding string value. In case of flags enum, it will
   fetch Description attribute array & prepare comma separated string as output.

    // convert to string
    Public Overrides Function ConvertTo(context As ITypeDescriptorContext,
                                        culture As CultureInfo,
                                        value As Object,
                                        destinationType As Type) As Object
        If destinationType = Nothing Then
            Throw New ArgumentNullException("destinationType")
        End If

        If destinationType = GetType(String) AndAlso value <> Nothing Then
            ' Raise an argument exception if the value isn't defined and if 
            ' the enum isn't a flags style.
            '
            Dim underlyingType As Type = [Enum].GetUnderlyingType(Me.EnumType)
            If TypeOf value Is IConvertible AndAlso
                value.[GetType]() <> underlyingType Then
                value = (DirectCast(value, IConvertible)).ToType(underlyingType, culture)
            End If
            If Not Me.EnumType.IsDefined(GetType(FlagsAttribute), False) AndAlso
                Not [Enum].IsDefined(Me.EnumType, value) Then
                Throw New ArgumentException([String].Format(
                            "The value '{0}' is not a valid value for the enum '{1}'.",
                            value.ToString(), Me.EnumType.Name))
            End If

            ' If the enum value is decorated with the Description attribute, 
            ' return the Description value; otherwise return the name.
            Dim enumName As String = [Enum].Format(Me.EnumType, value, "G")
            Dim nameOrDesc As String

            If enumName.IndexOf(enumSeperator) <> -1 Then
                ' This is a flags enum. Split the descriptions with ', '.
                Dim firstTime As Boolean = True
                Dim retval As New StringBuilder()

                For Each v As String In enumName.Split(enumSeperator)
                    nameOrDesc = v.Trim()

                    Dim info As FieldInfo = Me.EnumType.GetField(nameOrDesc)
                    Dim attrs As DescriptionAttribute() =
                        DirectCast(info.GetCustomAttributes(GetType(DescriptionAttribute),
                                                            False), 
                                                        DescriptionAttribute())
                    If attrs.Length > 0 Then
                        nameOrDesc = attrs(0).Description
                    End If

                    If firstTime Then
                        retval.Append(nameOrDesc)
                        firstTime = False
                    Else
                        retval.Append(enumSeperator)
                        retval.Append(" "c)
                        retval.Append(nameOrDesc)
                    End If
                Next

                Return retval.ToString()
            Else
                Dim info As FieldInfo = Me.EnumType.GetField(enumName)
                If info <> Nothing Then
                    Dim attrs As DescriptionAttribute() =
                       DirectCast(info.GetCustomAttributes(GetType(DescriptionAttribute),
                                                          False), DescriptionAttribute())
                    nameOrDesc = If((attrs.Length > 0), attrs(0).Description, enumName)
                Else
                    nameOrDesc = enumName
                End If
                Return nameOrDesc
            End If
        End If

        Return MyBase.ConvertTo(context, culture, value, destinationType)
    End Function


/////////////////////////////////////////////////////////////////////////////////////////
References:

EnumConverter Class
http://msdn.microsoft.com/en-us/library/system.componentmodel.enumconverter.aspx

DescriptionAttribute Members
http://msdn.microsoft.com/en-us/library/system.componentmodel.descriptionattribute_members(v=VS.85).aspx

/////////////////////////////////////////////////////////////////////////////////////////