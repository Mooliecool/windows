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
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.ComponentModel.Design;

namespace Microsoft.Samples.Workflow.SimpleActivity
{
    [ToolboxItemAttribute(typeof(ActivityToolboxItem))]
    public partial class SendEmailActivity : System.Workflow.ComponentModel.Activity
    {
        public SendEmailActivity()
        {
            InitializeComponent();
        }

        // override Execute with your custom logic
        protected override ActivityExecutionStatus Execute(ActivityExecutionContext context)
        {
            // Create an instance of CustomActivityEventArgs
            CustomActivityEventArgs customActivityEventArgs = new CustomActivityEventArgs(this.Description);
            // raise the SendingEvent event and pass customActivityEventArgs 
            this.RaiseGenericEvent<CustomActivityEventArgs>(SendingEvent, this, customActivityEventArgs);

            // This is where the logic of the e-mail should go
            Console.WriteLine("The Logic to send the e-mail goes here");
            Console.WriteLine("The 'To' property is: " + this.To.ToString());
            Console.WriteLine("The 'From' property is: " + this.From.ToString());

            return ActivityExecutionStatus.Closed;
        }

        public static DependencyProperty ToProperty = DependencyProperty.Register("To", typeof(System.String), typeof(SendEmailActivity));

        [DescriptionAttribute("Please specify the email address of the receipent ")]
        [DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)]
        [BrowsableAttribute(true)]
        public string To
        {
            get
            {
                return ((String)(base.GetValue(SendEmailActivity.ToProperty)));
            }
            set
            {
                base.SetValue(SendEmailActivity.ToProperty, value);
            }
        }

        public static DependencyProperty FromProperty = DependencyProperty.Register("From", typeof(System.String), typeof(SendEmailActivity));

        [DescriptionAttribute("Please specify the email address of the sender")]
        [DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)]
        [BrowsableAttribute(true)]
        public string From
        {
            get
            {
                return ((String)(base.GetValue(SendEmailActivity.FromProperty)));
            }
            set
            {
                base.SetValue(SendEmailActivity.FromProperty, value);
            }
        }

        // Create a DependencyProperty SendingEvent and the BeforeSend event handler
        public static DependencyProperty SendingEvent = DependencyProperty.Register("Sending", typeof(EventHandler<CustomActivityEventArgs>), typeof(SendEmailActivity));
        [DescriptionAttribute("Use this Handler to do some pre-processing logic ")]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [BrowsableAttribute(true)]
        [Category("Handlers")]
        public event EventHandler<CustomActivityEventArgs> Sending
        {
            add
            {
                base.AddHandler(SendEmailActivity.SendingEvent, value);
            }
            remove
            {
                base.RemoveHandler(SendEmailActivity.SendingEvent, value);
            }
        }
    }

    // Create a CustomActivityEventArgs 
    public class CustomActivityEventArgs : EventArgs
    {
        private string activityDescriptionValue;

        public string ActivityDescription
        {
            get { return activityDescriptionValue; }
            set { activityDescriptionValue = value; }
        }
	
        public CustomActivityEventArgs(string activityDescription)
        {
            this.ActivityDescription = activityDescription;
        }
    }
}
