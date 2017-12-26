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
using System.ComponentModel;
using System.Runtime.Serialization;
using System.Workflow.Activities;
using System.Workflow.ComponentModel;


namespace Microsoft.Samples.Workflow.Compensation
{
    public sealed partial class PurchaseOrder : SequentialWorkflowActivity
    {
        public PurchaseOrder()
        {
            InitializeComponent();
        }

        void DiscontinuedProduct_ExecuteCode(object sender, EventArgs e)
        {
            Console.WriteLine("Product discontinued");
        }

        private void ReceiveOrderHandler(object sender, EventArgs e)
        {
            Console.WriteLine("Received order");
        }

        private void RefundHandler(object sender, EventArgs e)
        {
            Console.WriteLine("Refunding");
        }

        private void ProductCheckHandler(object sender, ConditionalEventArgs e)
        {
            e.Result = false;
        }

        private void ShipHandler(object sender, EventArgs e)
        {
            Console.WriteLine("Ship order");
        }

        private void WithdrawFundsHandler(object sender, EventArgs e)
        {
            Console.WriteLine("Withdraw funds");
        }

        public DiscontinuedProductException discontinuedProductException1 = new Microsoft.Samples.Workflow.Compensation.DiscontinuedProductException();

        
    }

    [SerializableAttribute()]
    public class DiscontinuedProductException : Exception
    {
        public DiscontinuedProductException()
            : base()
        {
        }

        public DiscontinuedProductException(string message)
            : base(message)
        {
        }

        public DiscontinuedProductException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

        protected DiscontinuedProductException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
    }
}
