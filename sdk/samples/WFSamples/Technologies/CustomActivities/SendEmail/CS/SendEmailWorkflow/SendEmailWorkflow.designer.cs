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

namespace Microsoft.Samples.Workflow.SendEmail
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
            this.sendEmail = new Microsoft.Samples.Workflow.SendEmail.SendEmailActivity();
            // 
            // sendEmail
            // 
            this.sendEmail.Bcc = null;
            this.sendEmail.Body = null;
            this.sendEmail.CC = null;
            this.sendEmail.From = "someone@example.com";
            this.sendEmail.Name = "sendEmail";
            this.sendEmail.Port = 25;
            this.sendEmail.ReplyTo = null;
            this.sendEmail.SmtpHost = "localhost";
            this.sendEmail.Subject = "Test Email from Workflow";
            this.sendEmail.To = "someone@example.com";
            this.sendEmail.SendingEmail += new System.EventHandler(this.sendEmail_SendingEmail);
            this.sendEmail.SentEmail += new System.EventHandler(this.sendEmail_SentEmail);
            // 
            // SendEmailWorkflow
            // 
            this.Activities.Add(this.sendEmail);
            this.Name = "SendEmailWorkflow";
            this.CanModifyActivities = false;

        }

        #endregion

        private SendEmailActivity sendEmail;






















    }
}
