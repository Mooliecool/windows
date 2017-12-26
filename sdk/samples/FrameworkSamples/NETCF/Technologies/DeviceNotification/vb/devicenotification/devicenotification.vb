'---------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
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


Imports Microsoft.WindowsCE.Forms
Imports System.Text.StringBuilder

Public Class NotificationSample
    Private notify As New Notification

    Private Sub Notification_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load, MyBase.Load, MyBase.Load, MyBase.Load, MyBase.Load, MyBase.Load
        'this sub will populate notify with the default data

        'Filling NotifyText with html for notification layout
        'This html defines the Notification bubble.
        Dim notifyHtml As System.Text.StringBuilder

        notifyHtml = New System.Text.StringBuilder("<html><body>")
        notifyHtml.Append("<a href=""URL Here"">Test Link</a>")
        notifyHtml.Append("<p><form method=""GET"" action=mybubble>")
        notifyHtml.Append("<p>This is an <font color=""#0000FF""><b>HTML</b></font> notification stored in a  <font color=""#FF0000""><i>string</i></font> table!</p>")
        notifyHtml.Append("<p><input type=text name=textinput value=""Input Sample""><input type='submit'></p>")
        notifyHtml.Append("<p align=right><input type=button name=OK value='Ok'> <input type=button name='cmd:2' value='Cancel'></p>")
        notifyHtml.Append("</body></html>")

        'Set notifyHtml text to NotifyText Control
        Me.NotifyText.Text = notifyHtml.ToString

        'Setting Notify with default data from controls
        notify.Caption = Me.NotifyCaption.Text
        notify.Critical = Me.NotifyCritical.Checked
        notify.InitialDuration = CInt(Me.NotifyDuration.Text)
        notify.Text = Me.NotifyText.Text

        Me.NotifyIcon.Text = "Icon1"

        'Add response and Change Handlers
        AddHandler notify.BalloonChanged, AddressOf OnNotifyBubChange
        AddHandler notify.ResponseSubmitted, AddressOf OnNotifyResponse

    End Sub

    Private Sub NotifyCaption_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles NotifyCaption.TextChanged
        'Sets Notify Caption to the content of the Forms NotifyCaption text box.
        notify.Caption = Me.NotifyCaption.Text
    End Sub

    Private Sub NotifyText_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles NotifyText.TextChanged
        'Sets Notify Text to the content of the Forms NotifyText text box.
        notify.Text = Me.NotifyText.Text
    End Sub

    Private Sub NotifyIcon_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles NotifyIcon.SelectedIndexChanged
        'Sets Notify Icon to the matching icon selection
        Select Case Me.NotifyIcon.Text
            Case "Icon1"
                notify.Icon = My.Resources.icon1
            Case "Icon2"
                notify.Icon = My.Resources.bel
        End Select
    End Sub

    Private Sub NotifyCritical_CheckStateChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles NotifyCritical.CheckStateChanged
        'Sets Notify Critical to checked state of NotifyCritical on the form
        notify.Critical = Me.NotifyCritical.Checked
    End Sub

    Private Sub NotifyShow_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles NotifyShow.Click
        'Makes Notify visible
        notify.Visible = True
    End Sub

    Private Sub NotifyHide_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles NotifyHide.Click
        'Hides Notify object
        notify.Visible = False
    End Sub

    Protected Sub OnNotifyBubChange(ByVal obj As Object, ByVal e As BalloonChangedEventArgs)
        'Sub handeles Nofificaiton Change
        Me.NotifyVisable.Text = e.Visible.ToString
    End Sub

    Protected Sub OnNotifyResponse(ByVal obj As Object, ByVal e As ResponseSubmittedEventArgs)
        'Sub handles response from Notification
        Me.NotifyReponce.Text = e.Response.ToString
    End Sub

    Private Sub NewNotificationAdd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles NewNotificationAdd.Click
        'Sub creates a new notification to show how two notifications behave.  This notificaiton will not
        'report back to the form.

        Dim notify2 As New Notification

        notify2.Text = "<html><body><p>Sample Notification<p/><p align=right><input type=button name=OK value='Ok'> <input type=button name='Cancel' value='Cancel'></p></body></html>"
        notify2.Icon = My.Resources.cedialer
        notify2.Visible = True
    End Sub


    Private Sub NumericUpDown1_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles NotifyDuration.ValueChanged
        notify.InitialDuration = Me.NotifyDuration.Value
    End Sub

    Private Sub NotificationSample_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles Me.Closing
        'Clean up the Nofity object
        notify.Dispose()
    End Sub


End Class
