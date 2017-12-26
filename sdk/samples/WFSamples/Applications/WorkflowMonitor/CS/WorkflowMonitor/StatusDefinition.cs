//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------
using System;
using System.Windows.Forms;

namespace Microsoft.Samples.Workflow.WorkflowMonitor
{
    //Class to store workflow instance information - create one per workflow instance in the database
    internal class WorkflowStatusInfo
    {
        private string idValue;
        private string nameValue;
        private string statusValue;
        private string createdDateTimeValue;
        private Guid instanceIdValue;
        private ListViewItem listViewItemValue;

        internal WorkflowStatusInfo(string id, string name, string status, string createdDateTime, Guid instanceId, ListViewItem listViewItem)
        {
            this.idValue = id;
            this.nameValue = name;
            this.statusValue = status;
            this.createdDateTimeValue = createdDateTime;
            this.instanceIdValue = instanceId;
            this.listViewItemValue = listViewItem;
        }

        internal string Status
        {
            set { statusValue = value; }
        }

        internal ListViewItem WorkflowListViewItem
        {
            get { return listViewItemValue; }
        }

        internal Guid InstanceId
        {
            get { return instanceIdValue; }
        }
    }

    //Class to store activity information - create one per activity for the selected workflow
    internal class ActivityStatusInfo
    {
        private string nameValue;
        private string statusValue;

        internal ActivityStatusInfo(string name, string status)
        {
            this.nameValue = name;
            this.statusValue = status;
        }

        internal string Name
        {
            get { return nameValue; }
        }

        internal string Status
        {
            get { return statusValue; }
        }
    }
}
