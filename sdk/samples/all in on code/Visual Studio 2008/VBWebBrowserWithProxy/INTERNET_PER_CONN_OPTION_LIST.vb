'*************************** Module Header ******************************'
' Module Name:  INTERNET_PER_CONN_OPTION_LIST.vb
' Project:	    VBWebBrowserWithProxy
' Copyright (c) Microsoft Corporation.
' 
' The struct INTERNET_PER_CONN_OPTION contains a list of options that to be 
' set to internet connection.
' Visit http://msdn.microsoft.com/en-us/library/aa385146(VS.85).aspx to get the 
' detailed description.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.Runtime.InteropServices

<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi)> _
Public Structure INTERNET_PER_CONN_OPTION_LIST
    Public Size As Integer

    ' The connection to be set. NULL means LAN.
    Public Connection As IntPtr

    Public OptionCount As Integer
    Public OptionError As Integer

    ' List of INTERNET_PER_CONN_OPTIONs.
    Public pOptions As IntPtr
End Structure
