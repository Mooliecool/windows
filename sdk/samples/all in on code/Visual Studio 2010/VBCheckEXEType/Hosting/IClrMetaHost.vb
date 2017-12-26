'*************************** Module Header ******************************'
' Module Name:  IClrMetaHost.vb
' Project:	    VBCheckEXEType
' Copyright (c) Microsoft Corporation.
' 
' 
' Provides methods that return a specific version of the common language 
' runtime (CLR) based on its version number, list all installed CLRs, list all 
' runtimes that are loaded in a specified process, discover the CLR version used 
' to compile an assembly, exit a process with a clean runtime shutdown, and query 
' legacy API binding.
'
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

Imports System
Imports System.Runtime.CompilerServices
Imports System.Runtime.InteropServices
Imports System.Security
Imports System.Text

Namespace Hosting
    <ComImport()>
    <SecurityCritical()>
    <InterfaceType(ComInterfaceType.InterfaceIsIUnknown)>
    <Guid("D332DB9E-B9B3-4125-8207-A14884F53216")>
    Public Interface IClrMetaHost
        <MethodImpl(MethodImplOptions.InternalCall, MethodCodeType:=MethodCodeType.Runtime)>
        Function GetRuntime(<[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal version As String,
                            <[In](), MarshalAs(UnmanagedType.LPStruct)> ByVal interfaceId As Guid) _
                        As <MarshalAs(UnmanagedType.Interface)> Object

        <MethodImpl(MethodImplOptions.InternalCall, MethodCodeType:=MethodCodeType.Runtime)>
        Sub GetVersionFromFile(<[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal filePath As String,
                               <Out(), MarshalAs(UnmanagedType.LPWStr)> ByVal buffer As StringBuilder,
                               <[In](), Out(), MarshalAs(UnmanagedType.U4)> ByRef bufferLength As UInteger)

        <MethodImpl(MethodImplOptions.InternalCall, MethodCodeType:=MethodCodeType.Runtime)>
        Function EnumerateInstalledRuntimes() As <MarshalAs(UnmanagedType.Interface)> IEnumUnknown

        <MethodImpl(MethodImplOptions.InternalCall, MethodCodeType:=MethodCodeType.Runtime)>
        Function EnumerateLoadedRuntimes(<[In]()> ByVal processHandle As IntPtr) _
        As <MarshalAs(UnmanagedType.Interface)> IEnumUnknown

        <PreserveSig(), MethodImpl(MethodImplOptions.InternalCall, MethodCodeType:=MethodCodeType.Runtime)>
        Function Reserved01(<[In]()> ByVal reserved1 As IntPtr) As Integer
    End Interface
End Namespace