'***************************** Module Header ******************************\
'Module Name:  TestForm.vb
'Project:      VBOneNoteRibbonAddIn
'Copyright (c) Microsoft Corporation.
'
'The Window Form for testing to open
'
'This source is subject to the Microsoft Public License.
'See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'All other rights reserved.
'
'THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************
Imports OneNote = Microsoft.Office.Interop.OneNote
Imports System.Runtime.InteropServices
Imports System.Xml.Linq
Imports System.Linq
<ComVisible(False)> _
Partial Public Class TestForm
    Inherits Form
    Private oneNoteApp As OneNote.Application = Nothing

    Public Sub New(ByVal oneNote As OneNote.Application)
        oneNoteApp = oneNote
        InitializeComponent()
    End Sub
    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
        Handles btnGetPageTitle.Click

        MessageBox.Show(GetPageTitle())
    End Sub
    ''' <summary>
    ''' Get the title of the page
    ''' </summary>
    ''' <returns>string</returns>
    Private Function GetPageTitle() As String
        Dim pageXmlOut As String = GetActivePageContent()
        Dim doc = XDocument.Parse(pageXmlOut)
        Dim pageTitle As String = ""
        pageTitle = doc.Descendants().FirstOrDefault().Attribute("ID").NextAttribute.Value

        Return pageTitle
    End Function

    ''' <summary>
    ''' Get active page content and output the xml string
    ''' </summary>
    ''' <returns>string</returns>
    Private Function GetActivePageContent() As String
        Dim activeObjectID As String = Me.GetActiveObjectID(_ObjectType.Page)
        Dim pageXmlOut As String = ""
        oneNoteApp.GetPageContent(activeObjectID, pageXmlOut)

        Return pageXmlOut
    End Function

    ''' <summary>
    ''' Get ID of current page 
    ''' </summary>
    ''' <param name="obj">_Object Type</param>
    ''' <returns>current page Id</returns>
    Private Function GetActiveObjectID(ByVal obj As _ObjectType) As String
        Dim currentPageId As String = ""
        Dim count As UInteger = oneNoteApp.Windows.Count
        For Each window As OneNote.Window In oneNoteApp.Windows
            If window.Active Then
                Select Case obj
                    Case _ObjectType.Notebook
                        currentPageId = window.CurrentNotebookId
                        Exit Select
                    Case _ObjectType.Section
                        currentPageId = window.CurrentSectionId
                        Exit Select
                    Case _ObjectType.SectionGroup
                        currentPageId = window.CurrentSectionGroupId
                        Exit Select
                End Select

                currentPageId = window.CurrentPageId
            End If
        Next

        Return currentPageId

    End Function

    ''' <summary>
    ''' Nested types
    ''' </summary>
    Private Enum _ObjectType
        Notebook
        Section
        SectionGroup
        Page
        SelectedPages
        PageObject
    End Enum
End Class