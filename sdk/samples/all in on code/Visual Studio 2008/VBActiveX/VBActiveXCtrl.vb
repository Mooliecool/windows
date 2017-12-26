'****************************** Module Header ******************************'
' Module Name:  VBActiveXCtrl.vb
' Project:      VBActiveX
' Copyright (c) Microsoft Corporation.
' 
' The sample demonstrates an ActiveX control written in VB.NET. ActiveX 
' controls (formerly known as OLE controls) are small program building blocks 
' that can work in a variety of different containers, ranging from software 
' development tools to end-user productivity tools. For example, it can be  
' used to create distributed applications that work over the Internet through  
' web browsers. ActiveX controls can be written in MFC, ATL, C++, C#, Borland  
' Delphi and Visual Basic. In this sample, we focus on writing an ActiveX 
' control using VB.NET. We will go through the basic steps of adding UI, 
' properties, methods, and events to the control.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Imports directives"

Imports System.ComponentModel
Imports System.Runtime.InteropServices
Imports System.Security.Permissions

#End Region


<ComClass(VBActiveXCtrl.ClassId, VBActiveXCtrl.InterfaceId, _
          VBActiveXCtrl.EventsId)> _
Public Class VBActiveXCtrl

#Region "ActiveX Control Registration"

    ' These  GUIDs provide the COM identity for this class and its COM 
    ' interfaces. If you change them, existing clients will no longer be 
    ' able to access the class.

    Public Const ClassId As String _
    = "81F71529-0F1C-3905-94E5-82ADBB9DFB5B"
    Public Const InterfaceId As String _
    = "916F51ED-11DA-317B-988E-427C2B5032C5"
    Public Const EventsId As String _
    = "2994FDB9-EBD0-4708-9197-D2B90E261E4F"


    ' These routines perform the additional COM registration needed by 
    ' ActiveX controls

    <EditorBrowsable(EditorBrowsableState.Never)> _
    <ComRegisterFunction()> _
    Private Shared Sub Register(ByVal t As Type)
        Try
            ActiveXCtrlHelper.RegasmRegisterControl(t)
        Catch ex As Exception
            Console.WriteLine(ex.Message) ' Log the error
            Throw ex ' Re-throw the exception
        End Try
    End Sub

    <EditorBrowsable(EditorBrowsableState.Never)> _
    <ComUnregisterFunction()> _
    Private Shared Sub Unregister(ByVal t As Type)
        Try
            ActiveXCtrlHelper.RegasmUnregisterControl(t)
        Catch ex As Exception
            Console.WriteLine(ex.Message) ' Log the error
            Throw ex ' Re-throw the exception
        End Try
    End Sub

#End Region

#Region "Initialization"

    Public Sub New()

        ' This call is required by the Windows Form Designer.
        InitializeComponent()

        'Raise Load event
        Me.OnCreateControl()

    End Sub

    ' This event will hook up the necessary handlers
    Private Sub VBActiveXCtrl_ControlAdded(ByVal sender As Object, _
                                           ByVal e As ControlEventArgs) _
                                           Handles Me.ControlAdded
        ActiveXCtrlHelper.WireUpHandlers( _
        e.Control, AddressOf ValidationHandler)
    End Sub

    ' Ensures that the Validating and Validated events fire appropriately
    Friend Sub ValidationHandler(ByVal sender As Object, ByVal e As EventArgs)

        If Me.ContainsFocus Then Return

        Me.OnLeave(e) 'Raise Leave event

        If Me.CausesValidation Then
            Dim validationArgs As New CancelEventArgs
            Me.OnValidating(validationArgs)

            If validationArgs.Cancel AndAlso _
            Me.ActiveControl IsNot Nothing Then
                Me.ActiveControl.Focus()
            Else
                'Raise Validated event
                Me.OnValidated(e)
            End If
        End If

    End Sub

    <SecurityPermission(SecurityAction.LinkDemand, _
                        Flags:=SecurityPermissionFlag.UnmanagedCode)> _
    Protected Overrides Sub WndProc(ByRef m As System.Windows.Forms.Message)

        Const WM_SETFOCUS As Integer = &H7
        Const WM_PARENTNOTIFY As Integer = &H210
        Const WM_DESTROY As Integer = &H2
        Const WM_LBUTTONDOWN As Integer = &H201
        Const WM_RBUTTONDOWN As Integer = &H204

        If m.Msg = WM_SETFOCUS Then
            'Raise Enter event
            Me.OnEnter(New System.EventArgs)

        ElseIf m.Msg = WM_PARENTNOTIFY AndAlso _
            (m.WParam.ToInt32 = WM_LBUTTONDOWN OrElse _
             m.WParam.ToInt32 = WM_RBUTTONDOWN) Then

            If Not Me.ContainsFocus Then
                'Raise Enter event
                Me.OnEnter(New System.EventArgs)
            End If

        ElseIf m.Msg = WM_DESTROY AndAlso _
        Not Me.IsDisposed AndAlso Not Me.Disposing Then
            ' Used to ensure the cleanup of the control
            Me.Dispose()
        End If

        MyBase.WndProc(m)
    End Sub

    ' Ensures that tabbing across the container and the .NET controls works 
    ' as expected
    Private Sub UserControl_LostFocus(ByVal sender As Object, _
                                      ByVal e As System.EventArgs) _
                                      Handles Me.LostFocus
        ActiveXCtrlHelper.HandleFocus(Me)
    End Sub

#End Region

#Region "Properties"

    ' Typical control properties

    Public Shadows Property Visible() As Boolean
        Get
            Return MyBase.Visible
        End Get
        Set(ByVal value As Boolean)
            MyBase.Visible = value
        End Set
    End Property

    Public Shadows Property Enabled() As Boolean
        Get
            Return MyBase.Enabled
        End Get
        Set(ByVal value As Boolean)
            MyBase.Enabled = value
        End Set
    End Property

    Public Shadows Property ForeColor() As Integer
        Get
            Return ActiveXCtrlHelper.GetOleColorFromColor(MyBase.ForeColor)
        End Get
        Set(ByVal value As Integer)
            MyBase.ForeColor = ActiveXCtrlHelper.GetColorFromOleColor(value)
        End Set
    End Property

    Public Shadows Property BackColor() As Integer
        Get
            Return ActiveXCtrlHelper.GetOleColorFromColor(MyBase.BackColor)
        End Get
        Set(ByVal value As Integer)
            MyBase.BackColor = ActiveXCtrlHelper.GetColorFromOleColor(value)
        End Set
    End Property

    ' Customer properties

    Private fField As Single = 0

    Public Property FloatProperty() As Single
        Get
            Return Me.fField
        End Get
        Set(ByVal value As Single)
            Dim cancel As Boolean = False
            ' Raise the event FloatPropertyChanging
            RaiseEvent FloatPropertyChanging(value, cancel)
            If Not cancel Then
                Me.fField = value
                Me.lbFloatProperty.Text = value.ToString
            End If
        End Set
    End Property

#End Region

#Region "Methods"

    ' Typical control methods

    Public Overrides Sub Refresh()
        MyBase.Refresh()
    End Sub

    ' Custom methods

    Public Function HelloWorld() As String
        Return "HelloWorld"
    End Function

#End Region

#Region "Events"

    ' This section shows the examples of exposing a control's events.
    ' Typically, you just need to
    ' 1) Declare the event as you want it.
    ' 2) Raise the event in the appropriate control event.

    Public Shadows Event Click()

    Private Sub VBActiveXCtrl_Click(ByVal sender As Object, _
                                    ByVal e As System.EventArgs) _
                                    Handles MyBase.Click
        RaiseEvent Click()  ' Raise the new Click event.
    End Sub

    Public Event FloatPropertyChanging(ByVal NewValue As Single, _
                                       ByRef Cancel As Boolean)

#End Region

    Private Sub bnMessage_Click(ByVal sender As System.Object, _
                            ByVal e As System.EventArgs) _
                            Handles bnMessage.Click
        MessageBox.Show(tbMessage.Text, "HelloWorld", _
                MessageBoxButtons.OK, MessageBoxIcon.Information)
    End Sub

End Class
