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
Imports System.Windows.Forms
Imports Outlook = Microsoft.Office.Interop.Outlook

Public Class DummyActivity
    Inherits System.Workflow.ComponentModel.Activity
    Dim TitleProperty As String = ""

    <System.ComponentModel.Description("This is the description which appears in the Property Browser")> _
    <System.ComponentModel.Category("This is the category which will be displayed in the Property Browser")> _
    <System.ComponentModel.Browsable(True)> _
    <System.ComponentModel.DesignerSerializationVisibility(System.ComponentModel.DesignerSerializationVisibility.Visible)> _
    Public Property Title() As String
        Get
            Return TitleProperty
        End Get
        Set(ByVal Value As String)
            TitleProperty = Value
        End Set
    End Property

End Class
