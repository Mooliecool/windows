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
using System.Workflow.ComponentModel;
using System.Windows.Forms;
using Outlook = Microsoft.Office.Interop.Outlook;

namespace Microsoft.Samples.Workflow.OutlookWorkflowWizard
{
    public partial class EvaluateOutboxItems : BaseMailbox
    {
        public EvaluateOutboxItems()
        {
            InitializeComponent();
        }

        protected override ActivityExecutionStatus Execute(ActivityExecutionContext context)
        {
            MessageBox.Show("Searching your Outbox");
            Outlook.Application outlookApp = new Outlook.Application();
            Outlook.MAPIFolder oSentItems = outlookApp.GetNamespace("MAPI").GetDefaultFolder(Outlook.OlDefaultFolders.olFolderOutbox);
            Outlook.Items oItems = oSentItems.Items;
            foreach (Object rawItem in oItems)
            {
                if (rawItem is Outlook.MailItem)
                {
                    Outlook.MailItem item = (Outlook.MailItem)rawItem;
                    switch (Filter)
                    {
                        case (FilterOption.Subject):
                            if ((item.Subject != null) && (item.Subject.Equals(FilterValue)))
                            {
                                MessageBox.Show("Found message with Subject filter value[" + FilterValue + "]:" + item.Body);
                                (this.Parent.Activities[1] as DummyActivity).TitleProperty = item.Subject;
                            }
                            break;
                        case (FilterOption.From):
                            if ((item.SenderEmailAddress != null) && (item.SenderEmailAddress.Equals(FilterValue)))
                            {
                                MessageBox.Show("Found message with From filter value[" + FilterValue + "]:" + item.Body);
                                (this.Parent.Activities[1] as DummyActivity).TitleProperty = item.Subject;
                            }
                            break;
                        case (FilterOption.To):
                            if ((item.To != null) && (item.To.Equals(FilterValue)))
                            {
                                MessageBox.Show("Found message with To filter value[" + FilterValue + "]:" + item.Body);
                                (this.Parent.Activities[1] as DummyActivity).TitleProperty = item.Subject;
                            }
                            break;
                        case (FilterOption.CC):
                            if ((item.CC != null) && (item.CC.Equals(FilterValue)))
                            {
                                MessageBox.Show("Found message with CC filter value[" + FilterValue + "]:" + item.Body);
                                (this.Parent.Activities[1] as DummyActivity).TitleProperty = item.Subject;
                            }
                            break;
                        case (FilterOption.Bcc):
                            if ((item.BCC != null) && (item.BCC.Equals(FilterValue)))
                            {
                                MessageBox.Show("Found message with BCC filter value[" + FilterValue + "]:" + item.Body);
                                (this.Parent.Activities[1] as DummyActivity).TitleProperty = item.Subject;
                            }
                            break;
                    }
                }
            }
            MessageBox.Show("Done with Execute in EvaluateOutboxItems");
            return ActivityExecutionStatus.Closed;
        }
    }
}
