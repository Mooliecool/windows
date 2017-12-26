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
''' Book class that contains the details of a book
''' </summary>
Public Class Book

    ''' <summary>
    ''' Title of the book (private member)
    ''' </summary>
    Private titleValue As String

    ''' <summary>
    ''' Title of the book (public property)
    ''' </summary>
    Public Property Title() As String
        Get
            Return titleValue
        End Get
        Set(ByVal value As String)
            titleValue = value
        End Set
    End Property



    ''' <summary>
    ''' Author of the book (private member)
    ''' </summary>
    Private authorValue As Author

    ''' <summary>
    ''' Author of the book (public property)
    ''' </summary>
    Public Property Author() As Author
        Get
            Return authorValue
        End Get
        Set(ByVal value As Author)
            authorValue = value
        End Set
    End Property

    ''' <summary>
    ''' Price of the book (private member)
    ''' </summary>
    Private priceValue As Double

    ''' <summary>
    ''' Price of the book (public property) 
    ''' </summary>
    Public Property Price() As Double
        Get
            Return priceValue
        End Get
        Set(ByVal value As Double)
            priceValue = value
        End Set
    End Property

    ''' <summary>
    ''' overriden ToString implementation
    ''' </summary>
    ''' <returns></returns>
    Public Overrides Function ToString() As String
        Return String.Format(CultureInfo.CurrentCulture, "Title ={0} , Author ={1} , Price ={2:c} ", Title, Author, Price)
    End Function



End Class
