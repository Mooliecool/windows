'/************************************* Module Header **************************************\
'* Module Name:	PaneControl.vb
'* Project:		VBWordDocument
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

#Region "Import directives"

Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Drawing
Imports System.Data
Imports System.Linq
Imports System.Text
Imports System.Windows.Forms
Imports Microsoft.VisualStudio.Tools.Applications.Runtime
Imports Office = Microsoft.Office.Core
Imports Word = Microsoft.Office.Interop.Word
Imports Microsoft.Win32

#End Region


Public Class PaneControl

    Dim RangeStart As Object = 0
    Dim RangeEnd As Object = 0
    Dim selection As Object = Globals.ThisDocument.ThisApplication.Selection.Range

#Region "Main Functions"

    Sub LoadDefaultBuildingBlock()

        Dim Categorie As Object = "Built-In"
        Dim BuildingBlockName As Object = "Double Table "
        Dim oTrue As Object = True

        'The path of the Building Block.dotx may depends on your opreation system,
        'We could use HKLM hive to locate the Word.
        Dim WordPath As Object
        WordPath = Registry.GetValue("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Office\12.0\Word\InstallRoot", "Path", "")
        Globals.ThisDocument.AttachedTemplate = WordPath & "Document Parts\1033\Building Blocks.dotx"
        Dim template As Word.Template = Globals.ThisDocument.AttachedTemplate
        'For example we indert the "Double Table" into our document, we located the specific Building Block via WdBuildingBlockTypes,Categorie and BuildingBlockName.
        template.BuildingBlockTypes.Item(Microsoft.Office.Interop.Word.WdBuildingBlockTypes.wdTypeTables).Categories.Item(Categorie).BuildingBlocks.Item(BuildingBlockName).Insert(Globals.ThisDocument.Application.Selection.Range, oTrue)
    End Sub

    Function AddContentControl(ByVal ct As Word.WdContentControlType, ByRef selection As Object) As Word.ContentControl

        Dim units As Object = Word.WdUnits.wdCharacter
        Dim count As Object = 1
        Dim missing As Object = Type.Missing

        Dim ContentControl As Word.ContentControl = Globals.ThisDocument.ContentControls.Add(ct, selection)
        ContentControl.Range.Select()
        'by default after we inserted the content control the cursion is within the scope of that control,therefore,use Word OM to move the cursor out of the 
        'new control, and ready to insert the next one.
        Globals.ThisDocument.ThisApplication.Selection.MoveRight(units, count, missing)
        Globals.ThisDocument.ThisApplication.Selection.TypeParagraph()
        selection = Globals.ThisDocument.ThisApplication.Selection.Range
        Return ContentControl

    End Function

#End Region

#Region "Event Handlers"

    Private Sub btnInsertCC_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnInsertCC.Click
        Globals.ThisDocument.Range(RangeStart, RangeEnd).Select()
        AddContentControl(Microsoft.Office.Interop.Word.WdContentControlType.wdContentControlBuildingBlockGallery, selection)

        AddContentControl(Microsoft.Office.Interop.Word.WdContentControlType.wdContentControlComboBox, selection)

        AddContentControl(Microsoft.Office.Interop.Word.WdContentControlType.wdContentControlDate, selection)

        AddContentControl(Microsoft.Office.Interop.Word.WdContentControlType.wdContentControlDropdownList, selection)

        AddContentControl(Microsoft.Office.Interop.Word.WdContentControlType.wdContentControlPicture, selection)

        AddContentControl(Microsoft.Office.Interop.Word.WdContentControlType.wdContentControlRichText, selection)
    End Sub

    Private Sub btnInsertBuiltIn_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnInsertBuiltIn.Click
        Globals.ThisDocument.Range(RangeStart, RangeEnd).Select()
        LoadDefaultBuildingBlock()
    End Sub

    Private Sub btnCCXMLMapping_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCCXMLMapping.Click
        Globals.ThisDocument.Range(RangeStart, RangeEnd).Select()
        'mapping to the value of creator of this document.
        AddContentControl(Microsoft.Office.Interop.Word.WdContentControlType.wdContentControlText, selection).XMLMapping.SetMapping("/ns1:coreProperties[1]/ns0:creator[1]", "", _
         Globals.ThisDocument.CustomXMLParts(1))
    End Sub

#End Region

End Class
