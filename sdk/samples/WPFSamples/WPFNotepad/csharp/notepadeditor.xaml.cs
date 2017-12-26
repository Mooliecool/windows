#define DEBUG
/*******************************************************************************
 *
 * Copyright (c) 2004-2005 Microsoft Corporation.  All rights reserved.
 *
 * Description: The NotepadEditor class defines main client area of the application
 * and the logic of editing comamands.
 *
 *******************************************************************************/

using System;
using Microsoft.Win32;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Threading;
using System.Windows.Media;
using System.Windows.Documents;
using System.Windows.Documents.Serialization;
using System.Diagnostics;
using System.IO;
using System.Xml;
using System.Text;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Resources;
using System.Printing;
using System.Windows.Xps;
using System.IO.Packaging;
using System.Drawing.Printing;
using PageSetupDialog = System.Windows.Forms.PageSetupDialog;
using PageSetupDialogResult = System.Windows.Forms.DialogResult;

[assembly: AssemblyTitle("WPF Notepad")]

namespace Microsoft.Samples.WPFNotepad
{
    public partial class NotepadEditor : Window
    {
        #region Public extern methods

        //------------------------------------------------------
        //
        //  Public extern methods
        //
        //------------------------------------------------------

        [DllImport("shell32.dll")]
        // using the ShellAbout dialog
        public static extern int ShellAbout(IntPtr hWnd, string szApp, string szOtherStuff, IntPtr hIcon);
        [DllImport("user32.dll")]
        // for loading the WPF Notepad icon
        public static extern IntPtr LoadImage(int Hinstance, string name, int type, int width, int height, int load);

        #endregion Public extern methods


        #region public Constructors

        //------------------------------------------------------
        //
        //  public Constructors
        //
        //------------------------------------------------------

        public NotepadEditor()
        {
            InitializeComponent();
            // Created the Header/Footer panel at the top of the window
            SetTextMode(_thisApp.CurrentEditingMode, true);
            SetMenuItemsPanelToWrapPanel();

            _pageOrientation = null;
            _pageSize = null;
        }

        #endregion public Constructors


        #region Public fields

        //------------------------------------------------------
        //
        //  Public fields
        //
        //------------------------------------------------------

        public static RoutedCommand FindCommand = new RoutedCommand("Find", typeof(NotepadEditor), null);
        public static RoutedCommand FindNextCommand = new RoutedCommand("FindNext", typeof(NotepadEditor), null);
        public static RoutedCommand ReplaceCommand = new RoutedCommand("Replace", typeof(NotepadEditor), null);
        public static RoutedCommand GotoCommand = new RoutedCommand("Goto", typeof(NotepadEditor), null);
        public static RoutedCommand DateTimeCommand = new RoutedCommand("DateTime", typeof(NotepadEditor), null);
        public static RoutedCommand EscapeToCCommand = new RoutedCommand("EscapeToC", typeof(NotepadEditor), null);
        public static RoutedCommand EscapeToXmlCommand = new RoutedCommand("EscapeToXml", typeof(NotepadEditor), null);
        public static RoutedCommand PageSetupCommand = new RoutedCommand("PageSetup", typeof(NotepadEditor), null);
        public static RoutedCommand PrintAsyncCommand = new RoutedCommand("PrintAsync", typeof(NotepadEditor), null);
        public static RoutedCommand ExitCommand = new RoutedCommand("Exit", typeof(NotepadEditor), null);
        public static RoutedCommand WordWrapCommand = new RoutedCommand("WordWrap", typeof(NotepadEditor), null);
        public static RoutedCommand FontCommand = new RoutedCommand("Font", typeof(NotepadEditor), null);
        public static RoutedCommand ShowStatusBarCommand = new RoutedCommand("ShowStatusBar", typeof(NotepadEditor), null);
        public static RoutedCommand ShowHeaderFooterCommand = new RoutedCommand("ShowHeaderFooter", typeof(NotepadEditor), null);
        public static RoutedCommand AboutCommand = new RoutedCommand("About", typeof(NotepadEditor), null);
        public static RoutedCommand ShowBugsCommand = new RoutedCommand("ShowBugs", typeof(NotepadEditor), null);
        public static RoutedCommand FileAssociationsCommand = new RoutedCommand("FileAssociations", typeof(NotepadEditor), null);
        public static RoutedCommand HelpTopicsCommand = new RoutedCommand("HelpTopics", typeof(NotepadEditor), null);
        public static RoutedCommand ReportAnIssueCommand = new RoutedCommand("ReportAnIssue", typeof(NotepadEditor), null);
        public static RoutedCommand SpellCheckCommand = new RoutedCommand("SpellCheck", typeof(NotepadEditor), null);
        public static RoutedCommand RichTextModeCommand = new RoutedCommand("RichTextMode", typeof(NotepadEditor), null);
        public static RoutedCommand RightToLeftReadCommand = new RoutedCommand("RightToLeftRead", typeof(NotepadEditor), null);
        public static RoutedCommand RichRightToLeftReadCommand = new RoutedCommand("RichRightToLeftRead", typeof(NotepadEditor), null);

        #endregion Public fields

        #region internal Properties

        //------------------------------------------------------
        //
        //  internal Properties
        //
        //------------------------------------------------------

        internal NotepadDocument Document
        {
            get
            {
                return _doc;
            }
            set
            {
                _doc = value;
                int startIndex = Document.FileName.LastIndexOf('.') + 1;
                string extension = _doc.FileName.Substring(startIndex);
                if (extension == "xaml")
                {
                    _richTextMenu.IsChecked = true;
                    SetTextMode(EditingMode.Rich, false);
                }
                else
                {
                    _richTextMenu.IsChecked = false;
                    SetTextMode(EditingMode.Plain, false);
                }

                if (_thisApp.CurrentEditingMode == EditingMode.Rich)
                {
                    TextRange tr = new TextRange(_richTextBox.Document.ContentStart, _richTextBox.Document.ContentEnd);
                    if (_doc.Content != null)
                    {
                        try
                        {
                            TextRange_SetXml(tr, _doc.Content);                            
                            
                            if (tr.IsEmpty)
                            {
                                //TextRange load has silently failed. 
                                //Open the xaml as plain text contents in plain text mode.
                                _richTextMenu.IsChecked = false;
                                SetTextMode(EditingMode.Plain, false);
                                _plainTextBox.Text = _doc.Content;
                            }
                        }
                        catch
                        {
                            //Not able to load the xaml contents. 
                            //Open the xaml contents in plain text mode 
                            _richTextMenu.IsChecked = false;
                            SetTextMode(EditingMode.Plain, false);
                            _plainTextBox.Text = _doc.Content;
                        }
                    }
                    else
                    {
                        tr.Text = "";
                    }
                }
                else
                {
                    _plainTextBox.Text = _doc.Content;
                }

                Document.IsDirty = false;
                _mainWindow.Title = _doc.DisplayName;
                _currentTextBox.Focus();
            }
        }


        /// <summary>
        /// Gets the current FlowDirection setting from the Menu.
        /// </summary>
        FlowDirection FlowDirectionSettings
        {
            get
            {
                MenuItem _RTLMenuItem = _currentTextBox.ContextMenu.Items[9] as MenuItem;
                return _RTLMenuItem.IsChecked ? FlowDirection.RightToLeft : FlowDirection.LeftToRight;
            }
        }

        #endregion internal Properties



        #region private Properties


        //------------------------------------------------------
        //
        //  private Properties
        //
        //------------------------------------------------------

        int CaretLineNumber
        {
            get
            {
                int caretPos = SelectionStart;

                if (_thisApp.CurrentEditingMode == EditingMode.Plain)
                {
                    try
                    {
                        return (_plainTextBox.GetLineIndexFromCharacterIndex(caretPos) + 1);
                    }

                    catch (ArgumentOutOfRangeException aoore)
                    {
                        System.Diagnostics.Debug.Print("ArgumentOutOfRangeException thrown. Windows OS Bug 1554576");
                        return 1;
                    }
                }
                else
                {
                    // TODO: Rich Text box status bar
                    //string content = TextContent.Substring(0, caretPos);
                    //int count = 0;
                    //for (int i = 0; i < content.Length; i++)
                    //{
                    //    if (content[i] == '\n')
                    //        count++;
                    //}
                    //return count + 1;
                    return 0;
                }
            }
            set
            {
                if (_thisApp.CurrentEditingMode == EditingMode.Plain)
                {
                    // publically we are 1 based, so adjust...
                    value = value - 1;
                    int count = 0;
                    int pos = 0;
                    while (count < value)
                    {
                        pos = TextContent.IndexOf("\r\n", pos);
                        if (pos != -1)
                        {
                            count++;
                            pos += 2;
                        }
                        else
                        {
                            pos = TextContent.Length;
                            break;
                        }
                    }
                    SelectionStart = pos;
                    SelectionLength = 0;
                }
            }
        }

        int SelectionLength
        {
            get
            {
                if (_thisApp.CurrentEditingMode == EditingMode.Rich)
                {
                    return _richTextBox.Selection.Text.Length;
                }
                else
                {
                    return _plainTextBox.SelectionLength;
                }
            }
            set
            {
                if (_thisApp.CurrentEditingMode == EditingMode.Rich)
                {
                    TextPointer p = (_richTextBox.Selection.Start);
                    p = p.GetPositionAtOffset(value);
                    _richTextBox.Selection.Select(_richTextBox.Selection.Start, p);
                }
                else
                {
                    _plainTextBox.SelectionLength = value;
                }
            }
        }

        int SelectionStart
        {
            get
            {
                if (_thisApp.CurrentEditingMode == EditingMode.Rich)
                {

                    return _richTextBox.Document.ContentStart.GetOffsetToPosition(_richTextBox.Selection.Start);
                }
                else
                {
                    return _plainTextBox.SelectionStart;
                }
            }
            set
            {
                if (_thisApp.CurrentEditingMode == EditingMode.Rich)
                {
                    TextPointer p = (_richTextBox.Document.ContentStart);
                    p = p.GetPositionAtOffset(value);
                    _richTextBox.Selection.Select(p, p);
                }
                else
                {
                    _plainTextBox.SelectionStart = value;
                }
            }
        }

        string SelectedText
        {
            get  //returns text in current selection
            {
                if (_thisApp.CurrentEditingMode == EditingMode.Rich)
                {
                    if (_richTextBox != null)
                    {
                        return _richTextBox.Selection.Text;
                    }
                }
                else
                {
                    if (_plainTextBox != null)
                    {
                        return _plainTextBox.SelectedText;
                    }
                }

                return String.Empty;
            }
            set //replaces current selection with 'value'
            {
                if (_thisApp.CurrentEditingMode == EditingMode.Rich)
                {
                    TextRange tr = new TextRange(_richTextBox.Selection.Start, _richTextBox.Selection.End);
                    tr.Text = value;
                }
                else
                {
                    _plainTextBox.SelectedText = value;
                }
            }
        }

        string TextContent
        {
            get
            {
                if (_thisApp.CurrentEditingMode == EditingMode.Rich)
                {
                    if (_richTextBox != null)
                    {
                        TextRange tr = new TextRange(_richTextBox.Document.ContentStart, _richTextBox.Document.ContentEnd);
                        return tr.Text;
                    }
                }
                else
                {
                    if (_plainTextBox != null)
                    {
                        return _plainTextBox.Text;
                    }
                }

                return String.Empty;
            }
        }

        /// <summary>
        /// Shows/Hides the status bar on the main window
        /// </summary>
        bool StatusBarVisible
        {
            get
            {
                return (STATUS_BAR.Visibility == Visibility.Visible ? true : false);
            }

            set
            {
                STATUS_BAR.Visibility = (value ? Visibility.Visible : Visibility.Collapsed);
            }
        }

        #endregion private Properties


        #region private Methods

        //------------------------------------------------------
        //
        //  private Methods
        //
        //------------------------------------------------------

        void SetTextMode(EditingMode editingMode, bool initialSetting)
        {
            textBoxHostingPanel.Children.Remove(_currentTextBox);
            switch (editingMode)
            {
                case EditingMode.Plain:
                    //setup a fresh new TextBox
                    _plainTextBox = (TextBox)_thisApp.Resources["plainTextBox"];
                    textBoxHostingPanel.Children.Add(_plainTextBox);
                    DockPanel.SetDock(_plainTextBox, Dock.Bottom);

                    _plainTextBox.Text = TextContent;
                    _currentTextBox = _plainTextBox;
                    _richTextBox = null;

                    //set font info based on previous notepad session
                    LoadSettingsFromRegistry(initialSetting);
                    _wordWrapMenu.IsEnabled = _replace.IsEnabled = true;
                    ((TextBox)_currentTextBox).TextWrapping = _plainTextWrapped;
                    if (_plainTextWrapped == TextWrapping.Wrap)
                    {
                        _wordWrapMenu.IsChecked = true;
                        _statusBarMenu.IsEnabled = StatusBarVisible = _statusBarMenu.IsChecked = false;
                        _goTo.IsEnabled = false;
                    }
                    else
                    {
                        _wordWrapMenu.IsChecked = false;
                        _goTo.IsEnabled = true;
                        _statusBarMenu.IsEnabled = true;
                        StatusBarVisible = _statusBarMenu.IsChecked = _plainTextHadStatusbar;
                    }
                    break;

                case EditingMode.Rich:
                    // Disable find for RichTextBox per Windows OS Bug 1199620
                    _find.IsEnabled = false;
                    _findNext.IsEnabled = false;
                    _replace.IsEnabled = false;

                    //persist current plaintextbox settings to registry
                    SaveSettingsToRegistry();

                    //setup a fresh new RichTextBox
                    _richTextBox = (RichTextBox)_thisApp.Resources["richTextBox"];
                    textBoxHostingPanel.Children.Add(_richTextBox);
                    DockPanel.SetDock(_plainTextBox, Dock.Bottom);

                    //make the fontsize/style/family match what the plainTextBox was using
                    TextRange tr = new TextRange(_richTextBox.Document.ContentStart, _richTextBox.Document.ContentEnd);

                    //To be valid, a rich text box must have at least one paragraph (see WindowsOSBugs #1139312).
                    Run run = new Run(TextContent);
                    _richTextBox.Document.Blocks.Clear();

                    if (_richTextBox.Document.Blocks.Count == 1)
                    {
                        ((Paragraph)_richTextBox.Document.Blocks.FirstBlock).Inlines.Add(run);
                    }
                    else
                    {
                        _richTextBox.Document.Blocks.Add(new Paragraph(run));
                    }

                    ((Paragraph)_richTextBox.Document.Blocks.FirstBlock).Margin = new Thickness(0);

                    tr.ApplyPropertyValue(TextElement.FontFamilyProperty, _currentTextBox.FontFamily);
                    tr.ApplyPropertyValue(TextElement.FontSizeProperty, _currentTextBox.FontSize);
                    tr.ApplyPropertyValue(TextElement.FontStyleProperty, _currentTextBox.FontStyle);
                    tr.ApplyPropertyValue(TextElement.FontWeightProperty, _currentTextBox.FontWeight);
                    tr.ApplyPropertyValue(TextElement.FontStretchProperty, _currentTextBox.FontStretch);
                    _currentTextBox = _richTextBox;
                    _wordWrapMenu.IsChecked = true;
                    _statusBarMenu.IsChecked = StatusBarVisible = false;
                    _wordWrapMenu.IsEnabled = _statusBarMenu.IsEnabled = _goTo.IsEnabled = false;
                    _plainTextBox = null;
                    break;

                default:
                    throw new NotImplementedException();
            }

            // TODO: Need to persist item state in the "Format" menu into the registry
            // These settings including "Word Wrap" should also carry over from Standard to RichText mode
            // regardless of which TextBox is active.
            _currentTextBox.SpellCheck.IsEnabled = _spellChecked.IsChecked;

            _thisApp.CurrentEditingMode = editingMode;
            _currentTextBox.TextChanged += new TextChangedEventHandler(ContentTextChanged);
            if (StatusBarVisible)
            {
                _currentTextBox.SelectionChanged += new RoutedEventHandler(OnCaretPositionChanged);
            }

            _currentTextBox.ContextMenuOpening += new ContextMenuEventHandler(_currentTextBox_ContextMenuOpening);
            _currentTextBox.ContextMenuClosing += new ContextMenuEventHandler(_currentTextBox_ContextMenuClosing);

            _redo.IsEnabled = this._currentTextBox.CanRedo;
            _undo.IsEnabled = this._currentTextBox.CanUndo;

            _currentTextBox.Focus();
        }

        void _currentTextBox_ContextMenuClosing(object sender, ContextMenuEventArgs e)
        {
            TextBoxBase _element = sender as TextBoxBase;
            if (_numberOfSpellerChoices > 0)
            {
                while (_numberOfSpellerChoices-- > 0)
                {
                    _element.ContextMenu.Items.RemoveAt(0);
                }
            }
        }

        void _currentTextBox_ContextMenuOpening(object sender, ContextMenuEventArgs e)
        {
            AddSpellerItems(sender as TextBoxBase);
            ((TextBoxBase)sender).ContextMenu.Placement = PlacementMode.RelativePoint;
        }

        private void AddSpellerItems(TextBoxBase _textBox)
        {
            MenuItem menuItem;
            SpellingError spellingError = null;
            spellingError = (_textBox is TextBox) ?
                              ((TextBox)_textBox).GetSpellingError(((TextBox)_textBox).CaretIndex) :
                                  ((RichTextBox)_textBox).GetSpellingError(((RichTextBox)_textBox).CaretPosition);

            if (spellingError == null)
                return;

            bool addedSuggestion = false;
            ContextMenu cm = _textBox.ContextMenu;
            while (_numberOfSpellerChoices-- > 0)
            {
                _textBox.ContextMenu.Items.RemoveAt(0);
            }
            _numberOfSpellerChoices = 0;
            foreach (string suggestion in spellingError.Suggestions)
            {
                menuItem = new MenuItem();
                TextBlock text = new TextBlock();
                text.FontWeight = FontWeights.Bold;
                text.Text = suggestion;
                menuItem.Header = text;
                menuItem.Command = EditingCommands.CorrectSpellingError;
                menuItem.CommandParameter = suggestion;
                cm.Items.Insert(_numberOfSpellerChoices++, menuItem);
                menuItem.CommandTarget = _textBox;

                addedSuggestion = true;
            }

            if (!addedSuggestion)
            {
                menuItem = new MenuItem();
                menuItem.Header = "No Spelling Suggestions";
                menuItem.IsEnabled = false;
                cm.Items.Insert(_numberOfSpellerChoices++, menuItem);
            }

            cm.Items.Insert(_numberOfSpellerChoices++, new Separator());

            menuItem = new MenuItem();
            menuItem.Header = "IgnoreAll";
            menuItem.Command = EditingCommands.IgnoreSpellingError;
            cm.Items.Insert(_numberOfSpellerChoices++, menuItem);
            menuItem.CommandTarget = _textBox;

            return ;
        }

        void DoSpellCheckCommand(object sender, ExecutedRoutedEventArgs e)
        {
            _currentTextBox.SpellCheck.IsEnabled = !(_currentTextBox.SpellCheck.IsEnabled);
            _spellChecked.IsChecked = _currentTextBox.SpellCheck.IsEnabled;
        }

        void DoRichTextModeCommand(object sender, ExecutedRoutedEventArgs e)
        {
            Document.IsTextModeChanged = true;
            if (_thisApp.CurrentEditingMode == EditingMode.Rich)
            {
                _richTextMenu.IsChecked = false;
                SetTextMode(EditingMode.Plain, false);
            }
            else
            {
                _richTextMenu.IsChecked = true;
                SetTextMode(EditingMode.Rich, false);
            }
        }

        void SetMenuItemsPanelToWrapPanel()
        {

            FrameworkElementFactory panel = new FrameworkElementFactory(typeof(WrapPanel));
            panel.SetValue(WrapPanel.HorizontalAlignmentProperty, HorizontalAlignment.Left);
            _mainMenu.ItemsPanel = new ItemsPanelTemplate(panel);
        }

        void WindowClosed(object sender, EventArgs e)
        {
            SaveSettingsToRegistry();
        }

        void SaveSettingsToRegistry()
        {
            RegistryKey key = Registry.CurrentUser.CreateSubKey(@"Software\Microsoft\Notepad");
            if (key == null)
            {
                return;
            }
            key.SetValue("iPointSize", _fontSize * 720 / 96, RegistryValueKind.DWord);  // From pixels to 10ths of a point
            key.SetValue("lfFaceName", _typeface.FontFamily.Source);
            key.SetValue("lfWeight", _typeface.Weight.ToOpenTypeWeight(), RegistryValueKind.DWord);
            key.SetValue("lfItalic", ((_typeface.Style != FontStyles.Normal) ? 1 : 0), RegistryValueKind.DWord);
            key.SetValue("StatusBar", (_plainTextHadStatusbar ? 1 : 0), RegistryValueKind.DWord);
            //TextWrapping property is only for plainTextBox
            if (_currentTextBox is TextBox)
            {
                key.SetValue("fWrap", ( ((TextBox)_currentTextBox).TextWrapping == TextWrapping.NoWrap ? 0 : 1 ), RegistryValueKind.DWord);
            }
            else
            {
                key.SetValue("fWrap", 1, RegistryValueKind.DWord);
            }

            key.SetValue("iMarginLeft",   _leftMargin  /* * 1000*/, RegistryValueKind.DWord);
            key.SetValue("iMarginRight",  _rightMargin /* * 1000*/, RegistryValueKind.DWord);
            key.SetValue("iMarginTop",    _topMargin   /* * 1000*/, RegistryValueKind.DWord);
            key.SetValue("iMarginBottom", _bottomMargin/* * 1000*/, RegistryValueKind.DWord);

            if ((this.Visibility == Visibility.Visible) && (this.WindowState == WindowState.Normal))
            {
                key.SetValue("iWindowPosX", _mainWindow.Left, RegistryValueKind.DWord);
                key.SetValue("iWindowPosY", _mainWindow.Top, RegistryValueKind.DWord);
                key.SetValue("iWindowPosDX", _mainWindow.ActualWidth, RegistryValueKind.DWord);
                key.SetValue("iWindowPosDY", _mainWindow.ActualHeight, RegistryValueKind.DWord);
            }
        }

        void WindowLoaded(object sender, EventArgs e)
        {
            _currentTextBox.Focus();
        }

        void LoadSettingsFromRegistry(bool initialSetting)
        {
            RegistryKey key = Registry.CurrentUser.CreateSubKey(@"Software\Microsoft\Notepad");

            if (key == null)
            {
                return;
            }
            _plainTextHadStatusbar = (int)key.GetValue("StatusBar", 0) == 1 ? true : false;
            _plainTextWrapped = (int)key.GetValue("fWrap", 0) == 1 ? TextWrapping.Wrap : TextWrapping.NoWrap;
            if (initialSetting)
            {
                _mainWindow.Left = (int)key.GetValue("iWindowPosX", 150);
                _mainWindow.Top = (int)key.GetValue("iWindowPosY", 230);
                _mainWindow.Width = (int)key.GetValue("iWindowPosDX", 700);
                _mainWindow.Height = (int)key.GetValue("iWindowPosDY", 600);
            }

            //These default values assume US locale, i.e., measurements in inches
            //metric defaults are 2500/2000
            //the registry stores in 100ths of an inch (or of a printer unit)
            // Win32 PageSetupDialog uses hundredths of inches. 
            _leftMargin = (int)key.GetValue("iMarginLeft", 75);
            _rightMargin = (int)key.GetValue("iMarginRight", 75);
            _topMargin = (int)key.GetValue("iMarginTop", 100);
            _bottomMargin = (int)key.GetValue("iMarginBottom", 100);

            _plainTextBox.FontFamily = new FontFamily((string)key.GetValue("lfFaceName", "Lucida Console"));
            _plainTextBox.FontStyle = ((int)key.GetValue("lfItalic", 0) == 0) ? FontStyles.Normal : FontStyles.Italic;
            _plainTextBox.FontWeight = FontWeight.FromOpenTypeWeight((int)key.GetValue("lfWeight", 400));
            _plainTextBox.FontStretch = FontStretches.Normal;
            double fontSize = ((int)key.GetValue("iPointSize", 100)) / 10;
            _plainTextBox.FontSize = fontSize * 96.0 / 72.0;      //standard conversion between pixels and points

            // Copy plain text font settings into rich text defaults
            _typeface = new Typeface(_plainTextBox.FontFamily, _plainTextBox.FontStyle, _plainTextBox.FontWeight, _plainTextBox.FontStretch);
            _fontSize = _plainTextBox.FontSize;
        }

        void WindowDragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent("FileDrop"))
            {
                e.Effects = DragDropEffects.Copy;
            }
       }

        void WindowDrop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent("FileDrop"))
            {
                string filename = ((string[])e.Data.GetData("FileDrop"))[0];
                if (!TryDocClose())
                    return;
                NotepadDocument doc = new NotepadDocument(filename);
                Document = doc;
            }
        }

        void WindowClosing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (Document != null)
            {
                e.Cancel = !TryDocClose();
            }
        }

        void WindowActivated(object sender, EventArgs e)
        {
            Debug.Print("WindowActivated");
            if (_currentTextBox != null)
                _currentTextBox.Focus();
        }

        void OnCaretPositionChanged(object sender, RoutedEventArgs e)
        {
            if (_thisApp.CurrentEditingMode == EditingMode.Plain)
            {
                int caretPos = SelectionStart;
                int lineNumber = _plainTextBox.GetLineIndexFromCharacterIndex(caretPos)+ 1;
                int colNumber;
                try
                {
                    colNumber = caretPos - _plainTextBox.GetCharacterIndexFromLineIndex(lineNumber - 1);
                }

                catch (ArgumentOutOfRangeException aoore)
                {
                    colNumber = 0;
                    System.Diagnostics.Debug.Print("ArgumentOutOfRangeException thrown. Windows OS Bug 1554576");
                }
                _statusBarText.Text = "Ln " + lineNumber.ToString() + ", Col " + colNumber.ToString();
            }
            else
            {
                // TODO: Rich Text box status bar
            }
        }

        void ContentTextChanged(object sender, TextChangedEventArgs e)
        {
            this._redo.IsEnabled = this._currentTextBox.CanRedo;
            this._undo.IsEnabled = this._currentTextBox.CanUndo;
            Document.IsDirty = this._currentTextBox.CanUndo;
        }

        void CanExecuteFindCommand(object sender, CanExecuteRoutedEventArgs e)
        {
            if (_thisApp.CurrentEditingMode == EditingMode.Plain)
            {
                e.CanExecute = !String.IsNullOrEmpty(TextContent);
            }
        }

        void DoSaveCommand(object sender, ExecutedRoutedEventArgs e)
        {
            UpdateDocumentContent();
            if (Document.IsTextModeChanged)
            {
                Document.IsTextModeChanged = !Document.SaveNew();
            }
            else
            {
                Document.Save();
            }
            _mainWindow.Title = Document.DisplayName;
        }

        void DoSaveAsCommand(object sender, ExecutedRoutedEventArgs e)
        {
            //if (Document != null)
            {
                UpdateDocumentContent();
                Document.SaveNew();
                _mainWindow.Title = Document.DisplayName;
            }
        }

        void DoExitCommand(object sender, ExecutedRoutedEventArgs e)
        {
            this.Close();
        }

        void DoNewCommand(object sender, ExecutedRoutedEventArgs e)
        {
            if (TryDocClose())
            {
                Document = new NotepadDocument();
                _mainWindow.Title = Document.DisplayName;
                _statusBarText.Text = "Ln 1, Col 1";
            }
        }

        void DoOpenCommand(object sender, ExecutedRoutedEventArgs e)
        {
            if (!TryDocClose())
                return;
            NotepadDocument doc = new NotepadDocument();
            if (doc.Open())
            {
                Document = doc;
                Document.IsDirty = false;
            }
        }

        void UpdateDocumentContent()
        {
            switch (_thisApp.CurrentEditingMode)
            {
                case EditingMode.Rich:
                    // take current content, serialize it to xaml, save in _doc.Content;
                    TextRange tr = new TextRange(_richTextBox.Document.ContentStart, _richTextBox.Document.ContentEnd);
                    _doc.Content = TextRange_GetXml(tr);
                    break;
                case EditingMode.Plain:
                    _doc.Content = TextContent;
                    break;
                default:
                    throw new NotImplementedException("Editing modes besides Plain & Rich are not supported.");
            }
        }

        bool TryDocClose()
        {
            UpdateDocumentContent();
            
            if (_plainTextBox is TextBox)
            {
                _plainTextBox.IsUndoEnabled = false;
                _plainTextBox.IsUndoEnabled = true;
            }
            if (_richTextBox is RichTextBox)
            {
                _richTextBox.IsUndoEnabled = false;
                _richTextBox.IsUndoEnabled = true;
            }
         
            return _doc.Close();
        }

        void DoRightToLeftReadCommand(object sender, ExecutedRoutedEventArgs e)
        {
            _currentTextBox.FlowDirection = FlowDirectionSettings;
        }

        void DoDeleteCommand(object sender, ExecutedRoutedEventArgs e)
        {
            SelectedText = "";
        }

        void CanExecuteDeleteCommand(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = SelectedText.Length > 0;
        }

        void EnsureFind()
        {
            if (_dlg == null)
            {
                _dlg = new FindDialog();

                _dlg._findWhat.Focus();
                if (_findString != null)
                {
                    _dlg._findWhat.SelectedText = _findString;
                    _dlg.ReplaceWith = _replaceString;
                }

                _dlg.Owner = this;
                _firstReplace = true;

                _dlg.FindNext += delegate(object sender, EventArgs args)
                {
                    StoreFindDialogBoxValues();
                    Find(false);
                };

                _dlg.Replace += delegate(object sender, EventArgs args)
                {
                    StoreFindDialogBoxValues();
                    String _currentFindSelection = SelectedText;
                    if (!_findMatchCase)
                    {
                        _currentFindSelection = _currentFindSelection.ToLower();
                        _findString =_findString.ToLower();
                    }
                    if ((!_firstReplace) || ((_currentFindSelection == _findString)))
                    {
                        using (_currentTextBox.DeclareChangeBlock())
                        {
                            SelectedText = _dlg.ReplaceWith;
                        }
                    }
                    else
                    {
                        _firstReplace = false;
                    }
                    Find(false);
                    _dlg.Focus();
                };

                _dlg.ReplaceAll += delegate(object sender, EventArgs args)
                {
                    StoreFindDialogBoxValues();
                    SelectionStart = 0;
                    SelectionLength = 0;
                    while (Find(true))
                    {
                        using (_currentTextBox.DeclareChangeBlock())
                        {
                            SelectedText = _dlg.ReplaceWith;
                            SelectionLength = _dlg.ReplaceWith.Length;
                        }
                    }
                    SelectionStart = 0;
                    SelectionLength = 0;
                    _dlg.Focus();
                };
                _dlg.Closed += delegate(object sender, EventArgs args)
                {
                    _dlg = null;
                };
            }
        }

        void StoreFindDialogBoxValues()
        {
             _findString = _dlg.FindWhat;
             _replaceString = _dlg.ReplaceWith;
        }

        void UpdateFindInfo()
        {
            _findString = _dlg.FindWhat.ToString();
            if (_dlg.MatchCase == true)
                _findMatchCase = true;
            else
                _findMatchCase = false;

            if ((bool)_dlg.SearchUp)
                _findUpward = true;
            else
                _findUpward = false;
        }

        void DoFindCommand(object sender, ExecutedRoutedEventArgs e)
        {
            if (string.IsNullOrEmpty(TextContent))
                return;
            EnsureFind();
            _dlg.Title = _resourceManager.GetString("FindDialogTitleText");
            _dlg.ShowReplace = false;
            _dlg.Show();
        }

        void DoFindNextCommand(object sender, ExecutedRoutedEventArgs e)
        {
            if (string.IsNullOrEmpty(_findString))
            {
                EnsureFind();
                _dlg.ShowReplace = false;
                _dlg.Show();
            }
            else
                Find(false);
        }

        bool Find(bool ReplaceAll)
        {
            if (_dlg != null)
                UpdateFindInfo();

            // Implementing Find in RTB using the internal find method.
            // The current internal implementation works only with text without any inline tags (no text formatting).
            if (_thisApp.CurrentEditingMode == EditingMode.Rich)
            {
                Assembly frameworkAssembly;     // current assembly (for reflection of Find method)
                Type findEngineType;            // Type we're looking for
                BindingFlags flags;             // params for reflection
                object[] args;                  // args for calling internal method
                TextRange searchContainer;      // RTB to search in
                string searchPattern;           // pattern to search for
                MethodInfo findMethod;          // located Find method


                System.Globalization.CultureInfo cultureInfo;
                flags = BindingFlags.InvokeMethod | BindingFlags.NonPublic | BindingFlags.Public | BindingFlags.Static;

                TextRange rangeFound;
                int findFlags = 0;

                // searching downward- from current caret position to end of RTB.
                // TODO Find upwards not implemented.
                searchContainer = new TextRange(_richTextBox.Selection.End, _richTextBox.Document.ContentEnd);

                //text to be found
                searchPattern = _findString;

                //if case has to be matched
                findFlags = _findMatchCase?1:0;
                cultureInfo = System.Globalization.CultureInfo.CurrentCulture;
                args = new object[] { searchContainer, searchPattern, findFlags, cultureInfo };
                frameworkAssembly = typeof(RichTextBox).Assembly;
                findEngineType = frameworkAssembly.GetType("System.Windows.Documents.TextFindEngine", true, false);
                findMethod = findEngineType.GetMethod("Find", flags);

                if (findMethod == null)
                {
                    throw new ApplicationException(@"Find method on TextFindEngine type not found");
                }
                
                rangeFound = findMethod.Invoke(null, args) as TextRange;

                if (rangeFound == null)
                {
                    if (!ReplaceAll)
                    {
                        String message = String.Format(_resourceManager.GetString("cannotfindstring"), _findString);
                        MessageBox.Show(message, _resourceManager.GetString("WPFnotepad"), MessageBoxButton.OK, MessageBoxImage.Information);
                        _firstReplace = true;
                    }

                    return false;
                }
                else
                {
                    _richTextBox.Selection.Select(rangeFound.Start, rangeFound.End);
                    return true;
                }
            }
            //find in plain text box.
            else
            {
                String contentText = "";
                String findText = "";
                int found = -1;

                // if case is not an issue, then convert them all to lower case
                if (!_findMatchCase)
                {
                    contentText = TextContent.ToLower();
                    findText = _findString.ToLower();
                }
                else
                {
                    contentText = TextContent;
                    findText = _findString;
                }
                // if we are doing upward search, then we start at the (caret position - selection length)
                if (_findUpward && !ReplaceAll)
                {
                    // forward selection
                    if (SelectionStart > 0)
                        found = contentText.LastIndexOf(findText, SelectionStart - 1);
                }
                // if we are doing search downward, then selection start the at (caret position + selection length)
                else
                {
                    found = contentText.IndexOf(findText, SelectionStart + SelectionLength);
                }

                if (found != -1)
                {
                    _currentTextBox.Focus();
                    SelectionStart = found;
                    SelectionLength = findText.Length;
                    return true;
                }
                else if (!ReplaceAll)
                {
                    String message = String.Format(_resourceManager.GetString("cannotfindstring"), _findString);
                    MessageBox.Show(message, _resourceManager.GetString("WPFnotepad"), MessageBoxButton.OK, MessageBoxImage.Information);
                    _firstReplace = true;
                    return false;
                }
                else
                {
                    return false;
                }
            }
        }

        void DoReplaceCommand(object sender, ExecutedRoutedEventArgs e)
        {
            EnsureFind();
            _dlg.Title = _resourceManager.GetString("ReplaceDialogTitleText");
            _dlg.ShowReplace = true;
            _dlg.Show();
            _dlg.FindWhat = _findString;
            _dlg.ReplaceWith = _replaceString;
        }

        private void DoEscapeToCCommand(object sender, ExecutedRoutedEventArgs e)
        {
            // Format string: \ + x + hex w/ 4 digits minimum
            InternalDoEscape("\\x{0:x4}");
        }

        private void DoEscapeToXmlCommand(object sender, ExecutedRoutedEventArgs e)
        {
            // Format string: & + # + x + hex w/ 4 digits minimum + ;
            InternalDoEscape("&#x{0:x4};");
        }

        void DoGotoCommand(object sender, ExecutedRoutedEventArgs e)
        {
            if (_thisApp.CurrentEditingMode != EditingMode.Plain)
            {
                return;
            }
            if (!_goTo.IsEnabled)
            {
                return;
            }
            GoToDialog dlg = new GoToDialog();
            dlg.Owner = this;
            dlg.LineNumber = CaretLineNumber;
            dlg.MaxLineNumber = _plainTextBox.LineCount;

            dlg.GotoActivate += delegate(object sender2, EventArgs e2)
            {
                dlg.LineNumber = CaretLineNumber;
            };

            if (dlg.ShowDialog() == true)
            {
                CaretLineNumber = dlg.LineNumber;
            }
        }

        void DoSelectAllCommand(object sender, ExecutedRoutedEventArgs e)
        {
            _currentTextBox.Focus();
            _currentTextBox.SelectAll();
        }

        void DoTimeDateCommand(object sender, ExecutedRoutedEventArgs e)
        {

            string date = DateTime.Now.ToString();
            using (_currentTextBox.DeclareChangeBlock())
            {
                SelectedText += date;
            }
            SelectionStart += date.Length;
            SelectionLength = 0;
        }


        void DoWordWrapCommand(object sender, ExecutedRoutedEventArgs e)
        {
            // WordWrap menu is only active when _currentTextBox == _plainTextBox
            if (_currentTextBox is TextBox)
            {
                if (((TextBox)_currentTextBox).TextWrapping == TextWrapping.NoWrap)
                {
                    ((TextBox)_currentTextBox).TextWrapping = TextWrapping.Wrap;
                    _plainTextWrapped = TextWrapping.Wrap;
                    _currentTextBox.HorizontalScrollBarVisibility = ScrollBarVisibility.Hidden;
                    StatusBarVisible = false;
                    _statusBarMenu.IsChecked = false;
                    _statusBarMenu.IsEnabled = false;
                }
                else
                {
                    ((TextBox)_currentTextBox).TextWrapping = TextWrapping.NoWrap;
                    _plainTextWrapped = TextWrapping.NoWrap;
                    StatusBarVisible = _statusBarMenu.IsChecked = _statusBarChecked;
                    _statusBarMenu.IsEnabled = true;
                    _currentTextBox.HorizontalScrollBarVisibility = ScrollBarVisibility.Visible;
                }
                _wordWrapMenu.IsChecked = _plainTextWrapped == TextWrapping.Wrap ? true : false;
                _goTo.IsEnabled = !_wordWrapMenu.IsChecked;
            }
        }


        void ApplyPlainTextFontChoice(object sender, FontChooserDialogAppliedEventsArgs e)
        {
            _plainTextBox.FontFamily  = e.FontChoice.FontFamily;
            _plainTextBox.FontStyle   = e.FontChoice.FontStyle;
            _plainTextBox.FontWeight  = e.FontChoice.FontWeight;
            _plainTextBox.FontStretch = e.FontChoice.FontStretch;
            _plainTextBox.FontSize    = e.FontChoice.FontSize;
            _plainTextBox.Foreground  = e.FontChoice.Foreground;

            _typeface = new Typeface(e.FontChoice.FontFamily, e.FontChoice.FontStyle, e.FontChoice.FontWeight, e.FontChoice.FontStretch);
            _fontSize = e.FontChoice.FontSize;
        }

        void DoFontCommand(object sender, ExecutedRoutedEventArgs e)
        {
            if (_thisApp.CurrentEditingMode == EditingMode.Rich)
            {
                FontChooser fontChooser = new FontChooser(_richTextBox);
                SetChildWindowPosition(fontChooser);
                fontChooser.Owner = this;
                fontChooser.ShowDialog();
                if ((bool)fontChooser.DialogResult)
                {
                    Document.IsDirty = true;
                }
            }
            else
            {
                FontChoice choice    = new FontChoice();

                choice.FontFamily  = _plainTextBox.FontFamily;
                choice.FontStyle   = _plainTextBox.FontStyle;
                choice.FontWeight  = _plainTextBox.FontWeight;
                choice.FontStretch = _plainTextBox.FontStretch;
                choice.FontSize    = _plainTextBox.FontSize;
                choice.Foreground  = (SolidColorBrush)_plainTextBox.Foreground;

                FontChooser fontChooser = new FontChooser(choice);
                fontChooser.FontChosen += ApplyPlainTextFontChoice;

                SetChildWindowPosition(fontChooser);
                fontChooser.Owner = this;
                fontChooser.ShowDialog();
            }
        }


        void DoAboutCommand(object sender, ExecutedRoutedEventArgs e)
        {
            IntPtr hWnd = (IntPtr)0;
            IntPtr hIcon = LoadImage(0, "notepad.ico", 1, 0, 0, 16);
            string szApp = "WPF Notepad";
            string szOtherStuff = "";
            ShellAbout(hWnd, szApp, szOtherStuff, hIcon);
        }

        void DoShowBugsCommand(object sender, ExecutedRoutedEventArgs e)
        {
            // This method is no longer in use...
        }

        void DoFileAssociationsCommand(object sender, ExecutedRoutedEventArgs e)
        {
            FileAssociationsDialog fad = new FileAssociationsDialog();
            fad.Owner = this;
            fad.ShowDialog();
        }

        void DoShowHeaderFooterCommand(object sender, ExecutedRoutedEventArgs e)
        {
            
            if (_headerFooterMenu.IsChecked)
            {
                _headersPrintPanel.Visibility = Visibility.Visible;
            }
            else
            {
                _headersPrintPanel.Visibility = Visibility.Collapsed;
            }
        }

        void DoShowStatusBarCommand(object sender, ExecutedRoutedEventArgs e)
        {
            _plainTextHadStatusbar = StatusBarVisible = !StatusBarVisible;
            if (StatusBarVisible)
            {
                _statusBarChecked = true;
                _mainWindow.ResizeMode = ResizeMode.CanResizeWithGrip;
                _currentTextBox.SelectionChanged +=new RoutedEventHandler(OnCaretPositionChanged);
            }
            else
            {
                _statusBarChecked = false;
                _mainWindow.ResizeMode = ResizeMode.CanResize;
                _currentTextBox.SelectionChanged -= new RoutedEventHandler(OnCaretPositionChanged);
            }
        }

        void DoPageSetupCommand(object sender, ExecutedRoutedEventArgs e)
        {
            PageSetupDialog pageSetupDlg = SetUpPageSetupDialog();
            PageSetupDialogResult dialogResult = pageSetupDlg.ShowDialog();

            if (dialogResult == PageSetupDialogResult.OK)
            {
                SavePageSetupInformation(pageSetupDlg);
            }
        }

        internal PageSetupDialog SetUpPageSetupDialog()
        {
            PageSetupDialog pageSetupDlg = new PageSetupDialog();
            pageSetupDlg.PageSettings = new System.Drawing.Printing.PageSettings();
            pageSetupDlg.PrinterSettings = new System.Drawing.Printing.PrinterSettings();

            //LoadCurrentPrinterChoice(pageSetupDlg);
            //load up margin info
            // Win32 PageSetupDialog uses hundredths of inches. 
            pageSetupDlg.PageSettings.Margins.Left = (int)_leftMargin;
            pageSetupDlg.PageSettings.Margins.Right = (int)_rightMargin;
            pageSetupDlg.PageSettings.Margins.Top = (int)_topMargin;
            pageSetupDlg.PageSettings.Margins.Bottom = (int)_bottomMargin;

            PrintTicket printTicket;

            if (_currentPrinterSettings != null)
            {
                printTicket = _currentPrinterSettings;
            }
            else
            {
                printTicket = new PrintTicket();
            }

            // Load up orientation info
            if (_pageOrientation != null)
            {
                printTicket.PageOrientation = (PageOrientation)_pageOrientation;

                switch (_pageOrientation)
                {
                    case PageOrientation.Landscape:
                        pageSetupDlg.PageSettings.Landscape = true;
                        break;
                    case PageOrientation.Portrait:
                        pageSetupDlg.PageSettings.Landscape = false;
                        break;
                    // Any other settings, the Win32 PageSetupDialog cannot handle so, we default to portrait
                    default:
                        pageSetupDlg.PageSettings.Landscape = false;
                        break;
                }
            }

            // Load up page size info
            if (_pageSize != null)
            {
                printTicket.PageMediaSize = new PageMediaSize((PageMediaSizeName)_pageSize);
            }
            _currentPrinterSettings = printTicket;
            
            return pageSetupDlg;
        }

        internal void SavePageSetupInformation(PageSetupDialog pageSetupDlg)
        {
            // Save any preferences that might have been customized:
            // Current printer, page size, orientation, margins
            //    SaveCurrentPrinterChoice(pageSetupDlg);
            
            // Win32 PageSetupDialog uses hundredths of inches. 
            _leftMargin = pageSetupDlg.PageSettings.Margins.Left;// / 25400;
            _rightMargin = pageSetupDlg.PageSettings.Margins.Right;// / 25400;
            _topMargin = pageSetupDlg.PageSettings.Margins.Top;// / 25400;
            _bottomMargin = pageSetupDlg.PageSettings.Margins.Bottom;// / 25400;

            PrintTicket printTicket = _currentPrinterSettings;
            if (pageSetupDlg.PageSettings.Landscape)
            {
                _pageOrientation = printTicket.PageOrientation = PageOrientation.Landscape;
            }
            else
            {
                _pageOrientation = printTicket.PageOrientation = PageOrientation.Portrait;
            }

            if (printTicket.PageMediaSize != null)
            {
                _pageSize = printTicket.PageMediaSize.PageMediaSizeName;
            }
        }

        internal void LoadCurrentPrinterChoice(PrintDialog dlg)
        {
            //load up printer info
            if (_currentPrinter != null)
            {
                dlg.PrintQueue = _currentPrinter;
            }
            if (_currentPrinterSettings != null)
            {
                dlg.PrintTicket = _currentPrinterSettings;
            }
        }

        internal void SaveCurrentPrinterChoice(PrintDialog dlg)
        {
            if (dlg.PrintQueue != null)
            {
                _currentPrinter = dlg.PrintQueue;
            }
            if (dlg.PrintTicket != null)
            {
                _currentPrinterSettings = dlg.PrintTicket;
            }
        }

        internal void SetUpPrintDialog()
        {
            printDlg = new PrintDialog();
            LoadCurrentPrinterChoice(printDlg);
        }


        internal void SavePrintInformation()
        {
            SaveCurrentPrinterChoice(printDlg);
        }


        internal void PrintDocument()
        {
            PrintTicket printTicket = printDlg.PrintTicket;

            FlowDocument flowDocForPrinting = GetFlowDocumentForPrinting(printTicket);

            // In case the user has changed some settings (e.g., orientation or page size)
            // by going to the printer preferences dialog from the print dialog itself,
            // set the printer queue's User ticket to the one attached to the print dialog.
            printDlg.PrintQueue.UserPrintTicket = printTicket;

            // Print the FlowDocument

            printDlg.PrintQueue.CurrentJobSettings.Description = Document.DisplayName;
            XpsDocumentWriter docWriter = PrintQueue.CreateXpsDocumentWriter(printDlg.PrintQueue);

            // Use our IDocumentPaginator implementation so we can insert headers and footers,
            // if present.
            // PrintableAreaHeight and Width are passed to the paginator to establish the 
            // true printable area for the document.
            HeaderFooterDocumentPaginator paginator = new HeaderFooterDocumentPaginator(
                ((IDocumentPaginatorSource)flowDocForPrinting).DocumentPaginator,
                _headerText.Text,
                _footerText.Text,
                printDlg.PrintableAreaHeight,
                printDlg.PrintableAreaWidth);

            if (_asyncPrintFlag == false)
            {
                try
                {
                    docWriter.Write(paginator, printTicket);
                }
                catch (PrintingCanceledException pce)
                {
                    Debug.Print("Printing was canceled.");
                }
            }
            else
            {
                // Changes for Async printing start here:
                Application.Current.MainWindow.Opacity = 0.7;
                PrintProgressWindow dlg = new PrintProgressWindow(docWriter);
                dlg.Owner = this;
                dlg.PageNumber = 0;

                docWriter.WritingProgressChanged += new WritingProgressChangedEventHandler(dlg.OnWritingProgressChanged);
                docWriter.WritingCompleted += new WritingCompletedEventHandler(dlg.OnWritingCompleted);
                docWriter.WriteAsync(paginator, printTicket);
                dlg.ShowDialog();

                // Reset the flag here for next printing invocation.
                _asyncPrintFlag = false;
            }
        }
        void DoPrintAsyncCommand(object sender, ExecutedRoutedEventArgs e)
        {
            _asyncPrintFlag = true;
            DoPrintCommand(sender, e);
        }
        void DoPrintCommand(object sender, ExecutedRoutedEventArgs e)
        {
            SetUpPrintDialog();

            bool? dialogResult = printDlg.ShowDialog();

            if (dialogResult == true)
            {
                PrintDocument();
                SavePrintInformation();
            }
        }

        internal FlowDocument GetFlowDocumentForPrinting(PrintTicket printTicket)
        {
            //Create a FlowDocument that will contain text to be printed.
            FlowDocument flowDoc = new FlowDocument();

            TextRange flowDocTextRange = new TextRange(flowDoc.ContentStart, flowDoc.ContentEnd);
            TextRange rtbTextRange;

            //Using RichTextBox to generate text in FlowDocument

            //case 1: Printing contents of Rich Text Box.
            if (_thisApp.CurrentEditingMode == EditingMode.Rich)
            {
                // Using current Rich textbox to generate text for Flow Document.
                rtbTextRange = new TextRange(_richTextBox.Document.ContentStart, _richTextBox.Document.ContentEnd);
            }
            //Printing contents of a plain text box
            else
            {
                //create a new Rich Text Box and transfer printable text from current Textbox to it.
                RichTextBox printRichTextBox = new RichTextBox();
                printRichTextBox.FontFamily = _plainTextBox.FontFamily;
                printRichTextBox.FontSize = _plainTextBox.FontSize;
                printRichTextBox.FontWeight = _plainTextBox.FontWeight;
                printRichTextBox.Foreground = _plainTextBox.Foreground;

                // Create a new paragraph block containing the text from the TextBox and add it to
                // the RichTextBox's document.
                Paragraph newPara = new Paragraph();
                newPara.Inlines.Clear();
                newPara.Inlines.Add(new Run(TextContent));
                if (printRichTextBox.Document.Blocks.FirstBlock != null)
                {
                    printRichTextBox.Document.Blocks.InsertBefore(printRichTextBox.Document.Blocks.FirstBlock, newPara);
                }
                else
                {
                    printRichTextBox.Document.Blocks.Add(newPara);
                }


                rtbTextRange = new TextRange(printRichTextBox.Document.ContentStart, printRichTextBox.Document.ContentEnd);
            }

            //transfering contents of RichtextBox textrange to flowDocTextRange in XML format
            //flowDoc now contains text to be printed

            string _rangeXML = TextRange_GetXml(rtbTextRange);
            TextRange_SetXml(flowDocTextRange, _rangeXML);

            ////Set margins of the Flow Document to values set in Page SetUp dialog.
            ////convert inches to logical pixels
            // Win32 PageSetupDialog uses hundredths of inches.
            // Divide by 100 to start with inches.
            LengthConverter converter = new LengthConverter();
            double leftInPixels = (double)converter.ConvertFromInvariantString((_leftMargin/100).ToString() + " in");
            double rightInPixels = (double)converter.ConvertFromInvariantString((_rightMargin/100).ToString() + " in");
            double topInPixels = (double)converter.ConvertFromInvariantString((_topMargin/100).ToString() + " in");
            double bottomInPixels = (double)converter.ConvertFromInvariantString((_bottomMargin/100).ToString() + " in");

            System.Windows.Thickness pagethickness = new Thickness(leftInPixels, topInPixels, rightInPixels, bottomInPixels);
            flowDoc.PagePadding = pagethickness;

            double maxColumnWidth;

            if (printTicket.PageMediaSize != null &&
                printTicket.PageMediaSize.Width != null)
            {
                maxColumnWidth = printTicket.PageMediaSize.Width.Value - leftInPixels - rightInPixels;
            }
            else
            {
                // fallback to Letter size if PrintTicket doesn't specify the media width
                maxColumnWidth = 816 - leftInPixels - rightInPixels;
            }

            flowDoc.ColumnWidth = maxColumnWidth;       //ensures we get only one column
            flowDoc.FlowDirection = FlowDirectionSettings;

            return flowDoc;
        }

        void DoHelpTopicsCommand(object sender, ExecutedRoutedEventArgs e)
        {
            string winDir = Environment.ExpandEnvironmentVariables("%windir%");
            string helpFile = Path.Combine(winDir, @"Help\notepad.chm");
            if (File.Exists(helpFile))
            {
                Process.Start(helpFile);
            }
            else
            {
                System.Windows.Forms.MessageBox.Show(
                    _resourceManager.GetString("HelpFileNotFoundError"),
                    _resourceManager.GetString("WPFnotepad"),
                    System.Windows.Forms.MessageBoxButtons.OK,
                    System.Windows.Forms.MessageBoxIcon.Exclamation);
            }
        }


        void DoReportAnIssueCommand(object sender, ExecutedRoutedEventArgs e)
        {
            Process.Start("mailto:avachow");
        }

        /// <summary>Performs an escape of the current selection into the specified format.</summary>
        private void InternalDoEscape(string format)
        {
            StringBuilder sb;
            string text;

            text = SelectedText;
            if (text == null || text.Length == 0)
            {
                return;
            }

            sb = new StringBuilder(text.Length * 4);
            for (int i = 0; i < text.Length; i++)
            {
                char c = text[i];

                //
                // Allow a few simple characters to pass through, escape
                // everything else.
                //
                if ((c >= 'A' && c <='Z') ||
                    (c >= 'a' && c <='z') ||
                    (c >= '0' && c <='9') ||
                    (c == ' ' || c == '.' || c == ',' || c == '-'))
                {
                    sb.Append(c);
                }
                else
                {
                    int numericChar = (int) c;
                    sb.Append(String.Format(format, numericChar));
                }
            }

            using (_currentTextBox.DeclareChangeBlock())
            {
                SelectedText = sb.ToString();
            }
        }

        // Sets the position of any window to a offset relative to its parent
        // This is needed for reliable test automation for Windows that doesn't
        // center to parent window by default or is larger than the size of the parent.
        // VScan will clip and fail in those cases.
        public void SetChildWindowPosition(Window target)
        {
            target.Top  = _mainWindow.Top  + CHILD_WINDOW_OFFSET_TOP;
            target.Left = _mainWindow.Left + CHILD_WINDOW_OFFSET_LEFT;
        }

        /// <summary>
        /// Set xml to TextRange.Xml property.
        /// </summary>
        /// <param name="range">TextRange</param>
        /// <param name="xml">Xaml to be set</param>        
        private void TextRange_SetXml(TextRange range, string xml)
        {
            MemoryStream mstream;
            if (null == xml)
            {
                throw new ArgumentNullException("xml");
            }
            if (range == null)
            {
                throw new ArgumentNullException("range");
            }

            mstream = new MemoryStream();
            StreamWriter sWriter = new StreamWriter(mstream);

            mstream.Seek(0, SeekOrigin.Begin); //this line may not be needed.
            sWriter.Write(xml);
            sWriter.Flush();

            //move the stream pointer to the beginning.
            mstream.Seek(0, SeekOrigin.Begin);

            range.Load(mstream, DataFormats.Xaml);
        }

        /// <summary>
        /// Get xaml from TextRange.Xml property
        /// </summary>
        /// <param name="range">TextRange</param>
        /// <returns>return a string serialized from the TextRange</returns>
        private  string TextRange_GetXml(TextRange range)
        {
            MemoryStream mstream;

            if (range == null)
            {
                throw new ArgumentNullException("range");
            }

            mstream = new MemoryStream();
            range.Save(mstream, DataFormats.Xaml);

            //must move the stream pointer to the beginning since range.save() will move it to the end.
            mstream.Seek(0, SeekOrigin.Begin);

            //Create a stream reader to read the xaml.
            StreamReader stringReader = new StreamReader(mstream);

            return stringReader.ReadToEnd();
        }

        #endregion private Methods


        #region private Fields

        //------------------------------------------------------
        //
        //  private Fields
        //
        //------------------------------------------------------

        NotepadDocument _doc = new NotepadDocument();
        ResourceManager _resourceManager = new ResourceManager("Microsoft.Samples.WPFNotepad.stringtable", Assembly.GetExecutingAssembly());
        TextBox _plainTextBox;
        RichTextBox _richTextBox;
        FindDialog  _dlg = null;
        Typeface    _typeface;
        double      _fontSize;
        string       _findString;
        string       _replaceString;
        bool         _findMatchCase = false;
        bool         _findUpward    = false;
        bool         _statusBarChecked;
        TextWrapping _plainTextWrapped;
        bool         _plainTextHadStatusbar;
        Boolean      _firstReplace;

        int _numberOfSpellerChoices = 0;
        PageOrientation? _pageOrientation;
        PageMediaSizeName?   _pageSize;

        //printing related
        double      _leftMargin, _rightMargin, _topMargin, _bottomMargin;            //Margins saved in units of 1000th of an inch.
        PrintQueue  _currentPrinter = null;
        PrintTicket _currentPrinterSettings = null;

        NotepadApp  _thisApp = (NotepadApp)(Application.Current);
        System.Windows.Controls.Primitives.TextBoxBase _currentTextBox;

        // Offset to position of all child windows relative to the parent
        const int   CHILD_WINDOW_OFFSET_LEFT    = 20;
        const int   CHILD_WINDOW_OFFSET_TOP     = 20;

        //maximum pages that can be prined using the winforms Print Dialog
        const int   MAX_PRINTED_PAGES           = 9999;

        // Flag which indicates whether synchronous
        // or asynchronous printing was invoked.
        bool _asyncPrintFlag = false;

        System.Windows.Controls.PrintDialog printDlg = null;

        #endregion private Fields
    }

}
