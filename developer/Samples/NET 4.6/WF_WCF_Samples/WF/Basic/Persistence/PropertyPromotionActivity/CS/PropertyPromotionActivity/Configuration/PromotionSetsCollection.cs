//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Text;

namespace Microsoft.Samples.PropertyPromotionActivity.Configuration
{
    class PromotionSetsCollection : ConfigurationElementCollection
    {
        public override ConfigurationElementCollectionType CollectionType
        {
            get
            {
                return ConfigurationElementCollectionType.BasicMap;
            }
        }

        protected override string ElementName
        {
            get
            {
                return "promotionSet";
            }
        }

        protected override ConfigurationElement CreateNewElement()
        {
            return new PromotionSet();
        }

        protected override object GetElementKey(ConfigurationElement element)
        {
            return ((PromotionSet) element).Name;
        }
    }
}
