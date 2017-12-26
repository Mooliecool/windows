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
Imports System.Windows.Forms

''' <summary>
''' Module which holds shared instances of both SearchForm and GroupForm
''' </summary>
Module Common

    'private instance of SearchForm
    Private searchFormValue As Form


    ''' <summary>
    ''' public  Property that gets/sets shared instance of SearchForm
    ''' </summary>
    Public Property SearchForm() As Form
        Get
            Return Common.searchFormValue
        End Get
        Set(ByVal value As Form)
            Common.searchFormValue = value
        End Set
    End Property

    'private instance of GroupForm
    Private groupFormValue As Form


    ''' <summary>
    ''' public  Property that gets/sets shared instance of GroupForm
    ''' </summary>
    Public Property GroupForm() As Form
        Get
            Return Common.groupFormValue
        End Get
        Set(ByVal value As Form)
            Common.groupFormValue = value
        End Set
    End Property

    ''' <summary>
    ''' CloseAll, closes all instances of the forms (if created)
    ''' </summary>
    Public Sub CloseAll()

        If Not SearchForm Is Nothing Then
            SearchForm.Close()
        End If


        If Not GroupForm Is Nothing Then
            GroupForm.Close()
        End If


    End Sub
End Module

