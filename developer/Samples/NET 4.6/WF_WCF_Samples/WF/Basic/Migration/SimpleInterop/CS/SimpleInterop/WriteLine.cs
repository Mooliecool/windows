//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Workflow.ComponentModel;

namespace Microsoft.Samples.WF.Migration
{
    public class WriteLine : Activity
    {
        public WriteLine()
            : base() { }

        public static readonly DependencyProperty TextProperty =
            DependencyProperty.Register("Text", typeof(string), typeof(WriteLine));

        public string Text
        {
            get
            {
                return (string) base.GetValue(TextProperty);
            }
            set
            {
                base.SetValue(TextProperty, value);
            }
        }

        protected override ActivityExecutionStatus Execute(ActivityExecutionContext context)
        {
            Console.WriteLine(this.Text);
            return ActivityExecutionStatus.Closed;
        }
    }
}
