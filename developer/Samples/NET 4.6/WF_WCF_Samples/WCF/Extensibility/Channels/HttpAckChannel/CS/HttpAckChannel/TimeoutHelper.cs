//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;

namespace Microsoft.Samples.HttpAckChannel
{
    struct TimeoutHelper
    {
        DateTime deadline;
        bool deadlineSet;
        TimeSpan originalTimeout;
        public static readonly TimeSpan MaxWait = TimeSpan.FromMilliseconds(Int32.MaxValue);

        public TimeoutHelper(TimeSpan timeout)
        {
            this.originalTimeout = timeout;
            this.deadline = DateTime.MaxValue;
            this.deadlineSet = (timeout == TimeSpan.MaxValue);
        }

        public TimeSpan RemainingTime()
        {
            if (!this.deadlineSet)
            {
                this.SetDeadline();
                return this.originalTimeout;
            }
            else if (this.deadline == DateTime.MaxValue)
            {
                return TimeSpan.MaxValue;
            }
            else
            {
                TimeSpan remaining = this.deadline - DateTime.UtcNow;
                if (remaining <= TimeSpan.Zero)
                {
                    return TimeSpan.Zero;
                }
                else
                {
                    return remaining;
                }
            }
        }

        void SetDeadline()
        {
            this.deadline = DateTime.UtcNow + this.originalTimeout;
            this.deadlineSet = true;
        }
    }
}
