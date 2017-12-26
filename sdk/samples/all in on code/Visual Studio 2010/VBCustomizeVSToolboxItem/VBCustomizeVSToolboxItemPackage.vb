'****************************** Module Header ******************************\
' Module Name:  VBCustomizeVSToolboxItemPackage.vb
' Project:	    VBCustomizeVSToolboxItem
' Copyright (c) Microsoft Corporation.
' 
' The class VBCustomizeVSToolboxItemPackage inherits the class 
' Microsoft.VisualStudio.Shell.Package class. It overrides the Initialize method.
' 
' If you add a new item to Vs2010 toolbox, the display name and tooltip of the new
' item are the same by default. The sample demonstrates how to add an item with 
' custom tooltip to Visual Studio Toolbox. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Imports System.Globalization
Imports System.IO
Imports System.Runtime.InteropServices
Imports System.Text
Imports System.Windows.Forms
Imports Microsoft.VisualStudio
Imports Microsoft.VisualStudio.Shell
Imports Microsoft.VisualStudio.Shell.Interop

''' <summary>
''' This is the class that implements the package exposed by this assembly.
'''
''' The minimum requirement for a class to be considered a valid package for Visual Studio
''' is to implement the IVsPackage interface and register itself with the shell.
''' This package uses the helper classes defined inside the Managed Package Framework (MPF)
''' to do it: it derives from the Package class that provides the implementation of the 
''' IVsPackage interface and uses the registration attributes defined in the framework to 
''' register itself and its components with the shell.
''' </summary>
<DefaultRegistryRoot("Software\Microsoft\VisualStudio\10.0"), _
PackageRegistration(UseManagedResourcesOnly:=True), _
ProvideAutoLoad(VSConstants.UICONTEXT.SolutionExists_string), _
InstalledProductRegistration("#110", "#112", "1.0", IconResourceID:=400), _
Guid(GuidList.guidVBCustomizeVSToolboxItemPkgString), _
ProvideToolboxItems(1)> _
Public NotInheritable Class VBCustomizeVSToolboxItemPackage
    Inherits Package

    ' Define the tab, item, tooltip, description and drag-drop text.

    Const toolboxTabString As String = "VB Custom Toolbox Tab"
    Const toolboxItemString As String = "VB Custom Toolbox Item"
    Const toolboxTooltipString As String = "VB Custom Toolbox Tooltip"
    Const toolboxDescriptionString As String = "VB Custom Toolbox Description"
    Const toolboxItemTextString As String = "VB Hello world!"

    ' The IVsToolbox2 service.
    Dim vsToolbox2 As IVsToolbox2

    ' The IVsActivityLog service.
    Dim vsActivityLog As IVsActivityLog

    ' Memory stream to store the tooltip data.
    Dim tooltipStream As Stream

    ''' <summary>
    ''' Default constructor of the package.
    ''' Inside this method you can place any initialization code that does not require 
    ''' any Visual Studio service because at this point the package object is created but 
    ''' not sited yet inside Visual Studio environment. The place to do all the other 
    ''' initialization is the Initialize method.
    ''' </summary>
    Public Sub New()
        Trace.WriteLine(String.Format(CultureInfo.CurrentCulture, "Entering constructor for: {0}", Me.GetType().Name))
    End Sub



    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    ' Overriden Package Implementation
#Region "Package Members"

    ''' <summary>
    ''' Initialization of the package; this method is called right after the package is sited, so this is the place
    ''' where you can put all the initialization code that rely on services provided by VisualStudio.
    ''' </summary>
    Protected Overrides Sub Initialize()
        MyBase.Initialize()

        ' Initialize the services.
        vsActivityLog = TryCast(GetService(GetType(SVsActivityLog)), IVsActivityLog)
        vsToolbox2 = TryCast(GetService(GetType(SVsToolbox)), IVsToolbox2)

        LogEntry(__ACTIVITYLOG_ENTRYTYPE.ALE_INFORMATION, String.Format("Entering initializer for: {0}", Me.ToString()))

        ' Add the toolbox item if it does not exist.
        Try
            If Not VerifyToolboxTabExist() Then
                AddToolboxTab()
            End If

            If Not VerifyToolboxItemExist() Then
                AddToolboxItem()
            End If

            vsToolbox2.UpdateToolboxUI()
        Catch ex As Exception
            LogEntry(__ACTIVITYLOG_ENTRYTYPE.ALE_ERROR, ex.Message)
            LogEntry(__ACTIVITYLOG_ENTRYTYPE.ALE_ERROR, ex.StackTrace)
        End Try
    End Sub

    ''' <summary>
    ''' Log the VS activity using the IVsActivityLog service.
    ''' </summary>
    Private Sub LogEntry(ByVal type As __ACTIVITYLOG_ENTRYTYPE, ByVal message As String)

        If vsActivityLog IsNot Nothing Then
            Dim hr As Integer = vsActivityLog.LogEntry(CUInt(type), Me.ToString(), message)
            ErrorHandler.ThrowOnFailure(hr)
        End If
    End Sub

    ''' <summary>
    ''' Verify whether the toolbox tab exists.
    ''' </summary>
    Private Function VerifyToolboxTabExist() As Boolean
        LogEntry(__ACTIVITYLOG_ENTRYTYPE.ALE_INFORMATION, String.Format("Entering VerifyToolboxTabExist for: {0}", Me.ToString()))

        Dim exist As Boolean = False

        Dim tabs As IEnumToolboxTabs = Nothing
        Dim num As UInteger

        ErrorHandler.ThrowOnFailure(vsToolbox2.EnumTabs(tabs))
        Dim rgelt(0) As String
        Dim i As Integer = tabs.Next(1, rgelt, num)
        Do While (ErrorHandler.Succeeded(i) AndAlso (num > 0)) AndAlso (rgelt(0) IsNot Nothing)
            If rgelt(0) = toolboxTabString Then
                exist = True
                Exit Do
            End If
            i = tabs.Next(1, rgelt, num)
        Loop

        LogEntry(__ACTIVITYLOG_ENTRYTYPE.ALE_INFORMATION, String.Format("VerifyToolboxTabExist {0}: {1}", toolboxTabString, exist))

        Return exist
    End Function

    ''' <summary>
    ''' Add the toolbox tab. 
    ''' </summary>
    Private Sub AddToolboxTab()
        LogEntry(__ACTIVITYLOG_ENTRYTYPE.ALE_INFORMATION, String.Format("Entering AddToolboxTab for: {0}", toolboxTabString))

        ErrorHandler.ThrowOnFailure(vsToolbox2.AddTab(toolboxTabString))
    End Sub

    ''' <summary>
    ''' Verify whether the toolbox item exists.
    ''' </summary>
    Private Function VerifyToolboxItemExist() As Boolean
        LogEntry(__ACTIVITYLOG_ENTRYTYPE.ALE_INFORMATION, String.Format("Entering VerifyToolboxItemExist for: {0}", Me.ToString()))

        Dim exist As Boolean = False

        Dim items As IEnumToolboxItems = Nothing
        Dim num As UInteger
        ErrorHandler.ThrowOnFailure(vsToolbox2.EnumItems(toolboxTabString, items))
        Dim rgelt = New Microsoft.VisualStudio.OLE.Interop.IDataObject(0) {}
        Dim i As Integer = items.Next(1, rgelt, num)
        Do While (ErrorHandler.Succeeded(i) AndAlso (num > 0)) AndAlso (rgelt(0) IsNot Nothing)
            Dim displayName As String = Nothing

            Dim hr = (TryCast(vsToolbox2, IVsToolbox3)).GetItemDisplayName(rgelt(0), displayName)
            ErrorHandler.ThrowOnFailure(hr)

            If displayName.Equals(toolboxItemString, StringComparison.OrdinalIgnoreCase) Then
                exist = True
                Exit Do
            End If
            i = items.Next(1, rgelt, num)
        Loop

        LogEntry(__ACTIVITYLOG_ENTRYTYPE.ALE_INFORMATION, String.Format("VerifyToolboxItemExist {0}: {1}", toolboxItemString, exist))

        Return exist
    End Function


    ''' <summary>
    ''' Add the toolbox item. 
    ''' </summary>
    Private Sub AddToolboxItem()
        LogEntry(__ACTIVITYLOG_ENTRYTYPE.ALE_INFORMATION, String.Format("Entering AddToolboxItem for: {0}", toolboxItemString))

        tooltipStream = SaveStringToStreamRaw(FormatTooltipData(toolboxTooltipString, toolboxDescriptionString))
        Dim toolboxData = New Microsoft.VisualStudio.Shell.OleDataObject()

        ' Set the tooltip.
        toolboxData.SetData("VSToolboxTipInfo", tooltipStream)

        ' Set the Drag-Drop text.
        toolboxData.SetData(DataFormats.Text, toolboxItemTextString)

        Dim itemInfo(0) As TBXITEMINFO
        itemInfo(0).bstrText = toolboxItemString
        itemInfo(0).hBmp = IntPtr.Zero
        itemInfo(0).dwFlags = CUInt(__TBXITEMINFOFLAGS.TBXIF_DONTPERSIST)

        ErrorHandler.ThrowOnFailure(vsToolbox2.AddItem(toolboxData, itemInfo, toolboxTabString))
    End Sub

    ''' <summary>
    ''' Format the tooltip.
    ''' </summary>
    Private Function FormatTooltipData(ByVal toolName As String, ByVal description As String) As String
        Const NameHeader As String = "Name:"
        Const DescriptionHeader As String = "Description:"

        Dim ch As Char = CChar(ChrW((1 + NameHeader.Length) + toolName.Length))
        Dim str As String = ch.ToString() & NameHeader & toolName
        If description IsNot Nothing Then
            ch = CChar(ChrW((1 + DescriptionHeader.Length) + description.Length))
            str = str & ch.ToString() & DescriptionHeader & description
        End If
        str &= vbNullChar
        Return str
    End Function

    Private Function SaveStringToStreamRaw(ByVal value As String) As Stream
        Dim bytes() As Byte = New UnicodeEncoding().GetBytes(value)
        Dim stream As MemoryStream = Nothing
        If (bytes IsNot Nothing) AndAlso (bytes.Length > 0) Then
            stream = New MemoryStream(bytes.Length)
            stream.Write(bytes, 0, bytes.Length)
            stream.Flush()
        Else
            stream = New MemoryStream()
        End If
        stream.WriteByte(0)
        stream.WriteByte(0)
        stream.Flush()
        stream.Position = 0L
        Return stream
    End Function

#End Region

    Protected Overrides Sub Dispose(disposing As Boolean)
        MyBase.Dispose(disposing)

        If disposing And Me.tooltipStream IsNot Nothing Then
            Me.tooltipStream.Dispose()
        End If
    End Sub

End Class
