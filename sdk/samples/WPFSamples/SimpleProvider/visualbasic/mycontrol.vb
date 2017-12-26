 '************************************************************************************************
'
' File: MyControl.cs
'
' Description: Implements a simple custom control that supports UI Automation.
' 
' See ProviderForm.cs for a full description of this sample.
'   
' 
'  This file is part of the Microsoft Windows SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
' This source code is intended only as a supplement to Microsoft
' Development Tools and/or on-line documentation.  See these other
' materials for detailed information regarding Microsoft code samples.
' 
' THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
' KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
' IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
' 
'***********************************************************************************************

Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Windows.Automation.Provider
Imports System.Windows.Automation
Imports System.Drawing
Imports System.Windows.Forms
Imports System.Diagnostics
Imports System.Security.Permissions


Class CustomButton
    Inherits Control
    Implements IRawElementProviderSimple, IInvokeProvider

    Private myHandle As IntPtr
    Private buttonState As Boolean

    ''' <summary>
    ''' Constructor.
    ''' </summary>
    ''' <param name="rect">Position and size of control.</param>
    Public Sub New()
        buttonState = False
        myHandle = Handle

        ' Add event handlers.
        AddHandler MouseDown, AddressOf Me.CustomButton_MouseDown
        AddHandler Me.KeyPress, AddressOf Me.CustomButton_KeyPress
        AddHandler Me.GotFocus, AddressOf CustomButton_ChangeFocus
        AddHandler Me.LostFocus, AddressOf CustomButton_ChangeFocus

    End Sub 'New


    ''' <summary>
    ''' Handles WM_GETOBJECT message; others are passed to base handler.
    ''' </summary>
    ''' <param name="m">Windows message.</param>
    ''' <remarks>This method provides the link with UI Automation.</remarks>
    <PermissionSetAttribute(SecurityAction.Demand, Unrestricted:=True)> _
    Protected Overrides Sub WndProc(ByRef m As Message)
        ' 0x3D == WM_GETOBJECT
        If m.Msg = &H3D AndAlso m.LParam.ToInt32() = AutomationInteropProvider.RootObjectId Then
            m.Result = AutomationInteropProvider.ReturnRawElementProvider(Handle, m.WParam, m.LParam, CType(Me, IRawElementProviderSimple))
            Return
        End If
        MyBase.WndProc(m)
    End Sub 'WndProc


    ''' <summary>
    ''' Ensure that the focus rectangle is drawn or erased when focus changes.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Sub CustomButton_ChangeFocus(ByVal sender As Object, ByVal e As EventArgs)
        Refresh()
    End Sub 'CustomButton_ChangeFocus



    ''' <summary>
    ''' Handles Paint event.
    ''' </summary>
    ''' <param name="e">Event arguments.</param>
    Protected Overrides Sub OnPaint(ByVal e As PaintEventArgs)
        Dim controlRect As New Rectangle(ClientRectangle.Left + 2, _
            ClientRectangle.Top + 2, _
            ClientRectangle.Width - 4, _
            ClientRectangle.Height - 4)
        Dim brush As System.Drawing.Drawing2D.HatchBrush
        If (buttonState) Then
            brush = New System.Drawing.Drawing2D.HatchBrush( _
                Drawing2D.HatchStyle.DarkHorizontal, Color.Red, Color.White)
        Else
            brush = New System.Drawing.Drawing2D.HatchBrush( _
                Drawing2D.HatchStyle.DarkVertical, Color.Green, Color.White)
        End If
        e.Graphics.FillRectangle(Brush, controlRect)
        If Focused Then
            ControlPaint.DrawFocusRectangle(e.Graphics, ClientRectangle)
        End If

    End Sub 'OnPaint


    ''' <summary>
    ''' Responds to a button click, regardless of whether it was caused by a mouse or
    ''' keyboard click or by InvokePattern.Invoke. 
    ''' </summary>
    Private Sub RespondToClick()
        buttonState = Not buttonState
        Me.Focus()
        Me.Refresh()

        ' Raise an event.
        If AutomationInteropProvider.ClientsAreListening Then
            Dim args As New AutomationEventArgs(InvokePatternIdentifiers.InvokedEvent)
            AutomationInteropProvider.RaiseAutomationEvent( _
                InvokePatternIdentifiers.InvokedEvent, Me, args)
        End If

    End Sub 'RespondToClick


    ''' <summary>
    ''' Handles MouseDown event.
    ''' </summary>
    ''' <param name="sender">Object that raised the event.</param>
    ''' <param name="e">Event arguments.</param>
    Public Sub CustomButton_MouseDown(ByVal sender As Object, _
        ByVal e As MouseEventArgs)

        RespondToClick()
    End Sub 'CustomButton_MouseDown


    ''' <summary>
    ''' Handles Keypress event.
    ''' </summary>
    ''' <param name="sender">Object that raised the event.</param>
    ''' <param name="e">Event arguments.</param>
    Public Sub CustomButton_KeyPress(ByVal sender As Object, _
        ByVal e As KeyPressEventArgs)

        If e.KeyChar = ChrW(Keys.Space) Then
            RespondToClick()
        End If

    End Sub 'CustomButton_KeyPress

#Region "IRawElementProviderSimple"


    ''' <summary>
    ''' Returns the object that supports the specified pattern.
    ''' </summary>
    ''' <param name="patternId">ID of the pattern.</param>
    ''' <returns>Object that implements IInvokeProvider.</returns>
    Function GetPatternProvider(ByVal patternId As Integer) As Object _
        Implements IRawElementProviderSimple.GetPatternProvider

        If patternId = InvokePatternIdentifiers.Pattern.Id Then
            Return Me
        Else
            Return Nothing
        End If

    End Function 'IRawElementProviderSimple.GetPatternProvider


    ''' <summary>
    ''' Returns property values.
    ''' </summary>
    ''' <param name="propId">Property identifier.</param>
    ''' <returns>Property value.</returns>
    Function GetPropertyValue(ByVal propId As Integer) As Object _
        Implements IRawElementProviderSimple.GetPropertyValue

        If propId = AutomationElementIdentifiers.ClassNameProperty.Id Then
            Return "CustomButtonControlClass"
        ElseIf propId = AutomationElementIdentifiers.ControlTypeProperty.Id Then
            Return ControlType.Button.Id
        End If
        If propId = AutomationElementIdentifiers.HelpTextProperty.Id Then
            Return "Change the color and pattern."
        End If
        If propId = AutomationElementIdentifiers.IsEnabledProperty.Id Then
            Return True
        Else
            Return Nothing
        End If

    End Function 'IRawElementProviderSimple.GetPropertyValue


    ''' <summary>
    ''' Tells UI Automation that this control is hosted in an HWND, which has 
    ''' its own provider.
    ''' </summary>
    ReadOnly Property HostRawElementProvider() As IRawElementProviderSimple _
        Implements IRawElementProviderSimple.HostRawElementProvider
        Get
            Return AutomationInteropProvider.HostProviderFromHandle(myHandle)
        End Get
    End Property

    ''' <summary>
    ''' Retrieves provider options.
    ''' </summary>
    ReadOnly Property ProviderOptions() As ProviderOptions _
        Implements IRawElementProviderSimple.ProviderOptions
        Get
            Return ProviderOptions.ServerSideProvider
        End Get
    End Property

#End Region


#Region "IInvokeProvider"

    ''' <summary>
    ''' Responds to an InvokePattern.Invoke by simulating a MouseDown event.
    ''' </summary>
    ''' <remarks>
    ''' Use different method name to avoid clashing with Control.Invoke.
    ''' </remarks>
    Sub Invoke1() Implements IInvokeProvider.Invoke
        ' If the control is not enabled, we're responsible for letting UI Automation know.
        ' It catches the exception and then throws it to the client.
        Dim provider As IRawElementProviderSimple = Me
        If False = CBool(provider.GetPropertyValue( _
            AutomationElementIdentifiers.IsEnabledProperty.Id)) Then
            Throw New ElementNotEnabledException()
        End If

        ' Create arguments for the click event. The parameters aren't used.
        Dim mouseArgs As New MouseEventArgs(Windows.Forms.MouseButtons.Left, _
            1, 0, 0, 0)

        ' Simulate a mouse click. We cannot call RespondToClick directly, 
        ' because it is illegal to update the UI from a different thread.
        Dim handler As MouseEventHandler = AddressOf CustomButton_MouseDown
        BeginInvoke(handler, New Object() {Me, mouseArgs})

    End Sub 'IInvokeProvider.Invoke

#End Region

End Class 'CustomButton 

