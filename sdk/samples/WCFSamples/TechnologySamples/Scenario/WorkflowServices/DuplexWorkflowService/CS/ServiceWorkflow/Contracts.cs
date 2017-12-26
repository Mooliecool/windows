//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Runtime.Serialization;
using System.ServiceModel;

namespace Microsoft.WorkflowServices.Samples
{
    [ServiceContract(Namespace = "http://Microsoft.WorkflowServices.Samples")]
    public interface IForwardContract
    {
        [OperationContract()]
        void BeginWorkflow(EndpointAddress10 returnAddress);
        [OperationContract(IsOneWay = true)]
        void BeginWorkItem(string value);
        [OperationContract(IsOneWay = true)]
        void ContinueWorkItem(int value);
        [OperationContract(IsOneWay = true)]
        void CompleteWorkItem(string value);
        [OperationContract(IsOneWay = true)]
        void CompleteWorkflow();
    }

    [ServiceContract(Namespace = "http://Microsoft.WorkflowServices.Samples")]
    public interface IReverseContract
    {
        [OperationContract(IsOneWay = true)]
        void WorkItemComplete(WorkItem item);
    }

    [ServiceContract(Namespace = "http://Microsoft.WorkflowServices.Samples")]
    public interface IHostForwardContract
    {
        [OperationContract()]
        void BeginWork(string returnUri);
        [OperationContract(IsOneWay = true)]
        void SubmitWorkItem(string itemName);
        [OperationContract(IsOneWay = true)]
        void WorkComplete();
    }    
    
    [Serializable]
    [DataContract(Namespace = "http://Microsoft.WorkflowServices.Samples")]
    public class WorkItem
    {
        [DataMember]
        public string FirstPart;
        [DataMember]
        public List<int> PartsList;
        [DataMember]
        public string LastPart;
    }

}
