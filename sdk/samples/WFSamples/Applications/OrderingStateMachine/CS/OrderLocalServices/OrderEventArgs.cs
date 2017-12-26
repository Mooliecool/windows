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
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.OrderApplication
{
    [Serializable]
    public class OrderEventArgs : ExternalDataEventArgs
    {
        private string orderIdValue;

        public OrderEventArgs(Guid instanceId, string orderId)
            : base(instanceId)
        {
            orderIdValue = orderId;
        }

        public string OrderId
        {
            get { return orderIdValue; }
            set { orderIdValue = value; }
        }
    }
}
