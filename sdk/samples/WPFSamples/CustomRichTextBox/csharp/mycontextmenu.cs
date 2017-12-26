using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;

namespace CustomRichTextBoxSample
{
    /// <summary>
    /// Custom context menu for MyRichTextBox type.
    /// </summary>
    internal class MyContextMenu : ContextMenu
    {
        public MyContextMenu(MyRichTextBox myRichTextBox) : base()
        {
            _myRichTextBox = myRichTextBox;
        }

        protected override void OnOpened(RoutedEventArgs e)
        {
            this.Items.Clear();
            this.AddEmailNameDictionaryMenuItems();
            this.AddSpellerMenuItems();
            this.AddClipboardMenuItems();            
        }

        // Helper to add menu items for Cut, Copy and Paste commands.
        private void AddClipboardMenuItems()
        {
            MenuItem menuItem;

            menuItem = new MenuItem();
            menuItem.Header = ApplicationCommands.Cut.Text;
            menuItem.CommandTarget = _myRichTextBox;
            menuItem.Command = ApplicationCommands.Cut;
            this.Items.Add(menuItem);

            menuItem = new MenuItem();
            menuItem.Header = ApplicationCommands.Copy.Text;
            menuItem.CommandTarget = _myRichTextBox;
            menuItem.Command = ApplicationCommands.Copy;
            this.Items.Add(menuItem);

            menuItem = new MenuItem();
            menuItem.Header = ApplicationCommands.Paste.Text;
            menuItem.CommandTarget = _myRichTextBox;
            menuItem.Command = ApplicationCommands.Paste;
            this.Items.Add(menuItem);
        }

        // Helper to add speller menu items.
        private void AddSpellerMenuItems()
        {
            TextPointer mousePosition = this.GetMousePosition();
            if (mousePosition != null)
            {
                SpellingError spellingError = _myRichTextBox.GetSpellingError(mousePosition);
                if (spellingError != null)
                {
                    foreach (string suggestion in spellingError.Suggestions)
                    {
                        MenuItem menuItem = new MenuItem();
                        menuItem.Header = suggestion;
                        menuItem.FontWeight = FontWeights.Bold;
                        menuItem.Command = EditingCommands.CorrectSpellingError;
                        menuItem.CommandParameter = suggestion;
                        menuItem.CommandTarget = _myRichTextBox;
                        this.Items.Add(menuItem);
                    }

                    this.AddSeperator();
                    MenuItem ignoreAllMenuItem = new MenuItem();
                    ignoreAllMenuItem.Header = EditingCommands.IgnoreSpellingError.Text;
                    ignoreAllMenuItem.Command = EditingCommands.IgnoreSpellingError;
                    ignoreAllMenuItem.CommandTarget = _myRichTextBox;

                    this.AddSeperator();
                }
            }
        }

        // Helper to add dictionary menu items.
        private void AddEmailNameDictionaryMenuItems()
        {
            TextPointer mousePosition = this.GetMousePosition();
            if (mousePosition != null && mousePosition.Parent != null && mousePosition.Parent is Run)
            {
                Run run = (Run)mousePosition.Parent;
                if (_myRichTextBox.EmailNamesDictionary.ContainsKey(run.Text))
                {
                    MenuItem menuItem = new MenuItem();
                    menuItem.Header = _myRichTextBox.EmailNamesDictionary[run.Text];
                    this.Items.Add(menuItem);
                    this.AddSeperator();
                }
            }
        }

        // Helper which adds a seperator item.
        private void AddSeperator()
        {
            this.Items.Add(new Separator());
        }

        // Helper to get a TextPointer position from MyRichTextBox.
        private TextPointer GetMousePosition()
        {
            Point mousePoint = Mouse.GetPosition(_myRichTextBox);
            return _myRichTextBox.GetPositionFromPoint(mousePoint, /*snapToText*/true);
        }

        private readonly MyRichTextBox _myRichTextBox;
    }
}