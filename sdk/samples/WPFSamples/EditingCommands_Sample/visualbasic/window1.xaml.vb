Imports System     
Imports System.Windows     
Imports System.Windows.Controls
Imports System.Windows.Documents

Namespace SDKSample

    Partial Public Class Window1
        Inherits Window

        Public Sub ExecuteCommand(ByVal sender As Object, ByVal args As RoutedEventArgs)
            Dim command As String = CType(sender, Button).Name

            Dim target As IInputElement
            If routeToRTB.IsChecked.Value = True Then
                target = sampleRTB
            ElseIf routeToTB.IsChecked.Value = True Then
                target = sampleTB
            Else
                target = samplePWB
            End If

            Select Case command

                Case "AlignCenter"
                    EditingCommands.AlignCenter.Execute(Nothing, target)
                Case "AlignJustify"
                    EditingCommands.AlignJustify.Execute(Nothing, target)
                Case "AlignRight"
                    EditingCommands.AlignRight.Execute(Nothing, target)
                Case "AlignLeft"
                    EditingCommands.AlignLeft.Execute(Nothing, target)
                    'Caret Movement by Line
                Case "MoveUpByLine"
                    EditingCommands.MoveUpByLine.Execute(Nothing, target)
                Case "MoveDownByLine"
                    EditingCommands.MoveDownByLine.Execute(Nothing, target)
                Case "MoveToLineEnd"
                    EditingCommands.MoveToLineEnd.Execute(Nothing, target)
                Case "MoveToLineStart"
                    EditingCommands.MoveToLineStart.Execute(Nothing, target)
                    'Caret Movement by Word
                Case "MoveLeftByWord"
                    EditingCommands.MoveLeftByWord.Execute(Nothing, target)
                Case "MoveRightByWord"
                    EditingCommands.MoveRightByWord.Execute(Nothing, target)
                    'Caret Movement by Paragraph
                Case "MoveUpByParagraph"
                    EditingCommands.MoveUpByParagraph.Execute(Nothing, target)
                Case "MoveDownByParagraph"
                    EditingCommands.MoveDownByParagraph.Execute(Nothing, target)
                    'Caret Movement by Page
                Case "MoveUpByPage"
                    EditingCommands.MoveUpByPage.Execute(Nothing, target)
                Case "MoveDownByPage"
                    EditingCommands.MoveDownByPage.Execute(Nothing, target)
                    'Caret Movement by Document
                Case "MoveToDocumentEnd"
                    EditingCommands.MoveToDocumentEnd.Execute(Nothing, target)
                Case "MoveToDocumentStart"
                    EditingCommands.MoveToDocumentStart.Execute(Nothing, target)
                    'Deletion
                Case "Delete"
                    EditingCommands.Delete.Execute(Nothing, target)
                Case "DeleteNextWord"
                    EditingCommands.DeleteNextWord.Execute(Nothing, target)
                Case "DeletePreviousWord"
                    EditingCommands.DeletePreviousWord.Execute(Nothing, target)
                    'Spelling Errors
                Case "CorrectSpellingError"
                    EditingCommands.CorrectSpellingError.Execute(Nothing, target)
                Case "IgnoreSpellingError"
                    EditingCommands.IgnoreSpellingError.Execute(Nothing, target)
                    'Toggle Insert
                Case "ToggleInsert"
                    EditingCommands.ToggleInsert.Execute(Nothing, target)
                    'Symbol Entry
                Case "Backspace"
                    EditingCommands.Backspace.Execute(Nothing, target)
                Case "EnterLineBreak"
                    EditingCommands.EnterLineBreak.Execute(Nothing, target)
                Case "EnterParagraphBreak"
                    EditingCommands.EnterParagraphBreak.Execute(Nothing, target)
                Case "TabBackward"
                    EditingCommands.TabBackward.Execute(Nothing, target)
                Case "TabForward"
                    EditingCommands.TabForward.Execute(Nothing, target)
                    'Paragraph Formatting
                Case "IncreaseIndentation"
                    EditingCommands.IncreaseIndentation.Execute(Nothing, target)
                Case "DecreaseIndentation"
                    EditingCommands.DecreaseIndentation.Execute(Nothing, target)
                Case "ToggleBullets"
                    EditingCommands.ToggleBullets.Execute(Nothing, target)
                Case "ToggleNumbering"
                    EditingCommands.ToggleNumbering.Execute(Nothing, target)
                    'Formatting
                Case "IncreaseFontSize"
                    EditingCommands.IncreaseFontSize.Execute(Nothing, target)
                Case "DecreaseFontSize"
                    EditingCommands.DecreaseFontSize.Execute(Nothing, target)
                Case "ToggleBold"
                    EditingCommands.ToggleBold.Execute(Nothing, target)
                Case "ToggleItalic"
                    EditingCommands.ToggleItalic.Execute(Nothing, target)
                Case "ToggleSubscript"
                    EditingCommands.ToggleSubscript.Execute(Nothing, target)
                Case "ToggleSuperscript"
                    EditingCommands.ToggleSuperscript.Execute(Nothing, target)
                Case "ToggleUnderline"
                    EditingCommands.ToggleUnderline.Execute(Nothing, target)
                    'Selection by Line
                Case "SelectUpByLine"
                    EditingCommands.SelectUpByLine.Execute(Nothing, target)
                Case "SelectDownByLine"
                    EditingCommands.SelectDownByLine.Execute(Nothing, target)
                Case "SelectToLineEnd"
                    EditingCommands.SelectToLineEnd.Execute(Nothing, target)
                Case "SelectToLineStart"
                    EditingCommands.SelectToLineStart.Execute(Nothing, target)
                    'Selection by Character
                Case "SelectLeftByCharacter"
                    EditingCommands.SelectLeftByCharacter.Execute(Nothing, target)
                Case "SelectRightByCharacter"
                    EditingCommands.SelectRightByCharacter.Execute(Nothing, target)
                    'Caret Selection by Word
                Case "SelectLeftByWord"
                    EditingCommands.SelectLeftByWord.Execute(Nothing, target)
                Case "SelectRightByWord"
                    EditingCommands.SelectRightByWord.Execute(Nothing, target)
                    'Selection by Paragraph
                Case "SelectUpByParagraph"
                    EditingCommands.SelectUpByParagraph.Execute(Nothing, target)
                Case "SelectDownByParagraph"
                    EditingCommands.SelectDownByParagraph.Execute(Nothing, target)
                    'Selection by Page
                Case "SelectUpByPage"
                    EditingCommands.SelectUpByPage.Execute(Nothing, target)
                Case "SelectDownByPage"
                    EditingCommands.SelectDownByPage.Execute(Nothing, target)
                    'Selection by Document
                Case "SelectToDocumentEnd"
                    EditingCommands.SelectToDocumentEnd.Execute(Nothing, target)
                Case "SelectToDocumentStart"
                    EditingCommands.SelectToDocumentStart.Execute(Nothing, target)

            End Select
            target.Focus()
        End Sub

    End Class
End Namespace
