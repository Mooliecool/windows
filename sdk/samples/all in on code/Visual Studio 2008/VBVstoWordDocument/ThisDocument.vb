'* Copyright (c) Microsoft Corporation.
'* 
'* The VBWordDocument project provides the examples on how manipulate Word 2007 Content Controls 
'* in a VSTO document-level project

'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\******************************************************************************************/

#Region "Importing directives"
Imports System
Imports System.Collections.Generic
Imports System.Data
Imports System.Linq
Imports System.Text
Imports System.Windows.Forms
Imports System.Xml.Linq
Imports Microsoft.VisualStudio.Tools.Applications.Runtime
Imports Office = Microsoft.Office.Core
Imports Word = Microsoft.Office.Interop.Word
Imports System.Diagnostics
#End Region


Public Class ThisDocument

    Private Sub ThisDocument_Startup(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Startup
        Me.ActionsPane.Visible = True
        Dim p As PaneControl = New PaneControl()
        Me.ActionsPane.Controls.Add(p)
    End Sub

    Private Sub ThisDocument_Shutdown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shutdown

    End Sub

    Private Sub ThisDocument_ContentControlAfterAdd(ByVal NewContentControl As Microsoft.Office.Interop.Word.ContentControl, ByVal InUndoRedo As System.Boolean) Handles Me.ContentControlAfterAdd
        Debug.Print("ThisDocument_ContentControlAfterAdd :" + NewContentControl.Title)
    End Sub

    Private Sub ThisDocument_ContentControlBeforeContentUpdate(ByVal ContentControl As Microsoft.Office.Interop.Word.ContentControl, ByRef Content As System.String) Handles Me.ContentControlBeforeContentUpdate
        Debug.Print("ThisDocument_ContentControlBeforeContentUpdate :" + ContentControl.Title)
    End Sub

    Private Sub ThisDocument_ContentControlBeforeDelete(ByVal OldContentControl As Microsoft.Office.Interop.Word.ContentControl, ByVal InUndoRedo As System.Boolean) Handles Me.ContentControlBeforeDelete
        Debug.Print("ThisDocument_ContentControlBeforeDelete :" + OldContentControl.Title)
    End Sub

    Private Sub ThisDocument_ContentControlBeforeStoreUpdate(ByVal ContentControl As Microsoft.Office.Interop.Word.ContentControl, ByRef Content As System.String) Handles Me.ContentControlBeforeStoreUpdate
        Debug.Print("ThisDocument_ContentControlBeforeStoreUpdate :" + ContentControl.Title)
    End Sub

    Private Sub ThisDocument_ContentControlOnEnter(ByVal ContentControl As Microsoft.Office.Interop.Word.ContentControl) Handles Me.ContentControlOnEnter
        Debug.Print("ThisDocument_ContentControlOnEnter :" + ContentControl.Title)
    End Sub

    Private Sub ThisDocument_ContentControlOnExit(ByVal ContentControl As Microsoft.Office.Interop.Word.ContentControl, ByRef Cancel As System.Boolean) Handles Me.ContentControlOnExit
        Debug.Print("ThisDocument_ContentControlOnExit :" + ContentControl.Title)
    End Sub

End Class
