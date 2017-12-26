'*************************** Module Header ******************************'
' Module Name:  OpenImageHandler.vb
' Project:      VBCustomIEContextMenu
' Copyright (c) Microsoft Corporation.
' 
' The class OpenImageHandler implements the interface IDocHostUIHandler.ShowContextMenu
' method. For other methods in the interface IDocHostUIHandler, just return 1 which means
' the default handler will be used.
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

Imports VBCustomIEContextMenu.NativeMethods
Imports mshtml
Imports SHDocVw
Imports System.Runtime.InteropServices

Friend Class OpenImageHandler
    Implements NativeMethods.IDocHostUIHandler, IDisposable

    Private disposed As Boolean = False

    ' The IE instance that hosts this WebBrowser Control.
    Public host As InternetExplorer

    ' The custom context menu. 
    Private contextMenu As ContextMenuStrip

    Private menuItem As ToolStripMenuItem


    ''' <summary>
    ''' Initialize the handler.
    ''' </summary>
    Public Sub New(ByVal host As InternetExplorer)
        Me.host = host

        contextMenu = New ContextMenuStrip()
        menuItem = New ToolStripMenuItem()
        menuItem.Size = New Size(180, 100)
        menuItem.Text = "Open in new Tab"
        AddHandler menuItem.Click, AddressOf menuItem_Click
        contextMenu.Items.Add(menuItem)
    End Sub

    Private Sub menuItem_Click(ByVal sender As Object, ByVal e As EventArgs)

        Try
            TryCast(host.Document, HTMLDocument).parentWindow.open(
                TryCast(contextMenu.Tag, String))
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try
    End Sub


#Region "IDocHostUIHandler"

    ''' <summary>
    ''' Show custom Context Menu for Image.
    ''' </summary>
    ''' <param name="dwID">
    ''' A DWORD that specifies the identifier of the shortcut menu to be displayed. 
    ''' See NativeMethods.CONTEXT_MENU_CONST.
    ''' </param>
    ''' <param name="pt">
    ''' The screen coordinates for the menu.
    ''' </param>
    ''' <param name="pcmdtReserved"></param>
    ''' <param name="pdispReserved">
    ''' The object at the screen coordinates specified in ppt. This enables a host to
    ''' pass particular objects, such as anchor tags and images, to provide more 
    ''' specific context.
    ''' </param>
    ''' <returns>
    ''' Return 0 means that host displayed its UI. MSHTML will not attempt to display its UI. 
    ''' </returns>
    Public Function ShowContextMenu(ByVal dwID As Integer,
                                    ByVal pt As POINT,
                                    ByVal pcmdtReserved As Object,
                                    ByVal pdispReserved As Object) As Integer _
                                Implements IDocHostUIHandler.ShowContextMenu

        If dwID = NativeMethods.CONTEXT_MENU_CONST.CONTEXT_MENU_IMAGE Then
            Dim img = TryCast(pdispReserved, IHTMLImgElement)
            If Not img Is Nothing Then
                contextMenu.Tag = img.src
                contextMenu.Show(pt.x, pt.y)
                Return 0
            End If
        End If
        Return 1
    End Function

    Public Function GetHostInfo(ByVal info As DOCHOSTUIINFO) As Integer _
        Implements IDocHostUIHandler.GetHostInfo

        Return 1
    End Function

    Public Function ShowUI(ByVal dwID As Integer,
                           ByVal activeObject As IOleInPlaceActiveObject,
                           ByVal commandTarget As IOleCommandTarget,
                           ByVal frame As IOleInPlaceFrame,
                           ByVal doc As IOleInPlaceUIWindow) As Integer _
                       Implements IDocHostUIHandler.ShowUI

        Return 1
    End Function

    Public Function HideUI() As Integer Implements IDocHostUIHandler.HideUI

        Return 1
    End Function

    Public Function UpdateUI() As Integer Implements IDocHostUIHandler.UpdateUI

        Return 1
    End Function

    Public Function EnableModeless(ByVal fEnable As Boolean) As Integer _
        Implements IDocHostUIHandler.EnableModeless

        Return 1
    End Function

    Public Function OnDocWindowActivate(ByVal fActivate As Boolean) As Integer _
        Implements IDocHostUIHandler.OnDocWindowActivate

        Return 1
    End Function

    Public Function OnFrameWindowActivate(ByVal fActivate As Boolean) As Integer _
        Implements IDocHostUIHandler.OnFrameWindowActivate

        Return 1
    End Function

    Public Function ResizeBorder(ByVal rect As COMRECT,
                                 ByVal doc As IOleInPlaceUIWindow,
                                 ByVal fFrameWindow As Boolean) As Integer _
                             Implements IDocHostUIHandler.ResizeBorder

        Return 1
    End Function

    Public Function TranslateAccelerator(ByRef msg_Renamed As MSG,
                                         ByRef group As Guid,
                                         ByVal nCmdID As Integer) As Integer _
                                     Implements IDocHostUIHandler.TranslateAccelerator

        Return 1
    End Function

    Public Function GetOptionKeyPath(ByVal pbstrKey() As String, ByVal dw As Integer) As Integer _
        Implements IDocHostUIHandler.GetOptionKeyPath

        Return 1
    End Function

    Public Function GetDropTarget(ByVal pDropTarget As IOleDropTarget,
                                  <Out()> ByRef ppDropTarget As IOleDropTarget) _
                              As Integer Implements IDocHostUIHandler.GetDropTarget

        ppDropTarget = Nothing
        Return 1
    End Function

    Public Function GetExternal(<System.Runtime.InteropServices.Out()> ByRef ppDispatch As Object) As Integer _
        Implements IDocHostUIHandler.GetExternal

        ppDispatch = Nothing
        Return 1
    End Function

    Public Function TranslateUrl(ByVal dwTranslate As Integer,
                                 ByVal strURLIn As String,
                                 <Out()> ByRef pstrURLOut As String) As Integer _
                             Implements IDocHostUIHandler.TranslateUrl

        pstrURLOut = String.Empty
        Return 1
    End Function

    Public Function FilterDataObject(ByVal pDO As System.Runtime.InteropServices.ComTypes.IDataObject,
                                     <Out()> ByRef ppDORet As System.Runtime.InteropServices.ComTypes.IDataObject) As Integer _
                                  Implements IDocHostUIHandler.FilterDataObject

        ppDORet = Nothing
        Return 1
    End Function


#End Region

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
            If contextMenu IsNot Nothing Then
                contextMenu.Dispose()
            End If

            If menuItem IsNot Nothing Then
                menuItem.Dispose()
            End If
        End If
        disposed = True
    End Sub



End Class
