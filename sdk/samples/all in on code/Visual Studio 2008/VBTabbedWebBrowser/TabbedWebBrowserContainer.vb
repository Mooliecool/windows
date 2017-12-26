'*************************** Module Header ******************************'
' Module Name:  TabbedWebBrowserContainer.cs
' Project:	    VBTabbedWebBrowser
' Copyright (c) Microsoft Corporation.
' 
' This is a UserControl that contains a System.Windows.Forms.TabControl. The
' TabControl does not support to create/close a tab in UI, so this UserControl 
' supplies the method to create/close the tab. 
' 
' When add a new TabPage to the TabControl, the type is WebBrowserTabPage which 
' inherits System.Windows.Forms.TabPage, and this UserControl exposes the 
' methods GoBack, GoForward and Refresh of the System.Windows.Forms.WebBrowser
' class. It also subcribes the NewWindow event of WebBrowserTabPage, when a 
' NewWindow event is fired in the WebBrowser, it will create a tab other than 
' open the page in Internet Explorer. 
' 
' The "Open in new Tab" context command is disabled in WebBorwser by default, you
' can add a value *.exe=1 (* means the process name)to the key 
' HKCU\Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_TABBED_BROWSING.
' See http://msdn.microsoft.com/en-us/library/ms537636(VS.85).aspx
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'**************************************************************************'

Imports Microsoft.Win32
Imports System.Security.Permissions

Partial Public Class TabbedWebBrowserContainer
    Inherits UserControl

    ''' <summary>
    ''' A static property to get or set whether the "Open in new Tab" context 
    ''' menu in WebBrowser is enabled.
    ''' </summary>
    Public Shared Property IsTabEnabled() As Boolean
        <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
        Get
            Using key As RegistryKey = Registry.CurrentUser.OpenSubKey( _
                    "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_TABBED_BROWSING")
                Dim processName As String = Process.GetCurrentProcess().ProcessName & ".exe"
                Dim keyValue As Integer = CInt(Fix(key.GetValue(processName, 0)))
                Return keyValue = 1
            End Using
        End Get
        <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
        Set(ByVal value As Boolean)
            Using key As RegistryKey = Registry.CurrentUser.OpenSubKey( _
                    "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_TABBED_BROWSING", True)
                Dim processName As String = Process.GetCurrentProcess().ProcessName & ".exe"
                Dim keyValue As Integer = CInt(Fix(key.GetValue(processName, 0)))

                Dim isEnabled As Boolean = keyValue = 1
                If isEnabled <> value Then
                    key.SetValue(processName, If(value, 1, 0))
                End If
            End Using
        End Set
    End Property

    ''' <summary>
    ''' The select tab of the tab control.
    ''' </summary>
    Public ReadOnly Property ActiveTab() As WebBrowserTabPage
        Get
            If TabControl.SelectedTab IsNot Nothing Then
                Return TryCast(TabControl.SelectedTab, WebBrowserTabPage)
            Else
                Return Nothing
            End If
        End Get
    End Property

    ''' <summary>
    ''' This control should have one tab at least.
    ''' </summary>
    Public ReadOnly Property CanCloseActivePage() As Boolean
        Get
            Return TabControl.TabPages.Count > 1
        End Get
    End Property


    Public Sub New()
        InitializeComponent()
    End Sub

    ''' <summary>
    ''' Create a new tab and navigate to "about:blank" when the control is loaded.
    ''' </summary>
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Protected Overrides Sub OnLoad(ByVal e As EventArgs)
        MyBase.OnLoad(e)
        NewTab("about:blank")
    End Sub

    ''' <summary>
    ''' Navigate the WebBrowser control in the ActiveTab to the Url.
    ''' </summary>
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Public Sub Navigate(ByVal url As String)
        If Me.ActiveTab IsNot Nothing Then
            Try
                Me.ActiveTab.WebBrowser.Navigate(url)
            Catch e1 As ArgumentException

            Catch
                Throw
            End Try
        End If
    End Sub

    ''' <summary>
    ''' Create a new WebBrowserTabPage instance, add it to the tab control, and 
    ''' subscribe the its NewWindow event.
    ''' </summary>
    ''' <returns></returns>
    Private Function CreateTabPage() As WebBrowserTabPage
        Dim tab As New WebBrowserTabPage()
        AddHandler tab.NewWindow, AddressOf tab_NewWindow
        Me.tabControl.TabPages.Add(tab)
        Me.tabControl.SelectedTab = tab
        Return tab
    End Function

    ''' <summary>
    ''' Create a WebBrowserTabPage and then navigate to the Url.
    ''' </summary>
    ''' <param name="url"></param>
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Public Sub NewTab(ByVal url As String)
        CreateTabPage()
        Navigate(url)
    End Sub

    ''' <summary>
    ''' Close the active tab.
    ''' </summary>
    Public Sub CloseActiveTab()
        ' This control should have one tab at least.
        If CanCloseActivePage Then
            Dim tabToClose = Me.ActiveTab
            Me.tabControl.TabPages.Remove(tabToClose)
        End If
    End Sub

    ''' <summary>
    ''' Handle the NewWindow event of the WebBrowserTabPage. when a NewWindow event
    ''' is fired in the WebBrowser, create a tab other than open the page in Internet
    ''' Explorer. 
    ''' </summary>
    Private Sub tab_NewWindow(ByVal sender As Object, ByVal e As WebBrowserNewWindowEventArgs)
        If TabbedWebBrowserContainer.IsTabEnabled Then
            NewTab(e.Url)
            e.Cancel = True
        End If
    End Sub

    ''' <summary>
    ''' Expose the GoBack method of the WebBrowser control in the ActiveTab.
    ''' </summary>
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Public Sub GoBack()
        Me.ActiveTab.WebBrowser.GoBack()
    End Sub

    ''' <summary>
    ''' Expose the GoForward method of the WebBrowser control in the ActiveTab.
    ''' </summary>
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Public Sub GoForward()
        Me.ActiveTab.WebBrowser.GoForward()
    End Sub

    ''' <summary>
    ''' Expose the Refresh method of the WebBrowser control in the ActiveTab.
    ''' </summary>
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Public Sub RefreshWebBrowser()
        Me.ActiveTab.WebBrowser.Refresh()
    End Sub
End Class
