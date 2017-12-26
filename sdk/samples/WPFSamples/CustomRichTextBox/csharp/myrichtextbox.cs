using System;
using System.Collections;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;

namespace CustomRichTextBoxSample
{
    /// <summary>
    /// An extension of RichTextBox that allows only plain text input.
    /// This class auto-formats words in the document using a dictionary lookup.
    /// <remarks>
    /// One of the applications of such a class can be a code editor.
    /// Syntax highlight for keywords can be implemented using this approach.
    /// </remarks>
    /// </summary>
    public class MyRichTextBox : RichTextBox
    {
        //------------------------------------------------------
        //
        //  Constructors
        //
        //------------------------------------------------------

        // Ctor.
        static MyRichTextBox()
        {
            RegisterCommandHandlers();

            _emailNamesDictionary = new Dictionary<string, string>();
            _emailNamesDictionary.Add("alias1", "Emailname 1");
            _emailNamesDictionary.Add("alias2", "Emailname 2");
            _emailNamesDictionary.Add("alias3", "Emailname 3");
        }

        static void RegisterCommandHandlers()
        {
            // Register command handlers for all rich text formatting commands.
            // We disable all commands by returning false in OnCanExecute event handler,
            // thus making this control a "plain text only" RichTextBox.
            foreach (RoutedUICommand command in _formattingCommands)
            {
                CommandManager.RegisterClassCommandBinding(typeof(MyRichTextBox),
                    new CommandBinding(command, new ExecutedRoutedEventHandler(OnFormattingCommand), 
                    new CanExecuteRoutedEventHandler(OnCanExecuteFormattingCommand)));
            }

            // Command handlers for Cut, Copy and Paste commands.
            // To enforce that data can be copied or pasted from the clipboard in text format only.
            CommandManager.RegisterClassCommandBinding(typeof(MyRichTextBox),
                new CommandBinding(ApplicationCommands.Copy, new ExecutedRoutedEventHandler(OnCopy), 
                new CanExecuteRoutedEventHandler(OnCanExecuteCopy)));
            CommandManager.RegisterClassCommandBinding(typeof(MyRichTextBox),
                new CommandBinding(ApplicationCommands.Paste, new ExecutedRoutedEventHandler(OnPaste), 
                new CanExecuteRoutedEventHandler(OnCanExecutePaste)));
            CommandManager.RegisterClassCommandBinding(typeof(MyRichTextBox),
                new CommandBinding(ApplicationCommands.Cut, new ExecutedRoutedEventHandler(OnCut), 
                new CanExecuteRoutedEventHandler(OnCanExecuteCut)));
        }

        // Ctor.
        public MyRichTextBox() : base()
        {
            this._words = new List<Word>();
            this.TextChanged += this.TextChangedEventHandler;

            MyContextMenu myContextMenu = new MyContextMenu(this);
            myContextMenu.Placement = PlacementMode.RelativePoint;
            myContextMenu.PlacementTarget = this;

            this.ContextMenu = myContextMenu;
        }

        //------------------------------------------------------
        //
        //  Public Properties
        //
        //------------------------------------------------------

        #region Public Properties

        /// <summary>
        /// Dictionary of email names which are auto-formatted by this RichTextBox.
        /// </summary>
        public Dictionary<string, string> EmailNamesDictionary
        {
            get
            {
                return _emailNamesDictionary;
            }
        }

        #endregion

        //------------------------------------------------------
        //
        //  Event Handlers
        //
        //------------------------------------------------------

        #region Event Handlers

        /// <summary>
        /// Event handler for all formatting commands.
        /// </summary>
        private static void OnFormattingCommand(object sender, ExecutedRoutedEventArgs e)
        {
            // Do nothing, and set command handled to true.
            e.Handled = true;
        }

        /// <summary>
        /// Event handler for ApplicationCommands.Copy command.
        /// <remarks>
        /// We want to enforce that data can be set on the clipboard 
        /// only in plain text format from this RichTextBox.
        /// </remarks>
        /// </summary>
        private static void OnCopy(object sender, ExecutedRoutedEventArgs e)
        {
            MyRichTextBox myRichTextBox = (MyRichTextBox)sender;
            string selectionText = myRichTextBox.Selection.Text;
            Clipboard.SetText(selectionText);
            e.Handled = true;
        }

        /// <summary>
        /// Event handler for ApplicationCommands.Cut command.
        /// <remarks>
        /// We want to enforce that data can be set on the clipboard 
        /// only in plain text format from this RichTextBox.
        /// </remarks>
        /// </summary>
        private static void OnCut(object sender, ExecutedRoutedEventArgs e)
        {
            MyRichTextBox myRichTextBox = (MyRichTextBox)sender;
            string selectionText = myRichTextBox.Selection.Text;
            myRichTextBox.Selection.Text = String.Empty;
            Clipboard.SetText(selectionText);
            e.Handled = true;
        }

        /// <summary>
        /// Event handler for ApplicationCommands.Paste command.
        /// <remarks>
        /// We want to allow paste only in plain text format.
        /// </remarks>
        /// </summary>
        private static void OnPaste(object sender, ExecutedRoutedEventArgs e)
        {
            MyRichTextBox myRichTextBox = (MyRichTextBox)sender;
            
            // Handle paste only if clipboard supports text format.
            if (Clipboard.ContainsText())
            {
                myRichTextBox.Selection.Text = Clipboard.GetText();
            }
            e.Handled = true;
        }

        /// <summary>
        /// CanExecute event handler.
        /// </summary>
        private static void OnCanExecuteFormattingCommand(object target, CanExecuteRoutedEventArgs args)
        {
            args.CanExecute = true;
        }

        /// <summary>
        /// CanExecute event handler for ApplicationCommands.Copy.
        /// </summary>
        private static void OnCanExecuteCopy(object target, CanExecuteRoutedEventArgs args)
        {
            MyRichTextBox myRichTextBox = (MyRichTextBox)target;
            args.CanExecute = myRichTextBox.IsEnabled && !myRichTextBox.Selection.IsEmpty;
        }

        /// <summary>
        /// CanExecute event handler for ApplicationCommands.Cut.
        /// </summary>
        private static void OnCanExecuteCut(object target, CanExecuteRoutedEventArgs args)
        {
            MyRichTextBox myRichTextBox = (MyRichTextBox)target;
            args.CanExecute = myRichTextBox.IsEnabled && !myRichTextBox.IsReadOnly && !myRichTextBox.Selection.IsEmpty;
        }

        /// <summary>
        /// CanExecute event handler for ApplicationCommand.Paste.
        /// </summary>
        private static void OnCanExecutePaste(object target, CanExecuteRoutedEventArgs args)
        {
            MyRichTextBox myRichTextBox = (MyRichTextBox)target;
            args.CanExecute = myRichTextBox.IsEnabled && !myRichTextBox.IsReadOnly && Clipboard.ContainsText();
        }

        /// <summary>
        /// Event handler for RichTextBox.TextChanged event.
        /// </summary>
        private void TextChangedEventHandler(object sender, TextChangedEventArgs e)
        {
            // Clear all formatting properties in the document.
            // This is necessary since a paste command could have inserted text inside or at boundaries of a keyword from dictionary.
            TextRange documentRange = new TextRange(this.Document.ContentStart, this.Document.ContentEnd);
            documentRange.ClearAllProperties();

            // Reparse the document to scan for matching words.
            TextPointer navigator = this.Document.ContentStart;
            while (navigator.CompareTo(this.Document.ContentEnd) < 0)
            {
                TextPointerContext context = navigator.GetPointerContext(LogicalDirection.Backward);
                if (context == TextPointerContext.ElementStart && navigator.Parent is Run)
                {
                    this.AddMatchingWordsInRun((Run)navigator.Parent);
                }
                navigator = navigator.GetNextContextPosition(LogicalDirection.Forward);
            }

            // Format words found.
            this.FormatWords();
        }

        #endregion

        //------------------------------------------------------
        //
        //  Private Methods
        //
        //------------------------------------------------------

        #region Private Methods

        /// <summary>
        /// Helper to apply formatting properties to matching words in the document.
        /// </summary>
        private void FormatWords()
        {
            // Applying formatting properties, triggers another TextChangedEvent. Remove event handler temporarily.
            this.TextChanged -= this.TextChangedEventHandler;

            // Add formatting for matching words.
            foreach (Word word in _words)
            {
                TextRange range = new TextRange(word.Start, word.End);
                range.ApplyPropertyValue(TextElement.ForegroundProperty, new SolidColorBrush(Colors.Blue));
                range.ApplyPropertyValue(TextElement.FontWeightProperty, FontWeights.Bold);
            }
            _words.Clear();

            // Add TextChanged handler back.
            this.TextChanged += this.TextChangedEventHandler;
        }

        /// <summary>
        /// Scans passed Run's text, for any matching words from dictionary.
        /// </summary>
        private void AddMatchingWordsInRun(Run run)
        {
            string runText = run.Text;

            int wordStartIndex = 0;
            int wordEndIndex = 0;
            for (int i = 0; i < runText.Length; i++)
            {
                if (Char.IsWhiteSpace(runText[i]))
                {
                    if (i > 0 && !Char.IsWhiteSpace(runText[i - 1]))
                    {
                        wordEndIndex = i - 1;
                        string wordInRun = runText.Substring(wordStartIndex, wordEndIndex - wordStartIndex + 1);

                        if (_emailNamesDictionary.ContainsKey(wordInRun))
                        {
                            TextPointer wordStart = run.ContentStart.GetPositionAtOffset(wordStartIndex, LogicalDirection.Forward);
                            TextPointer wordEnd = run.ContentStart.GetPositionAtOffset(wordEndIndex + 1, LogicalDirection.Backward);
                            _words.Add(new Word(wordStart, wordEnd));
                        }
                    }
                    wordStartIndex = i + 1;
                }
            }
            
            // Check if the last word in the Run is a matching word.
            string lastWordInRun = runText.Substring(wordStartIndex, runText.Length - wordStartIndex);
            if (_emailNamesDictionary.ContainsKey(lastWordInRun))
            {
                TextPointer wordStart = run.ContentStart.GetPositionAtOffset(wordStartIndex, LogicalDirection.Forward);
                TextPointer wordEnd = run.ContentStart.GetPositionAtOffset(runText.Length, LogicalDirection.Backward);
                _words.Add(new Word(wordStart, wordEnd));
            }
        }

        #endregion

        //------------------------------------------------------
        //
        //  Private Types
        //
        //------------------------------------------------------

        #region Private Types

        /// <summary>
        /// This class encapsulates a matching word by two TextPointer positions, 
        /// start and end, with forward and backward gravities respectively.
        /// </summary>
        private class Word
        {
            public Word(TextPointer wordStart, TextPointer wordEnd)
            {
                _wordStart = wordStart.GetPositionAtOffset(0, LogicalDirection.Forward);
                _wordEnd = wordEnd.GetPositionAtOffset(0, LogicalDirection.Backward);
            }

            public TextPointer Start
            {
                get
                {
                    return _wordStart;
                }
            }

            public TextPointer End
            {
                get
                {
                    return _wordEnd;
                }
            }

            private readonly TextPointer _wordStart;
            private readonly TextPointer _wordEnd;
        }

        #endregion

        //------------------------------------------------------
        //
        //  Private Members
        //
        //------------------------------------------------------

        #region Private Members

        // Static member for email names dictionary.
        private static readonly Dictionary<string, string> _emailNamesDictionary;

        // Static list of editing formatting commands. In the ctor we disable all these commands.
        private static readonly RoutedUICommand[] _formattingCommands = new RoutedUICommand[]
            {
                EditingCommands.ToggleBold,
                EditingCommands.ToggleItalic,
                EditingCommands.ToggleUnderline,
                EditingCommands.ToggleSubscript,
                EditingCommands.ToggleSuperscript,
                EditingCommands.IncreaseFontSize,
                EditingCommands.DecreaseFontSize,
                EditingCommands.ToggleBullets,
                EditingCommands.ToggleNumbering,
            };

        // List of matching words found in the document.
        private List<Word> _words;

        #endregion Private Members
    }
}