//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities;

namespace Microsoft.Samples.Activities.Statements
{

    public sealed class ReadLine : NativeActivity<string>
    {
        public string BookmarkName { get; set; }

        protected override void Execute(NativeActivityContext context)
        {
            context.CreateBookmark(this.BookmarkName, this.Continue);
        }

        protected override bool CanInduceIdle
        {
            get
            {
                return true;
            }
        }

        void Continue(NativeActivityContext context, Bookmark bookmark, object value)
        {
            if (value is string)
            {
                context.SetValue(this.Result, (string)value);
            }
        }
    }
}
