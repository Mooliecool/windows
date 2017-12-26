'************************** Module Header ******************************'
' Module Name:  IEDownloadManager.vb
' Project:      VBIEDownloadManager
' Copyright (c) Microsoft Corporation.
' 
' The ability to implement a custom download manager was introduced in Microsoft 
' Internet Explorer 5.5. This feature enables you to extend the functionality of 
' Windows Internet Explorer and WebBrowser applications by implementing a Component
' Object Model (COM) object to handle the file download process.
' 
' A download manager is implemented as a COM object that exposes the IUnknown and
' IDownloadManager interface. IDownloadManager has only one method, 
' IDownloadManager::Download, which is called by Internet Explorer or a WebBrowser
' application to download a file. 
' 
' For Internet Explorer 6 and later, if the WebBrowser application does not implement
' the IServiceProvider::QueryService method, or when using Internet Explorer itself 
' for which IServiceProvider::QueryService cannot be implemented, the application 
' checks for the presence of a registry key containing the class identifier (CLSID) 
' of the download manager COM object. The CLSID can be provided in either of the 
' following registry values.
' 
'     HKEY_LOCAL_MACHINE
'          Software
'               Microsoft
'                    Internet Explorer
'                         DownloadUI
'     HKEY_CURRENT_USER
'          Software
'               Microsoft
'                    Internet Explorer
'                         DownloadUI
' 
' DownloadUI is not a key, it is a REG_SZ value under Software\Microsoft\Internet Explorer.
' 
' If the application cannot locate a custom download manager the default download user 
' interface is used.
' 
' The IEDownloadManager class implements the IDownloadManager interface. When IE starts to 
' download a file, the Download method of this class will be called, and then the 
' VBWebDownloader.exe will be launched to download the file.
' 
' This class will also set the registry values when this assembly is registered to COM.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************'

Imports System.IO
Imports System.Reflection
Imports System.Runtime.InteropServices.ComTypes
Imports Microsoft.Win32
Imports System.Runtime.InteropServices

<ComVisible(True), Guid("bdb9c34c-d0ca-448e-b497-8de62e709744")>
Public Class IEDownloadManager
    Implements NativeMethods.IDownloadManager

    ''' <summary>
    ''' Return S_OK (0) so that IE will stop to download the file itself. 
    ''' Else the default download user interface is used.
    ''' </summary>
    Public Function Download(ByVal pmk As IMoniker, ByVal pbc As IBindCtx,
                             ByVal dwBindVerb As UInteger, ByVal grfBINDF As Integer,
                             ByVal pBindInfo As IntPtr, ByVal pszHeaders As String,
                             ByVal pszRedir As String, ByVal uiCP As UInteger) As Integer Implements NativeMethods.IDownloadManager.Download

        ' Get the display name of the pointer to an IMoniker interface that specifies
        ' the object to be downloaded.
        Dim name As String = String.Empty
        pmk.GetDisplayName(pbc, Nothing, name)

        If Not String.IsNullOrEmpty(name) Then
            Dim url As Uri = Nothing
            Dim result As Boolean = Uri.TryCreate(name, UriKind.Absolute, url)

            If result Then

                ' Launch VBWebDownloader.exe to download the file. 
                Dim assemblyFile As New FileInfo(
                    System.Reflection.Assembly.GetExecutingAssembly().Location)
                Dim start As ProcessStartInfo = New ProcessStartInfo With _
                    {.Arguments = name,
                     .FileName = String.Format("{0}\VBWebDownloader.exe", assemblyFile.DirectoryName)}
                Process.Start(start)
                Return 0
            End If
        End If
        Return 1
    End Function

#Region "ComRegister Functions"

    ''' <summary>
    ''' Called when derived class is registered as a COM server.
    ''' </summary>
    <ComRegisterFunction()>
    Public Shared Sub Register(ByVal t As Type)
        Dim ieKeyPath As String = "SOFTWARE\Microsoft\Internet Explorer\"
        Using ieKey As RegistryKey = Registry.CurrentUser.CreateSubKey(ieKeyPath)
            ieKey.SetValue("DownloadUI", t.GUID.ToString("B"))
        End Using
    End Sub

    ''' <summary>
    ''' Called when derived class is unregistered as a COM server.
    ''' </summary>
    <ComUnregisterFunction()>
    Public Shared Sub Unregister(ByVal t As Type)
        Dim ieKeyPath As String = "SOFTWARE\Microsoft\Internet Explorer\"
        Using ieKey As RegistryKey = Registry.CurrentUser.OpenSubKey(ieKeyPath, True)
            ieKey.DeleteValue("DownloadUI")
        End Using
    End Sub
#End Region

End Class
