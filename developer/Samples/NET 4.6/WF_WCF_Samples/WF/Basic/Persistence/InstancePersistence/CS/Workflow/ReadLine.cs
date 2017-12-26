//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;

namespace Microsoft.Samples.Activities
{

    public sealed class ReadLine: NativeActivity<string>
    {
        public ReadLine()
        {
        }
        
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

            if (name == null)
            {
                throw new ArgumentException(string.Format("ReadLine {0}: BookmarkName cannot be null", this.DisplayName), "BookmarkName");
            }

            context.CreateBookmark(name, new BookmarkCallback(OnReadComplete));
        }

        void OnReadComplete(NativeActivityContext context, Bookmark bookmark, object state)
        {
            string input = state as string; 
            
            if (input == null)
            {
                throw new ArgumentException(string.Format("ReadLine {0}: ReadLine must be resumed with a non-null string"), "state");
            }

            context.SetValue(base.Result, input);
        }
    }
}

