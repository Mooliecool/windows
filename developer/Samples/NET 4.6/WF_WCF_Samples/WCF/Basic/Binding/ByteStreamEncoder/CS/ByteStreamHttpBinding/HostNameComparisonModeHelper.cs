//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.ComponentModel;
using System.ServiceModel;

namespace Microsoft.Samples.ByteStreamEncoder
{

    static class HostNameComparisonModeHelper
    {
        public static void Validate(HostNameComparisonMode value)
        {
            if (!IsDefined(value))
            {
                throw(new InvalidEnumArgumentException("value", (int)value,
                    typeof(HostNameComparisonMode)));
            }
        }
        internal static bool IsDefined(HostNameComparisonMode value)
        {
            return
                value == HostNameComparisonMode.StrongWildcard
                || value == HostNameComparisonMode.Exact
                || value == HostNameComparisonMode.WeakWildcard;
        }
    }
}
