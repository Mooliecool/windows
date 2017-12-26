/*******************************************************************************
 *
 * Copyright (c) 2004-2006 Microsoft Corporation.  All rights reserved.
 *
 * Description: The PrintProgressWindow class defines dialog UI for the WritingProgress dialog box
 *
 *******************************************************************************/

namespace Microsoft.Samples.WPFNotepad
{
    using System;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Documents.Serialization;
    using System.Resources;
    using System.Reflection;
    using System.Printing;
    using System.Windows.Xps;
    using System.Diagnostics;
    using System.ComponentModel;

    public partial class PrintProgressWindow : Window, INotifyPropertyChanged
    {
        
        #region public Constructors
        //------------------------------------------------------
        //
        //  public Constructors
        //
        //------------------------------------------------------
        public PrintProgressWindow(XpsDocumentWriter docWriter)
        {
            InitializeComponent();
            _docWriter = docWriter;
        }
        #endregion public Constructors

        #region public Events
        //------------------------------------------------------
        //
        //  public Events
        //
        //------------------------------------------------------
        public event EventHandler WritingProgressActivate;

        public void OnWritingProgressChanged(object sender, WritingProgressChangedEventArgs e)
        {
            Debug.WriteLine("OnWritingProgressChanged ProgressPercentage = " + ((Double)e.ProgressPercentage).ToString());
            Debug.WriteLine("OnWritingProgressChanged Number             = " + e.Number.ToString());
            WritingProgressPercentage = e.ProgressPercentage;
            WritingProgressPercentage = e.Number; 
            switch (e.WritingLevel)
            {
                case WritingProgressChangeLevel.FixedPageWritingProgress:
                    PageNumber = e.Number;
                    break;
                case WritingProgressChangeLevel.FixedDocumentWritingProgress:
                    this.Title = "Printing Complete";
                    break;
            }
        }

        public void OnWritingCompleted(object sender, WritingCompletedEventArgs e)
        {
            Debug.WriteLine("OnWritingCompleted fired. Cancelled = " + e.Cancelled.ToString());
            this.Close();
            Application.Current.MainWindow.Opacity = 1.0;
        }

        #endregion public Events

        #region public Properties
        //------------------------------------------------------
        //
        //  public Properties
        //
        //------------------------------------------------------
        public int PageNumber
        {
            get
            {
                return _pageNumber;
            }
            set
            {
                if (_pageNumber.Equals(value) == false)
                {
                    _pageNumber = value;
                    // Call OnPropertyChanged whenever the property is updated
                    OnPropertyChanged("PageNumber");
                }
            }
        }
        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(String info)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(info));
        }
        public int WritingProgressPercentage
        {
            get
            {
                return _writingProgressPercentage;
            }
            set
            {
                _writingProgressPercentage = value;
                _writingProgressBar.Value = _writingProgressPercentage;
                // Call OnPropertyChanged whenever the property is updated
                OnPropertyChanged("WritingProgressPercentage");
            }
        }

        #endregion public Properties

        #region private Methods
        //------------------------------------------------------
        //
        //  private Methods
        //
        //------------------------------------------------------
        void CancelClicked(object sender, RoutedEventArgs e)
        {
            try
            {
                Debug.Write("CancelAsync: ");
                _docWriter.CancelAsync();
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Failed");
                MessageBox.Show("Exception Cancelling Async printing: " + ex.ToString());
                return;
            }
            Debug.WriteLine("Successful");
            DialogResult = false;
            this.Close();
            Debug.WriteLine("Successfully closed PrintProgressWindow");
            Application.Current.MainWindow.Opacity = 1.0;
        }

        #endregion private Methods

        #region private Fields
        //------------------------------------------------------
        //
        //  private Fields
        //
        //------------------------------------------------------

        int _pageNumber = -1;
        int _writingProgressPercentage = -1;
        ResourceManager rm = new ResourceManager("Microsoft.Samples.WPFNotepad.stringtable", Assembly.GetExecutingAssembly());
        XpsDocumentWriter _docWriter = null;
        #endregion private Fields
    }
}
