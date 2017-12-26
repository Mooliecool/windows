'****************************** Module Header ******************************'
' Module Name:  NativeMethod.vb
' Project:      VBExeCOMServer
' Copyright (c) Microsoft Corporation.
' 
' The P/Invoke signatures of some native APIs.
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

Imports System.Runtime.InteropServices

#End Region


Friend Class NativeMethod

    ''' <summary>
    ''' Get current thread ID.
    ''' </summary>
    ''' <returns></returns>
    <DllImport("kernel32.dll")> _
    Friend Shared Function GetCurrentThreadId() As UInt32
    End Function


    ''' <summary>
    ''' Get current process ID.
    ''' </summary>
    ''' <returns></returns>
    <DllImport("kernel32.dll")> _
    Friend Shared Function GetCurrentProcessId() As UInt32
    End Function


    ''' <summary>
    ''' The GetMessage function retrieves a message from the calling thread's 
    ''' message queue. The function dispatches incoming sent messages until a 
    ''' posted message is available for retrieval. 
    ''' </summary>
    ''' <param name="lpMsg">
    ''' Pointer to an MSG structure that receives message information from 
    ''' the thread's message queue.
    ''' </param>
    ''' <param name="hWnd">
    ''' Handle to the window whose messages are to be retrieved.
    ''' </param>
    ''' <param name="wMsgFilterMin">
    ''' Specifies the integer value of the lowest message value to be 
    ''' retrieved. 
    ''' </param>
    ''' <param name="wMsgFilterMax">
    ''' Specifies the integer value of the highest message value to be 
    ''' retrieved.
    ''' </param>
    ''' <returns></returns>
    <DllImport("user32.dll")> _
    Friend Shared Function GetMessage(<Out()> ByRef lpMsg As MSG, _
                                      ByVal hWnd As IntPtr, _
                                      ByVal wMsgFilterMin As UInt32, _
                                      ByVal wMsgFilterMax As UInt32) As Boolean
    End Function


    ''' <summary>
    ''' The TranslateMessage function translates virtual-key messages into 
    ''' character messages. The character messages are posted to the calling 
    ''' thread's message queue, to be read the next time the thread calls the 
    ''' GetMessage or PeekMessage function.
    ''' </summary>
    ''' <param name="lpMsg"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <DllImport("user32.dll")> _
    Friend Shared Function TranslateMessage(<[In]()> ByRef lpMsg As MSG) As Boolean
    End Function


    ''' <summary>
    ''' The DispatchMessage function dispatches a message to a window 
    ''' procedure. It is typically used to dispatch a message retrieved by 
    ''' the GetMessage function.
    ''' </summary>
    ''' <param name="lpMsg"></param>
    ''' <returns></returns>
    <DllImport("user32.dll")> _
    Friend Shared Function DispatchMessage(<[In]()> ByRef lpMsg As MSG) As IntPtr
    End Function


    ''' <summary>
    ''' The PostThreadMessage function posts a message to the message queue 
    ''' of the specified thread. It returns without waiting for the thread to 
    ''' process the message.
    ''' </summary>
    ''' <param name="idThread">
    ''' Identifier of the thread to which the message is to be posted.
    ''' </param>
    ''' <param name="Msg">Specifies the type of message to be posted.</param>
    ''' <param name="wParam">
    ''' Specifies additional message-specific information.
    ''' </param>
    ''' <param name="lParam">
    ''' Specifies additional message-specific information.
    ''' </param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <DllImport("user32.dll")> _
    Friend Shared Function PostThreadMessage(ByVal idThread As UInt32, _
                                             ByVal Msg As UInt32, _
                                             ByVal wParam As UIntPtr, _
                                             ByVal lParam As IntPtr) As Boolean
    End Function


    Friend Const WM_QUIT As Integer = &H12

End Class


<StructLayout(LayoutKind.Sequential)> _
Friend Structure MSG
    Public hWnd As IntPtr
    Public message As UInt32
    Public wParam As IntPtr
    Public lParam As IntPtr
    Public time As UInt32
    Public pt As POINT
End Structure


<StructLayout(LayoutKind.Sequential)> _
Friend Structure POINT
    Public X As Integer
    Public Y As Integer

    Public Sub New(ByVal x As Integer, ByVal y As Integer)
        Me.X = x
        Me.Y = y
    End Sub
End Structure