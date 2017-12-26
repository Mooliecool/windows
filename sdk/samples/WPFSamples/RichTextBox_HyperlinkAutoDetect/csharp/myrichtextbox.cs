using System;
using System.Collections;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;

namespace HyperlinkAutoDetect
{
    /// <summary>
    /// Custom RichTextBox class that performs hyperlink auto-detection.
    /// </summary>
    public class MyRichTextBox : RichTextBox
    {
        // Ctor.
        static MyRichTextBox()
        {
            // KeyDown event.
            EventManager.RegisterClassHandler(typeof(MyRichTextBox), KeyDownEvent, new KeyEventHandler(OnKeyDown), /*handledEventsToo*/true);
        }

        /// <summary>
        /// Event handler for KeyDown event to auto-detect hyperlinks on space, enter and backspace keys. 
        /// </summary>
        private static void OnKeyDown(object sender, KeyEventArgs e)
        {
            MyRichTextBox myRichTextBox = (MyRichTextBox)sender;

            if (e.Key != Key.Space && e.Key != Key.Back && e.Key != Key.Return)
            {
                return;
            }

            if (!myRichTextBox.Selection.IsEmpty)
            {
                myRichTextBox.Selection.Text = String.Empty;
            }

            TextPointer caretPosition = myRichTextBox.Selection.Start;

            if (e.Key == Key.Space || e.Key == Key.Return)
            {
                TextPointer wordStartPosition;
                string word = GetPreceedingWordInParagraph(caretPosition, out wordStartPosition);

                if (word == "www.microsoft.com") // A real app would need a more sophisticated RegEx match expression for hyperlinks.
                {
                    // Insert hyperlink element at word boundaries.
                    new Hyperlink(
                        wordStartPosition.GetPositionAtOffset(0, LogicalDirection.Backward),
                        caretPosition.GetPositionAtOffset(0, LogicalDirection.Forward));

                    // No need to update RichTextBox caret position, 
                    // since we only inserted a Hyperlink ElementEnd following current caretPosition.
                    // Subsequent handling of space input by base RichTextBox will update selection.
                }
            }
            else // Key.Back
            {
                TextPointer backspacePosition = caretPosition.GetNextInsertionPosition(LogicalDirection.Backward);
                Hyperlink hyperlink;
                if (backspacePosition != null && IsHyperlinkBoundaryCrossed(caretPosition, backspacePosition, out hyperlink))
                {
                    // Remember caretPosition with forward gravity. This is necessary since we are going to delete 
                    // the hyperlink element preceeding caretPosition and after deletion current caretPosition 
                    // (with backward gravity) will follow content preceeding the hyperlink. 
                    // We want to remember content following the hyperlink to set new caret position at.

                    TextPointer newCaretPosition = caretPosition.GetPositionAtOffset(0, LogicalDirection.Forward);

                    // Deleting the hyperlink is done using logic below.

                    // 1. Copy its children Inline to a temporary array.
                    InlineCollection hyperlinkChildren = hyperlink.Inlines;
                    Inline[] inlines = new Inline[hyperlinkChildren.Count];
                    hyperlinkChildren.CopyTo(inlines, 0);

                    // 2. Remove each child from parent hyperlink element and insert it after the hyperlink.
                    for (int i = inlines.Length - 1; i >= 0; i--)
                    {
                        hyperlinkChildren.Remove(inlines[i]);
                        hyperlink.SiblingInlines.InsertAfter(hyperlink, inlines[i]);
                    }

                    // 3. Apply hyperlink's local formatting properties to inlines (which are now outside hyperlink scope).
                    LocalValueEnumerator localProperties = hyperlink.GetLocalValueEnumerator();
                    TextRange inlineRange = new TextRange(inlines[0].ContentStart, inlines[inlines.Length - 1].ContentEnd);

                    while (localProperties.MoveNext())
                    {
                        LocalValueEntry property = localProperties.Current;
                        DependencyProperty dp = property.Property;
                        object value = property.Value;

                        if (!dp.ReadOnly &&
                            dp != Inline.TextDecorationsProperty && // Ignore hyperlink defaults.
                            dp != TextElement.ForegroundProperty &&
                            !IsHyperlinkProperty(dp))
                        {
                            inlineRange.ApplyPropertyValue(dp, value);
                        }
                    }

                    // 4. Delete the (empty) hyperlink element.
                    hyperlink.SiblingInlines.Remove(hyperlink);

                    // 5. Update selection, since we deleted Hyperlink element and caretPosition was at that Hyperlink's end boundary.
                    myRichTextBox.Selection.Select(newCaretPosition, newCaretPosition);
                }
            }
        }

        // Helper that returns true when passed property applies to Hyperlink only.
        private static bool IsHyperlinkProperty(DependencyProperty dp)
        {
            return dp == Hyperlink.CommandProperty ||
                dp == Hyperlink.CommandParameterProperty ||
                dp == Hyperlink.CommandTargetProperty ||
                dp == Hyperlink.NavigateUriProperty ||
                dp == Hyperlink.TargetNameProperty;
        }

        // Helper that returns true if passed caretPosition and backspacePosition cross a hyperlink end boundary
        // (under the assumption that caretPosition and backSpacePosition are adjacent insertion positions).
        private static bool IsHyperlinkBoundaryCrossed(TextPointer caretPosition, TextPointer backspacePosition, out Hyperlink backspacePositionHyperlink)
        {
            Hyperlink caretPositionHyperlink = GetHyperlinkAncestor(caretPosition);
            backspacePositionHyperlink = GetHyperlinkAncestor(backspacePosition);

            return (caretPositionHyperlink == null && backspacePositionHyperlink != null) ||
                (caretPositionHyperlink != null && backspacePositionHyperlink != null && caretPositionHyperlink != backspacePositionHyperlink);
        }

        // Helper that returns a hyperlink ancestor of passed position.
        private static Hyperlink GetHyperlinkAncestor(TextPointer position)
        {
            Inline parent = position.Parent as Inline;
            while (parent != null && !(parent is Hyperlink))
            {
                parent = parent.Parent as Inline;
            }

            return parent as Hyperlink;
        }

        // Helper that returns a word preceeding the passed position in its paragraph, 
        // wordStartPosition points to the start position of word.
        private static string GetPreceedingWordInParagraph(TextPointer position, out TextPointer wordStartPosition)
        {
            wordStartPosition = null;
            string word = String.Empty;

            Paragraph paragraph = position.Paragraph;
            if (paragraph != null)
            {
                TextPointer navigator = position;
                while (navigator.CompareTo(paragraph.ContentStart) > 0)
                {
                    string runText = navigator.GetTextInRun(LogicalDirection.Backward);

                    if (runText.Contains(" ")) // Any globalized application would need more sophisticated word break testing.
                    {
                        int index = runText.LastIndexOf(" ");
                        word = runText.Substring(index + 1, runText.Length - index - 1) + word;
                        wordStartPosition = navigator.GetPositionAtOffset(-1 * (runText.Length - index - 1));
                        break;
                    }
                    else
                    {
                        wordStartPosition = navigator;
                        word = runText + word;
                    }
                    navigator = navigator.GetNextContextPosition(LogicalDirection.Backward);
                }
            }

            return word;
        }
    }
}