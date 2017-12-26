'************************************* Module Header **************************************'
'* Module Name:	Ribbon.vb
'* Project:		VBOutlookRibbonXml
'* Copyright (c) Microsoft Corporation.
'* 
'* The VBOutlookRibbonXml example demonstrates how to use Ribbon XML to create customized 
'* Ribbon for Outlook 2007 inspectors.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 9/17/2009 18:00 PM Li ZhenHao Created

'******************************************************************************************

#Region "Imports directives"
Imports System.Collections.Generic
Imports System.Windows.Forms
Imports Microsoft.Office.Interop.Outlook
Imports Office = Microsoft.Office.Core
Imports System.Diagnostics
Imports System.Runtime.InteropServices
Imports System.Drawing
Imports Microsoft.Office.Tools.Ribbon

#End Region

<Runtime.InteropServices.ComVisible(True)> _
Public Class Ribbon
    Implements Office.IRibbonExtensibility

#Region "Private members"
    ''' <summary>
    ''' The reference to the IRibbonUI interface.
    ''' </summary>
    Private ribbon As Office.IRibbonUI
#End Region

#Region "Private inner class"
    Private Class Settings

        ''' <summary>
        ''' Inspector - Settings pair.
        ''' </summary>
        Private Shared RibbonSettings As New Dictionary(Of Inspector, Settings)
        Public GroupThreeVisible As Boolean = True

        Public MySecondTabVisible As Boolean = False
        Public TxtEditValue As String = "This is a label."

        ''' <summary>
        '''   Get the Inspector specific ribbon item setting.
        ''' </summary>
        ''' <param name="inspector">The target Inspector.</param>
        ''' <returns>Setting associated with the Inspector.</returns>


        Public Shared Function GetSettings(ByVal inspector As Inspector) As Settings

            If Not RibbonSettings.ContainsKey(inspector) Then

                Dim s As New Settings()
                RibbonSettings.Add(inspector, s)
                AddHandler inspector.Close, AddressOf Inspector_Close
                Return s
            End If


            Return RibbonSettings.Item(inspector)
        End Function

        Public Shared Sub Inspector_Close()

            For Each inspector As Inspector In RibbonSettings.Keys

                Dim i As Integer

                For i = 1 To i <= inspector.Application.Inspectors.Count

                    If inspector.Application.Inspectors(i).Equals(inspector) Then

                        Exit For


                    End If

                Next

                If i > inspector.Application.Inspectors.Count Then

                    RibbonSettings.Remove(inspector)
                    Marshal.ReleaseComObject(inspector)
                    GC.Collect()
                    Exit For
                End If


            Next
        End Sub
    End Class

#End Region

#Region ".ctor"
    Public Sub New()
    End Sub
#End Region

#Region "IRibbonExtensibility Members"
    '''<summary>
    ''' Outlook will call this method to get the Ribbon XML.
    ''' </summary>
    Public Function GetCustomUI(ByVal ribbonID As String) As String Implements Office.IRibbonExtensibility.GetCustomUI
        ' We will show our customized Ribbon on the following types of
        ' inspectors
        If ribbonID = "Microsoft.Word.Document" Or _
            ribbonID = "Microsoft.Outlook.Mail.Read" Or _
            ribbonID = "Microsoft.Outlook.Mail.Compose" Or _
            ribbonID = "Microsoft.Outlook.MeetingRequest.Read" Or _
            ribbonID = "Microsoft.Outlook.MeetingRequest.Send" Or _
            ribbonID = "Microsoft.Outlook.Appointment" Or _
            ribbonID = "Microsoft.Outlook.Contact" Or _
            ribbonID = "Microsoft.Outlook.Task" Then

            Return GetResourceText("VBOutlookRibbonXml.Ribbon.xml")

        Else
            Return Nothing
        End If
    End Function
#End Region

#Region "Ribbon Callbacks"
    ' Create callback methods here. For more information about adding callback methods, 
    ' select the Ribbon XML item in Solution Explorer and then press F1

    ''' <summary>
    '''Outlook will call this method when the ribbon is being loaded.
    ''' </summary>
    '''<param name="ribbonUI">Reference to the IRibbonUI interface.</param>
    Public Sub Ribbon_Load(ByVal ribbonUI As Office.IRibbonUI)
        Me.ribbon = ribbonUI
    End Sub

    ''' <summary>
    ''' Outlook will call this method to get the content XML for dynamic menus.
    ''' We can generate different XML contents at runtime so the menu can be *dynamic*.
    ''' </summary>
    ''' <param name="control">The control whose content is being retrieved.</param>
    ''' <returns>Content XML for the control.</returns>
    Public Function Ribbon_GetContent(ByVal control As Office.IRibbonControl) As String

        Dim sb As New StringBuilder()

        Select Case (control.Id)

            Case "mnuSample"
                sb.Append("<menu xmlns=""http://schemas.microsoft.com/office/2006/01/customui"">")
                sb.Append("<dynamicMenu id=""mnuSubMenu"" label=""Sub Menu"" getContent=""Ribbon_GetContent""/>")
                sb.Append("<button id=""btnItem1"" label=""Item 1"" />")
                sb.Append("<menuSeparator id=""separator2"" />")
                sb.Append("<button id=""btnItem2"" label=""Item 2"" />")
                sb.Append("</menu>")
                Exit Select

            Case "mnuSubMenu"
                sb.Append("<menu xmlns=""http://schemas.microsoft.com/office/2006/01/customui"">")
                sb.Append("<button id=""btnSubItem1"" label=""Sub Item 1""/>")
                sb.Append("<button id=""btnSubItem2"" label=""Sub Item 2""/>")
                sb.Append("<button id=""btnSubItem3"" label=""Sub Item 3""/>")
                sb.Append("</menu>")
                Exit Select

        End Select
        Return sb.ToString()
    End Function

    ''' <summary>
    ''' Outlook will call this method to get the item image.
    ''' </summary>
    ''' <param name="imageName">Name of the image.</param>
    ''' <returns>The Bitmap object linked to the imageName.</returns>
    Public Function LoadImage(ByVal imageName As String) As Bitmap

        Dim bmp As Bitmap = Nothing

        Select Case (imageName.ToLowerInvariant())
            Case "globe"
                bmp = My.Resources.Globe

            Case "audiocd"
                bmp = My.Resources.AudioCD

            Case "blankcd"
                bmp = My.Resources.blank_cd

            Case "audiodvd"
                bmp = My.Resources.audiodvd

            Case "bdmovie"
                bmp = My.Resources.Blu_RayMovieDisk

            Case "burncd"
                bmp = My.Resources.BurnCD

            Case "audiocdplus"
                bmp = My.Resources.AudioCDPlus

            Case "vcd"
                bmp = My.Resources.CD_V
        End Select

        Return bmp
    End Function

    ''' <summary>
    ''' Outlook will call this method to get the item's Visible value.
    ''' </summary>
    Public Function GetVisible(ByVal control As Office.IRibbonControl) As Boolean
        Dim visible As Boolean = False
        Dim inspector As Inspector = control.Context

        Select Case (control.Id)

            Case "grpThree"
                visible = Settings.GetSettings(inspector).GroupThreeVisible

            Case "mySecondTab"
                visible = Settings.GetSettings(inspector).MySecondTabVisible

            Case "grpMail"
                visible = TypeOf inspector.CurrentItem Is MailItem

            Case "grpAppointmentItem"
                visible = TypeOf inspector.CurrentItem Is AppointmentItem

            Case "grpTaskItem"
                visible = TypeOf inspector.CurrentItem Is TaskItem

            Case "grpContactItem"
                visible = TypeOf inspector.CurrentItem Is ContactItem
        End Select

        Return visible
    End Function
    ''' <summary>
    ''' Outlook will call this method when SplitButton clicked
    ''' </summary>
    ''' <param name="control"></param>
    ''' <remarks></remarks>
    Public Sub splitButton_Click(ByVal control As Office.IRibbonControl)
        MessageBox.Show("SplitButton Clicked", _
                          "SplitButton Clicked", _
                          MessageBoxButtons.OK, _
                          MessageBoxIcon.Information)
    End Sub

    ''' <summary>    
    '''  Outlook will call this method when SplitButton item is clicked
    ''' </summary>
    ''' <param name="control"></param>
    ''' <remarks></remarks>
    Public Sub btnAlign_Click(ByVal control As Office.IRibbonControl)
        Dim lbl As String = Nothing

        Select Case control.Id
            Case "btnAlignLeft"
                lbl = "Left"

            Case "btnAlignCenter"
                lbl = "Center"

            Case "btnAlignRight"
                lbl = "Right"

        End Select

        MessageBox.Show(lbl, _
                   "SplitButton Item Clicked", _
                   MessageBoxButtons.OK, _
                   MessageBoxIcon.Information)
    End Sub

    ''' <summary>
    ''' Outlook will call this method to get the "Pressed" status of the chkShowGroup
    ''' control. See Ribbon.xml for the chkShowGroup control.
    ''' </summary>
    Public Function chkShowGroup_GetPressed(ByVal control As Office.IRibbonControl) As Boolean
        Return Settings.GetSettings(control.Context).GroupThreeVisible
    End Function

    ''' <summary>
    ''' Outlook will call this method when the chkShowGroup is clicked.
    ''' </summary>
    Public Sub chkShowGroup_OnAction(ByVal control As Office.IRibbonControl, ByVal pressed As Boolean)

        Settings.GetSettings(control.Context).GroupThreeVisible = pressed
        Me.ribbon.InvalidateControl("grpThree")

    End Sub

    ''' <summary>
    ''' Outlook will call this method when Pressed status of the tbSecondTab is needed.
    ''' </summary>
    Public Function tbSecondTab_GetPressed(ByVal control As Office.IRibbonControl) As Boolean
        Return Settings.GetSettings(control.Context).MySecondTabVisible
    End Function

    ''' <summary>
    ''' Outlook will call this method when the tbSecondTab is clicked.
    ''' </summary>
    Public Sub tbSecondTab_OnAction(ByVal control As Office.IRibbonControl, ByVal pressed As Boolean)
        Settings.GetSettings(control.Context).MySecondTabVisible = pressed
        Me.ribbon.InvalidateControl("mySecondTab")
    End Sub

    ''' <summary>
    ''' Outlook will call this method when the content of txtEdit is changed.
    ''' </summary>
    Public Sub txtEdit_OnChange(ByVal control As Office.IRibbonControl, ByVal text As String)
        Settings.GetSettings(control.Context).TxtEditValue = text
        Me.ribbon.InvalidateControl("lblSample")
    End Sub

    ''' <summary>
    ''' Outlook will call this method when the label text of lblSample is needed.
    ''' </summary>
    Public Function lblSample_GetLabel(ByVal control As Office.IRibbonControl) As String
        Return Settings.GetSettings(control.Context).TxtEditValue
    End Function

    ''' <summary>
    ''' Outlook will call this method when the btnWeb button is clicked.
    ''' </summary>
    Public Sub btnWeb_OnAction(ByVal control As Office.IRibbonControl)
        Process.Start("http://cfx.codeplex.com")
    End Sub

    ''' <summary>
    ''' Outlook will call this method when the dialog launcher in group one is clicked.
    ''' </summary>
    Public Sub grpOneDlgLauncher_OnAction(ByVal control As Office.IRibbonControl)
        Using colorDlg As ColorDialog = New ColorDialog()

            If colorDlg.ShowDialog() = DialogResult.OK Then
                MessageBox.Show(colorDlg.Color.ToString(), "Selected Color", MessageBoxButtons.OK, MessageBoxIcon.Information)
            End If
        End Using
    End Sub

    ''' <summary>
    ''' Outlook will call this method when the lblMainMode label is needed.
    ''' </summary>
    Public Function lblMainMode_GetLabel(ByVal control As Office.IRibbonControl) As String
        Dim m As MailItem = (control.Context).CurrentItem

        If Not m Is Nothing Then
            Return String.Format("Current Mode: {0}", IIf(m.EntryID Is Nothing, "Drafting", "Reading"))
        End If

        Return String.Empty
    End Function

#End Region

#Region "Helpers"

    Private Shared Function GetResourceText(ByVal resourceName As String) As String
        Dim asm As Reflection.Assembly = Reflection.Assembly.GetExecutingAssembly()
        Dim resourceNames() As String = asm.GetManifestResourceNames()
        For i As Integer = 0 To resourceNames.Length - 1
            If String.Compare(resourceName, resourceNames(i), StringComparison.OrdinalIgnoreCase) = 0 Then
                Using resourceReader As IO.StreamReader = New IO.StreamReader(asm.GetManifestResourceStream(resourceNames(i)))
                    If resourceReader IsNot Nothing Then
                        Return resourceReader.ReadToEnd()
                    End If
                End Using
            End If
        Next
        Return Nothing
    End Function

#End Region

End Class
