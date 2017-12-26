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
Imports System.Workflow.Activities
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Design

<ToolboxItemAttribute(GetType(ActivityToolboxItem))> _
Public Class SendEmailActivity
    Inherits Activity

    ' Override Execute with your custom logic
    Protected Overrides Function Execute(ByVal context As ActivityExecutionContext) As ActivityExecutionStatus
        ' Create an instance of CustomActivityEventArgs
        Dim customActivityEventArgs As CustomActivityEventArgs = New CustomActivityEventArgs(Me.Description)
        ' raise the BeforeSendEvent event and pass customActivityEventArgs 
        Me.RaiseGenericEvent(Of CustomActivityEventArgs)(SendingEvent, Me, customActivityEventArgs)

        ' This is where the logic of the e-mail should go
        Console.WriteLine("The Logic to send the e-mail goes here")
        Console.WriteLine("The 'To' property is: " + Me.Recipient.ToString())
        Console.WriteLine("The 'From' property is: " + Me.FromEmail.ToString())

        Return System.Workflow.ComponentModel.ActivityExecutionStatus.Closed
    End Function

    Public Shared RecipientProperty As DependencyProperty = DependencyProperty.Register("Recipient", GetType(System.String), GetType(SendEmailActivity))

    <DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)> _
   <BrowsableAttribute(True)> _
    <DescriptionAttribute("Please specify the email address of the receipent")> _
  Public Property Recipient() As System.String
        Get
            Return CType(MyBase.GetValue(RecipientProperty), [String])

        End Get
        Set(ByVal value As System.String)
            MyBase.SetValue(RecipientProperty, value)

        End Set
    End Property
    Public Shared FromEmailProperty As DependencyProperty = DependencyProperty.Register("FromEmail", GetType(System.String), GetType(SendEmailActivity))

    <DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)> _
   <BrowsableAttribute(True)> _
    <DescriptionAttribute("Please specify the email address of the sender")> _
  Public Property FromEmail() As System.String
        Get
            Return CType(MyBase.GetValue(FromEmailProperty), String)

        End Get
        Set(ByVal value As System.String)
            MyBase.SetValue(FromEmailProperty, value)

        End Set
    End Property

    ' Create a DependencyProperty BeforeSendEvent and the BeforeSend event handler
    Public Shared SendingEvent As DependencyProperty = DependencyProperty.Register("Sending", GetType(EventHandler(Of CustomActivityEventArgs)), GetType(SendEmailActivity))

    <DescriptionAttribute("Use this Handler to do some pre-processing logic")> _
    <DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)> _
    <BrowsableAttribute(True)> _
    <Category("Handlers")> _
    Public Custom Event Sending As EventHandler(Of CustomActivityEventArgs)
        AddHandler(ByVal value As EventHandler(Of CustomActivityEventArgs))
            MyBase.AddHandler(SendingEvent, value)
        End AddHandler

        RemoveHandler(ByVal value As EventHandler(Of CustomActivityEventArgs))
            MyBase.RemoveHandler(SendingEvent, value)
        End RemoveHandler

        RaiseEvent(ByVal sender As Object, ByVal e As CustomActivityEventArgs)

        End RaiseEvent
    End Event
End Class

' Create a CustomActivityEventArgs 
Public Class CustomActivityEventArgs
    Inherits EventArgs

    Private activityDescriptionValue As String

    Public ReadOnly Property ActivityDescription() As String
        Get
            Return activityDescriptionValue
        End Get
    End Property

    Public Sub New(ByVal activityDescription As String)
        Me.activityDescriptionValue = activityDescription
    End Sub
End Class