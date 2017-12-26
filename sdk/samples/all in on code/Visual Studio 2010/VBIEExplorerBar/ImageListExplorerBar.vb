'*************************** Module Header ******************************'
' Module Name:  ImageListExplorerBar.vb
' Project:	    VBIEExplorerBar
' Copyright (c) Microsoft Corporation.
' 
' The class ImageListExplorerBar inherits the class System.Windows.Forms.UserControl, 
' and implements the IObjectWithSite, IDeskBand, IDockingWindow, IOleWindow and 
' IInputObject interfaces.
' 
' To add an Explorer Bar in IE, you can follow these steps:
' 
' 1. Create a valid GUID for this ComVisible class. 
' 
' 2. Implement the IObjectWithSite, IDeskBand, IDockingWindow, IOleWindow and 
'    IInputObject interfaces.
'    
' 3. Register this assembly to COM.
' 
' 4. 4.Create a new key using the category identifier (CATID) of the type of 
'    Explorer Bar you are creating as the name of the key. This can be one of
'    the following values: 
'    {00021494-0000-0000-C000-000000000046} Defines a horizontal Explorer Bar. 
'    {00021493-0000-0000-C000-000000000046} Defines a vertical Explorer Bar. 
'    
'    The result should look like:
'
'    HKEY_CLASSES_ROOT\CLSID\<Your GUID>\Implemented Categories\{00021494-0000-0000-C000-000000000046}
'    or  
'    HKEY_CLASSES_ROOT\CLSID\<Your GUID>\Implemented Categories\{00021493-0000-0000-C000-000000000046}
'    
' 5. Delete following registry keys because they cache the ExplorerBar enum.
' 
'    HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Discardable\PostSetup\
'    Component Categories\{00021493-0000-0000-C000-000000000046}\Enum
'    or
'    HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Discardable\PostSetup\
'    Component Categories\{00021494-0000-0000-C000-000000000046}\Enum
'
'
' 6. Set the size of the Explorer Bar in the registry.
' 
'    HKEY_LOCAL_MACHINE\Software\Microsoft\Internet Explorer\Explorer Bars\<Your GUID>\BarSize
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
Imports Microsoft.Win32
Imports SHDocVw

<ComVisible(True), Guid("5802D092-1784-4908-8CDB-99B6842D353F")>
Partial Public Class ImageListExplorerBar
    Inherits UserControl
    Implements NativeMethods.IObjectWithSite, NativeMethods.IDeskBand, NativeMethods.IDockingWindow, NativeMethods.IOleWindow, NativeMethods.IInputObject

    ' The title of the explorer bar.
    Private Const imageListExplorerBarTitle As String = "Image List Explorer Bar"

    ' Defines a vertical Explorer Bar. 
    Private Const verticalExplorerBarCATID As String = "{00021493-0000-0000-C000-000000000046}"

    ' The IInputObjectSite object.
    'INSTANT VB NOTE: The variable site was renamed since Visual Basic does not allow class members with the same name:
    Private site_Renamed As NativeMethods.IInputObjectSite

    ' The Internet Explorer instance.
    Private explorer As InternetExplorer

    Public Sub New()
        InitializeComponent()
    End Sub

#Region "NativeMethods.IObjectWithSite"

    Public Sub SetSite(ByVal pUnkSite As Object) Implements NativeMethods.IObjectWithSite.SetSite

        ' Release previous COM objects.
        If Me.site_Renamed IsNot Nothing Then
            Marshal.ReleaseComObject(Me.site_Renamed)
        End If
        If Me.explorer IsNot Nothing Then
            Marshal.ReleaseComObject(Me.explorer)
            Me.explorer = Nothing
        End If

        ' pUnkSite is a pointer to object that implements IOleWindowSite. 
        Me.site_Renamed = TryCast(pUnkSite, NativeMethods.IInputObjectSite)

        If Me.site_Renamed IsNot Nothing Then

            ' The site implements IServiceProvider interface and can be used to 
            ' get InternetExplorer instance.
            Dim provider = TryCast(Me.site_Renamed, NativeMethods._IServiceProvider)
            Dim guid_Renamed As New Guid("{0002DF05-0000-0000-C000-000000000046}")
            Dim riid As New Guid("{00000000-0000-0000-C000-000000000046}")
            Try
                Dim webBrowser As Object
                provider.QueryService(guid_Renamed, riid, webBrowser)
                Me.explorer = TryCast(webBrowser, InternetExplorer)
            Catch e1 As COMException
            End Try
        End If

    End Sub


    Public Sub GetSite(ByRef riid As Guid, <Out()> ByRef ppvSite As Object) Implements NativeMethods.IObjectWithSite.GetSite
        ppvSite = Me.site_Renamed
    End Sub

#End Region

#Region "NativeMethods.IDeskBand"

    ''' <summary>
    ''' Gets the information for a band object.
    ''' </summary>
    Public Sub GetBandInfo(ByVal dwBandID As UInteger,
                           ByVal dwViewMode As UInteger,
                           ByRef pdbi As NativeMethods.DESKBANDINFO) _
                       Implements NativeMethods.IDeskBand.GetBandInfo
        pdbi.wszTitle = ImageListExplorerBar.imageListExplorerBarTitle
        pdbi.ptActual.X = MyBase.Size.Width
        pdbi.ptActual.Y = MyBase.Size.Height
        pdbi.ptMaxSize.X = -1
        pdbi.ptMaxSize.Y = -1
        pdbi.ptMinSize.X = -1
        pdbi.ptMinSize.Y = -1
        pdbi.ptIntegral.X = -1
        pdbi.ptIntegral.Y = -1
        pdbi.dwModeFlags = NativeMethods.DBIM.NORMAL Or NativeMethods.DBIM.VARIABLEHEIGHT
    End Sub

    Public Sub ShowDW(ByVal fShow As Boolean) _
        Implements NativeMethods.IDeskBand.ShowDW, NativeMethods.IDockingWindow.ShowDW
        If fShow Then
            Me.Show()
        Else
            Me.Hide()
        End If
    End Sub

    Public Sub CloseDW(ByVal dwReserved As UInteger) _
        Implements NativeMethods.IDeskBand.CloseDW, NativeMethods.IDockingWindow.CloseDW
        Me.Dispose(True)
    End Sub

    Public Sub ResizeBorderDW(ByVal prcBorder As IntPtr,
                              ByVal punkToolbarSite As Object,
                              ByVal fReserved As Boolean) _
                          Implements NativeMethods.IDeskBand.ResizeBorderDW, NativeMethods.IDockingWindow.ResizeBorderDW
    End Sub

    Public Sub GetWindow(<Out()> ByRef hwnd As IntPtr) _
        Implements NativeMethods.IDeskBand.GetWindow, NativeMethods.IDockingWindow.GetWindow, NativeMethods.IOleWindow.GetWindow
        hwnd = Me.Handle
    End Sub

    Public Sub ContextSensitiveHelp(ByVal fEnterMode As Boolean) _
        Implements NativeMethods.IDeskBand.ContextSensitiveHelp, NativeMethods.IDockingWindow.ContextSensitiveHelp, NativeMethods.IOleWindow.ContextSensitiveHelp
    End Sub

#End Region


#Region "NativeMethods.IInputObject"

    ''' <summary>
    ''' UI-activates or deactivates the object.
    ''' </summary>
    ''' <param name="fActivate">
    ''' Indicates if the object is being activated or deactivated. If this value is 
    ''' nonzero, the object is being activated. If this value is zero, the object is
    ''' being deactivated.
    ''' </param>
    Public Sub UIActivateIO(ByVal fActivate As Integer,
                            ByRef msg As NativeMethods.MSG) _
                        Implements NativeMethods.IInputObject.UIActivateIO
        If fActivate <> 0 Then
            Dim nextControl As Control = MyBase.GetNextControl(Me, True)
            If Control.ModifierKeys = Keys.Shift Then
                nextControl = MyBase.GetNextControl(nextControl, False)
            End If
            If nextControl IsNot Nothing Then
                nextControl.Select()
            End If
            MyBase.Focus()
        End If

    End Sub

    Public Function HasFocusIO() As Integer Implements NativeMethods.IInputObject.HasFocusIO
        If Not MyBase.ContainsFocus Then
            Return 1
        End If
        Return 0
    End Function

    ''' <summary>
    ''' Enables the object to process keyboard accelerators.
    ''' </summary>
    Public Function TranslateAcceleratorIO(ByRef msg As NativeMethods.MSG) As Integer _
        Implements NativeMethods.IInputObject.TranslateAcceleratorIO
        If (msg.message = 256) AndAlso ((msg.wParam = 9) OrElse (msg.wParam = 117)) Then
            If MyBase.SelectNextControl(MyBase.ActiveControl, Control.ModifierKeys <> Keys.Shift, True, True, False) Then
                Return 0
            End If
        End If
        Return 1

    End Function
#End Region

#Region "ComRegister Functions"

    ''' <summary>
    ''' Called when derived class is registered as a COM server.
    ''' </summary>
    <ComRegisterFunctionAttribute()>
    Public Shared Sub Register(ByVal t As Type)

        ' Add the category identifier for a vertical Explorer Bar and set other calues.
        Dim clsidkey As RegistryKey = Registry.ClassesRoot.CreateSubKey("CLSID\" & t.GUID.ToString("B"))
        clsidkey.SetValue(Nothing, ImageListExplorerBar.imageListExplorerBarTitle)
        clsidkey.SetValue("MenuText", ImageListExplorerBar.imageListExplorerBarTitle)
        clsidkey.SetValue("HelpText", "See Readme.txt for detailed help!")
        clsidkey.CreateSubKey("Implemented Categories").CreateSubKey(ImageListExplorerBar.verticalExplorerBarCATID)
        clsidkey.Close()

        ' Set Bar size.
        Dim explorerBarKeyPath As String = "SOFTWARE\Microsoft\Internet Explorer\Explorer Bars\" & t.GUID.ToString("B")
        Dim explorerBarKey As RegistryKey = Registry.LocalMachine.CreateSubKey(explorerBarKeyPath)
        explorerBarKey.SetValue("BarSize", New Byte() {6, 1, 0, 0, 0, 0, 0, 0}, RegistryValueKind.Binary)
        explorerBarKey.Close()


        Registry.CurrentUser.CreateSubKey(explorerBarKeyPath).SetValue(
            "BarSize", New Byte() {6, 1, 0, 0, 0, 0, 0, 0}, RegistryValueKind.Binary)

        ' Remove the cache.
        Try
            Dim catPath As String =
                "Software\Microsoft\Windows\CurrentVersion\Explorer\Discardable\PostSetup\Component Categories\" _
                & ImageListExplorerBar.verticalExplorerBarCATID

            Registry.CurrentUser.OpenSubKey(catPath, True).DeleteSubKey("Enum")
        Catch
        End Try

        Try
            Dim catPath As String =
                "Software\Microsoft\Windows\CurrentVersion\Explorer\Discardable\PostSetup\Component Categories64\" _
                & ImageListExplorerBar.verticalExplorerBarCATID

            Registry.CurrentUser.OpenSubKey(catPath, True).DeleteSubKey("Enum")
        Catch
        End Try
    End Sub

    ''' <summary>
    ''' Called when derived class is unregistered as a COM server.
    ''' </summary>
    <ComUnregisterFunctionAttribute()>
    Public Shared Sub Unregister(ByVal t As Type)
        Dim clsidkeypath As String = t.GUID.ToString("B")
        Registry.ClassesRoot.OpenSubKey("CLSID", True).DeleteSubKeyTree(clsidkeypath)

        Dim explorerBarsKeyPath As String = "SOFTWARE\Microsoft\Internet Explorer\Explorer Bars"

        Registry.LocalMachine.OpenSubKey(explorerBarsKeyPath, True).DeleteSubKey(t.GUID.ToString("B"))
        Registry.CurrentUser.OpenSubKey(explorerBarsKeyPath, True).DeleteSubKey(t.GUID.ToString("B"))


    End Sub


#End Region

    Private Sub btnGetImg_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnGetImg.Click
        lstImg.Items.Clear()

        Dim doc As mshtml.HTMLDocument = TryCast(explorer.Document, mshtml.HTMLDocument)

        Dim imgs = doc.getElementsByTagName("img")

        For Each img In imgs
            Dim imgElement As mshtml.IHTMLImgElement = TryCast(img, mshtml.IHTMLImgElement)
            If imgElement IsNot Nothing AndAlso (Not lstImg.Items.Contains(imgElement.src)) Then
                lstImg.Items.Add(imgElement.src)
            End If
        Next img
    End Sub

    Private Sub lstImg_DoubleClick(ByVal sender As Object, ByVal e As EventArgs) Handles lstImg.DoubleClick
        If lstImg.SelectedItem IsNot Nothing Then
            Dim url As String = TryCast(lstImg.SelectedItem, String)
            Dim doc As mshtml.HTMLDocument = TryCast(explorer.Document, mshtml.HTMLDocument)
            doc.parentWindow.open(url)
        End If
    End Sub
End Class
