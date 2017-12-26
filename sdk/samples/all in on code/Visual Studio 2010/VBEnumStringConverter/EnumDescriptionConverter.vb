'********************************** Module Header **************************************'
' Module Name:      EnumDescriptionConverter.vb 
' Project:          VBEnumStringConverter
' Copyright (c)     Microsoft Corporation. 
'
' The file provides a type converter to convert Enum objects to and from the description
' string specified by the DescriptionAttribute: EnumDescriptionConverter. The class 
' derives from the built-in EnumConverter class.  Different from EnumConverter which
' does not take the DescriptionAttribute into consideration, EnumDescriptionConverter
' converts an enum object to its description string if the DescriptionAttribute is
' attached to the object. When the class is used to convert a string to an enum object,
' the class also attempts to match the string with the description string specified by
' the DescriptionAttribute.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE. 
'\**************************************************************************************'


Imports System.ComponentModel
Imports System.Globalization
Imports System.Reflection
Imports System.Text


''' <summary>
''' Provides a type converter to convert Enum objects to and from the description string
''' specified by the DescriptionAttribute.  The class derives from the built-in 
''' EnumConverter class.  Different from EnumConverter which does not take the
''' DescriptionAttribute into consideration, EnumDescriptionConverter converts an enum 
''' object to its description string if the DescriptionAttribute is attached to the 
''' object. When the class is used to convert a string to an enum object, the class also 
''' attempts to match the string with the description string specified by the
''' DescriptionAttribute.
''' </summary>
Public Class EnumDescriptionConverter
    Inherits EnumConverter
    Private Const enumSeperator As Char = ","c

    Public Sub New(type As Type)
        MyBase.New(type)
    End Sub


#Region " ConvertFrom (String)"

    ''' <summary>
    ''' Converts the given value to the enum type of this converter. If the given value
    ''' is a string, the method will attempt to match the string with the names, and the
    ''' description strings specified by the DescriptionAttribute of each enum object.
    ''' </summary>
    ''' <param name="context">
    ''' An ITypeDescriptorContext that provides a format context.
    ''' </param>
    ''' <param name="culture">
    ''' An optional CultureInfo. If not supplied, the current culture is assumed.
    ''' </param>
    ''' <param name="value">The enum object to convert. </param>
    ''' <returns>
    ''' An enum object that represents the converted value.
    ''' </returns>
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

    ''' <summary>
    ''' Converts the string representation of the name, or the description string 
    ''' specified by the DescriptionAttribute, or numeric value of one or more enumerated
    ''' constants to an equivalent enumerated object. 
    ''' </summary>
    ''' <param name="enumType">An enumeration type. </param>
    ''' <param name="value">
    ''' A string containing the name or the description or value to convert.
    ''' </param>
    ''' <returns>
    ''' An object of type enumType whose value is represented by value.
    ''' </returns>
    ''' <remarks>
    ''' Different from Enum.Parse that disregards the DescriptionAttribute, this method
    ''' can also convert the description string to an equivalent enum object.
    ''' The comparison of strings is not case sensitive.
    ''' </remarks>
    Private Shared Function Parse(enumType As Type, value As String) As Object
        If enumType = Nothing Then
            Throw New ArgumentNullException("enumType")
        End If

        If Not enumType.IsEnum Then
            Throw New ArgumentException("Type provided must be an Enum." & vbCr & vbLf &
                                        "Parameter name: enumType")
        End If

        If value = Nothing Then
            Throw New ArgumentNullException("value")
        End If

        value = value.Trim()
        If value.Length = 0 Then
            Throw New ArgumentException(
                "Must specify valid information for parsing in the string.")
        End If

        If [Char].IsDigit(value(0)) OrElse value(0) = "-"c OrElse value(0) = "+"c Then
            Dim underlyingType As Type = [Enum].GetUnderlyingType(enumType)
            Dim temp As Object
            Try
                temp = Convert.ChangeType(value, underlyingType,
                                          CultureInfo.InvariantCulture)
                Return [Enum].ToObject(enumType, temp)
                ' We need to Parse this a String instead. There are cases 
                ' when you tlbimp enum that can have values of the form "3D".
            Catch generatedExceptionName As FormatException
            End Try
        End If

        Dim result As ULong = 0
        Dim dict As Dictionary(Of String, ULong) = GetStringToEnumDictionary(enumType)

        For Each v As String In value.Split(enumSeperator)
            Dim nameOrDesc As String = v.Trim()
            If dict.ContainsKey(nameOrDesc) Then
                result = result Or dict(nameOrDesc)
            Else
                Throw New ArgumentException([String].Format(
                                          "Requested value '{0}' was not found.", value))
            End If
        Next

        Return [Enum].ToObject(enumType, result)
    End Function

    Private Shared fieldInfoHash As Hashtable = Hashtable.Synchronized(New Hashtable())
    Private Const maxHashElements As Integer = 100
    ' To trim the working set
    Private Shared Function GetStringToEnumDictionary(enumType _
                                         As Type) As Dictionary(Of String, ULong)
        Debug.Assert(enumType <> Nothing)
        Debug.Assert(enumType.IsEnum)

        Dim dict As Dictionary(Of String, ULong) = DirectCast(fieldInfoHash(enumType), 
            Dictionary(Of String, ULong))
        If dict Is Nothing Then
            ' To reduce the working set we clear the HashTable when a 
            ' threshold number of elements are inserted.
            If fieldInfoHash.Count > maxHashElements Then
                fieldInfoHash.Clear()
            End If

            ' Create a dictionary with a case-insensitive string comparer.
            dict = New Dictionary(Of String, ULong)(StringComparer.OrdinalIgnoreCase)

            Dim fields As FieldInfo() = enumType.GetFields(BindingFlags.[Static] Or
                                                           BindingFlags.[Public])
            For Each info As FieldInfo In fields
                Dim enumValue As ULong = Convert.ToUInt64(info.GetValue(info.Name))

                ' Add the <name, enumValue> pair.
                dict.Add(info.Name, enumValue)

                ' Add the <description, enumValue> pair if available.
                Dim attrs As DescriptionAttribute() =
                    DirectCast(info.GetCustomAttributes(GetType(DescriptionAttribute),
                                                        False), DescriptionAttribute())
                If attrs.Length > 0 Then
                    dict.Add(attrs(0).Description, enumValue)
                End If
            Next

            fieldInfoHash.Add(enumType, dict)
        End If
        Return dict
    End Function


#End Region


#Region " CovertFrom (Enum Value)"

    ''' <summary>
    ''' Converts the given enum value object to the specified destination type. If the 
    ''' destination type is string, the method converts the enum value to its description
    ''' string when the DescriptionAttribute is attached to the enum value. 
    ''' </summary>
    ''' <param name="context">
    ''' An ITypeDescriptorContext that provides a format context.
    ''' </param>
    ''' <param name="culture">
    ''' An optional CultureInfo. If not supplied, the current culture is assumed.
    ''' </param>
    ''' <param name="value">The enum object to convert</param>
    ''' <param name="destinationType">
    ''' The Type to convert the value to.
    ''' </param>
    ''' <returns>
    ''' An object that represents the converted value.
    ''' </returns>
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
#End Region


End Class
