//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Microsoft.Samples.PropertyPromotionActivity
{
    // Note: This activity cannot be configured in the WF Designer. If you want to use the WF Designer,
    //       please use PromoteValue instead.
    public class PromoteValues : CodeActivity
    {
        public PromoteValues()
        {
            this.ValuesToPromote = new Dictionary<string, InArgument>();
        }

        public bool ClearExistingPromotedData { get; set; }
        public IDictionary<string, InArgument> ValuesToPromote { get; set; }

        protected override void Execute(CodeActivityContext context)
        {
            PromotionPersistenceParticipant persistenceParticipant = context.GetExtension<PromotionPersistenceParticipant>();

            if (persistenceParticipant != null)
            {
                if (this.ClearExistingPromotedData)
                {
                    persistenceParticipant.ClearExistingPromotionData();
                }

                foreach (KeyValuePair<string, InArgument> promotedProperty in ValuesToPromote)
                {
                    string propertyName = promotedProperty.Key;
                    object propertyValue = promotedProperty.Value != null ? promotedProperty.Value.Get(context) : null;
                    persistenceParticipant.AddPromotedVariable(propertyName, propertyValue);
                }
            }
        }
    }
}
