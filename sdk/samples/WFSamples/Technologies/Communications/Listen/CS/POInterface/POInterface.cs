//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.Workflow.ComponentModel;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.Listen
{
    // Class defines the message passed between the local service and the workflow
    // The Serializable attribute is a required attribute
    // indicating that a class can be serialized 
    [Serializable]
    public class OrderEventArgs : ExternalDataEventArgs
    {
        private string id;

        public OrderEventArgs(Guid instanceId, string id)
            : base(instanceId)
        {
            this.id = id;
        }

        // Gets or sets an order id value
        // Sets by the workflow to pass an order id
        public string OrderId
        {
            get
            {
                return this.id;
            }
            set
            {
                this.id = value;
            }
        }
    }

    // The ExternalDataExchange attribute is a required attribute
    // indicating that the local service participates in data exchange with a workflow
    [ExternalDataExchange]
    public interface IOrderService
    {
        void CreateOrder(string id);
        
        event EventHandler<OrderEventArgs> OrderApproved;
        event EventHandler<OrderEventArgs> OrderRejected;
    }
}