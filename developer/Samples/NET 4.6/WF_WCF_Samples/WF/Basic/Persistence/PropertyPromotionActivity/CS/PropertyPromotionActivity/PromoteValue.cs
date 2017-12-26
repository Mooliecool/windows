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
    public class PromoteValue<T> : CodeActivity
    {
        public PromoteValue()
        {
        }

        public bool ClearExistingPromotedData { get; set; }
        public string Name { get; set; }
        public InArgument<T> Value { get; set; }

        protected override void Execute(CodeActivityContext context)
        {
            PromotionPersistenceParticipant persistenceParticipant = context.GetExtension<PromotionPersistenceParticipant>();

            if (persistenceParticipant != null && this.Name != null && this.Value != null)
            {
                if (this.ClearExistingPromotedData)
                {
                    persistenceParticipant.ClearExistingPromotionData();
                }

                string propertyName = this.Name;
                T propertyValue = context.GetValue<T>(this.Value);
                persistenceParticipant.AddPromotedVariable(propertyName, propertyValue);
            }
        }
    }
}
