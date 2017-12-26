using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Markup;
using System.Reflection;
using System.Windows.Input;
using System.Xml;
using System.Windows.Navigation; 
using System.Collections;
using System.IO; 


namespace EditingExaminer
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>

    public partial class MainWindow : Window
    {

        Hashtable _table;
        ArrayList _commandHistory;
        int _historyCounter;
        int _dotCounter = 0; 
        TextBox _inputTextBox; 

        public MainWindow()
        {
        }

        void WindowLoaded(object o, RoutedEventArgs args)
        {
            AddToolTips();
            _historyCounter = 0;
            _commandHistory = new ArrayList();
            UpdateDisplayTabs(null, null);
            MainEditor.Selection.Changed += new EventHandler(UpdateDisplayTabs);

             //Set up the Immediate window
            RefreshTable();
            CommandInputBox.PreviewKeyUp += new System.Windows.Input.KeyEventHandler(InputBox_PreviewKeyUp);
            CommandInputBox_FocusEvent(CommandInputBox, null);

         
            _inputTextBox = ReflectionUtils.GetProperty(CommandInputBox, "EditableTextBoxSite") as TextBox;
            _inputTextBox.TextChanged += new TextChangedEventHandler(_inputTextBox_TextChanged);
        }

        void RefreshTable()
        {
            if (_table == null)
            {
                _table = new Hashtable();
            }
            else
            {
                _table.Clear();
            }
            _table.Add("RichTextBox", MainEditor);
            _table.Add("Selection", MainEditor.Selection);
            _table.Add("Document", MainEditor.Document);
            _table.Add("null", null);
            _table.Add("True", true);
            _table.Add("False", false);
        }

        void w_LoadCompleted(object sender, NavigationEventArgs e)
        {
            NavigationWindow w = sender as NavigationWindow;
            if (w != null)
            {
                w.Height = 800;
                w.Width = 950;
            }
        }
       
        void _inputTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            //If the caret is at the beginning of the TextBox, move it to the end.
            if (_inputTextBox.SelectionStart == 0 && _inputTextBox.SelectionLength == 0 && _inputTextBox.Text.Length > 0)
            {
                _inputTextBox.SelectionStart = _inputTextBox.Text.Length;
            }
        }

        void InputBox_PreviewKeyUp(object sender, System.Windows.Input.KeyEventArgs e)
        {
            CommandLine commandLine;
            string command;
            try
            {
                switch(e.Key)
                {
                    case Key.Return:
                                         
                        command = CommandInputBox.Text;
                        if (command.Length == 0)
                        {
                            return; 
                        }
                        for (int i = 0; i < _commandHistory.Count; i++ )
                        {
                            if (command == (string)_commandHistory[i])
                            {
                                _commandHistory.RemoveAt(i);
                                i = _commandHistory.Count;
                            }
                        }
                        _commandHistory.Insert(0, command);
                        commandLine = new CommandLine(command.Replace("\r\n", ""), _table);
                        command += "\r\n[" + commandLine.Result.ToString() + "]\r\n";
                        ImmediateWindow.Text += command;
                        ImmediateWindow.ScrollToEnd();
                        CommandInputBox.Text = "";                     
                        break;
                    case Key.Up:
                        if (_historyCounter < _commandHistory.Count)
                        {
                            CommandInputBox.Text = _commandHistory[_historyCounter] as string;
                            _historyCounter++;
                        }
                        break;
                    case  Key.Down:
                        if (_historyCounter > 0)
                        {
                            _historyCounter--;
                            CommandInputBox.Text = _commandHistory[_historyCounter] as string;
                        }
                        break;
                    default:
                     
                        _historyCounter=0;
                        break;
                }
                //If the count of Dots in the command line changed, perform a new search for members.
                //Note: the ComboBox is using the up/down key to search for the previous/next item. 
                if (!(e.Key == Key.Down || e.Key == Key.Up))
                {
                    CheckDots();
                }
                OnError(null);
            }
            catch (Exception exception)
            {
                OnError(exception);
            }
        }

        void CheckDots()
        {
            int currentDotcounter; 
            string str = CommandInputBox.Text;
            currentDotcounter = 0;
        
            //Count the currently dot 
            while (str.Contains("."))
            {
                currentDotcounter++;
                str = str.Substring(str.IndexOf(".") + 1);
            }
            //if Dot count changed, perfrom a new search
            if (currentDotcounter != _dotCounter)
            {
                _dotCounter = currentDotcounter;
                if (currentDotcounter == 0)
                {
                    //no dot
                    SetDefaultItems();
                }
                else
                {
                    //more than one dots
                    SearchForMembers();
                }
            }
        }
        void SearchForMembers()
        {
            Type type;
            PropertyInfo[] pInfo;
            MethodInfo[] mInfos; 
            FieldInfo [] fInfos;
            BindingFlags bindingAttr;
            ArrayList strList;
            string tempStr;
            bindingAttr = BindingFlags.Public 
                | BindingFlags.Instance 
                | BindingFlags.Static 
                | BindingFlags.FlattenHierarchy;
            string str = CommandInputBox.Text.Substring(0, CommandInputBox.Text.LastIndexOf("."));
            tempStr = str + ".";
            try
            {
                if (str.Contains("="))
                {
                    str = str.Substring(str.LastIndexOf("=") + 1);
                }
                if (str.Contains(","))
                {
                    str = str.Substring(str.LastIndexOf(",") + 1);
                }
                else if (str.Contains("("))
                {
                    str = str.Substring(str.LastIndexOf("(") + 1);
                }
                str = str.Trim();
                CommandLine commandline = new CommandLine(str, _table);

                if (commandline.Result.Value != null)
                {
                    type = commandline.Result.Value.GetType();
                }
                else
                {
                    type = ReflectionUtils.FindType(str);
                }

                if (type != null)
                {
                    strList = new ArrayList();
                    pInfo = type.GetProperties(bindingAttr);
                    str = CommandInputBox.Text;
                    CommandInputBox.Items.Clear();
                    CommandInputBox.Text = str;
                    _inputTextBox.SelectionStart = str.LastIndexOf(".") + 1;
                    _inputTextBox.SelectionLength = str.Length - str.LastIndexOf(".") - 1;
                    foreach (PropertyInfo info in pInfo)
                    {
                        if (!strList.Contains(info.Name))
                        {
                            strList.Add(info.Name);
                            AddComboBoxitem(tempStr + info.Name);
                        }
                    }
                    mInfos = type.GetMethods(bindingAttr);
                    foreach (MethodInfo minfo in mInfos)
                    {

                        if (!strList.Contains(minfo.Name))
                        {
                            strList.Add(minfo.Name);
                            AddComboBoxitem(tempStr + minfo.Name);
                        }
                    }
                    fInfos = type.GetFields(bindingAttr);
                    foreach (FieldInfo fInfo in fInfos)
                    {
                        if (!strList.Contains(fInfo.Name))
                        {
                            strList.Add(fInfo.Name);
                            AddComboBoxitem(tempStr + fInfo.Name);
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                OnError(ex);
            }
        }
        void AddComboBoxitem(string str)
        {
            ComboBoxItem item = new ComboBoxItem();
            item.Content = str;
            CommandInputBox.Items.Add(item);
        }
        void AddToolTips()
        {
            SetToolTip(MainEditor, "The RichTextBox for you to perform Editing!");
            SetToolTip(ImmediateWindow, "Log for the previous commands and results!");
            SetToolTip(CommandInputBox, "TextBox for you to input your command!");
            SetToolTip(TextTreeView, "TreeView of the rich document!");
            SetToolTip(SelectionXaml, "XAML of the current selection!");
            SetToolTip(TextSerializedXaml, "XAML of the doucment created by editing serializer!");
            SetToolTip(CoreXaml, "XAML of the document created by the Core parser!");
            SetToolTip(ErrorMessageBox, "TextBox used to show errors!");
        }
       
        void CommandInputBox_FocusEvent(object sender, RoutedEventArgs e)
        {
            if (e != null && e.RoutedEvent.Name == "GotFocus")
            {

                CommandInputBox.Foreground = Brushes.Black;
                CommandInputBox.Text = "";
                CommandInputBox.Items.Clear();
                SetDefaultItems();
               
            }
            else
            {
                CommandInputBox.Foreground = Brushes.Blue;
                CommandInputBox.Text = "Type Command here!";
                CommandInputBox.Items.Clear();
            }
        }

        void SetDefaultItems()
        {
            ComboBoxItem item;
            IDictionaryEnumerator enumerator = _table.GetEnumerator();
            string str = CommandInputBox.Text;
            CommandInputBox.Items.Clear();
            CommandInputBox.Text = str;
            _inputTextBox.SelectionStart = str.Length;
            while (enumerator.MoveNext())
            {
                item = new ComboBoxItem();
                item.Content = enumerator.Key;
                CommandInputBox.Items.Add(item);
            }
        }
       
        void UpdateDisplayTabs(object o, object o1)
        {
            Exception exception=null;
            try
            {

                if (o == MainTab && MainTab.SelectedItem == PanelTab)
                {
                    TabControl.SelectedItem = TabCoreXaml;
                }

                 TabItem selectedItem = TabControl.SelectedItem as TabItem;
                 switch (selectedItem.Header.ToString())
                 {
                     case "CoreXaml":
                         TextRange range = new TextRange(CoreXaml.Document.ContentEnd, CoreXaml.Document.ContentStart);
                         if (((TabItem)MainTab.SelectedItem).Header.ToString() == "RichTextBox")
                         {
                            XamlHelper.TextRange_SetXml(range, XamlHelper.ColoringXaml(XamlHelper.IndentXaml(XamlWriter.Save(MainEditor.Document))));
                         }
                         else if (((TabItem)MainTab.SelectedItem).Header.ToString() == "Panel")
                         {
                             XamlHelper.TextRange_SetXml(range, XamlHelper.ColoringXaml(XamlHelper.IndentXaml(XamlWriter.Save(PanelTab.Content))));
                         }
                         break;
                     case "SelectionXaml":
                         SelectionXaml.Text = XamlHelper.IndentXaml(XamlHelper.TextRange_GetXml(MainEditor.Selection)); break;
                     case "TextSerializedXaml":
                         TextSerializedXaml.Text = XamlHelper.IndentXaml(XamlHelper.TextRange_GetXml(new TextRange(MainEditor.Document.ContentEnd, MainEditor.Document.ContentStart))); break;
                     case "DocumentTree":
                         TreeViewhelper.SetupTreeView(TextTreeView, MainEditor.Document); break;
                     default:
                         OnError(new Exception("Can't find specified TabItem!"));
                         break;
                 }
            }
            catch (Exception e)
            {
                exception = e;
            }
            if (exception == null)
            {
                OnError(null);
            }
            else
            {
                OnError(exception);
            }
         }

        void OnError(Exception e)
        {
            if (e != null)
            {
                ErrorMessageBox.Text = e.Message + "\r\n" + e.StackTrace;
            }
            else
            {
                ErrorMessageBox.Text = "No exception!";
            }

        }

        void SetToolTip(Control element, object tip)
        {
            if (tip == null)
            {
                tip = "No tip";
            }
            element.ToolTip = new ToolTip();
            element.ToolTip = tip;    
        }

        void ParsingXaml(object sender, RoutedEventArgs e)
        {
            MenuItem item = sender as MenuItem;
            try
            {
                string xaml=""; 
                switch (item.Header.ToString())
                {
                    case "SelectionXaml":
                        xaml =  XamlHelper.RemoveIndentation(TextSerializedXaml.Text); 
                        XamlHelper.TextRange_SetXml(MainEditor.Selection, xaml);
                        break;
                    case "TextSerializedXaml":
                        xaml =  XamlHelper.RemoveIndentation(TextSerializedXaml.Text); 
                        XamlHelper.TextRange_SetXml(new TextRange(MainEditor.Document.ContentEnd, MainEditor.Document.ContentStart), xaml);
                        break;
                    case "CoreXaml":
                        TextRange range = new TextRange(CoreXaml.Document.ContentEnd, CoreXaml.Document.ContentStart);
                        xaml = XamlHelper.RemoveIndentation(range.Text); 
                        if (((TabItem)MainTab.SelectedItem).Header.ToString() == "RichTextBox")
                        {
                           MainEditor.Document = (FlowDocument)XamlHelper.ParseXaml(xaml);
                           MainEditor.Selection.Changed += new EventHandler(UpdateDisplayTabs);
                           RefreshTable();
                        }
                        else if (((TabItem)MainTab.SelectedItem).Header.ToString() == "Panel")
                        {
                            ((TabItem)MainTab.SelectedItem).Content = XamlHelper.ParseXaml(xaml);
                            OnError(null);
                        }
                        break;
                    default:
                        OnError(new Exception("Don't know how to parse the xaml"));
                        break;
                }
            }
            catch (Exception exception)
            {
                OnError(exception);
            }

        }

        void window_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            try
            {
                TopPanel.Width = this.ActualWidth - 10;
                TopPanel.Height = this.ActualHeight - 36;
                MainTab.Width = TopPanel.Width;
                MainTab.Height = TopPanel.Height / 2;

                MainEditor.Width = RichTab.Width;
                MainEditor.Height = RichTab.Height ;

                ErrorMessageBox.Height = 60;
                Panel2.Width = TopPanel.Width;
                Panel2.Height = TopPanel.Height / 2 - ErrorMessageBox.Height;
                Panel3.Width = Panel2.Width * 0.45;
                TabControl.Height = Panel2.Height;
                TabControl.Width = Panel2.Width * 0.55;
                TextTreeView.Height = Panel2.Height - CommandInputBox.ActualHeight - 7;

                Panel3.Height = Panel2.Height;
                ImmediateWindow.Height = Panel3.Height - CommandInputBox.ActualHeight - Label1.ActualHeight;
                CommandInputBox.Width = Panel3.Width;
                ImmediateWindow.Width = Panel3.Width;
                Label1.Width = Panel3.Width;
            }
            catch (Exception exception)
            {
                OnError(exception);
            }
        }
    }
}