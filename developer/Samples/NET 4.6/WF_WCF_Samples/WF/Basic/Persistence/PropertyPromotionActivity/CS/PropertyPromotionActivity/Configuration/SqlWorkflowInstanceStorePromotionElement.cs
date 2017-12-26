//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.ServiceModel.Activities.Configuration;
using System.ServiceModel.Activities.Description;
using System.Text;

namespace Microsoft.Samples.PropertyPromotionActivity.Configuration
{
    class SqlWorkflowInstanceStorePromotionElement : SqlWorkflowInstanceStoreElement
    {
        public override Type BehaviorType
        {
            get { return typeof(SqlWorkflowInstanceStorePromotionBehavior); }
        }

        [ConfigurationProperty("promotionSets", IsDefaultCollection = false)]
        public PromotionSetsCollection PromotionSets
        {
            get
            {
                return (PromotionSetsCollection) base["promotionSets"];
            }
        }

        protected override object CreateBehavior()
        {
            SqlWorkflowInstanceStorePromotionBehavior behavior = new SqlWorkflowInstanceStorePromotionBehavior((SqlWorkflowInstanceStoreBehavior) base.CreateBehavior());

            foreach (PromotionSet promotionSet in this.PromotionSets)
            {
                List<string> sqlVariantProperties = new List<string>();
                List<string> binaryProperties = new List<string>();

                foreach (PromotedValueElement promotedValue in promotionSet)
                {
                    (promotedValue.IsSqlVariant ? sqlVariantProperties : binaryProperties).Add(promotedValue.PropertyName);
                }

                behavior.Promote(promotionSet.Name, sqlVariantProperties, binaryProperties);
            }

            return behavior;
        }
    }
}
