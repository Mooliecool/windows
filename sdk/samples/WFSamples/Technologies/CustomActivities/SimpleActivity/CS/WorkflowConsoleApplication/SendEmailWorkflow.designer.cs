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
using System.ComponentModel;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.SimpleActivity
{
    public sealed partial class SendEmailWorkflow
    {
        #region Designer generated code
        
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            this.sendEmailActivity1 = new Microsoft.Samples.Workflow.SimpleActivity.SendEmailActivity();
            // 
            // sendEmailActivity1
            // 
            this.sendEmailActivity1.Description = "Send Email Activity";
            this.sendEmailActivity1.From = "sender@contoso.com";
            this.sendEmailActivity1.Name = "sendEmailActivity1";
            this.sendEmailActivity1.To = "recipient@fabrikam.com";
            this.sendEmailActivity1.Sending += new EventHandler<CustomActivityEventArgs>(this.OnBeforeSend);
            // 
            // SendEmailWorkflow
            // 
            this.Activities.Add(this.sendEmailActivity1);
            this.Name = "SendEmailWorkflow";
            this.CanModifyActivities = false;

        }

        #endregion

        private SendEmailActivity sendEmailActivity1;

    }
}
