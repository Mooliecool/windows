'****************************** Module Header ******************************'
' Module Name:  NativeMethods.vb
' Project:      VBOfficeRibbonAccessibility
' Copyright (c) Microsoft Corporation.
'
' Declares the P/Invoke signatures of native APIs.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.Runtime.InteropServices
Imports Microsoft.Office.Core


Friend Class NativeMethods

    ' Retrieves the child ID or IDispatch of each child within an accessible 
    ' container object.
    <DllImport("oleacc.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function AccessibleChildren( _
        ByVal paccContainer As IAccessible, _
        ByVal iChildStart As Integer, _
        ByVal cChildren As Integer, _
        <Out(), MarshalAs(UnmanagedType.LPArray, SizeParamIndex:=4)> _
        ByVal rgvarChildren As Object(), _
        ByRef pcObtained As Integer) As Integer
    End Function

    ' Retrieves the address of the specified interface for the object 
    ' associated with the specified window.
    <DllImport("oleacc.dll", PreserveSig:=False, CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function AccessibleObjectFromWindow( _
        ByVal hwnd As Integer, _
        ByVal dwId As Integer, _
        ByRef riid As Guid) _
    As <MarshalAs(UnmanagedType.Interface)> Object
    End Function

    ' Retrieves the localized string that describes the object's role for the 
    ' specified role value.
    <DllImport("oleacc.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function GetRoleText( _
        ByVal dwRole As UInt32, _
        <Out()> ByVal lpszRole As StringBuilder, _
        ByVal cchRoleMax As UInt32) As UInt32
    End Function

    ' Retrieves a localized string that describes an object's state for a 
    ' single predefined state bit flag. Because state values are a 
    ' combination of one or more bit flags, clients call this function more 
    ' than once to retrieve all state strings.
    <DllImport("oleacc.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function GetStateText( _
        ByVal dwStateBit As MSAAStateConstants, _
        <Out()> ByVal lpszStateBit As StringBuilder, _
        ByVal cchStateBitMax As UInt32) As UInt32
    End Function

End Class


<Flags()> _
Friend Enum MSAAStateConstants
    STATE_SYSTEM_NORMAL = 0
    STATE_SYSTEM_UNAVAILABLE = 1
    STATE_SYSTEM_SELECTED = 2
    STATE_SYSTEM_FOCUSED = 4
    STATE_SYSTEM_PRESSED = 8
    STATE_SYSTEM_CHECKED = &H10
    STATE_SYSTEM_MIXED = &H20
    STATE_SYSTEM_READONLY = &H40
    STATE_SYSTEM_HOTTRACKED = &H80
    STATE_SYSTEM_DEFAULT = &H100
    STATE_SYSTEM_EXPANDED = &H200
    STATE_SYSTEM_COLLAPSED = &H400
    STATE_SYSTEM_BUSY = &H800
    STATE_SYSTEM_FLOATING = &H1000
    STATE_SYSTEM_MARQUEED = &H2000
    STATE_SYSTEM_ANIMATED = &H4000
    STATE_SYSTEM_INVISIBLE = &H8000
    STATE_SYSTEM_OFFSCREEN = &H10000
    STATE_SYSTEM_SIZEABLE = &H20000
    STATE_SYSTEM_MOVEABLE = &H40000
    STATE_SYSTEM_SELFVOICING = &H80000
    STATE_SYSTEM_FOCUSABLE = &H100000
    STATE_SYSTEM_SELECTABLE = &H200000
    STATE_SYSTEM_LINKED = &H400000
    STATE_SYSTEM_TRAVERSED = &H800000
    STATE_SYSTEM_MULTISELECTABLE = &H1000000
    STATE_SYSTEM_EXTSELECTABLE = &H2000000
    STATE_SYSTEM_ALERT_LOW = &H4000000
    STATE_SYSTEM_ALERT_MEDIUM = &H8000000
    STATE_SYSTEM_ALERT_HIGH = &H10000000
    STATE_SYSTEM_HASPOPUP = &H40000000
    STATE_SYSTEM_VALID = &H1FFFFFFF
End Enum