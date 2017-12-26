'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.Workflow.ComponentModel

Public Class BaseOutlookItem
    Inherits Activity

    Dim toValue As String
    Public Property [To]() As String
        Get
            Return Me.toValue
        End Get
        Set(ByVal value As String)

        End Set
    End Property

    Dim subjectValue As String
    Public Property Subject() As String
        Get
            Return Me.subjectValue
        End Get
        Set(ByVal value As String)
            Me.subjectValue = value
        End Set
    End Property

    Dim bodyValue As String
    Public Property Body() As String
        Get
            Return Me.bodyValue
        End Get
        Set(ByVal value As String)
            Me.bodyValue = value
        End Set
    End Property
End Class
