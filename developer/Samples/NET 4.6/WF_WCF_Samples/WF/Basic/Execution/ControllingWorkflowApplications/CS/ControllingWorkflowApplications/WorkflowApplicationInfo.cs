//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Activities.Hosting;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;

namespace Microsoft.Samples.WorkflowApplicationReadLineHost
{

    //Facilitates binding workflow application information to the HostView
    public class WorkflowApplicationInfo : INotifyPropertyChanged
    {
        Guid id;
        bool isLoaded;
        ReadOnlyCollection<BookmarkInfo> availableBookmarks;
        bool canResumeBookmarks;
        TextWriter instanceWriter;

        public event PropertyChangedEventHandler PropertyChanged;

        public WorkflowApplicationInfo()
        {
            this.isLoaded = true;
        }

        public bool IsLoaded
        {
            get
            {
                return this.isLoaded;
            }
            set
            {
                this.isLoaded = value;
                OnPropertyChanged("IsLoaded");
            }
        }

        public ReadOnlyCollection<BookmarkInfo> AvailableBookmarks
        {
            get
            {
                return this.availableBookmarks;
            }
            set
            {
                this.availableBookmarks = value;
                OnPropertyChanged("AvailableBookmarks");
            }
        }

        public Guid Id
        {
            get
            {
                return this.id;
            }
            set
            {
                this.id = value;
                OnPropertyChanged("Id");
            }
        }

        public bool CanResumeBookmarks
        {
            get
            {
                return this.canResumeBookmarks;
            }
            set
            {
                this.canResumeBookmarks = value;
                OnPropertyChanged("CanResumeBookmarks");
            }
        }

        public TextWriter InstanceWriter
        {
            get
            {
                return this.instanceWriter;
            }
            set
            {
                this.instanceWriter = value;
                OnPropertyChanged("InstanceWriter");
            }
        }

        void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}
