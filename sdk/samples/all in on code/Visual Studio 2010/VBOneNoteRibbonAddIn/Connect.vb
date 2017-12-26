'***************************** Module Header ******************************\
'Module Name:  Connect.vb
'Project:      VBOneNoteRibbonAddIn
'Copyright (c) Microsoft Corporation.
'
'Hosts the event notifications that occur to add-ins, such as when they are loaded, 
'unloaded, updated, and so forth.
'
'This source is subject to the Microsoft Public License.
'See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'All other rights reserved.
'
'THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************

Imports Extensibility
Imports System.Runtime.InteropServices
Imports Microsoft.Office.Core
Imports System.IO
Imports VBOneNoteRibbonAddIn.My
Imports Microsoft.Office.Interop.OneNote
Imports System.Drawing.Imaging
Imports System.Runtime.InteropServices.ComTypes
Imports VBOneNoteRibbonAddIn
Imports OneNote = Microsoft.Office.Interop.OneNote


#Region " Read me for Add-in installation and setup information. "
' When run, the Add-in wizard prepared the registry for the Add-in.
' At a later time, if the Add-in becomes unavailable for reasons such as:
'   1) You moved this project to a computer other than which is was originally created on.
'   2) You chose 'Yes' when presented with a message asking if you wish to remove the Add-in.
'   3) Registry corruption.
' you will need to re-register the Add-in by building the VBOneNoteRibbonAddInSetup project, 
' right click the project in the Solution Explorer, then choose install.
#End Region

<GuidAttribute("89BE33A8-624A-47AE-B7BA-E8B9117A5BC4"),
 ProgIdAttribute("VBOneNoteRibbonAddIn.Connect")> _
Public Class Connect

    Implements Extensibility.IDTExtensibility2
    Implements IRibbonExtensibility

    Private applicationObject As Object
    Private addInInstance As Object

    Public Sub OnBeginShutdown(ByRef custom As System.Array) _
        Implements Extensibility.IDTExtensibility2.OnBeginShutdown

        MessageBox.Show("VBOneNoteRibbonAddIn OnBeginShutdown")
        If Me.applicationObject IsNot Nothing Then
            Me.applicationObject = Nothing
        End If
    End Sub

    Public Sub OnAddInsUpdate(ByRef custom As System.Array) _
        Implements Extensibility.IDTExtensibility2.OnAddInsUpdate

        MessageBox.Show("VBOneNoteRibbonAddIn OnAddInsUpdate")
    End Sub

    Public Sub OnStartupComplete(ByRef custom As System.Array) _
        Implements Extensibility.IDTExtensibility2.OnStartupComplete

        MessageBox.Show("VBOneNoteRibbonAddIn OnStartupComplete")
    End Sub

    Public Sub OnDisconnection(ByVal RemoveMode As Extensibility.ext_DisconnectMode, _
                               ByRef custom As System.Array) _
                           Implements Extensibility.IDTExtensibility2.OnDisconnection

        MessageBox.Show("VBOneNoteRibbonAddIn OnDisconnection")
        Me.applicationObject = Nothing
        GC.Collect()
        GC.WaitForPendingFinalizers()
    End Sub

    Public Sub OnConnection(ByVal application As Object, _
                            ByVal connectMode As Extensibility.ext_ConnectMode, _
                            ByVal addInInst As Object, _
                            ByRef custom As System.Array) _
                        Implements Extensibility.IDTExtensibility2.OnConnection
        MessageBox.Show("VBOneNoteRibbonAddIn OnConnection")
        applicationObject = application
        addInInstance = addInInst
    End Sub

    ''' <summary>
    '''     Loads the XML markup from an XML customization file 
    '''     that customizes the Ribbon user interface.
    ''' </summary>
    ''' <param name="RibbonID">The ID for the RibbonX UI</param>
    ''' <returns>string</returns>
    Public Function GetCustomUI(ByVal RibbonID As String) As String _
        Implements IRibbonExtensibility.GetCustomUI

        Return Resources.customUI
    End Function

    ''' <summary>
    '''     Implements the OnGetImage method in customUI.xml
    ''' </summary>
    ''' <param name="imageName">the image name in customUI.xml</param>
    ''' <returns>memory stream contains image</returns>
    Public Function OnGetImage(ByVal imageName As String) As IStream
        Dim stream As New MemoryStream()

        If imageName = "showform.png" Then
            Resources.showform.Save(stream, ImageFormat.Png)
        End If

        Return New ReadOnlyIStreamWrapper(stream)
    End Function

    ''' <summary>
    '''     show Windows Form method
    ''' </summary>
    ''' <param name="control">Represents the object passed into every
    ''' Ribbon user interface (UI) control's callback procedure.</param>
    Public Sub ShowForm(ByVal control As IRibbonControl)
        Dim context As OneNote.Window = TryCast(control.Context, OneNote.Window)
        Dim owner As New CWin32WindowWrapper(CType(context.WindowHandle, IntPtr))
        Dim form As New TestForm(TryCast(applicationObject, OneNote.Application))
        form.ShowDialog(owner)

        form.Dispose()
        form = Nothing
        context = Nothing
        owner = Nothing
        GC.Collect()
        GC.WaitForPendingFinalizers()
        GC.Collect()
    End Sub

End Class
