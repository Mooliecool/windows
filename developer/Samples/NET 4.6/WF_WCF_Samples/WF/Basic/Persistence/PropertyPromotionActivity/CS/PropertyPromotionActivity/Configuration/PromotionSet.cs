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
    class PromotionSet : ConfigurationElementCollection
    {
        public override ConfigurationElementCollectionType CollectionType
        {
            get
            {
                return ConfigurationElementCollectionType.BasicMap;
            }
        }

        [ConfigurationProperty("name", IsKey = true, IsRequired = true)]
        public string Name
        {
            get
            {
                return (string) base["name"];
            }
            set
            {
                base["name"] = value;
            }
        }

        protected override string ElementName
        {
            get
            {
                return "promotedValue";
            }
        }

        protected override ConfigurationElement CreateNewElement()
        {
            return new PromotedValueElement();
        }

        protected override object GetElementKey(ConfigurationElement element)
        {
            return ((PromotedValueElement) element).PropertyName;
        }
    }
}
