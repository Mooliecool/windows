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
Imports System.ComponentModel.Design
Imports System.Collections
Imports System.Drawing
Imports System.Reflection
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Serialization
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.Runtime
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class SendEmailWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Me.sendEmail = New SendEmailActivity
        '
        'sendEmail
        '
        Me.sendEmail.Bcc = Nothing
        Me.sendEmail.Body = Nothing
        Me.sendEmail.CC = Nothing
        Me.sendEmail.EmailTo = "someone@example.com"
        Me.sendEmail.FromEmail = "someone@example.com"
        Me.sendEmail.Name = "sendEmail"
        Me.sendEmail.Port = 25
        Me.sendEmail.ReplyTo = Nothing
        Me.sendEmail.SmtpHost = "localhost"
        Me.sendEmail.Subject = "Test Email From Workflow"
        AddHandler Me.sendEmail.SendingEmail, AddressOf Me.sendEmail_SendingEmail
        AddHandler Me.sendEmail.SentEmail, AddressOf Me.sendEmail_SentEmail
        '
        'SendEmailWorkflow
        '
        Me.Activities.Add(Me.sendEmail)
        Me.Name = "SendEmailWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private WithEvents sendEmail As SendEmailActivity

End Class
