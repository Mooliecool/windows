//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.ServiceModel.Channels;

namespace Microsoft.Samples.HttpAckChannel
{
    class ReceiveContextSettings : IReceiveContextSettings
    {
        public bool Enabled {get; set;}

        public TimeSpan ValidityDuration
        {
            get { return TimeSpan.FromMinutes(5); }
        }
    }
}
