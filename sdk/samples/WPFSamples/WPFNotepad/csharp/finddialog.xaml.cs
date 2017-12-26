/*******************************************************************************
 *
 * Copyright (c) 2004-2005 Microsoft Corporation.  All rights reserved.
 *
 * Description: The FindDialog class defines dialog UI for the find command
 *
 *******************************************************************************/


namespace Microsoft.Samples.WPFNotepad
{
    using System;
    using System.Windows;
    using System.Windows.Controls;

    public partial class FindDialog : Window
    {
        #region public Constructors

        //------------------------------------------------------
        //
        //  public Constructors
        //
        //------------------------------------------------------

        public FindDialog()
        {
            InitializeComponent();
            _findNext.IsEnabled = _replace.IsEnabled = _replaceAll.IsEnabled = !string.IsNullOrEmpty(_findWhat.Text);
        }
        
        #endregion public Constructors


        #region public Properties

        //------------------------------------------------------
        //
        //  public Properties
        //
        //------------------------------------------------------

        public string FindWhat
        {
            get { return _findWhat.Text; }
            set
              {
                  _findWhat.Text = value;
              }
        }
        public string ReplaceWith 
        { 
            get { return _replaceWith.Text; }
            set
            {
                _replaceWith.Text = value;
            }
        }
        public bool? MatchCase { get { return _matchCase.IsChecked; } }
        public bool? SearchUp { get { return _findUp.IsChecked; } }

        public bool ShowReplace {
            get { return _replaceWith.Visibility == Visibility.Visible; }
            set
            {
                Visibility toBe;
                if (value)
                {
                    toBe = Visibility.Visible;
                    _directionGroupBox.Visibility = Visibility.Collapsed;
                    _findDown.IsChecked = true;
                }
                else
                {
                    toBe = Visibility.Collapsed;
                    _directionGroupBox.Visibility = Visibility.Visible;
                }
                _replaceLabel.Visibility = _replaceWith.Visibility = _replace.Visibility = _replaceAll.Visibility = toBe;
            }
        }

        #endregion public Properties


        #region public Events
    
        //------------------------------------------------------
        //
        //  public Events
        //
        //------------------------------------------------------

        public event EventHandler FindNext;
        public event EventHandler Replace;
        public event EventHandler ReplaceAll;

        #endregion public Events


        #region private Methods

        //------------------------------------------------------
        //
        //  private Methods
        //
        //------------------------------------------------------

        void FindNextClicked(object sender, RoutedEventArgs e)
        {
            if(FindNext != null)
            {
                FindNext(this, EventArgs.Empty);
            }
        }

        void ReplaceClicked(object sender, RoutedEventArgs e)
        {
            if(Replace != null)
            {
                Replace(this, EventArgs.Empty);
            }
        }
        
        void ReplaceAllClicked(object sender, RoutedEventArgs e)
        {
            if(ReplaceAll != null)
            {
                ReplaceAll(this, EventArgs.Empty);
            }
        }
        
        void CancelClicked(object sender, RoutedEventArgs e)
        {
            Close();
        }

        void OnActivated(object sender, EventArgs e)
        {
            _findWhat.Focus();
        }

        void FindTextChanged(object sender, TextChangedEventArgs e)
        {
            _findNext.IsEnabled = _replace.IsEnabled = _replaceAll.IsEnabled =!string.IsNullOrEmpty(_findWhat.Text);
        }

        #endregion private Methods

    }
}
