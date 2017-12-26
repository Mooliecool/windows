//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Activities.Hosting;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Threading;
using Microsoft.Win32;

namespace Microsoft.Samples.WorkflowApplicationReadLineHost
{

    public partial class HostView : Window, IHostView
    {
        List<WorkflowApplicationInfo> instanceInfos;
        WorkflowApplicationManager manager;
        WindowTextWriter outputWriter;
        WindowTextWriter errorWriter;
        bool usePersistence;
        bool useActivityTracking;
        bool managerOpened;
        ICollectionView defaultView;

        public HostView()
        {
            this.instanceInfos = new List<WorkflowApplicationInfo>();
            InitializeComponent();
            this.outputWriter = new WindowTextWriter(this.outputTextBox);
            this.errorWriter = new WindowTextWriter(this.errorTextBox);
        }

        public TextWriter OutputWriter
        {
            get
            {
                return this.outputWriter;
            }
        }

        public TextWriter ErrorWriter
        {
            get
            {
                return this.errorWriter;
            }
        }

        public bool UsePersistence
        {
            get
            {
                return this.usePersistence;
            }
        }

        public bool UseActivityTracking
        {
            get
            {
                return this.useActivityTracking;
            }
        }

        public void Initialize(WorkflowApplicationManager manager)
        {
            this.manager = manager;
            this.manager.Open();
            this.DataContextChanged += new DependencyPropertyChangedEventHandler(OnDataContextChanged);
            this.defaultView = CollectionViewSource.GetDefaultView(this.instanceInfos);
            this.DataContext = this.instanceInfos;
            this.defaultView.CurrentChanged += new EventHandler(OnCurrentChanged);
        }

        public void ManagerOpened()
        {
            this.Dispatcher.BeginInvoke(
                new Action(() =>
                    {
                        this.managerOpened = true;
                        this.manager.LoadExistingInstances();
                        SetRunButtonState();
                    }));
        }

        public void UpdateInstances(List<WorkflowApplicationInfo> instanceInfos)
        {
            this.Dispatcher.BeginInvoke(
                new Action(() =>
                {
                    WorkflowApplicationInfo originalApplicationInfo = CurrentWorkflowApplicationInfo;
                    BookmarkInfo originalBookmarkInfo = this.bookmarksComboBox.SelectedItem as BookmarkInfo;

                    this.instanceInfos = instanceInfos;
                    this.DataContext = this.instanceInfos;

                    if (originalApplicationInfo != null)
                    {
                        originalApplicationInfo = this.instanceInfos.Find(applicationInfo => applicationInfo.Id == originalApplicationInfo.Id);
                        if (originalApplicationInfo != null && this.defaultView != null)
                        {
                            this.defaultView.MoveCurrentTo(originalApplicationInfo);
                        }
                        if (originalBookmarkInfo != null) 
                        {                            
                            BookmarkInfo newBookmarkInfo = this.bookmarksComboBox.Items.Cast<BookmarkInfo>().FirstOrDefault(
                                (bookmarkInfo) => bookmarkInfo.BookmarkName == originalBookmarkInfo.BookmarkName);
                            if (newBookmarkInfo != null)
                            {
                                this.bookmarksComboBox.SelectedItem = newBookmarkInfo;
                            }
                        }
                    }
                }), DispatcherPriority.DataBind);
        }

        public TextWriter CreateInstanceWriter()
        {
            return new WindowTextWriter(this.instanceOutputTextBox);
        }

        public void SelectInstance(Guid id)
        {
            this.Dispatcher.BeginInvoke(DispatcherPriority.DataBind, new Action(() =>
                {
                    //make the InstanceInfo with the matching id the current item                    
                    this.defaultView.MoveCurrentTo(this.instanceInfos.Find(applicationInfo => applicationInfo.Id == id));
                }));
        }

        public void Dispatch(Action work)
        {
            this.Dispatcher.BeginInvoke(work, DispatcherPriority.Background);
        }

        void SetUiState()
        {
            if (this.IsInitialized)
            {
                WorkflowApplicationInfo currentApplicationInfo = CurrentWorkflowApplicationInfo;
                bool currentApplicationInfoIsNull = currentApplicationInfo == null;
                bool currentApplicationInfoIsLoaded = !currentApplicationInfoIsNull && currentApplicationInfo.IsLoaded;

                this.abortButton.IsEnabled = currentApplicationInfoIsLoaded;
                this.cancelButton.IsEnabled = currentApplicationInfoIsLoaded;
                this.terminateButton.IsEnabled = currentApplicationInfoIsLoaded;
                SetResumeBookmarkButtonState();
            }
        }

        void SetResumeBookmarkButtonState()
        {
            if (this.IsInitialized)
            {
                this.resumeBookmarkButton.IsEnabled = (this.bookmarksComboBox.SelectedIndex != -1);
            }
        }

        void SetRunButtonState()
        {
            if (this.IsInitialized)
            {
                this.runButton.IsEnabled = this.managerOpened;
            }
        }

        WorkflowApplicationInfo CurrentWorkflowApplicationInfo
        {
            get
            {
                return CollectionViewSource.GetDefaultView(this.DataContext).CurrentItem as WorkflowApplicationInfo;
            }
        }

        protected override void OnActivated(EventArgs e)
        {
            if (this.manager == null)
            {
                throw new InvalidOperationException("The HostView must be initialized with a WorkflowApplicationManager before being activated.");
            }
            base.OnActivated(e);
        }

        protected override void OnClosed(EventArgs e)
        {
            this.manager.Close();
            this.outputWriter.Close();
            this.errorWriter.Close();
            base.OnClosed(e);
        }

        void OnDataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            this.defaultView.CurrentChanged -= new EventHandler(OnCurrentChanged);
            this.defaultView = CollectionViewSource.GetDefaultView(this.DataContext);
            this.defaultView.CurrentChanged += new EventHandler(OnCurrentChanged);
        }

        void OnCurrentChanged(object sender, EventArgs e)
        {
            WorkflowApplicationInfo currentApplicationInfo = CurrentWorkflowApplicationInfo;

            if (currentApplicationInfo != null)
            {
                this.instanceOutputTextBox.Text = currentApplicationInfo.InstanceWriter.ToString();
            }
            SetUiState();
        }

        void OnRunButtonClicked(object sender, RoutedEventArgs e)
        {
            string path = this.definitionPathTextBox.Text;
            if (string.IsNullOrEmpty(path))
            {
                this.ErrorWriter.WriteLine("Please specify a valid path to a xaml workflow definition.");
            }
            else
            {
                this.manager.Run(this.definitionPathTextBox.Text);
            }
        }

        void OnBrowseDefinitionsButtonClicked(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog
            {
                Filter = "Xaml files (*.xaml, *.xamlx)|*.xaml;*.xamlx",
                Multiselect = false,
                Title = "Open Workflow Definition"
            };
            Nullable<bool> fileSelected = openFileDialog.ShowDialog();
            if (fileSelected.HasValue && fileSelected.Value)
            {
                this.definitionPathTextBox.Text = openFileDialog.FileName;
            }
        }

        void OnCancelButtonClicked(object sender, RoutedEventArgs e)
        {
            WorkflowApplicationInfo applicationInfo = this.CurrentWorkflowApplicationInfo;
            this.manager.Cancel(applicationInfo.Id);
        }

        void OnTerminateButtonClicked(object sender, RoutedEventArgs e)
        {
            WorkflowApplicationInfo applicationInfo = this.CurrentWorkflowApplicationInfo;
            this.manager.Terminate(applicationInfo.Id, "User initiated Terminate");
        }

        void OnAbortButtonClicked(object sender, RoutedEventArgs e)
        {
            WorkflowApplicationInfo applicationInfo = this.CurrentWorkflowApplicationInfo;
            this.manager.Abort(applicationInfo.Id, "User requested Abort");
        }

        void OnResumeBookmarkButtonClicked(object sender, RoutedEventArgs e)
        {
            WorkflowApplicationInfo applicationInfo = this.CurrentWorkflowApplicationInfo;
            BookmarkInfo bookmarkInfo = this.bookmarksComboBox.SelectedItem as BookmarkInfo;

            if (applicationInfo != null && bookmarkInfo != null)
            {
                this.manager.ResumeBookmark(applicationInfo.Id, bookmarkInfo.BookmarkName, this.bookmarkValueTextBox.Text);
            }
            this.bookmarkValueTextBox.Text = string.Empty;
        }

        void UsePersistenceCheckBoxChecked(object sender, RoutedEventArgs e)
        {
            this.usePersistence = this.usePersistenceCheckBox.IsChecked.HasValue && this.usePersistenceCheckBox.IsChecked.Value;
        }

        void UseActivityTrackingCheckBoxChecked(object sender, RoutedEventArgs e)
        {
            this.useActivityTracking = this.useActivityTrackingCheckBox.IsChecked.HasValue && this.useActivityTrackingCheckBox.IsChecked.Value;
        }

        void OnBookmarksComboBoxSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            SetResumeBookmarkButtonState();
        }

        void OnBookmarkValueTextBoxTextChanged(object sender, TextChangedEventArgs e)
        {
            SetResumeBookmarkButtonState();
        }

        void OnScrollableTextChanged(object sender, TextChangedEventArgs e)
        {
            ((TextBox)sender).ScrollToEnd();
        }
    }
}
