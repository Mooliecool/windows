using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace SDKSample
{
    public partial class Window1 : Window
    {

        public Window1()
        {
            InitializeComponent();
        }

        void ExecuteCommand(Object sender, RoutedEventArgs args)
        {
            string command = ((Button)sender).Name;

            IInputElement target = routeToRTB.IsChecked.Value ? sampleRTB as IInputElement :
                (routeToTB.IsChecked.Value ? sampleTB as IInputElement : samplePWB as IInputElement);

            switch (command)
            {
                // Paragraph Alignment
                case "AlignCenter":
                    EditingCommands.AlignCenter.Execute(null, target);
                    break;
                case "AlignJustify":
                    EditingCommands.AlignJustify.Execute(null, target);
                    break;
                case "AlignRight":
                    EditingCommands.AlignRight.Execute(null, target);
                    break;
                case "AlignLeft":
                    EditingCommands.AlignLeft.Execute(null, target);
                    break;

                //Caret Movement by Line
                case "MoveUpByLine":
                    EditingCommands.MoveUpByLine.Execute(null, target);
                    break;
                case "MoveDownByLine":
                    EditingCommands.MoveDownByLine.Execute(null, target);
                    break;
                case "MoveToLineEnd":
                    EditingCommands.MoveToLineEnd.Execute(null, target);
                    break;
                case "MoveToLineStart":
                    EditingCommands.MoveToLineStart.Execute(null, target);
                    break;

                //Caret Movement by Character
                case "MoveLeftByCharacter":
                    EditingCommands.MoveLeftByCharacter.Execute(null, target);
                    break;
                case "MoveRightByCharacter":
                    EditingCommands.MoveRightByCharacter.Execute(null, target);
                    break;

                //Caret Movement by Word
                case "MoveLeftByWord":
                    EditingCommands.MoveLeftByWord.Execute(null, target);
                    break;
                case "MoveRightByWord":
                    EditingCommands.MoveRightByWord.Execute(null, target);
                    break;

                //Caret Movement by Paragraph
                case "MoveUpByParagraph":
                    EditingCommands.MoveUpByParagraph.Execute(null, target);
                    break;
                case "MoveDownByParagraph":
                    EditingCommands.MoveDownByParagraph.Execute(null, target);
                    break;

                //Caret Movement by Page
                case "MoveUpByPage":
                    EditingCommands.MoveUpByPage.Execute(null, target);
                    break;
                case "MoveDownByPage":
                    EditingCommands.MoveDownByPage.Execute(null, target);
                    break;

                //Caret Movement by Document
                case "MoveToDocumentEnd":
                    EditingCommands.MoveToDocumentEnd.Execute(null, target);
                    break;
                case "MoveToDocumentStart":
                    EditingCommands.MoveToDocumentStart.Execute(null, target);
                    break;

                //Deletion
                case "Delete":
                    EditingCommands.Delete.Execute(null, target);
                    break;
                case "DeleteNextWord":
                    EditingCommands.DeleteNextWord.Execute(null, target);
                    break;
                case "DeletePreviousWord":
                    EditingCommands.DeletePreviousWord.Execute(null, target);
                    break;

                //Spelling Errors
                case "CorrectSpellingError":
                    EditingCommands.CorrectSpellingError.Execute(null, target);
                    break;
                case "IgnoreSpellingError":
                    EditingCommands.IgnoreSpellingError.Execute(null, target);
                    break;

                //Toggle Insert
                case "ToggleInsert":
                    EditingCommands.ToggleInsert.Execute(null, target);
                    break;

                //Symbol Entry
                case "Backspace":
                    EditingCommands.Backspace.Execute(null, target);
                    break;
                case "EnterLineBreak":
                    EditingCommands.EnterLineBreak.Execute(null, target);
                    break;
                case "EnterParagraphBreak":
                    EditingCommands.EnterParagraphBreak.Execute(null, target);
                    break;
                case "TabBackward":
                    EditingCommands.TabBackward.Execute(null, target);
                    break;
                case "TabForward":
                    EditingCommands.TabForward.Execute(null, target);
                    break;

                //Paragraph Formatting
                case "IncreaseIndentation":
                    EditingCommands.IncreaseIndentation.Execute(null, target);
                    break;
                case "DecreaseIndentation":
                    EditingCommands.DecreaseIndentation.Execute(null, target);
                    break;
                case "ToggleBullets":
                    EditingCommands.ToggleBullets.Execute(null, target);
                    break;
                case "ToggleNumbering":
                    EditingCommands.ToggleNumbering.Execute(null, target);
                    break;

                //Formatting
                case "IncreaseFontSize":
                    EditingCommands.IncreaseFontSize.Execute(null, target);
                    break;
                case "DecreaseFontSize":
                    EditingCommands.DecreaseFontSize.Execute(null, target);
                    break;
                case "ToggleBold":
                    EditingCommands.ToggleBold.Execute(null, target);
                    break;
                case "ToggleItalic":
                    EditingCommands.ToggleItalic.Execute(null, target);
                    break;
                case "ToggleSubscript":
                    EditingCommands.ToggleSubscript.Execute(null, target);
                    break;
                case "ToggleSuperscript":
                    EditingCommands.ToggleSuperscript.Execute(null, target);
                    break;
                case "ToggleUnderline":
                    EditingCommands.ToggleUnderline.Execute(null, target);
                    break;


                //Selection by Line
                case "SelectUpByLine":
                    EditingCommands.SelectUpByLine.Execute(null, target);
                    break;
                case "SelectDownByLine":
                    EditingCommands.SelectDownByLine.Execute(null, target);
                    break;
                case "SelectToLineEnd":
                    EditingCommands.SelectToLineEnd.Execute(null, target);
                    break;
                case "SelectToLineStart":
                    EditingCommands.SelectToLineStart.Execute(null, target);
                    break;

                //Selection by Character
                case "SelectLeftByCharacter":
                    EditingCommands.SelectLeftByCharacter.Execute(null, target);
                    break;
                case "SelectRightByCharacter":
                    EditingCommands.SelectRightByCharacter.Execute(null, target);
                    break;

                //Caret Selection by Word
                case "SelectLeftByWord":
                    EditingCommands.SelectLeftByWord.Execute(null, target);
                    break;
                case "SelectRightByWord":
                    EditingCommands.SelectRightByWord.Execute(null, target);
                    break;

                //Selection by Paragraph
                case "SelectUpByParagraph":
                    EditingCommands.SelectUpByParagraph.Execute(null, target);
                    break;
                case "SelectDownByParagraph":
                    EditingCommands.SelectDownByParagraph.Execute(null, target);
                    break;

                //Selection by Page
                case "SelectUpByPage":
                    EditingCommands.SelectUpByPage.Execute(null, target);
                    break;
                case "SelectDownByPage":
                    EditingCommands.SelectDownByPage.Execute(null, target);
                    break;

                //Selection by Document
                case "SelectToDocumentEnd":
                    EditingCommands.SelectToDocumentEnd.Execute(null, target);
                    break;
                case "SelectToDocumentStart":
                    EditingCommands.SelectToDocumentStart.Execute(null, target);
                    break;


                default:
                    break;
            }
            target.Focus();
        }


    }  // End class Window1

}  // End namespace EditingCommands_Sample