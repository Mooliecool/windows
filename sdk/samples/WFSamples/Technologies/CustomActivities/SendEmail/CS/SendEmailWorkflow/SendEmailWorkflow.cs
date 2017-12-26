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
using System.Net.Mail;
using System.Globalization;
using System.ComponentModel;
using System.DirectoryServices;
using System.DirectoryServices.ActiveDirectory;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.SendEmail
{
    public sealed partial class SendEmailWorkflow : SequentialWorkflowActivity
    {
        public SendEmailWorkflow()
        {
            InitializeComponent();
        }

        private void sendEmail_SentEmail(object sender, EventArgs e)
        {
            Console.WriteLine("The email has been sent.");
        }

        private void sendEmail_SendingEmail(object sender, EventArgs e)
        {
             Console.WriteLine("Preparing to send email.");
        }
    }
}
