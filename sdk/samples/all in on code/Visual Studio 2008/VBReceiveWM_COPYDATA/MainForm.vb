'******************************* Module Header *********************************'
' Module Name:  MainForm.vb
' Project:      VBReceiveWM_COPYDATA
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
' This code sample demonstrates receiving a custom data structure (MyStruct) from 
' the sending application (VBSendWM_COPYDATA) by handling WM_COPYDATA messages.
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


Public Class MainForm

    Protected Overrides Sub WndProc(ByRef m As System.Windows.Forms.Message)
        If (m.Msg = WM_COPYDATA) Then
            ' Get the COPYDATASTRUCT struct from lParam.
            Dim cds As COPYDATASTRUCT = m.GetLParam(GetType(COPYDATASTRUCT))

            ' If the size matches
            If (cds.cbData = Marshal.SizeOf(GetType(MyStruct))) Then
                ' Marshal the data from the unmanaged memory block to a MyStruct 
                ' managed struct.
                Dim myStruct As MyStruct = Marshal.PtrToStructure(cds.lpData, _
                    GetType(MyStruct))

                ' Display the MyStruct data members.
                Me.lbNumber.Text = myStruct.Number.ToString
                Me.lbMessage.Text = myStruct.Message
            End If
        End If

        MyBase.WndProc(m)
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

#End Region

End Class