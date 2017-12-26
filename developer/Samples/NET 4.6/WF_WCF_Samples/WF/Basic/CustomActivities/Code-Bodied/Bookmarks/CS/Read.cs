//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities;

namespace Microsoft.Samples.Bookmarks
{

    public sealed class Read<TResult> : NativeActivity<TResult>
    {
        public Read()
            : base()
        {
        }

        public string BookmarkName { get; set; }

        // Must return true for a NativeActivity that creates a bookmark
        protected override bool CanInduceIdle
        { 
            get { return true; } 
        } 

        protected override void Execute(NativeActivityContext context)
        {
            context.CreateBookmark(this.BookmarkName, new BookmarkCallback(this.Continue));
        }

        void Continue(NativeActivityContext context, Bookmark bookmark, object obj)
        {
            this.Result.Set(context, (TResult)obj); 
        }
    }
}
