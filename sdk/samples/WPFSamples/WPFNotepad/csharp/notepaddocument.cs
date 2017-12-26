/*******************************************************************************
 *
 * Copyright (c) 2004-2005 Microsoft Corporation.  All rights reserved.
 *
 * Description: The NotepadDocument class manages file I/O and printing.
 *
 *******************************************************************************/


namespace Microsoft.Samples.WPFNotepad
{
    using System;
    using System.IO;
    using wf=System.Windows.Forms;
    using System.Windows;
    using System.Windows.Controls;
    using Microsoft.Win32;
    using System.Resources; 
    using System.Reflection;

    /*
        //namespaces added for Printing
        using System.Windows.Documents;
        using System.Windows.Printing;
        using System.Windows.Data;
        using System.Windows.Media;
    */

    class NotepadDocument
    {

        #region internal Constructors

        //------------------------------------------------------
        //
        //  internal Constructors
        //
        //------------------------------------------------------

        internal NotepadDocument()
        {
        }

        internal NotepadDocument(string physicalLocation)
        {
            PhysicalLocation = physicalLocation;
            LoadDocStream();
        }

        #endregion public Constructors


        #region public Properties

        //------------------------------------------------------
        //
        //  public Properties
        //
        //------------------------------------------------------

        public string FileName
        {
            get
            {
                if (string.IsNullOrEmpty(_physicalLocation))
                {
                    return Application.Current.Resources["untitled"].ToString();
                }
                else
                {
                    int startIndex = _physicalLocation.LastIndexOf('\\') + 1;
                    return _physicalLocation.Substring(startIndex);
                }
            }
        }

        public string DisplayName
        {
            get
            {
                if (string.IsNullOrEmpty(_physicalLocation))
                    return Application.Current.Resources["untitled"].ToString() + " " +Application.Current.Resources["ApplicationTitle"].ToString();
                else
                {
                    int startIndex = _physicalLocation.LastIndexOf('\\') + 1;
                    int endIndex;
                    using (RegistryKey filekey = Registry.CurrentUser.CreateSubKey(@"Software\Microsoft\Windows\CurrentVersion\Explorer\Advanced"))
                    {
                        if ((filekey != null) && (filekey.GetValue("HideFileExt", 0).ToString() == "0"))
                        {
                            endIndex = _physicalLocation.Length;
                        }
                        else
                        {
                            endIndex = _physicalLocation.LastIndexOf('.');
                        }
                    }
                    if (endIndex < startIndex)
                    {
                        return _physicalLocation.Substring(startIndex) + " " +Application.Current.Resources["ApplicationTitle"].ToString();
                    }
                    else // C:\Windows.Net    est.txt
                    {
                        return _physicalLocation.Substring(startIndex, endIndex - startIndex) + " " +Application.Current.Resources["ApplicationTitle"].ToString();
                    }
                }
 
            }
        }


        public string PhysicalLocation
        {
            get { return _physicalLocation; }
            set { _physicalLocation = value; }
        }

        public string Content
        {
            get { return _content; }
            set { _content = value; }
        }

        public bool IsDirty
        {
            get { return _isDirty; }
            set { _isDirty = value; }
        }

        public bool IsTextModeChanged
        {
            get { return _isTextModeChanged; }
            set { _isTextModeChanged = value; }
        }

        #endregion public Properties


        #region public Methods

        //------------------------------------------------------
        //
        //  public Methods
        //
        //------------------------------------------------------

        public bool Close()
        {
            bool result = true;
            if ((_isDirty)||(_isTextModeChanged))
            {
                string message = string.Format(_resourceManager.GetString("closemsg"),
                    (_physicalLocation.Length > 0 ? _physicalLocation : Application.Current.Resources["untitled"].ToString()));

                switch (MessageBox.Show(message, _resourceManager.GetString("notepad"), MessageBoxButton.YesNoCancel, MessageBoxImage.Exclamation))
                {
                    case MessageBoxResult.Yes:
                        if ((_physicalLocation.Length > 0)&&(!_isTextModeChanged))
                            result = Save();
                        else
                            result = SaveNew();
                        break;
                    case MessageBoxResult.No:
                        // do nothing...
                        break;
                    case MessageBoxResult.Cancel:
                        result = false;
                        break;
                }

                if (result)
                {
                    _isLoaded = false;
                    _isTextModeChanged = false;
                    _isDirty = false;
                    _content = "";
                }
            }
            return result;
        }

        public bool SaveNew()
        {
            wf.SaveFileDialog dlg = new wf.SaveFileDialog();
            SetApropriateDialogFilter(dlg);

            if (dlg.ShowDialog() == wf.DialogResult.OK)
            {
                try
                {
                    _physicalLocation = dlg.FileName;
                }
                catch (Exception e)
                {
                    MessageBox.Show(e.ToString());
                }

                return Save();
            }
            else return false;
        }

        public bool Save()
        {
            if (_physicalLocation.Length > 0)
            {
                try
                {
                    using (StreamWriter sw = new StreamWriter(_physicalLocation))
                    {
                        sw.Write(_content);
                        _isLoaded = true;
                        _isDirty = false;
                        return true;
                    }
                }

                // This catch handles the exception raised when trying to save a read only file
                catch (System.UnauthorizedAccessException e)
                {
                    string message = string.Format(_resourceManager.GetString("UnauthorizedAccessWarningMessage"),
                                                  PhysicalLocation);
                    MessageBox.Show(message, _resourceManager.GetString("notepad"), MessageBoxButton.OK, MessageBoxImage.Exclamation);
                    SaveNew();
                }

                catch (Exception e)
                {
                    MessageBox.Show(string.Format(_resourceManager.GetString("saveexception"),
                                                  PhysicalLocation, e.Message));
                }

            }
            else
            {
                return SaveNew();
            }
            return false;
        }

        public bool Open()
        {
            if (!_isLoaded)
            {
                if (PhysicalLocation.Length == 0)
                {
                    wf.OpenFileDialog dlg = new wf.OpenFileDialog();
                    SetApropriateDialogFilter(dlg);
                    if (!string.IsNullOrEmpty(_physicalLocation))
                    {
                        dlg.InitialDirectory = _physicalLocation.Substring(0, _physicalLocation.LastIndexOf('\\'));
                    }
                    else
                    {
                        dlg.InitialDirectory = Environment.GetEnvironmentVariable("SystemDrive") + "\\";
                    }
                    dlg.Multiselect = false;
                    dlg.CheckFileExists = true;

                    if (dlg.ShowDialog() == wf.DialogResult.OK)
                    {
                        _physicalLocation = dlg.FileName;
                    }
                    else
                    {
                        return false;
                    }
                }

                if (_physicalLocation.Length > 0)
                {
                    LoadDocStream();
                }
                else
                {
                    return false;
                }
            }
            _isLoaded = true;
            _isDirty = false;            
            return true;
        }

        #endregion public Methods


        #region private Methods

        //------------------------------------------------------
        //
        //  private Methods
        //
        //------------------------------------------------------

        void LoadDocStream()
        {
            string appTitle = _resourceManager.GetString("notepad");
            try
            {
                using (StreamReader sr = new StreamReader(_physicalLocation))
                {
                    _content = sr.ReadToEnd();
                }
                return;
            }
            catch (System.ArgumentException ae)
            {
                string message = _resourceManager.GetString("PathSyntaxError");
                MessageBox.Show(message, appTitle);
            }
            catch (DirectoryNotFoundException dnfe)
            {
                string message = _resourceManager.GetString("PathNotFoundError");
                MessageBox.Show(message, appTitle);
            }
            catch (DriveNotFoundException drivenotfoundexception)
            {
                string message = _resourceManager.GetString("PathNotFoundError");
                MessageBox.Show(message, appTitle);
            }
            catch (FileNotFoundException fnfe)
            {
                string message = _resourceManager.GetString("FileNotFoundWarning");
                message = String.Format(message, _physicalLocation);
                MessageBoxResult result = MessageBox.Show(message, appTitle, MessageBoxButton.YesNoCancel);
                if (result == MessageBoxResult.Yes)
                {
                    if (CreateNewFile(_physicalLocation))
                        return;
                }
            }
            catch (OutOfMemoryException oome)
            {
                MessageBox.Show("File too large for WPFNotepad to open");
                return;
            }

            //if we haven't returned yet, then there was a problem. Proceed with blank doc
            _physicalLocation = "";
        }

        bool CreateNewFile(string location)
        {
            try
            {
                File.CreateText(location);
                return true;
            }
            catch
            {
                return false;
            }
        }

        void SetApropriateDialogFilter(wf.FileDialog dlg)
        {
            switch (thisApp.CurrentEditingMode)
            {
                case EditingMode.Rich:
                    dlg.Filter = "Xaml Documents (*.xaml)|*.xaml|All Files|*.*";
                    dlg.DefaultExt = "xaml";
                    break;
                case EditingMode.Plain:
                    dlg.Filter = "Text Documents (*.txt)|*.txt|All Files|*.*";
                    dlg.DefaultExt = "txt";
                    break;
                default:
                    throw new NotImplementedException(@"Editing modes besides Plain & Rich are not supported.");
            }
        }

    #endregion private Methods


    #region private Fields

        //------------------------------------------------------
        //
        //  private Fields
        //
        //------------------------------------------------------

        string _physicalLocation = "";
        string _content;
        bool _isDirty;
        bool _isTextModeChanged;
        bool _isLoaded = false;
        NotepadApp thisApp = (NotepadApp)(Application.Current);
        ResourceManager _resourceManager = new ResourceManager("Microsoft.Samples.WPFNotepad.stringtable", Assembly.GetExecutingAssembly());


    #endregion private Fields
    }
}
