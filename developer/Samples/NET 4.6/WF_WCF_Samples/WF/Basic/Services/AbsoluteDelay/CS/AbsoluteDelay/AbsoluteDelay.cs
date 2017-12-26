//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;

namespace Microsoft.Samples.AbsoluteDelay
{
    // The AbsoluteDelay activity is similar to the Delay activity that ships with WF 4.0, except that it takes a DateTime that dictates
    // when the timer should expire and workflow execution resumed, instead of a relative TimeSpan. (note: you could easily get the same
    // behavior by using the Delay activity and writing an expression to do the conversion when setting the Duration InArgument, but this
    // activity is for illustrative purposes). 
    //
    // AbsoluteDelay must be a NativeActivity because it needs to create a bookmark to register with the timer extension
    // We are using a base class of NativeActivity<TResult> which will inherit an OutArgument of Result. This result is set after the timer has expired.
    // 
    public class AbsoluteDelay : NativeActivity<DateTime>
    {
        private Variable<Bookmark> timerExpiredBookmark;

        public AbsoluteDelay()
        {
            this.timerExpiredBookmark = new Variable<Bookmark>();
        }

        // The DateTime when the timer should be set to expire
        [RequiredArgument]
        public InArgument<DateTime> ExpirationTime { get; set; }

        // Any NativeActivity that registers bookmarks and can cause the WorkflowInstance 
        // to go idle, MUST override this property to return true.
        protected override bool CanInduceIdle
        {
            get
            {
                return true;
            }
        }

        protected override void CacheMetadata(NativeActivityMetadata metadata)
        {
            base.CacheMetadata(metadata);

            metadata.AddImplementationVariable(this.timerExpiredBookmark);

            // Because of this statement, it is not required to add the DurableTimerExtension to the WorkflowApplication.Extensions
            // collection to get durable timers support. It is a good idea to do this with activities that REQUIRE extensions to function properly. 
            metadata.AddDefaultExtensionProvider(new Func<TimerExtension>(() => { return new DurableTimerExtension(); }));
        }

        protected override void Execute(NativeActivityContext context)
        {
            DateTime expTime = this.ExpirationTime.Get(context);

            // DurableTimerExtension.Register takes a TimeSpan so we use this method to convert between
            // an the expired DateTime and how long of a delay to register for the timer
            TimeSpan delay = expTime - DateTime.Now;

            // If the input DateTime is already past due, then there is no need to register a timer.
            if (delay > TimeSpan.Zero)
            {
                // Get the timer extension. Since we are asking for a type of TimerExtension and not specifically DurableTimerExtension,
                // this activity could potentially be reused with a custom TimerExtension.
                TimerExtension timerExtension = context.GetExtension<TimerExtension>();

                // The bookmark that will be resumed when the timer expires
                Bookmark bookmark = context.CreateBookmark(new BookmarkCallback(OnTimerExpired));

                Console.WriteLine("AbsoluteDelay: Registering timer to expire at {0}", expTime.ToString());
                timerExtension.RegisterTimer(delay, bookmark);
            }
            else
            {
                Console.WriteLine("AbsoluteDelay: Requested expiry time is already past due, skipping timer registration");
            }
        }

        // The TimerExtensions support cancellation, so if this activity is canceled, propagate that signal to the TimerExtension
        protected override void Cancel(NativeActivityContext context)
        {
            base.Cancel(context);

            TimerExtension timerExtension = context.GetExtension<TimerExtension>();

            Bookmark bookmark = this.timerExpiredBookmark.Get(context);

            if (bookmark != null)
            {
                timerExtension.CancelTimer(bookmark);
            }
        }
       
        private void OnTimerExpired(NativeActivityContext context, Bookmark bookmark, Object value)
        {
            // The AbsoluteDelay activity returns the time at which the timer was expired (and workflow instance resumed)
            // so that the outer workflow can verify the correct behavior of the timer.
            this.Result.Set(context, DateTime.Now);
        }
    }
}
