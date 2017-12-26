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
    class PromotedValueElement : ConfigurationElement
    {
        public PromotedValueElement()
        {
        }

        [ConfigurationProperty("isSqlVariant", IsKey = false, DefaultValue = true)]
        public bool IsSqlVariant
        {
            get
            {
                return (bool) this["isSqlVariant"];
            }
            set
            {
                this["isSqlVariant"] = value;
            }
        }

        [ConfigurationProperty("propertyName", IsRequired = true, IsKey = true)]
        public string PropertyName
        {
            get
            {
                return (string) this["propertyName"];
            }
            set
            {
                this["propertyName"] = value;
            }
        }
    }
}
