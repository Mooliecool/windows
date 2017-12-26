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

namespace Microsoft.Samples.Workflow.OrderApplication
{
    [Serializable]
    public class OrderService : IOrderService
    {
        public void RaiseOrderCreatedEvent(string orderId,Guid instanceId)
        {
            if (OrderCreated != null)
            {
                OrderEventArgs e = new OrderEventArgs(instanceId, orderId);
                OrderCreated(this, e);
            }
        }

        public void RaiseOrderShippedEvent(string orderId,Guid instanceId)
        {
            if (OrderShipped != null)
            {
                OrderEventArgs e = new OrderEventArgs(instanceId, orderId);
                OrderShipped(this, e);
            }
        }

        public void RaiseOrderUpdatedEvent(string orderId,Guid instanceId)
        {
            if (OrderUpdated != null)
            {
                OrderEventArgs e = new OrderEventArgs(instanceId, orderId);
                OrderUpdated(this, e);
            }
        }
        public void RaiseOrderProcessedEvent(string orderId,Guid instanceId)
        {
            if (OrderProcessed != null)
            {
                OrderEventArgs e = new OrderEventArgs(instanceId, orderId);
                OrderProcessed(this, e);
            }
        }
        public void RaiseOrderCanceledEvent(string orderId,Guid instanceId)
        {
            if (OrderCanceled != null)
            {
                OrderEventArgs e = new OrderEventArgs(instanceId, orderId);
                OrderCanceled(this, e);
            }
        }
        public event EventHandler<OrderEventArgs> OrderCreated;
        public event EventHandler<OrderEventArgs> OrderShipped;
        public event EventHandler<OrderEventArgs> OrderUpdated;
        public event EventHandler<OrderEventArgs> OrderProcessed;
        public event EventHandler<OrderEventArgs> OrderCanceled;
    }

}
