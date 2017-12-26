'*************************** Module Header ******************************'
' Module Name:  BHOIEContextMenu.vb
' Project:	    VBBrowserHelperObject
' Copyright (c) Microsoft Corporation.
' 
' The class BHOIEContextMenu is a Browser Helper Object which runs within Internet
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
' This class is used to disable the default context menu in IE. It also supplies 
' functions to register this BHO to IE.
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
Imports Microsoft.Win32
Imports SHDocVw
Imports mshtml

''' <summary>
''' Set the GUID of this class and specify that this class is ComVisible.
''' A BHO must implement the interface IObjectWithSite. 
''' </summary>
<ComVisible(True), ClassInterface(ClassInterfaceType.None),
Guid("C42D40F0-BEBF-418D-8EA1-18D99AC2AB17")>
Public Class BHOIEContextMenu
    Implements IObjectWithSite
    ' Current IE instance. For IE7 or later version, an IE Tab is just 
    ' an IE instance.
    Private ieInstance As InternetExplorer

    ' To register a BHO, a new key should be created under this key.
    Private Const BHORegistryKey As String =
    "Software\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects"



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
        Dim key As RegistryKey = Registry.LocalMachine.OpenSubKey(BHORegistryKey, True)
        If key Is Nothing Then
            key = Registry.LocalMachine.CreateSubKey(BHORegistryKey)
        End If

        ' 32 digits separated by hyphens, enclosed in braces: 
        ' {00000000-0000-0000-0000-000000000000}
        Dim bhoKeyStr As String = t.GUID.ToString("B")

        Dim bhoKey As RegistryKey = key.OpenSubKey(bhoKeyStr, True)

        ' Create a new key.
        If bhoKey Is Nothing Then
            bhoKey = key.CreateSubKey(bhoKeyStr)
        End If

        ' NoExplorer:dword = 1 prevents the BHO to be loaded by Explorer
        Dim name As String = "NoExplorer"
        Dim value As Object = CObj(1)
        bhoKey.SetValue(name, value)
        key.Close()
        bhoKey.Close()
    End Sub

    ''' <summary>
    ''' When this class is unregistered from COM, delete the key.
    ''' </summary>
    <ComUnregisterFunction()>
    Public Shared Sub UnregisterBHO(ByVal t As Type)
        Dim key As RegistryKey = Registry.LocalMachine.OpenSubKey(BHORegistryKey, True)
        Dim guidString As String = t.GUID.ToString("B")
        If key IsNot Nothing Then
            key.DeleteSubKey(guidString, False)
        End If
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
                       <System.Runtime.InteropServices.Out()> ByRef ppvSite As Object) _
                   Implements IObjectWithSite.GetSite
        Dim punk As IntPtr = Marshal.GetIUnknownForObject(ieInstance)
        ppvSite = New Object()
        Dim ppvSiteIntPtr As IntPtr = Marshal.GetIUnknownForObject(ppvSite)
        Dim hr As Integer = Marshal.QueryInterface(punk, guid_Renamed, ppvSiteIntPtr)
        Marshal.ThrowExceptionForHR(hr)
        Marshal.Release(ppvSiteIntPtr)
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
        Dim urlstring As String = TryCast(URL, String)

        If String.IsNullOrEmpty(urlstring) _
            OrElse urlstring.Equals("about:blank", StringComparison.OrdinalIgnoreCase) Then
            Return
        End If

        Dim explorer As InternetExplorer = TryCast(pDisp, InternetExplorer)

        ' Set the handler of the document in InternetExplorer.
        If explorer IsNot Nothing Then
            SetHandler(explorer)
        End If
    End Sub


    Private Sub SetHandler(ByVal explorer As InternetExplorer)
        Try

            ' Register the oncontextmenu event of the  document in InternetExplorer.
            Dim helper As New HTMLDocumentEventHelper(
                TryCast(explorer.Document, HTMLDocument))
            AddHandler helper.oncontextmenu, AddressOf oncontextmenuHandler
        Catch
        End Try
    End Sub

    ''' <summary>
    ''' Handle the oncontextmenu event.
    ''' </summary>
    ''' <param name="e"></param>
    Private Sub oncontextmenuHandler(ByVal e As IHTMLEventObj)

        ' To cancel the default behavior, set the returnValue property of the event
        ' object to false.
        e.returnValue = False

    End Sub

#End Region

End Class
