//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

namespace Microsoft.Samples.EmulatingBreakInWhile
{
    public class Vendor
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public int Reliability { get; set; }

        public override string ToString()
        {
            return string.Format("{0} (Reliability: {1})", this.Name, this.Reliability);
        }
    } 
}
