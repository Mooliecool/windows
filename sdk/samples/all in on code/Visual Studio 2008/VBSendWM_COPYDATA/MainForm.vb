'******************************* Module Header *********************************'
' Module Name:  MainForm.vb
' Project:      VBSendWM_COPYDATA
' Copyright (c) Microsoft Corporation.
'
' Inter-process Communication (IPC) based on the Windows message WM_COPYDATA is a 
' mechanism for exchanging data among Windows applications in the local machine. 
' The receiving application must be a Windows application. The data being passed 
' must not contain pointers or other references to objects not accessible to the 
' application receiving the data. While WM_COPYDATA is being sent, the referenced 
' data must not be changed by another thread of the sending process. The receiving 
' application should consider the data read-only. If the receiving application 
' must access the data after SendMessage returns, it needs to copy the data into a 
' local buffer.
' 
' This code sample demonstrates sending a custom data structure (MyStruct) to the 
' receiving Windows application (VBReceiveWM_COPYDATA) by using 
' SendMessage(WM_COPYDATA). If the data structure fails to be passed, the 
' application displays the error code for diagnostics. A typical error code is 
' 0x5 (Access is denied) caused by User Interface Privilege Isolation (UIPI). 
' UIPI prevents processes from sending selected window messages and other USER 
' APIs to processes running with higher integrity. When the receiving application 
' (VBReceiveWM_COPYDATA) runs at an integrity level higher than this sending 
' application, you will see the "SendMessage(WM_COPYDATA) failed w/err 0x00000005" 
' error message.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*******************************************************************************'

Imports System.Runtime.InteropServices
Imports System.Security


Public Class MainForm

    Private Sub btnSendMessage_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles btnSendMessage.Click
        ' Find the target window handle.
        Dim hTargetWnd As IntPtr = NativeMethod.FindWindow(Nothing, "VBReceiveWM_COPYDATA")
        If (hTargetWnd = IntPtr.Zero) Then
            MessageBox.Show("Unable to find the ""VBReceiveWM_COPYDATA"" window", _
                   "Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
            Return
        End If

        ' Prepare the COPYDATASTRUCT struct with the data to be sent.
        Dim myStruct As MyStruct

        Dim nNumber As Integer
        If Not Integer.TryParse(Me.tbNumber.Text, nNumber) Then
            MessageBox.Show("Invalid value of Number!")
            Return
        End If

        myStruct.Number = nNumber
        myStruct.Message = Me.tbMessage.Text

        ' Marshal the managed struct to a native block of memory.
        Dim myStructSize As Integer = Marshal.SizeOf(myStruct)
        Dim pMyStruct As IntPtr = Marshal.AllocHGlobal(myStructSize)
        Try
            Marshal.StructureToPtr(myStruct, pMyStruct, True)

            Dim cds As New COPYDATASTRUCT
            cds.cbData = myStructSize
            cds.lpData = pMyStruct

            ' Send the COPYDATASTRUCT struct through the WM_COPYDATA message to 
            ' the receiving window. (The application must use SendMessage, 
            ' instead of PostMessage to send WM_COPYDATA because the receiving 
            ' application must accept while it is guaranteed to be valid.)
            NativeMethod.SendMessage(hTargetWnd, WM_COPYDATA, Me.Handle, cds)

            Dim result As Integer = Marshal.GetLastWin32Error
            If (result <> 0) Then
                MessageBox.Show(String.Format( _
                    "SendMessage(WM_COPYDATA) failed w/err 0x{0:X}", result))
            End If
        Finally
            Marshal.FreeHGlobal(pMyStruct)
        End Try

    End Sub


    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Unicode)> _
    Friend Structure MyStruct
        Public Number As Integer

        <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=&H100)> _
        Public Message As String
    End Structure


#Region "Native API Signatures and Types"

    ''' <summary>
    ''' An application sends the WM_COPYDATA message to pass data to another 
    ''' application.
    ''' </summary>
    Friend Const WM_COPYDATA As Integer = &H4A


    ''' <summary>
    ''' The COPYDATASTRUCT structure contains data to be passed to another 
    ''' application by the WM_COPYDATA message. 
    ''' </summary>
    <StructLayout(LayoutKind.Sequential)> _
    Friend Structure COPYDATASTRUCT
        Public dwData As IntPtr
        Public cbData As Integer
        Public lpData As IntPtr
    End Structure


    <SuppressUnmanagedCodeSecurity()> _
    Friend Class NativeMethod

        ''' <summary>
        ''' Sends the specified message to a window or windows. The SendMessage 
        ''' function calls the window procedure for the specified window and does 
        ''' not return until the window procedure has processed the message. 
        ''' </summary>
        ''' <param name="hWnd">
        ''' Handle to the window whose window procedure will receive the message.
        ''' </param>
        ''' <param name="Msg">Specifies the message to be sent.</param>
        ''' <param name="wParam">
        ''' Specifies additional message-specific information.
        ''' </param>
        ''' <param name="lParam">
        ''' Specifies additional message-specific information.
        ''' </param>
        ''' <returns></returns>
        <DllImport("user32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Public Shared Function SendMessage( _
            ByVal hWnd As IntPtr, _
            ByVal Msg As Integer, _
            ByVal wParam As IntPtr, _
            ByRef lParam As COPYDATASTRUCT) _
            As IntPtr
        End Function


        ''' <summary>
        ''' The FindWindow function retrieves a handle to the top-level window 
        ''' whose class name and window name match the specified strings. This 
        ''' function does not search child windows. This function does not 
        ''' perform a case-sensitive search.
        ''' </summary>
        ''' <param name="lpClassName">Class name</param>
        ''' <param name="lpWindowName">Window caption</param>
        ''' <returns></returns>
        <DllImport("user32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
        Public Shared Function FindWindow( _
            ByVal lpClassName As String, _
            ByVal lpWindowName As String) _
            As IntPtr
        End Function

    End Class

#End Region

End Class
