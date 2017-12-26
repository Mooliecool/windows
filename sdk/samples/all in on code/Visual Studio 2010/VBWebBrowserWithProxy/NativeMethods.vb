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
    <DllImport("wininet.dll", SetLastError:=True, CharSet:=CharSet.Auto)>
    Friend Shared Function InternetOpen(ByVal lpszAgent As String,
                                        ByVal dwAccessType As Integer,
                                        ByVal lpszProxyName As String,
                                        ByVal lpszProxyBypass As String,
                                        ByVal dwFlags As Integer) As IntPtr
    End Function

    ''' <summary>
    ''' Close a single Internet handle.
    ''' </summary>
    <DllImport("wininet.dll", SetLastError:=True)>
    Friend Shared Function InternetCloseHandle(ByVal hInternet As IntPtr) As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function

    ''' <summary>
    ''' Sets an Internet option.
    ''' </summary>
    <DllImport("wininet.dll", CharSet:=CharSet.Ansi, SetLastError:=True)>
    Friend Shared Function InternetSetOption(ByVal hInternet As IntPtr,
                                             ByVal dwOption As INTERNET_OPTION,
                                             ByVal lpBuffer As IntPtr,
                                             ByVal lpdwBufferLength As Integer) As Boolean
    End Function

    ''' <summary>
    ''' Queries an Internet option on the specified handle. The Handle will be always 0.
    ''' </summary>
    <DllImport("wininet.dll", CharSet:=CharSet.Ansi, SetLastError:=True)>
    Friend Shared Function InternetQueryOption(ByVal hInternet As IntPtr,
                                               ByVal dwOption As INTERNET_OPTION,
                                               ByRef OptionList As INTERNET_PER_CONN_OPTION_LIST,
                                               ByRef lpdwBufferLength As Integer) As Boolean
    End Function
End Class