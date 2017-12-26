'***************************** Module Header *******************************'
' Module Name:  MainForm.vb
' Project:	    VBRegisterHotkey
' Copyright (c) Microsoft Corporation.
' 
' This is the main form of this application. It is used to initialize the UI
' and handle the events.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'


Partial Public Class MainForm
    Inherits Form

    Private _hotKeyToRegister As HotKeyRegister = Nothing

    Private _registerKey As Keys = Keys.None

    Private _registerModifiers As KeyModifiers = KeyModifiers.None

    Public Sub New()
        InitializeComponent()
    End Sub


    ''' <summary>
    ''' Handle the KeyDown of tbHotKey. In this event handler, check the pressed keys.
    ''' The keys that must be pressed in combination with the key Ctrl, Shift or Alt,
    ''' like Ctrl+Alt+T. The method HotKeyRegister.GetModifiers could check whether 
    ''' "T" is pressed.
    ''' </summary>
    Private Sub tbHotKey_KeyDown(ByVal sender As Object, ByVal e As KeyEventArgs) _
        Handles tbHotKey.KeyDown
        '  The key event should not be sent to the underlying control.
        e.SuppressKeyPress = True

        ' Check whether the modifier keys are pressed.
        If e.Modifiers <> Keys.None Then
            Dim key As Keys = Keys.None
            Dim modifiers As KeyModifiers = HotKeyRegister.GetModifiers(e.KeyData, key)

            ' The pressed key is valid.
            If key <> Keys.None Then

                Me._registerKey = key
                Me._registerModifiers = modifiers

                ' Display the pressed key in the textbox.
                tbHotKey.Text = String.Format("{0}+{1}",
                    Me._registerModifiers, Me._registerKey)

                ' Enable the button.
                btnRegister.Enabled = True
            End If
        End If
    End Sub


    ''' <summary>
    ''' Handle the Click event of btnRegister.
    ''' </summary>
    Private Sub btnRegister_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnRegister.Click

        Try
            ' Register the hotkey.
            _hotKeyToRegister = New HotKeyRegister(Me.Handle, 100,
                Me._registerModifiers, Me._registerKey)

            ' Register the HotKeyPressed event.
            AddHandler _hotKeyToRegister.HotKeyPressed, AddressOf HotKeyPressed

            ' Update the UI.
            btnRegister.Enabled = False
            tbHotKey.Enabled = False
            btnUnregister.Enabled = True

        Catch _argumentException As ArgumentException
            MessageBox.Show(_argumentException.Message)
        Catch _applicationException As ApplicationException
            MessageBox.Show(_applicationException.Message)
        End Try
    End Sub


    ''' <summary>
    ''' Show a message box if the HotKeyPressed event is raised.
    ''' </summary>
    Private Sub HotKeyPressed(ByVal sender As Object, ByVal e As EventArgs)

        If Me.WindowState = FormWindowState.Minimized Then
            Me.WindowState = FormWindowState.Normal
        End If
        Me.Activate()

    End Sub


    ''' <summary>
    ''' Handle the Click event of btnUnregister.
    ''' </summary>
    Private Sub btnUnregister_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnUnregister.Click

        ' Dispose the hotKeyToRegister.
        If _hotKeyToRegister IsNot Nothing Then
            _hotKeyToRegister.Dispose()
            _hotKeyToRegister = Nothing
        End If

        ' Update the UI.
        tbHotKey.Enabled = True
        btnRegister.Enabled = True
        btnUnregister.Enabled = False
    End Sub


    ''' <summary>
    ''' Dispose the hotKeyToRegister when the form is closed.
    ''' </summary>
    Protected Overrides Sub OnClosed(ByVal e As EventArgs)
        If _hotKeyToRegister IsNot Nothing Then
            _hotKeyToRegister.Dispose()
            _hotKeyToRegister = Nothing
        End If

        MyBase.OnClosed(e)
    End Sub

End Class
