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
Imports System.ComponentModel
Imports System.Workflow.ComponentModel

Public Class BaseMailbox
    Inherits Activity

    Dim filterOptionValue As FilterOption
    Public Property Filter() As FilterOption
        Get
            Return Me.filterOptionValue
        End Get
        Set(ByVal value As FilterOption)
            Me.filterOptionValue = value
        End Set
    End Property

    Dim filterValueValue As String
    Public Property FilterValue() As String
        Get
            Return Me.filterValueValue
        End Get
        Set(ByVal value As String)
            Me.filterValueValue = value
        End Set
    End Property
End Class

Public Enum FilterOption
    <Description("Subject")> _
    Subject = 0
    <Description("From")> _
    FromEmail = 1
    <Description("To")> _
    [To] = 2
    <Description("CC")> _
    CC = 3
    <Description("BCC")> _
    Bcc = 4
End Enum