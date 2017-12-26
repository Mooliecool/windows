//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;

namespace Microsoft.Samples.WorkflowApplicationReadLineHost
{

    public sealed class ReadLine : NativeActivity<string>
    {
        public ReadLine() 
        {
        }

        [RequiredArgument]
        public InArgument<string> BookmarkName { get; set; }

        protected override bool CanInduceIdle
        {
            get
            {
                return true;
            }
        }

        protected override void Execute(NativeActivityContext context)
        {
            string name = this.BookmarkName.Get(context);

            context.CreateBookmark(name, new BookmarkCallback(OnReadComplete));
        }

        void OnReadComplete(NativeActivityContext context, Bookmark bookmark, object state)
        {
            string input = state as string;

            context.SetValue(base.Result, input);
        }
    }
}
