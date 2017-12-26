//  Copyright (c) Microsoft Corporation. All rights reserved.

namespace Microsoft.Syndication.Samples
{
    class ItemCounter
    {
        int numItems;

        public int GetCount()
        {
            lock (this)
            {
                return numItems;
            }
        }

        public void Increment()
        {
            lock (this)
            {
                ++numItems;
            }
        }
    }
}
