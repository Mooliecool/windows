'*************************** Module Header ******************************'
' Module Name:  OpenImageBHO.vb
' Project:      VBCustomIEContextMenu
' Copyright (c) Microsoft Corporation.
' 
' The class OpenImageBHO is a Browser Helper Object which runs within Internet
' Explorer and offers additional services.
' 
' A BHO is a dynamic-link library (DLL) capable of attaching itself to any new 
' instance of Internet Explorer or Windows Explorer. Such a module can get in touch 
' with the browser through the container's site. In general, a site is an intermediate
' object placed in the middle of the container and each contained object. When the
' container is Internet Explorer (or Windows Explorer), the object is now required 
' to implement a simpler and lighter interface called IObjectWithSite. 
' It provides just two methods SetSite and GetSite. 
' 
' This class is used to set the IDocHostUIHandler of the HtmlDocument.
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
Imports VBCustomIEContextMenu.NativeMethods
Imports Microsoft.Win32
Imports SHDocVw

''' <summary>
''' Set the GUID of this class and specify that this class is ComVisible.
''' A BHO must implement the interface IObjectWithSite. 
''' </summary>
<ComVisible(True)>
<ClassInterface(ClassInterfaceType.None)>
<Guid("AA0B1334-E7F5-4F75-A1DE-0993098AAF01")>
Public Class OpenImageBHO
    Implements IObjectWithSite, IDisposable

    Private disposed As Boolean = False

    ' To register a BHO, a new key should be created under this key.
    Private Const BHORegistryKey As String =
        "Software\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects"

    ' Current IE instance. For IE7 or later version, an IE Tab is just 
    ' an IE instance.
    Private ieInstance As InternetExplorer

    Private openImageDocHostUIHandler As OpenImageHandler


#Region "Com Register/UnRegister Methods"
    ''' <summary>
    ''' When this class is registered to COM, add a new key to the BHORegistryKey 
    ''' to make IE use this BHO.
    ''' On 64bit machine, if the platform of this assembly and the installer is x86,
    ''' 32 bit IE can use this BHO. If the platform of this assembly and the installer
    ''' is x64, 64 bit IE can use this BHO.
    ''' </summary>
    <ComRegisterFunction()>
    Public Shared Sub RegisterBHO(ByVal t As Type)

        ' If the key exists, CreateSubKey will open it.
        Dim bhosKey As RegistryKey =
            Registry.LocalMachine.CreateSubKey(BHORegistryKey, RegistryKeyPermissionCheck.ReadWriteSubTree)

        ' 32 digits separated by hyphens, enclosed in braces: 
        ' {00000000-0000-0000-0000-000000000000}
        Dim bhoKeyStr As String = t.GUID.ToString("B")

        Dim bhoKey As RegistryKey = bhosKey.OpenSubKey(bhoKeyStr, True)

        ' Create a new key.
        If bhoKey Is Nothing Then
            bhoKey = bhosKey.CreateSubKey(bhoKeyStr)
        End If

        ' NoExplorer:dword = 1 prevents the BHO to be loaded by Explorer
        bhoKey.SetValue("NoExplorer", 1)
        bhosKey.Close()
        bhoKey.Close()
    End Sub

    ''' <summary>
    ''' When this class is unregistered from COM, delete the key.
    ''' </summary>
    <ComUnregisterFunction()>
    Public Shared Sub UnregisterBHO(ByVal t As Type)
        Dim bhosKey As RegistryKey =
            Registry.LocalMachine.OpenSubKey(BHORegistryKey, True)
        Dim guidString As String = t.GUID.ToString("B")
        If bhosKey IsNot Nothing Then
            bhosKey.DeleteSubKey(guidString, False)
        End If

        bhosKey.Close()
    End Sub


#End Region

#Region "IObjectWithSite Members"
    ''' <summary>
    ''' This method is called when the BHO is instantiated and when
    ''' it is destroyed. The site is an object implemented the 
    ''' interface InternetExplorer.
    ''' </summary>
    ''' <param name="site"></param>
    Public Sub SetSite(ByVal site As Object) Implements IObjectWithSite.SetSite

        If site IsNot Nothing Then
            ieInstance = CType(site, InternetExplorer)

            openImageDocHostUIHandler = New OpenImageHandler(ieInstance)

            ' Register the DocumentComplete event.
            AddHandler ieInstance.DocumentComplete, AddressOf ieInstance_DocumentComplete
        End If
    End Sub

    ''' <summary>
    ''' Retrieves and returns the specified interface from the last site
    ''' set through SetSite(). The typical implementation will query the
    ''' previously stored pUnkSite pointer for the specified interface.
    ''' </summary>
    Public Sub GetSite(ByRef guid_Renamed As Guid,
                       <Out()> ByRef ppvSite As Object) Implements IObjectWithSite.GetSite

        Dim punk As IntPtr = Marshal.GetIUnknownForObject(ieInstance)
        ppvSite = New Object()
        Dim ppvSiteIntPtr As IntPtr = Marshal.GetIUnknownForObject(ppvSite)
        Dim hr As Integer = Marshal.QueryInterface(punk, guid_Renamed, ppvSiteIntPtr)
        Marshal.Release(punk)
    End Sub
#End Region

#Region "event handler"

    ''' <summary>
    ''' Handle the DocumentComplete event.
    ''' </summary>
    ''' <param name="pDisp">
    ''' The pDisp is an an object implemented the interface InternetExplorer.
    ''' By default, this object is the same as the ieInstance, but if the page 
    ''' contains many frames, each frame has its own document.
    ''' </param>
    Private Sub ieInstance_DocumentComplete(ByVal pDisp As Object, ByRef URL As Object)
        Dim urlstr As String = TryCast(URL, String)

        If String.IsNullOrEmpty(urlstr) _
            OrElse urlstr.Equals("about:blank", StringComparison.OrdinalIgnoreCase) Then
            Return
        End If

        Dim explorer As InternetExplorer = TryCast(pDisp, InternetExplorer)

        ' Set the handler of the document in InternetExplorer.
        If explorer IsNot Nothing Then
            Dim customDoc As NativeMethods.ICustomDoc = CType(explorer.Document, NativeMethods.ICustomDoc)
            customDoc.SetUIHandler(openImageDocHostUIHandler)
        End If
    End Sub
#End Region

#Region "IDisposable Support"
   Public Sub Dispose() Implements IDisposable.Dispose
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub

    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        ' Protect from being called multiple times.
        If disposed Then
            Return
        End If

        If disposing Then
            ' Clean up all managed resources.
            If openImageDocHostUIHandler IsNot Nothing Then
                openImageDocHostUIHandler.Dispose()
            End If

        End If
        disposed = True
    End Sub
#End Region

    
End Class
