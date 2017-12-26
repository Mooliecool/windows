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
using System.Workflow.Runtime;
using System.Threading;

namespace Microsoft.Samples.Workflow.Listen
{
    // Local service that implements the contract on the host side
    // i.e. it implements the methods and calls the events, which are
    // implemented by the workflow
    class OrderServiceImpl : IOrderService
    {
        string orderId;
        public WorkflowInstance instanceId;

        // Called by the workflow to pass an order id
        public void CreateOrder(string Id)
        {
            Console.WriteLine("\nPurchase Order Created in System");
            orderId = Id;
        }

        // Called by the host to approve an order
        public void ApproveOrder()
        {
            EventHandler<OrderEventArgs> orderApproved = this.OrderApproved;
            if (orderApproved != null)
                orderApproved(null, new OrderEventArgs(instanceId.InstanceId, orderId));
        }

        // Called by the host to reject an order
        public void RejectOrder()
        {
            EventHandler<OrderEventArgs> orderRejected = this.OrderRejected;
            if (orderRejected != null)
                orderRejected(null, new OrderEventArgs(instanceId.InstanceId, orderId));
        }

        // Events that handled within a workflow by HandleExternalEventActivity activities
        public event EventHandler<OrderEventArgs> OrderApproved;
        public event EventHandler<OrderEventArgs> OrderRejected;
    }
}
