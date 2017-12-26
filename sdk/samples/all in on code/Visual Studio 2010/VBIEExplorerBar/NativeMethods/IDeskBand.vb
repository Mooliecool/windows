'*************************** Module Header ******************************'
' Module Name:  IDeskBand.vb
' Project:	    VBIEExplorerBar
' Copyright (c) Microsoft Corporation.
' 
' Gets information about a band object.  
' 
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

Namespace NativeMethods
    <ComImport()>
    <InterfaceType(ComInterfaceType.InterfaceIsIUnknown)>
    <Guid("EB0FE172-1A3A-11D0-89B3-00A0C90A90AC")>
    Public Interface IDeskBand
        Sub GetWindow(<Out()> ByRef phwnd As IntPtr)

        Sub ContextSensitiveHelp(<[In]()> ByVal fEnterMode As Boolean)

        Sub ShowDW(<[In]()> ByVal fShow As Boolean)

        Sub CloseDW(<[In]()> ByVal dwReserved As UInteger)

        Sub ResizeBorderDW(ByVal prcBorder As IntPtr,
                           <[In](), MarshalAs(UnmanagedType.IUnknown)> ByVal punkToolbarSite As Object,
                           ByVal fReserved As Boolean)

        Sub GetBandInfo(ByVal dwBandID As UInteger,
                        ByVal dwViewMode As UInteger,
                        ByRef pdbi As DESKBANDINFO)
    End Interface
End Namespace
