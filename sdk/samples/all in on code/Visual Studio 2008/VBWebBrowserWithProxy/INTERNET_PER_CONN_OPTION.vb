'*************************** Module Header ******************************'
' Module Name:  INTERNET_PER_CONN_OPTION.vb
' Project:	    VBWebBrowserWithProxy
' Copyright (c) Microsoft Corporation.
' 
' This file defines the struct INTERNET_PER_CONN_OPTION and contains used by it.
' The struct INTERNET_PER_CONN_OPTION contains the value of an option that to be 
' set to internet settings.
' Visit http://msdn.microsoft.com/en-us/library/aa385145(VS.85).aspx to get the 
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

''' <summary>
''' Constants used in INTERNET_PER_CONN_OPTION_OptionUnion struct.
''' </summary>
Public Enum INTERNET_PER_CONN_OptionEnum
    INTERNET_PER_CONN_FLAGS = 1
    INTERNET_PER_CONN_PROXY_SERVER = 2
    INTERNET_PER_CONN_PROXY_BYPASS = 3
    INTERNET_PER_CONN_AUTOCONFIG_URL = 4
    INTERNET_PER_CONN_AUTODISCOVERY_FLAGS = 5
    INTERNET_PER_CONN_AUTOCONFIG_SECONDARY_URL = 6
    INTERNET_PER_CONN_AUTOCONFIG_RELOAD_DELAY_MINS = 7
    INTERNET_PER_CONN_AUTOCONFIG_LAST_DETECT_TIME = 8
    INTERNET_PER_CONN_AUTOCONFIG_LAST_DETECT_URL = 9
    INTERNET_PER_CONN_FLAGS_UI = 10
End Enum

''' <summary>
''' Constants used in INTERNET_PER_CONN_OPTON struct.
''' </summary>
Public Enum INTERNET_OPTION_PER_CONN_FLAGS
    PROXY_TYPE_DIRECT = &H1 ' direct to net
    PROXY_TYPE_PROXY = &H2 ' via named proxy
    PROXY_TYPE_AUTO_PROXY_URL = &H4 ' autoproxy URL
    PROXY_TYPE_AUTO_DETECT = &H8 ' use autoproxy detection
End Enum

''' <summary>
''' Used in INTERNET_PER_CONN_OPTION.
''' When create a instance of OptionUnion, only one filed will be used.
''' The StructLayout and FieldOffset attributes could help to decrease the struct size.
''' </summary>
<StructLayout(LayoutKind.Explicit)> _
Public Structure INTERNET_PER_CONN_OPTION_OptionUnion
    ' A value in INTERNET_OPTION_PER_CONN_FLAGS.
    <FieldOffset(0)> _
    Public dwValue As Integer
    <FieldOffset(0)> _
    Public pszValue As IntPtr
    <FieldOffset(0)> _
    Public ftValue As System.Runtime.InteropServices.ComTypes.FILETIME
End Structure

<StructLayout(LayoutKind.Sequential)> _
Public Structure INTERNET_PER_CONN_OPTION
    ' A value in INTERNET_PER_CONN_OptionEnum.
    Public dwOption As Integer
    Public Value As INTERNET_PER_CONN_OPTION_OptionUnion
End Structure
