Imports System.Globalization

''---------------------------------------------------------------------
''  This file is part of the Microsoft .NET Framework SDK Code Samples.
'' 
''  Copyright (C) Microsoft Corporation.  All rights reserved.
'' 
''This source code is intended only as a supplement to Microsoft
''Development Tools and/or on-line documentation.  See these other
''materials for detailed information regarding Microsoft code samples.
'' 
''THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
''KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
''IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
''PARTICULAR PURPOSE.
''---------------------------------------------------------------------
''' <summary>
''' Class that holds a book's Author details
''' </summary>
Public Class Author

    ''' <summary>
    ''' FirstName of the book (private member)
    ''' </summary>
    Private firstNameValue As String

    ''' <summary>
    ''' FirstName of the book (public property)
    ''' </summary>
    Public Property FirstName() As String

        Get
            Return firstNameValue
        End Get

        Set(ByVal value As String)
            firstNameValue = value
        End Set

    End Property


    ''' <summary>
    ''' LastName of the book (private member)
    ''' </summary>
    Private lastNameValue As String

    ''' <summary>
    ''' LastName of the book (public property)
    ''' </summary>
    Public Property LastName() As String
        Get
            Return lastNameValue
        End Get
        Set(ByVal value As String)
            lastNameValue = value
        End Set
    End Property

    ''' <summary>
    ''' Country of the book (private member)
    ''' </summary>
    Private countryValue As String

    ''' <summary>
    ''' Country of the book (public property)
    ''' </summary>
    Public Property Country() As String
        Get
            Return countryValue
        End Get
        Set(ByVal value As String)
            countryValue = value
        End Set
    End Property

    ''' <summary>
    ''' overriden ToString implementation
    ''' </summary>
    ''' <returns></returns>
    Public Overrides Function ToString() As String
        Return String.Format(CultureInfo.CurrentCulture, "{0} {1}", FirstName, LastName)
    End Function

End Class
