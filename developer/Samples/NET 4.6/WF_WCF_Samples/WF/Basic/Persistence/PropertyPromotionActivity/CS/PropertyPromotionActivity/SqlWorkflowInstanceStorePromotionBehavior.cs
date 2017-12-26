//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Linq;
using System.ServiceModel.Activities.Description;
using System.ServiceModel.Description;
using System.Text;
using System.Xml.Linq;
using System.ServiceModel;
using System.ServiceModel.Activities;

namespace Microsoft.Samples.PropertyPromotionActivity
{
    public class SqlWorkflowInstanceStorePromotionBehavior : SqlWorkflowInstanceStoreBehavior, IServiceBehavior
    {
        public SqlWorkflowInstanceStorePromotionBehavior(string connectionString)
            : base(connectionString)
        {
        }

        internal SqlWorkflowInstanceStorePromotionBehavior(SqlWorkflowInstanceStoreBehavior sqlWorkflowInstanceStoreBehavior)
        {
            base.ConnectionString = sqlWorkflowInstanceStoreBehavior.ConnectionString;
            base.HostLockRenewalPeriod = sqlWorkflowInstanceStoreBehavior.HostLockRenewalPeriod;
            base.InstanceCompletionAction = sqlWorkflowInstanceStoreBehavior.InstanceCompletionAction;
            base.InstanceEncodingOption = sqlWorkflowInstanceStoreBehavior.InstanceEncodingOption;
            base.InstanceLockedExceptionAction = sqlWorkflowInstanceStoreBehavior.InstanceLockedExceptionAction;
            base.RunnableInstancesDetectionPeriod = sqlWorkflowInstanceStoreBehavior.RunnableInstancesDetectionPeriod;
        }

        public SqlWorkflowInstanceStorePromotionBehavior()
        {
        }

        public new void ApplyDispatchBehavior(ServiceDescription serviceDescription, ServiceHostBase serviceHostBase)
        {
            if (serviceHostBase == null)
            {
                throw new ArgumentNullException("serviceHostBase");
            }

            WorkflowServiceHost workflowServiceHost = serviceHostBase as WorkflowServiceHost;

            if (workflowServiceHost != null)
            {
                workflowServiceHost.WorkflowExtensions.Add<PromotionPersistenceParticipant>(() => new PromotionPersistenceParticipant());
            }

            base.ApplyDispatchBehavior(serviceDescription, serviceHostBase);
        }

        public void Promote(string name, IEnumerable<string> promoteAsSqlVariant, IEnumerable<string> promoteAsBinary)
        {
            XNamespace xNS = PromotionPersistenceParticipant.PromotedPropertyXNamespace;
            List<XName> sqlVariantPromotions = null;
            List<XName> binaryPromotions = null;

            if (promoteAsSqlVariant != null)
            {
                sqlVariantPromotions = new List<XName>();

                foreach (string propertyName in promoteAsSqlVariant)
                {
                    sqlVariantPromotions.Add(xNS.GetName(propertyName));
                }
            }

            if (promoteAsBinary != null)
            {
                binaryPromotions = new List<XName>();

                foreach (string propertyName in promoteAsBinary)
                {
                    binaryPromotions.Add(xNS.GetName(propertyName));
                }
            }

            base.Promote(name, sqlVariantPromotions, binaryPromotions);
        }
    }
}
