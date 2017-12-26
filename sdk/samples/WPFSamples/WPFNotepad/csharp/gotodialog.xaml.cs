/*******************************************************************************
 *
 * Copyright (c) 2004-2005 Microsoft Corporation.  All rights reserved.
 *
 * Description: The GoToDialog class defines dialog UI for the GoTo dialog box
 *
 *******************************************************************************/


namespace Microsoft.Samples.WPFNotepad
{
    using System;
    using System.Windows;
    using System.Windows.Controls;
    using System.Resources;
    using System.Reflection;


    public partial class GoToDialog : Window
    {
        #region public Constructors

        //------------------------------------------------------
        //
        //  public Constructors
        //
        //------------------------------------------------------

        public GoToDialog()
        {
            InitializeComponent();
        }

        #endregion public Constructors


        #region public Events

        //------------------------------------------------------
        //
        //  public Events
        //
        //------------------------------------------------------

        public event EventHandler GotoActivate;

        #endregion public Events


        #region public Properties

        //------------------------------------------------------
        //
        //  public Properties
        //
        //------------------------------------------------------

        public int LineNumber
        {
            get
            {
                return _lineNumber;
            }
            set
            {
                _lineNumber = value;
                _lineNumberTextBox.Text = _lineNumber.ToString();
            }
        }

        public int MaxLineNumber
        {
            get
            {
                return _maxLineNumber;
            }
            set
            {
                _maxLineNumber = value;
            }
        }

        #endregion public Properties


        #region private Methods

        //------------------------------------------------------
        //
        //  private Methods
        //
        //------------------------------------------------------

        void OKClicked(object sender, RoutedEventArgs e)
        {
            if (string.IsNullOrEmpty(_lineNumberTextBox.Text))
            {
                MessageBox.Show(rm.GetString("GotoErrorMsg"), rm.GetString("GotoErrorDialogTitle"));
                return;
            }
            if (!int.TryParse(_lineNumberTextBox.Text, out _lineNumber))
            {
                MessageBox.Show(rm.GetString("GotoFormatErrorMsg"), rm.GetString("GotoErrorDialogTitle"));
                return;
            }
            if (LineNumber > _maxLineNumber || LineNumber <= 0)
            {
                MessageBox.Show(rm.GetString("GotoErrorMsg"), rm.GetString("GotoErrorDialogTitle"));
                return;
            }
            DialogResult = true;
            Close();
        }

        void CancelClicked(object sender, RoutedEventArgs e)
        {
            DialogResult = false;
            Close();
        }

        void OnActivated(object sender, EventArgs e)
        {
            if (GotoActivate != null)
                GotoActivate(this, EventArgs.Empty);
            _lineNumberTextBox.Focus();
            _lineNumberTextBox.SelectAll();
        }

        #endregion private Methods

        #region private Fields

        //------------------------------------------------------
        //
        //  private Fields
        //
        //------------------------------------------------------

        int _lineNumber = 0;
        int _maxLineNumber;
        ResourceManager rm = new ResourceManager("Microsoft.Samples.WPFNotepad.stringtable", Assembly.GetExecutingAssembly());


        #endregion private Fields
    }
}
