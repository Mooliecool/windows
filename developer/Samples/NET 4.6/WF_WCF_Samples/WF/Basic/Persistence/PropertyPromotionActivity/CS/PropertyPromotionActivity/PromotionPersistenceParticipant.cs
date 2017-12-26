//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities.Persistence;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Microsoft.Samples.PropertyPromotionActivity
{
    class PromotionPersistenceParticipant : PersistenceParticipant
    {
        public static XNamespace PromotedPropertyXNamespace = XNamespace.Get("http://schemas.microsoft.com/framework/samples/persistence/SqlWorkflowInstanceStorePromotions");
        Dictionary<XName, object> promotedProperties;

        public PromotionPersistenceParticipant()
        {
            this.promotedProperties = new Dictionary<XName, object>();
        }

        public void AddPromotedVariable(string variableName, object variableValue)
        {
            this.promotedProperties[PromotedPropertyXNamespace.GetName(variableName)] = variableValue;
        }

        public void ClearExistingPromotionData()
        {
            this.promotedProperties.Clear();
        }

        protected override void CollectValues(out IDictionary<XName, object> readWriteValues, out IDictionary<XName, object> writeOnlyValues)
        {
            readWriteValues = null;
            writeOnlyValues = this.promotedProperties;
        }
    }
}
