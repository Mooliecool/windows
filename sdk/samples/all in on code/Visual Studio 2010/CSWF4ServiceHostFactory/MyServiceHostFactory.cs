/****************************** Module Header ******************************\
* Module Name:  MyServiceHostFactory.cs
* Project:		CSWF4ServiceHostFactory
* Copyright (c) Microsoft Corporation.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
\***************************************************************************/
using System;
using System.Collections.Generic;
using System.Workflow.Activities;
using System.Activities;
using System.Workflow.Runtime;
using System.ServiceModel.Activities.Activation;
using System.ServiceModel.Activities;
using System.ServiceModel.Description;
using System.Activities.DurableInstancing;
using System.Configuration;

namespace CSWF4ServiceHostFactory
{
    public class MyServiceHostFactory : 
            System.ServiceModel.Activities.Activation.WorkflowServiceHostFactory 
    {
        protected override WorkflowServiceHost CreateWorkflowServiceHost(WorkflowService service,
                                                                         Uri[] baseAddresses) 
        {
            WorkflowServiceHost host = 
                base.CreateWorkflowServiceHost(service, baseAddresses);
            string connectionString = 
                ConfigurationManager.AppSettings["SqlWF4PersistenceConnectionString"].ToString();
            SqlWorkflowInstanceStore instanceStore = 
                new SqlWorkflowInstanceStore(connectionString);
            instanceStore.InstanceCompletionAction = 
                InstanceCompletionAction.DeleteNothing;
            host.DurableInstancingOptions.InstanceStore = instanceStore;
            return host;
        }
    }
}
