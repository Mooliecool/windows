'****************************** Module Header ******************************'
' Module Name:  NativeMethod.vb
' Project:      VBPInvokeDll
' Copyright (c) Microsoft Corporation.
' 
' The P/Invoke signatures of the methods exported from the unmanaged DLLs.
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

Imports System.Security
Imports System.Runtime.InteropServices

#End Region


' Function delegate of the 'PFN_COMPARE' callback function. The delegate 
' type has the UnmanagedFunctionPointerAttribute. Using this attribute, you 
' can specify the calling convention of the native function pointer type. In 
' the native API's header file, the callback PFN_COMPARE is defined as 
' __stdcall (CALLBACK), so here we specify CallingConvention.StdCall.
<UnmanagedFunctionPointer(CallingConvention.StdCall)> _
Delegate Function CompareCallback(ByVal a As Integer, ByVal b As Integer) As Integer


<SuppressUnmanagedCodeSecurity()> _
Friend Class NativeMethod

    <DllImport("CppDynamicLinkLibrary.dll", CharSet:=CharSet.Auto, _
        CallingConvention:=CallingConvention.Cdecl)> _
    Public Shared Function GetStringLength1(ByVal str As String) As Integer
    End Function


    <DllImport("CppDynamicLinkLibrary.dll", CharSet:=CharSet.Auto, _
        CallingConvention:=CallingConvention.StdCall)> _
    Public Shared Function GetStringLength2(ByVal str As String) As Integer
    End Function


    <DllImport("CppDynamicLinkLibrary.dll", CharSet:=CharSet.Auto)> _
    Public Shared Function Max( _
        ByVal a As Integer, _
        ByVal b As Integer, _
        ByVal cmpFunc As CompareCallback) _
        As Integer
    End Function


    <DllImport("user32.dll", CharSet:=CharSet.Auto)> _
    Public Shared Function MessageBox( _
        ByVal hWnd As IntPtr, _
        ByVal [text] As String, _
        ByVal caption As String, _
        ByVal options As MessageBoxOptions) _
        As MessageBoxResult
    End Function


    <DllImport("msvcrt.dll", CharSet:=CharSet.Ansi, _
        CallingConvention:=CallingConvention.Cdecl)> _
    Public Shared Function printf( _
        ByVal format As String, _
        ByVal arg1 As String, _
        ByVal arg2 As String) _
        As Integer
    End Function

End Class


''' <summary>
''' Flags that define appearance and behaviour of a standard message box 
''' displayed by a call to the MessageBox function.
''' </summary>
<Flags()> _
Public Enum MessageBoxOptions As UInt32
    Ok = 0
    OkCancel = 1
    AbortRetryIgnore = 2
    YesNoCancel = 3
    YesNo = 4
    RetryCancel = 5
    CancelTryContinue = 6

    IconHand = &H10
    IconQuestion = &H20
    IconExclamation = &H30
    IconAsterisk = &H40
    UserIcon = &H80

    IconWarning = &H30
    IconError = &H10
    IconInformation = &H40
    IconStop = &H10

    DefButton1 = 0
    DefButton2 = &H100
    DefButton3 = &H200
    DefButton4 = &H300

    ApplicationModal = 0
    SystemModal = &H1000
    TaskModal = &H2000

    Help = &H4000
    NoFocus = &H8000

    SetForeground = &H10000
    DefaultDesktopOnly = &H20000
    Topmost = &H40000
    Right = &H80000
    RTLReading = &H100000
End Enum


''' <summary>
''' Represents possible values returned by the MessageBox function.
''' </summary>
Public Enum MessageBoxResult As UInt32
    Ok = 1
    Cancel
    Abort
    Retry
    Ignore
    Yes
    No
    Close
    Help
    TryAgain
    [Continue]
    Timeout = &H7D00
End Enum