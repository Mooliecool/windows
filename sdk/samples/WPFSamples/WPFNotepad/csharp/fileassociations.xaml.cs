/*******************************************************************************
 *
 * Copyright (c) 2004-2006 Microsoft Corporation.  All rights reserved.
 *
 * Description: The FileAssociationsDialog class allows the user to manage
 * their file associations for text -- allowing them to turn on and off the
 * default usage of WPFNotepad in place of regular Notepad.
 *
 *******************************************************************************/


namespace Microsoft.Samples.WPFNotepad
{
    using System;
    using System.Windows;
    using System.Windows.Controls;
    using System.Diagnostics;
    using System.IO;
    using Microsoft.Win32;

    public partial class FileAssociationsDialog : Window
    {
        #region public Constructors

        //------------------------------------------------------
        //
        //  public Constructors
        //
        //------------------------------------------------------

        public FileAssociationsDialog()
        {
            InitializeComponent();
        }

        #endregion public Constructors


        #region private Methods

        //------------------------------------------------------
        //
        //  private Methods
        //
        //------------------------------------------------------

        void this_Loaded(object sender, EventArgs args)
        {
            switch (QueryWhichNotepadIsConfiguredInRegistry())
            {
                case WPF_NOTEPAD_EXE:
                    _SystemNotepadRadioButton.IsSelected = false;
                    _WPFNotepadRadioButton.IsSelected = true;
                    break;
                case NOTEPAD_EXE:
                default:
                    _WPFNotepadRadioButton.IsSelected = false;
                    _SystemNotepadRadioButton.IsSelected = true;
                    break;
            }
        }

        void OKButton_Click(object sender, RoutedEventArgs e)
        {
            if ((bool)_WPFNotepadRadioButton.IsSelected)
            {
                SetWPFNotepadAssociations();
            }
            else if ((bool)_SystemNotepadRadioButton.IsSelected)
            {
                SetSystemNotepadAssociations();
            }
            this.Close();
        }

        void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        void SetWPFNotepadAssociations()
        {
            /*
            Set this string value in the registry
             HKEY_CURRENT_USER\software\microsoft\windows\currentversion\explorer\fileexts\.txt\Application = WPFnotepad.exe
             */

            SetRegistryKeys(WPF_NOTEPAD_EXE);
        }

        void SetSystemNotepadAssociations()
        {
            /*
            Set this string value in the registry
             HKEY_CURRENT_USER\software\microsoft\windows\currentversion\explorer\fileexts\.txt\Application = notepad.exe
             */

            SetRegistryKeys(NOTEPAD_EXE);
        }

        void SetRegistryKeys(string appName)
        {
            string fullPathName = NOTEPAD_EXE;

            if (appName == WPF_NOTEPAD_EXE)
            {
                fullPathName = System.Reflection.Assembly.GetExecutingAssembly().Location;
            }

            try
            {
                // HKEY_CURRENT_USER\software\microsoft\windows\currentversion\explorer\fileexts\.txt
                using (RegistryKey key = Registry.CurrentUser.CreateSubKey(@"software\microsoft\windows\currentversion\explorer\fileexts\.txt"))
                {
                    if (key != null)
                    {
                        key.SetValue(APPLICATION_STRING, appName);
                    }
                }
                // HKEY_CLASSES_ROOT\txtfile\shell\open\command
                using (RegistryKey key = Registry.ClassesRoot.CreateSubKey(@"txtfile\shell\open\command"))
                {
                    if (key != null)
                    {
                        key.SetValue("", fullPathName + " %1");
                    }
                }
                // HKEY_LOCAL_MACHINE\SOFTWARE\Classes\txtfile\shell\open\command
                using (RegistryKey key = Registry.LocalMachine.CreateSubKey(@"SOFTWARE\Classes\txtfile\shell\open\command"))
                {
                    if (key != null)
                    {
                        key.SetValue("", fullPathName + " %1");
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("WPFNotepad was unable to save your preference: " + ex.Message);
            }
        }

        string QueryWhichNotepadIsConfiguredInRegistry()
        {
            string location;
            RegistryKey key = Registry.CurrentUser.OpenSubKey(@"software\microsoft\windows\currentversion\explorer\fileexts\.txt");
            if (key != null)
            {
                location = key.GetValue(APPLICATION_STRING) as string;
                if (location != null)
                {
                    if (String.Compare(location, WPF_NOTEPAD_EXE, true) == 0)
                    {
                        return WPF_NOTEPAD_EXE;
                    }
                }
            }
            return NOTEPAD_EXE;
        }


        #endregion private Methods

        #region private fields

        private const string WPF_NOTEPAD_EXE = "WPFNotepad.exe";
        private const string NOTEPAD_EXE = "Notepad.exe";
        private const string APPLICATION_STRING = "Application";

        #endregion
    }
}
