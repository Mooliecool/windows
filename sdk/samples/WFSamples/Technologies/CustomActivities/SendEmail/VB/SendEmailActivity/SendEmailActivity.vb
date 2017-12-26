'---------------------------------------------------------------------
'   This file is part of the Windows Workflow Foundation SDK Code Samples.
'  
'   Copyright (C) Microsoft Corporation.  All rights reserved.
'  
' This source code is intended only as a supplement to Microsoft
' Development Tools and/or on-line documentation.  See these other
' materials for detailed information regarding Microsoft code samples.
'  
' THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
' KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
' IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.ComponentModel
Imports System.ComponentModel.Design
Imports System.Collections
Imports System.Drawing
Imports System.Net.Mail
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Serialization
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.Runtime
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules

<ActivityValidator(GetType(SendEmailValidator))> _
<ToolboxBitmap(GetType(SendEmailActivity), "Resources.EmailMessage.png")> _
<DefaultEvent("SendingEmail")> _
<DefaultProperty("To")> _
Public Class SendEmailActivity
    Inherits Activity

    ' Define the DependencyProperty objects for all of the Properties 
    ' ...and Events exposed by this activity
    Public Shared FromEmailProperty As DependencyProperty = DependencyProperty.Register("FromEmail", GetType(String), GetType(SendEmailActivity), New PropertyMetadata("someone@example.com"))
    Public Shared ToProperty As DependencyProperty = DependencyProperty.Register("To", GetType(String), GetType(SendEmailActivity), New PropertyMetadata("someone@example.com"))
    Public Shared BodyProperty As DependencyProperty = DependencyProperty.Register("Body", GetType(String), GetType(SendEmailActivity))
    Public Shared SubjectProperty As DependencyProperty = DependencyProperty.Register("Subject", GetType(String), GetType(SendEmailActivity))
    Public Shared HtmlBodyProperty As DependencyProperty = DependencyProperty.Register("HtmlBody", GetType(Boolean), GetType(SendEmailActivity), New PropertyMetadata(False))
    Public Shared CCProperty As DependencyProperty = DependencyProperty.Register("CC", GetType(String), GetType(SendEmailActivity))
    Public Shared BccProperty As DependencyProperty = DependencyProperty.Register("Bcc", GetType(String), GetType(SendEmailActivity))
    Public Shared PortProperty As DependencyProperty = DependencyProperty.Register("Port", GetType(Integer), GetType(SendEmailActivity), New PropertyMetadata(25))
    Public Shared SmtpHostProperty As DependencyProperty = DependencyProperty.Register("SmtpHost", GetType(String), GetType(SendEmailActivity), New PropertyMetadata("localhost"))
    Public Shared ReplyToProperty As DependencyProperty = DependencyProperty.Register("ReplyTo", GetType(String), GetType(SendEmailActivity))

    Public Shared SendingEmailEvent As DependencyProperty = DependencyProperty.Register("SendingEmail", GetType(EventHandler), GetType(SendEmailActivity), New PropertyMetadata())
    Public Shared SentEmailEvent As DependencyProperty = DependencyProperty.Register("SentEmail", GetType(EventHandler), GetType(SendEmailActivity), New PropertyMetadata())

    ' Define constant values for the Property categories.  
    Private Const MessagePropertiesCategory As String = "Email Message"
    Private Const SMTPPropertiesCategory As String = "Email Server"
    Private Const EventsCategory As String = "Handlers"

#Region "Email Message Properties"

    <DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)> _
    <BrowsableAttribute(True)> _
    <DescriptionAttribute("The To property is used to specify the recipient's email address.")> _
    <CategoryAttribute(MessagePropertiesCategory)> _
    Public Property EmailTo() As String
        Get
            Return CType(MyBase.GetValue(SendEmailActivity.ToProperty), String)
        End Get
        Set(ByVal value As String)
            MyBase.SetValue(SendEmailActivity.ToProperty, value)
        End Set
    End Property

    <DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)> _
    <BrowsableAttribute(True)> _
    <DescriptionAttribute("The Subject property is used to specify the subject of the Email message.")> _
    <CategoryAttribute(MessagePropertiesCategory)> _
    Public Property Subject() As String
        Get
            Return CType(MyBase.GetValue(SendEmailActivity.SubjectProperty), String)
        End Get
        Set(ByVal value As String)
            MyBase.SetValue(SendEmailActivity.SubjectProperty, value)
        End Set
    End Property

    <DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)> _
    <BrowsableAttribute(True)> _
    <DescriptionAttribute("The From property is used to specify the From (Sender's) address for the email mesage.")> _
    <CategoryAttribute(MessagePropertiesCategory)> _
    Public Property FromEmail() As String
        Get
            Return CType(MyBase.GetValue(SendEmailActivity.FromEmailProperty), String)
        End Get
        Set(ByVal value As String)
            MyBase.SetValue(SendEmailActivity.FromEmailProperty, value)
        End Set
    End Property

    <DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)> _
    <BrowsableAttribute(True)> _
    <DescriptionAttribute("The Body property is used to specify the Body of the email message.")> _
    <CategoryAttribute(MessagePropertiesCategory)> _
    Public Property Body() As String
        Get
            Return CType(MyBase.GetValue(SendEmailActivity.BodyProperty), String)
        End Get
        Set(ByVal value As String)
            MyBase.SetValue(SendEmailActivity.BodyProperty, value)
        End Set
    End Property

    <DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)> _
    <Description("The HTMLBody property is used to specify whether the Body is formatted as HTML (True) or not (False)")> _
    <Category(MessagePropertiesCategory)> _
    <Browsable(True)> _
    <DefaultValue(False)> _
    Public Property HtmlBody() As Boolean
        Get
            Return CType(MyBase.GetValue(SendEmailActivity.HtmlBodyProperty), Boolean)
        End Get
        Set(ByVal value As Boolean)
            MyBase.SetValue(SendEmailActivity.HtmlBodyProperty, value)
        End Set
    End Property


    <DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)> _
    <Description("The CC property is used to set the CC recipients for the email message.")> _
    <Category(MessagePropertiesCategory)> _
    <Browsable(True)> _
    Public Property CC() As String
        Get
            Return CType(MyBase.GetValue(SendEmailActivity.CCProperty), String)
        End Get
        Set(ByVal value As String)
            MyBase.SetValue(SendEmailActivity.CCProperty, value)
        End Set
    End Property

    <DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)> _
    <Description("The Bcc property is used to set the Bcc recipients for the email message.")> _
    <Category(MessagePropertiesCategory)> _
    <Browsable(True)> _
    Public Property Bcc() As String
        Get
            Return CType(MyBase.GetValue(SendEmailActivity.BccProperty), String)
        End Get
        Set(ByVal value As String)
            MyBase.SetValue(SendEmailActivity.BccProperty, value)
        End Set
    End Property

    <DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)> _
    <Description("The email address that should be used for reply messages.")> _
    <Category(MessagePropertiesCategory)> _
    <Browsable(True)> _
    Public Property ReplyTo() As String
        Get
            Return CType(MyBase.GetValue(SendEmailActivity.ReplyToProperty), String)
        End Get
        Set(ByVal value As String)
            MyBase.SetValue(SendEmailActivity.ReplyToProperty, value)
        End Set
    End Property

#End Region

#Region "SMTP Properties"

    <DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)> _
    <Description("The SMTP host is the machine running SMTP that sends the email.  The default is 'localhost'")> _
    <Category(SMTPPropertiesCategory)> _
    <Browsable(True)> _
    Public Property SmtpHost() As String
        Get
            Return CType(MyBase.GetValue(SendEmailActivity.SmtpHostProperty), String)
        End Get
        Set(ByVal value As String)
            MyBase.SetValue(SendEmailActivity.SmtpHostProperty, value)
        End Set
    End Property


    <DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)> _
    <Description("Specify the Port used for SMTP.  The default is 25.")> _
    <Category(SMTPPropertiesCategory)> _
    <Browsable(True)> _
    Public Property Port() As Integer
        Get
            Return CType(MyBase.GetValue(SendEmailActivity.PortProperty), Integer)
        End Get
        Set(ByVal value As Integer)
            MyBase.SetValue(SendEmailActivity.PortProperty, value)
        End Set
    End Property

#End Region

#Region "Public Events"

    <DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)> _
    <Description("The SendingEmail event is raised before an email is sent through SMTP.")> _
    <Category(EventsCategory)> _
    <Browsable(True)> _
    Public Custom Event SendingEmail As EventHandler
        AddHandler(ByVal value As EventHandler)
            MyBase.AddHandler(SendEmailActivity.SendingEmailEvent, value)
        End AddHandler

        RemoveHandler(ByVal value As EventHandler)
            MyBase.RemoveHandler(SendEmailActivity.SendingEmailEvent, value)
        End RemoveHandler

        RaiseEvent(ByVal sender As Object, ByVal e As System.EventArgs)

        End RaiseEvent
    End Event

    <DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)> _
    <Description("The SentEmail event is raised after an email is sent through SMTP.")> _
    <Category(EventsCategory)> _
    <Browsable(True)> _
    Public Custom Event SentEmail As EventHandler
        AddHandler(ByVal value As EventHandler)
            MyBase.AddHandler(SendEmailActivity.SentEmailEvent, value)
        End AddHandler

        RemoveHandler(ByVal value As EventHandler)
            MyBase.RemoveHandler(SendEmailActivity.SentEmailEvent, value)
        End RemoveHandler

        RaiseEvent(ByVal sender As Object, ByVal e As System.EventArgs)

        End RaiseEvent
    End Event

#End Region

#Region "Activity Execution Logic"


    ' During execution the SendEmail activity should create and send the email using SMTP.  

    Protected Overrides Function Execute(ByVal context As ActivityExecutionContext) As ActivityExecutionStatus
        Try
            ' Raise the SendingEmail event to the parent workflow or activity
            MyBase.RaiseEvent(SendEmailActivity.SendingEmailEvent, Me, EventArgs.Empty)

            ' Send the email now
            Me.SendEmailUsingSmtp()

            ' Raise the SentEmail event to the parent workflow or activity
            MyBase.RaiseEvent(SendEmailActivity.SentEmailEvent, Me, EventArgs.Empty)

            ' Return the closed status indicating that this activity is complete.
            Return ActivityExecutionStatus.Closed
        Catch
            ' An unhandled exception occurred.  Throw it back to the WorkflowRuntime.
            Throw
        End Try
    End Function

    Private Sub SendEmailUsingSmtp()
        ' Create a new SmtpClient for sending the email
        Dim client As New SmtpClient()

        ' Use the properties of the activity to construct a new MailMessage
        Dim message As New MailMessage()
        message.From = New MailAddress(Me.FromEmail)
        message.To.Add(Me.EmailTo)


        ' Assign the message values if they are valid.
        If Not String.IsNullOrEmpty(Me.CC) Then
            message.CC.Add(Me.CC)
        End If

        If Not String.IsNullOrEmpty(Me.Bcc) Then
            message.Bcc.Add(Me.Bcc)
        End If

        If Not String.IsNullOrEmpty(Me.Subject) Then
            message.Subject = Me.Subject
        End If

        If Not String.IsNullOrEmpty(Me.Body) Then
            message.Body = Me.Body
        End If

        If Not String.IsNullOrEmpty(Me.ReplyTo) Then
            message.ReplyTo = New MailAddress(Me.ReplyTo)
        End If

        message.IsBodyHtml = Me.HtmlBody

        ' Set the SMTP host and send the mail
        client.Host = Me.SmtpHost
        client.Port = Me.Port
        client.Send(message)
    End Sub
#End Region
End Class
