' *************************** Module Header ******************************\
' Module Name:  NativeMethods.vb
' Project:      VBWebBrowserWithProxy
' Copyright (c) Microsoft Corporation.
' 
' This class is a simple .NET wrapper of wininet.dll. It contains 4 extern
' methods in wininet.dll. They are InternetOpen, InternetCloseHandle, 
' InternetSetOption and InternetQueryOption.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
' **************************************************************************

Imports System.Runtime.InteropServices

Friend NotInheritable Class NativeMethods
    ''' <summary>
    ''' Initialize an application's use of the WinINet functions.
    ''' See 
    ''' </summary>
    <DllImport("wininet.dll", SetLastError:=True, CharSet:=CharSet.Auto)> _
    Friend Shared Function InternetOpen(ByVal lpszAgent As String, _
                                        ByVal dwAccessType As Integer, _
                                        ByVal lpszProxyName As String, _
                                        ByVal lpszProxyBypass As String, _
                                        ByVal dwFlags As Integer) As IntPtr
    End Function

    ''' <summary>
    ''' Close a single Internet handle.
    ''' </summary>
    <DllImport("wininet.dll", SetLastError:=True)> _
    Friend Shared Function InternetCloseHandle(ByVal hInternet As IntPtr) As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function

    ''' <summary>
    ''' Sets an Internet option.
    ''' </summary>
    <DllImport("wininet.dll", CharSet:=CharSet.Ansi, SetLastError:=True)> _
    Friend Shared Function InternetSetOption(ByVal hInternet As IntPtr, _
                                             ByVal dwOption As INTERNET_OPTION, _
                                             ByVal lpBuffer As IntPtr, _
                                             ByVal lpdwBufferLength As Integer) As Boolean
    End Function

    ''' <summary>
    ''' Queries an Internet option on the specified handle. The Handle will be always 0.
    ''' </summary>
    <DllImport("wininet.dll", CharSet:=CharSet.Ansi, SetLastError:=True)> _
    Friend Shared Function InternetQueryOption(ByVal hInternet As IntPtr, _
                                               ByVal dwOption As INTERNET_OPTION, _
                                               ByRef OptionList As INTERNET_PER_CONN_OPTION_LIST, _
                                               ByRef lpdwBufferLength As Integer) As Boolean
    End Function
End Class