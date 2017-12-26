//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System.Activities;
using System.ComponentModel;
using System.Windows.Markup;
using Microsoft.Samples.Activities.Statements.Design;

namespace Microsoft.Samples.Activities.Statements
{

    [Designer(typeof(CommentOutDesigner))]
    [ContentProperty("Body")]
    public sealed class CommentOut : CodeActivity
    {
        [DefaultValue(null)]
        public Activity Body { get; set; }

        protected override void Execute(CodeActivityContext context)
        {
            // This is an empty method because this activity is meant to "comment" other activities out,
            // so it intentionally does nothing at execution time.
        }
    }
}
