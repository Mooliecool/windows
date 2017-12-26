'********************************** Module Header ***********************************'
' Module Name:  ActiveXCtrlHelper.vb
' Project:      VBActiveX
' Copyright (c) Microsoft Corporation.
' 
' ActiveXCtrlHelper provides the helper functions to register/unregister an ActiveX 
' control, and helps to handle the focus and tabbing across the container and the 
' .NET controls.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************************'

#Region "Imports directives"

Imports System.Runtime.InteropServices
Imports Microsoft.Win32
Imports System.Reflection
Imports System.Drawing

#End Region


<ComVisible(False)> _
Friend Class ActiveXCtrlHelper
    Inherits AxHost

    Friend Sub New()
        MyBase.New(Nothing)
    End Sub

#Region "ActiveX Control Registration"

#Region "OLEMISC Enumeration"

    ' Ref: http://msdn.microsoft.com/en-us/library/ms678497.aspx
    Const OLEMISC_RECOMPOSEONRESIZE As Integer = 1
    Const OLEMISC_CANTLINKINSIDE As Integer = 16
    Const OLEMISC_INSIDEOUT As Integer = 128
    Const OLEMISC_ACTIVATEWHENVISIBLE As Integer = 256
    Const OLEMISC_SETCLIENTSITEFIRST As Integer = 131072

#End Region

    ''' <summary>
    ''' Register the control as an ActiveX control.
    ''' </summary>
    ''' <param name="t"></param>
    ''' <remarks></remarks>
    Public Shared Sub RegasmRegisterControl(ByVal t As Type)

        ' Check the argument
        GuardNullType(t, "t")
        GuardTypeIsControl(t)

        ' Open the CLSID key of the control
        Using keyCLSID As RegistryKey = Registry.ClassesRoot.OpenSubKey( _
        "CLSID\" & t.GUID.ToString("B"), True)


            ' Set "InprocServer32" to register a 32-bit in-process server.
            ' InprocServer32 = <path to 32-bit inproc server>
            ' Ref: http://msdn.microsoft.com/en-us/library/ms683844.aspx

            Dim keyInproc As RegistryKey = keyCLSID.OpenSubKey("InprocServer32", True)
            If keyInproc IsNot Nothing Then
                keyInproc.SetValue(Nothing, _
                                   Environment.SystemDirectory & "\mscoree.dll")
            End If


            ' Create "Control" to identify it as an ActiveX Control.
            ' Ref: http://msdn.microsoft.com/en-us/library/ms680056.aspx

            Using keyCtrl As RegistryKey = keyCLSID.CreateSubKey("Control")
            End Using


            ' Create "MiscStatus" to specify how to create/display an object. 
            ' MiscStatus = <combination of values from OLEMISC enumeration>
            ' Ref: http://msdn.microsoft.com/en-us/library/ms683733.aspx

            Using keyMisc As RegistryKey = keyCLSID.CreateSubKey("MiscStatus")
                Dim nMiscStatus As Integer = OLEMISC_RECOMPOSEONRESIZE + _
                OLEMISC_CANTLINKINSIDE + OLEMISC_INSIDEOUT + _
                OLEMISC_ACTIVATEWHENVISIBLE + OLEMISC_SETCLIENTSITEFIRST

                keyMisc.SetValue("", nMiscStatus.ToString, RegistryValueKind.String)
            End Using


            ' Create "ToolBoxBitmap32" to identify the module name and the resource  
            ' ID for a 16 x 16 bitmap as the toolbar button face.
            ' ToolBoxBitmap32 = <filename>.<ext>, <resourceID>
            ' Ref: http://msdn.microsoft.com/en-us/library/ms687316.aspx

            Using keyBitmap As RegistryKey = keyCLSID.CreateSubKey("ToolBoxBitmap32")

                ' If you want different icons for each control in the assembly you 
                ' can modify this section to specify a different icon each time. 
                ' Each specified icon must be embedded as a win32 resource in the 
                ' assembly; the default one is at the index 101, but you can use 
                ' additional ones.
                keyBitmap.SetValue("", Assembly.GetExecutingAssembly.Location & _
                                   ", 101", RegistryValueKind.String)
            End Using


            ' Create "TypeLib" to specify the typelib GUID associated with the class. 

            Using keyTypeLib As RegistryKey = keyCLSID.CreateSubKey("TypeLib")
                Dim libId As Guid = Marshal.GetTypeLibGuidForAssembly(t.Assembly)
                keyTypeLib.SetValue("", libId.ToString("B"), RegistryValueKind.String)
            End Using


            ' Create "Version" to specify the version of the control. 
            ' Ref: http://msdn.microsoft.com/en-us/library/ms686568.aspx

            Using keyVersion As RegistryKey = keyCLSID.CreateSubKey("Version")
                Dim nMajor, nMinor As Integer
                Marshal.GetTypeLibVersionForAssembly(t.Assembly, nMajor, nMinor)
                keyVersion.SetValue("", String.Format("{0}.{1}", nMajor, nMinor))
            End Using

        End Using

    End Sub

    ''' <summary>
    ''' Unregister the control.
    ''' </summary>
    ''' <param name="t"></param>
    ''' <remarks></remarks>
    Public Shared Sub RegasmUnregisterControl(ByVal t As Type)

        ' Check the argument
        GuardNullType(t, "t")
        GuardTypeIsControl(t)

        ' Delete the CLSID key of the control
        Registry.ClassesRoot.DeleteSubKeyTree("CLSID\" & t.GUID.ToString("B"))

    End Sub

    Private Shared Sub GuardNullType(ByVal t As Type, ByVal param As String)
        If t Is Nothing Then
            Throw New ArgumentException( _
            "The CLR type must be specified.", param)
        End If
    End Sub

    Private Shared Sub GuardTypeIsControl(ByVal t As Type)
        If Not GetType(Control).IsAssignableFrom(t) Then
            Throw New ArgumentException( _
            "Type argument must be a Windows Forms control.")
        End If
    End Sub

#End Region

#Region "Type Converter"

    ''' <summary>
    ''' Convert OleColor to System.Drawing.Color
    ''' </summary>
    ''' <param name="oleColor"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Friend Shared Shadows Function GetColorFromOleColor(ByVal oleColor As Integer) As Color
        Return AxHost.GetColorFromOleColor(CIntToUInt(oleColor))
    End Function

    ''' <summary>
    ''' Convert System.Drawing.Color to OleColor 
    ''' </summary>
    ''' <param name="color"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Friend Shared Shadows Function GetOleColorFromColor(ByVal color As Color) As Integer
        Return CUIntToInt(AxHost.GetOleColorFromColor(color))
    End Function

    Friend Shared Function CUIntToInt(ByVal uiArg As UInteger) As Integer
        If uiArg <= Integer.MaxValue Then
            Return CInt(uiArg)
        End If
        Return CInt(uiArg - 2 * (CUInt(Integer.MaxValue) + 1))
    End Function

    Friend Shared Function CIntToUInt(ByVal iArg As Integer) As UInteger
        If iArg < 0 Then
            Return CUInt(UInteger.MaxValue + iArg + 1)
        End If
        Return CUInt(iArg)
    End Function

#End Region

#Region "Tab Handler"

    ''' <summary>
    ''' Register tab handler and focus-related event handlers for the control and its 
    ''' child controls.
    ''' </summary>
    ''' <param name="ctrl"></param>
    ''' <param name="ValidationHandler"></param>
    ''' <remarks></remarks>
    Friend Shared Sub WireUpHandlers(ByVal ctrl As Control, _
                                     ByVal ValidationHandler As EventHandler)

        If ctrl IsNot Nothing Then
            AddHandler ctrl.KeyDown, AddressOf ActiveXCtrlHelper.TabHandler
            AddHandler ctrl.LostFocus, ValidationHandler

            If ctrl.HasChildren Then
                For Each child As Control In ctrl.Controls
                    WireUpHandlers(child, ValidationHandler)
                Next
            End If
        End If
    End Sub

    ''' <summary>
    ''' Handler of "Tab" and "Shift"+"Tab".
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Friend Shared Sub TabHandler(ByVal sender As Object, ByVal e As KeyEventArgs)

        If e.KeyCode = Keys.Tab Then
            Dim ctrl As Control = CType(sender, Control)

            Dim usrCtrl As UserControl = GetParentUserControl(ctrl)

            Dim firstCtrl As Control = usrCtrl.GetNextControl(Nothing, True)
            Do Until (firstCtrl Is Nothing OrElse firstCtrl.CanSelect)
                firstCtrl = usrCtrl.GetNextControl(firstCtrl, True)
            Loop

            Dim lastCtrl As Control = usrCtrl.GetNextControl(Nothing, False)
            Do Until (lastCtrl Is Nothing OrElse lastCtrl.CanSelect)
                lastCtrl = usrCtrl.GetNextControl(lastCtrl, False)
            Loop

            If ctrl Is lastCtrl OrElse ctrl Is firstCtrl OrElse _
                lastCtrl.Contains(ctrl) OrElse firstCtrl.Contains(ctrl) Then

                usrCtrl.SelectNextControl(CType(sender, Control), _
                                          lastCtrl Is usrCtrl.ActiveControl, _
                                          True, True, True)
            End If
        End If
    End Sub

    Private Shared Function GetParentUserControl(ByVal ctrl As Control) As UserControl
        If ctrl Is Nothing Then Return Nothing

        Do Until ctrl.Parent Is Nothing
            ctrl = ctrl.Parent
        Loop
        If ctrl IsNot Nothing Then
            Return DirectCast(ctrl, UserControl)
        End If

        Return Nothing
    End Function

#End Region

#Region "Focus Handler"

    ''' <summary>
    ''' Handle the focus of the ActiveX control, including its child controls
    ''' </summary>
    ''' <param name="usrCtrl">the ActiveX control</param>
    ''' <remarks></remarks>
    Friend Shared Sub HandleFocus(ByVal usrCtrl As UserControl)

        If My.Computer.Keyboard.AltKeyDown Then
            HandleAccessorKey(usrCtrl.GetNextControl(Nothing, True), usrCtrl)
        Else
            'Move to the first control that can receive focus, taking into account
            'the possibility that the user pressed <Shift>+<Tab>, in which case we
            'need to start at the end and work backwards.        
            Dim ctl As Control = usrCtrl.GetNextControl( _
            Nothing, Not My.Computer.Keyboard.ShiftKeyDown)
            While ctl IsNot Nothing
                If ctl.Enabled AndAlso ctl.CanSelect Then
                    ctl.Focus()
                    Exit While
                Else
                    ctl = usrCtrl.GetNextControl( _
                    ctl, Not My.Computer.Keyboard.ShiftKeyDown)
                End If
            End While
        End If

    End Sub

    Private Const KEY_PRESSED As Integer = &H1000
    Private Declare Function GetKeyState Lib "user32" Alias "GetKeyState" _
    (ByVal ByValnVirtKey As Integer) As Short

    ''' <summary>
    ''' Get X in the accessor key "Alt + X"
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Shared Function CheckForAccessorKey() As Integer

        If My.Computer.Keyboard.AltKeyDown Then
            For i As Integer = Keys.A To Keys.Z
                If (GetKeyState(i) And KEY_PRESSED) <> 0 Then
                    Return i
                End If
            Next
        End If
        Return -1

    End Function

    ''' <summary>
    ''' Check the accessor key, find the next selectable control that matches the 
    ''' accessor key and give it the focus.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="usrCtrl"></param>
    ''' <remarks></remarks>
    Private Shared Sub HandleAccessorKey(ByVal sender As Object, _
                                         ByVal usrCtrl As UserControl)

        ' Get X in the accessor key <Alt + X>
        Dim key As Integer = CheckForAccessorKey()
        If key = -1 Then Return

        Dim ctrl As Control = usrCtrl.GetNextControl(CType(sender, Control), False)

        Do
            ctrl = usrCtrl.GetNextControl(ctrl, True)
            If ctrl IsNot Nothing AndAlso _
            Control.IsMnemonic(ChrW(key), ctrl.Text) AndAlso _
            Not KeyConflict(ChrW(key), usrCtrl) Then

                ' If we land on a label or other non-selectable control then go to 
                ' the next control in the tab order
                If Not ctrl.CanSelect Then
                    Dim ctlAfterLabel As Control = usrCtrl.GetNextControl(ctrl, True)
                    If ctlAfterLabel IsNot Nothing AndAlso ctlAfterLabel.CanFocus Then
                        ctlAfterLabel.Focus()
                    End If
                Else
                    ctrl.Focus()
                End If
                Exit Do
            End If
            ' Loop until we hit the end of the tab order. If we have hit the end  
            ' of the tab order we do not want to loop back because the parent 
            ' form's controls come next in the tab order.
        Loop Until ctrl Is Nothing
    End Sub

    Private Shared Function KeyConflict(ByVal key As Char, _
                                        ByVal u As UserControl) As Boolean
        Dim flag As Boolean = False
        For Each ctl As Control In u.Controls
            If Control.IsMnemonic(key, ctl.Text) Then
                If flag Then Return True
                flag = True
            End If
        Next
        Return False
    End Function

#End Region

End Class
